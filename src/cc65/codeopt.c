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
#include "coptind.h"
#include "error.h"
#include "global.h"
#include "codeopt.h"



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
/*  	     	   	       Helper functions	    			     */
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



static int FindTosCmpCond (const char* Name)
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



static int IsUnsignedCmp (int Code)
/* Check if this is an unsigned compare */
{
    CHECK (Code >= 0);
    return CmpSignedTab [Code] == 0;
}



static const char* MakeBoolCmp (cmp_t Cond)
/* Create the name of a bool transformer subroutine for the given code. The
 * result is placed into a static buffer, so beware!
 */
{
    static char Buffer[20];
    CHECK (Cond != CMP_INV);
    sprintf (Buffer, "bool%s", CmpSuffixTab[Cond]);
    return Buffer;
}



static const char* MakeTosCmp (cmp_t Cond)
/* Create the name of a tos compare subroutine for the given code. The
 * result is placed into a static buffer, so beware!
 */
{
    static char Buffer[20];
    CHECK (Cond != CMP_INV);
    sprintf (Buffer, "tos%sax", CmpSuffixTab[Cond]);
    return Buffer;
}



static int IsBitOp (const CodeEntry* E)
/* Check if E is one of the bit operations (and, or, eor) */
{
    return (E->OPC == OPC_AND || E->OPC == OPC_ORA || E->OPC == OPC_EOR);
}



static int IsCmpToZero (const CodeEntry* E)
/* Check if the given instrcuction is a compare to zero instruction */
{
    return (E->OPC == OPC_CMP             &&
	    E->AM  == AM_IMM              &&
	    (E->Flags & CEF_NUMARG) != 0  &&
	    E->Num == 0);
}



static int IsSpLoad (const CodeEntry* E)
/* Return true if this is the load of A from the stack */
{
    return E->OPC == OPC_LDA && E->AM == AM_ZP_INDY && strcmp (E->Arg, "sp") == 0;
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
    L[0] = GetCodeEntry (S, Index);

    /* Check for the sequence */
    return (L[0]->OPC == OPC_LDY                      &&
	    L[0]->AM == AM_IMM                        &&
	    (L[0]->Flags & CEF_NUMARG) != 0           &&
       	    GetCodeEntries (S, L+1, Index+1, Count-1) &&
       	    IsSpLoad (L[1])                           &&
	    !CodeEntryHasLabel (L[1])                 &&
	    L[2]->OPC == OPC_TAX                      &&
	    !CodeEntryHasLabel (L[2])                 &&
	    L[3]->OPC == OPC_DEY                      &&
	    !CodeEntryHasLabel (L[3])                 &&
	    IsSpLoad (L[4])                           &&
	    !CodeEntryHasLabel (L[4]));		      
}



static int IsImmCmp16 (CodeSeg* S, CodeEntry** L)
/* Check if the instructions at L are an immidiate compare of a/x:
 *
 *
 */
{
    return (L[0]->OPC == OPC_CPX                                             &&
	    L[0]->AM == AM_IMM                                               &&
	    (L[0]->Flags & CEF_NUMARG) != 0                                  &&
	    !CodeEntryHasLabel (L[0])                                        &&
	    (L[1]->OPC == OPC_JNE || L[1]->OPC == OPC_BNE)                   &&
       	    L[1]->JumpTo != 0                                                &&
	    !CodeEntryHasLabel (L[1])                                        &&
       	    L[2]->OPC == OPC_CMP                                             &&
	    L[2]->AM == AM_IMM                                               &&
	    (L[2]->Flags & CEF_NUMARG) != 0                                  &&
	    (L[3]->Info & OF_ZBRA) != 0                                      &&
	    L[3]->JumpTo != 0                                                &&
	    (L[1]->JumpTo->Owner == L[3] || L[1]->JumpTo == L[3]->JumpTo));
}



/*****************************************************************************/
/*  	       Remove calls to the bool transformer subroutines		     */
/*****************************************************************************/



static unsigned OptBoolTransforms (CodeSeg* S)
/* Try to remove the call to boolean transformer routines where the call is
 * not really needed.
 */
{
    unsigned Changes = 0;

    /* Walk over the entries */
    unsigned I = 0;
    while (I < GetCodeEntryCount (S)) {

	CodeEntry* N;
	cmp_t Cond;

      	/* Get next entry */
       	CodeEntry* E = GetCodeEntry (S, I);

	/* Check for a boolean transformer */
	if (E->OPC == OPC_JSR                            &&
	    (Cond = FindBoolCmpCond (E->Arg)) != CMP_INV &&
	    (N = GetNextCodeEntry (S, I)) != 0           &&
	    (N->Info & OF_ZBRA) != 0) {

	    CodeEntry* X;
	    CodeLabel* L;

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
 	    switch (Cond) {

	       	case CMP_EQ:
	       	    ReplaceOPC (N, OPC_JEQ);
	      	    break;

	      	case CMP_NE:
	      	    ReplaceOPC (N, OPC_JNE);
	      	    break;

	       	case CMP_GT:
		    /* Replace by
		     *     beq @L
		     *     jpl Target
		     * @L: ...
		     */
		    if ((X = GetNextCodeEntry (S, I+1)) == 0) {
		    	/* No such entry */
		    	goto NextEntry;
		    }
		    L = GenCodeLabel (S, X);
		    X = NewCodeEntry (OPC_BEQ, AM_BRA, L->Name, L);
		    InsertCodeEntry (S, X, I+1);
       	       	    ReplaceOPC (N, OPC_JPL);
		    break;

	      	case CMP_GE:
	      	    ReplaceOPC (N, OPC_JPL);
	      	    break;

	      	case CMP_LT:
	      	    ReplaceOPC (N, OPC_JMI);
	      	    break;

	      	case CMP_LE:
		    /* Replace by
		     *	   jmi Target
		     *     jeq Target
		     */
		    ReplaceOPC (N, OPC_JMI);
		    L = N->JumpTo;
       	       	    X = NewCodeEntry (OPC_JEQ, AM_BRA, L->Name, L);
		    InsertCodeEntry (S, X, I+2);
		    break;

     	      	case CMP_UGT:
		    /* Replace by
		     *     beq @L
		     *     jcs Target
		     * @L: ...
		     */
		    if ((X = GetNextCodeEntry (S, I+1)) == 0) {
		    	/* No such entry */
		    	goto NextEntry;
		    }
		    L = GenCodeLabel (S, X);
		    X = NewCodeEntry (OPC_BEQ, AM_BRA, L->Name, L);
		    InsertCodeEntry (S, X, I+1);
       	       	    ReplaceOPC (N, OPC_JCS);
		    break;

	     	case CMP_UGE:
       	       	    ReplaceOPC (N, OPC_JCS);
	     	    break;

	     	case CMP_ULT:
	     	    ReplaceOPC (N, OPC_JCC);
	     	    break;

	     	case CMP_ULE:
		    /* Replace by
		     *	   jcc Target
		     *     jeq Target
		     */
		    ReplaceOPC (N, OPC_JCC);
		    L = N->JumpTo;
       	       	    X = NewCodeEntry (OPC_JEQ, AM_BRA, L->Name, L);
		    InsertCodeEntry (S, X, I+2);
		    break;

     	    	default:
	    	    Internal ("Unknown jump condition: %d", Cond);

	    }

	    /* Remove the call to the bool transformer */
	    DelCodeEntry (S, I);

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
/*		  	  Optimizations for compares                         */
/*****************************************************************************/



static unsigned OptCmp1 (CodeSeg* S)
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
    while (I < GetCodeEntryCount (S)) {

	CodeEntry* L[2];

      	/* Get next entry */
       	CodeEntry* E = GetCodeEntry (S, I);

     	/* Check for the sequence */
       	if (E->OPC == OPC_STX 	  		&&
	    GetCodeEntries (S, L, I+1, 2)	&&
       	    L[0]->OPC == OPC_STX		&&
	    strcmp (L[0]->Arg, "tmp1") == 0     &&
	    !CodeEntryHasLabel (L[0])           &&
	    L[1]->OPC == OPC_ORA	    	&&
	    strcmp (L[1]->Arg, "tmp1") == 0     &&
	    !CodeEntryHasLabel (L[1])) {

	    /* Remove the remaining instructions */
	    DelCodeEntries (S, I+1, 2);

	    /* Insert the ora instead */
	    InsertCodeEntry (S, NewCodeEntry (OPC_ORA, E->AM, E->Arg, 0), I+1);

	    /* Remember, we had changes */
	    ++Changes;

	}

	/* Next entry */
	++I;

    }

    /* Return the number of changes made */
    return Changes;
}



static unsigned OptCmp2 (CodeSeg* S)
/* Search for
 *
 *     	lda/and/ora/eor	...
 *  	cmp #$00
 *  	jeq/jne
 *
 * and remove the cmp.
 */
{
    unsigned Changes = 0;

    /* Walk over the entries */
    unsigned I = 0;
    while (I < GetCodeEntryCount (S)) {

	CodeEntry* L[2];

      	/* Get next entry */
       	CodeEntry* E = GetCodeEntry (S, I);

     	/* Check for the sequence */
       	if ((E->OPC == OPC_LDA || IsBitOp (E))	  &&
	    GetCodeEntries (S, L, I+1, 2)      	  &&
       	    IsCmpToZero (L[0])                    &&
	    !CodeEntryHasLabel (L[0])             &&
       	    (L[1]->Info & OF_FBRA) != 0           &&
	    !CodeEntryHasLabel (L[1])) {

	    /* Remove the compare */
	    DelCodeEntry (S, I+1);

	    /* Remember, we had changes */
	    ++Changes;

	}

	/* Next entry */
	++I;

    }

    /* Return the number of changes made */
    return Changes;
}



static unsigned OptCmp3 (CodeSeg* S)
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
    while (I < GetCodeEntryCount (S)) {

	CodeEntry* L[5];

      	/* Get next entry */
       	CodeEntry* E = GetCodeEntry (S, I);

     	/* Check for the sequence */
       	if (E->OPC == OPC_LDA                                                &&
	    GetCodeEntries (S, L, I+1, 5)      	                             &&
	    L[0]->OPC == OPC_LDX                                             &&
	    !CodeEntryHasLabel (L[0])                                        &&
	    IsImmCmp16 (S, L+1)) {

	    if (L[1]->Num == 0 && L[3]->Num == 0) {
		/* The value is zero, we may use the simple code version. */
		ReplaceOPC (L[0], OPC_ORA);
		DelCodeEntries (S, I+2, 3);
       	    } else {
		/* Move the lda instruction after the first branch. This will
		 * improve speed, since the load is delayed after the first
		 * test.
		 */
		MoveCodeEntry (S, I, I+4);

		/* We will replace the ldx/cpx by lda/cmp */
		ReplaceOPC (L[0], OPC_LDA);
		ReplaceOPC (L[1], OPC_CMP);

	    }

	    ++Changes;
	}

	/* Next entry */
	++I;

    }

    /* Return the number of changes made */
    return Changes;
}



static unsigned OptCmp4 (CodeSeg* S)
/* Optimize compares of local variables:
 *
 *      ldy     #o
 *      lda     (sp),y
 *      tax
 *      dey
 *      lda     (sp),y
 *      cpx     #a
 *      bne     L1
 *   	cmp	#b
 *      jne/jeq L2
 */
{
    unsigned Changes = 0;

    /* Walk over the entries */
    unsigned I = 0;
    while (I < GetCodeEntryCount (S)) {

	CodeEntry* L[9];

     	/* Check for the sequence */
       	if (IsLocalLoad16 (S, I, L, 9) && IsImmCmp16 (S, L+5)) {

       	    if (L[5]->Num == 0 && L[7]->Num == 0) {

		/* The value is zero, we may use the simple code version:
		 *      ldy     #o
		 *      lda     (sp),y
		 *      dey
		 *      ora    	(sp),y
		 *      jne/jeq ...
		 */
		ReplaceOPC (L[4], OPC_ORA);
		DelCodeEntries (S, I+5, 3);   /* cpx/bne/cmp */
		DelCodeEntry (S, I+2);        /* tax */

       	    } else {

		/* Change the code to just use the A register. Move the load
		 * of the low byte after the first branch if possible:
		 *
		 *      ldy     #o
		 *      lda     (sp),y
		 *      cmp     #a
		 *      bne     L1
		 *      dey
		 *      lda     (sp),y
		 *   	cmp	#b
		 *      jne/jeq ...
		 */
       	       	DelCodeEntry (S, I+2);         /* tax */
		ReplaceOPC (L[5], OPC_CMP);    /* cpx -> cmp */
		MoveCodeEntry (S, I+4, I+2);   /* cmp */
		MoveCodeEntry (S, I+5, I+3);   /* bne */

	    }

	    ++Changes;
	}

	/* Next entry */
	++I;

    }

    /* Return the number of changes made */
    return Changes;
}



/*****************************************************************************/
/*		  	      nega optimizations			     */
/*****************************************************************************/



static unsigned OptNegA1 (CodeSeg* S)
/* Check for
 *
 *	ldx	#$00
 *	lda	..
 * 	jsr	bnega
 *
 * Remove the ldx if the lda does not use it.
 */
{
    unsigned Changes = 0;

    /* Walk over the entries */
    unsigned I = 0;
    while (I < GetCodeEntryCount (S)) {

	CodeEntry* L[2];

      	/* Get next entry */
       	CodeEntry* E = GetCodeEntry (S, I);

     	/* Check for a ldx */
       	if (E->OPC == OPC_LDX 			&&
	    E->AM == AM_IMM	    		&&
	    (E->Flags & CEF_NUMARG) != 0	&&
	    E->Num == 0	   			&&
	    GetCodeEntries (S, L, I+1, 2)	&&
	    L[0]->OPC == OPC_LDA		&&
	    (L[0]->Use & REG_X) == 0	    	&&
	    L[1]->OPC == OPC_JSR	    	&&
	    strcmp (L[1]->Arg, "bnega") == 0) {

	    /* Remove the ldx instruction */
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



static unsigned OptNegA2 (CodeSeg* S)
/* Check for
 *
 *	lda	..
 * 	jsr	bnega
 *	jeq/jne	..
 *
 * Adjust the conditional branch and remove the call to the subroutine.
 */
{
    unsigned Changes = 0;

    /* Walk over the entries */
    unsigned I = 0;
    while (I < GetCodeEntryCount (S)) {

	CodeEntry* L[2];

      	/* Get next entry */
       	CodeEntry* E = GetCodeEntry (S, I);

     	/* Check for the sequence */
       	if (E->OPC == OPC_LDA  	  	    	&&
	    GetCodeEntries (S, L, I+1, 2)	&&
       	    L[0]->OPC == OPC_JSR  	    	&&
	    strcmp (L[0]->Arg, "bnega") == 0	&&
	    !CodeEntryHasLabel (L[0])		&&
	    (L[1]->Info & OF_ZBRA) != 0) {

	    /* Invert the branch */
	    ReplaceOPC (L[1], GetInverseBranch (L[1]->OPC));

	    /* Delete the subroutine call */
	    DelCodeEntry (S, I+1);

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
/*		   	      negax optimizations			     */
/*****************************************************************************/



static unsigned OptNegAX1 (CodeSeg* S)
/* Search for the sequence:
 *
 *  	lda	(xx),y
 *  	tax
 *  	dey
 *  	lda	(xx),y
 *  	jsr	bnegax
 *  	jne/jeq	...
 *
 * and replace it by
 *
 *  	lda    	(xx),y
 *  	dey
 *  	ora    	(xx),y
 *	jeq/jne	...
 */
{
    unsigned Changes = 0;

    /* Walk over the entries */
    unsigned I = 0;
    while (I < GetCodeEntryCount (S)) {

	CodeEntry* L[5];

      	/* Get next entry */
       	CodeEntry* E = GetCodeEntry (S, I);

     	/* Check for the sequence */
       	if (E->OPC == OPC_LDA  	      		&&
	    E->AM == AM_ZP_INDY	      		&&
	    GetCodeEntries (S, L, I+1, 5)	&&
	    L[0]->OPC == OPC_TAX    		&&
	    L[1]->OPC == OPC_DEY    		&&
	    L[2]->OPC == OPC_LDA    		&&
	    L[2]->AM == AM_ZP_INDY  		&&
	    strcmp (L[2]->Arg, E->Arg) == 0	&&
	    !CodeEntryHasLabel (L[2])		&&
	    L[3]->OPC == OPC_JSR    		&&
	    strcmp (L[3]->Arg, "bnegax") == 0	&&
	    !CodeEntryHasLabel (L[3])		&&
       	    (L[4]->Info & OF_ZBRA) != 0) {

	    /* lda --> ora */
	    ReplaceOPC (L[2], OPC_ORA);

	    /* Invert the branch */
	    ReplaceOPC (L[4], GetInverseBranch (L[4]->OPC));

	    /* Delete the entries no longer needed. Beware: Deleting entries
	     * will change the indices.
	     */
       	    DelCodeEntry (S, I+4);	    	/* jsr bnegax */
	    DelCodeEntry (S, I+1);	    	/* tax */

	    /* Remember, we had changes */
	    ++Changes;

	}

	/* Next entry */
	++I;

    }

    /* Return the number of changes made */
    return Changes;
}



static unsigned OptNegAX2 (CodeSeg* S)
/* Search for the sequence:
 *
 *  	lda	xx
 *  	ldx	yy
 *  	jsr	bnegax
 *  	jne/jeq	...
 *
 * and replace it by
 *
 *  	lda    	xx
 *	ora	xx+1
 *	jeq/jne	...
 */
{
    unsigned Changes = 0;

    /* Walk over the entries */
    unsigned I = 0;
    while (I < GetCodeEntryCount (S)) {

	CodeEntry* L[3];

      	/* Get next entry */
       	CodeEntry* E = GetCodeEntry (S, I);

     	/* Check for the sequence */
       	if (E->OPC == OPC_LDA  	      		&&
       	    GetCodeEntries (S, L, I+1, 3)	&&
	    L[0]->OPC == OPC_LDX       	       	&&
	    !CodeEntryHasLabel (L[0]) 		&&
       	    L[1]->OPC == OPC_JSR      		&&
	    strcmp (L[1]->Arg, "bnegax") == 0	&&
	    !CodeEntryHasLabel (L[1]) 		&&
       	    (L[2]->Info & OF_ZBRA) != 0) {

	    /* ldx --> ora */
	    ReplaceOPC (L[0], OPC_ORA);

	    /* Invert the branch */
       	    ReplaceOPC (L[2], GetInverseBranch (L[2]->OPC));

	    /* Delete the subroutine call */
       	    DelCodeEntry (S, I+2);

	    /* Remember, we had changes */
	    ++Changes;

	}

	/* Next entry */
	++I;

    }

    /* Return the number of changes made */
    return Changes;
}



static unsigned OptNegAX3 (CodeSeg* S)
/* Search for the sequence:
 *
 *  	jsr   	_xxx
 *  	jsr   	bnega(x)
 *  	jeq/jne	...
 *
 * and replace it by:
 *
 *      jsr	_xxx
 *  	<boolean test>
 *  	jne/jeq	...
 */
{
    unsigned Changes = 0;

    /* Walk over the entries */
    unsigned I = 0;
    while (I < GetCodeEntryCount (S)) {

	CodeEntry* L[2];

      	/* Get next entry */
       	CodeEntry* E = GetCodeEntry (S, I);

     	/* Check for the sequence */
       	if (E->OPC == OPC_JSR  	      		&&
	    E->Arg[0] == '_'	      		&&
       	    GetCodeEntries (S, L, I+1, 2)	&&
       	    L[0]->OPC == OPC_JSR       	       	&&
	    strncmp (L[0]->Arg,"bnega",5) == 0	&&
	    !CodeEntryHasLabel (L[0]) 		&&
       	    (L[1]->Info & OF_ZBRA) != 0) {

	    /* Check if we're calling bnega or bnegax */
	    int ByteSized = (strcmp (L[0]->Arg, "bnega") == 0);

	    /* Delete the subroutine call */
	    DelCodeEntry (S, I+1);

	    /* Insert apropriate test code */
	    if (ByteSized) {
		/* Test bytes */
		InsertCodeEntry (S, NewCodeEntry (OPC_TAX, AM_IMP, 0, 0), I+1);
	    } else {
		/* Test words */
		InsertCodeEntry (S, NewCodeEntry (OPC_STX, AM_ZP, "tmp1", 0), I+1);
		InsertCodeEntry (S, NewCodeEntry (OPC_ORA, AM_ZP, "tmp1", 0), I+2);
	    }

	    /* Invert the branch */
       	    ReplaceOPC (L[1], GetInverseBranch (L[1]->OPC));

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
/*     	       	      	  	     Code	   			     */
/*****************************************************************************/



/* Table with all the optimization functions */
typedef struct OptFunc OptFunc;
struct OptFunc {
    unsigned (*Func) (CodeSeg*);/* Optimizer function */
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
    /* Optimize conditional branches */
    { OptCondBranches, 	    "OptCondBranches", 		0    	},
    /* Replace jumps to RTS by RTS */
    { OptRTSJumps,     	    "OptRTSJumps",		0      	},
    /* Remove calls to the bool transformer subroutines	*/
    { OptBoolTransforms,    "OptBoolTransforms",	0	},
    /* Optimize calls to nega */
    { OptNegA1,	       	    "OptNegA1",			0	},
    /* Optimize calls to nega */
    { OptNegA2,	       	    "OptNegA2",			0	},
    /* Optimize calls to negax */
    { OptNegAX1,       	    "OptNegAX1",		0	},
    /* Optimize calls to negax */
    { OptNegAX2,       	    "OptNegAX2",       	       	0      	},
    /* Optimize calls to negax */
    { OptNegAX3,       	    "OptNegAX3",       	       	0      	},
    /* Optimize compares */
    { OptCmp1,              "OptCmp1",                  0       },
    /* Optimize compares */
    { OptCmp2,              "OptCmp2",                  0       },
    /* Optimize compares */
    { OptCmp3,              "OptCmp3",                  0       },
    /* Optimize compares */
    { OptCmp4,              "OptCmp4",                  0       },
    /* Remove unused loads */
    { OptUnusedLoads,	    "OptUnusedLoads",		0	},
    /* Optimize branch distance */
    { OptBranchDist,   	    "OptBranchDist",		0	},
};



static OptFunc* FindOptStep (const char* Name)
/* Find an optimizer step by name in the table and return a pointer. Print an
 * error and call AbEnd if not found.
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
    unsigned I;
    unsigned Pass = 0;
    unsigned OptChanges;

    /* If we shouldn't run the optimizer, bail out */
    if (!Optimize) {
	return;
    }

    /* Print the name of the function we are working on */
    if (S->Func) {
     	Print (stdout, 1, "Running optimizer for function `%s'\n", S->Func->Name);
    } else {
     	Print (stdout, 1, "Running optimizer for global code segment\n");
    }

    /* Repeat all steps until there are no more changes */
    do {
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
	       	unsigned Changes = OptFuncs[I].Func (S);
		OptChanges += Changes;
		Print (stdout, 1, "%u Changes\n", Changes);
	    }
	}

    } while (OptChanges > 0);
}



