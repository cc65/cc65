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
#include "chartype.h"
#include "print.h"
#include "xmalloc.h"
#include "xsprintf.h"

/* cc65 */
#include "asmlabel.h"
#include "codeent.h"
#include "codeinfo.h"
#include "coptind.h"
#include "coptstop.h"
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
	    L[0]->AM == AM65_IMM                         &&
	    (L[0]->Flags & CEF_NUMARG) != 0              &&
       	    CS_GetEntries (S, L+1, Index+1, Count-1)     &&
       	    IsSpLoad (L[1])                              &&
	    !CE_HasLabel (L[1])                          &&
	    L[2]->OPC == OP65_TAX                        &&
	    !CE_HasLabel (L[2])                          &&
	    L[3]->OPC == OP65_DEY                        &&
	    !CE_HasLabel (L[3])                          &&
	    IsSpLoad (L[4])                              &&
	    !CE_HasLabel (L[4]));
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
/*			     Optimize subtractions                           */
/*****************************************************************************/



static unsigned OptSub1 (CodeSeg* S)
/* Search for the sequence
 *
 *  	sbc     ...
 *      bcs     L
 *  	dex
 * L:
 *
 * and remove the handling of the high byte if X is not used later.
 */
{
    unsigned Changes = 0;

    /* Walk over the entries */
    unsigned I = 0;
    while (I < CS_GetEntryCount (S)) {

	CodeEntry* L[3];

      	/* Get next entry */
       	CodeEntry* E = CS_GetEntry (S, I);

     	/* Check for the sequence */
       	if (E->OPC == OP65_SBC 	  		             &&
	    CS_GetEntries (S, L, I+1, 3) 	             &&
       	    (L[0]->OPC == OP65_BCS || L[0]->OPC == OP65_JCS) &&
	    L[0]->JumpTo != 0                                &&
	    !CE_HasLabel (L[0])                              &&
	    L[1]->OPC == OP65_DEX       	       	     &&
	    !CE_HasLabel (L[1])                              &&
	    L[0]->JumpTo->Owner == L[2]                      &&
	    !RegXUsed (S, I+3)) {

	    /* Remove the bcs/dex */
	    CS_DelEntries (S, I+1, 2);

	    /* Remember, we had changes */
	    ++Changes;

	}

	/* Next entry */
	++I;

    }

    /* Return the number of changes made */
    return Changes;
}



static unsigned OptSub2 (CodeSeg* S)
/* Search for the sequence
 *
 *  	lda     xx
 *      sec
 *  	sta     tmp1
 *      lda     yy
 *      sbc     tmp1
 *      sta     yy
 *
 * and replace it by
 *
 *      sec
 *      lda     yy
 *     	sbc     xx
 *      sta     yy
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
       	if (E->OPC == OP65_LDA 	      		           &&
	    CS_GetEntries (S, L, I+1, 5) 	           &&
       	    L[0]->OPC == OP65_SEC                          &&
	    !CE_HasLabel (L[0])                            &&
       	    L[1]->OPC == OP65_STA       	       	   &&
	    strcmp (L[1]->Arg, "tmp1") == 0                &&
	    !CE_HasLabel (L[1])                            &&
	    L[2]->OPC == OP65_LDA                          &&
       	    !CE_HasLabel (L[2])                            &&
	    L[3]->OPC == OP65_SBC                          &&
	    strcmp (L[3]->Arg, "tmp1") == 0                &&
       	    !CE_HasLabel (L[3])                            &&
	    L[4]->OPC == OP65_STA                          &&
	    strcmp (L[4]->Arg, L[2]->Arg) == 0             &&
       	    !CE_HasLabel (L[4])) {

	    /* Remove the store to tmp1 */
	    CS_DelEntry (S, I+2);

	    /* Remove the subtraction */
	    CS_DelEntry (S, I+3);

	    /* Move the lda to the position of the subtraction and change the
	     * op to SBC.
	     */
	    CS_MoveEntry (S, I, I+3);
	    CE_ReplaceOPC (E, OP65_SBC);

	    /* If the sequence head had a label, move this label back to the
	     * head.
	     */
	    if (CE_HasLabel (E)) {
		CS_MoveLabels (S, E, L[0]);
  	    }

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
/*			      Optimize additions                             */
/*****************************************************************************/



static unsigned OptAdd1 (CodeSeg* S)
/* Search for the sequence
 *
 *     	jsr     pushax
 *      ldy     xxx
 *  	ldx     #$00
 *      lda     (sp),y
 *      jsr     tosaddax
 *
 * and replace it by:
 *
 *      ldy     xxx-2
 *      clc
 *      adc     (sp),y
 *      bcc     L
 *      inx
 * L:
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
	if (E->OPC == OP65_JSR                               &&
	    strcmp (E->Arg, "pushax") == 0                   &&
       	    CS_GetEntries (S, L, I+1, 5)   	             &&
       	    L[0]->OPC == OP65_LDY                            &&
	    CE_KnownImm (L[0])                               &&
	    !CE_HasLabel (L[0])                              &&
	    L[1]->OPC == OP65_LDX                            &&
	    CE_KnownImm (L[1])                               &&
	    L[1]->Num == 0                                   &&
	    !CE_HasLabel (L[1])                              &&
	    L[2]->OPC == OP65_LDA                            &&
	    !CE_HasLabel (L[2])                              &&
	    L[3]->OPC == OP65_JSR                            &&
	    strcmp (L[3]->Arg, "tosaddax") == 0              &&
	    !CE_HasLabel (L[3])) {

	    CodeEntry* X;
	    CodeLabel* Label;

	    /* Remove the call to pushax */
	    CS_DelEntry (S, I);

	    /* Correct the stack offset (needed since pushax was removed) */
	    CE_SetNumArg (L[0], L[0]->Num - 2);

	    /* Add the clc . */
	    X = NewCodeEntry (OP65_CLC, AM65_IMP, 0, 0, L[3]->LI);
	    CS_InsertEntry (S, X, I+1);

	    /* Remove the load */
	    CS_DelEntry (S, I+3);      /* lda */
	    CS_DelEntry (S, I+2);      /* ldx */

	    /* Add the adc */
	    X = NewCodeEntry (OP65_ADC, AM65_ZP_INDY, "sp", 0, L[3]->LI);
	    CS_InsertEntry (S, X, I+2);

	    /* Generate the branch label and the branch */
	    Label = CS_GenLabel (S, L[4]);
	    X = NewCodeEntry (OP65_BCC, AM65_BRA, Label->Name, Label, L[3]->LI);
	    CS_InsertEntry (S, X, I+3);

	    /* Generate the increment of the high byte */
	    X = NewCodeEntry (OP65_INX, AM65_IMP, 0, 0, L[3]->LI);
	    CS_InsertEntry (S, X, I+4);

	    /* Delete the call to tosaddax */
	    CS_DelEntry (S, I+5);

	    /* Remember, we had changes */
	    ++Changes;

	}

	/* Next entry */
	++I;

    }

    /* Return the number of changes made */
    return Changes;
}



static unsigned OptAdd2 (CodeSeg* S)
/* Search for the sequence
 *
 *     	ldy     #xx
 *      lda     (sp),y
 *  	tax
 *      dey
 *      lda     (sp),y
 *      ldy     #$yy
 *      jsr     addeqysp
 *
 * and replace it by:
 *
 *      ldy     #xx-1
 *      lda     (sp),y
 *      ldy     #yy
 *      clc
 *      adc     (sp),y
 *      sta     (sp),y
 *      ldy     #xx
 *      lda     (sp),y
 *      ldy     #yy+1
 *      adc     (sp),y
 *      sta     (sp),y
 *
 * provided that a/x is not used later.
 */
{
    unsigned Changes = 0;

    /* Walk over the entries */
    unsigned I = 0;
    while (I < CS_GetEntryCount (S)) {

     	CodeEntry* L[7];

      	/* Get next entry */
       	L[0] = CS_GetEntry (S, I);

     	/* Check for the sequence */
	if (L[0]->OPC == OP65_LDY               &&
	    CE_KnownImm (L[0])                  &&
       	    CS_GetEntries (S, L+1, I+1, 6)   	&&
	    L[1]->OPC == OP65_LDA               &&
	    L[1]->AM == AM65_ZP_INDY            &&
	    !CE_HasLabel (L[1])                 &&
	    L[2]->OPC == OP65_TAX               &&
	    !CE_HasLabel (L[2])                 &&
	    L[3]->OPC == OP65_DEY               &&
       	    !CE_HasLabel (L[3])                 &&
	    L[4]->OPC == OP65_LDA               &&
	    L[4]->AM == AM65_ZP_INDY            &&
	    !CE_HasLabel (L[4])                 &&
	    L[5]->OPC == OP65_LDY               &&
	    CE_KnownImm (L[5])                  &&
	    !CE_HasLabel (L[5])                 &&
	    L[6]->OPC == OP65_JSR               &&
       	    strcmp (L[6]->Arg, "addeqysp") == 0 &&
	    !CE_HasLabel (L[6])                 &&
	    (GetRegInfo (S, I+7, REG_AX) & REG_AX) == 0) {

	    char Buf [20];
	    CodeEntry* X;
	    int Offs;


	    /* Create a replacement for the first LDY */
	    Offs = (int) (L[0]->Num - 1);
	    xsprintf (Buf, sizeof (Buf), "$%02X", Offs);
	    X = NewCodeEntry (OP65_LDY, AM65_IMM, Buf, 0, L[0]->LI);
	    CS_InsertEntry (S, X, I+1);
	    CS_DelEntry (S, I);

	    /* Load Y with the low offset of the target variable */
	    X = NewCodeEntry (OP65_LDY, AM65_IMM, L[5]->Arg, 0, L[1]->LI);
	    CS_InsertEntry (S, X, I+2);

	    /* Add the CLC */
	    X = NewCodeEntry (OP65_CLC, AM65_IMP, 0, 0, L[1]->LI);
	    CS_InsertEntry (S, X, I+3);

	    /* Remove the TAX/DEY sequence */
	    CS_DelEntry (S, I+5);      /* dey */
	    CS_DelEntry (S, I+4);      /* tax */

	    /* Addition of the low byte */
	    X = NewCodeEntry (OP65_ADC, AM65_ZP_INDY, "sp", 0, L[4]->LI);
	    CS_InsertEntry (S, X, I+4);
	    X = NewCodeEntry (OP65_STA, AM65_ZP_INDY, "sp", 0, L[4]->LI);
	    CS_InsertEntry (S, X, I+5);

	    /* LDY */
	    xsprintf (Buf, sizeof (Buf), "$%02X", (Offs+1));
	    X = NewCodeEntry (OP65_LDY, AM65_IMM, Buf, 0, L[4]->LI);
	    CS_InsertEntry (S, X, I+6);

	    /* Addition of the high byte */
	    xsprintf (Buf, sizeof (Buf), "$%02X", (int)(L[5]->Num+1));
	    X = NewCodeEntry (OP65_LDY, AM65_IMM, Buf, 0, L[5]->LI);
	    CS_InsertEntry (S, X, I+8);
	    X = NewCodeEntry (OP65_ADC, AM65_ZP_INDY, "sp", 0, L[6]->LI);
	    CS_InsertEntry (S, X, I+9);
	    X = NewCodeEntry (OP65_STA, AM65_ZP_INDY, "sp", 0, L[6]->LI);
	    CS_InsertEntry (S, X, I+10);

	    /* Delete the remaining stuff */
	    CS_DelEntry (S, I+12);
	    CS_DelEntry (S, I+11);

	    /* Remember, we had changes */
	    ++Changes;

	}

	/* Next entry */
	++I;

    }

    /* Return the number of changes made */
    return Changes;
}



static unsigned OptAdd3 (CodeSeg* S)
/* Search for the sequence
 *
 *  	adc     ...
 *      bcc     L
 *  	inx
 * L:
 *
 * and remove the handling of the high byte if X is not used later.
 */
{
    unsigned Changes = 0;

    /* Walk over the entries */
    unsigned I = 0;
    while (I < CS_GetEntryCount (S)) {

	CodeEntry* L[3];

      	/* Get next entry */
       	CodeEntry* E = CS_GetEntry (S, I);

     	/* Check for the sequence */
       	if (E->OPC == OP65_ADC 	  	     	             &&
	    CS_GetEntries (S, L, I+1, 3)   	             &&
       	    (L[0]->OPC == OP65_BCC || L[0]->OPC == OP65_JCC) &&
	    L[0]->JumpTo != 0                                &&
	    !CE_HasLabel (L[0])                              &&
	    L[1]->OPC == OP65_INX            	       	     &&
	    !CE_HasLabel (L[1])                              &&
	    L[0]->JumpTo->Owner == L[2]                      &&
	    !RegXUsed (S, I+3)) {

	    /* Remove the bcs/dex */
	    CS_DelEntries (S, I+1, 2);

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
/*				Optimize shifts                              */
/*****************************************************************************/



static unsigned OptShift1 (CodeSeg* S)
/* A call to the shlaxN routine may get replaced by one or more asl insns
 * if the value of X is not used later.
 */
{
    unsigned Changes = 0;

    /* Walk over the entries */
    unsigned I = 0;
    while (I < CS_GetEntryCount (S)) {

      	/* Get next entry */
       	CodeEntry* E = CS_GetEntry (S, I);

     	/* Check for the sequence */
	if (E->OPC == OP65_JSR                       &&
       	    (strncmp (E->Arg, "shlax", 5) == 0 ||
	     strncmp (E->Arg, "aslax", 5) == 0)	     &&
	    strlen (E->Arg) == 6                     &&
	    IsDigit (E->Arg[5])                      &&
	    !RegXUsed (S, I+1)) {

	    /* Insert shift insns */
	    unsigned Count = E->Arg[5] - '0';
	    while (Count--) {
	    	CodeEntry* X = NewCodeEntry (OP65_ASL, AM65_ACC, "a", 0, E->LI);
	    	CS_InsertEntry (S, X, I+1);
	    }

	    /* Delete the call to shlax */
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



static unsigned OptCmp2 (CodeSeg* S)
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
		 *      dey
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
		 *      dey
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



static unsigned OptCmp5 (CodeSeg* S)
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
       	if (E->OPC == OP65_JSR 	  		        &&
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



static unsigned OptCmp6 (CodeSeg* S)
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



static unsigned OptCmp7 (CodeSeg* S)
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



/*****************************************************************************/
/*	    			Optimize tests                               */
/*****************************************************************************/



static unsigned OptTest1 (CodeSeg* S)
/* On a sequence
 *
 *     stx     xxx
 *     ora     xxx
 *     beq/bne ...
 *
 * if X is zero, the sequence may be changed to
 *
 *     cmp     #$00
 *     beq/bne ...
 *
 * which may be optimized further by another step.
 *
 * If A is zero, the sequence may be changed to
 *
 *     txa
 *     beq/bne ...
 *
 */
{
    unsigned Changes = 0;
    unsigned I;

    /* Generate register info for this step */
    CS_GenRegInfo (S);

    /* Walk over the entries */
    I = 0;
    while (I < CS_GetEntryCount (S)) {

	CodeEntry* L[3];

      	/* Get next entry */
       	L[0] = CS_GetEntry (S, I);

	/* Check if it's the sequence we're searching for */
	if (L[0]->OPC == OP65_STX              &&
	    CS_GetEntries (S, L+1, I+1, 2)     &&
	    !CE_HasLabel (L[1])                &&
	    L[1]->OPC == OP65_ORA              &&
	    strcmp (L[0]->Arg, L[1]->Arg) == 0 &&
	    !CE_HasLabel (L[2])                &&
	    (L[2]->Info & OF_ZBRA) != 0) {

	    /* Check if X is zero */
	    if (L[0]->RI->In.RegX == 0) {

		/* Insert the compare */
		CodeEntry* N = NewCodeEntry (OP65_CMP, AM65_IMM, "$00", 0, L[0]->LI);
		CS_InsertEntry (S, N, I+2);

		/* Remove the two other insns */
		CS_DelEntry (S, I+1);
		CS_DelEntry (S, I);

		/* We had changes */
		++Changes;

	    /* Check if A is zero */
	    } else if (L[1]->RI->In.RegA == 0) {

		/* Insert the txa */
		CodeEntry* N = NewCodeEntry (OP65_TXA, AM65_IMP, 0, 0, L[1]->LI);
		CS_InsertEntry (S, N, I+2);

		/* Remove the two other insns */
		CS_DelEntry (S, I+1);
	  	CS_DelEntry (S, I);

	  	/* We had changes */
	  	++Changes;
	    }
	}

	/* Next entry */
	++I;

    }

    /* Free register info */
    CS_FreeRegInfo (S);

    /* Return the number of changes made */
    return Changes;
}



/*****************************************************************************/
/*		  	      nega optimizations			     */
/*****************************************************************************/



static unsigned OptNegA1 (CodeSeg* S)
/* Check for
 *
 *	ldx 	#$00
 *	lda 	..
 * 	jsr 	bnega
 *
 * Remove the ldx if the lda does not use it.
 */
{
    unsigned Changes = 0;

    /* Walk over the entries */
    unsigned I = 0;
    while (I < CS_GetEntryCount (S)) {

	CodeEntry* L[2];

      	/* Get next entry */
       	CodeEntry* E = CS_GetEntry (S, I);

     	/* Check for a ldx */
       	if (E->OPC == OP65_LDX 			&&
	    E->AM == AM65_IMM	    		&&
	    (E->Flags & CEF_NUMARG) != 0	&&
	    E->Num == 0	   			&&
  	    CS_GetEntries (S, L, I+1, 2)	&&
	    L[0]->OPC == OP65_LDA		&&
	    (L[0]->Use & REG_X) == 0	    	&&
	    !CE_HasLabel (L[0])                 &&
	    L[1]->OPC == OP65_JSR	    	&&
	    strcmp (L[1]->Arg, "bnega") == 0    &&
	    !CE_HasLabel (L[1])) {

	    /* Remove the ldx instruction */
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



static unsigned OptNegA2 (CodeSeg* S)
/* Check for
 *
 *	lda 	..
 * 	jsr 	bnega
 *	jeq/jne	..
 *
 * Adjust the conditional branch and remove the call to the subroutine.
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
	     E->OPC == OP65_TYA)                &&
	    CS_GetEntries (S, L, I+1, 2)	&&
       	    L[0]->OPC == OP65_JSR  	    	&&
	    strcmp (L[0]->Arg, "bnega") == 0	&&
	    !CE_HasLabel (L[0])	  	        &&
	    (L[1]->Info & OF_ZBRA) != 0         &&
	    !CE_HasLabel (L[1])) {

	    /* Invert the branch */
	    CE_ReplaceOPC (L[1], GetInverseBranch (L[1]->OPC));

	    /* Delete the subroutine call */
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



/*****************************************************************************/
/*		   	      negax optimizations			     */
/*****************************************************************************/



static unsigned OptNegAX1 (CodeSeg* S)
/* On a call to bnegax, if X is zero, the result depends only on the value in
 * A, so change the call to a call to bnega. This will get further optimized
 * later if possible.
 */
{
    unsigned Changes = 0;
    unsigned I;

    /* Generate register info for this step */
    CS_GenRegInfo (S);

    /* Walk over the entries */
    I = 0;
    while (I < CS_GetEntryCount (S)) {

      	/* Get next entry */
       	CodeEntry* E = CS_GetEntry (S, I);

	/* Check if this is a call to bnegax, and if X is known and zero */
	if (E->OPC == OP65_JSR              &&
	    E->RI->In.RegX == 0             &&
	    strcmp (E->Arg, "bnegax") == 0) {

	    /* We're cheating somewhat here ... */
	    E->Arg[5] = '\0';
	    E->Use &= ~REG_X;

	    /* We had changes */
	    ++Changes;
	}

	/* Next entry */
	++I;

    }

    /* Free register info */
    CS_FreeRegInfo (S);

    /* Return the number of changes made */
    return Changes;
}



static unsigned OptNegAX2 (CodeSeg* S)
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
    while (I < CS_GetEntryCount (S)) {

	CodeEntry* L[5];

      	/* Get next entry */
       	CodeEntry* E = CS_GetEntry (S, I);

     	/* Check for the sequence */
       	if (E->OPC == OP65_LDA  	      	&&
	    E->AM == AM65_ZP_INDY	      	&&
	    CS_GetEntries (S, L, I+1, 5)	&&
	    L[0]->OPC == OP65_TAX    		&&
	    L[1]->OPC == OP65_DEY    		&&
      	    L[2]->OPC == OP65_LDA    		&&
	    L[2]->AM == AM65_ZP_INDY  		&&
	    strcmp (L[2]->Arg, E->Arg) == 0	&&
	    !CE_HasLabel (L[2])		        &&
	    L[3]->OPC == OP65_JSR    		&&
	    strcmp (L[3]->Arg, "bnegax") == 0	&&
	    !CE_HasLabel (L[3])		        &&
       	    (L[4]->Info & OF_ZBRA) != 0         &&
	    !CE_HasLabel (L[4])) {

	    /* lda --> ora */
	    CE_ReplaceOPC (L[2], OP65_ORA);

  	    /* Invert the branch */
	    CE_ReplaceOPC (L[4], GetInverseBranch (L[4]->OPC));

      	    /* Delete the entries no longer needed. Beware: Deleting entries
	     * will change the indices.
	     */
       	    CS_DelEntry (S, I+4);	    	/* jsr bnegax */
	    CS_DelEntry (S, I+1);	    	/* tax */

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
 *  	lda	xx
 *  	ldx	yy
 *  	jsr	bnegax
 *    	jne/jeq	...
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
    while (I < CS_GetEntryCount (S)) {

      	CodeEntry* L[3];

      	/* Get next entry */
       	CodeEntry* E = CS_GetEntry (S, I);

     	/* Check for the sequence */
       	if (E->OPC == OP65_LDA  	      	&&
       	    CS_GetEntries (S, L, I+1, 3)	&&
	    L[0]->OPC == OP65_LDX       	&&
	    !CE_HasLabel (L[0]) 		&&
       	    L[1]->OPC == OP65_JSR      		&&
	    strcmp (L[1]->Arg, "bnegax") == 0	&&
	    !CE_HasLabel (L[1]) 		&&
       	    (L[2]->Info & OF_ZBRA) != 0         &&
	    !CE_HasLabel (L[2])) {

	    /* ldx --> ora */
	    CE_ReplaceOPC (L[0], OP65_ORA);

	    /* Invert the branch */
       	    CE_ReplaceOPC (L[2], GetInverseBranch (L[2]->OPC));

	    /* Delete the subroutine call */
       	    CS_DelEntry (S, I+2);

	    /* Remember, we had changes */
	    ++Changes;

	}

	/* Next entry */
	++I;

    }

    /* Return the number of changes made */
    return Changes;
}



static unsigned OptNegAX4 (CodeSeg* S)
/* Search for the sequence:
 *
 *    	jsr   	xxx
 *  	jsr   	bnega(x)
 *  	jeq/jne	...
 *
 * and replace it by:
 *
 *      jsr	xxx
 *  	<boolean test>
 *  	jne/jeq	...
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
       	if (E->OPC == OP65_JSR  	      	&&
       	    CS_GetEntries (S, L, I+1, 2)   	&&
       	    L[0]->OPC == OP65_JSR              	&&
	    strncmp (L[0]->Arg,"bnega",5) == 0 	&&
	    !CE_HasLabel (L[0]) 	       	&&
       	    (L[1]->Info & OF_ZBRA) != 0         &&
	    !CE_HasLabel (L[1])) {

	    CodeEntry* X;

	    /* Check if we're calling bnega or bnegax */
	    int ByteSized = (strcmp (L[0]->Arg, "bnega") == 0);

	    /* Insert apropriate test code */
	    if (ByteSized) {
	     	/* Test bytes */
	    	X = NewCodeEntry (OP65_TAX, AM65_IMP, 0, 0, L[0]->LI);
  	    	CS_InsertEntry (S, X, I+2);
	    } else {
	    	/* Test words */
	    	X = NewCodeEntry (OP65_STX, AM65_ZP, "tmp1", 0, L[0]->LI);
      	    	CS_InsertEntry (S, X, I+2);
	    	X = NewCodeEntry (OP65_ORA, AM65_ZP, "tmp1", 0, L[0]->LI);
	    	CS_InsertEntry (S, X, I+3);
	    }

	    /* Delete the subroutine call */
	    CS_DelEntry (S, I+1);

	    /* Invert the branch */
       	    CE_ReplaceOPC (L[1], GetInverseBranch (L[1]->OPC));

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
/*		       Optimize stores through pointers                      */
/*****************************************************************************/



static unsigned OptPtrStore1Sub (CodeSeg* S, unsigned I, CodeEntry** const L)
/* Check if this is one of the allowed suboperation for OptPtrStore1 */
{
    /* Check for a label attached to the entry */
    if (CE_HasLabel (L[0])) {
	return 0;
    }

    /* Check for single insn sub ops */
    if (L[0]->OPC == OP65_AND                                           ||
    	L[0]->OPC == OP65_EOR                                           ||
    	L[0]->OPC == OP65_ORA                                           ||
    	(L[0]->OPC == OP65_JSR && strncmp (L[0]->Arg, "shlax", 5) == 0) ||
       	(L[0]->OPC == OP65_JSR && strncmp (L[0]->Arg, "shrax", 5) == 0)) {

    	/* One insn */
    	return 1;

    } else if (L[0]->OPC == OP65_CLC                      &&
       	       (L[1] = CS_GetNextEntry (S, I)) != 0       &&
	       L[1]->OPC == OP65_ADC                      &&
	       !CE_HasLabel (L[1])) {
	return 2;
    } else if (L[0]->OPC == OP65_SEC                      &&
	       (L[1] = CS_GetNextEntry (S, I)) != 0       &&
	       L[1]->OPC == OP65_SBC                      &&
	       !CE_HasLabel (L[1])) {
	return 2;
    }



    /* Not found */
    return 0;
}



static unsigned OptPtrStore1 (CodeSeg* S)
/* Search for the sequence:
 *
 *    	jsr   	pushax
 *      ldy     xxx
 *      jsr     ldauidx
 *      subop
 *      ldy     yyy
 *  	jsr   	staspidx
 *
 * and replace it by:
 *
 *      sta     ptr1
 *      stx     ptr1+1
 *      ldy     xxx
 *      ldx     #$00
 *      lda     (ptr1),y
 *	subop
 *      ldy     yyy
 *      sta     (ptr1),y
 */
{
    unsigned Changes = 0;

    /* Walk over the entries */
    unsigned I = 0;
    while (I < CS_GetEntryCount (S)) {

	unsigned K;
     	CodeEntry* L[10];

      	/* Get next entry */
       	L[0] = CS_GetEntry (S, I);

     	/* Check for the sequence */
       	if (L[0]->OPC == OP65_JSR    	      	    &&
     	    strcmp (L[0]->Arg, "pushax") == 0       &&
       	    CS_GetEntries (S, L+1, I+1, 3)     	    &&
       	    L[1]->OPC == OP65_LDY              	    &&
     	    CE_KnownImm (L[1])                      &&
     	    !CE_HasLabel (L[1])      	       	    &&
     	    L[2]->OPC == OP65_JSR                   &&
     	    strcmp (L[2]->Arg, "ldauidx") == 0      &&
     	    !CE_HasLabel (L[2])                     &&
       	    (K = OptPtrStore1Sub (S, I+3, L+3)) > 0 &&
	    CS_GetEntries (S, L+3+K, I+3+K, 2)      &&
       	    L[3+K]->OPC == OP65_LDY                 &&
     	    CE_KnownImm (L[3+K])                    &&
	    !CE_HasLabel (L[3+K])                   &&
	    L[4+K]->OPC == OP65_JSR                 &&
	    strcmp (L[4+K]->Arg, "staspidx") == 0   &&
	    !CE_HasLabel (L[4+K])) {

	    CodeEntry* X;

	    /* Create and insert the stores */
       	    X = NewCodeEntry (OP65_STA, AM65_ZP, "ptr1", 0, L[0]->LI);
	    CS_InsertEntry (S, X, I+1);

	    X = NewCodeEntry (OP65_STX, AM65_ZP, "ptr1+1", 0, L[0]->LI);
	    CS_InsertEntry (S, X, I+2);

	    /* Delete the call to pushax */
	    CS_DelEntry (S, I);

	    /* Delete the call to ldauidx */
	    CS_DelEntry (S, I+3);

	    /* Insert the load from ptr1 */
	    X = NewCodeEntry (OP65_LDX, AM65_IMM, "$00", 0, L[3]->LI);
	    CS_InsertEntry (S, X, I+3);
	    X = NewCodeEntry (OP65_LDA, AM65_ZP_INDY, "ptr1", 0, L[2]->LI);
	    CS_InsertEntry (S, X, I+4);

	    /* Insert the store through ptr1 */
	    X = NewCodeEntry (OP65_STA, AM65_ZP_INDY, "ptr1", 0, L[3]->LI);
	    CS_InsertEntry (S, X, I+6+K);

	    /* Delete the call to staspidx */
	    CS_DelEntry (S, I+7+K);

	    /* Remember, we had changes */
	    ++Changes;

	}

	/* Next entry */
	++I;

    }

    /* Return the number of changes made */
    return Changes;
}



static unsigned OptPtrStore2 (CodeSeg* S)
/* Search for the sequence:
 *
 *    	jsr   	pushax
 *      lda     xxx
 *      ldy     yyy
 *  	jsr   	staspidx
 *
 * and replace it by:
 *
 *      sta     ptr1
 *      stx     ptr1+1
 *      lda     xxx
 *      ldy     yyy
 *      sta     (ptr1),y
 */
{
    unsigned Changes = 0;

    /* Walk over the entries */
    unsigned I = 0;
    while (I < CS_GetEntryCount (S)) {

	CodeEntry* L[4];

      	/* Get next entry */
       	L[0] = CS_GetEntry (S, I);

     	/* Check for the sequence */
       	if (L[0]->OPC == OP65_JSR  	      	&&
	    strcmp (L[0]->Arg, "pushax") == 0   &&
       	    CS_GetEntries (S, L+1, I+1, 3)   	&&
       	    L[1]->OPC == OP65_LDA              	&&
	    !CE_HasLabel (L[1])    	       	&&
	    L[2]->OPC == OP65_LDY               &&
	    !CE_HasLabel (L[2])                 &&
	    L[3]->OPC == OP65_JSR               &&
	    strcmp (L[3]->Arg, "staspidx") == 0 &&
	    !CE_HasLabel (L[3])) {

	    CodeEntry* X;

	    /* Create and insert the stores */
       	    X = NewCodeEntry (OP65_STA, AM65_ZP, "ptr1", 0, L[0]->LI);
	    CS_InsertEntry (S, X, I+1);

	    X = NewCodeEntry (OP65_STX, AM65_ZP, "ptr1+1", 0, L[0]->LI);
	    CS_InsertEntry (S, X, I+2);

	    /* Delete the call to pushax */
	    CS_DelEntry (S, I);

	    /* Insert the store through ptr1 */
	    X = NewCodeEntry (OP65_STA, AM65_ZP_INDY, "ptr1", 0, L[3]->LI);
	    CS_InsertEntry (S, X, I+4);

	    /* Delete the call to staspidx */
	    CS_DelEntry (S, I+5);

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
/*			Optimize loads through pointers                      */
/*****************************************************************************/



static unsigned OptPtrLoad1 (CodeSeg* S)
/* Search for the sequence:
 *
 *      tax
 *      dey
 *      lda     (sp),y             # May be any destination
 *      ldy     ...
 *  	jsr    	ldauidx
 *
 * and replace it by:
 *
 *      sta     ptr1+1
 *      dey
 *      lda     (sp),y
 *      sta     ptr1
 *      ldy     ...
 *      ldx     #$00
 *      lda     (ptr1),y
 */
{
    unsigned Changes = 0;

    /* Walk over the entries */
    unsigned I = 0;
    while (I < CS_GetEntryCount (S)) {

	CodeEntry* L[5];

      	/* Get next entry */
       	L[0] = CS_GetEntry (S, I);

     	/* Check for the sequence */
       	if (L[0]->OPC == OP65_TAX      	       	&&
       	    CS_GetEntries (S, L+1, I+1, 4)     	&&
       	    L[1]->OPC == OP65_DEY              	&&
	    !CE_HasLabel (L[1])        	       	&&
	    L[2]->OPC == OP65_LDA               &&
	    !CE_HasLabel (L[2])                 &&
	    L[3]->OPC == OP65_LDY               &&
	    !CE_HasLabel (L[3])                 &&
	    L[4]->OPC == OP65_JSR               &&
       	    strcmp (L[4]->Arg, "ldauidx") == 0  &&
	    !CE_HasLabel (L[4])) {

	    CodeEntry* X;

       	    /* Store the high byte and remove the TAX instead */
	    X = NewCodeEntry (OP65_STA, AM65_ZP, "ptr1+1", 0, L[0]->LI);
	    CS_InsertEntry (S, X, I+1);
	    CS_DelEntry (S, I);

	    /* Store the low byte */
	    X = NewCodeEntry (OP65_STA, AM65_ZP, "ptr1", 0, L[2]->LI);
	    CS_InsertEntry (S, X, I+3);

	    /* Delete the call to ldauidx */
	    CS_DelEntry (S, I+5);

	    /* Load high and low byte */
	    X = NewCodeEntry (OP65_LDX, AM65_IMM, "$00", 0, L[3]->LI);
	    CS_InsertEntry (S, X, I+5);
	    X = NewCodeEntry (OP65_LDA, AM65_ZP_INDY, "ptr1", 0, L[3]->LI);
	    CS_InsertEntry (S, X, I+6);

	    /* Remember, we had changes */
	    ++Changes;

	}

	/* Next entry */
	++I;

    }

    /* Return the number of changes made */
    return Changes;
}



static unsigned OptPtrLoad2 (CodeSeg* S)
/* Search for the sequence:
 *
 *      adc    	xxx
 *      tay
 *      txa
 *      adc     yyy
 *      tax
 *      tya
 *      ldy
 *  	jsr    	ldauidx
 *
 * and replace it by:
 *
 *      adc    	xxx
 *      sta     ptr1
 *      txa
 *      adc     yyy
 *      sta     ptr1+1
 *      ldy
 *     	ldx     #$00
 *      lda     (ptr1),y
 */
{
    unsigned Changes = 0;

    /* Walk over the entries */
    unsigned I = 0;
    while (I < CS_GetEntryCount (S)) {

	CodeEntry* L[8];

      	/* Get next entry */
       	L[0] = CS_GetEntry (S, I);

     	/* Check for the sequence */
       	if (L[0]->OPC == OP65_ADC      	       	&&
       	    CS_GetEntries (S, L+1, I+1, 7)     	&&
       	    L[1]->OPC == OP65_TAY              	&&
	    !CE_HasLabel (L[1])        	       	&&
	    L[2]->OPC == OP65_TXA               &&
	    !CE_HasLabel (L[2])                 &&
	    L[3]->OPC == OP65_ADC               &&
	    !CE_HasLabel (L[3])                 &&
	    L[4]->OPC == OP65_TAX               &&
	    !CE_HasLabel (L[4])                 &&
	    L[5]->OPC == OP65_TYA               &&
	    !CE_HasLabel (L[5])                 &&
	    L[6]->OPC == OP65_LDY               &&
	    !CE_HasLabel (L[6])                 &&
	    L[7]->OPC == OP65_JSR               &&
       	    strcmp (L[7]->Arg, "ldauidx") == 0  &&
	    !CE_HasLabel (L[7])) {

	    CodeEntry* X;

       	    /* Store the low byte and remove the TAY instead */
	    X = NewCodeEntry (OP65_STA, AM65_ZP, "ptr1", 0, L[0]->LI);
	    CS_InsertEntry (S, X, I+1);
	    CS_DelEntry (S, I+2);

	    /* Store the high byte */
	    X = NewCodeEntry (OP65_STA, AM65_ZP, "ptr1+1", 0, L[3]->LI);
	    CS_InsertEntry (S, X, I+4);

	    /* Delete more transfer insns */
	    CS_DelEntry (S, I+6);
	    CS_DelEntry (S, I+5);

	    /* Delete the call to ldauidx */
	    CS_DelEntry (S, I+6);

	    /* Load high and low byte */
	    X = NewCodeEntry (OP65_LDX, AM65_IMM, "$00", 0, L[6]->LI);
	    CS_InsertEntry (S, X, I+6);
	    X = NewCodeEntry (OP65_LDA, AM65_ZP_INDY, "ptr1", 0, L[6]->LI);
	    CS_InsertEntry (S, X, I+7);

	    /* Remember, we had changes */
	    ++Changes;

	}

	/* Next entry */
	++I;

    }

    /* Return the number of changes made */
    return Changes;
}



static unsigned OptPtrLoad3 (CodeSeg* S)
/* Search for the sequence:
 *
 *      adc    	xxx
 *      pha
 *      txa
 *      iny
 *      adc     yyy
 *      tax
 *      pla
 *      ldy
 *  	jsr    	ldauidx
 *
 * and replace it by:
 *
 *      adc    	xxx
 *      sta     ptr1
 *      txa
 *      iny
 *      adc     yyy
 *      sta     ptr1+1
 *      ldy
 *     	ldx     #$00
 *      lda     (ptr1),y
 */
{
    unsigned Changes = 0;

    /* Walk over the entries */
    unsigned I = 0;
    while (I < CS_GetEntryCount (S)) {

	CodeEntry* L[9];

      	/* Get next entry */
       	L[0] = CS_GetEntry (S, I);

     	/* Check for the sequence */
       	if (L[0]->OPC == OP65_ADC      	       	&&
       	    CS_GetEntries (S, L+1, I+1, 8)     	&&
       	    L[1]->OPC == OP65_PHA              	&&
	    !CE_HasLabel (L[1])        	       	&&
	    L[2]->OPC == OP65_TXA               &&
	    !CE_HasLabel (L[2])                 &&
	    L[3]->OPC == OP65_INY               &&
	    !CE_HasLabel (L[3])                 &&
       	    L[4]->OPC == OP65_ADC               &&
	    !CE_HasLabel (L[4])                 &&
	    L[5]->OPC == OP65_TAX               &&
	    !CE_HasLabel (L[5])                 &&
	    L[6]->OPC == OP65_PLA               &&
	    !CE_HasLabel (L[6])                 &&
	    L[7]->OPC == OP65_LDY               &&
	    !CE_HasLabel (L[7])                 &&
	    L[8]->OPC == OP65_JSR               &&
       	    strcmp (L[8]->Arg, "ldauidx") == 0  &&
	    !CE_HasLabel (L[8])) {

	    CodeEntry* X;

       	    /* Store the low byte and remove the PHA instead */
	    X = NewCodeEntry (OP65_STA, AM65_ZP, "ptr1", 0, L[0]->LI);
	    CS_InsertEntry (S, X, I+1);
	    CS_DelEntry (S, I+2);

	    /* Store the high byte */
	    X = NewCodeEntry (OP65_STA, AM65_ZP, "ptr1+1", 0, L[4]->LI);
	    CS_InsertEntry (S, X, I+5);

	    /* Delete more transfer and PLA insns */
	    CS_DelEntry (S, I+7);
	    CS_DelEntry (S, I+6);

	    /* Delete the call to ldauidx */
	    CS_DelEntry (S, I+7);

	    /* Load high and low byte */
	    X = NewCodeEntry (OP65_LDX, AM65_IMM, "$00", 0, L[6]->LI);
	    CS_InsertEntry (S, X, I+7);
	    X = NewCodeEntry (OP65_LDA, AM65_ZP_INDY, "ptr1", 0, L[6]->LI);
	    CS_InsertEntry (S, X, I+8);

	    /* Remember, we had changes */
	    ++Changes;

	}

	/* Next entry */
	++I;

    }

    /* Return the number of changes made */
    return Changes;
}



static unsigned OptPtrLoad4 (CodeSeg* S)
/* Search for the sequence:
 *
 *      lda     #<(label+0)
 *      ldx     #>(label+0)
 *      clc
 *      adc     xxx
 *      bcc     L
 *      inx
 * L:   ldy     #$00
 *      jsr     ldauidx
 *
 * and replace it by:
 *
 *      ldx     xxx
 *      lda     label,x
 *      ldx     #$00
 */
{
    unsigned Changes = 0;

    /* Walk over the entries */
    unsigned I = 0;
    while (I < CS_GetEntryCount (S)) {

	CodeEntry* L[8];
	unsigned Len;

      	/* Get next entry */
       	L[0] = CS_GetEntry (S, I);

     	/* Check for the sequence */
       	if (L[0]->OPC == OP65_LDA      	       	             &&
	    L[0]->AM == AM65_IMM                             &&
       	    CS_GetEntries (S, L+1, I+1, 7)     	             &&
       	    L[1]->OPC == OP65_LDX              	             &&
	    L[1]->AM == AM65_IMM                             &&
	    !CE_HasLabel (L[1])        	       	             &&
	    L[2]->OPC == OP65_CLC                            &&
	    !CE_HasLabel (L[2])                              &&
	    L[3]->OPC == OP65_ADC                            &&
	    (L[3]->AM == AM65_ABS || L[3]->AM == AM65_ZP)    &&
	    !CE_HasLabel (L[3])                              &&
       	    (L[4]->OPC == OP65_BCC || L[4]->OPC == OP65_JCC) &&
       	    L[4]->JumpTo != 0                                &&
       	    L[4]->JumpTo->Owner == L[6]                      &&
	    !CE_HasLabel (L[4])                              &&
	    L[5]->OPC == OP65_INX                            &&
	    !CE_HasLabel (L[5])                              &&
	    L[6]->OPC == OP65_LDY                            &&
	    CE_KnownImm (L[6])                               &&
	    L[6]->Num == 0                                   &&
       	    L[7]->OPC == OP65_JSR                            &&
       	    strcmp (L[7]->Arg, "ldauidx") == 0               &&
	    !CE_HasLabel (L[7])                              &&
	    /* Check the label last because this is quite costly */
	    (Len = strlen (L[0]->Arg)) > 3                   &&
	    L[0]->Arg[0] == '<'                              &&
	    L[0]->Arg[1] == '('                              &&
	    strlen (L[1]->Arg) == Len                        &&
	    L[1]->Arg[0] == '>'                              &&
       	    memcmp (L[0]->Arg+1, L[1]->Arg+1, Len-1) == 0) {

	    CodeEntry* X;
	    char* Label;

	    /* We will create all the new stuff behind the current one so
	     * we keep the line references.
	     */
	    X = NewCodeEntry (OP65_LDX,	L[3]->AM, L[3]->Arg, 0, L[0]->LI);
	    CS_InsertEntry (S, X, I+8);

	    Label = memcpy (xmalloc (Len-2), L[0]->Arg+2, Len-3);
	    Label[Len-3] = '\0';
	    X = NewCodeEntry (OP65_LDA, AM65_ABSX, Label, 0, L[0]->LI);
	    CS_InsertEntry (S, X, I+9);
	    xfree (Label);

	    X = NewCodeEntry (OP65_LDX, AM65_IMM, "$00", 0, L[0]->LI);
	    CS_InsertEntry (S, X, I+10);

	    /* Remove the old code */
	    CS_DelEntries (S, I, 8);

	    /* Remember, we had changes */
	    ++Changes;

	}

	/* Next entry */
	++I;

    }

    /* Return the number of changes made */
    return Changes;
}



static unsigned OptPtrLoad5 (CodeSeg* S)
/* Search for the sequence:
 *
 *      lda     #<(label+0)
 *      ldx     #>(label+0)
 *      ldy     #$xx
 *      clc
 *      adc     (sp),y
 *      bcc     L
 *      inx
 * L:   ldy     #$00
 *      jsr     ldauidx
 *
 * and replace it by:
 *
 *      ldy     #$xx
 *      lda     (sp),y
 *      tax
 *      lda     label,x
 *      ldx     #$00
 */
{
    unsigned Changes = 0;

    /* Walk over the entries */
    unsigned I = 0;
    while (I < CS_GetEntryCount (S)) {

	CodeEntry* L[9];
	unsigned Len;

      	/* Get next entry */
       	L[0] = CS_GetEntry (S, I);

     	/* Check for the sequence */
       	if (L[0]->OPC == OP65_LDA      	       	             &&
	    L[0]->AM == AM65_IMM                             &&
       	    CS_GetEntries (S, L+1, I+1, 8)     	             &&
       	    L[1]->OPC == OP65_LDX              	             &&
	    L[1]->AM == AM65_IMM                             &&
	    !CE_HasLabel (L[1])        	       	             &&
	    L[2]->OPC == OP65_LDY                            &&
	    CE_KnownImm (L[2])                               &&
	    !CE_HasLabel (L[2])                              &&
	    L[3]->OPC == OP65_CLC                            &&
	    !CE_HasLabel (L[3])                              &&
	    L[4]->OPC == OP65_ADC                            &&
	    L[4]->AM == AM65_ZP_INDY                         &&
	    !CE_HasLabel (L[4])                              &&
       	    (L[5]->OPC == OP65_BCC || L[5]->OPC == OP65_JCC) &&
       	    L[5]->JumpTo != 0                                &&
       	    L[5]->JumpTo->Owner == L[7]                      &&
	    !CE_HasLabel (L[5])                              &&
	    L[6]->OPC == OP65_INX                            &&
	    !CE_HasLabel (L[6])                              &&
	    L[7]->OPC == OP65_LDY                            &&
	    CE_KnownImm (L[7])                               &&
	    L[7]->Num == 0                                   &&
       	    L[8]->OPC == OP65_JSR                            &&
       	    strcmp (L[8]->Arg, "ldauidx") == 0               &&
	    !CE_HasLabel (L[8])                              &&
	    /* Check the label last because this is quite costly */
	    (Len = strlen (L[0]->Arg)) > 3                   &&
	    L[0]->Arg[0] == '<'                              &&
	    L[0]->Arg[1] == '('                              &&
	    strlen (L[1]->Arg) == Len                        &&
	    L[1]->Arg[0] == '>'                              &&
       	    memcmp (L[0]->Arg+1, L[1]->Arg+1, Len-1) == 0) {

	    CodeEntry* X;
	    char* Label;

	    /* Add the lda */
	    X = NewCodeEntry (OP65_LDA, AM65_ZP_INDY, L[4]->Arg, 0, L[0]->LI);
	    CS_InsertEntry (S, X, I+3);

	    /* Add the tax */
	    X = NewCodeEntry (OP65_TAX, AM65_IMP, 0, 0, L[0]->LI);
	    CS_InsertEntry (S, X, I+4);

	    /* Add the lda */
	    Label = memcpy (xmalloc (Len-2), L[0]->Arg+2, Len-3);
	    Label[Len-3] = '\0';
	    X = NewCodeEntry (OP65_LDA, AM65_ABSX, Label, 0, L[0]->LI);
	    CS_InsertEntry (S, X, I+5);
	    xfree (Label);

	    /* Add the ldx */
	    X = NewCodeEntry (OP65_LDX, AM65_IMM, "$00", 0, L[0]->LI);
	    CS_InsertEntry (S, X, I+6);

	    /* Remove the old code */
	    CS_DelEntries (S, I, 2);
	    CS_DelEntries (S, I+5, 6);

	    /* Remember, we had changes */
	    ++Changes;

	}

	/* Next entry */
	++I;

    }

    /* Return the number of changes made */
    return Changes;
}



static unsigned OptPtrLoad6 (CodeSeg* S)
/* Search for the sequence
 *
 *      ldy     ...
 *      jsr     ldauidx
 *
 * and replace it by:
 *
 *      ldy     ...
 *      stx     ptr1+1
 *      sta     ptr1
 *      ldx     #$00
 *      lda     (ptr1),y
 *
 * This step must be execute *after* OptPtrLoad1!
 */
{
    unsigned Changes = 0;

    /* Walk over the entries */
    unsigned I = 0;
    while (I < CS_GetEntryCount (S)) {

	CodeEntry* L[2];

      	/* Get next entry */
       	L[0] = CS_GetEntry (S, I);

     	/* Check for the sequence */
       	if (L[0]->OPC == OP65_LDY      	       	&&
       	    CS_GetEntries (S, L+1, I+1, 1)     	&&
       	    L[1]->OPC == OP65_JSR               &&
       	    strcmp (L[1]->Arg, "ldauidx") == 0  &&
	    !CE_HasLabel (L[1])) {

	    CodeEntry* X;

       	    /* Store the high byte */
       	    X = NewCodeEntry (OP65_STA, AM65_ZP, "ptr1", 0, L[0]->LI);
	    CS_InsertEntry (S, X, I+1);

	    /* Store the low byte */
	    X = NewCodeEntry (OP65_STX, AM65_ZP, "ptr1+1", 0, L[0]->LI);
	    CS_InsertEntry (S, X, I+2);

	    /* Delete the call to ldauidx */
	    CS_DelEntry (S, I+3);

	    /* Load the high and low byte */
	    X = NewCodeEntry (OP65_LDX, AM65_IMM, "$00", 0, L[0]->LI);
	    CS_InsertEntry (S, X, I+3);
	    X = NewCodeEntry (OP65_LDA, AM65_ZP_INDY, "ptr1", 0, L[0]->LI);
	    CS_InsertEntry (S, X, I+4);

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



/* Types of optimization steps */
enum {
    optPre,			/* Repeated once */
    optPreMain,                 /* Repeated more than once */
    optMain,                    /* dito */
    optPostMain,                /* dito */
    optPost                     /* Repeated once */
};

/* Table with all the optimization functions */
typedef struct OptFunc OptFunc;
struct OptFunc {
    unsigned        (*Func) (CodeSeg*); /* Optimizer function */
    const char*	    Name;  	        /* Name of optimizer step */
    unsigned char   Type;  		/* Type of this step */
    char	    Disabled;  	        /* True if pass disabled */
};

/* Macro that builds a table entry */
#define OptEntry(func,type)     { func, #func, type, 0 }

/* Table with optimizer steps */
static OptFunc OptFuncs [] = {
    /* Optimizes stores through pointers */
    OptEntry (OptPtrStore1, optPre),
    OptEntry (OptPtrStore2, optPre),
    /* Optimize loads through pointers */
    OptEntry (OptPtrLoad1, optPre),
    OptEntry (OptPtrLoad2, optPre),
    OptEntry (OptPtrLoad3, optPre),
    OptEntry (OptPtrLoad4, optPre),
    OptEntry (OptPtrLoad5, optPre),
    OptEntry (OptPtrLoad6, optMain),
    /* Optimize calls to nega */
    OptEntry (OptNegA1, optMain),
    OptEntry (OptNegA2, optMain),
    /* Optimize calls to negax */
    OptEntry (OptNegAX1, optPre),
    OptEntry (OptNegAX2, optPre),
    OptEntry (OptNegAX3, optPre),
    OptEntry (OptNegAX4, optPre),
    /* Optimize subtractions */
    OptEntry (OptSub1, optMain),
    OptEntry (OptSub2, optMain),
    /* Optimize additions */
    OptEntry (OptAdd1, optPre),
    OptEntry (OptAdd2, optPre),
    OptEntry (OptAdd3, optMain),
    /* Optimize shifts */
    OptEntry (OptShift1, optPre),
    /* Optimize jump cascades */
    OptEntry (OptJumpCascades, optMain),
    /* Remove dead jumps */
    OptEntry (OptDeadJumps, optMain),
    /* Change jsr/rts to jmp */
    OptEntry (OptRTS, optMain),
    /* Remove dead code */
    OptEntry (OptDeadCode, optMain),
    /* Optimize jump targets */
    OptEntry (OptJumpTarget, optMain),
    /* Optimize conditional branches */
    OptEntry (OptCondBranches, optMain),
    /* Replace jumps to RTS by RTS */
    OptEntry (OptRTSJumps, optMain),
    /* Remove calls to the bool transformer subroutines	*/
    OptEntry (OptBoolTransforms, optMain),
    /* Optimize compares */
    OptEntry (OptCmp1, optMain),
    OptEntry (OptCmp2, optMain),
    OptEntry (OptCmp3, optMain),
    OptEntry (OptCmp4, optMain),
    OptEntry (OptCmp5, optMain),
    OptEntry (OptCmp6, optMain),
    OptEntry (OptCmp7, optMain),
    /* Optimize tests */
    OptEntry (OptTest1, optMain),
    /* Remove unused loads */
    OptEntry (OptUnusedLoads, optMain),
    OptEntry (OptUnusedStores, optMain),
    OptEntry (OptDuplicateLoads, optMain),
    OptEntry (OptStoreLoad, optMain),
    OptEntry (OptTransfers, optMain),
    /* Optimize operations that use the stack to pass operands */
    OptEntry (OptStackOps, optMain),
    /* Optimize branch distance */
    OptEntry (OptBranchDist, optPost),
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
    if (strcmp (Name, "any") == 0) {
	unsigned I;
       	for (I = 0; I < sizeof(OptFuncs)/sizeof(OptFuncs[0]); ++I) {
       	    OptFuncs[I].Disabled = 1;
    	}
    } else {
     	OptFunc* F = FindOptStep (Name);
     	F->Disabled = 1;
    }
}



void EnableOpt (const char* Name)
/* Enable the optimization with the given name */
{
    if (strcmp (Name, "any") == 0) {
	unsigned I;
       	for (I = 0; I < sizeof(OptFuncs)/sizeof(OptFuncs[0]); ++I) {
       	    OptFuncs[I].Disabled = 0;
	}
    } else {
     	OptFunc* F = FindOptStep (Name);
     	F->Disabled = 0;
    }
}



void ListOptSteps (FILE* F)
/* List all optimization steps */
{
    unsigned I;
    for (I = 0; I < sizeof(OptFuncs)/sizeof(OptFuncs[0]); ++I) {
	fprintf (F, "%s\n", OptFuncs[I].Name);
    }
}



static void RepeatOptStep (CodeSeg* S, unsigned char Type, unsigned Max)
/* Repeat the optimizer step of type Type at may Max times */
{
    unsigned I;
    unsigned Pass = 0;
    unsigned OptChanges;

    /* Repeat max times of until there are no more changes */
    do {
     	/* Reset the number of changes */
     	OptChanges = 0;

    	/* Keep the user hapy */
    	Print (stdout, 1, "  Optimizer pass %u:\n", ++Pass);

       	/* Run all optimization steps */
       	for (I = 0; I < sizeof(OptFuncs)/sizeof(OptFuncs[0]); ++I) {

	    /* Get the table entry */
	    const OptFunc* F = OptFuncs + I;

	    /* Check if the type matches */
	    if (F->Type != Type) {
		/* Skip it */
		continue;
	    }

    	    /* Print the name of the following optimizer step */
    	    Print (stdout, 1, "    %s:%*s", F->Name, (int) (30-strlen(F->Name)), "");

    	    /* Check if the step is disabled */
       	    if (F->Disabled) {
    	       	Print (stdout, 1, "Disabled\n");
    	    } else {
    	       	unsigned Changes = F->Func (S);
    		OptChanges += Changes;
    		Print (stdout, 1, "%u Changes\n", Changes);
    	    }
    	}

    } while (--Max > 0 && OptChanges > 0);
}



void RunOpt (CodeSeg* S)
/* Run the optimizer */
{

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
    RepeatOptStep (S, optPre, 1);
    RepeatOptStep (S, optPreMain, 0xFFFF);
    RepeatOptStep (S, optMain, 0xFFFF);
    RepeatOptStep (S, optPostMain, 0xFFFF);
    RepeatOptStep (S, optPost, 1);
}



