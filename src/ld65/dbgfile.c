/*****************************************************************************/
/*                                                                           */
/*                                 dbgfile.c                                 */
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
#include "library.h"
#include "lineinfo.h"
#include "scopes.h"
#include "segments.h"
#include "span.h"
#include "tpool.h"



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



static void AssignIds (void)
/* Assign the base ids for debug info output. Within each module, many of the
** items are addressed by ids which are actually the indices of the items in
** the collections. To make them unique, we must assign a unique base to each
** range.
*/
{
    /* Walk over all modules */
    unsigned I;
    unsigned HLLSymBaseId = 0;
    unsigned ScopeBaseId  = 0;
    unsigned SpanBaseId   = 0;
    unsigned SymBaseId    = 0;
    for (I = 0; I < CollCount (&ObjDataList); ++I) {

        /* Get this module */
        ObjData* O = CollAt (&ObjDataList, I);

        /* Assign the module id */
        O->Id = I;

        /* Assign base ids */
        O->HLLSymBaseId = HLLSymBaseId;
        O->ScopeBaseId  = ScopeBaseId;
        O->SpanBaseId   = SpanBaseId;
        O->SymBaseId    = SymBaseId;

        /* Bump the base ids */
        HLLSymBaseId  += CollCount (&O->HLLDbgSyms);
        ScopeBaseId   += CollCount (&O->Scopes);
        SpanBaseId    += CollCount (&O->Spans);
        SymBaseId     += CollCount (&O->DbgSyms);
    }

    /* Assign the ids to the file infos */
    AssignFileInfoIds ();

    /* Assign the ids to line infos */
    AssignLineInfoIds ();
}



void CreateDbgFile (void)
/* Create a debug info file */
{
    /* Open the debug info file */
    FILE* F = fopen (DbgFileName, "w");
    if (F == 0) {
        Error ("Cannot create debug file `%s': %s", DbgFileName, strerror (errno));
    }

    /* Output version information */
    fprintf (F, "version\tmajor=2,minor=0\n");

    /* Output a line with the item numbers so the debug info module is able
    ** to preallocate the required memory.
    */
    fprintf (
        F,
        "info\tcsym=%u,file=%u,lib=%u,line=%u,mod=%u,scope=%u,seg=%u,span=%u,sym=%u,type=%u\n",
        HLLDbgSymCount (),
        FileInfoCount (),
        LibraryCount (),
        LineInfoCount (),
        ObjDataCount (),
        ScopeCount (),
        SegmentCount (),
        SpanCount (),
        DbgSymCount (),
        TypeCount ()
    );

    /* Assign the ids to the items */
    AssignIds ();

    /* Output high level language symbols */
    PrintHLLDbgSyms (F);

    /* Output files */
    PrintDbgFileInfo (F);

    /* Output libraries */
    PrintDbgLibraries (F);

    /* Output line info */
    PrintDbgLineInfo (F);

    /* Output modules */
    PrintDbgModules (F);

    /* Output the segment info */
    PrintDbgSegments (F);

    /* Output spans */
    PrintDbgSpans (F);

    /* Output scopes */
    PrintDbgScopes (F);

    /* Output symbols */
    PrintDbgSyms (F);

    /* Output types */
    PrintDbgTypes (F);

    /* Close the file */
    if (fclose (F) != 0) {
        Error ("Error closing debug file `%s': %s", DbgFileName, strerror (errno));
    }
}
