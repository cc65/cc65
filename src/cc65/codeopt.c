/*****************************************************************************/
/*                                                                           */
/*				   codeopt.c				     */
/*                                                                           */
/*			     Optimizer subroutines			     */
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



/* b6502 */
#include "codeent.h"
#include "codeopt.h"



/*****************************************************************************/
/*  	       	 	  	     Data				     */
/*****************************************************************************/



/* Counter for the number of changes in one run. The optimizer process is
 * repeated until there are no more changes.
 */
static unsigned OptChanges;



/*****************************************************************************/
/*			       Remove dead jumps			     */
/*****************************************************************************/



static void OptDeadJumps (CodeSeg* S)
/* Remove dead jumps (jumps to the next instruction) */
{
    CodeEntry* E;
    unsigned I;

    /* Get the number of entries, bail out if we have less than two entries */
    unsigned Count = CollCount (&S->Entries);
    if (Count < 2) {
     	return;
    }

    /* Walk over all entries minus the last one */
    I = 0;
    while (I < Count-1) {

	/* Get the next entry */
	E = CollAt (&S->Entries, I);

	/* Check if it's a branch, if it has a local target, and if the target
	 * is the next instruction.
	 */
	if (E->AM == AM_BRA) {
	    printf ("BRA on entry %u:\n", I);
	    if (E->JumpTo) {
		printf ("  JumpTo ok\n");
		if (E->JumpTo->Owner == CollAt (&S->Entries, I+1)) {
		    printf ("  Branch to next insn\n");
		}
	    }
	}

	if (E->AM == AM_BRA && E->JumpTo && E->JumpTo->Owner == CollAt (&S->Entries, I+1)) {

	    /* Remember the label */
	    CodeLabel* L = E->JumpTo;

	    /* Jump to next instruction, remove it */
	    unsigned Remaining = RemoveLabelRef (L, E);
	    CollDelete (&S->Entries, I);
	    FreeCodeEntry (E);
	    --Count;

	    /* If the label has no more references, remove it */
	    if (Remaining == 0) {
		CollDeleteItem (&L->Owner->Labels, L);
		FreeCodeLabel (L);
	    }

	    /* Remember we had changes */
	    ++OptChanges;

	} else {

	    /* Next entry */
	    ++I;

	}
    }
}



/*****************************************************************************/
/*     	       	      	  	     Code				     */
/*****************************************************************************/



void RunOpt (CodeSeg* S)
/* Run the optimizer */
{
    printf ("Optimize\n");

    /* Repeat all steps until there are no more changes */
    do {

     	/* Reset the number of changes */
     	OptChanges = 0;

	OptDeadJumps (S);

    } while (OptChanges > 0);
}



