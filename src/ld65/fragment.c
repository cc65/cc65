/*****************************************************************************/
/*                                                                           */
/*				  fragment.c				     */
/*                                                                           */
/*			  Code/data fragment routines			     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2003 Ullrich von Bassewitz                                       */
/*               Römerstrasse 52                                             */
/*               D-70794 Filderstadt                                         */
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
#include "xmalloc.h"

/* ld65 */
#include "segments.h"
#include "fragment.h"



/*****************************************************************************/
/*     	      	     	   	     Code  	      	  	  	     */
/*****************************************************************************/



Fragment* NewFragment (unsigned char Type, unsigned long Size, Section* S)
/* Create a new fragment and insert it into the section S */
{
    /* Allocate memory */
    Fragment* F = xmalloc (sizeof (Fragment) - 1 + Size);    

    /* Initialize the data */
    F->Next      = 0;
    F->Obj       = 0;
    F->Size      = Size;
    F->Expr      = 0;
    InitFilePos (&F->Pos);
    F->LI        = 0;
    F->WarnExpr  = 0;
    F->ErrorExpr = 0;
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



