/*****************************************************************************/
/*                                                                           */
/*                                coptbool.c                                 */
/*                                                                           */
/*                        Optimize boolean sequences                         */
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
#include "error.h"
#include "coptbool.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Table used to invert a condition, indexed by condition */
static const unsigned char CmpInvertTab[] = {
    CMP_NE, CMP_EQ,
    CMP_LE, CMP_LT, CMP_GE, CMP_GT,
    CMP_ULE, CMP_ULT, CMP_UGE, CMP_UGT
};



/*****************************************************************************/
/*                             Helper functions                              */
/*****************************************************************************/



static void ReplaceBranchCond (CodeSeg* S, unsigned I, cmp_t Cond)
/* Helper function for the replacement of routines that return a boolean
** followed by a conditional jump. Instead of the boolean value, the condition
** codes are evaluated directly.
** I is the index of the conditional branch, the sequence is already checked
** to be correct.
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
            **     beq @L
            **     jpl Target
            ** @L: ...
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
            **     jmi Target
            **     jeq Target
            */
            CE_ReplaceOPC (E, OP65_JMI);
            L = E->JumpTo;
            N = NewCodeEntry (OP65_JEQ, AM65_BRA, L->Name, L, E->LI);
            CS_InsertEntry (S, N, I+1);
            break;

        case CMP_UGT:
            /* Replace by
            **     beq @L
            **     jcs Target
            ** @L: ...
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
            **     jcc Target
            **     jeq Target
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



/*****************************************************************************/
/*    Optimize bool comparison and transformer subroutines with branches     */
/*****************************************************************************/



unsigned OptBoolCmp (CodeSeg* S)
/* Search for calls to compare subroutines followed by a conditional branch
** and replace them by cheaper versions, since the branch means that the
** boolean value returned by these routines is not needed (we may also check
** that explicitly, but for the current code generator it is always true).
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
        if (E->OPC == OP65_JSR                          &&
            (Cond = FindTosCmpCond (E->Arg)) != CMP_INV &&
            (N = CS_GetNextEntry (S, I)) != 0           &&
            (N->Info & OF_ZBRA) != 0                    &&
            !CE_HasLabel (N)) {

            /* The tos... functions will return a boolean value in a/x and
            ** the Z flag says if this value is zero or not. We will call
            ** a cheaper subroutine instead, one that does not return a
            ** boolean value but only valid flags. Note: jeq jumps if
            ** the condition is not met, jne jumps if the condition is met.
            ** Invert the code if we jump on condition not met.
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
            ReplaceBranchCond (S, I+1, Cond);

            /* Remember, we had changes */
            ++Changes;

        }

        /* Next entry */
        ++I;

    }

    /* Return the number of changes made */
    return Changes;
}



unsigned OptBoolTrans (CodeSeg* S)
/* Try to remove the call to boolean transformer routines where the call is
** not really needed and change following branch condition accordingly.
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
            (N = CS_GetNextEntry (S, I)) != 0            &&
            (N->Info & OF_ZBRA) != 0                     &&
            (GetRegInfo (S, I + 2, PSTATE_Z) & PSTATE_Z) == 0) {

            /* Make the boolean transformer unnecessary by changing the
            ** the conditional jump to evaluate the condition flags that
            ** are set after the compare directly. Note: jeq jumps if
            ** the condition is not met, jne jumps if the condition is met.
            ** Invert the code if we jump on condition not met.
            */
            if (GetBranchCond (N->OPC) == BC_EQ) {
                /* Jumps if condition false, invert condition */
                Cond = CmpInvertTab [Cond];
            }

            /* Check if we can replace the code by something better */
            ReplaceBranchCond (S, I+1, Cond);

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



unsigned OptBoolUnary (CodeSeg* S)
/* Try to remove the call to a bcastax/bnegax routines where the call is
** not really needed and change following branch condition accordingly.
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
            (N = CS_GetNextEntry (S, I)) != 0            &&
            (N->Info & OF_ZBRA) != 0) {

            /* Make the boolean transformer unnecessary by changing the
            ** the conditional jump to evaluate the condition flags that
            ** are set after the compare directly. Note: jeq jumps if
            ** the condition is not met, jne jumps if the condition is met.
            ** Invert the code if we jump on condition not met.
            */
            if (GetBranchCond (N->OPC) == BC_EQ) {
                /* Jumps if condition false, invert condition */
                Cond = CmpInvertTab [Cond];
            }

            /* Check if we can replace the code by something better */
            ReplaceBranchCond (S, I+1, Cond);

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
/*           Remove calls to the boolean cast/negation subroutines           */
/*****************************************************************************/



unsigned OptBoolUnary1 (CodeSeg* S)
/* Search for and remove cmp #0/bcastax/boolne following a bcastax/bnegax.
** Or search for and remove cmp #1/bnegax/booleq following a bcastax/bnegax
** and invert the bcastax/bnegax.
*/
{
    unsigned Changes = 0;
    int      Neg = 0;

    /* Walk over the entries */
    unsigned I = 0;
    while (I < CS_GetEntryCount (S)) {

        CodeEntry* L[2];

        /* Get next entry */
        L[0] = CS_GetEntry (S, I);

        /* Check for the sequence.
        ** We allow the first entry to have labels.
        */
        if (L[0]->OPC == OP65_JSR                   &&
            (L[1] = CS_GetNextEntry (S, I)) != 0    &&
            !CE_HasLabel (L[1])) {
            if (strcmp (L[0]->Arg, "bnegax") == 0) {
                Neg = 1;
            } else if (strcmp (L[0]->Arg, "bcastax") == 0) {
                Neg = 0;
            } else {
                /* Next entry */
                ++I;
                continue;
            }
            if ((L[1]->OPC == OP65_CMP && CE_IsKnownImm (L[1], 0x0)) ||
                CE_IsCallTo (L[1], "boolne") ||
                CE_IsCallTo (L[1], "bcastax")) {
                /* Delete the entry no longer needed. */
                CS_DelEntry (S, I + 1);

                /* Remember, we had changes */
                ++Changes;

                /* We are still at this index */
                continue;

            } else if ((L[1]->OPC == OP65_CMP && CE_IsKnownImm (L[1], 0x1)) ||
                CE_IsCallTo (L[1], "booleq") ||
                CE_IsCallTo (L[1], "bnegax")) {
                /* Invert the previous bool conversion */
                CE_SetArg (L[0], Neg ? "bcastax" : "bnegax");

                /* Delete the entry no longer needed */
                CS_DelEntry (S, I + 1);

                /* Remember, we had changes */
                ++Changes;

                /* We are still at this index */
                continue;
            }
        }

        /* Next entry */
        ++I;

    }

    /* Return the number of changes made */
    return Changes;
}



unsigned OptBoolUnary2 (CodeSeg* S)
/* Search for and remove cmp #0/bcastax/boolne following a boolean transformer.
** Or search for and remove cmp #1/bnegax/booleq following a boolean transformer
** and invert the boolean transformer.
*/
{
    unsigned Changes = 0;
    cmp_t Cond;
    char Buf[16];

    /* Walk over the entries */
    unsigned I = 0;
    while (I < CS_GetEntryCount (S)) {

        CodeEntry* L[2];

        /* Get next entry */
        L[0] = CS_GetEntry (S, I);

        /* Check for the sequence.
        ** We allow the first entry to have labels.
        */
        if (L[0]->OPC == OP65_JSR                   &&
            (L[1] = CS_GetNextEntry (S, I)) != 0    &&
            !CE_HasLabel (L[1])                     &&
            (Cond = FindBoolCmpCond (L[0]->Arg)) != CMP_INV) {
            if ((L[1]->OPC == OP65_CMP && CE_IsKnownImm (L[1], 0x0)) ||
                CE_IsCallTo (L[1], "boolne") ||
                CE_IsCallTo (L[1], "bcastax")) {
                /* Delete the entry no longer needed */
                CS_DelEntry (S, I + 1);

                /* Remember, we had changes */
                ++Changes;

                /* We are still at this index */
                continue;

            } else if ((L[1]->OPC == OP65_CMP && CE_IsKnownImm (L[1], 0x1)) ||
                CE_IsCallTo (L[1], "booleq") ||
                CE_IsCallTo (L[1], "bnegax")) {
                /* Invert the bool conversion */
                if (GetBoolCmpSuffix (Buf, GetNegatedCond (Cond)) == 0) {
                    Internal ("No inverted boolean transformer for: %s", L[0]->Arg);
                }
                CE_SetArg (L[0], Buf);

                /* Delete the entry no longer needed */
                CS_DelEntry (S, I + 1);

                /* Remember, we had changes */
                ++Changes;

                /* We are still at this index */
                continue;
            }
        }

        /* Next entry */
        ++I;

    }

    /* Return the number of changes made */
    return Changes;
}



unsigned OptBoolUnary3 (CodeSeg* S)
/* If A == 0, replace bcastax/bnegax with
**
**      cpx #0
**      jsr boolne/booleq
**
** Or if X == 0, replace bcastax/bnegax with
**
**      cmp #0
**      jsr boolne/booleq
**
*/
{
    unsigned    Changes = 0;
    opc_t       Op      = OP65_COUNT;
    const char* Sub     = 0;

    /* Walk over the entries */
    unsigned I = 0;
    while (I < CS_GetEntryCount (S)) {

        CodeEntry* E;
        CodeEntry* X;

        /* Get next entry */
        E = CS_GetEntry (S, I);

        /* Check for the sequence */
        if (!CE_HasLabel (E)) {
            /* Choose the right subroutine */
            if (CE_IsCallTo (E, "bnegax")) {
                Sub = "booleq";
            } else if (CE_IsCallTo (E, "bcastax")) {
                Sub = "boolne";
            }
            /* Choose the right opcode */
            if (RegValIsKnown (E->RI->In.RegA) && E->RI->In.RegA == 0) {
                Op = OP65_CPX;
            } else if (RegValIsKnown (E->RI->In.RegX) && E->RI->In.RegX == 0) {
                Op = OP65_CMP;
            }
            /* Replace the sequence if all requirements are met*/
            if (Op != OP65_COUNT && Sub != 0) {
                /* Replace bcastax/bnegax with boolne/booleq */
                CE_SetArg (E, Sub);

                /* Insert the compare */
                X = NewCodeEntry (Op, AM65_IMM, "$00", 0, E->LI);
                CS_InsertEntry (S, X, I);

                /* Remember, we had changes */
                ++Changes;

                /* Correct the index */
                ++I;
            }

            /* Reset the choices */
            Op  = OP65_COUNT;
            Sub = 0;
        }

        /* Next entry */
        ++I;

    }

    /* Return the number of changes made */
    return Changes;
}



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
        if (E->OPC == OP65_LDX              &&
            E->AM == AM65_IMM               &&
            CE_HasNumArg (E)                &&
            E->Num == 0                     &&
            CS_GetEntries (S, L, I+1, 2)    &&
            L[0]->OPC == OP65_LDA           &&
            (L[0]->Use & REG_X) == 0        &&
            !CE_HasLabel (L[0])             &&
            CE_IsCallTo (L[1], "bnega")     &&
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
            !CE_HasLabel (L[1])                 &&
            (GetRegInfo (S, I + 3, PSTATE_Z) & PSTATE_Z) == 0) {

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
            (L[3]->Info & OF_ZBRA) != 0         &&
            (GetRegInfo (S, I + 4, PSTATE_Z) & PSTATE_Z) == 0) {

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
            !CE_HasLabel (L[2])                 &&
            (GetRegInfo (S, I + 4, PSTATE_Z) & PSTATE_Z) == 0) {

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
            !CE_HasLabel (L[1])                 &&
            (GetRegInfo (S, I + 3, PSTATE_Z) & PSTATE_Z) == 0) {

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
