/*****************************************************************************/
/*                                                                           */
/*				   dbginfo.c				     */
/*                                                                           */
/*		    Debug info handling for the ld65 linker                  */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2001-2010, Ullrich von Bassewitz                                      */
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



/* ld65 */
#include "dbginfo.h"
#include "fileinfo.h"
#include "lineinfo.h"
#include "segments.h"
#include "spool.h"



/*****************************************************************************/
/*     	      	     		     Code			       	     */
/*****************************************************************************/



void PrintDbgInfo (ObjData* O, FILE* F)
/* Print the debug info into a file */
{
    unsigned I, J;

    /* Output the files section */
    for (I = 0; I < CollCount (&O->Files); ++I) {
	const FileInfo* FI = CollConstAt (&O->Files, I);
	fprintf (F,
                 "file\tid=%u,name=\"%s\",size=%lu,mtime=0x%08lX\n",
                 FI->Id, GetString (FI->Name), FI->Size, FI->MTime);
    }

    /* Output the line infos */
    for (I = 0; I < CollCount (&O->LineInfos); ++I) {

	/* Get this line info */
	const LineInfo* LI = CollConstAt (&O->LineInfos, I);

	/* Get a pointer to the code ranges */
	const Collection* CodeRanges = &LI->CodeRanges;

	/* Code ranges */
	for (J = 0; J < CollCount (CodeRanges); ++J) {

	    /* Get this code range */
	    const CodeRange* R = CollConstAt (CodeRanges, J);

	    /* Print it */
            fprintf (F,
                     "line\tfile=%u,line=%lu,segment=%u,range=0x%06lX-0x%06lX",
                     LI->File->Id, LI->Pos.Line, R->Seg->Id,
                     R->Offs, R->Offs + R->Size - 1);
	}

	/* Terminate the line */
	fprintf (F, "\n");
    }
}




