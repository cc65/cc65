/*****************************************************************************/
/*                                                                           */
/*                                 objdata.c                                 */
/*                                                                           */
/*               Handling object file data for the ld65 linker               */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2011, Ullrich von Bassewitz                                      */
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
#include "check.h"
#include "xmalloc.h"

/* ld65 */
#include "error.h"
#include "exports.h"
#include "fileinfo.h"
#include "library.h"
#include "objdata.h"
#include "spool.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Collection containing used ObjData objects */
Collection       ObjDataList = STATIC_COLLECTION_INITIALIZER;



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



ObjData* NewObjData (void)
/* Allocate a new structure on the heap, insert it into the list, return it */
{
    /* Allocate memory */
    ObjData* O = xmalloc (sizeof (ObjData));

    /* Initialize the data */
    O->Next             = 0;
    O->Name             = INVALID_STRING_ID;
    O->Lib              = 0;
    O->MTime            = 0;
    O->Start            = 0;
    O->Flags            = 0;
    O->HLLSymBaseId     = 0;
    O->SymBaseId        = 0;
    O->ScopeBaseId      = 0;
    O->SpanBaseId       = 0;
    O->Files            = EmptyCollection;
    O->Sections         = EmptyCollection;
    O->Exports          = EmptyCollection;
    O->Imports          = EmptyCollection;
    O->DbgSyms          = EmptyCollection;
    O->HLLDbgSyms       = EmptyCollection;
    O->LineInfos        = EmptyCollection;
    O->StringCount      = 0;
    O->Strings          = 0;
    O->Assertions       = EmptyCollection;
    O->Scopes           = EmptyCollection;
    O->Spans            = EmptyCollection;

    /* Return the new entry */
    return O;
}



void FreeObjData (ObjData* O)
/* Free an ObjData object. NOTE: This function works only for unused object
** data, that is, ObjData objects that aren't used because they aren't
** referenced.
*/
{
    unsigned I;

    for (I = 0; I < CollCount (&O->Files); ++I) {
        CollDeleteItem (&((FileInfo*) CollAtUnchecked (&O->Files, I))->Modules, O);
    }
    DoneCollection (&O->Files);
    DoneCollection (&O->Sections);
    for (I = 0; I < CollCount (&O->Exports); ++I) {
        FreeExport (CollAtUnchecked (&O->Exports, I));
    }
    DoneCollection (&O->Exports);
    for (I = 0; I < CollCount (&O->Imports); ++I) {
        FreeImport (CollAtUnchecked (&O->Imports, I));
    }
    DoneCollection (&O->Imports);
    DoneCollection (&O->DbgSyms);
    DoneCollection (&O->HLLDbgSyms);

    for (I = 0; I < CollCount (&O->LineInfos); ++I) {
        FreeLineInfo (CollAtUnchecked (&O->LineInfos, I));
    }
    DoneCollection (&O->LineInfos);
    xfree (O->Strings);
    DoneCollection (&O->Assertions);
    DoneCollection (&O->Scopes);
    for (I = 0; I < CollCount (&O->Spans); ++I) {
        FreeSpan (CollAtUnchecked (&O->Spans, I));
    }
    DoneCollection (&O->Spans);

    xfree (O);
}



void FreeObjStrings (ObjData* O)
/* Free the module string data. Used once the object file is loaded completely
** when all strings are converted to global strings.
*/
{
    xfree (O->Strings);
    O->Strings = 0;
}



void InsertObjData (ObjData* O)
/* Insert the ObjData object into the collection of used ObjData objects. */
{
    CollAppend (&ObjDataList, O);
}



void InsertObjGlobals (ObjData* O)
/* Insert imports and exports from the object file into the global import and
** export lists.
*/
{
    unsigned I;

    /* Insert exports and imports */
    for (I = 0; I < CollCount (&O->Exports); ++I) {
        InsertExport (CollAt (&O->Exports, I));
    }
    for (I = 0; I < CollCount (&O->Imports); ++I) {
        InsertImport (CollAt (&O->Imports, I));
    }
}



unsigned MakeGlobalStringId (const ObjData* O, unsigned Index)
/* Convert a local string id into a global one and return it. */
{
    if (Index >= O->StringCount) {
        Error ("Invalid string index (%u) in module `%s'",
               Index, GetObjFileName (O));
    }
    return O->Strings[Index];
}



const char* GetObjFileName (const ObjData* O)
/* Get the name of the object file. Return "[linker generated]" if the object
** file is NULL.
*/
{
    return O? GetString (O->Name) : "[linker generated]";
}



const struct StrBuf* GetObjString (const ObjData* Obj, unsigned Id)
/* Get a string from an object file checking for an invalid index */
{
    return GetStrBuf (MakeGlobalStringId (Obj, Id));
}



struct Section* GetObjSection (const ObjData* O, unsigned Id)
/* Get a section from an object file checking for a valid index */
{
    if (Id >= CollCount (&O->Sections)) {
        Error ("Invalid section index (%u) in module `%s'",
               Id, GetObjFileName (O));
    }
    return CollAtUnchecked (&O->Sections, Id);
}



struct Import* GetObjImport (const ObjData* O, unsigned Id)
/* Get an import from an object file checking for a valid index */
{
    if (Id >= CollCount (&O->Imports)) {
        Error ("Invalid import index (%u) in module `%s'",
               Id, GetObjFileName (O));
    }
    return CollAtUnchecked (&O->Imports, Id);
}



struct Export* GetObjExport (const ObjData* O, unsigned Id)
/* Get an export from an object file checking for a valid index */
{
    if (Id >= CollCount (&O->Exports)) {
        Error ("Invalid export index (%u) in module `%s'",
               Id, GetObjFileName (O));
    }
    return CollAtUnchecked (&O->Exports, Id);
}



struct DbgSym* GetObjDbgSym (const ObjData* O, unsigned Id)
/* Get a debug symbol from an object file checking for a valid index */
{
    if (Id >= CollCount (&O->DbgSyms)) {
        Error ("Invalid debug symbol index (%u) in module `%s'",
               Id, GetObjFileName (O));
    }
    return CollAtUnchecked (&O->DbgSyms, Id);
}



struct Scope* GetObjScope (const ObjData* O, unsigned Id)
/* Get a scope from an object file checking for a valid index */
{
    if (Id >= CollCount (&O->Scopes)) {
        Error ("Invalid scope index (%u) in module `%s'",
               Id, GetObjFileName (O));
    }
    return CollAtUnchecked (&O->Scopes, Id);
}



unsigned ObjDataCount (void)
/* Return the total number of modules */
{
    return CollCount (&ObjDataList);
}



void PrintDbgModules (FILE* F)
/* Output the modules to a debug info file */
{
    unsigned I;

    /* Output modules */
    for (I = 0; I < CollCount (&ObjDataList); ++I) {

        /* Get this object file */
        const ObjData* O = CollConstAt (&ObjDataList, I);

        /* The main source file is the one at index zero */
        const FileInfo* Source = CollConstAt (&O->Files, 0);

        /* Output the module line */
        fprintf (F,
                 "mod\tid=%u,name=\"%s\",file=%u",
                 I,
                 GetObjFileName (O),
                 Source->Id);

        /* Add library if any */
        if (O->Lib != 0) {
            fprintf (F, ",lib=%u", GetLibId (O->Lib));
        }

        /* Terminate the output line */
        fputc ('\n', F);
    }

}
