/*****************************************************************************/
/*                                                                           */
/*				  lineinfo.c                                 */
/*                                                                           */
/*			Source file line info structure                      */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2001      Ullrich von Bassewitz                                       */
/*               Wacholderweg 14                                             */
/*               D-70597 Stuttgart                                           */
/* EMail:        uz@cc65.org                                                 */
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
#include "coll.h"
#include "xmalloc.h"

/* ca65 */
#include "lineinfo.h"



/*****************************************************************************/
/*  				     Data                                    */
/*****************************************************************************/



/* Linked list of all line infos */
LineInfo* LineInfoRoot  = 0;
LineInfo* LineInfoLast  = 0;
unsigned  LineInfoCount = 0;
unsigned  LineInfoValid = 0;              /* Valid, that is, used entries */

/* Static pointer to last line info or NULL if not active */
LineInfo* CurLineInfo   = 0;



/*****************************************************************************/
/*     	       	      	      	     Code			     	     */
/*****************************************************************************/



static LineInfo* NewLineInfo (unsigned FileIndex, unsigned long LineNum)
/* Create and return a new line info. Usage will be zero. */
{
    /* Allocate memory */
    LineInfo* LI = xmalloc (sizeof (LineInfo));

    /* Initialize the fields */
    LI->Next      = 0;
    LI->Usage     = 0;
    LI->LineNum   = LineNum;
    LI->FileIndex = FileIndex;
    LI->Index     = 0;           /* Currently invalid */

    /* Insert this structure into the line info list */
    if (LineInfoLast == 0) {
	LineInfoRoot = LI;
    } else {
	LineInfoLast->Next = LI;
    }
    LineInfoLast = LI;

    /* Count the line infos */
    ++LineInfoCount;

    /* Return the new struct */
    return LI;
}



LineInfo* UseLineInfo (LineInfo* LI)
/* Increase the reference count of the given line info and return it. The
 * function will gracefully accept NULL pointers and do nothing in this case.
 */
{
    CHECK (LI != 0);
    ++LI->Usage;
    return LI;
}



void GenLineInfo (unsigned FileIndex, unsigned long LineNum)
/* Generate a new line info */
{
    /* Create a new line info and make it current */
    CurLineInfo = NewLineInfo (FileIndex, LineNum);
}



void MakeLineInfoIndex (void)
/* Walk over the line info list and make an index of all entries ignoring
 * those with a usage count of zero.
 */
{
    LineInfo* LI  = LineInfoRoot;
    LineInfoValid = 0;
    while (LI) {
	if (LI->Usage) {
	    LI->Index = LineInfoValid++;
	}
	LI = LI->Next;
    }
}



