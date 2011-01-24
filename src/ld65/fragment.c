/*****************************************************************************/
/*                                                                           */
/*				  fragment.c				     */
/*                                                                           */
/*			  Code/data fragment routines			     */
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



/* common */
#include "fragdefs.h"
#include "xmalloc.h"

/* ld65 */
#include "error.h"
#include "fragment.h"
#include "lineinfo.h"
#include "objdata.h"
#include "segments.h"



/*****************************************************************************/
/*     	      	     	   	     Code  	      	  	  	     */
/*****************************************************************************/



Fragment* NewFragment (unsigned char Type, unsigned Size, Section* S)
/* Create a new fragment and insert it into the section S */
{
    Fragment* F;

    /* Calculate the size of the memory block. LitBuf is only needed if the
     * fragment contains literal data.
     */
    unsigned FragSize = sizeof (Fragment) - 1;
    if (Type == FRAG_LITERAL) {
        FragSize += Size;
    }

    /* Allocate memory */
    F = xmalloc (FragSize);

    /* Initialize the data */
    F->Next      = 0;
    F->Obj       = 0;
    F->Sec       = S;
    F->Size      = Size;
    F->Expr      = 0;
    F->LineInfos = EmptyCollection;
    F->Type      = Type;

    /* Insert the code fragment into the section */
    if (S->FragRoot == 0) {
      	/* First fragment */
      	S->FragRoot = F;
    } else {
      	S->FragLast->Next = F;
    }
    S->FragLast = F;

    /* Increment the size of the section by the size of the fragment */
    S->Size += Size;

    /* Increment the size of the segment that contains the section */
    S->Seg->Size += Size;

    /* Return the new fragment */
    return F;
}



void FragResolveLineInfos (Fragment* F)
/* Resolve the back pointers for the line infos */
{
    unsigned I;

    /* Walk over all line infos for this fragment */
    for (I = 0; I < CollCount (&F->LineInfos); ++I) {
        /* Get a pointer to this line info */
        LineInfo* LI = CollAtUnchecked (&F->LineInfos, I);

        /* Add the back pointer to the line info */
        CollAppend (&LI->Fragments, F);
    }
}



const char* GetFragmentSourceName (const Fragment* F)
/* Return the name of the source file for this fragment */
{
    /* Each fragment has the basic info in line info #0 */
    const LineInfo* LI = CollConstAt (&F->LineInfos, 0);

    /* Return the source file name */
    return GetSourceFileName (F->Obj, LI->Pos.Name);
}



unsigned long GetFragmentSourceLine (const Fragment* F)
/* Return the source file line for this fragment */
{
    /* Each fragment has the basic info in line info #0 */
    const LineInfo* LI = CollConstAt (&F->LineInfos, 0);

    /* Return the source file line */
    return LI->Pos.Line;
}



