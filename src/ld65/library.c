/*****************************************************************************/
/*									     */
/*				   library.c				     */
/*									     */
/*	    Library data structures and helpers for the ld65 linker	     */
/*									     */
/*									     */
/*									     */
/* (C) 1998-2003 Ullrich von Bassewitz                                       */
/*               Römerstrasse 52                                             */
/*               D-70794 Filderstadt                                         */
/* EMail:        uz@cc65.org                                                 */
/*									     */
/*									     */
/* This software is provided 'as-is', without any expressed or implied	     */
/* warranty.  In no event will the authors be held liable for any damages    */
/* arising from the use of this software.				     */
/*									     */
/* Permission is granted to anyone to use this software for any purpose,     */
/* including commercial applications, and to alter it and redistribute it    */
/* freely, subject to the following restrictions:			     */
/*									     */
/* 1. The origin of this software must not be misrepresented; you must not   */
/*    claim that you wrote the original software. If you use this software   */
/*    in a product, an acknowledgment in the product documentation would be  */
/*    appreciated but is not required.					     */
/* 2. Altered source versions must be plainly marked as such, and must not   */
/*    be misrepresented as being the original software.			     */
/* 3. This notice may not be removed or altered from any source		     */
/*    distribution.							     */
/*									     */
/*****************************************************************************/



#include <stdio.h>
#include <string.h>
#include <errno.h>

/* common */
#include "exprdefs.h"
#include "filepos.h"
#include "libdefs.h"
#include "objdefs.h"
#include "symdefs.h"
#include "xmalloc.h"

/* ld65 */
#include "error.h"
#include "exports.h"
#include "fileio.h"
#include "library.h"
#include "objdata.h"
#include "objfile.h"
#include "spool.h"



/*****************************************************************************/
/*	  			     Data				     */
/*****************************************************************************/



/* Library data */
static FILE*		Lib		= 0;
static unsigned		ModuleCount	= 0;
static ObjData**	Index		= 0;



/*****************************************************************************/
/*	       		 Reading file data structures			     */
/*****************************************************************************/



static void LibReadObjHeader (ObjData* O, const char* LibName)
/* Read the header of the object file checking the signature */
{
    O->Header.Magic = Read32 (Lib);
    if (O->Header.Magic != OBJ_MAGIC) {
	Error ("Object file `%s' in library `%s' is invalid",
	       GetObjFileName (O), LibName);
    }
    O->Header.Version = Read16 (Lib);
    if (O->Header.Version != OBJ_VERSION) {
	Error ("Object file `%s' in library `%s' has wrong version",
	       GetObjFileName (O), LibName);
    }
    O->Header.Flags    	   = Read16 (Lib);
    O->Header.OptionOffs   = Read32 (Lib);
    O->Header.OptionSize   = Read32 (Lib);
    O->Header.FileOffs	   = Read32 (Lib);
    O->Header.FileSize	   = Read32 (Lib);
    O->Header.SegOffs	   = Read32 (Lib);
    O->Header.SegSize	   = Read32 (Lib);
    O->Header.ImportOffs   = Read32 (Lib);
    O->Header.ImportSize   = Read32 (Lib);
    O->Header.ExportOffs   = Read32 (Lib);
    O->Header.ExportSize   = Read32 (Lib);
    O->Header.DbgSymOffs   = Read32 (Lib);
    O->Header.DbgSymSize   = Read32 (Lib);
    O->Header.LineInfoOffs = Read32 (Lib);
    O->Header.LineInfoSize = Read32 (Lib);
    O->Header.StrPoolOffs  = Read32 (Lib);
    O->Header.StrPoolSize  = Read32 (Lib);
    O->Header.AssertOffs   = Read32 (Lib);
    O->Header.AssertSize   = Read32 (Lib);
}



static ObjData* ReadIndexEntry (void)
/* Read one entry in the index */
{
    /* Create a new entry and insert it into the list */
    ObjData* O	= NewObjData ();

    /* Module name */
    O->Name = ReadStr (Lib);

    /* Module flags/MTime/Start/Size */
    O->Flags	= Read16 (Lib);
    O->MTime    = Read32 (Lib);
    O->Start	= Read32 (Lib);
    Read32 (Lib);			/* Skip Size */

    /* Read the string pool */
    ObjReadStrPool (Lib, FileGetPos (Lib), O);

    /* Skip the export size, then read the exports */
    (void) ReadVar (Lib);
    ObjReadExports (Lib, FileGetPos (Lib), O);

    /* Skip the import size, then read the imports */
    (void) ReadVar (Lib);
    ObjReadImports (Lib, FileGetPos (Lib), O);

    /* Done */
    return O;
}



static void ReadIndex (void)
/* Read the index of a library file */
{
    unsigned I;

    /* Read the object file count and allocate memory */
    ModuleCount = ReadVar (Lib);
    Index = xmalloc (ModuleCount * sizeof (Index[0]));

    /* Read all entries in the index */
    for (I = 0; I < ModuleCount; ++I) {
       	Index[I] = ReadIndexEntry ();
    }
}



/*****************************************************************************/
/*	   		       High level stuff				     */
/*****************************************************************************/



static void LibCheckExports (ObjData* O)
/* Check if the exports from this file can satisfy any import requests. If so,
 * insert the imports and exports from this file and mark the file as added.
 */
{
    unsigned I;

    /* Check all exports */
    for (I = 0; I < O->ExportCount; ++I) {
	if (IsUnresolved (O->Exports[I]->Name)) {
	    /* We need this module */
	    O->Flags |= OBJ_REF;
	    break;
	}
    }

    /* If we need this module, insert the imports and exports */
    if (O->Flags & OBJ_REF) {
        InsertObjGlobals (O);
    }
}



void LibAdd (FILE* F, const char* Name)
/* Add files from the library to the list if there are references that could
 * be satisfied.
 */
{
    unsigned LibName;
    int HaveAdditions;
    unsigned I;
    LibHeader Header;

    /* Store the parameters, so they're visible for other routines */
    Lib     = F;
    LibName = GetStringId (Name);

    /* Read the remaining header fields (magic is already read) */
    Header.Magic   = LIB_MAGIC;
    Header.Version = Read16 (Lib);
    if (Header.Version != LIB_VERSION) {
       	Error ("Wrong data version in `%s'", Name);
    }
    Header.Flags   = Read16 (Lib);
    Header.IndexOffs = Read32 (Lib);

    /* Seek to the index position and read the index */
    fseek (Lib, Header.IndexOffs, SEEK_SET);
    ReadIndex ();

    /* Walk through all library modules and check for each module if there
     * are unresolved externals in existing modules that may be resolved
     * by adding the module. Repeat this step until no more object files
     * were added.
     */
    do {
    	HaveAdditions = 0;
    	for (I = 0; I < ModuleCount; ++I) {
    	    ObjData* O = Index [I];
    	    if ((O->Flags & OBJ_REF) == 0) {
    		LibCheckExports (O);
    		if (O->Flags & OBJ_REF) {
    		    /* The routine added the file */
    		    HaveAdditions = 1;
    		}
    	    }
    	}
    } while (HaveAdditions);

    /* Add the files list and sections for all requested modules */
    for (I = 0; I < ModuleCount; ++I) {

        /* Get the object data */
 	ObjData* O = Index [I];

        /* Is this object file referenced? */
 	if (O->Flags & OBJ_REF) {

 	    /* Seek to the start of the object file and read the header */
 	    fseek (Lib, O->Start, SEEK_SET);
 	    LibReadObjHeader (O, Name);

 	    /* Seek to the start of the files list and read the files list */
 	    ObjReadFiles (Lib, O->Start + O->Header.FileOffs, O);

 	    /* Seek to the start of the debug info and read the debug info */
       	    ObjReadDbgSyms (Lib, O->Start + O->Header.DbgSymOffs, O);

	    /* Seek to the start of the line infos and read them */
	    ObjReadLineInfos (Lib, O->Start + O->Header.LineInfoOffs, O);

            /* Read the assertions from the object file */
            ObjReadAssertions (Lib, O->Start + O->Header.AssertOffs, O);

 	    /* Seek to the start of the segment list and read the segments.
	     * This must be last, since the data here may reference other
	     * stuff.
	     */
 	    ObjReadSections (Lib, O->Start + O->Header.SegOffs, O);

            /* Add a pointer to the library name */
            O->LibName = LibName;

            /* All references to strings are now resolved, so we can delete
             * the module string pool.
             */
            FreeObjStrings (O);

            /* Insert the object into the list of all used object files */
            InsertObjData (O);

        } else {

            /* Unreferenced object file, remove it */
            FreeObjData (O);

        }
    }

    /* Done. Close the file, release allocated memory */
    fclose (F);
    xfree (Index);
    Lib		= 0;
    ModuleCount = 0;
    Index	= 0;
}



