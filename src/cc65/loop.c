/*****************************************************************************/
/*                                                                           */
/*				    loop.c				     */
/*                                                                           */
/*				Loop management				     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2000 Ullrich von Bassewitz                                       */
/*               Wacholderweg 14                                             */
/*               D-70597 Stuttgart                                           */
/* EMail:        uz@musoftware.de                                            */
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

/* cc65 */
#include "error.h"
#include "loop.h"



/*****************************************************************************/
/*	  	  		     Data		     		     */
/*****************************************************************************/



/* The root */
static LoopDesc* LoopStack = 0;



/*****************************************************************************/
/*	   	  		     Code		     		     */
/*****************************************************************************/



LoopDesc* AddLoop (unsigned sp, unsigned loop, unsigned label,
	     	   unsigned linc, unsigned lstat)
/* Create and add a new loop descriptor */
{
    /* Allocate a new struct */
    LoopDesc* L = (LoopDesc*) xmalloc (sizeof (LoopDesc));

    /* Fill in the data */
    L->StackPtr	= sp;
    L->Loop    	= loop;
    L->Label   	= label;
    L->linc    	= linc;
    L->lstat   	= lstat;

    /* Insert it into the list */
    L->Next = LoopStack;
    LoopStack = L;

    /* Return a pointer to the struct */
    return L;
}



LoopDesc* CurrentLoop (void)
/* Return a pointer to the descriptor of the current loop */
{
    return LoopStack;
}



void DelLoop (void)
/* Remove the current loop */
{
    LoopDesc* L = LoopStack;
    LoopStack = LoopStack->Next;
    xfree (L);
}



