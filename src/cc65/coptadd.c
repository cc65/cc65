/*****************************************************************************/
/*                                                                           */
/*				   coptadd.c                                 */
/*                                                                           */
/*			  Optimize addition sequences                        */
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
#include "xsprintf.h"

/* cc65 */
#include "codeent.h"
#include "codeinfo.h"
#include "coptadd.h"



/*****************************************************************************/
/*			      Optimize additions                             */
/*****************************************************************************/



unsigned OptAdd1 (CodeSeg* S)
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



unsigned OptAdd2 (CodeSeg* S)
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


	    /* Adjust the operand of the first LDY */
	    CE_SetNumArg (L[0], L[0]->Num - 1);

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
	    xsprintf (Buf, sizeof (Buf), "$%02X", (int) (L[0]->Num+1));
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



unsigned OptAdd3 (CodeSeg* S)
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



