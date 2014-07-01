/*****************************************************************************/
/*                                                                           */
/*                                 coptcmp.c                                 */
/*                                                                           */
/*                             Optimize compares                             */
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



#include <string.h>

/* cc65 */
#include "codeent.h"
#include "codeinfo.h"
#include "error.h"
#include "coptcmp.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Table used to invert a condition, indexed by condition */
static const unsigned char CmpInvertTab [] = {
    CMP_NE, CMP_EQ,
    CMP_LE, CMP_LT, CMP_GE, CMP_GT,
    CMP_ULE, CMP_ULT, CMP_UGE, CMP_UGT
};



/*****************************************************************************/
/*                             Helper functions                              */
/*****************************************************************************/



static void ReplaceCmp (CodeSeg* S, unsigned I, cmp_t Cond)
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



static int IsImmCmp16 (CodeEntry** L)
/* Check if the instructions at L are an immediate compare of a/x:
**
**
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
            (L[3]->Info & OF_CBRA) != 0                        &&
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



/*****************************************************************************/
/*             Remove calls to the bool transformer subroutines              */
/*****************************************************************************/



unsigned OptBoolTrans (CodeSeg* S)
/* Try to remove the call to boolean transformer routines where the call is
** not really needed.
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
/*                        Optimizations for compares                         */
/*****************************************************************************/



unsigned OptCmp1 (CodeSeg* S)
/* Search for the sequence
**
**      ldx     xx
**      stx     tmp1
**      ora     tmp1
**
** and replace it by
**
**      ora     xx
*/
{
    unsigned Changes = 0;

    /* Walk over the entries */
    unsigned I = 0;
    while (I < CS_GetEntryCount (S)) {

        CodeEntry* L[3];

        /* Get next entry */
        L[0] = CS_GetEntry (S, I);

        /* Check for the sequence */
        if (L[0]->OPC == OP65_LDX               &&
            !CS_RangeHasLabel (S, I+1, 2)       &&
            CS_GetEntries (S, L+1, I+1, 2)      &&
            L[1]->OPC == OP65_STX               &&
            strcmp (L[1]->Arg, "tmp1") == 0     &&
            L[2]->OPC == OP65_ORA               &&
            strcmp (L[2]->Arg, "tmp1") == 0) {

            CodeEntry* X;

            /* Insert the ora instead */
            X = NewCodeEntry (OP65_ORA, L[0]->AM, L[0]->Arg, 0, L[0]->LI);
            CS_InsertEntry (S, X, I);

            /* Remove all other instructions */
            CS_DelEntries (S, I+1, 3);

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
/* Search for the sequence
**
**      stx     xx
**      stx     tmp1
**      ora     tmp1
**
** and replace it by
**
**      stx     xx
**      ora     xx
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
        if (E->OPC == OP65_STX                  &&
            !CS_RangeHasLabel (S, I+1, 2)       &&
            CS_GetEntries (S, L, I+1, 2)        &&
            L[0]->OPC == OP65_STX               &&
            strcmp (L[0]->Arg, "tmp1") == 0     &&
            L[1]->OPC == OP65_ORA               &&
            strcmp (L[1]->Arg, "tmp1") == 0) {

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



unsigned OptCmp3 (CodeSeg* S)
/* Search for
**
**      lda/and/ora/eor ...
**      cmp #$00
**      jeq/jne
** or
**      lda/and/ora/eor ...
**      cmp #$00
**      jsr boolxx
**
** and remove the cmp.
*/
{
    unsigned Changes = 0;

    /* Walk over the entries */
    unsigned I = 0;
    while (I < CS_GetEntryCount (S)) {

        CodeEntry* L[3];

        /* Get next entry */
        L[0] = CS_GetEntry (S, I);

        /* Check for the sequence */
        if ((L[0]->OPC == OP65_ADC ||
             L[0]->OPC == OP65_AND ||
             L[0]->OPC == OP65_ASL ||
             L[0]->OPC == OP65_DEA ||
             L[0]->OPC == OP65_EOR ||
             L[0]->OPC == OP65_INA ||
             L[0]->OPC == OP65_LDA ||
             L[0]->OPC == OP65_LSR ||
             L[0]->OPC == OP65_ORA ||
             L[0]->OPC == OP65_PLA ||
             L[0]->OPC == OP65_SBC ||
             L[0]->OPC == OP65_TXA ||
             L[0]->OPC == OP65_TYA)         &&
            !CS_RangeHasLabel (S, I+1, 2)   &&
            CS_GetEntries (S, L+1, I+1, 2)  &&
            L[1]->OPC == OP65_CMP           &&
            CE_IsKnownImm (L[1], 0)) {

            int Delete = 0;

            /* Check for the call to boolxx. We only remove the compare if
            ** the carry flag is not evaluated later, because the load will
            ** not set the carry flag.
            */
            if (L[2]->OPC == OP65_JSR) {
                switch (FindBoolCmpCond (L[2]->Arg)) {

                    case CMP_EQ:
                    case CMP_NE:
                    case CMP_GT:
                    case CMP_GE:
                    case CMP_LT:
                    case CMP_LE:
                        /* Remove the compare */
                        Delete = 1;
                        break;

                    case CMP_UGT:
                    case CMP_UGE:
                    case CMP_ULT:
                    case CMP_ULE:
                    case CMP_INV:
                        /* Leave it alone */
                        break;
                }

            } else if ((L[2]->Info & OF_FBRA) != 0) {
                /* The following insn branches on the condition of the load,
                ** so the compare instruction might be removed. For safety,
                ** do some more checks if the carry isn't used later, since
                ** the compare does set the carry, but the load does not.
                */
                CodeEntry* E;
                CodeEntry* N;
                if ((E = CS_GetNextEntry (S, I+2)) != 0         &&
                    L[2]->JumpTo != 0                           &&
                    (N = L[2]->JumpTo->Owner) != 0              &&
                    N->OPC != OP65_BCC                          &&
                    N->OPC != OP65_BCS                          &&
                    N->OPC != OP65_JCC                          &&
                    N->OPC != OP65_JCS                          &&
                    (N->OPC != OP65_JSR                 ||
                    FindBoolCmpCond (N->Arg) == CMP_INV)) {

                    /* The following insn branches on the condition of a load,
                    ** and there's no use of the carry flag in sight, so the
                    ** compare instruction can be removed.
                    */
                    Delete = 1;
                }
            }

            /* Delete the compare if we can */
            if (Delete) {
                CS_DelEntry (S, I+1);
                ++Changes;
            }
        }

        /* Next entry */
        ++I;

    }

    /* Return the number of changes made */
    return Changes;
}



unsigned OptCmp4 (CodeSeg* S)
/* Search for
**
**      lda     x
**      ldx     y
**      cpx     #a
**      bne     L1
**      cmp     #b
** L1:  jne/jeq L2
**
** If a is zero, we may remove the compare. If a and b are both zero, we may
** replace it by the sequence
**
**      lda     x
**      ora     x+1
**      jne/jeq ...
**
** L1 may be either the label at the branch instruction, or the target label
** of this instruction.
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
            CS_GetEntries (S, L, I+1, 5)     &&
            L[0]->OPC == OP65_LDX            &&
            !CE_HasLabel (L[0])              &&
            IsImmCmp16 (L+1)                 &&
            !RegAXUsed (S, I+6)) {

            if ((L[4]->Info & OF_FBRA) != 0 && L[1]->Num == 0 && L[3]->Num == 0) {
                /* The value is zero, we may use the simple code version. */
                CE_ReplaceOPC (L[0], OP65_ORA);
                CS_DelEntries (S, I+2, 3);
            } else {
                /* Move the lda instruction after the first branch. This will
                ** improve speed, since the load is delayed after the first
                ** test.
                */
                CS_MoveEntry (S, I, I+4);

                /* We will replace the ldx/cpx by lda/cmp */
                CE_ReplaceOPC (L[0], OP65_LDA);
                CE_ReplaceOPC (L[1], OP65_CMP);

                /* Beware: If the first LDA instruction had a label, we have
                ** to move this label to the top of the sequence again.
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



unsigned OptCmp5 (CodeSeg* S)
/* Optimize compares of local variables:
**
**      ldy     #o
**      jsr     ldaxysp
**      cpx     #a
**      bne     L1
**      cmp     #b
**      jne/jeq L2
*/
{
    unsigned Changes = 0;

    /* Walk over the entries */
    unsigned I = 0;
    while (I < CS_GetEntryCount (S)) {

        CodeEntry* L[6];

        /* Get the next entry */
        L[0] = CS_GetEntry (S, I);

        /* Check for the sequence */
        if (L[0]->OPC == OP65_LDY           &&
            CE_IsConstImm (L[0])            &&
            CS_GetEntries (S, L+1, I+1, 5)  &&
            !CE_HasLabel (L[1])             &&
            CE_IsCallTo (L[1], "ldaxysp")   &&
            IsImmCmp16 (L+2)) {

            if ((L[5]->Info & OF_FBRA) != 0 && L[2]->Num == 0 && L[4]->Num == 0) {

                CodeEntry* X;
                char Buf[20];

                /* The value is zero, we may use the simple code version:
                **      ldy     #o-1
                **      lda     (sp),y
                **      ldy     #o
                **      ora     (sp),y
                **      jne/jeq ...
                */
                sprintf (Buf, "$%02X", (int)(L[0]->Num-1));
                X = NewCodeEntry (OP65_LDY, AM65_IMM, Buf, 0, L[0]->LI);
                CS_InsertEntry (S, X, I+1);

                X = NewCodeEntry (OP65_LDA, AM65_ZP_INDY, "sp", 0, L[1]->LI);
                CS_InsertEntry (S, X, I+2);

                X = NewCodeEntry (OP65_LDY, AM65_IMM, L[0]->Arg, 0, L[0]->LI);
                CS_InsertEntry (S, X, I+3);

                X = NewCodeEntry (OP65_ORA, AM65_ZP_INDY, "sp", 0, L[1]->LI);
                CS_InsertEntry (S, X, I+4);

                CS_DelEntries (S, I+5, 3);   /* cpx/bne/cmp */
                CS_DelEntry (S, I);          /* ldy */

            } else {

                CodeEntry* X;
                char Buf[20];

                /* Change the code to just use the A register. Move the load
                ** of the low byte after the first branch if possible:
                **
                **      ldy     #o
                **      lda     (sp),y
                **      cmp     #a
                **      bne     L1
                **      ldy     #o-1
                **      lda     (sp),y
                **      cmp     #b
                **      jne/jeq ...
                */
                X = NewCodeEntry (OP65_LDY, AM65_IMM, L[0]->Arg, 0, L[0]->LI);
                CS_InsertEntry (S, X, I+3);

                X = NewCodeEntry (OP65_LDA, AM65_ZP_INDY, "sp", 0, L[1]->LI);
                CS_InsertEntry (S, X, I+4);

                X = NewCodeEntry (OP65_CMP, L[2]->AM, L[2]->Arg, 0, L[2]->LI);
                CS_InsertEntry (S, X, I+5);

                sprintf (Buf, "$%02X", (int)(L[0]->Num-1));
                X = NewCodeEntry (OP65_LDY, AM65_IMM, Buf, 0, L[0]->LI);
                CS_InsertEntry (S, X, I+7);

                X = NewCodeEntry (OP65_LDA, AM65_ZP_INDY, "sp", 0, L[1]->LI);
                CS_InsertEntry (S, X, I+8);

                CS_DelEntries (S, I, 3);          /* ldy/jsr/cpx */

            }

            ++Changes;
        }

        /* Next entry */
        ++I;

    }

    /* Return the number of changes made */
    return Changes;
}



unsigned OptCmp6 (CodeSeg* S)
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



unsigned OptCmp7 (CodeSeg* S)
/* Search for a sequence ldx/txa/branch and remove the txa if A is not
** used later.
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



unsigned OptCmp8 (CodeSeg* S)
/* Check for register compares where the contents of the register and therefore
** the result of the compare is known.
*/
{
    unsigned Changes = 0;
    unsigned I;

    /* Walk over the entries */
    I = 0;
    while (I < CS_GetEntryCount (S)) {

        int RegVal;

        /* Get next entry */
        CodeEntry* E = CS_GetEntry (S, I);

        /* Check for a compare against an immediate value */
        if ((E->Info & OF_CMP) != 0           &&
            (RegVal = GetCmpRegVal (E)) >= 0  &&
            CE_IsConstImm (E)) {

            /* We are able to evaluate the compare at compile time. Check if
            ** one or more branches are ahead.
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
                        ** Just skipping anything here is rather stupid, but
                        ** the sequence is never generated by the compiler,
                        ** so it's quite safe to skip).
                        */
                        goto NextEntry;

                    default:
                        Internal ("Unknown branch condition");

                }

                /* If the condition is false, we may remove the jump. Otherwise
                ** the branch will always be taken, so we may replace it by a
                ** jump (and bail out).
                */
                if (!Cond) {
                    CS_DelEntry (S, I+1);
                } else {
                    CodeLabel* L = N->JumpTo;
                    const char* LabelName = L? L->Name : N->Arg;
                    CodeEntry* X = NewCodeEntry (OP65_JMP, AM65_BRA, LabelName, L, N->LI);
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

    /* Return the number of changes made */
    return Changes;
}



unsigned OptCmp9 (CodeSeg* S)
/* Search for the sequence
**
**    sbc       xx
**    bvs/bvc   L
**    eor       #$80
** L: asl       a
**    bcc/bcs   somewhere
**
** If A is not used later (which should be the case), we can branch on the N
** flag instead of the carry flag and remove the asl.
*/
{
    unsigned Changes = 0;
    unsigned I;

    /* Walk over the entries */
    I = 0;
    while (I < CS_GetEntryCount (S)) {

        CodeEntry* L[5];

        /* Get next entry */
        L[0] = CS_GetEntry (S, I);

        /* Check for the sequence */
        if (L[0]->OPC == OP65_SBC                       &&
            CS_GetEntries (S, L+1, I+1, 4)              &&
            (L[1]->OPC == OP65_BVC              ||
             L[1]->OPC == OP65_BVS)                     &&
            L[1]->JumpTo != 0                           &&
            L[1]->JumpTo->Owner == L[3]                 &&
            L[2]->OPC == OP65_EOR                       &&
            CE_IsKnownImm (L[2], 0x80)                  &&
            L[3]->OPC == OP65_ASL                       &&
            L[3]->AM == AM65_ACC                        &&
            (L[4]->OPC == OP65_BCC              ||
             L[4]->OPC == OP65_BCS              ||
             L[4]->OPC == OP65_JCC              ||
             L[4]->OPC == OP65_JCS)                     &&
            !CE_HasLabel (L[4])                         &&
            !RegAUsed (S, I+4)) {

            /* Replace the branch condition */
            switch (GetBranchCond (L[4]->OPC)) {
                case BC_CC:     CE_ReplaceOPC (L[4], OP65_JPL); break;
                case BC_CS:     CE_ReplaceOPC (L[4], OP65_JMI); break;
                default:        Internal ("Unknown branch condition in OptCmp9");
            }

            /* Delete the asl insn */
            CS_DelEntry (S, I+3);

            /* Next sequence is somewhat ahead (if any) */
            I += 3;

            /* Remember, we had changes */
            ++Changes;
        }

        /* Next entry */
        ++I;
    }

    /* Return the number of changes made */
    return Changes;
}
