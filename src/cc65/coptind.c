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
    while (I < CS_GetEntryCount (S)) {

	/* Get the next entry */
	CodeEntry* E = CS_GetEntry (S, I);

       	/* Check if it's an unconditional branch to a local target */
       	if ((E->Info & OF_UBRA) != 0 		&&
	    E->JumpTo != 0  			&&
	    E->JumpTo->Owner->OPC == OP65_RTS) {

	    /* Insert an RTS instruction */
	    CodeEntry* X = NewCodeEntry (OP65_RTS, AM65_IMP, 0, 0, E->LI);
	    CS_InsertEntry (S, X, I+1);

	    /* Delete the jump */
	    CS_DelEntry (S, I);

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
    unsigned Count = CS_GetEntryCount (S);
    if (Count < 2) {
     	return 0;
    }

    /* Walk over all entries minus the last one */
    I = 0;
    while (I < Count-1) {

	/* Get the next entry */
	E = CS_GetEntry (S, I);

	/* Check if it's a branch, if it has a local target, and if the target
	 * is the next instruction.
	 */
	if (E->AM == AM65_BRA && E->JumpTo && E->JumpTo->Owner == CS_GetEntry (S, I+1)) {

	    /* Delete the dead jump */
	    CS_DelEntry (S, I);

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
    unsigned Count = CS_GetEntryCount (S);
    if (Count < 2) {
     	return 0;
    }

    /* Walk over all entries */
    I = 0;
    while (I < Count) {

	CodeEntry* N;

 	/* Get this entry */
 	CodeEntry* E = CS_GetEntry (S, I);

       	/* Check if it's an unconditional branch, and if the next entry has
 	 * no labels attached
 	 */
       	if ((E->Info & OF_DEAD) != 0   	       &&
	    (N = CS_GetNextEntry (S, I)) != 0  &&
	    !CE_HasLabel (N)) {

 	    /* Delete the next entry */
 	    CS_DelEntry (S, I+1);

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
    while (I < CS_GetEntryCount (S)) {

	CodeEntry* N;
	CodeLabel* OldLabel;

	/* Get this entry */
	CodeEntry* E = CS_GetEntry (S, I);

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
	    	CS_InsertEntry (S, X, I+1);

	    	/* Remove E */
	    	CS_DelEntry (S, I);

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
	    	if ((X = CS_GetNextEntry (S, CS_GetEntryIndex (S, N))) == 0) {
	    	    /* N is the last entry, bail out */
	    	    goto NextEntry;
	    	}

	    	/* Get the label attached to X, create a new one if needed */
	    	LX = CS_GenLabel (S, X);

	    	/* Move the reference from E to the new label */
	    	CS_MoveLabelRef (S, E, LX);

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
    unsigned Count = CS_GetEntryCount (S);
    if (Count < 2) {
     	return 0;
    }

    /* Walk over all entries minus the last one */
    I = 0;
    while (I < Count-1) {

	CodeEntry* N;

	/* Get this entry */
	CodeEntry* E = CS_GetEntry (S, I);

	/* Check if it's a subroutine call and if the following insn is RTS */
	if (E->OPC == OP65_JSR         	       	  &&
	    (N = CS_GetNextEntry (S, I)) != 0 &&
	    N->OPC == OP65_RTS) {

	    /* Change the jsr to a jmp and use the additional info for a jump */
       	    E->AM = AM65_BRA;
	    CE_ReplaceOPC (E, OP65_JMP);

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
    unsigned Count = CS_GetEntryCount (S);
    if (Count < 3) {
     	return 0;
    }

    /* Walk over the entries */
    I = 0;
    while (I < Count-1) {

      	/* Get next entry */
       	E2 = CS_GetEntry (S, I+1);

	/* Check if we have a jump or branch, and a matching label */
       	if ((E2->Info & OF_UBRA) != 0 && E2->JumpTo) {

	    /* Get the target instruction for the label */
	    T2 = E2->JumpTo->Owner;

	    /* Get the entry preceeding this one (if possible) */
	    TI = CS_GetEntryIndex (S, T2);
	    if (TI == 0) {
	       	/* There is no entry before this one */
	       	goto NextEntry;
	    }
	    T1 = CS_GetEntry (S, TI-1);

	    /* Get the entry preceeding the jump */
	    E1 = CS_GetEntry (S, I);

	    /* Check if both preceeding instructions are identical */
	    if (!CodeEntriesAreEqual (E1, T1)) {
	    	/* Not equal, try next */
	    	goto NextEntry;
	    }

	    /* Get the label for the instruction preceeding the jump target.
	     * This routine will create a new label if the instruction does
	     * not already have one.
	     */
      	    TL1 = CS_GenLabel (S, T1);

	    /* Change the jump target to point to this new label */
	    CS_MoveLabelRef (S, E2, TL1);

	    /* If the instruction preceeding the jump has labels attached,
	     * move references to this label to the new label.
	     */
	    if (CE_HasLabel (E1)) {
	       	CS_MoveLabels (S, E1, T1);
	    }

	    /* Remove the entry preceeding the jump */
	    CS_DelEntry (S, I);
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
/*		   	 Optimize conditional branches			     */
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
    unsigned Count = CS_GetEntryCount (S);
    if (Count < 2) {
     	return 0;
    }

    /* Walk over the entries */
    I = 0;
    while (I < Count-1) {

	CodeEntry* N;
	CodeLabel* L;

      	/* Get next entry */
       	CodeEntry* E = CS_GetEntry (S, I);

	/* Check if it's a register load */
       	if ((E->Info & OF_LOAD) != 0   	       	  &&  /* It's a load instruction */
	    E->AM == AM65_IMM 	       		  &&  /* ..with immidiate addressing */
	    (E->Flags & CEF_NUMARG) != 0	  &&  /* ..and a numeric argument. */
	    (N = CS_GetNextEntry (S, I)) != 0     &&  /* There is a following entry */
       	    (N->Info & OF_CBRA) != 0		  &&  /* ..which is a conditional branch */
	    !CE_HasLabel (N)) {		      /* ..and does not have a label */

	    /* Get the branch condition */
	    bc_t BC = GetBranchCond (N->OPC);

	    /* Check the argument against the branch condition */
       	    if ((BC == BC_EQ && E->Num != 0) 	    	||
	       	(BC == BC_NE && E->Num == 0)	    	||
	       	(BC == BC_PL && (E->Num & 0x80) != 0)	||
	    	(BC == BC_MI && (E->Num & 0x80) == 0)) {

	    	/* Remove the conditional branch */
	    	CS_DelEntry (S, I+1);
	    	--Count;

	    	/* Remember, we had changes */
	    	++Changes;

     	    } else if ((BC == BC_EQ && E->Num == 0) 		||
     	    	       (BC == BC_NE && E->Num != 0) 		||
     	    	       (BC == BC_PL && (E->Num & 0x80) == 0)	||
     	    	       (BC == BC_MI && (E->Num & 0x80) != 0)) {

     		/* The branch is always taken, replace it by a jump */
     		CE_ReplaceOPC (N, OP65_JMP);

     		/* Remember, we had changes */
     		++Changes;
     	    }

	}

      	if ((E->Info & OF_CBRA) != 0	     	  &&  /* It's a conditional branch */
      	    (L = E->JumpTo) != 0	     	  &&  /* ..referencing a local label */
       	    (N = CS_GetNextEntry (S, I)) != 0     &&  /* There is a following entry */
      	    (N->Info & OF_UBRA) != 0	     	  &&  /* ..which is an uncond branch, */
      	    !CE_HasLabel (N) 	     	          &&  /* ..has no label attached */
      	    L->Owner == CS_GetNextEntry (S, I+1)) {/* ..and jump target follows */

      	    /* Replace the jump by a conditional branch with the inverse branch
      	     * condition than the branch around it.
      	     */
      	    CE_ReplaceOPC (N, GetInverseBranch (E->OPC));

      	    /* Remove the conditional branch */
      	    CS_DelEntry (S, I);
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
/*			Remove unused loads and stores                       */
/*****************************************************************************/



unsigned OptUnusedLoads (CodeSeg* S)
/* Remove loads of registers where the value loaded is not used later. */
{
    unsigned Changes = 0;

    /* Walk over the entries */
    unsigned I = 0;
    while (I < CS_GetEntryCount (S)) {

	CodeEntry* N;

      	/* Get next entry */
       	CodeEntry* E = CS_GetEntry (S, I);

	/* Check if it's a register load or transfer insn */
	if ((E->Info & (OF_LOAD | OF_XFR | OF_REG_INCDEC)) != 0  &&
	    (N = CS_GetNextEntry (S, I)) != 0                    &&
	    (N->Info & OF_FBRA) == 0) {

	    /* Check which sort of load or transfer it is */
	    unsigned R;
	    switch (E->OPC) {
		case OP65_DEA:
		case OP65_INA:
		case OP65_LDA:
	    	case OP65_TXA:
	    	case OP65_TYA: 	R = REG_A;	break;
		case OP65_DEX:
		case OP65_INX:
		case OP65_LDX:
	    	case OP65_TAX: 	R = REG_X;     	break;
		case OP65_DEY:
		case OP65_INY:
       	       	case OP65_LDY:
	    	case OP65_TAY: 	R = REG_Y;	break;
	    	default:     	goto NextEntry;	      	/* OOPS */
	    }

	    /* Get register usage and check if the register value is used later */
	    if ((GetRegInfo (S, I+1, R) & R) == 0) {

	    	/* Register value is not used, remove the load */
		CS_DelEntry (S, I);

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



unsigned OptUnusedStores (CodeSeg* S)
/* Remove stores into zero page registers that aren't used later */
{
    unsigned Changes = 0;

    /* Walk over the entries */
    unsigned I = 0;
    while (I < CS_GetEntryCount (S)) {

      	/* Get next entry */
       	CodeEntry* E = CS_GetEntry (S, I);

	/* Check if it's a register load or transfer insn */
       	if ((E->Info & OF_STORE) != 0    &&
	    E->AM == AM65_ZP             &&
	    (E->Chg & REG_ZP) != 0) {

	    /* Check for the zero page location. We know that there cannot be
	     * more than one zero page location involved in the store.
	     */
	    unsigned R = E->Chg & REG_ZP;

	    /* Get register usage and check if the register value is used later */
	    if ((GetRegInfo (S, I+1, R) & R) == 0) {

	    	/* Register value is not used, remove the load */
		CS_DelEntry (S, I);

		/* Remember, we had changes */
		++Changes;

	    }
	}

      	/* Next entry */
      	++I;

    }

    /* Return the number of changes made */
    return Changes;
}



unsigned OptDuplicateLoads (CodeSeg* S)
/* Remove loads of registers where the value loaded is already in the register. */
{
    unsigned Changes = 0;
    unsigned I;

    /* Generate register info for this step */
    CS_GenRegInfo (S);

    /* Walk over the entries */
    I = 0;
    while (I < CS_GetEntryCount (S)) {

    	CodeEntry* N;

      	/* Get next entry */
       	CodeEntry* E = CS_GetEntry (S, I);

    	/* Assume we won't delete the entry */
    	int Delete = 0;

       	/* Get a pointer to the input registers of the insn */
	const RegContents* In  = &E->RI->In;

	/* Handle the different instructions */
	switch (E->OPC) {

	    case OP65_LDA:
       	        if (In->RegA >= 0                     && /* Value of A is known */
       		    CE_KnownImm (E)                   && /* Value to be loaded is known */
       	       	    In->RegA == (long) E->Num         && /* Both are equal */
       	       	    (N = CS_GetNextEntry (S, I)) != 0 && /* There is a next entry */
		    (N->Info & OF_FBRA) == 0) {	       	 /* Which is not a cond branch */
		    Delete = 1;
		}
	        break;

	    case OP65_LDX:
       	        if (In->RegX >= 0                     && /* Value of X is known */
		    CE_KnownImm (E)                   && /* Value to be loaded is known */
		    In->RegX == (long) E->Num         && /* Both are equal */
       	       	    (N = CS_GetNextEntry (S, I)) != 0 && /* There is a next entry */
		    (N->Info & OF_FBRA) == 0) {	       	 /* Which is not a cond branch */
		    Delete = 1;
		}
	        break;

	    case OP65_LDY:
       	        if (In->RegY >= 0                     && /* Value of Y is known */
		    CE_KnownImm (E)                   && /* Value to be loaded is known */
		    In->RegY == (long) E->Num         && /* Both are equal */
       	       	    (N = CS_GetNextEntry (S, I)) != 0 && /* There is a next entry */
		    (N->Info & OF_FBRA) == 0) {	       	 /* Which is not a cond branch */
		    Delete = 1;
		}
	        break;

	    case OP65_STA:
	        /* If we store into a known zero page location, and this
		 * location does already contain the value to be stored,
		 * remove the store.
		 */
	        if (In->RegA >= 0                     && /* Value of A is known */
		    E->AM == AM65_ZP                  && /* Store into zp */
		    (((E->Chg & REG_SREG_LO) != 0 &&     /* Store into sreg */
		      In->RegA == In->SRegLo)       ||   /* Value identical */
       	       	     ((E->Chg & REG_SREG_HI) != 0 &&     /* Store into sreg+1 */
       	       	      In->RegA == In->SRegHi))) {        /* Value identical */
		    Delete = 1;
		}
	        break;

	    case OP65_STX:
	        /* If we store into a known zero page location, and this
		 * location does already contain the value to be stored,
		 * remove the store.
		 */
	        if (In->RegX >= 0                     && /* Value of A is known */
		    E->AM == AM65_ZP                  && /* Store into zp */
		    (((E->Chg & REG_SREG_LO) != 0 &&     /* Store into sreg */
		      In->RegX == In->SRegLo)       ||   /* Value identical */
       	       	     ((E->Chg & REG_SREG_HI) != 0 &&     /* Store into sreg+1 */
       	       	      In->RegX == In->SRegHi))) {        /* Value identical */
		    Delete = 1;

		/* If the value in the X register is known and the same as
		 * that in the A register, replace the store by a STA. The
		 * optimizer will then remove the load instruction for X
		 * later. STX does support the zeropage,y addressing mode,
		 * so be sure to check for that.
		 */
       	        } else if (In->RegX >= 0              &&
		    	   In->RegX == In->RegA       &&
		    	   E->AM != AM65_ABSY         &&
		    	   E->AM != AM65_ZPY) {
		    /* Use the A register instead */
       		    CE_ReplaceOPC (E, OP65_STA);
		}
	        break;

	    case OP65_STY:
	        /* If we store into a known zero page location, and this
		 * location does already contain the value to be stored,
		 * remove the store.
		 */
	        if (In->RegX >= 0                     && /* Value of A is known */
		    E->AM == AM65_ZP                  && /* Store into zp */
		    (((E->Chg & REG_SREG_LO) != 0 &&     /* Store into sreg */
		      In->RegX == In->SRegLo)       ||   /* Value identical */
       	       	     ((E->Chg & REG_SREG_HI) != 0 &&     /* Store into sreg+1 */
       	       	      In->RegX == In->SRegHi))) {        /* Value identical */
		    Delete = 1;
	        /* If the value in the Y register is known and the same as
		 * that in the A register, replace the store by a STA. The
		 * optimizer will then remove the load instruction for Y
		 * later. If replacement by A is not possible try a
		 * replacement by X, but check for invalid addressing modes
		 * in this case.
		 */
       	        } else if (In->RegY >= 0) {
		    if (In->RegY == In->RegA) {
		    	CE_ReplaceOPC (E, OP65_STA);
		    } else if (In->RegY == In->RegX   &&
			       E->AM != AM65_ABSX     &&
			       E->AM != AM65_ZPX) {
		    	CE_ReplaceOPC (E, OP65_STX);
		    }
		}
	        break;

	    case OP65_TAX:
                if (In->RegA >= 0                     &&
		    In->RegA == In->RegX              &&
		    (N = CS_GetNextEntry (S, I)) != 0 &&
		    (N->Info & OF_FBRA) == 0) {
		    /* Value is identical and not followed by a branch */
		    Delete = 1;
		}
	        break;

	    case OP65_TAY:
                if (In->RegA >= 0                 &&
		    In->RegA == In->RegY    &&
		    (N = CS_GetNextEntry (S, I)) != 0   &&
		    (N->Info & OF_FBRA) == 0) {
		    /* Value is identical and not followed by a branch */
		    Delete = 1;
		}
	        break;

       	    case OP65_TXA:
                if (In->RegX >= 0                 &&
		    In->RegX == In->RegA    &&
		    (N = CS_GetNextEntry (S, I)) != 0   &&
		    (N->Info & OF_FBRA) == 0) {
		    /* Value is identical and not followed by a branch */
		    Delete = 1;
		}
	        break;

	    case OP65_TYA:
                if (In->RegY >= 0                 &&
		    In->RegY == In->RegA    &&
		    (N = CS_GetNextEntry (S, I)) != 0   &&
		    (N->Info & OF_FBRA) == 0) {
		    /* Value is identical and not followed by a branch */
		    Delete = 1;
		}
	        break;

	    default:
	        break;

	}

	/* Delete the entry if requested */
	if (Delete) {

	    /* Register value is not used, remove the load */
	    CS_DelEntry (S, I);

	    /* Remember, we had changes */
	    ++Changes;

	} else {

	    /* Next entry */
	    ++I;

	}

    }

    /* Free register info */
    CS_FreeRegInfo (S);

    /* Return the number of changes made */
    return Changes;
}



unsigned OptStoreLoad (CodeSeg* S)
/* Remove a store followed by a load from the same location. */
{
    unsigned Changes = 0;

    /* Walk over the entries */
    unsigned I = 0;
    while (I < CS_GetEntryCount (S)) {

	CodeEntry* N;
	CodeEntry* X;

      	/* Get next entry */
       	CodeEntry* E = CS_GetEntry (S, I);

	/* Check if it is a store instruction followed by a load from the
	 * same address which is itself not followed by a conditional branch.
	 */
	if ((E->Info & OF_STORE) != 0                 &&
	    (N = CS_GetNextEntry (S, I)) != 0  	      &&
	    !CE_HasLabel (N)                          &&
       	    (N->Info & OF_LOAD) != 0                  &&
	    strcmp (E->Arg, N->Arg) == 0              &&
	    (X = CS_GetNextEntry (S, I+1)) != 0       &&
       	    (X->Info & OF_FBRA) == 0) {

	    /* Register value is not used, remove the load */
	    CS_DelEntry (S, I+1);

	    /* Remember, we had changes */
	    ++Changes;

	}

	/* Next entry */
	++I;

    }

    /* Return the number of changes made */
    return Changes;
}



unsigned OptTransfers (CodeSeg* S)
/* Remove transfers from one register to another and back */
{
    unsigned Changes = 0;

    /* Walk over the entries */
    unsigned I = 0;
    while (I < CS_GetEntryCount (S)) {

	CodeEntry* N;
	CodeEntry* X;
	CodeEntry* P;

      	/* Get next entry */
       	CodeEntry* E = CS_GetEntry (S, I);

	/* Check if it is a store instruction followed by a load from the
	 * same address which is itself not followed by a conditional branch.
	 */
       	if ((E->Info & OF_XFR) != 0                 &&
	    (N = CS_GetNextEntry (S, I)) != 0  	    &&
	    !CE_HasLabel (N)                        &&
       	    (N->Info & OF_XFR) != 0) {

	    /* Check if it's a transfer and back */
       	    if ((E->OPC == OP65_TAX && N->OPC == OP65_TXA && !RegXUsed (S, I+2)) ||
       	        (E->OPC == OP65_TAY && N->OPC == OP65_TYA && !RegYUsed (S, I+2)) ||
       	        (E->OPC == OP65_TXA && N->OPC == OP65_TAX && !RegAUsed (S, I+2)) ||
       	        (E->OPC == OP65_TYA && N->OPC == OP65_TAY && !RegAUsed (S, I+1))) {

		/* If the next insn is a conditional branch, check if the insn
		 * preceeding the first xfr will set the flags right, otherwise we
		 * may not remove the sequence.
		 */
		if ((X = CS_GetNextEntry (S, I+1)) == 0) {
		    goto NextEntry;
		}
		if ((X->Info & OF_FBRA) != 0) {
		    if (I == 0) {
			/* No preceeding entry */
			goto NextEntry;
		    }
		    P = CS_GetEntry (S, I-1);
		    if ((P->Info & OF_SETF) == 0) {
			/* Does not set the flags */
			goto NextEntry;
		    }
		}

		/* Remove both transfers */
		CS_DelEntry (S, I+1);
		CS_DelEntry (S, I);

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
/*		       	     Optimize branch types 			     */
/*****************************************************************************/



unsigned OptBranchDist (CodeSeg* S)
/* Change branches for the distance needed. */
{
    unsigned Changes = 0;
    unsigned I;

    /* Get the number of entries, bail out if we have not enough */
    unsigned Count = CS_GetEntryCount (S);

    /* Walk over the entries */
    I = 0;
    while (I < Count) {

      	/* Get next entry */
       	CodeEntry* E = CS_GetEntry (S, I);

	/* Check if it's a conditional branch to a local label. */
       	if ((E->Info & OF_CBRA) != 0) {

	    /* Is this a branch to a local symbol? */
	    if (E->JumpTo != 0) {

		/* Get the index of the branch target */
		unsigned TI = CS_GetEntryIndex (S, E->JumpTo->Owner);

		/* Determine the branch distance */
		int Distance = 0;
		if (TI >= I) {
		    /* Forward branch */
		    unsigned J = I;
		    while (J < TI) {
			CodeEntry* N = CS_GetEntry (S, J++);
		       	Distance += N->Size;
		    }
		} else {
		    /* Backward branch */
		    unsigned J = TI;
		    while (J < I) {
			CodeEntry* N = CS_GetEntry (S, J++);
			Distance += N->Size;
		    }
		}

		/* Make the branch short/long according to distance */
		if ((E->Info & OF_LBRA) == 0 && Distance > 120) {
		    /* Short branch but long distance */
		    CE_ReplaceOPC (E, MakeLongBranch (E->OPC));
		    ++Changes;
		} else if ((E->Info & OF_LBRA) != 0 && Distance < 120) {
		    /* Long branch but short distance */
		    CE_ReplaceOPC (E, MakeShortBranch (E->OPC));
		    ++Changes;
		}

	    } else if ((E->Info & OF_LBRA) == 0) {

		/* Short branch to external symbol - make it long */
		CE_ReplaceOPC (E, MakeLongBranch (E->OPC));
		++Changes;

	    }
	}

	/* Next entry */
	++I;

    }

    /* Return the number of changes made */
    return Changes;
}



