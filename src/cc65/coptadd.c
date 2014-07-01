/*****************************************************************************/
/*                                                                           */
/*                                 coptadd.c                                 */
/*                                                                           */
/*                        Optimize addition sequences                        */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2001-2005, Ullrich von Bassewitz                                      */
/*                Römerstrasse 52                                            */
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



/* common */
#include "chartype.h"

/* cc65 */
#include "codeent.h"
#include "codeinfo.h"
#include "coptadd.h"



/*****************************************************************************/
/*                            Optimize additions                             */
/*****************************************************************************/



unsigned OptAdd1 (CodeSeg* S)
/* Search for the sequence
**
**      ldy     #xx
**      jsr     ldaxysp
**      jsr     pushax
**      ldy     #yy
**      jsr     ldaxysp
**      jsr     tosaddax
**
** and replace it by:
**
**      ldy     #xx-1
**      lda     (sp),y
**      ldy     #yy-3
**      clc
**      adc     (sp),y
**      pha
**      ldy     #xx
**      lda     (sp),y
**      ldy     #yy-2
**      adc     (sp),y
**      tax
**      pla
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
        if (L[0]->OPC == OP65_LDY            &&
            CE_IsConstImm (L[0])             &&
            !CS_RangeHasLabel (S, I+1, 5)    &&
            CS_GetEntries (S, L+1, I+1, 5)   &&
            CE_IsCallTo (L[1], "ldaxysp")    &&
            CE_IsCallTo (L[2], "pushax")     &&
            L[3]->OPC == OP65_LDY            &&
            CE_IsConstImm (L[3])             &&
            CE_IsCallTo (L[4], "ldaxysp")    &&
            CE_IsCallTo (L[5], "tosaddax")) {

            CodeEntry* X;
            const char* Arg;

            /* Correct the stack of the first Y register load */
            CE_SetNumArg (L[0], L[0]->Num - 1);

            /* lda (sp),y */
            X = NewCodeEntry (OP65_LDA, AM65_ZP_INDY, "sp", 0, L[1]->LI);
            CS_InsertEntry (S, X, I+1);

            /* ldy #yy-3 */
            Arg = MakeHexArg (L[3]->Num - 3);
            X = NewCodeEntry (OP65_LDY, AM65_IMM, Arg, 0, L[4]->LI);
            CS_InsertEntry (S, X, I+2);

            /* clc */
            X = NewCodeEntry (OP65_CLC, AM65_IMP, 0, 0, L[5]->LI);
            CS_InsertEntry (S, X, I+3);

            /* adc (sp),y */
            X = NewCodeEntry (OP65_ADC, AM65_ZP_INDY, "sp", 0, L[5]->LI);
            CS_InsertEntry (S, X, I+4);

            /* pha */
            X = NewCodeEntry (OP65_PHA, AM65_IMP, 0, 0, L[5]->LI);
            CS_InsertEntry (S, X, I+5);

            /* ldy #xx (beware: L[0] has changed) */
            Arg = MakeHexArg (L[0]->Num + 1);
            X = NewCodeEntry (OP65_LDY, AM65_IMM, Arg, 0, L[1]->LI);
            CS_InsertEntry (S, X, I+6);

            /* lda (sp),y */
            X = NewCodeEntry (OP65_LDA, AM65_ZP_INDY, "sp", 0, L[1]->LI);
            CS_InsertEntry (S, X, I+7);

            /* ldy #yy-2 */
            Arg = MakeHexArg (L[3]->Num - 2);
            X = NewCodeEntry (OP65_LDY, AM65_IMM, Arg, 0, L[4]->LI);
            CS_InsertEntry (S, X, I+8);

            /* adc (sp),y */
            X = NewCodeEntry (OP65_ADC, AM65_ZP_INDY, "sp", 0, L[5]->LI);
            CS_InsertEntry (S, X, I+9);

            /* tax */
            X = NewCodeEntry (OP65_TAX, AM65_IMP, 0, 0, L[5]->LI);
            CS_InsertEntry (S, X, I+10);

            /* pla */
            X = NewCodeEntry (OP65_PLA, AM65_IMP, 0, 0, L[5]->LI);
            CS_InsertEntry (S, X, I+11);

            /* Delete the old code */
            CS_DelEntries (S, I+12, 5);

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
**
**      ldy     #xx
**      jsr     ldaxysp
**      ldy     #yy
**      jsr     addeqysp
**
** and replace it by:
**
**      ldy     #xx-1
**      lda     (sp),y
**      ldy     #yy
**      clc
**      adc     (sp),y
**      sta     (sp),y
**      ldy     #xx
**      lda     (sp),y
**      ldy     #yy+1
**      adc     (sp),y
**      sta     (sp),y
**
** provided that a/x is not used later.
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
        if (L[0]->OPC == OP65_LDY               &&
            CE_IsConstImm (L[0])                &&
            !CS_RangeHasLabel (S, I+1, 3)       &&
            CS_GetEntries (S, L+1, I+1, 3)      &&
            CE_IsCallTo (L[1], "ldaxysp")       &&
            L[2]->OPC == OP65_LDY               &&
            CE_IsConstImm (L[2])                &&
            CE_IsCallTo (L[3], "addeqysp")      &&
            (GetRegInfo (S, I+4, REG_AX) & REG_AX) == 0) {

            /* Insert new code behind the addeqysp */
            const char* Arg;
            CodeEntry* X;

            /* ldy     #xx-1 */
            Arg = MakeHexArg (L[0]->Num-1);
            X = NewCodeEntry (OP65_LDY, AM65_IMM, Arg, 0, L[0]->LI);
            CS_InsertEntry (S, X, I+4);

            /* lda     (sp),y */
            X = NewCodeEntry (OP65_LDA, AM65_ZP_INDY, "sp", 0, L[1]->LI);
            CS_InsertEntry (S, X, I+5);

            /* ldy     #yy */
            X = NewCodeEntry (OP65_LDY, AM65_IMM, L[2]->Arg, 0, L[2]->LI);
            CS_InsertEntry (S, X, I+6);

            /* clc */
            X = NewCodeEntry (OP65_CLC, AM65_IMP, 0, 0, L[3]->LI);
            CS_InsertEntry (S, X, I+7);

            /* adc     (sp),y */
            X = NewCodeEntry (OP65_ADC, AM65_ZP_INDY, "sp", 0, L[3]->LI);
            CS_InsertEntry (S, X, I+8);

            /* sta     (sp),y */
            X = NewCodeEntry (OP65_STA, AM65_ZP_INDY, "sp", 0, L[3]->LI);
            CS_InsertEntry (S, X, I+9);

            /* ldy     #xx */
            X = NewCodeEntry (OP65_LDY, AM65_IMM, L[0]->Arg, 0, L[0]->LI);
            CS_InsertEntry (S, X, I+10);

            /* lda     (sp),y */
            X = NewCodeEntry (OP65_LDA, AM65_ZP_INDY, "sp", 0, L[1]->LI);
            CS_InsertEntry (S, X, I+11);

            /* ldy     #yy+1 */
            Arg = MakeHexArg (L[2]->Num+1);
            X = NewCodeEntry (OP65_LDY, AM65_IMM, Arg, 0, L[2]->LI);
            CS_InsertEntry (S, X, I+12);

            /* adc     (sp),y */
            X = NewCodeEntry (OP65_ADC, AM65_ZP_INDY, "sp", 0, L[3]->LI);
            CS_InsertEntry (S, X, I+13);

            /* sta     (sp),y */
            X = NewCodeEntry (OP65_STA, AM65_ZP_INDY, "sp", 0, L[3]->LI);
            CS_InsertEntry (S, X, I+14);

            /* Delete the old code */
            CS_DelEntries (S, I, 4);

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
**
**      jsr     pushax
**      ldx     #$00
**      lda     xxx
**      jsr     tosaddax
**
** and replace it by
**
**      clc
**      adc     xxx
**      bcc     L1
**      inx
** L1:
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
        if (CE_IsCallTo (L[0], "pushax")                        &&
            CS_GetEntries (S, L+1, I+1, 4)                      &&
            !CS_RangeHasLabel (S, I+1, 3)                       &&
            L[1]->OPC == OP65_LDX                               &&
            CE_IsKnownImm (L[1], 0)                             &&
            L[2]->OPC == OP65_LDA                               &&
            CE_IsCallTo (L[3], "tosaddax")) {

            CodeEntry* X;
            CodeLabel* Label;

            /* Insert new code behind the sequence */
            X = NewCodeEntry (OP65_CLC, AM65_IMP, 0, 0, L[3]->LI);
            CS_InsertEntry (S, X, I+4);

            /* adc xxx */
            X = NewCodeEntry (OP65_ADC, L[2]->AM, L[2]->Arg, 0, L[3]->LI);
            CS_InsertEntry (S, X, I+5);

            /* bcc L1 */
            Label = CS_GenLabel (S, L[4]);
            X = NewCodeEntry (OP65_BCC, AM65_BRA, Label->Name, Label, L[3]->LI);
            CS_InsertEntry (S, X, I+6);

            /* inx */
            X = NewCodeEntry (OP65_INX, AM65_IMP, 0, 0, L[3]->LI);
            CS_InsertEntry (S, X, I+7);

            /* Delete the old code */
            CS_DelEntries (S, I, 4);

            /* Remember, we had changes */
            ++Changes;

        }

        /* Next entry */
        ++I;

    }

    /* Return the number of changes made */
    return Changes;
}



unsigned OptAdd4 (CodeSeg* S)
/* Search for the sequence
**
**      jsr     pushax
**      lda     xxx
**      ldx     yyy
**      jsr     tosaddax
**
** and replace it by
**
**      clc
**      adc     xxx
**      pha
**      txa
**      adc     yyy
**      tax
**      pla
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
        if (CE_IsCallTo (L[0], "pushax")                        &&
            CS_GetEntries (S, L+1, I+1, 3)                      &&
            !CS_RangeHasLabel (S, I+1, 3)                       &&
            L[1]->OPC == OP65_LDA                               &&
            (L[1]->AM == AM65_ABS || L[1]->AM == AM65_ZP)       &&
            L[2]->OPC == OP65_LDX                               &&
            (L[2]->AM == AM65_ABS || L[2]->AM == AM65_ZP)       &&
            CE_IsCallTo (L[3], "tosaddax")) {

            CodeEntry* X;

            /* Insert new code behind the sequence */
            X = NewCodeEntry (OP65_CLC, AM65_IMP, 0, 0, L[3]->LI);
            CS_InsertEntry (S, X, I+4);

            /* adc xxx */
            X = NewCodeEntry (OP65_ADC, L[1]->AM, L[1]->Arg, 0, L[3]->LI);
            CS_InsertEntry (S, X, I+5);

            /* pha */
            X = NewCodeEntry (OP65_PHA, AM65_IMP, 0, 0, L[3]->LI);
            CS_InsertEntry (S, X, I+6);

            /* txa */
            X = NewCodeEntry (OP65_TXA, AM65_IMP, 0, 0, L[3]->LI);
            CS_InsertEntry (S, X, I+7);

            /* adc yyy */
            X = NewCodeEntry (OP65_ADC, L[2]->AM, L[2]->Arg, 0, L[3]->LI);
            CS_InsertEntry (S, X, I+8);

            /* tax */
            X = NewCodeEntry (OP65_TAX, AM65_IMP, 0, 0, L[3]->LI);
            CS_InsertEntry (S, X, I+9);

            /* pla */
            X = NewCodeEntry (OP65_PLA, AM65_IMP, 0, 0, L[3]->LI);
            CS_InsertEntry (S, X, I+10);

            /* Delete the old code */
            CS_DelEntries (S, I, 4);

            /* Remember, we had changes */
            ++Changes;

        }

        /* Next entry */
        ++I;

    }

    /* Return the number of changes made */
    return Changes;
}



unsigned OptAdd5 (CodeSeg* S)
/* Search for a call to incaxn and replace it by an 8 bit add if the X register
** is not used later.
*/
{
    unsigned Changes = 0;

    /* Walk over the entries */
    unsigned I = 0;
    while (I < CS_GetEntryCount (S)) {

        CodeEntry* E;

        /* Get next entry */
        E = CS_GetEntry (S, I);

        /* Check for the sequence */
        if (E->OPC == OP65_JSR                          &&
            strncmp (E->Arg, "incax", 5) == 0           &&
            IsDigit (E->Arg[5])                         &&
            E->Arg[6] == '\0'                           &&
            !RegXUsed (S, I+1)) {

            CodeEntry* X;
            const char* Arg;

            /* Insert new code behind the sequence */
            X = NewCodeEntry (OP65_CLC, AM65_IMP, 0, 0, E->LI);
            CS_InsertEntry (S, X, I+1);

            Arg = MakeHexArg (E->Arg[5] - '0');
            X = NewCodeEntry (OP65_ADC, AM65_IMM, Arg, 0, E->LI);
            CS_InsertEntry (S, X, I+2);

            /* Delete the old code */
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



unsigned OptAdd6 (CodeSeg* S)
/* Search for the sequence
**
**      adc     ...
**      bcc     L
**      inx
** L:
**
** and remove the handling of the high byte if X is not used later.
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
        if (E->OPC == OP65_ADC                               &&
            CS_GetEntries (S, L, I+1, 3)                     &&
            (L[0]->OPC == OP65_BCC || L[0]->OPC == OP65_JCC) &&
            L[0]->JumpTo != 0                                &&
            !CE_HasLabel (L[0])                              &&
            L[1]->OPC == OP65_INX                            &&
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
