/*****************************************************************************/
/*                                                                           */
/*				   dbgfile.c				     */
/*                                                                           */
/*                  Debug file creation for the ld65 linker                  */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2003-2010, Ullrich von Bassewitz                                      */
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



#include <stdio.h>
#include <string.h>
#include <errno.h>

/* ld65 */
#include "dbgfile.h"
#include "dbginfo.h"
#include "dbgsyms.h"
#include "error.h"
#include "global.h"
#include "segments.h"



/*****************************************************************************/
/*  	     	 		     Code				     */
/*****************************************************************************/



void CreateDbgFile (void)
/* Create a debug info file */
{
    unsigned I;

    /* Open the debug info file */
    FILE* F = fopen (DbgFileName, "w");
    if (F == 0) {
       	Error ("Cannot create debug file `%s': %s", DbgFileName, strerror (errno));
    }

    /* Output version information */
    fprintf (F, "version\tmajor=1,minor=2\n");

    /* Clear the debug sym table (used to detect duplicates) */
    ClearDbgSymTable ();

    /* Output the segment info */
    PrintDbgSegments (F);

    /* Print line infos from all modules we have linked into the output file */
    for (I = 0; I < CollCount (&ObjDataList); ++I) {

        /* Get the object file */
        ObjData* O = CollAtUnchecked (&ObjDataList, I);

        /* Output debug info */
	PrintDbgInfo (O, F);
        PrintDbgSyms (O, F);
    }

    /* Close the file */
    if (fclose (F) != 0) {
	Error ("Error closing debug file `%s': %s", DbgFileName, strerror (errno));
    }
}



