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



#include <string.h>

/* common */
#include "abend.h"
#include "print.h"

/* cc65 */
#include "asmlabel.h"
#include "codeent.h"
#include "codeinfo.h"
#include "error.h"
#include "global.h"
#include "codeopt.h"



/*****************************************************************************/
/*  	       	  	  	     Data				     */
/*****************************************************************************/



/* Counter for the number of changes in one run. The optimizer process is
 * repeated until there are no more changes.
 */
static unsigned OptChanges;

/* Defines for the conditions in a compare */
typedef enum {
    CMP_INV = -1,
    CMP_EQ,
    CMP_NE,
    CMP_GT,
    CMP_GE,
    CMP_LT,
    CMP_LE,
    CMP_UGT,
    CMP_UGE,
    CMP_ULT,
    CMP_ULE
} cmp_t;

/* Table with the compare suffixes */
static const char CmpSuffixTab [][4] = {
    "eq", "ne", "gt", "ge", "lt", "le", "ugt", "uge", "ult", "ule"
};

/* Table used to invert a condition, indexed by condition */
static const unsigned char CmpInvertTab [] = {
    CMP_NE, CMP_EQ,
    CMP_LE, CMP_LT, CMP_GE, CMP_GT,
    CMP_ULE, CMP_ULT, CMP_UGE, CMP_UGT
};

/* Table to show which compares are signed (use the N flag) */
static const char CmpSignedTab [] = {
    0, 0, 1, 1, 1, 1, 0, 0, 0, 0
};



/*****************************************************************************/
/*	     	 	       Helper functions				     */
/*****************************************************************************/



static cmp_t FindCmpCond (const char* Suffix)
/* Map a condition suffix to a code. Return the code or CMP_INV on failure */
{
    int I;

    /* Linear search */
    for (I = 0; I < sizeof (CmpSuffixTab) / sizeof (CmpSuffixTab [0]); ++I) {
       	if (strcmp (Suffix, CmpSuffixTab [I]) == 0) {
	    /* Found */
     	    return I;
     	}
    }

    /* Not found */
    return CMP_INV;
}



/*****************************************************************************/
/*		  	       Remove dead jumps		      	     */
/*****************************************************************************/



static void OptDeadJumps (CodeSeg* S)
/* Remove dead jumps (jumps to the next instruction) */
{
    CodeEntry* E;
    unsigned I;

    /* Get the number of entries, bail out if we have less than two entries */
    unsigned Count = GetCodeEntryCount (S);
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
/*			       Remove dead code			      	     */
/*****************************************************************************/



static void OptDeadCode (CodeSeg* S)
/* Remove dead code (code that follows an unconditional jump or an rts/rti
 * and has no label)
 */
{
    unsigned I;

    /* Get the number of entries, bail out if we have less than two entries */
    unsigned Count = GetCodeEntryCount (S);
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
    unsigned Count = GetCodeEntryCount (S);
    if (Count == 0) {
     	return;
    }

    /* Walk over all entries */
    I = 0;
    while (I < Count) {

	/* Get this entry */
	CodeEntry* E = GetCodeEntry (S, I);

       	/* Check if it's a branch, if it has a jump label, and if this jump
	 * label is not attached to the instruction itself.
	 */
     	if ((E->Info & OF_BRA) != 0 && E->JumpTo != 0 && E->JumpTo->Owner != E) {

     	    /* Get the label this insn is branching to */
     	    CodeLabel* OldLabel = E->JumpTo;

     	    /* Get the entry we're branching to */
     	    CodeEntry* N = OldLabel->Owner;

	    /* If the entry we're branching to is not itself a branch, it is
	     * not what we're searching for.
	     */
	    if ((N->Info & OF_BRA) == 0) {
	       	goto NextEntry;
	    }

	    /* Check if we can use the final target label. This is the case,
	     * if the target branch is an absolut branch, or if it is a
	     * conditional branch checking the same condition as the first one.
	     */
	    if ((N->Info & OF_UBRA) != 0 ||
       	       	((E->Info & OF_CBRA) != 0 &&
		 GetBranchCond (E->OPC)  == GetBranchCond (N->OPC))) {

	     	/* This is a jump cascade and we may jump to the final target.
	     	 * If we have a label, move the reference to this label. If
	     	 * we don't have a label, use the argument instead.
	     	 */
	     	if (N->JumpTo) {
	     	    /* Move the reference to the new insn */
	     	    MoveCodeLabelRef (S, E, N->JumpTo);
	     	} else {
		    /* Remove the reference to the old label */
		    RemoveCodeLabelRef (S, E);
		}

	     	/* Use the new argument */
	     	CodeEntrySetArg (E, N->Arg);

	     	/* Use the usage information from the new instruction */
	     	E->Use = N->Use;
	     	E->Chg = N->Chg;

	     	/* Remember, we had changes */
	     	++OptChanges;

	     	/* Done */
	      	continue;

	    }

	    /* Check if both are conditional branches, and the condition of
	     * the second is the inverse of that of the first. In this case,
	     * the second branch will never be taken, and we may jump directly
	     * to the instruction behind this one.
	     */
	    goto NextEntry;

	}

NextEntry:
	/* Next entry */
	++I;

    }
}



/*****************************************************************************/
/*	     	    	       Optimize jsr/rts			      	     */
/*****************************************************************************/



static void OptRTS (CodeSeg* S)
/* Optimize subroutine calls followed by an RTS. The subroutine call will get
 * replaced by a jump. Don't bother to delete the RTS if it does not have a
 * label, the dead code elimination should take care of it.
 */
{
    unsigned I;

    /* Get the number of entries, bail out if we have less than 2 entries */
    unsigned Count = GetCodeEntryCount (S);
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

	    /* Change the jsr to a jmp and use the additional info for a jump */
	    E->OPC  = OPC_JMP;
	    E->AM   = AM_BRA;
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
    CodeEntry* E1;	/* Entry 1 */
    CodeEntry* E2;	/* Entry 2 */
    CodeEntry* T1;	/* Jump target entry 1 */
    CodeEntry* T2;	/* Jump target entry 2 */
    CodeLabel* TL1;	/* Target label 1 */
    unsigned TI;	/* Target index */
    unsigned I;

    /* Get the number of entries, bail out if we have not enough */
    unsigned Count = GetCodeEntryCount (S);
    if (Count < 3) {
     	return;
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
	    ++OptChanges;

	}

NextEntry:
	/* Next entry */
	++I;

    }
}



/*****************************************************************************/
/*	    	     Remove conditional jumps never taken		     */
/*****************************************************************************/



static void OptDeadCondBranches (CodeSeg* S)
/* If an immidiate load of a register is followed by a conditional jump that
 * is never taken because the load of the register sets the flags in such a
 * manner, remove the conditional branch.
 */
{
    unsigned I;

    /* Get the number of entries, bail out if we have not enough */
    unsigned Count = GetCodeEntryCount (S);
    if (Count < 2) {
     	return;
    }

    /* Walk over the entries */
    I = 0;
    while (I < Count-1) {

      	/* Get next entry */
       	CodeEntry* E = GetCodeEntry (S, I);

	/* Check if it's a register load */
       	if ((E->Info & OF_LOAD) != 0 && E->AM == AM_IMM && (E->Flags & CEF_NUMARG) != 0) {

	    bc_t BC;

	    /* Immidiate register load, get next instruction */
	    CodeEntry* N = GetCodeEntry (S, I+1);

	    /* Check if the following insn is a conditional branch or if it
	     * has a label attached.
	     */
	    if ((N->Info & OF_CBRA) == 0 || CodeEntryHasLabel (E)) {
		/* No conditional jump or label attached, bail out */
		goto NextEntry;
	    }

	    /* Get the branch condition */
	    BC = GetBranchCond (N->OPC);

	    /* Check the argument against the branch condition */
       	    if ((BC == BC_EQ && E->Num != 0) 		||
		(BC == BC_NE && E->Num == 0)		||
		(BC == BC_PL && (E->Num & 0x80) != 0)	||
		(BC == BC_MI && (E->Num & 0x80) == 0)) {

		/* Remove the conditional branch */
		DelCodeEntry (S, I+1);
		--Count;

		/* Remember, we had changes */
		++OptChanges;

	    }
	}

NextEntry:
	/* Next entry */
	++I;

    }
}



/*****************************************************************************/
/*	       Remove calls to the bool transformer subroutines		     */
/*****************************************************************************/



static void OptBoolTransforms (CodeSeg* S)
/* Try to remove the call to boolean transformer routines where the call is
 * not really needed.
 */
{
    unsigned I;

    /* Get the number of entries, bail out if we have not enough */
    unsigned Count = GetCodeEntryCount (S);
    if (Count < 2) {
     	return;
    }

    /* Walk over the entries */
    I = 0;
    while (I < Count-1) {

      	/* Get next entry */
       	CodeEntry* E = GetCodeEntry (S, I);

	/* Check for a boolean transformer */
	if (E->OPC == OPC_JSR && strncmp (E->Arg, "bool", 4) == 0) {

	    cmp_t Cond;

	    /* Get the next entry */
	    CodeEntry* N = GetCodeEntry (S, I+1);

	    /* Check if this is a conditional branch */
	    if ((N->Info & OF_CBRA) == 0) {
		/* No conditional branch, bail out */
		goto NextEntry;
	    }

	    /* Make the boolean transformer unnecessary by changing the
	     * the conditional jump to evaluate the condition flags that
	     * are set after the compare directly. Note: jeq jumps if
	     * the condition is not met, jne jumps if the condition is met.
	     */
	    Cond = FindCmpCond (E->Arg + 4);
	    if (Cond == CMP_INV) {
		/* Unknown function */
		goto NextEntry;
	    }

     	    /* Invert the code if we jump on condition not met. */
       	    if (GetBranchCond (N->OPC) == BC_EQ) {
	     	/* Jumps if condition false, invert condition */
	     	Cond = CmpInvertTab [Cond];
  	    }

	    /* Check if we can replace the code by something better */
 	    switch (Cond) {

	    	case CMP_EQ:
		    ReplaceOPC (N, OPC_JEQ);
	    	    break;

	    	case CMP_NE:
		    ReplaceOPC (N, OPC_JNE);
	    	    break;

	       	case CMP_GT:
		    /* Not now ### */
		    goto NextEntry;

	    	case CMP_GE:
		    ReplaceOPC (N, OPC_JPL);
	    	    break;

	    	case CMP_LT:
		    ReplaceOPC (N, OPC_JMI);
	    	    break;

	    	case CMP_LE:
		    /* Not now ### */
	    	    goto NextEntry;

     	    	case CMP_UGT:
		    /* Not now ### */
		    goto NextEntry;

	    	case CMP_UGE:
       	       	    ReplaceOPC (N, OPC_JCS);
	    	    break;

	    	case CMP_ULT:
		    ReplaceOPC (N, OPC_JCC);
	    	    break;

	    	case CMP_ULE:
		    /* Not now ### */
	    	    goto NextEntry;

     	    	default:
	    	    Internal ("Unknown jump condition: %d", Cond);

	    }

	    /* Remove the call to the bool transformer */
	    DelCodeEntry (S, I);
	    --Count;

	    /* Remember, we had changes */
	    ++OptChanges;

	}

NextEntry:
	/* Next entry */
	++I;

    }
}



/*****************************************************************************/
/*     	       	      	  	     Code	   			     */
/*****************************************************************************/



/* Table with all the optimization functions */
typedef struct OptFunc OptFunc;
struct OptFunc {
    void (*Func) (CodeSeg*);	/* Optimizer function */
    const char*	Name;		/* Name of optimizer step */
    char	Disabled;	/* True if pass disabled */
};



/* Table with optimizer steps -  are called in this order */
static OptFunc OptFuncs [] = {
    /* Optimize jump cascades */
    { OptJumpCascades, 	    "OptJumpCascades",		0      	},
    /* Remove dead jumps */
    { OptDeadJumps,    	    "OptDeadJumps",		0      	},
    /* Change jsr/rts to jmp */
    { OptRTS,  	       	    "OptRTS",			0      	},
    /* Remove dead code */
    { OptDeadCode,     	    "OptDeadCode",		0      	},
    /* Optimize jump targets */
    { OptJumpTarget,   	    "OptJumpTarget",		0      	},
    /* Remove dead conditional branches */
    { OptDeadCondBranches,  "OptDeadCondBranches",	0    	},
    /* Remove calls to the bool transformer subroutines	*/
    { OptBoolTransforms,    "OptBoolTransforms",	0	},
};



static OptFunc* FindOptStep (const char* Name)
/* Find an optimizer step by name in the table and return a pointer. Print an
 * error and cann AbEnd if not found.
 */
{
    unsigned I;

    /* Run all optimization steps */
    for (I = 0; I < sizeof(OptFuncs)/sizeof(OptFuncs[0]); ++I) {
    	if (strcmp (OptFuncs[I].Name, Name) == 0) {
    	    /* Found */
    	    return OptFuncs+I;
    	}
    }

    /* Not found */
    AbEnd ("Optimization step `%s' not found", Name);
    return 0;
}



void DisableOpt (const char* Name)
/* Disable the optimization with the given name */
{
    OptFunc* F  = FindOptStep (Name);
    F->Disabled = 1;
}



void EnableOpt (const char* Name)
/* Enable the optimization with the given name */
{
    OptFunc* F  = FindOptStep (Name);
    F->Disabled = 0;
}



void RunOpt (CodeSeg* S)
/* Run the optimizer */
{
    unsigned Pass = 0;

    /* Print the name of the function we are working on */
    if (S->Func) {
     	Print (stdout, 1, "Running optimizer for function `%s'\n", S->Func->Name);
    } else {
     	Print (stdout, 1, "Running optimizer for global code segment\n");
    }

    /* Repeat all steps until there are no more changes */
    do {

       	unsigned I;

     	/* Reset the number of changes */
     	OptChanges = 0;

	/* Keep the user hapy */
	Print (stdout, 1, "  Optimizer pass %u:\n", ++Pass);

       	/* Run all optimization steps */
       	for (I = 0; I < sizeof(OptFuncs)/sizeof(OptFuncs[0]); ++I) {

	    /* Print the name of the following optimizer step */
	    Print (stdout, 1, "    %s:%*s", OptFuncs[I].Name,
	       	   (int) (30-strlen(OptFuncs[I].Name)), "");

	    /* Check if the step is disabled */
       	    if (OptFuncs[I].Disabled) {
	       	Print (stdout, 1, "Disabled\n");
	    } else {
	       	unsigned Changes = OptChanges;
	       	OptFuncs[I].Func (S);
		Changes = OptChanges - Changes;
		Print (stdout, 1, "%u Changes\n", Changes);
	    }
	}

    } while (OptChanges > 0);
}



