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



#include <stdlib.h>
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
#include "coptadd.h"
#include "coptcmp.h"
#include "coptind.h"
#include "coptneg.h"
#include "coptstop.h"
#include "coptsub.h"
#include "copttest.h"
#include "cpu.h"
#include "error.h"
#include "global.h"
#include "codeopt.h"



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



static unsigned OptShift2 (CodeSeg* S)
/* A call to the shraxN routine may get replaced by one or more lsr insns
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
       	    strncmp (E->Arg, "shrax", 5) == 0        &&
	    strlen (E->Arg) == 6                     &&
	    IsDigit (E->Arg[5])                      &&
	    !RegXUsed (S, I+1)) {

	    /* Insert shift insns */
	    unsigned Count = E->Arg[5] - '0';
	    while (Count--) {
	    	CodeEntry* X = NewCodeEntry (OP65_LSR, AM65_ACC, "a", 0, E->LI);
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
       	if (CE_IsCall (L[0], "pushax")              &&
       	    CS_GetEntries (S, L+1, I+1, 3)     	    &&
       	    L[1]->OPC == OP65_LDY              	    &&
     	    CE_KnownImm (L[1])                      &&
     	    !CE_HasLabel (L[1])      	       	    &&
       	    CE_IsCall (L[2], "ldauidx")             &&
     	    !CE_HasLabel (L[2])                     &&
       	    (K = OptPtrStore1Sub (S, I+3, L+3)) > 0 &&
	    CS_GetEntries (S, L+3+K, I+3+K, 2)      &&
       	    L[3+K]->OPC == OP65_LDY                 &&
     	    CE_KnownImm (L[3+K])                    &&
	    !CE_HasLabel (L[3+K])                   &&
	    CE_IsCall (L[4+K], "staspidx")          &&
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
       	if (CE_IsCall (L[0], "pushax")          &&
       	    CS_GetEntries (S, L+1, I+1, 3)   	&&
       	    L[1]->OPC == OP65_LDA              	&&
	    !CE_HasLabel (L[1])    	       	&&
	    L[2]->OPC == OP65_LDY               &&
	    !CE_HasLabel (L[2])                 &&
	    CE_IsCall (L[3], "staspidx")        &&
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
	    CE_IsCall (L[4], "ldauidx")         &&
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
 *      clc
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
 *      clc
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

	CodeEntry* L[9];

      	/* Get next entry */
       	L[0] = CS_GetEntry (S, I);

     	/* Check for the sequence */
       	if (L[0]->OPC == OP65_CLC               &&
       	    CS_GetEntries (S, L+1, I+1, 8)     	&&
	    L[1]->OPC == OP65_ADC      	       	&&
	    !CE_HasLabel (L[1])                 &&
       	    L[2]->OPC == OP65_TAY              	&&
	    !CE_HasLabel (L[2])        	       	&&
	    L[3]->OPC == OP65_TXA               &&
	    !CE_HasLabel (L[3])                 &&
	    L[4]->OPC == OP65_ADC               &&
	    !CE_HasLabel (L[4])                 &&
	    L[5]->OPC == OP65_TAX               &&
	    !CE_HasLabel (L[5])                 &&
	    L[6]->OPC == OP65_TYA               &&
	    !CE_HasLabel (L[6])                 &&
	    L[7]->OPC == OP65_LDY               &&
	    !CE_HasLabel (L[7])                 &&
       	    CE_IsCall (L[8], "ldauidx")         &&
	    !CE_HasLabel (L[8])) {

	    CodeEntry* X;
	    CodeEntry* P;

       	    /* Store the low byte and remove the TAY instead */
	    X = NewCodeEntry (OP65_STA, AM65_ZP, "ptr1", 0, L[1]->LI);
	    CS_InsertEntry (S, X, I+2);
	    CS_DelEntry (S, I+3);

	    /* Store the high byte */
	    X = NewCodeEntry (OP65_STA, AM65_ZP, "ptr1+1", 0, L[4]->LI);
	    CS_InsertEntry (S, X, I+5);

	    /* If the instruction before the adc is a ldx, replace the
	     * txa by and lda with the same location of the ldx.
	     */
	    if ((P = CS_GetPrevEntry (S, I)) != 0 &&
	    	P->OPC == OP65_LDX                &&
	    	!CE_HasLabel (P)) {

	    	X = NewCodeEntry (OP65_LDA, P->AM, P->Arg, 0, P->LI);
	    	CS_InsertEntry (S, X, I+4);
	    	CS_DelEntry (S,	I+3);
	    }

	    /* Delete more transfer insns */
	    CS_DelEntry (S, I+7);
	    CS_DelEntry (S, I+6);

	    /* Delete the call to ldauidx */
	    CS_DelEntry (S, I+7);

	    /* Load high and low byte */
	    X = NewCodeEntry (OP65_LDX, AM65_IMM, "$00", 0, L[7]->LI);
	    CS_InsertEntry (S, X, I+7);
	    X = NewCodeEntry (OP65_LDA, AM65_ZP_INDY, "ptr1", 0, L[7]->LI);
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
       	    CE_IsCall (L[8], "ldauidx")         &&
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
 *      ldy     xxx
 *      ldx     #$00
 *      lda     label,y
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
       	    CE_IsCall (L[7], "ldauidx")                      &&
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
	    X = NewCodeEntry (OP65_LDY,	L[3]->AM, L[3]->Arg, 0, L[0]->LI);
	    CS_InsertEntry (S, X, I+8);

	    X = NewCodeEntry (OP65_LDX, AM65_IMM, "$00", 0, L[0]->LI);
	    CS_InsertEntry (S, X, I+9);

	    Label = memcpy (xmalloc (Len-2), L[0]->Arg+2, Len-3);
	    Label[Len-3] = '\0';
	    X = NewCodeEntry (OP65_LDA, AM65_ABSY, Label, 0, L[0]->LI);
	    CS_InsertEntry (S, X, I+10);
	    xfree (Label);

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
 *      tay
 *      ldx     #$00
 *      lda     label,y
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
       	    CE_IsCall (L[8], "ldauidx")                      &&
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

	    /* Add the tay */
	    X = NewCodeEntry (OP65_TAY, AM65_IMP, 0, 0, L[0]->LI);
	    CS_InsertEntry (S, X, I+4);

	    /* Add the ldx */
	    X = NewCodeEntry (OP65_LDX, AM65_IMM, "$00", 0, L[0]->LI);
	    CS_InsertEntry (S, X, I+5);

	    /* Add the lda */
	    Label = memcpy (xmalloc (Len-2), L[0]->Arg+2, Len-3);
	    Label[Len-3] = '\0';
	    X = NewCodeEntry (OP65_LDA, AM65_ABSY, Label, 0, L[0]->LI);
	    CS_InsertEntry (S, X, I+6);
	    xfree (Label);

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
       	    CE_IsCall (L[1], "ldauidx")         &&
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
/*			      Decouple operations                            */
/*****************************************************************************/



static unsigned OptDecouple (CodeSeg* S)
/* Decouple operations, that is, do the following replacements:
 *
 *   dex        -> ldx #imm
 *   inx        -> ldx #imm
 *   dey        -> ldy #imm
 *   iny        -> ldy #imm
 *   tax        -> ldx #imm
 *   txa        -> lda #imm
 *   tay        -> ldy #imm
 *   tya        -> lda #imm
 *   lda sreg   -> lda #imm
 *   ldx sreg   -> ldx #imm
 *   ldy sreg   -> ldy #imm
 *
 * Provided that the register values are known of course.
 */
{
    unsigned Changes = 0;
    unsigned I;

    /* Generate register info for the following step */
    CS_GenRegInfo (S);

    /* Walk over the entries */
    I = 0;
    while (I < CS_GetEntryCount (S)) {

	char Buf [16];

      	/* Get next entry */
       	CodeEntry* E = CS_GetEntry (S, I);

	/* Assume we have no replacement */
	CodeEntry* X = 0;

	/* Check the instruction */
	switch (E->OPC) {

	    case OP65_DEX:
	        if (E->RI->In.RegX >= 0) {
	      	    xsprintf (Buf, sizeof (Buf), "$%02X", (E->RI->In.RegX - 1) & 0xFF);
	      	    X = NewCodeEntry (OP65_LDX, AM65_IMM, Buf, 0, E->LI);
	    	}
	        break;

	    case OP65_DEY:
	        if (E->RI->In.RegY >= 0) {
	      	    xsprintf (Buf, sizeof (Buf), "$%02X", (E->RI->In.RegY - 1) & 0xFF);
	      	    X = NewCodeEntry (OP65_LDY, AM65_IMM, Buf, 0, E->LI);
	    	}
	        break;

	    case OP65_INX:
	        if (E->RI->In.RegX >= 0) {
	      	    xsprintf (Buf, sizeof (Buf), "$%02X", (E->RI->In.RegX + 1) & 0xFF);
	      	    X = NewCodeEntry (OP65_LDX, AM65_IMM, Buf, 0, E->LI);
	    	}
	        break;

	    case OP65_INY:
	        if (E->RI->In.RegY >= 0) {
	      	    xsprintf (Buf, sizeof (Buf), "$%02X", (E->RI->In.RegY + 1) & 0xFF);
	      	    X = NewCodeEntry (OP65_LDY, AM65_IMM, Buf, 0, E->LI);
	    	}
	        break;

	    case OP65_LDA:
	        if (E->AM == AM65_ZP) {
		    if ((E->Use & REG_SREG_LO) != 0 && E->RI->In.SRegLo >= 0) {
			xsprintf (Buf, sizeof (Buf), "$%02X", E->RI->In.SRegLo);
			X = NewCodeEntry (OP65_LDA, AM65_IMM, Buf, 0, E->LI);
		    } else if ((E->Use & REG_SREG_HI) != 0 && E->RI->In.SRegHi >= 0) {
			xsprintf (Buf, sizeof (Buf), "$%02X", E->RI->In.SRegHi);
			X = NewCodeEntry (OP65_LDA, AM65_IMM, Buf, 0, E->LI);
		    }
		}
		break;

	    case OP65_LDX:
	        if (E->AM == AM65_ZP) {
		    if ((E->Use & REG_SREG_LO) != 0 && E->RI->In.SRegLo >= 0) {
			xsprintf (Buf, sizeof (Buf), "$%02X", E->RI->In.SRegLo);
			X = NewCodeEntry (OP65_LDX, AM65_IMM, Buf, 0, E->LI);
		    } else if ((E->Use & REG_SREG_HI) != 0 && E->RI->In.SRegHi >= 0) {
			xsprintf (Buf, sizeof (Buf), "$%02X", E->RI->In.SRegHi);
			X = NewCodeEntry (OP65_LDX, AM65_IMM, Buf, 0, E->LI);
		    }
		}
		break;

	    case OP65_LDY:
	        if (E->AM == AM65_ZP) {
		    if ((E->Use & REG_SREG_LO) != 0 && E->RI->In.SRegLo >= 0) {
			xsprintf (Buf, sizeof (Buf), "$%02X", E->RI->In.SRegLo);
			X = NewCodeEntry (OP65_LDY, AM65_IMM, Buf, 0, E->LI);
		    } else if ((E->Use & REG_SREG_HI) != 0 && E->RI->In.SRegHi >= 0) {
			xsprintf (Buf, sizeof (Buf), "$%02X", E->RI->In.SRegHi);
			X = NewCodeEntry (OP65_LDY, AM65_IMM, Buf, 0, E->LI);
		    }
		}
		break;

	    case OP65_TAX:
	        if (E->RI->In.RegA >= 0) {
	      	    xsprintf (Buf, sizeof (Buf), "$%02X", E->RI->In.RegA);
	      	    X = NewCodeEntry (OP65_LDX, AM65_IMM, Buf, 0, E->LI);
	    	}
	        break;

	    case OP65_TAY:
	        if (E->RI->In.RegA >= 0) {
	      	    xsprintf (Buf, sizeof (Buf), "$%02X", E->RI->In.RegA);
	      	    X = NewCodeEntry (OP65_LDY, AM65_IMM, Buf, 0, E->LI);
	    	}
	        break;

	    case OP65_TXA:
	        if (E->RI->In.RegX >= 0) {
	      	    xsprintf (Buf, sizeof (Buf), "$%02X", E->RI->In.RegX);
	      	    X = NewCodeEntry (OP65_LDA, AM65_IMM, Buf, 0, E->LI);
	    	}
	        break;

	    case OP65_TYA:
	        if (E->RI->In.RegY >= 0) {
	      	    xsprintf (Buf, sizeof (Buf), "$%02X", E->RI->In.RegY);
	      	    X = NewCodeEntry (OP65_LDA, AM65_IMM, Buf, 0, E->LI);
	    	}
	        break;

	    default:
	        /* Avoid gcc warnings */
	        break;

	}

	/* Insert the replacement if we have one */
	if (X) {
	    CS_InsertEntry (S, X, I+1);
	    CS_DelEntry (S, I);
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



/*****************************************************************************/
/*	      		       Size optimization                             */
/*****************************************************************************/



#if 0
static unsigned OptSize1 (CodeSeg* S)
/* Do size optimization by calling special subroutines that preload registers.
 * This routine does not work standalone, it needs a following register load
 * removal pass.
 */
{
    static const char* Func = {
	"stax0sp",           /* staxysp, y = 0 */
	"addeq0sp",
	"ldax0sp",           /* ldaxysp, y = 1 */
       	"ldeax0sp",          /* ldeaxysp, y = 3 */
	"push0",             /* pushax, a = 0, x = 0 */
	"pusha0",            /* pushax, x = 0 */
	"pushaFF",           /* pushax, x = ff */
	"pusha0sp",          /* pushaysp, y = 0 */
	"tosadda0",          /* tosaddax, x = 0 */
	"tosanda0",          /* tosandax, x = 0 */
	"tosdiva0",          /* tosdivax, x = 0 */
	"toseqa0",           /* toseqax, x = 0 */
	"tosgea0",           /* tosgeax, x = 0 */
       	"tosgta0",           /* tosgtax, x = 0 */
	"tosadd0ax",         /* tosaddeax, sreg = 0 */
	"laddeqa",           /* laddeq, sreg = 0, x = 0 */
	"laddeq1",           /* laddeq, sreg = 0, x = 0, a = 1 */
	"laddeq0sp",         /* laddeqysp, y = 0 */
       	"tosand0ax",         /* tosandeax, sreg = 0 */
        "ldaxi",             /* ldaxidx, y = 1 */
	"ldeaxi",            /* ldeaxidx, y = 3 */
	"ldeax0sp",          /* ldeaxysp, y = 3 */
	"tosdiv0ax",         /* tosdiveax, sreg = 0 */
	"toslea0",           /* tosleax, x = 0 */
	"tosmod0ax",         /* tosmodeax, sreg = 0 */
	"tosmul0ax",         /* tosmuleax, sreg = 0 */
       	"tosumul0ax",        /* tosumuleax, sreg = 0 */
	"tosor0ax",          /* tosoreax, sreg = 0 */
	"push0ax",           /* pusheax, sreg = 0 */
	"tosrsub0ax",        /* tosrsubeax, sreg = 0 */
	"tosshl0ax",         /* tosshleax, sreg = 0 */
	"tosasl0ax",         /* tosasleax, sreg = 0 */
	"tosshr0ax",         /* tosshreax, sreg = 0 */
	"tosasr0ax",         /* tosasreax, sreg = 0 */
	"tossub0ax",         /* tossubeax, sreg = 0 */
	"lsubeqa",           /* lsubeq, sreg = 0, x = 0 */
	"lsubeq1",           /* lsubeq, sreg = 0, x = 0, a = 1 */
	"lsubeq0sp",         /* lsubeqysp, y = 0 */
	"toslta0",           /* tosltax, x = 0 */
	"tosudiv0ax",        /* tosudiveax, sreg = 0 */
	"tosumod0ax",        /* tosumodeax, sreg = 0 */
     	"tosxor0ax",         /* tosxoreax, sreg = 0 */
	"tosmoda0",          /* tosmodax, x = 0 */
	"tosmula0",          /* tosmulax, x = 0 */
	"tosumula0",         /* tosumulax, x = 0 */
	"tosnea0",           /* tosneax, x = 0 */
	"tosora0",           /* tosorax, x = 0 */
	"push1",             /* pushax, x = 0, a = 1 */
       	"push2",             /* pushax, x = 0, a = 2 */
	"push3",             /* pushax, x = 0, a = 3 */
	"push4",             /* pushax, x = 0, a = 4 */
	"push5",             /* pushax, x = 0, a = 5 */
	"push6",             /* pushax, x = 0, a = 6 */
	"push7",             /* pushax, x = 0, a = 7 */
	"pushc0",            /* pusha, a = 0 */
       	"pushc1",            /* pusha, a = 1 */
	"pushc2",            /* pusha, a = 2 */
	"tosrsuba0",         /* tosrsubax, x = 0 */
	"tosshla0",          /* tosshlax, x = 0 */
	"tosasla0",          /* tosaslax, x = 0 */
	"tosshra0",          /* tosshrax, x = 0 */
	"tosasra0",          /* tosasrax, x = 0 */
	"steax0sp",          /* steaxsp, y = 0 */
	"tossuba0",          /* tossubax, x = 0 */
	"subeq0sp",          /* subeqysp, y = 0 */
	"tosudiva0",         /* tosudivax, x = 0 */
	"tosugea0",          /* tosugeax, x = 0 */
       	"tosugta0",          /* tosugtax, x = 0 */
       	"tosulea0",          /* tosuleax, x = 0 */
       	"tosulta0",          /* tosultax, x = 0 */
       	"tosumoda0",         /* tosumodax, x = 0 */
       	"tosxora0",          /* tosxorax, x = 0 */
    };

    unsigned Changes = 0;
    unsigned I;

    /* Generate register info for the following step */
    CS_GenRegInfo (S);

    /* Walk over the entries */
    I = 0;
    while (I < CS_GetEntryCount (S)) {

      	/* Get next entry */
       	CodeEntry* E = CS_GetEntry (S, I);


	/* Next entry */
	++I;

    }

    /* Free register info */
    CS_FreeRegInfo (S);

    /* Return the number of changes made */
    return Changes;
}
#endif



static unsigned OptSize2 (CodeSeg* S)
/* Do size optimization by using shorter code sequences, even if this
 * introduces relations between instructions. This step must be one of the
 * last steps, because it makes further work much more difficult.
 */
{
    unsigned Changes = 0;
    unsigned I;

    /* Generate register info for the following step */
    CS_GenRegInfo (S);

    /* Walk over the entries */
    I = 0;
    while (I < CS_GetEntryCount (S)) {


      	/* Get next entry */
       	CodeEntry* E = CS_GetEntry (S, I);

	/* Assume we have no replacement */
	CodeEntry* X = 0;

	/* Check the instruction */
	switch (E->OPC) {

	    case OP65_LDA:
	        if (CE_KnownImm (E)) {
		    short Val = (short) E->Num;
		    if (Val == E->RI->In.RegX) {
		    	X = NewCodeEntry (OP65_TXA, AM65_IMP, 0, 0, E->LI);
		    } else if (Val == E->RI->In.RegY) {
		    	X = NewCodeEntry (OP65_TYA, AM65_IMP, 0, 0, E->LI);
		    } else if (E->RI->In.RegA >= 0 && CPU >= CPU_65C02) {
		    	if (Val == ((E->RI->In.RegA - 1) & 0xFF)) {
		     	    X = NewCodeEntry (OP65_DEA, AM65_IMP, 0, 0, E->LI);
		    	} else if (Val == ((E->RI->In.RegA + 1) & 0xFF)) {
		    	    X = NewCodeEntry (OP65_INA, AM65_IMP, 0, 0, E->LI);
	      	    	}
		    }
	      	}
	        break;

	    case OP65_LDX:
	        if (CE_KnownImm (E)) {
		    short Val = (short) E->Num;
		    if (E->RI->In.RegX >= 0 && Val == ((E->RI->In.RegX - 1) & 0xFF)) {
			X = NewCodeEntry (OP65_DEX, AM65_IMP, 0, 0, E->LI);
		    } else if (E->RI->In.RegX >= 0 && Val == ((E->RI->In.RegX + 1) & 0xFF)) {
			X = NewCodeEntry (OP65_INX, AM65_IMP, 0, 0, E->LI);
		    } else if (Val == E->RI->In.RegA) {
			X = NewCodeEntry (OP65_TAX, AM65_IMP, 0, 0, E->LI);
		    }
		}
	        break;

	    case OP65_LDY:
	        if (CE_KnownImm (E)) {
		    short Val = (short) E->Num;
		    if (E->RI->In.RegY >= 0 && Val == ((E->RI->In.RegY - 1) & 0xFF)) {
			X = NewCodeEntry (OP65_DEY, AM65_IMP, 0, 0, E->LI);
		    } else if (E->RI->In.RegY >= 0 && Val == ((E->RI->In.RegY + 1) & 0xFF)) {
			X = NewCodeEntry (OP65_INY, AM65_IMP, 0, 0, E->LI);
		    } else if (Val == E->RI->In.RegA) {
			X = NewCodeEntry (OP65_TAY, AM65_IMP, 0, 0, E->LI);
		    }
		}
	        break;

	    default:
	        /* Avoid gcc warnings */
	        break;

	}

	/* Insert the replacement if we have one */
	if (X) {
	    CS_InsertEntry (S, X, I+1);
	    CS_DelEntry (S, I);
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



/*****************************************************************************/
/*  	     	    		struct OptFunc                               */
/*****************************************************************************/



typedef struct OptFunc OptFunc;
struct OptFunc {
    unsigned       (*Func) (CodeSeg*);  /* Optimizer function */
    const char*    Name;                /* Name of the function/group */
    unsigned long  TotalRuns;		/* Total number of runs */
    unsigned long  LastRuns;            /* Last number of runs */
    unsigned long  TotalChanges;        /* Total number of changes */
    unsigned long  LastChanges;         /* Last number of changes */
    char           Disabled;            /* True if function disabled */
};



/*****************************************************************************/
/*     	       	      	       	     Code	   	  		     */
/*****************************************************************************/



/* Macro that builds a function description */
#define OptFuncEntry(func) static OptFuncDesc D##func = { func, #func, 0 }

/* A list of all the function descriptions */
static OptFunc DOptAdd1	       	= { OptAdd1,   	     "OptAdd1",      	0, 0, 0, 0, 0 };
static OptFunc DOptAdd2	       	= { OptAdd2,   	     "OptAdd2",      	0, 0, 0, 0, 0 };
static OptFunc DOptAdd3	       	= { OptAdd3,   	     "OptAdd3",      	0, 0, 0, 0, 0 };
static OptFunc DOptBoolTrans    = { OptBoolTrans,    "OptBoolTrans",    0, 0, 0, 0, 0 };
static OptFunc DOptBranchDist  	= { OptBranchDist,   "OptBranchDist",   0, 0, 0, 0, 0 };
static OptFunc DOptCmp1	       	= { OptCmp1,   	     "OptCmp1",      	0, 0, 0, 0, 0 };
static OptFunc DOptCmp2	       	= { OptCmp2,   	     "OptCmp2",      	0, 0, 0, 0, 0 };
static OptFunc DOptCmp3	       	= { OptCmp3,   	     "OptCmp3",      	0, 0, 0, 0, 0 };
static OptFunc DOptCmp4	       	= { OptCmp4,   	     "OptCmp4",      	0, 0, 0, 0, 0 };
static OptFunc DOptCmp5	       	= { OptCmp5,   	     "OptCmp5",      	0, 0, 0, 0, 0 };
static OptFunc DOptCmp6	       	= { OptCmp6,   	     "OptCmp6",      	0, 0, 0, 0, 0 };
static OptFunc DOptCmp7	       	= { OptCmp7,   	     "OptCmp7",      	0, 0, 0, 0, 0 };
static OptFunc DOptCondBranches	= { OptCondBranches, "OptCondBranches", 0, 0, 0, 0, 0 };
static OptFunc DOptDeadCode    	= { OptDeadCode,     "OptDeadCode",  	0, 0, 0, 0, 0 };
static OptFunc DOptDeadJumps   	= { OptDeadJumps,    "OptDeadJumps",    0, 0, 0, 0, 0 };
static OptFunc DOptDecouple     = { OptDecouple,     "OptDecouple",     0, 0, 0, 0, 0 };
static OptFunc DOptDupLoads     = { OptDupLoads,     "OptDupLoads",     0, 0, 0, 0, 0 };
static OptFunc DOptJumpCascades	= { OptJumpCascades, "OptJumpCascades", 0, 0, 0, 0, 0 };
static OptFunc DOptJumpTarget  	= { OptJumpTarget,   "OptJumpTarget",   0, 0, 0, 0, 0 };
static OptFunc DOptRTS 	       	= { OptRTS,    	     "OptRTS",       	0, 0, 0, 0, 0 };
static OptFunc DOptRTSJumps1    = { OptRTSJumps1,    "OptRTSJumps1",  	0, 0, 0, 0, 0 };
static OptFunc DOptRTSJumps2    = { OptRTSJumps2,    "OptRTSJumps2",  	0, 0, 0, 0, 0 };
static OptFunc DOptNegA1       	= { OptNegA1,  	     "OptNegA1",     	0, 0, 0, 0, 0 };
static OptFunc DOptNegA2       	= { OptNegA2,  	     "OptNegA2",     	0, 0, 0, 0, 0 };
static OptFunc DOptNegAX1      	= { OptNegAX1,       "OptNegAX1",    	0, 0, 0, 0, 0 };
static OptFunc DOptNegAX2      	= { OptNegAX2,       "OptNegAX2",    	0, 0, 0, 0, 0 };
static OptFunc DOptNegAX3      	= { OptNegAX3,       "OptNegAX3",    	0, 0, 0, 0, 0 };
static OptFunc DOptNegAX4      	= { OptNegAX4,       "OptNegAX4",    	0, 0, 0, 0, 0 };
static OptFunc DOptPtrLoad1    	= { OptPtrLoad1,     "OptPtrLoad1",  	0, 0, 0, 0, 0 };
static OptFunc DOptPtrLoad2    	= { OptPtrLoad2,     "OptPtrLoad2",  	0, 0, 0, 0, 0 };
static OptFunc DOptPtrLoad3    	= { OptPtrLoad3,     "OptPtrLoad3",  	0, 0, 0, 0, 0 };
static OptFunc DOptPtrLoad4    	= { OptPtrLoad4,     "OptPtrLoad4",  	0, 0, 0, 0, 0 };
static OptFunc DOptPtrLoad5    	= { OptPtrLoad5,     "OptPtrLoad5",  	0, 0, 0, 0, 0 };
static OptFunc DOptPtrLoad6    	= { OptPtrLoad6,     "OptPtrLoad6",  	0, 0, 0, 0, 0 };
static OptFunc DOptPtrStore1   	= { OptPtrStore1,    "OptPtrStore1",    0, 0, 0, 0, 0 };
static OptFunc DOptPtrStore2   	= { OptPtrStore2,    "OptPtrStore2",    0, 0, 0, 0, 0 };
static OptFunc DOptShift1      	= { OptShift1,       "OptShift1",    	0, 0, 0, 0, 0 };
static OptFunc DOptShift2      	= { OptShift2,       "OptShift2",    	0, 0, 0, 0, 0 };
/*static OptFunc DOptSize1        = { OptSize1,        "OptSize1",        0, 0, 0, 0, 0 };*/
static OptFunc DOptSize2        = { OptSize2,        "OptSize2",        0, 0, 0, 0, 0 };
static OptFunc DOptStackOps    	= { OptStackOps,     "OptStackOps",  	0, 0, 0, 0, 0 };
static OptFunc DOptStoreLoad   	= { OptStoreLoad,    "OptStoreLoad",    0, 0, 0, 0, 0 };
static OptFunc DOptSub1	       	= { OptSub1,   	     "OptSub1",      	0, 0, 0, 0, 0 };
static OptFunc DOptSub2	       	= { OptSub2,   	     "OptSub2",      	0, 0, 0, 0, 0 };
static OptFunc DOptTest1       	= { OptTest1,  	     "OptTest1",     	0, 0, 0, 0, 0 };
static OptFunc DOptTransfers   	= { OptTransfers,    "OptTransfers",    0, 0, 0, 0, 0 };
static OptFunc DOptUnusedLoads 	= { OptUnusedLoads,  "OptUnusedLoads",  0, 0, 0, 0, 0 };
static OptFunc DOptUnusedStores	= { OptUnusedStores, "OptUnusedStores", 0, 0, 0, 0, 0 };


/* Table containing all the steps in alphabetical order */
static OptFunc* OptFuncs[] = {
    &DOptAdd1,
    &DOptAdd2,
    &DOptAdd3,
    &DOptBoolTrans,
    &DOptBranchDist,
    &DOptCmp1,
    &DOptCmp2,
    &DOptCmp3,
    &DOptCmp4,
    &DOptCmp5,
    &DOptCmp6,
    &DOptCmp7,
    &DOptCondBranches,
    &DOptDeadCode,
    &DOptDeadJumps,
    &DOptDecouple,
    &DOptDupLoads,
    &DOptJumpCascades,
    &DOptJumpTarget,
    &DOptNegA1,
    &DOptNegA2,
    &DOptNegAX1,
    &DOptNegAX2,
    &DOptNegAX3,
    &DOptNegAX4,
    &DOptPtrLoad1,
    &DOptPtrLoad2,
    &DOptPtrLoad3,
    &DOptPtrLoad4,
    &DOptPtrLoad5,
    &DOptPtrLoad6,
    &DOptPtrStore1,
    &DOptPtrStore2,
    &DOptRTS,
    &DOptRTSJumps1,
    &DOptRTSJumps2,
    &DOptShift1,
    &DOptShift2,
    /*&DOptSize1,*/
    &DOptSize2,
    &DOptStackOps,
    &DOptStoreLoad,
    &DOptSub1,
    &DOptSub2,
    &DOptTest1,
    &DOptTransfers,
    &DOptUnusedLoads,
    &DOptUnusedStores,
};
#define OPTFUNC_COUNT  (sizeof(OptFuncs) / sizeof(OptFuncs[0]))



static int CmpOptStep (const void* Key, const void* Func)
/* Compare function for bsearch */
{
    return strcmp (Key, (*(const OptFunc**)Func)->Name);
}



static OptFunc* FindOptFunc (const char* Name)
/* Find an optimizer step by name in the table and return a pointer. Return
 * NULL if no such step is found.
 */
{
    /* Search for the function in the list */
    OptFunc** O = bsearch (Name, OptFuncs, OPTFUNC_COUNT, sizeof (OptFuncs[0]), CmpOptStep);
    return O? *O : 0;
}



static OptFunc* GetOptFunc (const char* Name)
/* Find an optimizer step by name in the table and return a pointer. Print an
 * error and call AbEnd if not found.
 */
{
    /* Search for the function in the list */
    OptFunc* F = FindOptFunc (Name);
    if (F == 0) {
	/* Not found */
	AbEnd ("Optimization step `%s' not found", Name);
    }
    return F;
}



void DisableOpt (const char* Name)
/* Disable the optimization with the given name */
{
    if (strcmp (Name, "any") == 0) {
	unsigned I;
       	for (I = 0; I < OPTFUNC_COUNT; ++I) {
       	    OptFuncs[I]->Disabled = 1;
    	}
    } else {
     	GetOptFunc(Name)->Disabled = 1;
    }
}



void EnableOpt (const char* Name)
/* Enable the optimization with the given name */
{
    if (strcmp (Name, "any") == 0) {
	unsigned I;
       	for (I = 0; I < OPTFUNC_COUNT; ++I) {
       	    OptFuncs[I]->Disabled = 0;
	}
    } else {
     	GetOptFunc(Name)->Disabled = 0;
    }
}



void ListOptSteps (FILE* F)
/* List all optimization steps */
{
    unsigned I;
    for (I = 0; I < OPTFUNC_COUNT; ++I) {
    	fprintf (F, "%s\n", OptFuncs[I]->Name);
    }
}



static void ReadOptStats (const char* Name)
/* Read the optimizer statistics file */
{
    char Buf [256];
    unsigned Lines;

    /* Try to open the file */
    FILE* F = fopen (Name, "r");
    if (F == 0) {
    	/* Ignore the error */
    	return;
    }

    /* Read and parse the lines */
    Lines = 0;
    while (fgets (Buf, sizeof (Buf), F) != 0) {

	char* B;
	unsigned Len;
	OptFunc* Func;

	/* Fields */
	char Name[32];
       	unsigned long  TotalRuns;
	unsigned long  TotalChanges;

	/* Count lines */
	++Lines;

	/* Remove trailing white space including the line terminator */
	B = Buf;
	Len = strlen (B);
	while (Len > 0 && IsSpace (B[Len-1])) {
	    --Len;
	}
    	B[Len] = '\0';

	/* Remove leading whitespace */
	while (IsSpace (*B)) {
	    ++B;
	}

	/* Check for empty and comment lines */
	if (*B == '\0' || *B == ';' || *B == '#') {
	    continue;
	}

	/* Parse the line */
       	if (sscanf (B, "%31s %lu %*u %lu %*u", Name, &TotalRuns, &TotalChanges) != 3) {
	    /* Syntax error */
	    continue;
	}

	/* Search for the optimizer step. */
	Func = FindOptFunc (Name);
	if (Func == 0) {
	    /* Not found */
	    continue;
	}

	/* Found the step, set the fields */
	Func->TotalRuns    = TotalRuns;
	Func->TotalChanges = TotalChanges;

    }

    /* Close the file, ignore errors here. */
    fclose (F);
}



static void WriteOptStats (const char* Name)
/* Write the optimizer statistics file */
{
    unsigned I;

    /* Try to open the file */
    FILE* F = fopen (Name, "w");
    if (F == 0) {
    	/* Ignore the error */
    	return;
    }

    /* Write the file */
    for (I = 0; I < OPTFUNC_COUNT; ++I) {
	const OptFunc* O = OptFuncs[I];
	fprintf (F,
       	       	 "%-20s %6lu %6lu %6lu %6lu\n",
		 O->Name,
		 O->TotalRuns,
		 O->LastRuns,
		 O->TotalChanges,
		 O->LastChanges);
    }

    /* Close the file, ignore errors here. */
    fclose (F);
}



static unsigned RunOptFunc (CodeSeg* S, OptFunc* F, unsigned Max)
/* Run one optimizer function Max times or until there are no more changes */
{
    unsigned Changes, C;

    /* Don't run the function if it is disabled */
    if (F->Disabled) {
    	return 0;
    }

    /* Run this until there are no more changes */
    Changes = 0;
    do {

	/* Run the function */
    	C = F->Func (S);
    	Changes += C;

    	/* Do statistics */
    	++F->TotalRuns;
    	++F->LastRuns;
    	F->TotalChanges += C;
    	F->LastChanges  += C;

    } while (--Max && C > 0);

    /* Return the number of changes */
    return Changes;
}



static void RunOptGroup1 (CodeSeg* S)
/* Run the first group of optimization steps. These steps translate known
 * patterns emitted by the code generator into more optimal patterns. Order
 * of the steps is important, because some of the steps done earlier cover
 * the same patterns as later steps as subpatterns.
 */
{
    RunOptFunc (S, &DOptPtrStore1, 1);
    RunOptFunc (S, &DOptPtrStore2, 1);
    RunOptFunc (S, &DOptPtrLoad1, 1);
    RunOptFunc (S, &DOptPtrLoad2, 1);
    RunOptFunc (S, &DOptPtrLoad3, 1);
    RunOptFunc (S, &DOptPtrLoad4, 1);
    RunOptFunc (S, &DOptPtrLoad5, 1);
    RunOptFunc (S, &DOptNegAX1, 1);
    RunOptFunc (S, &DOptNegAX2, 1);
    RunOptFunc (S, &DOptNegAX3, 1);
    RunOptFunc (S, &DOptNegAX4, 1);
    RunOptFunc (S, &DOptAdd1, 1);
    RunOptFunc (S, &DOptAdd2, 1);
    RunOptFunc (S, &DOptShift1, 1);
    RunOptFunc (S, &DOptShift2, 1);
}



static void RunOptGroup2 (CodeSeg* S)
/* Run one group of optimization steps. This step involves just decoupling
 * instructions by replacing them by instructions that do not depend on
 * previous instructions. This makes it easier to find instructions that
 * aren't used.
 */
{
    RunOptFunc (S, &DOptDecouple, 1);
}




static void RunOptGroup3 (CodeSeg* S)
/* Run one group of optimization steps. These steps depend on each other,
 * that means that one step may allow another step to do additional work,
 * so we will repeat the steps as long as we see any changes.
 */
{
    unsigned Changes;

    do {
	Changes = 0;

	Changes += RunOptFunc (S, &DOptPtrLoad6, 1);
       	Changes += RunOptFunc (S, &DOptNegA1, 1);
       	Changes += RunOptFunc (S, &DOptNegA2, 1);
       	Changes += RunOptFunc (S, &DOptSub1, 1);
       	Changes += RunOptFunc (S, &DOptSub2, 1);
       	Changes += RunOptFunc (S, &DOptAdd3, 1);
	Changes += RunOptFunc (S, &DOptStackOps, 1);
       	Changes += RunOptFunc (S, &DOptJumpCascades, 1);
       	Changes += RunOptFunc (S, &DOptDeadJumps, 1);
       	Changes += RunOptFunc (S, &DOptRTS, 1);
       	Changes += RunOptFunc (S, &DOptDeadCode, 1);
       	Changes += RunOptFunc (S, &DOptJumpTarget, 1);
	Changes += RunOptFunc (S, &DOptCondBranches, 1);
	Changes += RunOptFunc (S, &DOptRTSJumps1, 1);
	Changes += RunOptFunc (S, &DOptBoolTrans, 1);
	Changes += RunOptFunc (S, &DOptCmp1, 1);
	Changes += RunOptFunc (S, &DOptCmp2, 1);
	Changes += RunOptFunc (S, &DOptCmp3, 1);
	Changes += RunOptFunc (S, &DOptCmp4, 1);
	Changes += RunOptFunc (S, &DOptCmp5, 1);
	Changes += RunOptFunc (S, &DOptCmp6, 1);
	Changes += RunOptFunc (S, &DOptCmp7, 1);
	Changes += RunOptFunc (S, &DOptTest1, 1);
	Changes += RunOptFunc (S, &DOptUnusedLoads, 1);
	Changes += RunOptFunc (S, &DOptUnusedStores, 1);
	Changes += RunOptFunc (S, &DOptDupLoads, 1);
	Changes += RunOptFunc (S, &DOptStoreLoad, 1);
	Changes += RunOptFunc (S, &DOptTransfers, 1);

    } while (Changes);
}



static void RunOptGroup4 (CodeSeg* S)
/* The last group of optimization steps. Adjust branches.
 */
{
    /* Optimize for size, that is replace operations by shorter ones, even
     * if this does hinder further optimizations (no problem since we're
     * done soon).
     */
    RunOptFunc (S, &DOptSize2, 1);

    /* Run the jump target optimization again, since the size optimization
     * above may have opened new oportunities.
     */
    RunOptFunc (S, &DOptJumpTarget, 5);

    /* Adjust branch distances */
    RunOptFunc (S, &DOptBranchDist, 3);

    /* Replace conditional branches to RTS. If we had changes, we must run dead
     * code elimination again, since the change may have introduced dead code.
     */
    if (RunOptFunc (S, &DOptRTSJumps2, 1)) {
	RunOptFunc (S, &DOptDeadCode, 1);
    }
}



void RunOpt (CodeSeg* S)
/* Run the optimizer */
{
    const char* StatFileName;

    /* If we shouldn't run the optimizer, bail out */
    if (!Optimize) {
    	return;
    }

    /* Check if we are requested to write optimizer statistics */
    StatFileName = getenv ("CC65_OPTSTATS");
    if (StatFileName) {
	ReadOptStats (StatFileName);
    }

    /* Print the name of the function we are working on */
    if (S->Func) {
     	Print (stdout, 1, "Running optimizer for function `%s'\n", S->Func->Name);
    } else {
     	Print (stdout, 1, "Running optimizer for global code segment\n");
    }

    /* Run groups of optimizations */
    RunOptGroup1 (S);
    RunOptGroup2 (S);
    RunOptGroup3 (S);
    RunOptGroup4 (S);

    /* Write statistics */
    if (StatFileName) {
	WriteOptStats (StatFileName);
    }
}



