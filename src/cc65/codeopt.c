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



/* common */
#include "print.h"

/* cc65 */
#include "codeent.h"
#include "codeinfo.h"
#include "global.h"
#include "codeopt.h"



/*****************************************************************************/
/*  	       	  	  	     Data				     */
/*****************************************************************************/



/* Counter for the number of changes in one run. The optimizer process is
 * repeated until there are no more changes.
 */
static unsigned OptChanges;



/*****************************************************************************/
/*		  	       Remove dead jumps			     */
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
	if (E->AM == AM_BRA && E->JumpTo && E->JumpTo->Owner == CollAt (&S->Entries, I+1)) {

	    /* Delete the dead jump */
	    DelCodeEntry (S, I);

	    /* Keep the number of entries updated */
	    --Count;

	    /* Remember, we had changes */
	    ++OptChanges;

	} else {

	    /* Next entry */
	    ++I;

	}
    }
}



/*****************************************************************************/
/*			       Remove dead code				     */
/*****************************************************************************/



static void OptDeadCode (CodeSeg* S)
/* Remove dead code (code that follows an unconditional jump or an rts/rti
 * and has no label)
 */
{
    unsigned I;

    /* Get the number of entries, bail out if we have less than two entries */
    unsigned Count = CollCount (&S->Entries);
    if (Count < 2) {
     	return;
    }

    /* Walk over all entries minus the last one */
    I = 0;
    while (I < Count-1) {

 	/* Get this entry */
 	CodeEntry* E = CollAt (&S->Entries, I);

       	/* Check if it's an unconditional branch, and if the next entry has
 	 * no labels attached
 	 */
       	if ((E->OPC == OPC_JMP || E->OPC == OPC_BRA || E->OPC == OPC_RTS || E->OPC == OPC_RTI) &&
       	    !CodeEntryHasLabel (CollAt (&S->Entries, I+1))) {

 	    /* Delete the next entry */
 	    DelCodeEntry (S, I+1);

 	    /* Keep the number of entries updated */
 	    --Count;

 	    /* Remember, we had changes */
 	    ++OptChanges;

 	} else {

 	    /* Next entry */
 	    ++I;

 	}
    }
}



/*****************************************************************************/
/*			    Optimize jump cascades			     */
/*****************************************************************************/



static void OptJumpCascades (CodeSeg* S)
/* Optimize jump cascades (jumps to jumps). In such a case, the jump is
 * replaced by a jump to the final location. This will in some cases produce
 * worse code, because some jump targets are no longer reachable by short
 * branches, but this is quite rare, so there are more advantages than
 * disadvantages.
 */
{
    unsigned I;

    /* Get the number of entries, bail out if we have no entries */
    unsigned Count = CollCount (&S->Entries);
    if (Count == 0) {
     	return;
    }

    /* Walk over all entries */
    I = 0;
    while (I < Count) {

	CodeLabel* OldLabel;
	CodeLabel* NewLabel;

	/* Get this entry */
	CodeEntry* E = CollAt (&S->Entries, I);

       	/* Check if it's a branch, if it has a label attached, and if the
	 * instruction at this label is also a branch.
	 */
	if (E->AM == AM_BRA 				&&
	    (OldLabel = E->JumpTo) != 0 		&&
       	    OldLabel->Owner->AM == AM_BRA 		&&
	    (NewLabel = OldLabel->Owner->JumpTo) != 0) {

	    /* Get the instruction that has the new label attached */
	    CodeEntry* N = OldLabel->Owner;

	    /* Remove the reference to our label and delete it if this was
	     * the last reference.
	     */
	    if (RemoveLabelRef (OldLabel, E) == 0) {
		/* Delete it */
		DelCodeLabel (S, OldLabel);
	    }

	    /* Remove usage information from the entry and use the usage
	     * information from the new instruction instead.
	     */
	    E->Info &= ~(CI_MASK_USE | CI_MASK_CHG);
	    E->Info |= N->Info & ~(CI_MASK_USE | CI_MASK_CHG);

	    /* Use the new label */
	    AddLabelRef (NewLabel, E);

	    /* Remember ,we had changes */
	    ++OptChanges;

	}

	/* Next entry */
	++I;

    }
}



/*****************************************************************************/
/*     	       	      	  	     Code				     */
/*****************************************************************************/



void RunOpt (CodeSeg* S)
/* Run the optimizer */
{
    typedef void (*OptFunc) (CodeSeg*);

    /* Table with optimizer steps -  are called in this order */
    static const OptFunc OptFuncs [] = {
	OptJumpCascades,	/* Optimize jump cascades */
       	OptDeadJumps,  		/* Remove dead jumps */
	OptDeadCode,		/* Remove dead code */
    };

    /* Repeat all steps until there are no more changes */
    do {

	unsigned long Flags;
	unsigned      I;

     	/* Reset the number of changes */
     	OptChanges = 0;

       	/* Run all optimization steps */
	Flags = 1UL;
       	for (I = 0; I < sizeof(OptFuncs)/sizeof(OptFuncs[0]); ++I) {
	    if ((OptDisable & Flags) == 0) {
		OptFuncs[I] (S);
	    } else if (Verbosity > 0 || Debug) {
		printf ("Optimizer pass %u skipped\n", I);
	    }
	    Flags <<= 1;
	}

    } while (OptChanges > 0);
}



