/*****************************************************************************/
/*                                                                           */
/*				   dbgfile.c				     */
/*                                                                           */
/*                  Debug file creation for the ld65 linker                  */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2003-2011, Ullrich von Bassewitz                                      */
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
#include "dbgsyms.h"
#include "error.h"
#include "fileinfo.h"
#include "global.h"
#include "lineinfo.h"
#include "scopes.h"
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

    /* Output modules */
    for (I = 0; I < CollCount (&ObjDataList); ++I) {

        /* Get this object file */
        const ObjData* O = CollConstAt (&ObjDataList, I);

        /* The main source file is the one at index zero */
        const FileInfo* Source = CollConstAt (&O->Files, 0);

        /* Output the module line */
        fprintf (F,
                 "module\tid=%u,name=\"%s\",file=%u",
                 I,
                 GetObjFileName (O),
                 Source->Id);

        /* Add library if any */
        if (O->LibName != INVALID_STRING_ID) {
            fprintf (F,
                     ",lib=\"%s\",mtime=0x%08lX",
                     GetString (O->LibName),
                     O->MTime);
        }

        /* Terminate the output line */
        fputc ('\n', F);
    }

    /* Output the segment info */
    PrintDbgSegments (F);

    /* Output files */
    PrintDbgFileInfo (F);

    /* Output line info */
    PrintDbgLineInfo (F);

    /* Output symbols */
    PrintDbgSyms (F);

    /* Output scopes */
    PrintDbgScopes (F);

    /* Close the file */
    if (fclose (F) != 0) {
	Error ("Error closing debug file `%s': %s", DbgFileName, strerror (errno));
    }
}



