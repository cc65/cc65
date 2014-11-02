/*****************************************************************************/
/*                                                                           */
/*                                 coptneg.c                                 */
/*                                                                           */
/*                        Optimize negation sequences                        */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2001-2012, Ullrich von Bassewitz                                      */
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



/* cc65 */
#include "codeent.h"
#include "codeinfo.h"
#include "coptneg.h"



/*****************************************************************************/
/*                            bnega optimizations                            */
/*****************************************************************************/



unsigned OptBNegA1 (CodeSeg* S)
/* Check for
**
**      ldx     #$00
**      lda     ..
**      jsr     bnega
**
** Remove the ldx if the lda does not use it.
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
        if (E->OPC == OP65_LDX                  &&
            E->AM == AM65_IMM                   &&
            (E->Flags & CEF_NUMARG) != 0        &&
            E->Num == 0                         &&
            CS_GetEntries (S, L, I+1, 2)        &&
            L[0]->OPC == OP65_LDA               &&
            (L[0]->Use & REG_X) == 0            &&
            !CE_HasLabel (L[0])                 &&
            CE_IsCallTo (L[1], "bnega")         &&
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



unsigned OptBNegA2 (CodeSeg* S)
/* Check for
**
**      lda     ..
**      jsr     bnega
**      jeq/jne ..
**
** Adjust the conditional branch and remove the call to the subroutine.
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
             E->OPC == OP65_ORA ||
             E->OPC == OP65_PLA ||
             E->OPC == OP65_SBC ||
             E->OPC == OP65_TXA ||
             E->OPC == OP65_TYA)                &&
            CS_GetEntries (S, L, I+1, 2)        &&
            CE_IsCallTo (L[0], "bnega")         &&
            !CE_HasLabel (L[0])                 &&
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
/*                            bnegax optimizations                           */
/*****************************************************************************/



unsigned OptBNegAX1 (CodeSeg* S)
/* On a call to bnegax, if X is zero, the result depends only on the value in
** A, so change the call to a call to bnega. This will get further optimized
** later if possible.
*/
{
    unsigned Changes = 0;
    unsigned I;

    /* Walk over the entries */
    I = 0;
    while (I < CS_GetEntryCount (S)) {

        /* Get next entry */
        CodeEntry* E = CS_GetEntry (S, I);

        /* Check if this is a call to bnegax, and if X is known and zero */
        if (E->RI->In.RegX == 0 && CE_IsCallTo (E, "bnegax")) {

            CodeEntry* X = NewCodeEntry (OP65_JSR, AM65_ABS, "bnega", 0, E->LI);
            CS_InsertEntry (S, X, I+1);
            CS_DelEntry (S, I);

            /* We had changes */
            ++Changes;
        }

        /* Next entry */
        ++I;

    }

    /* Return the number of changes made */
    return Changes;
}



unsigned OptBNegAX2 (CodeSeg* S)
/* Search for the sequence:
**
**      ldy     #xx
**      jsr     ldaxysp
**      jsr     bnegax
**      jne/jeq ...
**
** and replace it by
**
**      ldy     #xx
**      lda     (sp),y
**      dey
**      ora     (sp),y
**      jeq/jne ...
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
            CE_IsCallTo (L[2], "bnegax")        &&
            (L[3]->Info & OF_ZBRA) != 0) {

            CodeEntry* X;

            /* lda (sp),y */
            X = NewCodeEntry (OP65_LDA, AM65_ZP_INDY, "sp", 0, L[1]->LI);
            CS_InsertEntry (S, X, I+1);

            /* dey */
            X = NewCodeEntry (OP65_DEY, AM65_IMP, 0, 0, L[1]->LI);
            CS_InsertEntry (S, X, I+2);

            /* ora (sp),y */
            X = NewCodeEntry (OP65_ORA, AM65_ZP_INDY, "sp", 0, L[1]->LI);
            CS_InsertEntry (S, X, I+3);

            /* Invert the branch */
            CE_ReplaceOPC (L[3], GetInverseBranch (L[3]->OPC));

            /* Delete the entries no longer needed. */
            CS_DelEntries (S, I+4, 2);

            /* Remember, we had changes */
            ++Changes;

        }

        /* Next entry */
        ++I;

    }

    /* Return the number of changes made */
    return Changes;
}



unsigned OptBNegAX3 (CodeSeg* S)
/* Search for the sequence:
**
**      lda     xx
**      ldx     yy
**      jsr     bnegax
**      jne/jeq ...
**
** and replace it by
**
**      lda     xx
**      ora     xx+1
**      jeq/jne ...
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
        if (E->OPC == OP65_LDA                  &&
            CS_GetEntries (S, L, I+1, 3)        &&
            L[0]->OPC == OP65_LDX               &&
            !CE_HasLabel (L[0])                 &&
            CE_IsCallTo (L[1], "bnegax")        &&
            !CE_HasLabel (L[1])                 &&
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



unsigned OptBNegAX4 (CodeSeg* S)
/* Search for the sequence:
**
**      jsr     xxx
**      jsr     bnega(x)
**      jeq/jne ...
**
** and replace it by:
**
**      jsr     xxx
**      <boolean test>
**      jne/jeq ...
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
        if (E->OPC == OP65_JSR                  &&
            CS_GetEntries (S, L, I+1, 2)        &&
            L[0]->OPC == OP65_JSR               &&
            strncmp (L[0]->Arg,"bnega",5) == 0  &&
            !CE_HasLabel (L[0])                 &&
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
/*                            negax optimizations                            */
/*****************************************************************************/



unsigned OptNegAX1 (CodeSeg* S)
/* Search for a call to negax and replace it by
**
**      eor     #$FF
**      clc
**      adc     #$01
**
** if X isn't used later.
*/
{
    unsigned Changes = 0;
    unsigned I;

    /* Walk over the entries */
    I = 0;
    while (I < CS_GetEntryCount (S)) {

        /* Get next entry */
        CodeEntry* E = CS_GetEntry (S, I);

        /* Check if this is a call to negax, and if X isn't used later */
        if (CE_IsCallTo (E, "negax") && !RegXUsed (S, I+1)) {

            CodeEntry* X;

            /* Add replacement code behind */
            X = NewCodeEntry (OP65_EOR, AM65_IMM, "$FF", 0, E->LI);
            CS_InsertEntry (S, X, I+1);

            X = NewCodeEntry (OP65_CLC, AM65_IMP, 0, 0, E->LI);
            CS_InsertEntry (S, X, I+2);

            X = NewCodeEntry (OP65_ADC, AM65_IMM, "$01", 0, E->LI);
            CS_InsertEntry (S, X, I+3);

            /* Delete the call to negax */
            CS_DelEntry (S, I);

            /* Skip the generated code */
            I += 2;

            /* We had changes */
            ++Changes;
        }

        /* Next entry */
        ++I;

    }

    /* Return the number of changes made */
    return Changes;
}



unsigned OptNegAX2 (CodeSeg* S)
/* Search for a call to negax and replace it by
**
**      ldx     #$FF
**      eor     #$FF
**      clc
**      adc     #$01
**      bne     L1
**      inx
** L1:
**
** if X is known and zero on entry.
*/
{
    unsigned Changes = 0;
    unsigned I;

    /* Walk over the entries */
    I = 0;
    while (I < CS_GetEntryCount (S)) {

        CodeEntry* P;

        /* Get next entry */
        CodeEntry* E = CS_GetEntry (S, I);

        /* Check if this is a call to negax, and if X is known and zero */
        if (E->RI->In.RegX == 0                 &&
            CE_IsCallTo (E, "negax")            &&
            (P = CS_GetNextEntry (S, I)) != 0) {

            CodeEntry* X;
            CodeLabel* L;

            /* Add replacement code behind */

            /* ldx #$FF */
            X = NewCodeEntry (OP65_LDX, AM65_IMM, "$FF", 0, E->LI);
            CS_InsertEntry (S, X, I+1);

            /* eor #$FF */
            X = NewCodeEntry (OP65_EOR, AM65_IMM, "$FF", 0, E->LI);
            CS_InsertEntry (S, X, I+2);

            /* clc */
            X = NewCodeEntry (OP65_CLC, AM65_IMP, 0, 0, E->LI);
            CS_InsertEntry (S, X, I+3);

            /* adc #$01 */
            X = NewCodeEntry (OP65_ADC, AM65_IMM, "$01", 0, E->LI);
            CS_InsertEntry (S, X, I+4);

            /* Get the label attached to the insn following the call */
            L = CS_GenLabel (S, P);

            /* bne L */
            X = NewCodeEntry (OP65_BNE, AM65_BRA, L->Name, L, E->LI);
            CS_InsertEntry (S, X, I+5);

            /* inx */
            X = NewCodeEntry (OP65_INX, AM65_IMP, 0, 0, E->LI);
            CS_InsertEntry (S, X, I+6);

            /* Delete the call to negax */
            CS_DelEntry (S, I);

            /* Skip the generated code */
            I += 5;

            /* We had changes */
            ++Changes;
        }

        /* Next entry */
        ++I;

    }

    /* Return the number of changes made */
    return Changes;
}



/*****************************************************************************/
/*                           complax optimizations                           */
/*****************************************************************************/



unsigned OptComplAX1 (CodeSeg* S)
/* Search for a call to complax and replace it by
**
**      eor     #$FF
**
** if X isn't used later.
*/
{
    unsigned Changes = 0;
    unsigned I;

    /* Walk over the entries */
    I = 0;
    while (I < CS_GetEntryCount (S)) {

        /* Get next entry */
        CodeEntry* E = CS_GetEntry (S, I);

        /* Check if this is a call to negax, and if X isn't used later */
        if (CE_IsCallTo (E, "complax") && !RegXUsed (S, I+1)) {

            CodeEntry* X;

            /* Add replacement code behind */
            X = NewCodeEntry (OP65_EOR, AM65_IMM, "$FF", 0, E->LI);
            CS_InsertEntry (S, X, I+1);

            /* Delete the call to negax */
            CS_DelEntry (S, I);

            /* We had changes */
            ++Changes;
        }

        /* Next entry */
        ++I;

    }

    /* Return the number of changes made */
    return Changes;
}
