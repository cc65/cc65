/*****************************************************************************/
/*                                                                           */
/*                               coptptrload.c                               */
/*                                                                           */
/*                      Optimize loads through pointers                      */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2001-2009 Ullrich von Bassewitz                                       */
/*               Roemerstrasse 52                                            */
/*               D-70794 Filderstadt                                         */
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
#include "xmalloc.h"

/* cc65 */
#include "codeent.h"
#include "codeinfo.h"
#include "coptptrload.h"



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



unsigned OptPtrLoad1 (CodeSeg* S)
/* Search for the sequence:
**
**      clc
**      adc     xxx
**      tay
**      txa
**      adc     yyy
**      tax
**      tya
**      ldy     #$00
**      jsr     ldauidx
**
** and replace it by:
**
**      sta     ptr1
**      txa
**      clc
**      adc     yyy
**      sta     ptr1+1
**      ldy     xxx
**      ldx     #$00
**      lda     (ptr1),y
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
            CS_GetEntries (S, L+1, I+1, 8)      &&
            L[1]->OPC == OP65_ADC               &&
            (L[1]->AM == AM65_ABS          ||
             L[1]->AM == AM65_ZP           ||
             L[1]->AM == AM65_IMM)              &&
            L[2]->OPC == OP65_TAY               &&
            L[3]->OPC == OP65_TXA               &&
            L[4]->OPC == OP65_ADC               &&
            L[5]->OPC == OP65_TAX               &&
            L[6]->OPC == OP65_TYA               &&
            L[7]->OPC == OP65_LDY               &&
            CE_IsKnownImm (L[7], 0)             &&
            CE_IsCallTo (L[8], "ldauidx")       &&
            !CS_RangeHasLabel (S, I+1, 8)) {

            CodeEntry* X;
            CodeEntry* P;

            /* Track the insertion point */
            unsigned IP = I+9;

            /* sta ptr1 */
            X = NewCodeEntry (OP65_STA, AM65_ZP, "ptr1", 0, L[2]->LI);
            CS_InsertEntry (S, X, IP++);

            /* If the instruction before the clc is a ldx, replace the
            ** txa by an lda with the same location of the ldx. Otherwise
            ** transfer the value in X to A.
            */
            if ((P = CS_GetPrevEntry (S, I)) != 0 &&
                P->OPC == OP65_LDX                &&
                !CE_HasLabel (P)) {
                X = NewCodeEntry (OP65_LDA, P->AM, P->Arg, 0, P->LI);
            } else {
                X = NewCodeEntry (OP65_TXA, AM65_IMP, 0, 0, L[3]->LI);
            }
            CS_InsertEntry (S, X, IP++);

            /* clc */
            X = NewCodeEntry (OP65_CLC, AM65_IMP, 0, 0, L[0]->LI);
            CS_InsertEntry (S, X, IP++);

            /* adc yyy */
            X = NewCodeEntry (OP65_ADC, L[4]->AM, L[4]->Arg, 0, L[4]->LI);
            CS_InsertEntry (S, X, IP++);

            /* sta ptr1+1 */
            X = NewCodeEntry (OP65_STA, AM65_ZP, "ptr1+1", 0, L[5]->LI);
            CS_InsertEntry (S, X, IP++);

            /* ldy xxx */
            X = NewCodeEntry (OP65_LDY, L[1]->AM, L[1]->Arg, 0, L[1]->LI);
            CS_InsertEntry (S, X, IP++);

            /* ldx #$00 */
            X = NewCodeEntry (OP65_LDX, AM65_IMM, "$00", 0, L[8]->LI);
            CS_InsertEntry (S, X, IP++);

            /* lda (ptr1),y */
            X = NewCodeEntry (OP65_LDA, AM65_ZP_INDY, "ptr1", 0, L[8]->LI);
            CS_InsertEntry (S, X, IP++);

            /* Remove the old instructions */
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



unsigned OptPtrLoad2 (CodeSeg* S)
/* Search for the sequence:
**
**      adc     xxx
**      pha
**      txa
**      iny
**      adc     yyy
**      tax
**      pla
**      ldy
**      jsr     ldauidx
**
** and replace it by:
**
**      adc     xxx
**      sta     ptr1
**      txa
**      iny
**      adc     yyy
**      sta     ptr1+1
**      ldy
**      ldx     #$00
**      lda     (ptr1),y
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
        if (L[0]->OPC == OP65_ADC               &&
            CS_GetEntries (S, L+1, I+1, 8)      &&
            L[1]->OPC == OP65_PHA               &&
            L[2]->OPC == OP65_TXA               &&
            L[3]->OPC == OP65_INY               &&
            L[4]->OPC == OP65_ADC               &&
            L[5]->OPC == OP65_TAX               &&
            L[6]->OPC == OP65_PLA               &&
            L[7]->OPC == OP65_LDY               &&
            CE_IsCallTo (L[8], "ldauidx")       &&
            !CS_RangeHasLabel (S, I+1, 8)) {

            CodeEntry* X;

            /* Store the low byte and remove the PHA instead */
            X = NewCodeEntry (OP65_STA, AM65_ZP, "ptr1", 0, L[0]->LI);
            CS_InsertEntry (S, X, I+1);

            /* Store the high byte */
            X = NewCodeEntry (OP65_STA, AM65_ZP, "ptr1+1", 0, L[4]->LI);
            CS_InsertEntry (S, X, I+6);

            /* Load high and low byte */
            X = NewCodeEntry (OP65_LDX, AM65_IMM, "$00", 0, L[6]->LI);
            CS_InsertEntry (S, X, I+10);
            X = NewCodeEntry (OP65_LDA, AM65_ZP_INDY, "ptr1", 0, L[6]->LI);
            CS_InsertEntry (S, X, I+11);

            /* Delete the old code */
            CS_DelEntry (S, I+12);      /* jsr ldauidx */
            CS_DelEntry (S, I+8);       /* pla */
            CS_DelEntry (S, I+7);       /* tax */
            CS_DelEntry (S, I+2);       /* pha */

            /* Remember, we had changes */
            ++Changes;

        }

        /* Next entry */
        ++I;

    }

    /* Return the number of changes made */
    return Changes;
}



unsigned OptPtrLoad3 (CodeSeg* S)
/* Search for the sequence:
**
**      lda     #<(label+0)
**      ldx     #>(label+0)
**      clc
**      adc     xxx
**      bcc     L
**      inx
** L:   ldy     #$00
**      jsr     ldauidx
**
** and replace it by:
**
**      ldy     xxx
**      ldx     #$00
**      lda     label,y
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
        if (L[0]->OPC == OP65_LDA                            &&
            L[0]->AM == AM65_IMM                             &&
            CS_GetEntries (S, L+1, I+1, 7)                   &&
            L[1]->OPC == OP65_LDX                            &&
            L[1]->AM == AM65_IMM                             &&
            L[2]->OPC == OP65_CLC                            &&
            L[3]->OPC == OP65_ADC                            &&
            (L[3]->AM == AM65_ABS || L[3]->AM == AM65_ZP)    &&
            (L[4]->OPC == OP65_BCC || L[4]->OPC == OP65_JCC) &&
            L[4]->JumpTo != 0                                &&
            L[4]->JumpTo->Owner == L[6]                      &&
            L[5]->OPC == OP65_INX                            &&
            L[6]->OPC == OP65_LDY                            &&
            CE_IsKnownImm (L[6], 0)                          &&
            CE_IsCallTo (L[7], "ldauidx")                    &&
            !CS_RangeHasLabel (S, I+1, 5)                    &&
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
            ** we keep the line references.
            */
            X = NewCodeEntry (OP65_LDY, L[3]->AM, L[3]->Arg, 0, L[0]->LI);
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



unsigned OptPtrLoad4 (CodeSeg* S)
/* Search for the sequence:
**
**      lda     #<(label+0)
**      ldx     #>(label+0)
**      ldy     #$xx
**      clc
**      adc     (sp),y
**      bcc     L
**      inx
** L:   ldy     #$00
**      jsr     ldauidx
**
** and replace it by:
**
**      ldy     #$xx
**      lda     (sp),y
**      tay
**      ldx     #$00
**      lda     label,y
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
        if (L[0]->OPC == OP65_LDA                            &&
            L[0]->AM == AM65_IMM                             &&
            CS_GetEntries (S, L+1, I+1, 8)                   &&
            L[1]->OPC == OP65_LDX                            &&
            L[1]->AM == AM65_IMM                             &&
            !CE_HasLabel (L[1])                              &&
            L[2]->OPC == OP65_LDY                            &&
            CE_IsConstImm (L[2])                             &&
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
            CE_IsKnownImm (L[7], 0)                          &&
            CE_IsCallTo (L[8], "ldauidx")                    &&
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



unsigned OptPtrLoad5 (CodeSeg* S)
/* Search for the sequence:
**
**      jsr     pushax
**      ldx     #$00
**      lda     yyy
**      jsr     tosaddax
**      ldy     #$00
**      jsr     ldauidx
**
** and replace it by:
**
**      sta     ptr1
**      stx     ptr1+1
**      ldy     yyy
**      ldx     #$00
**      lda     (ptr1),y
*/
{
    unsigned Changes = 0;

    /* Walk over the entries */
    unsigned I = 0;
    while (I < CS_GetEntryCount (S)) {

        CodeEntry* L[6];

        /* Get next entry */
        L[0] = CS_GetEntry (S, I);

        /* Check for the sequence */
        if (CE_IsCallTo (L[0], "pushax")                &&
            CS_GetEntries (S, L+1, I+1, 5)              &&
            L[1]->OPC == OP65_LDX                       &&
            CE_IsKnownImm (L[1], 0)                     &&
            L[2]->OPC == OP65_LDA                       &&
            (L[2]->AM == AM65_ABS       ||
             L[2]->AM == AM65_ZP        ||
             L[2]->AM == AM65_IMM)                      &&
            CE_IsCallTo (L[3], "tosaddax")              &&
            L[4]->OPC == OP65_LDY                       &&
            CE_IsKnownImm (L[4], 0)                     &&
            CE_IsCallTo (L[5], "ldauidx")               &&
            !CS_RangeHasLabel (S, I+1, 5)) {

            CodeEntry* X;

            /* sta ptr1 */
            X = NewCodeEntry (OP65_STA, AM65_ZP, "ptr1", 0, L[0]->LI);
            CS_InsertEntry (S, X, I+6);

            /* stx ptr1+1 */
            X = NewCodeEntry (OP65_STX, AM65_ZP, "ptr1+1", 0, L[0]->LI);
            CS_InsertEntry (S, X, I+7);

            /* ldy yyy */
            X = NewCodeEntry (OP65_LDY, L[2]->AM, L[2]->Arg, 0, L[2]->LI);
            CS_InsertEntry (S, X, I+8);

            /* ldx #$00 */
            X = NewCodeEntry (OP65_LDX, AM65_IMM, "$00", 0, L[5]->LI);
            CS_InsertEntry (S, X, I+9);

            /* lda (ptr1),y */
            X = NewCodeEntry (OP65_LDA, AM65_ZP_INDY, "ptr1", 0, L[5]->LI);
            CS_InsertEntry (S, X, I+10);

            /* Remove the old code */
            CS_DelEntries (S, I, 6);

            /* Remember, we had changes */
            ++Changes;

        }

        /* Next entry */
        ++I;

    }

    /* Return the number of changes made */
    return Changes;
}



unsigned OptPtrLoad6 (CodeSeg* S)
/* Search for the sequence:
**
**      jsr     pushax
**      ldy     #xxx
**      ldx     #$00
**      lda     (sp),y
**      jsr     tosaddax
**      ldy     #$00
**      jsr     ldauidx
**
** and replace it by:
**
**      sta     ptr1
**      stx     ptr1+1
**      ldy     #xxx-2
**      lda     (sp),y
**      tay
**      ldx     #$00
**      lda     (ptr1),y
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
        if (CE_IsCallTo (L[0], "pushax")                &&
            CS_GetEntries (S, L+1, I+1, 6)              &&
            L[1]->OPC == OP65_LDY                       &&
            CE_IsConstImm (L[1])                        &&
            L[1]->Num >= 2                              &&
            L[2]->OPC == OP65_LDX                       &&
            CE_IsKnownImm (L[2], 0)                     &&
            L[3]->OPC == OP65_LDA                       &&
            L[3]->AM == AM65_ZP_INDY                    &&
            CE_IsCallTo (L[4], "tosaddax")              &&
            L[5]->OPC == OP65_LDY                       &&
            CE_IsKnownImm (L[5], 0)                     &&
            CE_IsCallTo (L[6], "ldauidx")               &&
            !CS_RangeHasLabel (S, I+1, 6)               &&
            !RegYUsed (S, I+7)) {

            CodeEntry*  X;
            const char* Arg;

            /* sta ptr1 */
            X = NewCodeEntry (OP65_STA, AM65_ZP, "ptr1", 0, L[0]->LI);
            CS_InsertEntry (S, X, I+7);

            /* stx ptr1+1 */
            X = NewCodeEntry (OP65_STX, AM65_ZP, "ptr1+1", 0, L[0]->LI);
            CS_InsertEntry (S, X, I+8);

            /* ldy #xxx-2 */
            Arg = MakeHexArg (L[1]->Num - 2);
            X = NewCodeEntry (OP65_LDY, AM65_IMM, Arg, 0, L[1]->LI);
            CS_InsertEntry (S, X, I+9);

            /* lda (sp),y */
            X = NewCodeEntry (OP65_LDA, L[3]->AM, L[3]->Arg, 0, L[3]->LI);
            CS_InsertEntry (S, X, I+10);

            /* tay */
            X = NewCodeEntry (OP65_TAY, AM65_IMP, 0, 0, L[3]->LI);
            CS_InsertEntry (S, X, I+11);

            /* ldx #$00 */
            X = NewCodeEntry (OP65_LDX, AM65_IMM, "$00", 0, L[5]->LI);
            CS_InsertEntry (S, X, I+12);

            /* lda (ptr1),y */
            X = NewCodeEntry (OP65_LDA, AM65_ZP_INDY, "ptr1", 0, L[6]->LI);
            CS_InsertEntry (S, X, I+13);

            /* Remove the old code */
            CS_DelEntries (S, I, 7);

            /* Remember, we had changes */
            ++Changes;

        }

        /* Next entry */
        ++I;

    }

    /* Return the number of changes made */
    return Changes;
}



unsigned OptPtrLoad7 (CodeSeg* S)
/* Search for the sequence:
**
**      jsr     aslax1/shlax1
**      clc
**      adc     xxx
**      tay
**      txa
**      adc     yyy
**      tax
**      tya
**      ldy     zzz
**      jsr     ldaxidx
**
** and replace it by:
**
**      stx     tmp1
**      asl     a
**      rol     tmp1
**      clc
**      adc     xxx
**      sta     ptr1
**      lda     tmp1
**      adc     yyy
**      sta     ptr1+1
**      ldy     zzz
**      lda     (ptr1),y
**      tax
**      dey
**      lda     (ptr1),y
*/
{
    unsigned Changes = 0;
    unsigned I;

    /* Walk over the entries */
    I = 0;
    while (I < CS_GetEntryCount (S)) {

        CodeEntry* L[10];

        /* Get next entry */
        L[0] = CS_GetEntry (S, I);

        /* Check for the sequence */
        if (L[0]->OPC == OP65_JSR                               &&
            (strcmp (L[0]->Arg, "aslax1") == 0          ||
             strcmp (L[0]->Arg, "shlax1") == 0)                 &&
            CS_GetEntries (S, L+1, I+1, 9)                      &&
            L[1]->OPC == OP65_CLC                               &&
            L[2]->OPC == OP65_ADC                               &&
            L[3]->OPC == OP65_TAY                               &&
            L[4]->OPC == OP65_TXA                               &&
            L[5]->OPC == OP65_ADC                               &&
            L[6]->OPC == OP65_TAX                               &&
            L[7]->OPC == OP65_TYA                               &&
            L[8]->OPC == OP65_LDY                               &&
            CE_IsCallTo (L[9], "ldaxidx")                       &&
            !CS_RangeHasLabel (S, I+1, 9)) {

            CodeEntry* X;

            /* Track the insertion point */
            unsigned IP = I + 10;


            /* If X is zero on entry to aslax1, we can generate:
            **
            **      asl     a
            **      bcc     L1
            **      inx
            ** L1:  clc
            **
            ** instead of the code above. "lda tmp1" needs to be changed
            ** to "txa" in this case.
            */
            int ShortCode = (L[0]->RI->In.RegX == 0);

            if (ShortCode) {

                CodeLabel* Lab;

                /* asl a */
                X = NewCodeEntry (OP65_ASL, AM65_ACC, "a", 0, L[0]->LI);
                CS_InsertEntry (S, X, IP++);

                /* Generate clc first, since we need the label */
                X = NewCodeEntry (OP65_CLC, AM65_IMP, 0, 0, L[1]->LI);
                CS_InsertEntry (S, X, IP);

                /* Get the label */
                Lab = CS_GenLabel (S, X);

                /* bcc Lab */
                X = NewCodeEntry (OP65_BCC, AM65_BRA, Lab->Name, Lab, L[0]->LI);
                CS_InsertEntry (S, X, IP++);

                /* inx */
                X = NewCodeEntry (OP65_INX, AM65_IMP, 0, 0, L[0]->LI);
                CS_InsertEntry (S, X, IP++);

                /* Skip the clc insn */
                ++IP;

            } else {

                /* stx tmp1 */
                X = NewCodeEntry (OP65_STX, AM65_ZP, "tmp1", 0, L[0]->LI);
                CS_InsertEntry (S, X, IP++);

                /* asl a */
                X = NewCodeEntry (OP65_ASL, AM65_ACC, "a", 0, L[0]->LI);
                CS_InsertEntry (S, X, IP++);

                /* rol tmp1 */
                X = NewCodeEntry (OP65_ROL, AM65_ZP, "tmp1", 0, L[0]->LI);
                CS_InsertEntry (S, X, IP++);

                /* clc */
                X = NewCodeEntry (OP65_CLC, AM65_IMP, 0, 0, L[1]->LI);
                CS_InsertEntry (S, X, IP++);

            }

            /* adc xxx */
            X = NewCodeEntry (L[2]->OPC, L[2]->AM, L[2]->Arg, 0, L[2]->LI);
            CS_InsertEntry (S, X, IP++);

            /* sta ptr1 */
            X = NewCodeEntry (OP65_STA, AM65_ZP, "ptr1", 0, L[9]->LI);
            CS_InsertEntry (S, X, IP++);

            if (ShortCode) {
                /* txa */
                X = NewCodeEntry (OP65_TXA, AM65_IMP, 0, 0, L[4]->LI);
            } else {
                /* lda tmp1 */
                X = NewCodeEntry (OP65_LDA, AM65_ZP, "tmp1", 0, L[4]->LI);
            }
            CS_InsertEntry (S, X, IP++);

            /* adc xxx */
            X = NewCodeEntry (L[5]->OPC, L[5]->AM, L[5]->Arg, 0, L[5]->LI);
            CS_InsertEntry (S, X, IP++);

            /* sta ptr1+1 */
            X = NewCodeEntry (OP65_STA, AM65_ZP, "ptr1+1", 0, L[9]->LI);
            CS_InsertEntry (S, X, IP++);

            /* ldy zzz */
            X = NewCodeEntry (L[8]->OPC, L[8]->AM, L[8]->Arg, 0, L[8]->LI);
            CS_InsertEntry (S, X, IP++);

            /* lda (ptr1),y */
            X = NewCodeEntry (OP65_LDA, AM65_ZP_INDY, "ptr1", 0, L[9]->LI);
            CS_InsertEntry (S, X, IP++);

            /* tax */
            X = NewCodeEntry (OP65_TAX, AM65_IMP, 0, 0, L[9]->LI);
            CS_InsertEntry (S, X, IP++);

            /* dey */
            X = NewCodeEntry (OP65_DEY, AM65_IMP, 0, 0, L[9]->LI);
            CS_InsertEntry (S, X, IP++);

            /* lda (ptr1),y */
            X = NewCodeEntry (OP65_LDA, AM65_ZP_INDY, "ptr1", 0, L[9]->LI);
            CS_InsertEntry (S, X, IP++);

            /* Remove the old code */
            CS_DelEntries (S, I, 10);

            /* Remember, we had changes */
            ++Changes;

        }

        /* Next entry */
        ++I;

    }

    /* Return the number of changes made */
    return Changes;
}



unsigned OptPtrLoad11 (CodeSeg* S)
/* Search for the sequence:
**
**      clc
**      adc     xxx
**      bcc     L
**      inx
** L:   ldy     #$00
**      jsr     ldauidx
**
** and replace it by:
**
**      ldy     xxx
**      sta     ptr1
**      stx     ptr1+1
**      ldx     #$00
**      lda     (ptr1),y
*/
{
    unsigned Changes = 0;

    /* Walk over the entries */
    unsigned I = 0;
    while (I < CS_GetEntryCount (S)) {

        CodeEntry* L[6];

        /* Get next entry */
        L[0] = CS_GetEntry (S, I);

        /* Check for the sequence */
        if (L[0]->OPC == OP65_CLC                            &&
            CS_GetEntries (S, L+1, I+1, 5)                   &&
            L[1]->OPC == OP65_ADC                            &&
            (L[1]->AM == AM65_ABS || L[1]->AM == AM65_ZP || L[1]->AM == AM65_IMM)    &&
            (L[2]->OPC == OP65_BCC || L[2]->OPC == OP65_JCC) &&
            L[2]->JumpTo != 0                                &&
            L[2]->JumpTo->Owner == L[4]                      &&
            L[3]->OPC == OP65_INX                            &&
            L[4]->OPC == OP65_LDY                            &&
            CE_IsKnownImm (L[4], 0)                          &&
            CE_IsCallTo (L[5], "ldauidx")                    &&
            !CS_RangeHasLabel (S, I+1, 3)                    &&
            !CE_HasLabel (L[5])) {

            CodeEntry* X;

            /* We will create all the new stuff behind the current one so
            ** we keep the line references.
            */
            X = NewCodeEntry (OP65_LDY, L[1]->AM, L[1]->Arg, 0, L[0]->LI);
            CS_InsertEntry (S, X, I+6);

            /* sta ptr1 */
            X = NewCodeEntry (OP65_STA, AM65_ZP, "ptr1", 0, L[0]->LI);
            CS_InsertEntry (S, X, I+7);

            /* stx ptr1+1 */
            X = NewCodeEntry (OP65_STX, AM65_ZP, "ptr1+1", 0, L[0]->LI);
            CS_InsertEntry (S, X, I+8);

            /* ldx #$00 */
            X = NewCodeEntry (OP65_LDX, AM65_IMM, "$00", 0, L[0]->LI);
            CS_InsertEntry (S, X, I+9);

            X = NewCodeEntry (OP65_LDA, AM65_ZP_INDY, "ptr1", 0, L[0]->LI);
            CS_InsertEntry (S, X, I+10);

            /* Remove the old code */
            CS_DelEntries (S, I, 6);

            /* Remember, we had changes */
            ++Changes;

        }

        /* Next entry */
        ++I;

    }

    /* Return the number of changes made */
    return Changes;
}



unsigned OptPtrLoad12 (CodeSeg* S)
/* Search for the sequence:
**
**      lda     regbank+n
**      ldx     regbank+n+1
**      sta     regsave
**      stx     regsave+1
**      clc
**      adc     #$01
**      bcc     L0005
**      inx
** L:   sta     regbank+n
**      stx     regbank+n+1
**      lda     regsave
**      ldx     regsave+1
**      ldy     #$00
**      jsr     ldauidx
**
** and replace it by:
**
**      ldy     #$00
**      ldx     #$00
**      lda     (regbank+n),y
**      inc     regbank+n
**      bne     L1
**      inc     regbank+n+1
** L1:  tay                     <- only if flags are used
**
** This function must execute before OptPtrLoad7!
**
*/
{
    unsigned Changes = 0;

    /* Walk over the entries */
    unsigned I = 0;
    while (I < CS_GetEntryCount (S)) {

        CodeEntry* L[15];
        unsigned Len;

        /* Get next entry */
        L[0] = CS_GetEntry (S, I);

        /* Check for the sequence */
        if (L[0]->OPC == OP65_LDA                               &&
            L[0]->AM == AM65_ZP                                 &&
            strncmp (L[0]->Arg, "regbank+", 8) == 0             &&
            (Len = strlen (L[0]->Arg)) > 0                      &&
            CS_GetEntries (S, L+1, I+1, 14)                     &&
            !CS_RangeHasLabel (S, I+1, 7)                       &&
            !CS_RangeHasLabel (S, I+9, 5)                       &&
            L[1]->OPC == OP65_LDX                               &&
            L[1]->AM == AM65_ZP                                 &&
            strncmp (L[1]->Arg, L[0]->Arg, Len) == 0            &&
            strcmp (L[1]->Arg+Len, "+1") == 0                   &&
            L[2]->OPC == OP65_STA                               &&
            L[2]->AM == AM65_ZP                                 &&
            strcmp (L[2]->Arg, "regsave") == 0                  &&
            L[3]->OPC == OP65_STX                               &&
            L[3]->AM == AM65_ZP                                 &&
            strcmp (L[3]->Arg, "regsave+1") == 0                &&
            L[4]->OPC == OP65_CLC                               &&
            L[5]->OPC == OP65_ADC                               &&
            CE_IsKnownImm (L[5], 1)                             &&
            L[6]->OPC == OP65_BCC                               &&
            L[6]->JumpTo != 0                                   &&
            L[6]->JumpTo->Owner == L[8]                         &&
            L[7]->OPC == OP65_INX                               &&
            L[8]->OPC == OP65_STA                               &&
            L[8]->AM == AM65_ZP                                 &&
            strcmp (L[8]->Arg, L[0]->Arg) == 0                  &&
            L[9]->OPC == OP65_STX                               &&
            L[9]->AM == AM65_ZP                                 &&
            strcmp (L[9]->Arg, L[1]->Arg) == 0                  &&
            L[10]->OPC == OP65_LDA                              &&
            L[10]->AM == AM65_ZP                                &&
            strcmp (L[10]->Arg, "regsave") == 0                 &&
            L[11]->OPC == OP65_LDX                              &&
            L[11]->AM == AM65_ZP                                &&
            strcmp (L[11]->Arg, "regsave+1") == 0               &&
            L[12]->OPC == OP65_LDY                              &&
            CE_IsConstImm (L[12])                               &&
            CE_IsCallTo (L[13], "ldauidx")) {

            CodeEntry* X;
            CodeLabel* Label;

            /* Check if the instruction following the sequence uses the flags
            ** set by the load. If so, insert a test of the value in the
            ** accumulator.
            */
            if (CE_UseLoadFlags (L[14])) {
                X = NewCodeEntry (OP65_TAY, AM65_IMP, 0, 0, L[13]->LI);
                CS_InsertEntry (S, X, I+14);
            }

            /* Attach a label to L[14]. This may be either the just inserted
            ** instruction, or the one following the sequence.
            */
            Label = CS_GenLabel (S, L[14]);

            /* ldy #$xx */
            X = NewCodeEntry (OP65_LDY, AM65_IMM, L[12]->Arg, 0, L[12]->LI);
            CS_InsertEntry (S, X, I+14);

            /* ldx #$xx */
            X = NewCodeEntry (OP65_LDX, AM65_IMM, "$00", 0, L[13]->LI);
            CS_InsertEntry (S, X, I+15);

            /* lda (regbank+n),y */
            X = NewCodeEntry (OP65_LDA, AM65_ZP_INDY, L[0]->Arg, 0, L[13]->LI);
            CS_InsertEntry (S, X, I+16);

            /* inc regbank+n */
            X = NewCodeEntry (OP65_INC, AM65_ZP, L[0]->Arg, 0, L[5]->LI);
            CS_InsertEntry (S, X, I+17);

            /* bne ... */
            X = NewCodeEntry (OP65_BNE, AM65_BRA, Label->Name, Label, L[6]->LI);
            CS_InsertEntry (S, X, I+18);

            /* inc regbank+n+1 */
            X = NewCodeEntry (OP65_INC, AM65_ZP, L[1]->Arg, 0, L[7]->LI);
            CS_InsertEntry (S, X, I+19);

            /* Delete the old code */
            CS_DelEntries (S, I, 14);

            /* Remember, we had changes */
            ++Changes;

        }

        /* Next entry */
        ++I;

    }

    /* Return the number of changes made */
    return Changes;
}



unsigned OptPtrLoad13 (CodeSeg* S)
/* Search for the sequence:
**
**      lda     zp
**      ldx     zp+1
**      ldy     xx
**      jsr     ldauidx
**
** and replace it by:
**
**      ldy     xx
**      ldx     #$00
**      lda     (zp),y
*/
{
    unsigned Changes = 0;

    /* Walk over the entries */
    unsigned I = 0;
    while (I < CS_GetEntryCount (S)) {

        CodeEntry* L[4];
        unsigned Len;

        /* Get next entry */
        L[0] = CS_GetEntry (S, I);

        /* Check for the sequence */
        if (L[0]->OPC == OP65_LDA && L[0]->AM == AM65_ZP        &&
            CS_GetEntries (S, L+1, I+1, 3)                      &&
            !CS_RangeHasLabel (S, I+1, 3)                       &&
            L[1]->OPC == OP65_LDX && L[1]->AM == AM65_ZP        &&
            (Len = strlen (L[0]->Arg)) > 0                      &&
            strncmp (L[0]->Arg, L[1]->Arg, Len) == 0            &&
            strcmp (L[1]->Arg + Len, "+1") == 0                 &&
            L[2]->OPC == OP65_LDY                               &&
            CE_IsCallTo (L[3], "ldauidx")) {

            CodeEntry* X;

            /* ldx #$00 */
            X = NewCodeEntry (OP65_LDX, AM65_IMM, "$00", 0, L[3]->LI);
            CS_InsertEntry (S, X, I+3);

            /* lda (zp),y */
            X = NewCodeEntry (OP65_LDA, AM65_ZP_INDY, L[0]->Arg, 0, L[3]->LI);
            CS_InsertEntry (S, X, I+4);

            /* Remove the old code */
            CS_DelEntry (S, I+5);
            CS_DelEntries (S, I, 2);

            /* Remember, we had changes */
            ++Changes;

        }

        /* Next entry */
        ++I;

    }

    /* Return the number of changes made */
    return Changes;
}



unsigned OptPtrLoad14 (CodeSeg* S)
/* Search for the sequence:
**
**      lda     zp
**      ldx     zp+1
**      (anything that doesn't change a/x)
**      ldy     xx
**      jsr     ldauidx
**
** and replace it by:
**
**      lda     zp
**      ldx     zp+1
**      (anything that doesn't change a/x)
**      ldy     xx
**      ldx     #$00
**      lda     (zp),y
*/
{
    unsigned Changes = 0;
    unsigned I;

    /* Walk over the entries */
    I = 0;
    while (I < CS_GetEntryCount (S)) {

        CodeEntry* L[5];
        unsigned Len;

        /* Get next entry */
        L[0] = CS_GetEntry (S, I);

        /* Check for the sequence */
        if (L[0]->OPC == OP65_LDA && L[0]->AM == AM65_ZP        &&
            CS_GetEntries (S, L+1, I+1, 4)                      &&
            !CS_RangeHasLabel (S, I+1, 4)                       &&
            L[1]->OPC == OP65_LDX && L[1]->AM == AM65_ZP        &&
            (Len = strlen (L[0]->Arg)) > 0                      &&
            strncmp (L[0]->Arg, L[1]->Arg, Len) == 0            &&
            strcmp (L[1]->Arg + Len, "+1") == 0                 &&
            (L[2]->Chg & REG_AX) == 0                           &&
            L[3]->OPC == OP65_LDY                               &&
            CE_IsCallTo (L[4], "ldauidx")) {

            CodeEntry* X;

            /* ldx #$00 */
            X = NewCodeEntry (OP65_LDX, AM65_IMM, "$00", 0, L[3]->LI);
            CS_InsertEntry (S, X, I+5);

            /* lda (zp),y */
            X = NewCodeEntry (OP65_LDA, AM65_ZP_INDY, L[0]->Arg, 0, L[3]->LI);
            CS_InsertEntry (S, X, I+6);

            /* Remove the old code */
            CS_DelEntry (S, I+4);

            /* Remember, we had changes */
            ++Changes;

        }

        /* Next entry */
        ++I;

    }

    /* Return the number of changes made */
    return Changes;
}



unsigned OptPtrLoad15 (CodeSeg* S)
/* Search for the sequence:
**
**      lda     zp
**      ldx     zp+1
**      jsr     pushax          <- optional
**      ldy     xx
**      jsr     ldaxidx
**
** and replace it by:
**
**      lda     zp              <- only if
**      ldx     zp+1            <- call to
**      jsr     pushax          <- pushax present
**      ldy     xx
**      lda     (zp),y
**      tax
**      dey
**      lda     (zp),y
*/
{
    unsigned Changes = 0;

    /* Walk over the entries */
    unsigned I = 0;
    while (I < CS_GetEntryCount (S)) {

        CodeEntry* L[5];
        unsigned Len;

        /* Check for the start of the sequence */
        if (CS_GetEntries (S, L, I, 3)                          &&
            L[0]->OPC == OP65_LDA && L[0]->AM == AM65_ZP        &&
            L[1]->OPC == OP65_LDX && L[1]->AM == AM65_ZP        &&
            !CS_RangeHasLabel (S, I+1, 2)                       &&
            (Len = strlen (L[0]->Arg)) > 0                      &&
            strncmp (L[0]->Arg, L[1]->Arg, Len) == 0            &&
            strcmp (L[1]->Arg + Len, "+1") == 0) {

            unsigned PushAX = CE_IsCallTo (L[2], "pushax");

            /* Check for the remainder of the sequence */
            if (CS_GetEntries (S, L+3, I+3, 1 + PushAX)         &&
                !CS_RangeHasLabel (S, I+3, 1 + PushAX)          &&
                L[2+PushAX]->OPC == OP65_LDY                    &&
                CE_IsCallTo (L[3+PushAX], "ldaxidx")) {

                CodeEntry* X;

                /* lda (zp),y */
                X = NewCodeEntry (OP65_LDA, AM65_ZP_INDY, L[0]->Arg, 0, L[3]->LI);
                CS_InsertEntry (S, X, I+PushAX+4);

                /* tax */
                X = NewCodeEntry (OP65_TAX, AM65_IMP, 0, 0, L[3]->LI);
                CS_InsertEntry (S, X, I+PushAX+5);

                /* dey */
                X = NewCodeEntry (OP65_DEY, AM65_IMP, 0, 0, L[3]->LI);
                CS_InsertEntry (S, X, I+PushAX+6);

                /* lda (zp),y */
                X = NewCodeEntry (OP65_LDA, AM65_ZP_INDY, L[0]->Arg, 0, L[3]->LI);
                CS_InsertEntry (S, X, I+PushAX+7);

                /* Remove the old code */
                CS_DelEntry (S, I+PushAX+3);
                if (!PushAX) {
                    CS_DelEntries (S, I, 2);
                }

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



unsigned OptPtrLoad16 (CodeSeg* S)
/* Search for the sequence
**
**      ldy     ...
**      jsr     ldauidx
**
** and replace it by:
**
**      stx     ptr1+1
**      sta     ptr1
**      ldy     ...
**      ldx     #$00
**      lda     (ptr1),y
**
** This step must be executed *after* OptPtrLoad1!
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
        if (L[0]->OPC == OP65_LDY               &&
            CS_GetEntries (S, L+1, I+1, 1)      &&
            CE_IsCallTo (L[1], "ldauidx")       &&
            !CE_HasLabel (L[1])) {

            CodeEntry* X;

            /* stx ptr1+1 */
            X = NewCodeEntry (OP65_STA, AM65_ZP, "ptr1", 0, L[1]->LI);
            CS_InsertEntry (S, X, I+2);

            /* sta ptr1 */
            X = NewCodeEntry (OP65_STX, AM65_ZP, "ptr1+1", 0, L[1]->LI);
            CS_InsertEntry (S, X, I+3);

            /* ldy ... */
            X = NewCodeEntry (L[0]->OPC, L[0]->AM, L[0]->Arg, 0, L[0]->LI);
            CS_InsertEntry (S, X, I+4);

            /* ldx #$00 */
            X = NewCodeEntry (OP65_LDX, AM65_IMM, "$00", 0, L[1]->LI);
            CS_InsertEntry (S, X, I+5);

            /* lda (ptr1),y */
            X = NewCodeEntry (OP65_LDA, AM65_ZP_INDY, "ptr1", 0, L[1]->LI);
            CS_InsertEntry (S, X, I+6);

            /* Delete the old code */
            CS_DelEntries (S, I, 2);

            /* Remember, we had changes */
            ++Changes;

        }

        /* Next entry */
        ++I;

    }

    /* Return the number of changes made */
    return Changes;
}



unsigned OptPtrLoad17 (CodeSeg* S)
/* Search for the sequence
**
**      ldy     ...
**      jsr     ldaxidx
**
** and replace it by:
**
**      sta     ptr1
**      stx     ptr1+1
**      ldy     ...
**      lda     (ptr1),y
**      tax
**      dey
**      lda     (ptr1),y
**
** This step must be executed *after* OptPtrLoad9! While code size increases
** by more than 200%, inlining will greatly improve visibility for the
** optimizer, so often part of the code gets improved later. So we will mark
** the step with less than 200% so it gets executed when -Oi is in effect.
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
        if (L[0]->OPC == OP65_LDY               &&
            CS_GetEntries (S, L+1, I+1, 1)      &&
            CE_IsCallTo (L[1], "ldaxidx")       &&
            !CE_HasLabel (L[1])) {

            CodeEntry* X;

            /* Store the high byte */
            X = NewCodeEntry (OP65_STA, AM65_ZP, "ptr1", 0, L[0]->LI);
            CS_InsertEntry (S, X, I+2);

            /* Store the low byte */
            X = NewCodeEntry (OP65_STX, AM65_ZP, "ptr1+1", 0, L[0]->LI);
            CS_InsertEntry (S, X, I+3);

            /* ldy ... */
            X = NewCodeEntry (L[0]->OPC, L[0]->AM, L[0]->Arg, 0, L[0]->LI);
            CS_InsertEntry (S, X, I+4);

            /* lda (ptr1),y */
            X = NewCodeEntry (OP65_LDA, AM65_ZP_INDY, "ptr1", 0, L[1]->LI);
            CS_InsertEntry (S, X, I+5);

            /* tax */
            X = NewCodeEntry (OP65_TAX, AM65_IMP, 0, 0, L[1]->LI);
            CS_InsertEntry (S, X, I+6);

            /* dey */
            X = NewCodeEntry (OP65_DEY, AM65_IMP, 0, 0, L[1]->LI);
            CS_InsertEntry (S, X, I+7);

            /* lda (ptr1),y */
            X = NewCodeEntry (OP65_LDA, AM65_ZP_INDY, "ptr1", 0, L[1]->LI);
            CS_InsertEntry (S, X, I+8);

            /* Delete original sequence */
            CS_DelEntries (S, I, 2);

            /* Remember, we had changes */
            ++Changes;

        }

        /* Next entry */
        ++I;

    }

    /* Return the number of changes made */
    return Changes;
}
