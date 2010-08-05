/*****************************************************************************/
/*                                                                           */
/*				  lineinfo.h                                 */
/*                                                                           */
/*			Source file line info structure                      */
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



/* common */
#include "check.h"
#include "xmalloc.h"

/* ld65 */
#include "fileio.h"
#include "fragment.h"
#include "objdata.h"
#include "segments.h"
#include "lineinfo.h"



/*****************************************************************************/
/*     	       	      	      	     Code			     	     */
/*****************************************************************************/



static CodeRange* NewCodeRange (unsigned long Offs, unsigned long Size)
/* Create and return a new CodeRange struct */
{
    /* Allocate memory */
    CodeRange* R = xmalloc (sizeof (CodeRange));

    /* Initialize the fields */
    R->Offs = Offs;
    R->Size = Size;

    /* Return the new struct */
    return R;
}



LineInfo* NewLineInfo (ObjData* O, const FilePos* Pos)
/* Create and return a new LineInfo struct */
{
    /* Allocate memory */
    LineInfo* LI = xmalloc (sizeof (LineInfo));

    /* Make sure the name index is valid */
    CHECK (Pos->Name < CollCount (&O->Files));

    /* Initialize the fields */
    LI->File = CollAt (&O->Files, Pos->Name);
    LI->Pos = *Pos;
    InitCollection (&LI->Fragments);
    InitCollection (&LI->CodeRanges);

    /* Return the new struct */
    return LI;
}



LineInfo* ReadLineInfo (FILE* F, ObjData* O)
/* Read a line info from a file and return it */
{
    /* Read the file position */
    FilePos Pos;
    ReadFilePos (F, &Pos);

    /* Allocate a new LineInfo struct, initialize and return it */
    return NewLineInfo (O, &Pos);
}



static void AddCodeRange (LineInfo* LI, unsigned long Offs, unsigned long Size)
/* Add a range of code to this line */
{
    unsigned I;

    /* Get a pointer to the collection */
    Collection* CodeRanges = &LI->CodeRanges;

    /* We will keep the CodeRanges collection sorted by starting offset,
     * so we have to search for the correct insert position. Since in most
     * cases, the fragments have increasing order, and there is usually not
     * more than one or two ranges, we do a linear search.
     */
    for (I = 0; I < CollCount (CodeRanges); ++I) {
	CodeRange* R = CollAtUnchecked (CodeRanges, I);
       	if (Offs < R->Offs) {

       	    /* Got the insert position */
       	    if (Offs + Size == R->Offs) {
    		/* Merge the two */
       	     	R->Offs = Offs;
       	     	R->Size += Size;
       	    } else {
       	     	/* Insert a new entry */
       	     	CollInsert (CodeRanges, NewCodeRange (Offs, Size), I);
       	    }

       	    /* Done */
       	    return;

       	} else if (R->Offs + R->Size == Offs) {

	    /* This is the regular case. Merge the two. */
       	    R->Size += Size;

       	    /* Done */
       	    return;

       	}
    }

    /* We must append an entry */
    CollAppend (CodeRanges, NewCodeRange (Offs, Size));
}



void RelocLineInfo (Segment* S)
/* Relocate the line info for a segment. */
{
    unsigned long Offs = S->PC;

    /* Loop over all sections in this segment */
    Section* Sec = S->SecRoot;
    while (Sec) {
       	Fragment* Frag;

       	/* Adjust for fill bytes */
       	Offs += Sec->Fill;

       	/* Loop over all fragments in this section */
       	Frag = Sec->FragRoot;
       	while (Frag) {

            unsigned I;

       	    /* Add the range for this fragment to all line infos */
            for (I = 0; I < CollCount (&Frag->LineInfos); ++I) {
       	       	AddCodeRange (CollAt (&Frag->LineInfos, I), Offs, Frag->Size);
       	    }

       	    /* Update the offset */
       	    Offs += Frag->Size;

       	    /* Next fragment */
       	    Frag = Frag->Next;
       	}

       	/* Next section */
       	Sec = Sec->Next;
    }
}




