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
	E = GetCodeEntry (S, I);

	/* Check if it's a branch, if it has a local target, and if the target
	 * is the next instruction.
	 */
	if (E->AM == AM_BRA && E->JumpTo && E->JumpTo->Owner == GetCodeEntry (S, I+1)) {

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
 	CodeEntry* E = GetCodeEntry (S, I);

       	/* Check if it's an unconditional branch, and if the next entry has
 	 * no labels attached
 	 */
       	if ((E->Info & OF_DEAD) != 0 && !CodeEntryHasLabel (GetCodeEntry (S, I+1))) {

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
	CodeEntry* E = GetCodeEntry (S, I);

       	/* Check if it's a branch, if it has a label attached, and if the
	 * instruction at this label is also a branch, and (important) if
	 * both instructions are not identical.
	 */
       	if (E->AM == AM_BRA    	       	 		&&	/* It's a branch */
	    (OldLabel = E->JumpTo) != 0 		&&	/* Label attached */
       	    OldLabel->Owner->AM == AM_BRA 		&&	/* Jumps to a branch.. */
	    (NewLabel = OldLabel->Owner->JumpTo) != 0	&&	/* ..which has a label */
	    OldLabel->Owner != E) {				/* And both are distinct */

	    /* Get the instruction that has the new label attached */
	    CodeEntry* N = OldLabel->Owner;

	    /* Remove the reference to our label and delete it if this was
	     * the last reference.
	     */
	    if (RemoveLabelRef (OldLabel, E) == 0) {
		/* Delete it */
		DelCodeLabel (S, OldLabel);
	    }

	    /* Use the usage information from the new instruction */
	    E->Use = N->Use;
	    E->Chg = N->Chg;

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
/*			       Optimize jsr/rts				     */
/*****************************************************************************/



static void OptRTS (CodeSeg* S)
/* Optimize subroutine calls followed by an RTS. The subroutine call will get
 * replaced by a jump. Don't bother to delete the RTS if it does not have a
 * label, the dead code elimination should take care of it.
 */
{
    unsigned I;

    /* Get the number of entries, bail out if we have less than 2 entries */
    unsigned Count = CollCount (&S->Entries);
    if (Count < 2) {
     	return;
    }

    /* Walk over all entries minus the last one */
    I = 0;
    while (I < Count-1) {

	/* Get this entry */
	CodeEntry* E = GetCodeEntry (S, I);

	/* Check if it's a subroutine call and if the following insn is RTS */
	if (E->OPC == OPC_JSR && GetCodeEntry(S,I+1)->OPC == OPC_RTS) {

	    /* Change the jsr to a jmp */
	    E->OPC = OPC_JMP;

	    /* Change the opcode info to that of the jump */
	    E->Info = GetOPCInfo (OPC_JMP);
					   
       	    /* Remember, we had changes */
	    ++OptChanges;

	}

	/* Next entry */
	++I;

    }
}



/*****************************************************************************/
/*			     Optimize jump targets			     */
/*****************************************************************************/



static void OptJumpTarget (CodeSeg* S)
/* If the instruction preceeding an unconditional branch is the same as the
 * instruction preceeding the jump target, the jump target may be moved
 * one entry back. This is a size optimization, since the instruction before
 * the branch gets removed.
 */
{
    unsigned I;

    /* Get the number of entries, bail out if we have not enough */
    unsigned Count = CollCount (&S->Entries);
    if (Count < 3) {
     	return;
    }

    /* Walk over all entries minus the first one */
    I = 1;
    while (I < Count) {

	/* Get this entry and the entry before this one */
	CodeEntry* E = GetCodeEntry (S, I);

	/* Check if we have a jump or branch, and a matching label */
	if ((E->Info & OF_UBRA) != 0 && E->JumpTo) {

       	    /* Remember, we had changes */
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
	OptJumpCascades, 	/* Optimize jump cascades */
       	OptDeadJumps,  	 	/* Remove dead jumps */
	OptDeadCode,	 	/* Remove dead code */
	OptRTS,			/* Change jsr/rts to jmp */
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



