/*****************************************************************************/
/*                                                                           */
/*				   filetab.h				     */
/*                                                                           */
/*			   Input file table for ca65			     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000     Ullrich von Bassewitz                                        */
/*              Wacholderweg 14                                              */
/*              D-70597 Stuttgart                                            */
/* EMail:       uz@musoftware.de                                             */
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



/* common */
#include "check.h"
#include "xmalloc.h"

/* ca65 */
#include "error.h"
#include "objfile.h"
#include "filetab.h"



/*****************************************************************************/
/*     	       	    		     Data			   	     */
/*****************************************************************************/



/* List of input files */
static struct {
    unsigned long  MTime;		/* Time of last modification */
    unsigned long  Size;		/* Size of file */
    const char*	   Name;		/* Name of file */
} Files [MAX_INPUT_FILES];
static unsigned    FileCount = 0;



/*****************************************************************************/
/*     	       	    		     Code			   	     */
/*****************************************************************************/



const char* GetFileName (unsigned Name)
/* Get the name of a file where the name index is known */
{
    PRECONDITION (Name <= FileCount);
    if (Name == 0) {
	/* Name was defined outside any file scope, use the name of the first
	 * file instead. Errors are then reported with a file position of
     	 * line zero in the first file.
	 */
	if (FileCount == 0) {
    	    /* No files defined until now */
       	    return "(outside file scope)";
	} else {
	    return Files [0].Name;
	}
    } else {
        return Files [Name-1].Name;
    }
}



unsigned AddFile (const char* Name, unsigned long Size, unsigned long MTime)
/* Add a new file to the list of input files. Return the index of the file in
 * the table.
 */
{
    /* Check for a table overflow */
    if (FileCount >= MAX_INPUT_FILES) {
	/* Table overflow */
	Fatal (FAT_MAX_INPUT_FILES);
    }

    /* Add the file to the table */
    Files [FileCount].Name  = xstrdup (Name);
    Files [FileCount].Size  = Size;
    Files [FileCount].MTime = MTime;

    /* One more file */
    return ++FileCount;
}



void WriteFiles (void)
/* Write the list of input files to the object file */
{
    unsigned I;

    /* Tell the obj file module that we're about to start the file list */
    ObjStartFiles ();

    /* Write the file count */
    ObjWriteVar (FileCount);

    /* Write the file data */
    for (I = 0; I < FileCount; ++I) {
	ObjWrite32 (Files [I].MTime);
	ObjWrite32 (Files [I].Size);
	ObjWriteStr (Files [I].Name);
    }

    /* Done writing files */
    ObjEndFiles ();
}



