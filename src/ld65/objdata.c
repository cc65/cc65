/*****************************************************************************/
/*                                                                           */
/*				   objdata.c				     */
/*                                                                           */
/*		 Handling object file data for the ld65 linker		     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2003 Ullrich von Bassewitz                                       */
/*               Römerstraße 52                                              */
/*               D-70794 Filderstadt                                         */
/* EMail:        uz@cc65.org                                                 */
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
#include "objdata.h"
#include "spool.h"



/*****************************************************************************/
/*     	      	    		     Data				     */
/*****************************************************************************/



/* Collection containing used ObjData objects */
Collection       ObjDataList = STATIC_COLLECTION_INITIALIZER;



/*****************************************************************************/
/*     	      	    		     Code				     */
/*****************************************************************************/



ObjData* NewObjData (void)
/* Allocate a new structure on the heap, insert it into the list, return it */
{
    /* Allocate memory */
    ObjData* O = xmalloc (sizeof (ObjData));

    /* Initialize the data */
    O->Next        	= 0;
    O->Name  	   	= INVALID_STRING_ID;
    O->LibName    	= INVALID_STRING_ID;
    O->MTime            = 0;
    O->Flags   	   	= 0;
    O->Start	   	= 0;
    O->ExportCount 	= 0;
    O->Exports     	= 0;
    O->ImportCount 	= 0;
    O->Imports     	= 0;
    O->DbgSymCount	= 0;
    O->DbgSyms		= 0;
    O->LineInfoCount    = 0;
    O->LineInfos        = 0;
    O->StringCount      = 0;
    O->Strings          = 0;
    O->AssertionCount   = 0;
    O->Assertions       = 0;

    /* Return the new entry */
    return O;
}



void FreeObjData (ObjData* O)
/* Free an ObjData object. NOTE: This function works only for unused object
 * data, that is, ObjData objects that aren't used because they aren't
 * referenced.
 */
{
    /* Unused ObjData do only have the string pool, Exports and Imports. */
    while (O->ExportCount) {
        FreeExport (O->Exports[--O->ExportCount]);
    }
    xfree (O->Exports);
    while (O->ImportCount) {
        FreeImport (O->Imports[--O->ImportCount]);
    }
    xfree (O->Imports);
    xfree (O->Strings);
    xfree (O);
}



void FreeObjStrings (ObjData* O)
/* Free the module string data. Used once the object file is loaded completely
 * when all strings are converted to global strings.
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
 * export lists.
 */
{
    unsigned I;

    /* Insert exports and imports */
    for (I = 0; I < O->ExportCount; ++I) {
        InsertExport (O->Exports[I]);
    }
    for (I = 0; I < O->ImportCount; ++I) {
        InsertImport (O->Imports[I]);
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
 * file is NULL.
 */
{
    return O? GetString (O->Name) : "[linker generated]";
}



const char* GetSourceFileName (const ObjData* O, unsigned Index)
/* Get the name of the source file with the given index. If O is NULL, return
 * "[linker generated]" as the file name.
 */
{
    /* Check if we have an object file */
    if (O == 0) {

     	/* No object file */
     	return "[linker generated]";

    } else {

	/* Check the parameter */
        if (Index >= O->FileCount) {
            Error ("Invalid file index (%u) in module `%s' (input file corrupt?)",
                   Index, GetObjFileName (O));
        }

	/* Return the name */
	return GetString (O->Files[Index]->Name);

    }
}




