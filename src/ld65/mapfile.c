/*****************************************************************************/
/*                                                                           */
/*				   mapfile.c				     */
/*                                                                           */
/*		     Map file creation for the ld65 linker		     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998     Ullrich von Bassewitz                                        */
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



#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "global.h"
#include "error.h"
#include "objdata.h"
#include "segments.h"
#include "dbgsyms.h"
#include "exports.h"
#include "config.h"
#include "mapfile.h"



/*****************************************************************************/
/*  				     Code				     */
/*****************************************************************************/



void CreateMapFile (void)
/* Create a map file */
{
    ObjData* O;
    unsigned I;

    /* Open the map file */
    FILE* F = fopen (MapFileName, "w");
    if (F == 0) {
     	Error ("Cannot create map file `%s': %s", MapFileName, strerror (errno));
    }

    /* Write a modules list */
    fprintf (F, "Modules list:\n"
     	       	"-------------\n");
    O = ObjRoot;
    while (O) {
     	if (O->Flags & OBJ_HAVEDATA) {
     	    /* We've linked this module */
     	    if (O->LibName) {
     	       	/* The file is from a library */
     	       	fprintf (F, "%s(%s):\n", O->LibName, O->Name);
     	    } else {
     	       	fprintf (F, "%s:\n", O->Name);
     	    }
     	    for (I = 0; I < O->SectionCount; ++I) {
     		const Section* S = O->Sections [I];
     		/* Don't include zero sized sections if not explicitly
     		 * requested
     		 */
     		if (VerboseMap || S->Size > 0) {
       	       	    fprintf (F, "    %-15s   Offs = %06lX   Size = %06lX\n",
     			     S->Seg->Name, S->Offs, S->Size);
     		}
     	    }
     	}
     	O = O->Next;
    }

    /* Write the segment list */
    fprintf (F, "\n\n"
       	       	"Segment list:\n"
     		"-------------\n");
    PrintSegmentMap (F);

    /* Write the exports list */
    fprintf (F, "\n\n"
		"Exports list:\n"
		"-------------\n");
    PrintExportMap (F);

    /* Write the imports list */
    fprintf (F, "\n\n"
		"Imports list:\n"
		"-------------\n");
    PrintImportMap (F);

    /* Close the file */
    if (fclose (F) != 0) {
	Error ("Error closing map file `%s': %s", MapFileName, strerror (errno));
    }
}



void CreateLabelFile (void)
/* Create a label file */
{
    ObjData* O;

    /* Open the map file */
    FILE* F = fopen (LabelFileName, "w");
    if (F == 0) {
    	Error ("Cannot create label file `%s': %s", LabelFileName, strerror (errno));
    }

    /* Print the labels for the export symbols */
    PrintExportLabels (F);

    /* Print debug symbols from all modules we have linked into the output file */
    O = ObjRoot;
    while (O) {
     	if (O->Flags & OBJ_HAVEDATA) {
     	    /* We've linked this module */
	    PrintDbgSymLabels (O, F);

     	}
     	O = O->Next;
    }

    /* If we should mark write protected areas as such, do it */
    if (WProtSegs) {
	SegDesc* S = SegDescList;
	while (S) {
	    /* Is this segment write protected and contains data? */
	    if (S->Flags & SF_WPROT && S->Seg->Size > 0) {
		/* Write protect the memory area in VICE */
		fprintf (F, "wp %04lX %04lX\n",
			 S->Seg->PC,
			 S->Seg->PC + S->Seg->Size - 1);
	    }
	    /* Next segment */
	    S = S->Next;
	}
    }

    /* Close the file */
    if (fclose (F) != 0) {
	Error ("Error closing map file `%s': %s", LabelFileName, strerror (errno));
    }
}



