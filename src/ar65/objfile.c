/*****************************************************************************/
/*                                                                           */
/*                                 objfile.c                                 */
/*                                                                           */
/*                Object file handling for the ar65 archiver                 */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2012, Ullrich von Bassewitz                                      */
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
#include <errno.h>

/* common */
#include "cddefs.h"
#include "exprdefs.h"
#include "filestat.h"
#include "filetime.h"
#include "fname.h"
#include "symdefs.h"
#include "xmalloc.h"

/* ar65 */
#include "error.h"
#include "objdata.h"
#include "fileio.h"
#include "library.h"
#include "objfile.h"



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



static const char* GetModule (const char* Name)
/* Get a module name from the file name */
{
    /* Make a module name from the file name */
    const char* Module = FindName (Name);

    /* Must not end with a path separator */
    if (*Module == 0) {
        Error ("Cannot make module name from `%s'", Name);
    }

    /* Done */
    return Module;
}



static void ObjReadHeader (FILE* Obj, ObjHeader* H, const char* Name)
/* Read the header of the object file checking the signature */
{
    H->Magic      = Read32 (Obj);
    if (H->Magic != OBJ_MAGIC) {
        Error ("`%s' is not an object file", Name);
    }
    H->Version    = Read16 (Obj);
    if (H->Version != OBJ_VERSION) {
        Error ("Object file `%s' has wrong version", Name);
    }
    H->Flags        = Read16 (Obj);
    H->OptionOffs   = Read32 (Obj);
    H->OptionSize   = Read32 (Obj);
    H->FileOffs     = Read32 (Obj);
    H->FileSize     = Read32 (Obj);
    H->SegOffs      = Read32 (Obj);
    H->SegSize      = Read32 (Obj);
    H->ImportOffs   = Read32 (Obj);
    H->ImportSize   = Read32 (Obj);
    H->ExportOffs   = Read32 (Obj);
    H->ExportSize   = Read32 (Obj);
    H->DbgSymOffs   = Read32 (Obj);
    H->DbgSymSize   = Read32 (Obj);
    H->LineInfoOffs = Read32 (Obj);
    H->LineInfoSize = Read32 (Obj);
    H->StrPoolOffs  = Read32 (Obj);
    H->StrPoolSize  = Read32 (Obj);
    H->AssertOffs   = Read32 (Obj);
    H->AssertSize   = Read32 (Obj);
    H->ScopeOffs    = Read32 (Obj);
    H->ScopeSize    = Read32 (Obj);
    H->SpanOffs     = Read32 (Obj);
    H->SpanSize     = Read32 (Obj);
}



static void SkipExpr (FILE* F)
/* Skip an expression in F */
{
    /* Get the operation and skip it */
    unsigned char Op = Read8 (F);

    /* Handle then different expression nodes */
    switch (Op) {

        case EXPR_NULL:
            break;

        case EXPR_LITERAL:
            /* 32 bit literal value */
            (void) Read32 (F);
            break;

        case EXPR_SYMBOL:
            /* Variable seized symbol index */
            (void) ReadVar (F);
            break;

        case EXPR_SECTION:
            /* 8 bit segment number */
            (void) Read8 (F);
            break;

        default:
            /* What's left are unary and binary nodes */
            SkipExpr (F);   /* Left */
            SkipExpr (F);   /* right */
            break;
    }
}



static void SkipLineInfoList (FILE* F)
/* Skip a list of line infos in F */
{
    /* Number of indices preceeds the list */
    unsigned long Count = ReadVar (F);

    /* Skip indices */
    while (Count--) {
        (void) ReadVar (F);
    }
}



void ObjReadData (FILE* F, ObjData* O)
/* Read object file data from the given file. The function expects the Name
** and Start fields to be valid. Header and basic data are read.
*/
{
    unsigned long Count;

    /* Seek to the start of the object file data */
    fseek (F, O->Start, SEEK_SET);

    /* Read the object file header */
    ObjReadHeader (F, &O->Header, O->Name);

    /* Read the string pool */
    fseek (F, O->Start + O->Header.StrPoolOffs, SEEK_SET);
    Count = ReadVar (F);
    CollGrow (&O->Strings, Count);
    while (Count--) {
        CollAppend (&O->Strings, ReadStr (F));
    }

    /* Read the exports */
    fseek (F, O->Start + O->Header.ExportOffs, SEEK_SET);
    Count = ReadVar (F);
    CollGrow (&O->Exports, Count);
    while (Count--) {

        unsigned char ConDes[CD_TYPE_COUNT];

        /* Skip data until we get to the name */
        unsigned Type = ReadVar (F);
        (void) Read8 (F);       /* AddrSize */
        ReadData (F, ConDes, SYM_GET_CONDES_COUNT (Type));

        /* Now this is what we actually need: The name of the export */
        CollAppend (&O->Exports, CollAt (&O->Strings, ReadVar (F)));

        /* Skip the export value */
        if (SYM_IS_EXPR (Type)) {
            /* Expression tree */
            SkipExpr (F);
        } else {
            /* Literal value */
            (void) Read32 (F);
        }

        /* Skip the size if necessary */
        if (SYM_HAS_SIZE (Type)) {
            (void) ReadVar (F);
        }

        /* Line info indices */
        SkipLineInfoList (F);
        SkipLineInfoList (F);
    }
}



void ObjAdd (const char* Name)
/* Add an object file to the library */
{
    struct stat StatBuf;
    const char* Module;
    ObjHeader H;
    ObjData* O;

    /* Open the object file */
    FILE* Obj = fopen (Name, "rb");
    if (Obj == 0) {
        Error ("Could not open `%s': %s", Name, strerror (errno));
    }

    /* Get the modification time of the object file. There's a race condition
    ** here, since we cannot use fileno() (non-standard identifier in standard
    ** header file), and therefore not fstat. When using stat with the
    ** file name, there's a risk that the file was deleted and recreated
    ** while it was open. Since mtime and size are only used to check
    ** if a file has changed in the debugger, we will ignore this problem
    ** here.
    */
    if (FileStat (Name, &StatBuf) != 0) {
        Error ("Cannot stat object file `%s': %s", Name, strerror (errno));
    }

    /* Read and check the header */
    ObjReadHeader (Obj, &H, Name);

    /* Make a module name from the file name */
    Module = GetModule (Name);

    /* Check if we already have a module with this name */
    O = FindObjData (Module);
    if (O == 0) {
        /* Not found, create a new entry */
        O = NewObjData ();
    } else {
        /* Found - check the file modification times of the internal copy
        ** and the external one.
        */
        if (difftime ((time_t)O->MTime, StatBuf.st_mtime) > 0.0) {
            Warning ("Replacing module `%s' by older version in library `%s'",
                     O->Name, LibName);
        }

        /* Free data */
        ClearObjData (O);
    }

    /* Initialize the object module data structure */
    O->Name     = xstrdup (Module);
    O->Flags    = OBJ_HAVEDATA;
    O->MTime    = (unsigned long) StatBuf.st_mtime;
    O->Start    = 0;

    /* Determine the file size. Note: Race condition here */
    fseek (Obj, 0, SEEK_END);
    O->Size     = ftell (Obj);

    /* Read the basic data from the object file */
    ObjReadData (Obj, O);

    /* Copy the complete object data to the library file and update the
    ** starting offset
    */
    fseek (Obj, 0, SEEK_SET);
    O->Start    = LibCopyTo (Obj, O->Size);

    /* Done, close the file (we read it only, so no error check) */
    fclose (Obj);
}



void ObjExtract (const char* Name)
/* Extract a module from the library */
{
    FILE* Obj;

    /* Make a module name from the file name */
    const char* Module = GetModule (Name);

    /* Try to find the module in the library */
    const ObjData* O = FindObjData (Module);

    /* Bail out if the module does not exist */
    if (O == 0) {
        Error ("Module `%s' not found in library `%s'", Module, LibName);
    }

    /* Open the output file */
    Obj = fopen (Name, "w+b");
    if (Obj == 0) {
        Error ("Cannot open target file `%s': %s", Name, strerror (errno));
    }

    /* Copy the complete object file data from the library to the new object
    ** file.
    */
    LibCopyFrom (O->Start, O->Size, Obj);

    /* Close the file */
    if (fclose (Obj) != 0) {
        Error ("Problem closing object file `%s': %s", Name, strerror (errno));
    }

    /* Set access and modification time */
    if (SetFileTimes (Name, O->MTime) != 0) {
        Error ("Cannot set mod time on `%s': %s", Name, strerror (errno));
    }
}
