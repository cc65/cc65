/*****************************************************************************/
/*                                                                           */
/*                               coptptrstore.c                              */
/*                                                                           */
/*                      Optimize stores through pointers                     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2012,      Ullrich von Bassewitz                                      */
/*                Roemerstrasse 52                                           */
/*                D-70794 Filderstadt                                        */
/* EMail:         uz@cc65.org                                                */
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
#include "xmalloc.h"

/* cc65 */
#include "codeent.h"
#include "codeinfo.h"
#include "coptptrstore.h"



/*****************************************************************************/
/*                             Helper functions                              */
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



static const char* ZPLoadAX (CodeSeg* S, unsigned I)
/* If the two instructions at S/I are a load of A/X from a two byte zero byte
 * location, return the name of the zero page location. Otherwise return NULL.
 */
{
    CodeEntry* L[2];
    unsigned Len;

    if (CS_GetEntries (S, L, I, 2)                              &&
        L[0]->OPC == OP65_LDA                                   &&
        L[0]->AM == AM65_ZP                                     &&
        L[1]->OPC == OP65_LDX                                   &&
        L[1]->AM == AM65_ZP                                     &&
        !CE_HasLabel (L[1])                                     &&
        (Len = strlen (L[0]->Arg)) == strlen (L[1]->Arg) - 2    &&
        memcmp (L[0]->Arg, L[1]->Arg, Len) == 0                 &&
        L[1]->Arg[Len] == '+'                                   &&
        L[1]->Arg[Len+1] == '1') {

        /* Return the label */
        return L[0]->Arg;

    } else {

        /* Not found */
        return 0;

    }
}



/*****************************************************************************/
/*		   	 	     Code                                    */
/*****************************************************************************/



unsigned OptPtrStore1 (CodeSeg* S)
/* Search for the sequence:
 *
 *      lda     #<(label+0)
 *      ldx     #>(label+0)
 *      clc
 *      adc     xxx
 *      bcc     L
 *      inx
 * L:   jsr	pushax
 *	ldx	#$00
 *	lda	yyy
 *	ldy     #$00
 *      jsr     staspidx
 *
 * and replace it by:
 *
 *      ldy     xxx
 *	ldx	#$00
 *	lda	yyy
 *      sta	label,y
 */
{
    unsigned Changes = 0;

    /* Walk over the entries */
    unsigned I = 0;
    while (I < CS_GetEntryCount (S)) {

	CodeEntry* L[11];
	unsigned Len;

      	/* Get next entry */
       	L[0] = CS_GetEntry (S, I);

     	/* Check for the sequence */
       	if (L[0]->OPC == OP65_LDA      	       	             &&
	    L[0]->AM == AM65_IMM                             &&
       	    CS_GetEntries (S, L+1, I+1, 10)                  &&
       	    L[1]->OPC == OP65_LDX              	             &&
	    L[1]->AM == AM65_IMM                             &&
	    L[2]->OPC == OP65_CLC                            &&
	    L[3]->OPC == OP65_ADC                            &&
	    (L[3]->AM == AM65_ABS || L[3]->AM == AM65_ZP)    &&
       	    (L[4]->OPC == OP65_BCC || L[4]->OPC == OP65_JCC) &&
       	    L[4]->JumpTo != 0                                &&
       	    L[4]->JumpTo->Owner == L[6]                      &&
	    L[5]->OPC == OP65_INX                            &&
            CE_IsCallTo (L[6], "pushax")                     &&
            L[7]->OPC == OP65_LDX                            &&
            L[8]->OPC == OP65_LDA                            &&
       	    L[9]->OPC == OP65_LDY                            &&
       	    CE_IsKnownImm (L[9], 0)                          &&
       	    CE_IsCallTo (L[10], "staspidx")                  &&
       	    !CS_RangeHasLabel (S, I+1, 5)                    &&
            !CS_RangeHasLabel (S, I+7, 4)                    &&
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
	    CS_InsertEntry (S, X, I+11);

       	    X = NewCodeEntry (OP65_LDX, L[7]->AM, L[7]->Arg, 0, L[7]->LI);
	    CS_InsertEntry (S, X, I+12);

            X = NewCodeEntry (OP65_LDA, L[8]->AM, L[8]->Arg, 0, L[8]->LI);
            CS_InsertEntry (S, X, I+13);

	    Label = memcpy (xmalloc (Len-2), L[0]->Arg+2, Len-3);
	    Label[Len-3] = '\0';
       	    X = NewCodeEntry (OP65_STA, AM65_ABSY, Label, 0, L[10]->LI);
	    CS_InsertEntry (S, X, I+14);
	    xfree (Label);

	    /* Remove the old code */
	    CS_DelEntries (S, I, 11);

	    /* Remember, we had changes */
	    ++Changes;

	}

	/* Next entry */
	++I;

    }

    /* Return the number of changes made */
    return Changes;
}



unsigned OptPtrStore2 (CodeSeg* S)
/* Search for the sequence:
 *
 *      lda     #<(label+0)
 *      ldx     #>(label+0)
 *      ldy     aaa
 *      clc
 *      adc     (sp),y
 *      bcc     L
 *      inx
 * L:   jsr	pushax
 *	ldx	#$00
 *	lda	yyy
 *	ldy     #$00
 *      jsr     staspidx
 *
 * and replace it by:
 *
 *      ldy     aaa
 *	ldx	#$00
 *	lda	(sp),y
 *      tay
 *      lda     yyy
 *      sta	label,y
 */
{
    unsigned Changes = 0;

    /* Walk over the entries */
    unsigned I = 0;
    while (I < CS_GetEntryCount (S)) {

	CodeEntry* L[12];
	unsigned Len;

      	/* Get next entry */
       	L[0] = CS_GetEntry (S, I);

     	/* Check for the sequence */
       	if (L[0]->OPC == OP65_LDA      	       	             &&
	    L[0]->AM == AM65_IMM                             &&
       	    CS_GetEntries (S, L+1, I+1, 11)                  &&
       	    L[1]->OPC == OP65_LDX              	             &&
	    L[1]->AM == AM65_IMM                             &&
            L[2]->OPC == OP65_LDY                            &&
       	    L[3]->OPC == OP65_CLC                            &&
	    L[4]->OPC == OP65_ADC                            &&
       	    L[4]->AM == AM65_ZP_INDY                         &&
       	    (L[5]->OPC == OP65_BCC || L[5]->OPC == OP65_JCC) &&
       	    L[5]->JumpTo != 0                                &&
       	    L[5]->JumpTo->Owner == L[7]                      &&
	    L[6]->OPC == OP65_INX                            &&
            CE_IsCallTo (L[7], "pushax")                     &&
            L[8]->OPC == OP65_LDX                            &&
            L[9]->OPC == OP65_LDA                            &&
       	    L[10]->OPC == OP65_LDY                           &&
       	    CE_IsKnownImm (L[10], 0)                         &&
       	    CE_IsCallTo (L[11], "staspidx")                  &&
       	    !CS_RangeHasLabel (S, I+1, 6)                    &&
            !CS_RangeHasLabel (S, I+8, 4)                    &&
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
	    X = NewCodeEntry (OP65_LDY,	L[2]->AM, L[2]->Arg, 0, L[2]->LI);
	    CS_InsertEntry (S, X, I+12);

       	    X = NewCodeEntry (OP65_LDX, L[8]->AM, L[8]->Arg, 0, L[8]->LI);
	    CS_InsertEntry (S, X, I+13);

            X = NewCodeEntry (OP65_LDA, L[4]->AM, L[4]->Arg, 0, L[4]->LI);
            CS_InsertEntry (S, X, I+14);

       	    X = NewCodeEntry (OP65_TAY, AM65_IMP, 0, 0, L[4]->LI);
	    CS_InsertEntry (S, X, I+15);

            X = NewCodeEntry (OP65_LDA, L[9]->AM, L[9]->Arg, 0, L[9]->LI);
            CS_InsertEntry (S, X, I+16);

	    Label = memcpy (xmalloc (Len-2), L[0]->Arg+2, Len-3);
	    Label[Len-3] = '\0';
       	    X = NewCodeEntry (OP65_STA, AM65_ABSY, Label, 0, L[11]->LI);
	    CS_InsertEntry (S, X, I+17);
            xfree (Label);

	    /* Remove the old code */
	    CS_DelEntries (S, I, 12);

	    /* Remember, we had changes */
	    ++Changes;

	}

	/* Next entry */
	++I;

    }

    /* Return the number of changes made */
    return Changes;
}



unsigned OptPtrStore3 (CodeSeg* S)
/* Search for the sequence:
 *
 *      lda     #<(label+0)
 *      ldx     #>(label+0)
 *      ldy     aaa
 *      clc
 *      adc     (sp),y
 *      bcc     L
 *      inx
 * L:   jsr	pushax
 *      ldy     #bbb
 *	ldx	#$00
 *	lda	(sp),y
 *	ldy     #$00
 *      jsr     staspidx
 *
 * and replace it by:
 *
 *      ldy     aaa
 *	lda	(sp),y
 *      tax
 *      ldy     #bbb-2
 *      lda     (sp),y
 *      sta	label,x
 *	ldx	#$00
 */
{
    unsigned Changes = 0;

    /* Walk over the entries */
    unsigned I = 0;
    while (I < CS_GetEntryCount (S)) {

	CodeEntry* L[13];
	unsigned Len;

      	/* Get next entry */
       	L[0] = CS_GetEntry (S, I);

     	/* Check for the sequence */
       	if (L[0]->OPC == OP65_LDA      	       	             &&
	    L[0]->AM == AM65_IMM                             &&
       	    CS_GetEntries (S, L+1, I+1, 12)                  &&
       	    L[1]->OPC == OP65_LDX              	             &&
	    L[1]->AM == AM65_IMM                             &&
            L[2]->OPC == OP65_LDY                            &&
       	    L[3]->OPC == OP65_CLC                            &&
	    L[4]->OPC == OP65_ADC                            &&
       	    L[4]->AM == AM65_ZP_INDY                         &&
       	    (L[5]->OPC == OP65_BCC || L[5]->OPC == OP65_JCC) &&
       	    L[5]->JumpTo != 0                                &&
       	    L[5]->JumpTo->Owner == L[7]                      &&
	    L[6]->OPC == OP65_INX                            &&
            CE_IsCallTo (L[7], "pushax")                     &&
            L[8]->OPC == OP65_LDY                            &&
            CE_IsConstImm (L[8])                             &&
            L[8]->Num >= 2                                   &&
            L[9]->OPC == OP65_LDX                            &&
            L[10]->OPC == OP65_LDA                           &&
            L[10]->AM == AM65_ZP_INDY                        &&
       	    L[11]->OPC == OP65_LDY                           &&
       	    CE_IsKnownImm (L[11], 0)                         &&
       	    CE_IsCallTo (L[12], "staspidx")                  &&
       	    !CS_RangeHasLabel (S, I+1, 6)                    &&
            !CS_RangeHasLabel (S, I+8, 5)                    &&
	    /* Check the label last because this is quite costly */
	    (Len = strlen (L[0]->Arg)) > 3                   &&
	    L[0]->Arg[0] == '<'                              &&
	    L[0]->Arg[1] == '('                              &&
	    strlen (L[1]->Arg) == Len                        &&
	    L[1]->Arg[0] == '>'                              &&
       	    memcmp (L[0]->Arg+1, L[1]->Arg+1, Len-1) == 0) {

	    CodeEntry* X;
            const char* Arg;
	    char* Label;

	    /* We will create all the new stuff behind the current one so
	     * we keep the line references.
	     */
	    X = NewCodeEntry (OP65_LDY,	L[2]->AM, L[2]->Arg, 0, L[2]->LI);
	    CS_InsertEntry (S, X, I+13);

            X = NewCodeEntry (OP65_LDA, L[4]->AM, L[4]->Arg, 0, L[4]->LI);
            CS_InsertEntry (S, X, I+14);

       	    X = NewCodeEntry (OP65_TAX, AM65_IMP, 0, 0, L[4]->LI);
	    CS_InsertEntry (S, X, I+15);

            Arg = MakeHexArg (L[8]->Num - 2);
            X = NewCodeEntry (OP65_LDY, L[8]->AM, Arg, 0, L[8]->LI);
            CS_InsertEntry (S, X, I+16);

            X = NewCodeEntry (OP65_LDA, L[10]->AM, L[10]->Arg, 0, L[10]->LI);
            CS_InsertEntry (S, X, I+17);

	    Label = memcpy (xmalloc (Len-2), L[0]->Arg+2, Len-3);
	    Label[Len-3] = '\0';
       	    X = NewCodeEntry (OP65_STA, AM65_ABSX, Label, 0, L[12]->LI);
	    CS_InsertEntry (S, X, I+18);
            xfree (Label);

       	    X = NewCodeEntry (OP65_LDX, L[9]->AM, L[9]->Arg, 0, L[9]->LI);
	    CS_InsertEntry (S, X, I+19);

	    /* Remove the old code */
	    CS_DelEntries (S, I, 13);

	    /* Remember, we had changes */
	    ++Changes;

	}

	/* Next entry */
	++I;

    }

    /* Return the number of changes made */
    return Changes;
}



unsigned OptPtrStore4 (CodeSeg* S)
/* Search for the sequence:
 *
 *      clc
 *      adc     xxx
 *      bcc     L
 *      inx
 * L:   jsr	pushax
 *	ldx	#$00
 *	lda	yyy
 *	ldy     #$00
 *      jsr     staspidx
 *
 * and replace it by:
 *
 *      sta     ptr1
 *      stx     ptr1+1
 *      ldy     xxx
 *      ldx     #$00
 *      lda     yyy
 *      sta     (ptr1),y
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
       	if (L[0]->OPC == OP65_CLC                               &&
       	    CS_GetEntries (S, L+1, I+1, 8)                      &&
	    L[1]->OPC == OP65_ADC                               &&
	    (L[1]->AM == AM65_ABS                       ||
             L[1]->AM == AM65_ZP                        ||
             L[1]->AM == AM65_IMM)                              &&
       	    (L[2]->OPC == OP65_BCC || L[2]->OPC == OP65_JCC)    &&
       	    L[2]->JumpTo != 0                                   &&
       	    L[2]->JumpTo->Owner == L[4]                         &&
       	    L[3]->OPC == OP65_INX                               &&
            CE_IsCallTo (L[4], "pushax")                        &&
            L[5]->OPC == OP65_LDX                               &&
            L[6]->OPC == OP65_LDA                               &&
       	    L[7]->OPC == OP65_LDY                               &&
       	    CE_IsKnownImm (L[7], 0)                             &&
       	    CE_IsCallTo (L[8], "staspidx")                      &&
       	    !CS_RangeHasLabel (S, I+1, 3)                       &&
            !CS_RangeHasLabel (S, I+5, 4)) {

	    CodeEntry* X;
            const char* ZPLoc;
                                                      
            /* Track the insertion point */
            unsigned IP = I + 9;

            /* If the sequence is preceeded by a load of a ZP value, we can
             * use this ZP value as a pointer.
             */
            if (I < 2 || (ZPLoc = ZPLoadAX (S, I-2)) == 0) {

                ZPLoc = "ptr1";

                /* Must use ptr1 */
                X = NewCodeEntry (OP65_STA, AM65_ZP, "ptr1", 0, L[8]->LI);
                CS_InsertEntry (S, X, IP++);

                X = NewCodeEntry (OP65_STX, AM65_ZP, "ptr1+1", 0, L[8]->LI);
                CS_InsertEntry (S, X, IP++);

            }

            X = NewCodeEntry (OP65_LDY, L[1]->AM, L[1]->Arg, 0, L[1]->LI);
            CS_InsertEntry (S, X, IP++);

            X = NewCodeEntry (OP65_LDX, L[5]->AM, L[5]->Arg, 0, L[5]->LI);
            CS_InsertEntry (S, X, IP++);

            X = NewCodeEntry (OP65_LDA, L[6]->AM, L[6]->Arg, 0, L[6]->LI);
            CS_InsertEntry (S, X, IP++);

            X = NewCodeEntry (OP65_STA, AM65_ZP_INDY, ZPLoc, 0, L[8]->LI);
            CS_InsertEntry (S, X, IP++);

	    /* Remove the old code */
	    CS_DelEntries (S, I, 9);

	    /* Remember, we had changes */
	    ++Changes;

	}

	/* Next entry */
	++I;

    }

    /* Return the number of changes made */
    return Changes;
}



unsigned OptPtrStore5 (CodeSeg* S)
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
 *
 * In case a/x is loaded from the register bank before the pushax, we can even
 * use the register bank instead of ptr1.
 *
 *
 *    	jsr    	pushax
 *      ldy     xxx
 *      jsr     ldauidx
 *      ldx     #$00
 *      lda     (zp),y
 *      subop
 *      ldy     yyy
 *      sta     (zp),y
 *  	jsr   	staspidx
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
       	if (CE_IsCallTo (L[0], "pushax")            &&
       	    CS_GetEntries (S, L+1, I+1, 3)     	    &&
       	    L[1]->OPC == OP65_LDY              	    &&
       	    CE_IsConstImm (L[1])                    &&
     	    !CE_HasLabel (L[1])      	       	    &&
       	    CE_IsCallTo (L[2], "ldauidx")           &&
     	    !CE_HasLabel (L[2])                     &&
       	    (K = OptPtrStore1Sub (S, I+3, L+3)) > 0 &&
	    CS_GetEntries (S, L+3+K, I+3+K, 2)      &&
       	    L[3+K]->OPC == OP65_LDY                 &&
     	    CE_IsConstImm (L[3+K])                  &&
	    !CE_HasLabel (L[3+K])                   &&
	    CE_IsCallTo (L[4+K], "staspidx")        &&
	    !CE_HasLabel (L[4+K])) {


            const char* RegBank = 0;
            const char* ZPLoc   = "ptr1";
	    CodeEntry* X;


            /* Get the preceeding two instructions and check them. We check
             * for:
             *          lda     regbank+n
             *          ldx     regbank+n+1
             */
            if (I > 1) {
                CodeEntry* P[2];
                P[0] = CS_GetEntry (S, I-2);
                P[1] = CS_GetEntry (S, I-1);
                if (P[0]->OPC == OP65_LDA &&
                    P[0]->AM  == AM65_ZP  &&
                    P[1]->OPC == OP65_LDX &&
                    P[1]->AM  == AM65_ZP  &&
                    !CE_HasLabel (P[1])   &&
                    strncmp (P[0]->Arg, "regbank+", 8) == 0) {

                    unsigned Len = strlen (P[0]->Arg);

                    if (strncmp (P[0]->Arg, P[1]->Arg, Len) == 0 &&
                        P[1]->Arg[Len+0] == '+'                  &&
                        P[1]->Arg[Len+1] == '1'                  &&
                        P[1]->Arg[Len+2] == '\0') {

                        /* Ok, found. Use the name of the register bank */
                        RegBank = ZPLoc = P[0]->Arg;
                    }
                }
            }

	    /* Insert the load via the zp pointer */
	    X = NewCodeEntry (OP65_LDX, AM65_IMM, "$00", 0, L[3]->LI);
	    CS_InsertEntry (S, X, I+3);
	    X = NewCodeEntry (OP65_LDA, AM65_ZP_INDY, ZPLoc, 0, L[2]->LI);
   	    CS_InsertEntry (S, X, I+4);

   	    /* Insert the store through the zp pointer */
   	    X = NewCodeEntry (OP65_STA, AM65_ZP_INDY, ZPLoc, 0, L[3]->LI);
   	    CS_InsertEntry (S, X, I+6+K);

   	    /* Delete the old code */
   	    CS_DelEntry (S, I+7+K);     /* jsr spaspidx */
            CS_DelEntry (S, I+2);       /* jsr ldauidx */

	    /* Create and insert the stores into the zp pointer if needed */
            if (RegBank == 0) {
                X = NewCodeEntry (OP65_STA, AM65_ZP, "ptr1", 0, L[0]->LI);
                CS_InsertEntry (S, X, I+1);
                X = NewCodeEntry (OP65_STX, AM65_ZP, "ptr1+1", 0, L[0]->LI);
                CS_InsertEntry (S, X, I+2);
            }

            /* Delete more old code. Do it here to keep a label attached to
             * entry I in place.
             */
            CS_DelEntry (S, I);         /* jsr pushax */

   	    /* Remember, we had changes */
   	    ++Changes;

   	}

   	/* Next entry */
   	++I;

    }

    /* Return the number of changes made */
    return Changes;
}



