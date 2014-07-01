/*****************************************************************************/
/*                                                                           */
/*                                 mapfile.c                                 */
/*                                                                           */
/*                   Map file creation for the ld65 linker                   */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2010, Ullrich von Bassewitz                                      */
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
#include "config.h"
#include "dbgsyms.h"
#include "exports.h"
#include "global.h"
#include "error.h"
#include "library.h"
#include "mapfile.h"
#include "objdata.h"
#include "segments.h"
#include "spool.h"



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



void CreateMapFile (int ShortMap)
/* Create a map file. If ShortMap is true, only the segment lists are
** generated, not the import/export lists.
*/
{
    unsigned I;

    /* Open the map file */
    FILE* F = fopen (MapFileName, "w");
    if (F == 0) {
        Error ("Cannot create map file `%s': %s", MapFileName, strerror (errno));
    }

    /* Write a modules list */
    fprintf (F, "Modules list:\n"
                "-------------\n");
    for (I = 0; I < CollCount (&ObjDataList); ++I) {

        unsigned J;

        /* Get the object file */
        const ObjData* O = CollConstAt (&ObjDataList, I);

        /* Output the data */
        if (O->Lib) {
            /* The file is from a library */
            fprintf (F, "%s(%s):\n", GetLibFileName (O->Lib), GetObjFileName (O));
        } else {
            fprintf (F, "%s:\n", GetObjFileName (O));
        }
        for (J = 0; J < CollCount (&O->Sections); ++J) {
            const Section* S = CollConstAt (&O->Sections, J);
            /* Don't include zero sized sections if not explicitly
            ** requested
            */
            if (VerboseMap || S->Size > 0) {
                fprintf (F, 
                         "    %-17s Offs=%06lX  Size=%06lX  "
                         "Align=%05lX  Fill=%04lX\n",
                         GetString (S->Seg->Name), S->Offs, S->Size,
                         S->Alignment, S->Fill);
            }
        }
    }

    /* Write the segment list */
    fprintf (F, "\n\n"
                "Segment list:\n"
                "-------------\n");
    PrintSegmentMap (F);

    /* The remainder is not written for short map files */
    if (!ShortMap) {

        /* Write the exports list by name */
        fprintf (F, "\n\n"
                    "Exports list by name:\n"
                    "---------------------\n");
        PrintExportMapByName (F);

        /* Write the exports list by value */
        fprintf (F, "\n\n"
                    "Exports list by value:\n"
                    "----------------------\n");
        PrintExportMapByValue (F);

        /* Write the imports list */
        fprintf (F, "\n\n"
                    "Imports list:\n"
                    "-------------\n");
        PrintImportMap (F);
    }

    /* Close the file */
    if (fclose (F) != 0) {
        Error ("Error closing map file `%s': %s", MapFileName, strerror (errno));
    }
}



void CreateLabelFile (void)
/* Create a label file */
{
    /* Open the label file */
    FILE* F = fopen (LabelFileName, "w");
    if (F == 0) {
        Error ("Cannot create label file `%s': %s", LabelFileName, strerror (errno));
    }

    /* Print the labels for the export symbols */
    PrintExportLabels (F);

    /* Output the labels */
    PrintDbgSymLabels (F);

    /* Close the file */
    if (fclose (F) != 0) {
        Error ("Error closing label file `%s': %s", LabelFileName, strerror (errno));
    }
}
