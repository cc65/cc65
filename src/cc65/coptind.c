/*****************************************************************************/
/*                                                                           */
/*				   coptind.c				     */
/*                                                                           */
/*		Environment independent low level optimizations		     */
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



#include <string.h>

/* cc65 */
#include "codeent.h"
#include "codeinfo.h"
#include "codeopt.h"
#include "error.h"
#include "coptind.h"



/*****************************************************************************/
/*			  Replace jumps to RTS by RTS			     */
/*****************************************************************************/



unsigned OptRTSJumps (CodeSeg* S)
/* Replace jumps to RTS by RTS */
{
    unsigned Changes = 0;

    /* Walk over all entries minus the last one */
    unsigned I = 0;
    while (I < GetCodeEntryCount (S)) {

	/* Get the next entry */
	CodeEntry* E = GetCodeEntry (S, I);

       	/* Check if it's an unconditional branch to a local target */
       	if ((E->Info & OF_UBRA) != 0 		&&
	    E->JumpTo != 0  			&&
	    E->JumpTo->Owner->OPC == OP65_RTS) {

	    /* Insert an RTS instruction */
	    CodeEntry* X = NewCodeEntry (OP65_RTS, AM65_IMP, 0, 0, E->LI);
	    InsertCodeEntry (S, X, I+1);

	    /* Delete the jump */
	    DelCodeEntry (S, I);

	    /* Remember, we had changes */
	    ++Changes;

	}

	/* Next entry */
	++I;

    }

    /* Return the number of changes made */
    return Changes;
}



/*****************************************************************************/
/*	      	  	       Remove dead jumps		      	     */
/*****************************************************************************/



unsigned OptDeadJumps (CodeSeg* S)
/* Remove dead jumps (jumps to the next instruction) */
{
    unsigned Changes = 0;
    CodeEntry* E;
    unsigned I;

    /* Get the number of entries, bail out if we have less than two entries */
    unsigned Count = GetCodeEntryCount (S);
    if (Count < 2) {
     	return 0;
    }

    /* Walk over all entries minus the last one */
    I = 0;
    while (I < Count-1) {

	/* Get the next entry */
	E = GetCodeEntry (S, I);

	/* Check if it's a branch, if it has a local target, and if the target
	 * is the next instruction.
	 */
	if (E->AM == AM65_BRA && E->JumpTo && E->JumpTo->Owner == GetCodeEntry (S, I+1)) {

	    /* Delete the dead jump */
	    DelCodeEntry (S, I);

	    /* Keep the number of entries updated */
	    --Count;

	    /* Remember, we had changes */
	    ++Changes;

	} else {

	    /* Next entry */
	    ++I;

	}
    }

    /* Return the number of changes made */
    return Changes;
}



/*****************************************************************************/
/*			       Remove dead code			      	     */
/*****************************************************************************/



unsigned OptDeadCode (CodeSeg* S)
/* Remove dead code (code that follows an unconditional jump or an rts/rti
 * and has no label)
 */
{
    unsigned Changes = 0;
    unsigned I;

    /* Get the number of entries, bail out if we have less than two entries */
    unsigned Count = GetCodeEntryCount (S);
    if (Count < 2) {
     	return 0;
    }

    /* Walk over all entries */
    I = 0;
    while (I < Count) {

	CodeEntry* N;

 	/* Get this entry */
 	CodeEntry* E = GetCodeEntry (S, I);

       	/* Check if it's an unconditional branch, and if the next entry has
 	 * no labels attached
 	 */
       	if ((E->Info & OF_DEAD) != 0 		&&
	    (N = GetNextCodeEntry (S, I)) != 0	&&
	    !CodeEntryHasLabel (N)) {

 	    /* Delete the next entry */
 	    DelCodeEntry (S, I+1);

 	    /* Keep the number of entries updated */
 	    --Count;

 	    /* Remember, we had changes */
 	    ++Changes;

 	} else {

 	    /* Next entry */
 	    ++I;

 	}
    }

    /* Return the number of changes made */
    return Changes;
}



/*****************************************************************************/
/*			    Optimize jump cascades			     */
/*****************************************************************************/



unsigned OptJumpCascades (CodeSeg* S)
/* Optimize jump cascades (jumps to jumps). In such a case, the jump is
 * replaced by a jump to the final location. This will in some cases produce
 * worse code, because some jump targets are no longer reachable by short
 * branches, but this is quite rare, so there are more advantages than
 * disadvantages.
 */
{
    unsigned Changes = 0;

    /* Walk over all entries */
    unsigned I = 0;
    while (I < GetCodeEntryCount (S)) {

	CodeEntry* N;
	CodeLabel* OldLabel;

	/* Get this entry */
	CodeEntry* E = GetCodeEntry (S, I);

       	/* Check if it's a branch, if it has a jump label, if this jump
	 * label is not attached to the instruction itself, and if the
	 * target instruction is itself a branch.
	 */
     	if ((E->Info & OF_BRA) != 0        &&
	    (OldLabel = E->JumpTo) != 0    &&
	    (N = OldLabel->Owner) != E     &&
	    (N->Info & OF_BRA) != 0) {

	    /* Check if we can use the final target label. This is the case,
	     * if the target branch is an absolut branch, or if it is a
	     * conditional branch checking the same condition as the first one.
	     */
	    if ((N->Info & OF_UBRA) != 0 ||
       	       	((E->Info & OF_CBRA) != 0 &&
		 GetBranchCond (E->OPC)  == GetBranchCond (N->OPC))) {

	     	/* This is a jump cascade and we may jump to the final target.
		 * Insert a new instruction, then remove the old one
	     	 */
		CodeEntry* X = NewCodeEntry (E->OPC, E->AM, N->Arg, N->JumpTo, E->LI);

		/* Insert it behind E */
		InsertCodeEntry (S, X, I+1);

		/* Remove E */
		DelCodeEntry (S, I);

	     	/* Remember, we had changes */
	     	++Changes;

	     	/* Done */
	      	continue;

	    }

	    /* Check if both are conditional branches, and the condition of
	     * the second is the inverse of that of the first. In this case,
	     * the second branch will never be taken, and we may jump directly
	     * to the instruction behind this one.
	     */
	    if ((E->Info & OF_CBRA) != 0 && (N->Info & OF_CBRA) != 0) {

		CodeEntry* X;	/* Instruction behind N */
		CodeLabel* LX;	/* Label attached to X */

		/* Get the branch conditions of both branches */
		bc_t BC1 = GetBranchCond (E->OPC);
		bc_t BC2 = GetBranchCond (N->OPC);

		/* Check the branch conditions */
		if (BC1 != GetInverseCond (BC2)) {
		    /* Condition not met */
		    goto NextEntry;
		}

		/* We may jump behind this conditional branch. Get the
		 * pointer to the next instruction
		 */
		if ((X = GetNextCodeEntry (S, GetCodeEntryIndex (S, N))) == 0) {
		    /* N is the last entry, bail out */
		    goto NextEntry;
		}

		/* Get the label attached to X, create a new one if needed */
		LX = GenCodeLabel (S, X);

		/* Move the reference from E to the new label */
		MoveCodeLabelRef (S, E, LX);

	     	/* Remember, we had changes */
	     	++Changes;

	     	/* Done */
	      	continue;

	    }
	}

NextEntry:
	/* Next entry */
	++I;

    }

    /* Return the number of changes made */
    return Changes;
}



/*****************************************************************************/
/*	     	    	       Optimize jsr/rts			      	     */
/*****************************************************************************/



unsigned OptRTS (CodeSeg* S)
/* Optimize subroutine calls followed by an RTS. The subroutine call will get
 * replaced by a jump. Don't bother to delete the RTS if it does not have a
 * label, the dead code elimination should take care of it.
 */
{
    unsigned Changes = 0;
    unsigned I;

    /* Get the number of entries, bail out if we have less than 2 entries */
    unsigned Count = GetCodeEntryCount (S);
    if (Count < 2) {
     	return 0;
    }

    /* Walk over all entries minus the last one */
    I = 0;
    while (I < Count-1) {

	CodeEntry* N;

	/* Get this entry */
	CodeEntry* E = GetCodeEntry (S, I);

	/* Check if it's a subroutine call and if the following insn is RTS */
	if (E->OPC == OP65_JSR         	       	&&
	    (N = GetNextCodeEntry (S, I)) != 0	&&
	    N->OPC == OP65_RTS) {

	    /* Change the jsr to a jmp and use the additional info for a jump */
       	    E->AM = AM65_BRA;
	    ReplaceOPC (E, OP65_JMP);

       	    /* Remember, we had changes */
	    ++Changes;

	}

	/* Next entry */
	++I;

    }

    /* Return the number of changes made */
    return Changes;
}



/*****************************************************************************/
/*	      		     Optimize jump targets			     */
/*****************************************************************************/



unsigned OptJumpTarget (CodeSeg* S)
/* If the instruction preceeding an unconditional branch is the same as the
 * instruction preceeding the jump target, the jump target may be moved
 * one entry back. This is a size optimization, since the instruction before
 * the branch gets removed.
 */
{
    unsigned Changes = 0;
    CodeEntry* E1;     		/* Entry 1 */
    CodeEntry* E2;		/* Entry 2 */
    CodeEntry* T1;		/* Jump target entry 1 */
    CodeEntry* T2;		/* Jump target entry 2 */
    CodeLabel* TL1;		/* Target label 1 */
    unsigned TI;		/* Target index */
    unsigned I;

    /* Get the number of entries, bail out if we have not enough */
    unsigned Count = GetCodeEntryCount (S);
    if (Count < 3) {
     	return 0;
    }

    /* Walk over the entries */
    I = 0;
    while (I < Count-1) {

      	/* Get next entry */
       	E2 = GetCodeEntry (S, I+1);

	/* Check if we have a jump or branch, and a matching label */
       	if ((E2->Info & OF_UBRA) != 0 && E2->JumpTo) {

	    /* Get the target instruction for the label */
	    T2 = E2->JumpTo->Owner;

	    /* Get the entry preceeding this one (if possible) */
	    TI = GetCodeEntryIndex (S, T2);
	    if (TI == 0) {
	       	/* There is no entry before this one */
	       	goto NextEntry;
	    }
	    T1 = GetCodeEntry (S, TI-1);

	    /* Get the entry preceeding the jump */
	    E1 = GetCodeEntry (S, I);

	    /* Check if both preceeding instructions are identical */
	    if (!CodeEntriesAreEqual (E1, T1)) {
	    	/* Not equal, try next */
	    	goto NextEntry;
	    }

	    /* Get the label for the instruction preceeding the jump target.
	     * This routine will create a new label if the instruction does
	     * not already have one.
	     */
      	    TL1 = GenCodeLabel (S, T1);

	    /* Change the jump target to point to this new label */
	    MoveCodeLabelRef (S, E2, TL1);

	    /* If the instruction preceeding the jump has labels attached,
	     * move references to this label to the new label.
	     */
	    if (CodeEntryHasLabel (E1)) {
	    	MoveCodeLabels (S, E1, T1);
	    }

	    /* Remove the entry preceeding the jump */
	    DelCodeEntry (S, I);
	    --Count;

       	    /* Remember, we had changes */
	    ++Changes;

	}

NextEntry:
	/* Next entry */
	++I;

    }

    /* Return the number of changes made */
    return Changes;
}



/*****************************************************************************/
/*		 	 Optimize conditional branches			     */
/*****************************************************************************/



unsigned OptCondBranches (CodeSeg* S)
/* Performs several optimization steps:
 *
 *  - If an immidiate load of a register is followed by a conditional jump that
 *    is never taken because the load of the register sets the flags in such a
 *    manner, remove the conditional branch.
 *  - If the conditional branch is always taken because of the register load,
 *    replace it by a jmp.
 *  - If a conditional branch jumps around an unconditional branch, remove the
 *    conditional branch and make the jump a conditional branch with the
 *    inverse condition of the first one.
 */
{
    unsigned Changes = 0;
    unsigned I;

    /* Get the number of entries, bail out if we have not enough */
    unsigned Count = GetCodeEntryCount (S);
    if (Count < 2) {
     	return 0;
    }

    /* Walk over the entries */
    I = 0;
    while (I < Count-1) {

	CodeEntry* N;
	CodeLabel* L;

      	/* Get next entry */
       	CodeEntry* E = GetCodeEntry (S, I);

	/* Check if it's a register load */
       	if ((E->Info & OF_LOAD) != 0   		&&  /* It's a load instruction */
	    E->AM == AM65_IMM 	       		&&  /* ..with immidiate addressing */
	    (E->Flags & CEF_NUMARG) != 0	&&  /* ..and a numeric argument. */
	    (N = GetNextCodeEntry (S, I)) != 0	&&  /* There is a following entry */
       	    (N->Info & OF_CBRA) != 0		&&  /* ..which is a conditional branch */
	    !CodeEntryHasLabel (N)) {		    /* ..and does not have a label */

	    /* Get the branch condition */
	    bc_t BC = GetBranchCond (N->OPC);

	    /* Check the argument against the branch condition */
       	    if ((BC == BC_EQ && E->Num != 0) 		||
	       	(BC == BC_NE && E->Num == 0)		||
	       	(BC == BC_PL && (E->Num & 0x80) != 0)	||
	    	(BC == BC_MI && (E->Num & 0x80) == 0)) {

	    	/* Remove the conditional branch */
	    	DelCodeEntry (S, I+1);
	    	--Count;

	    	/* Remember, we had changes */
	    	++Changes;

     	    } else if ((BC == BC_EQ && E->Num == 0)		||
     	    	       (BC == BC_NE && E->Num != 0)		||
     	    	       (BC == BC_PL && (E->Num & 0x80) == 0)	||
     	    	       (BC == BC_MI && (E->Num & 0x80) != 0)) {

     		/* The branch is always taken, replace it by a jump */
     		ReplaceOPC (N, OP65_JMP);

     		/* Remember, we had changes */
     		++Changes;
     	    }

	}

      	if ((E->Info & OF_CBRA) != 0	     	&&  /* It's a conditional branch */
      	    (L = E->JumpTo) != 0	     	&&  /* ..referencing a local label */
       	    (N = GetNextCodeEntry (S, I)) != 0	&&  /* There is a following entry */
      	    (N->Info & OF_UBRA) != 0	     	&&  /* ..which is an uncond branch, */
      	    !CodeEntryHasLabel (N)	     	&&  /* ..has no label attached */
      	    L->Owner == GetNextCodeEntry (S, I+1)) {/* ..and jump target follows */

      	    /* Replace the jump by a conditional branch with the inverse branch
      	     * condition than the branch around it.
      	     */
      	    ReplaceOPC (N, GetInverseBranch (E->OPC));

      	    /* Remove the conditional branch */
      	    DelCodeEntry (S, I);
      	    --Count;

      	    /* Remember, we had changes */
      	    ++Changes;

      	}

     	/* Next entry */
     	++I;

    }

    /* Return the number of changes made */
    return Changes;
}



/*****************************************************************************/
/*			      Remove unused loads			     */
/*****************************************************************************/



unsigned OptUnusedLoads (CodeSeg* S)
/* Remove loads of registers where the value loaded is not used later. */
{
    unsigned Changes = 0;

    /* Walk over the entries */
    unsigned I = 0;
    while (I < GetCodeEntryCount (S)) {

	CodeEntry* N;

      	/* Get next entry */
       	CodeEntry* E = GetCodeEntry (S, I);

	/* Check if it's a register load or transfer insn */
	if ((E->Info & (OF_LOAD | OF_XFR)) != 0    &&
	    (N = GetNextCodeEntry (S, I)) != 0     &&
	    (N->Info & OF_FBRA) == 0) {

	    /* Check which sort of load or transfer it is */
	    unsigned R;
	    switch (E->OPC) {
		case OP65_TXA:
		case OP65_TYA:
		case OP65_LDA:	R = REG_A;	break;
		case OP65_TAX:
       	       	case OP65_LDX:  R = REG_X;	break;
		case OP65_TAY:
		case OP65_LDY:	R = REG_Y;	break;
		default:     	goto NextEntry;		/* OOPS */
	    }

	    /* Get register usage and check if the register value is used later */
	    if ((GetRegInfo (S, I+1) & R) == 0) {

		/* Register value is not used, remove the load */
		DelCodeEntry (S, I);

		/* Remember, we had changes */
		++Changes;

	    }
	}

NextEntry:
	/* Next entry */
	++I;

    }

    /* Return the number of changes made */
    return Changes;
}



/*****************************************************************************/
/*		       	     Optimize branch types			     */
/*****************************************************************************/



unsigned OptBranchDist (CodeSeg* S)
/* Change branches for the distance needed. */
{
    unsigned Changes = 0;
    unsigned I;

    /* Get the number of entries, bail out if we have not enough */
    unsigned Count = GetCodeEntryCount (S);

    /* Walk over the entries */
    I = 0;
    while (I < Count) {

      	/* Get next entry */
       	CodeEntry* E = GetCodeEntry (S, I);

	/* Check if it's a conditional branch to a local label. */
       	if ((E->Info & OF_CBRA) != 0) {

	    /* Is this a branch to a local symbol? */
	    if (E->JumpTo != 0) {

		/* Get the index of the branch target */
		unsigned TI = GetCodeEntryIndex (S, E->JumpTo->Owner);

		/* Determine the branch distance */
		int Distance = 0;
		if (TI >= I) {
		    /* Forward branch */
		    unsigned J = I;
		    while (J < TI) {
			CodeEntry* N = GetCodeEntry (S, J++);
			Distance += N->Size;
		    }
		} else {
		    /* Backward branch */
		    unsigned J = TI;
		    while (J < I) {
			CodeEntry* N = GetCodeEntry (S, J++);
			Distance += N->Size;
		    }
		}

		/* Make the branch short/long according to distance */
		if ((E->Info & OF_LBRA) == 0 && Distance > 120) {
		    /* Short branch but long distance */
		    ReplaceOPC (E, MakeLongBranch (E->OPC));
		    ++Changes;
		} else if ((E->Info & OF_LBRA) != 0 && Distance < 120) {
		    /* Long branch but short distance */
		    ReplaceOPC (E, MakeShortBranch (E->OPC));
		    ++Changes;
		}

	    } else if ((E->Info & OF_LBRA) == 0) {

		/* Short branch to external symbol - make it long */
		ReplaceOPC (E, MakeLongBranch (E->OPC));
		++Changes;

	    }
	}

	/* Next entry */
	++I;

    }

    /* Return the number of changes made */
    return Changes;
}







