/*****************************************************************************/
/*                                                                           */
/*                                  dump.c                                   */
/*                                                                           */
/*          Dump subroutines for the od65 object file dump utility           */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2002-2012, Ullrich von Bassewitz                                      */
/*                Roemerstrasse 52                                           */
/*                D-70794 Filderstadt                                        */
/* EMail:         uz@cc65.org                                                */
/*                                                                           */
/*                                                                           */
/* This software is provided 'as-is', without any expressed or implied       */
/* warranty.  In no event will the authors be held liable for any damages    */
/* arising from the use of this software.                                    */
/*                                                                           */
/* Permission is granted to anyone to use this software for any purpose,     */
/* including commercial applications, and to alter it and redistribute it    */
/* freely, subject to the following restrictions:                            */
/*                                                                           */
/* 1. The origin of this software must not be misrepresented; you must not   */
/*    claim that you wrote the original software. If you use this software   */
/*    in a product, an acknowledgment in the product documentation would be  */
/*    appreciated but is not required.                                       */
/* 2. Altered source versions must be plainly marked as such, and must not   */
/*    be misrepresented as being the original software.                      */
/* 3. This notice may not be removed or altered from any source              */
/*    distribution.                                                          */
/*                                                                           */
/*****************************************************************************/



#include <string.h>
#include <time.h>

/* common */
#include "addrsize.h"
#include "cddefs.h"
#include "coll.h"
#include "exprdefs.h"
#include "filepos.h"
#include "lidefs.h"
#include "objdefs.h"
#include "optdefs.h"
#include "scopedefs.h"
#include "symdefs.h"
#include "xmalloc.h"

/* od65 */
#include "error.h"
#include "fileio.h"
#include "dump.h"



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



static void DestroyStrPool (Collection* C)
/* Free all strings in the given pool plus the item pointers. Note: The
** collection may not be reused later.
*/
{
    unsigned I;
    for (I = 0; I < CollCount (C); ++I) {
        xfree (CollAtUnchecked (C, I));
    }
    DoneCollection (C);
}



static const char* GetString (const Collection* C, unsigned Index)
/* Get a string from a collection. In fact, this function calls CollConstAt,
** but will print a somewhat more readable error message if the index is out
** of bounds.
*/
{
    if (Index >= CollCount (C)) {
        Error ("Invalid string index (%u) - file corrupt!", Index);
    }
    return CollConstAt (C, Index);
}



static void DumpObjHeaderSection (const char* Name,
                                  unsigned long Offset,
                                  unsigned long Size)
/* Dump a header section */
{
    printf ("    %s:\n", Name);
    printf ("      Offset:%24lu\n", Offset);
    printf ("      Size:  %24lu\n", Size);
}



static char* TimeToStr (unsigned long Time)
/* Convert the time into a string and return it */
{
    /* Get the time and convert to string */
    time_t T = (time_t) Time;
    char*  S = asctime (localtime (&T));

    /* Remove the trailing newline */
    unsigned Len = strlen (S);
    if (Len > 0 && S[Len-1] == '\n') {
        S[Len-1 ] = '\0';
    }

    /* Return the time string */
    return S;
}



static void SkipLineInfoList (FILE* F)
/* Skip a line info list from the given file */
{
    /* Count preceeds the list */
    unsigned long Count = ReadVar (F);

    /* Skip indices */
    while (Count--) {
        (void) ReadVar (F);
    }
}



static void SkipSpanList (FILE* F)
/* Skip a span list from the given file */
{
    /* Count preceeds the list */
    unsigned long Count = ReadVar (F);

    /* Skip indices */
    while (Count--) {
        (void) ReadVar (F);
    }
}



static void SkipExpr (FILE* F)
/* Skip an expression from the given file */
{
    /* Read the node tag and handle NULL nodes */
    unsigned char Op = Read8 (F);
    if (Op == EXPR_NULL) {
        return;
    }

    /* Check the tag and handle the different expression types */
    if (EXPR_IS_LEAF (Op)) {
        switch (Op) {

            case EXPR_LITERAL:
                (void) Read32Signed (F);
                break;

            case EXPR_SYMBOL:
                /* Read the import number */
                (void) ReadVar (F);
                break;

            case EXPR_SECTION:
            case EXPR_BANK:
                /* Read the segment number */
                (void) ReadVar (F);
                break;

            default:
                Error ("Invalid expression op: %02X", Op);

        }

    } else {

        /* Not a leaf node */
        SkipExpr (F);
        SkipExpr (F);
    }
}



static const char* GetExportFlags (unsigned Flags, const unsigned char* ConDes)
/* Get the export flags as a (static) string */
{
    /* Static buffer */
    static char TypeDesc[256];
    static char* T;

    unsigned Count;
    unsigned I;

    /* Symbol type */
    TypeDesc[0] = '\0';
    switch (Flags & SYM_MASK_TYPE) {
        case SYM_STD:         strcat (TypeDesc, "SYM_STD");         break;
        case SYM_CHEAP_LOCAL: strcat (TypeDesc, "SYM_CHEAP_LOCAL"); break;
    }

    /* Symbol usage */
    switch (Flags & SYM_MASK_LABEL) {
        case SYM_EQUATE: strcat (TypeDesc, ",SYM_EQUATE"); break;
        case SYM_LABEL:  strcat (TypeDesc, ",SYM_LABEL");  break;
    }

    /* Type of expression */
    switch (Flags & SYM_MASK_VAL) {
        case SYM_CONST: strcat (TypeDesc, ",SYM_CONST"); break;
        case SYM_EXPR:  strcat (TypeDesc, ",SYM_EXPR");   break;
    }

    /* Size available? */
    if (SYM_HAS_SIZE (Flags)) {
        strcat (TypeDesc, ",SYM_SIZE");
    }


    /* Constructor/destructor declarations */
    T = TypeDesc + strlen (TypeDesc);
    Count = SYM_GET_CONDES_COUNT (Flags);
    if (Count > 0 && ConDes) {
        T += sprintf (T, ",SYM_CONDES=");
        for (I = 0; I < Count; ++I) {
            unsigned Type = CD_GET_TYPE (ConDes[I]);
            unsigned Prio = CD_GET_PRIO (ConDes[I]);
            if (I > 0) {
                *T++ = ',';
            }
            T += sprintf (T, "[%u,%u]", Type, Prio);
        }
    }

    /* Return the result */
    return TypeDesc;
}



static const char* GetScopeType (unsigned Type)
/* Return the name of a scope type */
{
    switch (Type) {
        case SCOPE_GLOBAL:      return "Global scope";
        case SCOPE_FILE:        return "File scope";
        case SCOPE_SCOPE:       return ".SCOPE or .PROC";
        case SCOPE_STRUCT:      return ".STRUCT";
        case SCOPE_ENUM:        return ".ENUM";
        case SCOPE_UNDEF:       return "Undefined";
        default:                return "Unknown scope type";
    }
}



void DumpObjHeader (FILE* F, unsigned long Offset)
/* Dump the header of the given object file */
{
    ObjHeader H;

    /* Seek to the header position */
    FileSetPos (F, Offset);

    /* Read the header */
    ReadObjHeader (F, &H);

    /* Now dump the information */

    /* Output a header */
    printf ("  Header:\n");

    /* Magic */
    printf ("    Magic:%17s0x%08lX\n", "", H.Magic);

    /* Version */
    printf ("    Version:%25u\n", H.Version);

    /* Flags */
    printf ("    Flags:%21s0x%04X  (", "", H.Flags);
    if (H.Flags & OBJ_FLAGS_DBGINFO) {
        printf ("OBJ_FLAGS_DBGINFO");
    }
    printf (")\n");

    /* Options */
    DumpObjHeaderSection ("Options", H.OptionOffs, H.OptionSize);

    /* Files */
    DumpObjHeaderSection ("Files", H.FileOffs, H.FileSize);

    /* Segments */
    DumpObjHeaderSection ("Segments", H.SegOffs, H.SegSize);

    /* Imports */
    DumpObjHeaderSection ("Imports", H.ImportOffs, H.ImportSize);

    /* Exports */
    DumpObjHeaderSection ("Exports", H.ExportOffs, H.ExportSize);

    /* Debug symbols */
    DumpObjHeaderSection ("Debug symbols", H.DbgSymOffs, H.DbgSymSize);

    /* Line infos */
    DumpObjHeaderSection ("Line infos", H.LineInfoOffs, H.LineInfoSize);

    /* String pool */
    DumpObjHeaderSection ("String pool", H.StrPoolOffs, H.StrPoolSize);

    /* Assertions */
    DumpObjHeaderSection ("Assertions", H.AssertOffs, H.AssertSize);

    /* Scopes */
    DumpObjHeaderSection ("Scopes", H.ScopeOffs, H.ScopeSize);
}



void DumpObjOptions (FILE* F, unsigned long Offset)
/* Dump the file options */
{
    ObjHeader  H;
    Collection StrPool = AUTO_COLLECTION_INITIALIZER;
    unsigned   Count;
    unsigned   I;

    /* Seek to the header position and read the header */
    FileSetPos (F, Offset);
    ReadObjHeader (F, &H);

    /* Seek to the start of the string pool and read it */
    FileSetPos (F, Offset + H.StrPoolOffs);
    ReadStrPool (F, &StrPool);

    /* Seek to the start of the options */
    FileSetPos (F, Offset + H.OptionOffs);

    /* Output a header */
    printf ("  Options:\n");

    /* Read the number of options and print it */
    Count = ReadVar (F);
    printf ("    Count:%27u\n", Count);

    /* Read and print all options */
    for (I = 0; I < Count; ++I) {

        const char*   ArgStr;
        unsigned      ArgLen;

        /* Read the type of the option and the value */
        unsigned char Type = Read8 (F);
        unsigned long Val  = ReadVar (F);

        /* Get the type of the argument */
        unsigned char ArgType = Type & OPT_ARGMASK;

        /* Determine which option follows */
        const char* TypeDesc;
        switch (Type) {
            case OPT_COMMENT:   TypeDesc = "OPT_COMMENT";       break;
            case OPT_AUTHOR:    TypeDesc = "OPT_AUTHOR";        break;
            case OPT_TRANSLATOR:TypeDesc = "OPT_TRANSLATOR";    break;
            case OPT_COMPILER:  TypeDesc = "OPT_COMPILER";      break;
            case OPT_OS:        TypeDesc = "OPT_OS";            break;
            case OPT_DATETIME:  TypeDesc = "OPT_DATETIME";      break;
            default:            TypeDesc = "OPT_UNKNOWN";       break;
        }

        /* Print the header */
        printf ("    Index:%27u\n", I);

        /* Print the data */
        printf ("      Type:%22s0x%02X  (%s)\n", "", Type, TypeDesc);
        switch (ArgType) {

            case OPT_ARGSTR:
                ArgStr = GetString (&StrPool, Val);
                ArgLen = strlen (ArgStr);
                printf ("      Data:%*s\"%s\"\n", (int)(24-ArgLen), "", ArgStr);
                break;

            case OPT_ARGNUM:
                printf ("      Data:%26lu", Val);
                if (Type == OPT_DATETIME) {
                    /* Print the time as a string */
                    printf ("  (%s)", TimeToStr (Val));
                }
                printf ("\n");
                break;

            default:
                /* Unknown argument type. This means that we cannot determine
                ** the option length, so we cannot proceed.
                */
                Error ("Unknown option type: 0x%02X", Type);
                break;
        }
    }

    /* Destroy the string pool */
    DestroyStrPool (&StrPool);
}



void DumpObjFiles (FILE* F, unsigned long Offset)
/* Dump the source files */
{
    ObjHeader  H;
    Collection StrPool = AUTO_COLLECTION_INITIALIZER;
    unsigned   Count;
    unsigned   I;

    /* Seek to the header position and read the header */
    FileSetPos (F, Offset);
    ReadObjHeader (F, &H);

    /* Seek to the start of the string pool and read it */
    FileSetPos (F, Offset + H.StrPoolOffs);
    ReadStrPool (F, &StrPool);

    /* Seek to the start of the source files */
    FileSetPos (F, Offset + H.FileOffs);

    /* Output a header */
    printf ("  Files:\n");

    /* Read the number of files and print it */
    Count = ReadVar (F);
    printf ("    Count:%27u\n", Count);

    /* Read and print all files */
    for (I = 0; I < Count; ++I) {

        /* Read the data for one file */
        const char*   Name  = GetString (&StrPool, ReadVar (F));
        unsigned long MTime = Read32 (F);
        unsigned long Size  = ReadVar (F);
        unsigned      Len   = strlen (Name);

        /* Print the header */
        printf ("    Index:%27u\n", I);

        /* Print the data */
        printf ("      Name:%*s\"%s\"\n", (int)(24-Len), "", Name);
        printf ("      Size:%26lu\n", Size);
        printf ("      Modification time:%13lu  (%s)\n", MTime, TimeToStr (MTime));
    }

    /* Destroy the string pool */
    DestroyStrPool (&StrPool);
}



void DumpObjSegments (FILE* F, unsigned long Offset)
/* Dump the segments in the object file */
{
    ObjHeader  H;
    Collection StrPool = AUTO_COLLECTION_INITIALIZER;
    unsigned   Count;
    unsigned   I;

    /* Seek to the header position and read the header */
    FileSetPos (F, Offset);
    ReadObjHeader (F, &H);

    /* Seek to the start of the string pool and read it */
    FileSetPos (F, Offset + H.StrPoolOffs);
    ReadStrPool (F, &StrPool);

    /* Seek to the start of the segments */
    FileSetPos (F, Offset + H.SegOffs);

    /* Output a header */
    printf ("  Segments:\n");

    /* Read the number of segments and print it */
    Count = ReadVar (F);
    printf ("    Count:%27u\n", Count);

    /* Read and print all segments */
    for (I = 0; I < Count; ++I) {

        /* Read the data for one segments */
        unsigned long DataSize  = Read32 (F);
        unsigned long NextSeg   = ftell (F) + DataSize;
        const char*   Name      = GetString (&StrPool, ReadVar (F));
        unsigned      Len       = strlen (Name);
        unsigned      Flags     = ReadVar (F);
        unsigned long Size      = ReadVar (F);
        unsigned long Align     = ReadVar (F);
        unsigned char AddrSize  = Read8 (F);
        unsigned long FragCount = ReadVar (F);

        /* Print the header */
        printf ("    Index:%27u\n", I);

        /* Print the data */
        printf ("      Name:%*s\"%s\"\n", (int)(24-Len), "", Name);
        printf ("      Flags:%25u\n", Flags);
        printf ("      Size:%26lu\n", Size);
        printf ("      Alignment:%21lu\n", Align);
        printf ("      Address size:%14s0x%02X  (%s)\n", "", AddrSize,
                AddrSizeToStr (AddrSize));
        printf ("      Fragment count:%16lu\n", FragCount);

        /* Seek to the end of the segment data (start of next) */
        FileSetPos (F, NextSeg);
    }

    /* Destroy the string pool */
    DestroyStrPool (&StrPool);
}



void DumpObjImports (FILE* F, unsigned long Offset)
/* Dump the imports in the object file */
{
    ObjHeader  H;
    Collection StrPool = AUTO_COLLECTION_INITIALIZER;
    unsigned   Count;
    unsigned   I;

    /* Seek to the header position and read the header */
    FileSetPos (F, Offset);
    ReadObjHeader (F, &H);

    /* Seek to the start of the string pool and read it */
    FileSetPos (F, Offset + H.StrPoolOffs);
    ReadStrPool (F, &StrPool);

    /* Seek to the start of the imports */
    FileSetPos (F, Offset + H.ImportOffs);

    /* Output a header */
    printf ("  Imports:\n");

    /* Read the number of imports and print it */
    Count = ReadVar (F);
    printf ("    Count:%27u\n", Count);

    /* Read and print all imports */
    for (I = 0; I < Count; ++I) {

        /* Read the data for one import */
        unsigned char AddrSize = Read8 (F);
        const char*   Name     = GetString (&StrPool, ReadVar (F));
        unsigned      Len      = strlen (Name);

        /* Skip both line info lists */
        SkipLineInfoList (F);
        SkipLineInfoList (F);

        /* Print the header */
        printf ("    Index:%27u\n", I);

        /* Print the data */
        printf ("      Address size:%14s0x%02X  (%s)\n", "", AddrSize,
                AddrSizeToStr (AddrSize));
        printf ("      Name:%*s\"%s\"\n", (int)(24-Len), "", Name);
    }

    /* Destroy the string pool */
    DestroyStrPool (&StrPool);
}



void DumpObjExports (FILE* F, unsigned long Offset)
/* Dump the exports in the object file */
{
    ObjHeader   H;
    Collection  StrPool = AUTO_COLLECTION_INITIALIZER;
    unsigned    Count;
    unsigned    I;

    /* Seek to the header position and read the header */
    FileSetPos (F, Offset);
    ReadObjHeader (F, &H);

    /* Seek to the start of the string pool and read it */
    FileSetPos (F, Offset + H.StrPoolOffs);
    ReadStrPool (F, &StrPool);

    /* Seek to the start of the exports */
    FileSetPos (F, Offset + H.ExportOffs);

    /* Output a header */
    printf ("  Exports:\n");

    /* Read the number of exports and print it */
    Count = ReadVar (F);
    printf ("    Count:%27u\n", Count);

    /* Read and print all exports */
    for (I = 0; I < Count; ++I) {

        unsigned long   Value = 0;
        unsigned long   Size = 0;
        unsigned char   ConDes[CD_TYPE_COUNT];
        const char*     Name;
        unsigned        Len;


        /* Read the data for one export */
        unsigned Type          = ReadVar (F);
        unsigned char AddrSize = Read8 (F);
        ReadData (F, ConDes, SYM_GET_CONDES_COUNT (Type));
        Name  = GetString (&StrPool, ReadVar (F));
        Len   = strlen (Name);
        if (SYM_IS_CONST (Type)) {
            Value = Read32 (F);
        } else {
            SkipExpr (F);
        }
        if (SYM_HAS_SIZE (Type)) {
            Size = ReadVar (F);
        }

        /* Skip both line infos lists */
        SkipLineInfoList (F);
        SkipLineInfoList (F);

        /* Print the header */
        printf ("    Index:%27u\n", I);

        /* Print the data */
        printf ("      Type:%22s0x%02X  (%s)\n", "", Type, GetExportFlags (Type, ConDes));
        printf ("      Address size:%14s0x%02X  (%s)\n", "", AddrSize,
                AddrSizeToStr (AddrSize));
        printf ("      Name:%*s\"%s\"\n", (int)(24-Len), "", Name);
        if (SYM_IS_CONST (Type)) {
            printf ("      Value:%15s0x%08lX  (%lu)\n", "", Value, Value);
        }
        if (SYM_HAS_SIZE (Type)) {
            printf ("      Size:%16s0x%04lX  (%lu)\n", "", Size, Size);
        }
    }

    /* Destroy the string pool */
    DestroyStrPool (&StrPool);
}



void DumpObjDbgSyms (FILE* F, unsigned long Offset)
/* Dump the debug symbols from an object file */
{
    ObjHeader   H;
    Collection  StrPool = AUTO_COLLECTION_INITIALIZER;
    unsigned    Count;
    unsigned    I;

    /* Seek to the header position and read the header */
    FileSetPos (F, Offset);
    ReadObjHeader (F, &H);

    /* Seek to the start of the string pool and read it */
    FileSetPos (F, Offset + H.StrPoolOffs);
    ReadStrPool (F, &StrPool);

    /* Seek to the start of the debug syms */
    FileSetPos (F, Offset + H.DbgSymOffs);

    /* Output a header */
    printf ("  Debug symbols:\n");

    /* Check if the object file was compiled with debug info */
    if ((H.Flags & OBJ_FLAGS_DBGINFO) == 0) {
        /* Print that there no debug symbols and bail out */
        printf ("    Count:%27u\n", 0);
        return;
    }

    /* Read the number of exports and print it */
    Count = ReadVar (F);
    printf ("    Count:%27u\n", Count);

    /* Read and print all debug symbols */
    for (I = 0; I < Count; ++I) {

        unsigned long   Value = 0;
        unsigned long   Size = 0;
        unsigned        ImportId = 0;
        unsigned        ExportId = 0;

        /* Read the data for one symbol */
        unsigned Type          = ReadVar (F);
        unsigned char AddrSize = Read8 (F);
        unsigned long Owner    = ReadVar (F);
        const char*   Name     = GetString (&StrPool, ReadVar (F));
        unsigned      Len      = strlen (Name);
        if (SYM_IS_CONST (Type)) {
            Value = Read32 (F);
        } else {
            SkipExpr (F);
        }
        if (SYM_HAS_SIZE (Type)) {
            Size = ReadVar (F);
        }
        if (SYM_IS_IMPORT (Type)) {
            ImportId = ReadVar (F);
        }
        if (SYM_IS_EXPORT (Type)) {
            ExportId = ReadVar (F);
        }

        /* Skip both line info lists */
        SkipLineInfoList (F);
        SkipLineInfoList (F);

        /* Print the header */
        printf ("    Index:%27u\n", I);

        /* Print the data */
        printf ("      Type:%22s0x%02X  (%s)\n", "", Type, GetExportFlags (Type, 0));
        printf ("      Address size:%14s0x%02X  (%s)\n", "", AddrSize,
                AddrSizeToStr (AddrSize));
        printf ("      Owner:%25lu\n", Owner);
        printf ("      Name:%*s\"%s\"\n", (int)(24-Len), "", Name);
        if (SYM_IS_CONST (Type)) {
            printf ("      Value:%15s0x%08lX  (%lu)\n", "", Value, Value);
        }
        if (SYM_HAS_SIZE (Type)) {
            printf ("      Size:%20s0x%04lX  (%lu)\n", "", Size, Size);
        }
        if (SYM_IS_IMPORT (Type)) {
            printf ("      Import:%24u\n", ImportId);
        }
        if (SYM_IS_EXPORT (Type)) {
            printf ("      Export:%24u\n", ExportId);
        }
    }

    /* Destroy the string pool */
    DestroyStrPool (&StrPool);
}



void DumpObjLineInfo (FILE* F, unsigned long Offset)
/* Dump the line info from an object file */
{
    ObjHeader   H;
    Collection  StrPool = AUTO_COLLECTION_INITIALIZER;
    unsigned    Count;
    unsigned    I;

    /* Seek to the header position and read the header */
    FileSetPos (F, Offset);
    ReadObjHeader (F, &H);

    /* Seek to the start of the string pool and read it */
    FileSetPos (F, Offset + H.StrPoolOffs);
    ReadStrPool (F, &StrPool);

    /* Seek to the start of line infos */
    FileSetPos (F, Offset + H.LineInfoOffs);

    /* Output a header */
    printf ("  Line info:\n");

    /* Check if the object file was compiled with debug info */
    if ((H.Flags & OBJ_FLAGS_DBGINFO) == 0) {
        /* Print that there no line infos and bail out */
        printf ("    Count:%27u\n", 0);
        return;
    }

    /* Read the number of line infos and print it */
    Count = ReadVar (F);
    printf ("    Count:%27u\n", Count);

    /* Read and print all line infos */
    for (I = 0; I < Count; ++I) {

        FilePos   Pos;
        unsigned  Type;

        /* File position of line info */
        ReadFilePos (F, &Pos);

        /* Type of line info */
        Type = ReadVar (F);

        /* Skip the spans */
        SkipSpanList (F);

        /* Print the header */
        printf ("    Index:%27u\n", I);

        /* Print the data */
        printf ("      Type:%26u\n", LI_GET_TYPE (Type));
        printf ("      Count:%25u\n", LI_GET_COUNT (Type));
        printf ("      Line:%26u\n", Pos.Line);
        printf ("      Col:%27u\n", Pos.Col);
        printf ("      Name:%26u\n", Pos.Name);
    }

    /* Destroy the string pool */
    DestroyStrPool (&StrPool);
}



void DumpObjScopes (FILE* F, unsigned long Offset)
/* Dump the scopes from an object file */
{
    ObjHeader   H;
    Collection  StrPool = AUTO_COLLECTION_INITIALIZER;
    unsigned    Count;
    unsigned    I;

    /* Seek to the header position and read the header */
    FileSetPos (F, Offset);
    ReadObjHeader (F, &H);

    /* Seek to the start of the string pool and read it */
    FileSetPos (F, Offset + H.StrPoolOffs);
    ReadStrPool (F, &StrPool);

    /* Seek to the start of scopes */
    FileSetPos (F, Offset + H.ScopeOffs);

    /* Output a header */
    printf ("  Scopes:\n");

    /* Check if the object file was compiled with debug info */
    if ((H.Flags & OBJ_FLAGS_DBGINFO) == 0) {
        /* Print that there no scopes and bail out */
        printf ("    Count:%27u\n", 0);
        return;
    }

    /* Read the number of scopes and print it */
    Count = ReadVar (F);
    printf ("    Count:%27u\n", Count);

    /* Read and print all scopes */
    for (I = 0; I < Count; ++I) {

        const char*     Name;
        unsigned        Len;

        /* Read the data */
        unsigned        ParentId = ReadVar (F);
        unsigned        LexicalLevel = ReadVar (F);
        unsigned        Flags = ReadVar (F);
        const char*     ScopeType = GetScopeType (ReadVar (F));

        /* Print the header */
        printf ("    Index:%27u\n", I);

        /* Print the data */
        printf ("      Parent id:%21u\n",       ParentId);
        printf ("      Lexical level:%17u\n",   LexicalLevel);
        printf ("      Flags:%21s0x%02X\n",     "", Flags);
        printf ("      Type:%26s\n",            ScopeType);

        /* Resolve and print the name */
        Name = GetString (&StrPool, ReadVar (F));
        Len  = strlen (Name);
        printf ("      Name:%*s\"%s\"\n", (int)(24-Len), "", Name);

        /* Size */
        if (SCOPE_HAS_SIZE (Flags)) {
            unsigned long Size = ReadVar (F);
            printf ("      Size:%20s0x%04lX  (%lu)\n", "", Size, Size);
        }

        /* Label */
        if (SCOPE_HAS_LABEL (Flags)) {
            unsigned LabelId = ReadVar (F);
            printf ("      Label id:%22u\n", LabelId);
        }

        /* Skip the spans */
        SkipSpanList (F);
    }

    /* Destroy the string pool */
    DestroyStrPool (&StrPool);
}



void DumpObjSegSize (FILE* F, unsigned long Offset)
/* Dump the sizes of the segment in the object file */
{
    ObjHeader   H;
    Collection  StrPool = AUTO_COLLECTION_INITIALIZER;
    unsigned    Count;

    /* Seek to the header position and read the header */
    FileSetPos (F, Offset);
    ReadObjHeader (F, &H);

    /* Seek to the start of the string pool and read it */
    FileSetPos (F, Offset + H.StrPoolOffs);
    ReadStrPool (F, &StrPool);

    /* Seek to the start of the segments */
    FileSetPos (F, Offset + H.SegOffs);

    /* Output a header */
    printf ("  Segment sizes:\n");

    /* Read the number of segments */
    Count = ReadVar (F);

    /* Read and print the sizes of all segments */
    while (Count--) {

        unsigned long Size;

        /* Read the data for one segment */
        unsigned long DataSize = Read32 (F);
        unsigned long NextSeg  = ftell (F) + DataSize;
        const char*   Name     = GetString (&StrPool, ReadVar (F));
        unsigned      Len      = strlen (Name);

        /* Skip segment flags, read size */
        (void) ReadVar (F);     
        Size = ReadVar (F);

        /* Skip alignment, type and fragment count */
        (void) ReadVar (F);
        (void) Read8 (F);
        (void) ReadVar (F);

        /* Print the size for this segment */
        printf ("    %s:%*s%6lu\n", Name, (int)(24-Len), "", Size);

        /* Seek to the end of the segment data (start of next) */
        FileSetPos (F, NextSeg);
    }

    /* Destroy the string pool */
    DestroyStrPool (&StrPool);
}
