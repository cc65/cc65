/*****************************************************************************/
/*                                                                           */
/*                                 objdata.h                                 */
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



#ifndef OBJDATA_H
#define OBJDATA_H



/* common */
#include "coll.h"
#include "inline.h"
#include "objdefs.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Forwards */
struct Export;
struct Import;
struct Library;
struct Scope;
struct Section;
struct StrBuf;

/* Values for the Flags field */
#define OBJ_REF         0x0001          /* We have a reference to this file */

/* Internal structure holding object file data */
typedef struct ObjData ObjData;
struct ObjData {
    ObjData*            Next;           /* Linked list of all objects */
    unsigned            Id;             /* Id of this module */
    unsigned            Name;           /* Module name */
    struct Library*     Lib;            /* Library where module comes from */
    unsigned long       MTime;          /* Time of last modification */
    ObjHeader           Header;         /* Header of file */
    unsigned long       Start;          /* Start offset of data in library */
    unsigned            Flags;

    unsigned            HLLSymBaseId;   /* Debug info base id for hll symbols */
    unsigned            SymBaseId;      /* Debug info base id for symbols */
    unsigned            ScopeBaseId;    /* Debug info base id for scopes */
    unsigned            SpanBaseId;     /* Debug info base id for spans */

    Collection          Files;          /* List of input files */
    Collection          Sections;       /* List of all sections */
    Collection          Exports;        /* List of all exports */
    Collection          Imports;        /* List of all imports */
    Collection          DbgSyms;        /* List of debug symbols */
    Collection          HLLDbgSyms;     /* List of hll debug symbols */
    Collection          LineInfos;      /* List of line infos */
    unsigned            StringCount;    /* Count of strings */
    unsigned*           Strings;        /* List of global string indices */
    Collection          Assertions;     /* List of module assertions */
    Collection          Scopes;         /* List of scopes */
    Collection          Spans;          /* List of spans */
};



/* Collection containing used ObjData objects */
extern Collection       ObjDataList;



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



ObjData* NewObjData (void);
/* Allocate a new structure on the heap, insert it into the list, return it */

void FreeObjData (ObjData* O);
/* Free an ObjData object. NOTE: This function works only for unused object
** data, that is, ObjData objects that aren't used because they aren't
** referenced.
*/

void FreeObjStrings (ObjData* O);
/* Free the module string data. Used once the object file is loaded completely
** when all strings are converted to global strings.
*/

void InsertObjData (ObjData* O);
/* Insert the ObjData object into the collection of used ObjData objects. */

void InsertObjGlobals (ObjData* O);
/* Insert imports and exports from the object file into the global import and
** export lists.
*/

unsigned MakeGlobalStringId (const ObjData* O, unsigned Index);
/* Convert a local string id into a global one and return it. */

const char* GetObjFileName (const ObjData* O);
/* Get the name of the object file. Return "[linker generated]" if the object
** file is NULL.
*/

#if defined(HAVE_INLINE)
INLINE int ObjHasFiles (const ObjData* O)
/* Return true if the files list does exist */
{
    return (O != 0 && CollCount (&O->Files) != 0);
}
#else
#  define ObjHasFiles(O)       ((O) != 0 && CollCount (&(O)->Files) != 0)
#endif

const struct StrBuf* GetObjString (const ObjData* Obj, unsigned Id);
/* Get a string from an object file checking for an invalid index */

struct Section* GetObjSection (const ObjData* Obj, unsigned Id);
/* Get a section from an object file checking for a valid index */

struct Import* GetObjImport (const ObjData* Obj, unsigned Id);
/* Get an import from an object file checking for a valid index */

struct Export* GetObjExport (const ObjData* Obj, unsigned Id);
/* Get an export from an object file checking for a valid index */

struct DbgSym* GetObjDbgSym (const ObjData* Obj, unsigned Id);
/* Get a debug symbol from an object file checking for a valid index */

struct Scope* GetObjScope (const ObjData* Obj, unsigned Id);
/* Get a scope from an object file checking for a valid index */

unsigned ObjDataCount (void);
/* Return the total number of modules */

void PrintDbgModules (FILE* F);
/* Output the modules to a debug info file */



/* End of objdata.h */

#endif
