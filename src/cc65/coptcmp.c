/*****************************************************************************/
/*                                                                           */
/*				   coptcmp.c                                 */
/*                                                                           */
/*			       Optimize compares                             */
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
#include "error.h"
#include "coptcmp.h"



/*****************************************************************************/
/*  	       	  	  	     Data				     */
/*****************************************************************************/



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
/*		  	       Helper functions                              */
/*****************************************************************************/



static cmp_t FindCmpCond (const char* Code, unsigned CodeLen)
/* Search for a compare condition by the given code using the given length */
{
    unsigned I;

    /* Linear search */
    for (I = 0; I < sizeof (CmpSuffixTab) / sizeof (CmpSuffixTab [0]); ++I) {
	if (strncmp (Code, CmpSuffixTab [I], CodeLen) == 0) {
	    /* Found */
	    return I;
	}
    }

    /* Not found */
    return CMP_INV;
}



static cmp_t FindBoolCmpCond (const char* Name)
/* Map a condition suffix to a code. Return the code or CMP_INV on failure */
{
    /* Check for the correct subroutine name */
    if (strncmp (Name, "bool", 4) == 0) {
	/* Name is ok, search for the code in the table */
	return FindCmpCond (Name+4, strlen(Name)-4);
    } else {
	/* Not found */
	return CMP_INV;
    }
}



static cmp_t FindTosCmpCond (const char* Name)
/* Check if this is a call to one of the TOS compare functions (tosgtax).
 * Return the condition code or CMP_INV on failure.
 */
{
    unsigned Len = strlen (Name);

    /* Check for the correct subroutine name */
    if (strncmp (Name, "tos", 3) == 0 && strcmp (Name+Len-2, "ax") == 0) {
	/* Name is ok, search for the code in the table */
	return FindCmpCond (Name+3, Len-3-2);
    } else {
	/* Not found */
	return CMP_INV;
    }
}



static void ReplaceCmp (CodeSeg* S, unsigned I, cmp_t Cond)
/* Helper function for the replacement of routines that return a boolean
 * followed by a conditional jump. Instead of the boolean value, the condition
 * codes are evaluated directly.
 * I is the index of the conditional branch, the sequence is already checked
 * to be correct.
 */
{
    CodeEntry* N;
    CodeLabel* L;

    /* Get the entry */
    CodeEntry* E = CS_GetEntry (S, I);

    /* Replace the conditional branch */
    switch (Cond) {

	case CMP_EQ:
	    CE_ReplaceOPC (E, OP65_JEQ);
	    break;

	case CMP_NE:
	    CE_ReplaceOPC (E, OP65_JNE);
	    break;

	case CMP_GT:
	    /* Replace by
	     *     beq @L
	     *     jpl Target
	     * @L: ...
	     */
	    if ((N = CS_GetNextEntry (S, I)) == 0) {
	    	/* No such entry */
	    	Internal ("Invalid program flow");
	    }
	    L = CS_GenLabel (S, N);
	    N = NewCodeEntry (OP65_BEQ, AM65_BRA, L->Name, L, E->LI);
	    CS_InsertEntry (S, N, I);
	    CE_ReplaceOPC (E, OP65_JPL);
	    break;

	case CMP_GE:
	    CE_ReplaceOPC (E, OP65_JPL);
	    break;

	case CMP_LT:
	    CE_ReplaceOPC (E, OP65_JMI);
	    break;

	case CMP_LE:
	    /* Replace by
	     * 	   jmi Target
	     *     jeq Target
	     */
	    CE_ReplaceOPC (E, OP65_JMI);
	    L = E->JumpTo;
	    N = NewCodeEntry (OP65_JEQ, AM65_BRA, L->Name, L, E->LI);
	    CS_InsertEntry (S, N, I+1);
	    break;

	case CMP_UGT:
	    /* Replace by
	     *     beq @L
	     *     jcs Target
	     * @L: ...
	     */
	    if ((N = CS_GetNextEntry (S, I)) == 0) {
	       	/* No such entry */
	       	Internal ("Invalid program flow");
	    }
	    L = CS_GenLabel (S, N);
	    N = NewCodeEntry (OP65_BEQ, AM65_BRA, L->Name, L, E->LI);
	    CS_InsertEntry (S, N, I);
	    CE_ReplaceOPC (E, OP65_JCS);
	    break;

	case CMP_UGE:
	    CE_ReplaceOPC (E, OP65_JCS);
	    break;

	case CMP_ULT:
	    CE_ReplaceOPC (E, OP65_JCC);
	    break;

	case CMP_ULE:
	    /* Replace by
	     * 	   jcc Target
	     *     jeq Target
	     */
	    CE_ReplaceOPC (E, OP65_JCC);
	    L = E->JumpTo;
	    N = NewCodeEntry (OP65_JEQ, AM65_BRA, L->Name, L, E->LI);
	    CS_InsertEntry (S, N, I+1);
	    break;

	default:
	    Internal ("Unknown jump condition: %d", Cond);

    }

}



static int IsImmCmp16 (CodeSeg* S, CodeEntry** L)
/* Check if the instructions at L are an immidiate compare of a/x:
 *
 *
 */
{
    return (L[0]->OPC == OP65_CPX                              &&
	    L[0]->AM == AM65_IMM                               &&
	    (L[0]->Flags & CEF_NUMARG) != 0                    &&
	    !CE_HasLabel (L[0])                                &&
	    (L[1]->OPC == OP65_JNE || L[1]->OPC == OP65_BNE)   &&
       	    L[1]->JumpTo != 0                                  &&
	    !CE_HasLabel (L[1])                                &&
       	    L[2]->OPC == OP65_CMP                              &&
	    L[2]->AM == AM65_IMM                               &&
	    (L[2]->Flags & CEF_NUMARG) != 0                    &&
	    (L[3]->Info & OF_ZBRA) != 0                        &&
	    L[3]->JumpTo != 0                                  &&
	    (L[1]->JumpTo->Owner == L[3] || L[1]->JumpTo == L[3]->JumpTo));
}



static int GetCmpRegVal (const CodeEntry* E)
/* Return the register value for an immediate compare */
{
    switch (E->OPC) {
	case OP65_CMP: return E->RI->In.RegA;
	case OP65_CPX: return E->RI->In.RegX;
	case OP65_CPY: return E->RI->In.RegY;
	default:       Internal ("Invalid opcode in GetCmpRegVal");
	               return 0;  /* Not reached */
    }
}



static int IsCmpToZero (const CodeEntry* E)
/* Check if the given instrcuction is a compare to zero instruction */
{
    return (E->OPC == OP65_CMP            &&
	    E->AM  == AM65_IMM            &&
	    (E->Flags & CEF_NUMARG) != 0  &&
	    E->Num == 0);
}



static int IsSpLoad (const CodeEntry* E)
/* Return true if this is the load of A from the stack */
{
    return E->OPC == OP65_LDA && E->AM == AM65_ZP_INDY && strcmp (E->Arg, "sp") == 0;
}



static int IsLocalLoad16 (CodeSeg* S, unsigned Index,
	       	     	  CodeEntry** L, unsigned Count)
/* Check if a 16 bit load of a local variable follows:
 *
 *      ldy     #$xx
 *      lda     (sp),y
 *      tax
 *      dey
 *      lda     (sp),y
 *
 * If so, read Count entries following the first ldy into L and return true
 * if this is possible. Otherwise return false.
 */
{
    /* Be sure we read enough entries for the check */
    CHECK (Count >= 5);

    /* Read the first entry */
    L[0] = CS_GetEntry (S, Index);

    /* Check for the sequence */
    return (L[0]->OPC == OP65_LDY                        &&
	    CE_KnownImm (L[0])                           &&
       	    CS_GetEntries (S, L+1, Index+1, Count-1)     &&
       	    IsSpLoad (L[1])                              &&
	    !CE_HasLabel (L[1])                          &&
	    L[2]->OPC == OP65_TAX                        &&
	    !CE_HasLabel (L[2])                          &&
	    L[3]->OPC == OP65_LDY                        &&
	    CE_KnownImm (L[3])                           &&
	    L[3]->Num == L[0]->Num - 1                   &&
	    !CE_HasLabel (L[3])                          &&
	    IsSpLoad (L[4])                              &&
	    !CE_HasLabel (L[4]));
}



/*****************************************************************************/
/*  	       Remove calls to the bool transformer subroutines		     */
/*****************************************************************************/



unsigned OptBoolTrans (CodeSeg* S)
/* Try to remove the call to boolean transformer routines where the call is
 * not really needed.
 */
{
    unsigned Changes = 0;

    /* Walk over the entries */
    unsigned I = 0;
    while (I < CS_GetEntryCount (S)) {

	CodeEntry* N;
	cmp_t Cond;

      	/* Get next entry */
       	CodeEntry* E = CS_GetEntry (S, I);

	/* Check for a boolean transformer */
	if (E->OPC == OP65_JSR                           &&
	    (Cond = FindBoolCmpCond (E->Arg)) != CMP_INV &&
	    (N = CS_GetNextEntry (S, I)) != 0        &&
	    (N->Info & OF_ZBRA) != 0) {

	    /* Make the boolean transformer unnecessary by changing the
	     * the conditional jump to evaluate the condition flags that
	     * are set after the compare directly. Note: jeq jumps if
	     * the condition is not met, jne jumps if the condition is met.
     	     * Invert the code if we jump on condition not met.
	     */
       	    if (GetBranchCond (N->OPC) == BC_EQ) {
	       	/* Jumps if condition false, invert condition */
	       	Cond = CmpInvertTab [Cond];
  	    }

	    /* Check if we can replace the code by something better */
	    ReplaceCmp (S, I+1, Cond);

	    /* Remove the call to the bool transformer */
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
/*		  	  Optimizations for compares                         */
/*****************************************************************************/
				    


unsigned OptCmp1 (CodeSeg* S)
/* Search for the sequence
 *
 *  	stx	xx
 *  	stx	tmp1
 *  	ora	tmp1
 *
 * and replace it by
 *
 *  	stx	xx
 *  	ora	xx
 */
{
    unsigned Changes = 0;

    /* Walk over the entries */
    unsigned I = 0;
    while (I < CS_GetEntryCount (S)) {

	CodeEntry* L[2];

      	/* Get next entry */
       	CodeEntry* E = CS_GetEntry (S, I);

     	/* Check for the sequence */
       	if (E->OPC == OP65_STX 	  		&&
	    CS_GetEntries (S, L, I+1, 2)	&&
       	    L[0]->OPC == OP65_STX		&&
	    strcmp (L[0]->Arg, "tmp1") == 0     &&
	    !CE_HasLabel (L[0])                 &&
	    L[1]->OPC == OP65_ORA	    	&&
	    strcmp (L[1]->Arg, "tmp1") == 0     &&
	    !CE_HasLabel (L[1])) {

	    /* Remove the remaining instructions */
	    CS_DelEntries (S, I+1, 2);

	    /* Insert the ora instead */
	    CS_InsertEntry (S, NewCodeEntry (OP65_ORA, E->AM, E->Arg, 0, E->LI), I+1);

	    /* Remember, we had changes */
	    ++Changes;

	}

	/* Next entry */
	++I;

    }

    /* Return the number of changes made */
    return Changes;
}



unsigned OptCmp2 (CodeSeg* S)
/* Search for
 *
 *     	lda/and/ora/eor	...
 *  	cmp #$00
 *  	jeq/jne
 * or
 *     	lda/and/ora/eor	...
 *  	cmp #$00
 *  	jsr boolxx
 *
 * and remove the cmp.
 */
{
    unsigned Changes = 0;

    /* Walk over the entries */
    unsigned I = 0;
    while (I < CS_GetEntryCount (S)) {

	CodeEntry* L[2];

      	/* Get next entry */
       	CodeEntry* E = CS_GetEntry (S, I);

     	/* Check for the sequence */
       	if ((E->OPC == OP65_ADC ||
	     E->OPC == OP65_AND ||
	     E->OPC == OP65_DEA ||
	     E->OPC == OP65_EOR ||
	     E->OPC == OP65_INA ||
       	     E->OPC == OP65_LDA ||
	     E->OPC == OP65_ORA	||
	     E->OPC == OP65_PLA ||
	     E->OPC == OP65_SBC ||
	     E->OPC == OP65_TXA ||
	     E->OPC == OP65_TYA)                       &&
	    CS_GetEntries (S, L, I+1, 2)   	       &&
       	    IsCmpToZero (L[0])                         &&
	    !CE_HasLabel (L[0])                        &&
       	    ((L[1]->Info & OF_FBRA) != 0         ||
	     (L[1]->OPC == OP65_JSR        &&
	      FindBoolCmpCond (L[1]->Arg) != CMP_INV)) &&
	    !CE_HasLabel (L[1])) {

	    /* Remove the compare */
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



unsigned OptCmp3 (CodeSeg* S)
/* Search for
 *
 *  	lda	x
 *  	ldx	y
 *  	cpx 	#a
 *  	bne 	L1
 *  	cmp 	#b
 *     	jne/jeq	L2
 *
 * If a is zero, we may remove the compare. If a and b are both zero, we may
 * replace it by the sequence
 *
 *  	lda 	x
 *  	ora 	x+1
 *  	jne/jeq ...
 *
 * L1 may be either the label at the branch instruction, or the target label
 * of this instruction.
 */
{
    unsigned Changes = 0;

    /* Walk over the entries */
    unsigned I = 0;
    while (I < CS_GetEntryCount (S)) {

	CodeEntry* L[5];

      	/* Get next entry */
       	CodeEntry* E = CS_GetEntry (S, I);

     	/* Check for the sequence */
       	if (E->OPC == OP65_LDA               &&
	    CS_GetEntries (S, L, I+1, 5) &&
	    L[0]->OPC == OP65_LDX            &&
	    !CE_HasLabel (L[0])              &&
	    IsImmCmp16 (S, L+1)) {

	    if (L[1]->Num == 0 && L[3]->Num == 0) {
		/* The value is zero, we may use the simple code version. */
		CE_ReplaceOPC (L[0], OP65_ORA);
		CS_DelEntries (S, I+2, 3);
       	    } else {
		/* Move the lda instruction after the first branch. This will
		 * improve speed, since the load is delayed after the first
		 * test.
		 */
		CS_MoveEntry (S, I, I+4);

		/* We will replace the ldx/cpx by lda/cmp */
	    	CE_ReplaceOPC (L[0], OP65_LDA);
		CE_ReplaceOPC (L[1], OP65_CMP);

		/* Beware: If the first LDA instruction had a label, we have
	     	 * to move this label to the top of the sequence again.
		 */
		if (CE_HasLabel (E)) {
		    CS_MoveLabels (S, E, L[0]);
		}

	    }

	    ++Changes;
	}

	/* Next entry */
	++I;

    }

    /* Return the number of changes made */
    return Changes;
}



unsigned OptCmp4 (CodeSeg* S)
/* Optimize compares of local variables:
 *
 *      ldy     #o
 *      lda     (sp),y
 *      tax
 *      dey
 *      lda     (sp),y
 *      cpx     #a
 *      bne     L1
 *   	cmp 	#b
 *      jne/jeq L2
 */
{
    unsigned Changes = 0;

    /* Walk over the entries */
    unsigned I = 0;
    while (I < CS_GetEntryCount (S)) {

	CodeEntry* L[9];

     	/* Check for the sequence */
       	if (IsLocalLoad16 (S, I, L, 9) && IsImmCmp16 (S, L+5)) {

       	    if (L[5]->Num == 0 && L[7]->Num == 0) {

	    	/* The value is zero, we may use the simple code version:
	    	 *      ldy     #o
	    	 *      lda     (sp),y
	    	 *      ldy     #o-1
	    	 *      ora    	(sp),y
		 *      jne/jeq ...
		 */
		CE_ReplaceOPC (L[4], OP65_ORA);
		CS_DelEntries (S, I+5, 3);   /* cpx/bne/cmp */
		CS_DelEntry (S, I+2);        /* tax */

       	    } else {

		/* Change the code to just use the A register. Move the load
		 * of the low byte after the first branch if possible:
		 *
		 *      ldy     #o
		 *      lda     (sp),y
		 *      cmp     #a
		 *      bne     L1
		 *      ldy     #o-1
		 *      lda     (sp),y
		 *   	cmp	#b
		 *      jne/jeq ...
		 */
       	       	CS_DelEntry (S, I+2);             /* tax */
		CE_ReplaceOPC (L[5], OP65_CMP);   /* cpx -> cmp */
		CS_MoveEntry (S, I+4, I+2);       /* cmp */
		CS_MoveEntry (S, I+5, I+3);       /* bne */

	    }

	    ++Changes;
	}

    	/* Next entry */
	++I;

    }

    /* Return the number of changes made */
    return Changes;
}



unsigned OptCmp5 (CodeSeg* S)
/* Search for calls to compare subroutines followed by a conditional branch
 * and replace them by cheaper versions, since the branch means that the
 * boolean value returned by these routines is not needed (we may also check
 * that explicitly, but for the current code generator it is always true).
 */
{
    unsigned Changes = 0;

    /* Walk over the entries */
    unsigned I = 0;
    while (I < CS_GetEntryCount (S)) {

	CodeEntry* N;
	cmp_t Cond;

      	/* Get next entry */
       	CodeEntry* E = CS_GetEntry (S, I);

     	/* Check for the sequence */
       	if (E->OPC == OP65_JSR 	    		        &&
	    (Cond = FindTosCmpCond (E->Arg)) != CMP_INV	&&
	    (N = CS_GetNextEntry (S, I)) != 0           &&
	    (N->Info & OF_ZBRA) != 0                    &&
       	    !CE_HasLabel (N)) {

       	    /* The tos... functions will return a boolean value in a/x and
	     * the Z flag says if this value is zero or not. We will call
	     * a cheaper subroutine instead, one that does not return a
	     * boolean value but only valid flags. Note: jeq jumps if
	     * the condition is not met, jne jumps if the condition is met.
     	     * Invert the code if we jump on condition not met.
    	     */
       	    if (GetBranchCond (N->OPC) == BC_EQ) {
	       	/* Jumps if condition false, invert condition */
	       	Cond = CmpInvertTab [Cond];
  	    }

	    /* Replace the subroutine call. */
	    E = NewCodeEntry (OP65_JSR, AM65_ABS, "tosicmp", 0, E->LI);
	    CS_InsertEntry (S, E, I+1);
	    CS_DelEntry (S, I);

	    /* Replace the conditional branch */
	    ReplaceCmp (S, I+1, Cond);

	    /* Remember, we had changes */
	    ++Changes;

	}

  	/* Next entry */
	++I;

    }

    /* Return the number of changes made */
    return Changes;
}



unsigned OptCmp6 (CodeSeg* S)
/* Search for a sequence ldx/txa/branch and remove the txa if A is not
 * used later.
 */
{
    unsigned Changes = 0;

    /* Walk over the entries */
    unsigned I = 0;
    while (I < CS_GetEntryCount (S)) {

       	CodeEntry* L[2];

       	/* Get next entry */
       	CodeEntry* E = CS_GetEntry (S, I);

       	/* Check for the sequence */
       	if ((E->OPC == OP65_LDX)                        &&
       	    CS_GetEntries (S, L, I+1, 2)                &&
       	    L[0]->OPC == OP65_TXA                       &&
       	    !CE_HasLabel (L[0])                         &&
       	    (L[1]->Info & OF_FBRA) != 0                 &&
       	    !CE_HasLabel (L[1])                         &&
	    !RegAUsed (S, I+3)) {

	    /* Remove the txa */
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



unsigned OptCmp7 (CodeSeg* S)
/* Check for register compares where the contents of the register and therefore
 * the result of the compare is known.
 */
{
    unsigned Changes = 0;
    unsigned I;

    /* Generate register info for this step */
    CS_GenRegInfo (S);

    /* Walk over the entries */
    I = 0;
    while (I < CS_GetEntryCount (S)) {

	int RegVal;

      	/* Get next entry */
       	CodeEntry* E = CS_GetEntry (S, I);

     	/* Check for a compare against an immediate value */
       	if ((E->Info & OF_CMP) != 0           &&
	    (RegVal = GetCmpRegVal (E)) >= 0  &&
	    CE_KnownImm (E)) {

	    /* We are able to evaluate the compare at compile time. Check if
	     * one or more branches are ahead.
	     */
	    unsigned JumpsChanged = 0;
	    CodeEntry* N;
	    while ((N = CS_GetNextEntry (S, I)) != 0 &&   /* Followed by something.. */
	       	   (N->Info & OF_CBRA) != 0          &&   /* ..that is a cond branch.. */
	       	   !CE_HasLabel (N)) {                    /* ..and has no label */

	       	/* Evaluate the branch condition */
	       	int Cond;
     		switch (GetBranchCond (N->OPC)) {
     	       	    case BC_CC:
     	       	        Cond = ((unsigned char)RegVal) < ((unsigned char)E->Num);
     	       	        break;

     	       	    case BC_CS:
     	       	        Cond = ((unsigned char)RegVal) >= ((unsigned char)E->Num);
     	       	        break;

     	       	    case BC_EQ:
     	       	        Cond = ((unsigned char)RegVal) == ((unsigned char)E->Num);
     	       	        break;

     	       	    case BC_MI:
       	       	        Cond = ((signed char)RegVal) < ((signed char)E->Num);
     	       	        break;

     	       	    case BC_NE:
     	       	        Cond = ((unsigned char)RegVal) != ((unsigned char)E->Num);
	       	        break;

	       	    case BC_PL:
       	       	        Cond = ((signed char)RegVal) >= ((signed char)E->Num);
	       	        break;

	       	    case BC_VC:
	       	    case BC_VS:
		        /* Not set by the compare operation, bail out (Note:
			 * Just skipping anything here is rather stupid, but
			 * the sequence is never generated by the compiler,
			 * so it's quite safe to skip).
			 */
		        goto NextEntry;

		    default:
		        Internal ("Unknown branch condition");

		}

	       	/* If the condition is false, we may remove the jump. Otherwise
	       	 * the branch will always be taken, so we may replace it by a
	       	 * jump (and bail out).
	       	 */
		if (!Cond) {
		    CS_DelEntry (S, I+1);
		} else {
		    CodeLabel* L = N->JumpTo;
		    CodeEntry* X = NewCodeEntry (OP65_JMP, AM65_BRA, L->Name, L, N->LI);
		    CS_InsertEntry (S, X, I+2);
		    CS_DelEntry (S, I+1);
		}

	     	/* Remember, we had changes */
	    	++JumpsChanged;
	     	++Changes;
	    }

	    /* If we have made changes above, we may also remove the compare */
	    if (JumpsChanged) {
		CS_DelEntry (S, I);
	    }

	}

NextEntry:
     	/* Next entry */
	++I;

    }

    /* Free register info */
    CS_FreeRegInfo (S);

    /* Return the number of changes made */
    return Changes;
}



