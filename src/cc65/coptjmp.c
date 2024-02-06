/*****************************************************************************/
/*                                                                           */
/*                                 coptjmp.c                                 */
/*                                                                           */
/*           Low level optimizations regarding branches and jumps            */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2001-2009, Ullrich von Bassewitz                                      */
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



/* common */
#include "cpu.h"

/* cc65 */
#include "codeent.h"
#include "coptjmp.h"
#include "codeinfo.h"
#include "codeopt.h"
#include "error.h"



/*****************************************************************************/
/*                             Helper functions                              */
/*****************************************************************************/



static int GetBranchDist (CodeSeg* S, unsigned From, CodeEntry* To)
/* Get the branch distance between the two entries and return it. The distance
** will be negative for backward jumps and positive for forward jumps.
*/
{
    /* Get the index of the branch target */
    unsigned TI = CS_GetEntryIndex (S, To);

    /* Determine the branch distance */
    int Distance = 0;
    if (TI >= From) {
        /* Forward branch, do not count the current insn */
        unsigned J = From+1;
        while (J < TI) {
            CodeEntry* N = CS_GetEntry (S, J++);
            Distance += N->Size;
        }
    } else {
        /* Backward branch */
        unsigned J = TI;
        while (J < From) {
            CodeEntry* N = CS_GetEntry (S, J++);
            Distance -= N->Size;
        }
    }

    /* Return the calculated distance */
    return Distance;
}



static int IsShortDist (int Distance)
/* Return true if the given distance is a short branch distance */
{
    return (Distance >= -125 && Distance <= 125);
}



static short ZPRegVal (unsigned short Use, const RegContents* RC)
/* Return the contents of the given zeropage register */
{
    if ((Use & REG_TMP1) != 0) {
        return RC->Tmp1;
    } else if ((Use & REG_PTR1_LO) != 0) {
        return RC->Ptr1Lo;
    } else if ((Use & REG_PTR1_HI) != 0) {
        return RC->Ptr1Hi;
    } else if ((Use & REG_SREG_LO) != 0) {
        return RC->SRegLo;
    } else if ((Use & REG_SREG_HI) != 0) {
        return RC->SRegHi;
    } else {
        return UNKNOWN_REGVAL;
    }
}



static short RegVal (unsigned short Use, const RegContents* RC)
/* Return the contents of the given register */
{
    if ((Use & REG_A) != 0) {
        return RC->RegA;
    } else if ((Use & REG_X) != 0) {
        return RC->RegX;
    } else if ((Use & REG_Y) != 0) {
        return RC->RegY;
    } else {
        return ZPRegVal (Use, RC);
    }
}



/*****************************************************************************/
/*                           Optimize branch types                           */
/*****************************************************************************/



unsigned OptBranchDist (CodeSeg* S)
/* Change branches for the distance needed. */
{
    unsigned Changes = 0;

    /* Walk over the entries */
    unsigned I = 0;
    while (I < CS_GetEntryCount (S)) {

        /* Get next entry */
        CodeEntry* E = CS_GetEntry (S, I);

        /* Check if it's a conditional branch to a local label. */
        if (E->Info & OF_CBRA) {

            /* Is this a branch to a local symbol? */
            if (E->JumpTo != 0) {

                /* Check if the branch distance is short */
                int IsShort = IsShortDist (GetBranchDist (S, I, E->JumpTo->Owner));

                /* Make the branch short/long according to distance */
                if ((E->Info & OF_LBRA) == 0 && !IsShort) {
                    /* Short branch but long distance */
                    CE_ReplaceOPC (E, MakeLongBranch (E->OPC));
                    ++Changes;
                } else if ((E->Info & OF_LBRA) != 0 && IsShort) {
                    /* Long branch but short distance */
                    CE_ReplaceOPC (E, MakeShortBranch (E->OPC));
                    ++Changes;
                }

            } else if ((E->Info & OF_LBRA) == 0) {

                /* Short branch to external symbol - make it long */
                CE_ReplaceOPC (E, MakeLongBranch (E->OPC));
                ++Changes;

            }

        } else if ((CPUIsets[CPU] & (CPU_ISET_65SC02 |CPU_ISET_6502DTV)) != 0 &&
                   (E->Info & OF_UBRA) != 0               &&
                   E->JumpTo != 0                         &&
                   IsShortDist (GetBranchDist (S, I, E->JumpTo->Owner))) {

            /* The jump is short and may be replaced by a BRA on the 65C02 CPU */
            CE_ReplaceOPC (E, OP65_BRA);
            ++Changes;
        }

        /* Next entry */
        ++I;

    }

    /* Return the number of changes made */
    return Changes;
}



unsigned OptBranchDist2 (CodeSeg* S)
/* Change BRA to JMP if target is an external symbol */
{
    unsigned Changes = 0;

    /* Walk over the entries */
    unsigned I = 0;
    while (I < CS_GetEntryCount (S)) {

        /* Get next entry */
        CodeEntry* E = CS_GetEntry (S, I);

       if ((CPUIsets[CPU] & (CPU_ISET_65SC02 |CPU_ISET_6502DTV)) != 0 && /* CPU has BRA */
           (E->Info & OF_UBRA) != 0                                   && /* is a unconditional branch */
           E->JumpTo == NULL) {                                          /* target is extern */
            /* BRA jumps to external symbol and must be replaced by a JMP on the 65C02 CPU */
            CE_ReplaceOPC (E, OP65_JMP);
            ++Changes;
        }

        /* Next entry */
        ++I;

    }

    /* Return the number of changes made */
    return Changes;
}



/*****************************************************************************/
/*                        Replace jumps to RTS by RTS                        */
/*****************************************************************************/



unsigned OptRTSJumps1 (CodeSeg* S)
/* Replace jumps to RTS by RTS */
{
    unsigned Changes = 0;

    /* Walk over all entries minus the last one */
    unsigned I = 0;
    while (I < CS_GetEntryCount (S)) {

        /* Get the next entry */
        CodeEntry* E = CS_GetEntry (S, I);

        /* Check if it's an unconditional branch to a local target */
        if ((E->Info & OF_UBRA) != 0            &&
            E->JumpTo != 0                      &&
            E->JumpTo->Owner->OPC == OP65_RTS) {

            /* Insert an RTS instruction */
            CodeEntry* X = NewCodeEntry (OP65_RTS, AM65_IMP, 0, 0, E->LI);
            CS_InsertEntry (S, X, I+1);

            /* Delete the jump */
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



unsigned OptRTSJumps2 (CodeSeg* S)
/* Replace long conditional jumps to RTS or to a final target */
{
    unsigned Changes = 0;

    /* Walk over all entries minus the last one */
    unsigned I = 0;
    while (I < CS_GetEntryCount (S) - 1) {

        /* Get the next entry */
        CodeEntry* E = CS_GetEntry (S, I);

        /* Check if it's an conditional branch to a local target */
        if ((E->Info & OF_CBRA) != 0            &&   /* Conditional branch */
            (E->Info & OF_LBRA) != 0            &&   /* Long branch */
            E->JumpTo != 0) {                        /* Local label */


            /* Get the jump target and the next entry. There's always a next
            ** entry, because we don't cover the last entry in the loop.
            */
            CodeEntry* X = 0;
            CodeEntry* T = E->JumpTo->Owner;
            CodeEntry* N = CS_GetNextEntry (S, I);

            /* Check if it's a jump to an RTS insn */
            if (T->OPC == OP65_RTS) {

                /* It's a jump to RTS. Create a conditional branch around an
                ** RTS insn.
                */
                X = NewCodeEntry (OP65_RTS, AM65_IMP, 0, 0, T->LI);

            } else if (T->OPC == OP65_JMP && T->JumpTo == 0) {

                /* It's a jump to a label outside the function. Create a
                ** conditional branch around a jump to the external label.
                */
                X = NewCodeEntry (OP65_JMP, AM65_ABS, T->Arg, T->JumpTo, T->LI);

            }

            /* If we have a replacement insn, insert it */
            if (X) {

                CodeLabel* LN;
                opc_t      NewBranch;

                /* Insert the new insn */
                CS_InsertEntry (S, X, I+1);

                /* Create a conditional branch with the inverse condition
                ** around the replacement insn
                */

                /* Get the new branch opcode */
                NewBranch = MakeShortBranch (GetInverseBranch (E->OPC));

                /* Get the label attached to N, create a new one if needed */
                LN = CS_GenLabel (S, N);

                /* Generate the branch */
                X = NewCodeEntry (NewBranch, AM65_BRA, LN->Name, LN, E->LI);
                CS_InsertEntry (S, X, I+1);

                /* Delete the long branch */
                CS_DelEntry (S, I);

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



/*****************************************************************************/
/*                             Remove dead jumps                             */
/*****************************************************************************/



unsigned OptDeadJumps (CodeSeg* S)
/* Remove dead jumps (jumps to the next instruction) */
{
    unsigned Changes = 0;

    /* Walk over all entries minus the last one */
    unsigned I = 0;
    while (I < CS_GetEntryCount (S)) {

        /* Get the next entry */
        CodeEntry* E = CS_GetEntry (S, I);

        /* Check if it's a branch, if it has a local target, and if the target
        ** is the next instruction.
        */
        if (E->AM == AM65_BRA                               &&
            E->JumpTo                                       &&
            E->JumpTo->Owner == CS_GetNextEntry (S, I)) {

            /* Delete the dead jump */
            CS_DelEntry (S, I);

            /* Remember, we had changes */
            ++Changes;

        } else {

            /* Next entry */
            ++I;

        }
    }

    /* Return the number of changes made */
    return Changes;
}



/*****************************************************************************/
/*                             Remove dead code                              */
/*****************************************************************************/



unsigned OptDeadCode (CodeSeg* S)
/* Remove dead code (code that follows an unconditional jump or an rts/rti
** and has no label)
*/
{
    unsigned Changes = 0;

    /* Walk over all entries */
    unsigned I = 0;
    while (I < CS_GetEntryCount (S)) {

        CodeEntry* N;
        CodeLabel* LN;

        /* Get this entry */
        CodeEntry* E = CS_GetEntry (S, I);

        /* Check if it's an unconditional branch, and if the next entry has
        ** no labels attached, or if the label is just used so that the insn
        ** can jump to itself.
        */
        if ((E->Info & OF_DEAD) != 0                     &&     /* Dead code follows */
            (N = CS_GetNextEntry (S, I)) != 0            &&     /* Has next entry */
            (!CE_HasLabel (N)                        ||         /* Don't has a label */
             ((N->Info & OF_UBRA) != 0          &&              /* Uncond branch */
              (LN = N->JumpTo) != 0             &&              /* Jumps to known label */
              LN->Owner == N                    &&              /* Attached to insn */
              CL_GetRefCount (LN) == 1))) {                     /* Only reference */

            /* Delete the next entry */
            CS_DelEntry (S, I+1);

            /* Remember, we had changes */
            ++Changes;

        } else {

            /* Next entry */
            ++I;

        }
    }

    /* Return the number of changes made */
    return Changes;
}



/*****************************************************************************/
/*                          Optimize jump cascades                           */
/*****************************************************************************/



unsigned OptJumpCascades (CodeSeg* S)
/* Optimize jump cascades (jumps to jumps). In such a case, the jump is
** replaced by a jump to the final location. This will in some cases produce
** worse code, because some jump targets are no longer reachable by short
** branches, but this is quite rare, so there are more advantages than
** disadvantages.
*/
{
    unsigned Changes = 0;

    /* Walk over all entries */
    unsigned I = 0;
    while (I < CS_GetEntryCount (S)) {

        CodeEntry* N;
        CodeLabel* OldLabel;

        /* Get this entry */
        CodeEntry* E = CS_GetEntry (S, I);

        /* Check:
        **   - if it's a branch,
        **   - if it has a jump label,
        **   - if this jump label is not attached to the instruction itself,
        **   - if the target instruction is itself a branch,
        **   - if either the first branch is unconditional or the target of
        **     the second branch is internal to the function.
        ** The latter condition will avoid conditional branches to targets
        ** outside of the function (usually incspx), which won't simplify the
        ** code, since conditional far branches are emulated by a short branch
        ** around a jump.
        */
        if ((E->Info & OF_BRA) != 0             &&
            (OldLabel = E->JumpTo) != 0         &&
            (N = OldLabel->Owner) != E          &&
            (N->Info & OF_BRA) != 0             &&
            ((E->Info & OF_CBRA) == 0   ||
             N->JumpTo != 0)) {

            /* Check if we can use the final target label. That is the case,
            ** if the target branch is an absolute branch; or, if it is a
            ** conditional branch checking the same condition as the first one.
            */
            if ((N->Info & OF_UBRA) != 0 ||
                ((E->Info & OF_CBRA) != 0 &&
                 GetBranchCond (E->OPC)  == GetBranchCond (N->OPC))) {

                /* This is a jump cascade and we may jump to the final target,
                ** provided that the other insn does not jump to itself. If
                ** this is the case, we can also jump to ourselves, otherwise
                ** insert a jump to the new instruction and remove the old one.
                */
                CodeEntry* X;
                CodeLabel* LN = N->JumpTo;

                if (LN != 0 && LN->Owner == N) {

                    /* We found a jump to a jump to itself. Replace our jump
                    ** by a jump to itself.
                    */
                    CodeLabel* LE = CS_GenLabel (S, E);
                    X = NewCodeEntry (E->OPC, E->AM, LE->Name, LE, E->LI);

                } else {

                    /* Jump to the final jump target */
                    X = NewCodeEntry (E->OPC, E->AM, N->Arg, N->JumpTo, E->LI);

                }

                /* Insert it behind E */
                CS_InsertEntry (S, X, I+1);

                /* Remove E */
                CS_DelEntry (S, I);

                /* Remember, we had changes */
                ++Changes;

            /* Check if both are conditional branches, and the condition of
            ** the second is the inverse of that of the first. In this case,
            ** the second branch will never be taken, and we may jump directly
            ** to the instruction behind this one.
            */
            } else if ((E->Info & OF_CBRA) != 0 && (N->Info & OF_CBRA) != 0) {

                CodeEntry* X;   /* Instruction behind N */
                CodeLabel* LX;  /* Label attached to X */

                /* Get the branch conditions of both branches */
                bc_t BC1 = GetBranchCond (E->OPC);
                bc_t BC2 = GetBranchCond (N->OPC);

                /* Check the branch conditions */
                if (BC1 != GetInverseCond (BC2)) {
                    /* Condition not met */
                    goto NextEntry;
                }

                /* We may jump behind this conditional branch. Get the
                ** pointer to the next instruction
                */
                if ((X = CS_GetNextEntry (S, CS_GetEntryIndex (S, N))) == 0) {
                    /* N is the last entry, bail out */
                    goto NextEntry;
                }

                /* Get the label attached to X, create a new one if needed */
                LX = CS_GenLabel (S, X);

                /* Move the reference from E to the new label */
                CS_MoveLabelRef (S, E, LX);

                /* Remember, we had changes */
                ++Changes;
            }
        }

NextEntry:
        /* Next entry */
        ++I;

    }

    /* Return the number of changes made */
    return Changes;
}



/*****************************************************************************/
/*                             Optimize jsr/rts                              */
/*****************************************************************************/



unsigned OptRTS (CodeSeg* S)
/* Optimize subroutine calls followed by an RTS. The subroutine call will get
** replaced by a jump. Don't bother to delete the RTS if it does not have a
** label, the dead code elimination should take care of it.
*/
{
    unsigned Changes = 0;

    /* Walk over all entries minus the last one */
    unsigned I = 0;
    while (I < CS_GetEntryCount (S)) {

        CodeEntry* N;

        /* Get this entry */
        CodeEntry* E = CS_GetEntry (S, I);

        /* Check if it's a subroutine call and if the following insn is RTS */
        if (E->OPC == OP65_JSR                    &&
            (N = CS_GetNextEntry (S, I)) != 0 &&
            N->OPC == OP65_RTS) {

            /* Change the jsr to a jmp and use the additional info for a jump */
            E->AM = AM65_BRA;
            CE_ReplaceOPC (E, OP65_JMP);

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
/*                           Optimize jump targets                           */
/*****************************************************************************/



unsigned OptJumpTarget1 (CodeSeg* S)
/* If the instruction preceeding an unconditional branch is the same as the
** instruction preceeding the jump target, the jump target may be moved
** one entry back. This is a size optimization, since the instruction before
** the branch gets removed.
*/
{
    unsigned Changes = 0;
    CodeEntry* E1;              /* Entry 1 */
    CodeEntry* E2;              /* Entry 2 */
    CodeEntry* T1;              /* Jump target entry 1 */
    CodeLabel* TL1;             /* Target label 1 */

    /* Walk over the entries */
    unsigned I = 0;
    while (I < CS_GetEntryCount (S)) {

        /* Get next entry */
        E2 = CS_GetNextEntry (S, I);

        /* Check if we have a jump or branch without a label attached, and
        ** a jump target, which is not attached to the jump itself
        */
        if (E2 != 0                     &&
            (E2->Info & OF_UBRA) != 0   &&
            !CE_HasLabel (E2)           &&
            E2->JumpTo                  &&
            E2->JumpTo->Owner != E2) {

            /* Get the entry preceeding the branch target */
            T1 = CS_GetPrevEntry (S, CS_GetEntryIndex (S, E2->JumpTo->Owner));
            if (T1 == 0) {
                /* There is no such entry */
                goto NextEntry;
            }

            /* The entry preceeding the branch target may not be the branch
            ** insn.
            */
            if (T1 == E2) {
                goto NextEntry;
            }

            /* Get the entry preceeding the jump */
            E1 = CS_GetEntry (S, I);

            /* Check if both preceeding instructions are identical */
            if (!CodeEntriesAreEqual (E1, T1)) {
                /* Not equal, try next */
                goto NextEntry;
            }

            /* Get the label for the instruction preceeding the jump target.
            ** This routine will create a new label if the instruction does
            ** not already have one.
            */
            TL1 = CS_GenLabel (S, T1);

            /* Change the jump target to point to this new label */
            CS_MoveLabelRef (S, E2, TL1);

            /* If the instruction preceeding the jump has labels attached,
            ** move references to this label to the new label.
            */
            if (CE_HasLabel (E1)) {
                CS_MoveLabels (S, E1, T1);
            }

            /* Remove the entry preceeding the jump */
            CS_DelEntry (S, I);

            /* Remember, we had changes */
            ++Changes;

        } else {
NextEntry:
            /* Next entry */
            ++I;
        }
    }

    /* Return the number of changes made */
    return Changes;
}



unsigned OptJumpTarget2 (CodeSeg* S)
/* If a bcs jumps to a sec insn or a bcc jumps to clc, skip this insn, since
** it's job is already done.
*/
{
    unsigned Changes = 0;

    /* Walk over the entries */
    unsigned I = 0;
    while (I < CS_GetEntryCount (S)) {

        /* OP that may be skipped */
        opc_t OPC;

        /* Jump target insn, old and new */
        CodeEntry* T;
        CodeEntry* N;

        /* New jump label */
        CodeLabel* L;

        /* Get next entry */
        CodeEntry* E = CS_GetEntry (S, I);

        /* Check if this is a bcc insn */
        if (E->OPC == OP65_BCC || E->OPC == OP65_JCC) {
            OPC = OP65_CLC;
        } else if (E->OPC == OP65_BCS || E->OPC == OP65_JCS) {
            OPC = OP65_SEC;
        } else {
            /* Not what we're looking for */
            goto NextEntry;
        }

        /* Must have a jump target */
        if (E->JumpTo == 0) {
            goto NextEntry;
        }

        /* Get the owner insn of the jump target and check if it's the one, we
        ** will skip if present.
        */
        T = E->JumpTo->Owner;
        if (T->OPC != OPC) {
            goto NextEntry;
        }

        /* Get the entry following the branch target */
        N = CS_GetNextEntry (S, CS_GetEntryIndex (S, T));
        if (N == 0) {
            /* There is no such entry */
            goto NextEntry;
        }

        /* Get the label for the instruction following the jump target.
        ** This routine will create a new label if the instruction does
        ** not already have one.
        */
        L = CS_GenLabel (S, N);

        /* Change the jump target to point to this new label */
        CS_MoveLabelRef (S, E, L);

        /* Remember that we had changes */
        ++Changes;

NextEntry:
        /* Next entry */
        ++I;
    }

    /* Return the number of changes made */
    return Changes;
}



unsigned OptJumpTarget3 (CodeSeg* S)
/* Jumps to load instructions of a register, that do already have the matching
** register contents may skip the load instruction, since it's job is already
** done.
*/
{
    unsigned Changes = 0;
    unsigned I;

    /* Walk over the entries */
    I = 0;
    while (I < CS_GetEntryCount (S)) {

        CodeEntry* N;

        /* Get next entry */
        CodeEntry* E = CS_GetEntry (S, I);

        /* Check if this is a load insn with a label and the next insn is not
        ** a conditional branch that needs the flags from the load.
        */
        if ((E->Info & OF_LOAD) != 0            &&
            CE_IsConstImm (E)                   &&
            CE_HasLabel (E)                     &&
            (N = CS_GetNextEntry (S, I)) != 0   &&
            !CE_UseLoadFlags (N)) {

            unsigned J;
            int      K;

            /* New jump label */
            CodeLabel* LN = 0;

            /* Walk over all insn that jump here */
            for (J = 0; J < CE_GetLabelCount (E); ++J) {

                /* Get the label */
                CodeLabel* L = CE_GetLabel (E, J);

                /* Loop over all insn that reference this label. Since we may
                ** eventually remove a reference in the loop, we must loop
                ** from end down to start.
                */
                for (K = CL_GetRefCount (L) - 1; K >= 0; --K) {

                    /* Get the entry that jumps here */
                    CodeEntry* Jump = CL_GetRef (L, K);

                    /* Get the register info from this insn */
                    short Val = RegVal (E->Chg, &Jump->RI->Out2);

                    /* Check if the outgoing value is the one thats's loaded */
                    if (Val == (unsigned char) E->Num) {

                        /* OK, skip the insn. First, generate a label for the
                        ** next insn after E.
                        */
                        if (LN == 0) {
                            LN = CS_GenLabel (S, N);
                        }

                        /* Change the jump target to point to this new label */
                        CS_MoveLabelRef (S, Jump, LN);

                        /* Remember that we had changes */
                        ++Changes;
                    }
                }
            }

        }

        /* Next entry */
        ++I;
    }

    /* Return the number of changes made */
    return Changes;
}



/*****************************************************************************/
/*                       Optimize conditional branches                       */
/*****************************************************************************/



unsigned OptCondBranch1 (CodeSeg* S)
/* Performs some optimization steps:
**  - If an immediate load of a register is followed by a conditional jump that
**    is never taken because the load of the register sets the flags in such a
**    manner, remove the conditional branch.
**  - If the conditional branch is always taken because of the register load,
**    replace it by a jmp.
*/
{
    unsigned Changes = 0;

    /* Walk over the entries */
    unsigned I = 0;
    while (I < CS_GetEntryCount (S)) {

        CodeEntry* N;

        /* Get next entry */
        CodeEntry* E = CS_GetEntry (S, I);

        /* Check if it's a register load */
        if ((E->Info & OF_LOAD) != 0            &&  /* It's a load instruction */
            E->AM == AM65_IMM                   &&  /* ..with immidiate addressing */
            CE_HasNumArg (E)                    &&  /* ..and a numeric argument. */
            (N = CS_GetNextEntry (S, I)) != 0   &&  /* There is a following entry */
            (N->Info & OF_CBRA) != 0            &&  /* ..which is a conditional branch */
            !CE_HasLabel (N)) {                     /* ..and does not have a label */

            /* Get the branch condition */
            bc_t BC = GetBranchCond (N->OPC);

            /* Check the argument against the branch condition */
            if ((BC == BC_EQ && E->Num != 0)            ||
                (BC == BC_NE && E->Num == 0)            ||
                (BC == BC_PL && (E->Num & 0x80) != 0)   ||
                (BC == BC_MI && (E->Num & 0x80) == 0)) {

                /* Remove the conditional branch */
                CS_DelEntry (S, I+1);

                /* Remember, we had changes */
                ++Changes;

            } else if ((BC == BC_EQ && E->Num == 0)             ||
                       (BC == BC_NE && E->Num != 0)             ||
                       (BC == BC_PL && (E->Num & 0x80) == 0)    ||
                       (BC == BC_MI && (E->Num & 0x80) != 0)) {

                /* The branch is always taken, replace it by a jump */
                CE_ReplaceOPC (N, OP65_JMP);

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



unsigned OptCondBranch2 (CodeSeg* S)
/* If a conditional branch jumps around an unconditional branch, remove the
** conditional branch and make the jump a conditional branch with the inverse
** condition of the first one.
*/
{
    unsigned Changes = 0;

    /* Walk over the entries */
    unsigned I = 0;
    while (I < CS_GetEntryCount (S)) {

        CodeEntry* N;
        CodeLabel* L;

        /* Get next entry */
        CodeEntry* E = CS_GetEntry (S, I);

        if ((E->Info & OF_CBRA) != 0              &&  /* It's a conditional branch */
            (L = E->JumpTo) != 0                  &&  /* ..referencing a local label */
            (N = CS_GetNextEntry (S, I)) != 0     &&  /* There is a following entry */
            (N->Info & OF_UBRA) != 0              &&  /* ..which is an uncond branch, */
            !CE_HasLabel (N)                      &&  /* ..has no label attached */
            L->Owner == CS_GetNextEntry (S, I+1)) {   /* ..and jump target follows */

            /* Replace the jump by a conditional branch with the inverse branch
            ** condition than the branch around it.
            */
            CE_ReplaceOPC (N, GetInverseBranch (E->OPC));

            /* Remove the conditional branch */
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



unsigned OptCondBranch3 (CodeSeg* S)
/* If the conditional branch is always taken because it follows an inverse
** conditional branch, replace it by a jmp.
*/
{
    unsigned Changes = 0;

    /* Walk over the entries */
    unsigned I = 0;
    while (I < CS_GetEntryCount (S)) {

        CodeEntry* N;

        /* Get next entry */
        CodeEntry* E = CS_GetEntry (S, I);

        /* Check if it's a conditional branch */
        if ((E->Info & OF_CBRA) != 0            &&  /* It's a conditional branch */
            (N = CS_GetNextEntry (S, I)) != 0   &&  /* There is a following entry */
            (N->Info & OF_CBRA) != 0            &&  /* ..which is a conditional branch */
            !CE_HasLabel (N)) {                     /* ..and does not have a label */

            /* Check if the branches conditions are inverse of each other */
            if (GetInverseCond (GetBranchCond (N->OPC)) == GetBranchCond (E->OPC)) {
                /* The branch is always taken, replace it by a jump */
                CE_ReplaceOPC (N, OP65_JMP);

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



unsigned OptCondBranchC (CodeSeg* S)
/* If on entry to a "rol a" instruction the accu is zero, and a beq/bne follows,
** we can remove the rol and branch on the state of the carry flag.
*/
{
    unsigned Changes = 0;
    unsigned I;

    /* Walk over the entries */
    I = 0;
    while (I < CS_GetEntryCount (S)) {

        CodeEntry* N;

        /* Get next entry */
        CodeEntry* E = CS_GetEntry (S, I);

        /* Check if it's a rol insn with A in accu and a branch follows */
        if (E->OPC == OP65_ROL                  &&
            E->AM == AM65_ACC                   &&
            E->RI->In.RegA == 0                 &&
            !CE_HasLabel (E)                    &&
            (N = CS_GetNextEntry (S, I)) != 0   &&
            (N->Info & OF_ZBRA) != 0            &&
            !RegAUsed (S, I+1)) {

            /* Replace the branch condition */
            switch (GetBranchCond (N->OPC)) {
                case BC_EQ:     CE_ReplaceOPC (N, OP65_JCC); break;
                case BC_NE:     CE_ReplaceOPC (N, OP65_JCS); break;
                default:        Internal ("Unknown branch condition in OptCondBranches2");
            }

            /* Delete the rol insn */
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
