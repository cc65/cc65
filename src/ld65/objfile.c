/*****************************************************************************/
/*                                                                           */
/*                                 objfile.c                                 */
/*                                                                           */
/*                 Object file handling for the ld65 linker                  */
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

/* common */
#include "fname.h"
#include "xmalloc.h"

/* ld65 */
#include "asserts.h"
#include "dbgsyms.h"
#include "error.h"
#include "exports.h"
#include "fileinfo.h"
#include "fileio.h"
#include "lineinfo.h"
#include "objdata.h"
#include "objfile.h"
#include "scopes.h"
#include "segments.h"
#include "spool.h"



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



static unsigned GetModule (const char* Name)
/* Get a module name index from the file name */
{
    /* Make a module name from the file name */
    const char* Module = FindName (Name);
    if (*Module == 0) {
        Error ("Cannot make module name from `%s'", Name);
    }
    return GetStringId (Module);
}



static void ObjReadHeader (FILE* Obj, ObjHeader* H, const char* Name)
/* Read the header of the object file checking the signature */
{
    H->Version    = Read16 (Obj);
    if (H->Version != OBJ_VERSION) {
        Error ("Object file `%s' has wrong version, expected %08X, got %08X",
               Name, OBJ_VERSION, H->Version);
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



void ObjReadFiles (FILE* F, unsigned long Pos, ObjData* O)
/* Read the files list from a file at the given position */
{
    unsigned I;
    unsigned FileCount;

    /* Seek to the correct position */
    FileSetPos (F, Pos);

    /* Read the data */
    FileCount  = ReadVar (F);
    CollGrow (&O->Files, FileCount);
    for (I = 0; I < FileCount; ++I) {
        CollAppend (&O->Files, ReadFileInfo (F, O));
    }
}



void ObjReadSections (FILE* F, unsigned long Pos, ObjData* O)
/* Read the section data from a file at the given position */
{
    unsigned I;
    unsigned SectionCount;

    /* Seek to the correct position */
    FileSetPos (F, Pos);

    /* Read the data */
    SectionCount = ReadVar (F);
    CollGrow (&O->Sections, SectionCount);
    for (I = 0; I < SectionCount; ++I) {
        CollAppend (&O->Sections, ReadSection (F, O));
    }
}



void ObjReadImports (FILE* F, unsigned long Pos, ObjData* O)
/* Read the imports from a file at the given position */
{
    unsigned I;
    unsigned ImportCount;

    /* Seek to the correct position */
    FileSetPos (F, Pos);

    /* Read the data */
    ImportCount = ReadVar (F);
    CollGrow (&O->Imports, ImportCount);
    for (I = 0; I < ImportCount; ++I) {
        CollAppend (&O->Imports, ReadImport (F, O));
    }
}



void ObjReadExports (FILE* F, unsigned long Pos, ObjData* O)
/* Read the exports from a file at the given position */
{
    unsigned I;
    unsigned ExportCount;

    /* Seek to the correct position */
    FileSetPos (F, Pos);

    /* Read the data */
    ExportCount = ReadVar (F);
    CollGrow (&O->Exports, ExportCount);
    for (I = 0; I < ExportCount; ++I) {
        CollAppend (&O->Exports, ReadExport (F, O));
    }
}



void ObjReadDbgSyms (FILE* F, unsigned long Pos, ObjData* O)
/* Read the debug symbols from a file at the given position */
{
    unsigned I;
    unsigned DbgSymCount;

    /* Seek to the correct position */
    FileSetPos (F, Pos);

    /* Read the asm debug symbols */
    DbgSymCount = ReadVar (F);
    CollGrow (&O->DbgSyms, DbgSymCount);
    for (I = 0; I < DbgSymCount; ++I) {
        CollAppend (&O->DbgSyms, ReadDbgSym (F, O, I));
    }

    /* Read the hll debug symbols */
    DbgSymCount = ReadVar (F);
    CollGrow (&O->HLLDbgSyms, DbgSymCount);
    for (I = 0; I < DbgSymCount; ++I) {
        CollAppend (&O->HLLDbgSyms, ReadHLLDbgSym (F, O, I));
    }
}



void ObjReadLineInfos (FILE* F, unsigned long Pos, ObjData* O)
/* Read the line infos from a file at the given position */
{
    unsigned I;
    unsigned LineInfoCount;

    /* Seek to the correct position */
    FileSetPos (F, Pos);

    /* Read the data */
    LineInfoCount = ReadVar (F);
    CollGrow (&O->LineInfos, LineInfoCount);
    for (I = 0; I < LineInfoCount; ++I) {
        CollAppend (&O->LineInfos, ReadLineInfo (F, O));
    }
}



void ObjReadStrPool (FILE* F, unsigned long Pos, ObjData* O)
/* Read the string pool from a file at the given position */
{
    unsigned I;

    /* Seek to the correct position */
    FileSetPos (F, Pos);

    /* Read the data */
    O->StringCount = ReadVar (F);
    O->Strings     = xmalloc (O->StringCount * sizeof (O->Strings[0]));
    for (I = 0; I < O->StringCount; ++I) {
        O->Strings[I] = ReadStr (F);
    }
}



void ObjReadAssertions (FILE* F, unsigned long Pos, ObjData* O)
/* Read the assertions from a file at the given offset */
{
    unsigned I;
    unsigned AssertionCount;

    /* Seek to the correct position */
    FileSetPos (F, Pos);

    /* Read the data */
    AssertionCount = ReadVar (F);
    CollGrow (&O->Assertions, AssertionCount);
    for (I = 0; I < AssertionCount; ++I) {
        CollAppend (&O->Assertions, ReadAssertion (F, O));
    }
}



void ObjReadScopes (FILE* F, unsigned long Pos, ObjData* O)
/* Read the scope table from a file at the given offset */
{
    unsigned I;
    unsigned ScopeCount;

    /* Seek to the correct position */
    FileSetPos (F, Pos);

    /* Read the data */
    ScopeCount = ReadVar (F);
    CollGrow (&O->Scopes, ScopeCount);
    for (I = 0; I < ScopeCount; ++I) {
        CollAppend (&O->Scopes,  ReadScope (F, O, I));
    }
}



void ObjReadSpans (FILE* F, unsigned long Pos, ObjData* O)
/* Read the span table from a file at the given offset */
{
    unsigned I;
    unsigned SpanCount;

    /* Seek to the correct position */
    FileSetPos (F, Pos);

    /* Read the data */
    SpanCount = ReadVar (F);
    CollGrow (&O->Spans, SpanCount);
    for (I = 0; I < SpanCount; ++I) {
        CollAppend (&O->Spans,  ReadSpan (F, O, I));
    }
}



void ObjAdd (FILE* Obj, const char* Name)
/* Add an object file to the module list */
{
    /* Create a new structure for the object file data */
    ObjData* O = NewObjData ();

    /* The magic was already read and checked, so set it in the header */
    O->Header.Magic = OBJ_MAGIC;

    /* Read and check the header */
    ObjReadHeader (Obj, &O->Header, Name);

    /* Initialize the object module data structure */
    O->Name  = GetModule (Name);

    /* Read the string pool from the object file */
    ObjReadStrPool (Obj, O->Header.StrPoolOffs, O);

    /* Read the files list from the object file */
    ObjReadFiles (Obj, O->Header.FileOffs, O);

    /* Read the line infos from the object file */
    ObjReadLineInfos (Obj, O->Header.LineInfoOffs, O);

    /* Read the imports list from the object file */
    ObjReadImports (Obj, O->Header.ImportOffs, O);

    /* Read the object file exports and insert them into the exports list */
    ObjReadExports (Obj, O->Header.ExportOffs, O);

    /* Read the object debug symbols from the object file */
    ObjReadDbgSyms (Obj, O->Header.DbgSymOffs, O);

    /* Read the assertions from the object file */
    ObjReadAssertions (Obj, O->Header.AssertOffs, O);

    /* Read the segment list from the object file. This must be late, since
    ** the expressions stored in the code may reference segments or imported
    ** symbols.
    */
    ObjReadSections (Obj, O->Header.SegOffs, O);

    /* Read the scope table from the object file. Scopes reference segments, so
    ** we must read them after the sections.
    */
    ObjReadScopes (Obj, O->Header.ScopeOffs, O);

    /* Read the spans from the object file */
    ObjReadSpans (Obj, O->Header.SpanOffs, O);

    /* Mark this object file as needed */
    O->Flags |= OBJ_REF;

    /* Done, close the file (we read it only, so no error check) */
    fclose (Obj);

    /* Insert the imports and exports to the global lists */
    InsertObjGlobals (O);

    /* Insert the object into the list of all used object files */
    InsertObjData (O);

    /* All references to strings are now resolved, so we can delete the module
    ** string pool.
    */
    FreeObjStrings (O);
}
