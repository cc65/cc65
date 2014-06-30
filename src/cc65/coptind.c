/*****************************************************************************/
/*                                                                           */
/*                                 coptind.c                                 */
/*                                                                           */
/*              Environment independent low level optimizations              */
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
#include "coptind.h"
#include "codeinfo.h"
#include "codeopt.h"
#include "error.h"



/*****************************************************************************/
/*                             Helper functions                              */
/*****************************************************************************/



static int MemAccess (CodeSeg* S, unsigned From, unsigned To, const CodeEntry* N)
/* Checks a range of code entries if there are any memory accesses to N->Arg */
{
    /* Get the length of the argument */
    unsigned NLen = strlen (N->Arg);

    /* What to check for? */
    enum {
        None    = 0x00,
        Base    = 0x01,         /* Check for location without "+1" */
        Word    = 0x02,         /* Check for location with "+1" added */
    } What = None;


    /* If the argument of N is a zero page location that ends with "+1", we
    ** must also check for word accesses to the location without +1.
    */
    if (N->AM == AM65_ZP && NLen > 2 && strcmp (N->Arg + NLen - 2, "+1") == 0) {
        What |= Base;
    }

    /* If the argument is zero page indirect, we must also check for accesses
    ** to "arg+1"
    */
    if (N->AM == AM65_ZP_INDY || N->AM == AM65_ZPX_IND || N->AM == AM65_ZP_IND) {
        What |= Word;
    }

    /* Walk over all code entries */
    while (From <= To) {

        /* Get the next entry */
        CodeEntry* E = CS_GetEntry (S, From);

        /* Check if there is an argument and if this argument equals Arg in
        ** some variants.
        */
        if (E->Arg[0] != '\0') {

            unsigned ELen;

            if (strcmp (E->Arg, N->Arg) == 0) {
                /* Found an access */
                return 1;
            }

            ELen = strlen (E->Arg);
            if ((What & Base) != 0) {
                if (ELen == NLen - 2 && strncmp (E->Arg, N->Arg, NLen-2) == 0) {
                    /* Found an access */
                    return 1;
                }
            }

            if ((What & Word) != 0) {
                if (ELen == NLen + 2 && strncmp (E->Arg, N->Arg, NLen) == 0 &&
                    E->Arg[NLen] == '+' && E->Arg[NLen+1] == '1') {
                    /* Found an access */
                    return 1;
                }
            }
        }

        /* Next entry */
        ++From;
    }

    /* Nothing found */
    return 0;
}



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



unsigned OptCondBranches1 (CodeSeg* S)
/* Performs several optimization steps:
**
**  - If an immediate load of a register is followed by a conditional jump that
**    is never taken because the load of the register sets the flags in such a
**    manner, remove the conditional branch.
**  - If the conditional branch is always taken because of the register load,
**    replace it by a jmp.
**  - If a conditional branch jumps around an unconditional branch, remove the
**    conditional branch and make the jump a conditional branch with the
**    inverse condition of the first one.
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

        /* Check if it's a register load */
        if ((E->Info & OF_LOAD) != 0              &&  /* It's a load instruction */
            E->AM == AM65_IMM                     &&  /* ..with immidiate addressing */
            (E->Flags & CEF_NUMARG) != 0          &&  /* ..and a numeric argument. */
            (N = CS_GetNextEntry (S, I)) != 0     &&  /* There is a following entry */
            (N->Info & OF_CBRA) != 0              &&  /* ..which is a conditional branch */
            !CE_HasLabel (N)) {               /* ..and does not have a label */

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



unsigned OptCondBranches2 (CodeSeg* S)
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



/*****************************************************************************/
/*                      Remove unused loads and stores                       */
/*****************************************************************************/



unsigned OptUnusedLoads (CodeSeg* S)
/* Remove loads of registers where the value loaded is not used later. */
{
    unsigned Changes = 0;

    /* Walk over the entries */
    unsigned I = 0;
    while (I < CS_GetEntryCount (S)) {

        CodeEntry* N;

        /* Get next entry */
        CodeEntry* E = CS_GetEntry (S, I);

        /* Check if it's a register load or transfer insn */
        if ((E->Info & (OF_LOAD | OF_XFR | OF_REG_INCDEC)) != 0         &&
            (N = CS_GetNextEntry (S, I)) != 0                           &&
            !CE_UseLoadFlags (N)) {

            /* Check which sort of load or transfer it is */
            unsigned R;
            switch (E->OPC) {
                case OP65_DEA:
                case OP65_INA:
                case OP65_LDA:
                case OP65_TXA:
                case OP65_TYA:  R = REG_A;      break;
                case OP65_DEX:
                case OP65_INX:
                case OP65_LDX:
                case OP65_TAX:  R = REG_X;      break;
                case OP65_DEY:
                case OP65_INY:
                case OP65_LDY:
                case OP65_TAY:  R = REG_Y;      break;
                default:        goto NextEntry;         /* OOPS */
            }

            /* Get register usage and check if the register value is used later */
            if ((GetRegInfo (S, I+1, R) & R) == 0) {

                /* Register value is not used, remove the load */
                CS_DelEntry (S, I);

                /* Remember, we had changes. Account the deleted entry in I. */
                ++Changes;
                --I;

            }
        }

NextEntry:
        /* Next entry */
        ++I;

    }

    /* Return the number of changes made */
    return Changes;
}



unsigned OptUnusedStores (CodeSeg* S)
/* Remove stores into zero page registers that aren't used later */
{
    unsigned Changes = 0;

    /* Walk over the entries */
    unsigned I = 0;
    while (I < CS_GetEntryCount (S)) {

        /* Get next entry */
        CodeEntry* E = CS_GetEntry (S, I);

        /* Check if it's a register load or transfer insn */
        if ((E->Info & OF_STORE) != 0    &&
            E->AM == AM65_ZP             &&
            (E->Chg & REG_ZP) != 0) {

            /* Check for the zero page location. We know that there cannot be
            ** more than one zero page location involved in the store.
            */
            unsigned R = E->Chg & REG_ZP;

            /* Get register usage and check if the register value is used later */
            if ((GetRegInfo (S, I+1, R) & R) == 0) {

                /* Register value is not used, remove the load */
                CS_DelEntry (S, I);

                /* Remember, we had changes */
                ++Changes;

                /* Continue with next insn */
                continue;
            }
        }

        /* Next entry */
        ++I;

    }

    /* Return the number of changes made */
    return Changes;
}



unsigned OptDupLoads (CodeSeg* S)
/* Remove loads of registers where the value loaded is already in the register. */
{
    unsigned Changes = 0;
    unsigned I;

    /* Walk over the entries */
    I = 0;
    while (I < CS_GetEntryCount (S)) {

        CodeEntry* N;

        /* Get next entry */
        CodeEntry* E = CS_GetEntry (S, I);

        /* Assume we won't delete the entry */
        int Delete = 0;

        /* Get a pointer to the input registers of the insn */
        const RegContents* In  = &E->RI->In;

        /* Handle the different instructions */
        switch (E->OPC) {

            case OP65_LDA:
                if (RegValIsKnown (In->RegA)          && /* Value of A is known */
                    CE_IsKnownImm (E, In->RegA)       && /* Value to be loaded is known */
                    (N = CS_GetNextEntry (S, I)) != 0 && /* There is a next entry */
                    !CE_UseLoadFlags (N)) {              /* Which does not use the flags */
                    Delete = 1;
                }
                break;

            case OP65_LDX:
                if (RegValIsKnown (In->RegX)          && /* Value of X is known */
                    CE_IsKnownImm (E, In->RegX)       && /* Value to be loaded is known */
                    (N = CS_GetNextEntry (S, I)) != 0 && /* There is a next entry */
                    !CE_UseLoadFlags (N)) {              /* Which does not use the flags */
                    Delete = 1;
                }
                break;

            case OP65_LDY:
                if (RegValIsKnown (In->RegY)          && /* Value of Y is known */
                    CE_IsKnownImm (E, In->RegY)       && /* Value to be loaded is known */
                    (N = CS_GetNextEntry (S, I)) != 0 && /* There is a next entry */
                    !CE_UseLoadFlags (N)) {              /* Which does not use the flags */
                    Delete = 1;
                }
                break;

            case OP65_STA:
                /* If we store into a known zero page location, and this
                ** location does already contain the value to be stored,
                ** remove the store.
                */
                if (RegValIsKnown (In->RegA)          && /* Value of A is known */
                    E->AM == AM65_ZP                  && /* Store into zp */
                    In->RegA == ZPRegVal (E->Chg, In)) { /* Value identical */

                    Delete = 1;
                }
                break;

            case OP65_STX:
                /* If we store into a known zero page location, and this
                ** location does already contain the value to be stored,
                ** remove the store.
                */
                if (RegValIsKnown (In->RegX)          && /* Value of A is known */
                    E->AM == AM65_ZP                  && /* Store into zp */
                    In->RegX == ZPRegVal (E->Chg, In)) { /* Value identical */

                    Delete = 1;

                /* If the value in the X register is known and the same as
                ** that in the A register, replace the store by a STA. The
                ** optimizer will then remove the load instruction for X
                ** later. STX does support the zeropage,y addressing mode,
                ** so be sure to check for that.
                */
                } else if (RegValIsKnown (In->RegX)   &&
                           In->RegX == In->RegA       &&
                           E->AM != AM65_ABSY         &&
                           E->AM != AM65_ZPY) {
                    /* Use the A register instead */
                    CE_ReplaceOPC (E, OP65_STA);
                }
                break;

            case OP65_STY:
                /* If we store into a known zero page location, and this
                ** location does already contain the value to be stored,
                ** remove the store.
                */
                if (RegValIsKnown (In->RegY)          && /* Value of Y is known */
                    E->AM == AM65_ZP                  && /* Store into zp */
                    In->RegY == ZPRegVal (E->Chg, In)) { /* Value identical */

                    Delete = 1;

                /* If the value in the Y register is known and the same as
                ** that in the A register, replace the store by a STA. The
                ** optimizer will then remove the load instruction for Y
                ** later. If replacement by A is not possible try a
                ** replacement by X, but check for invalid addressing modes
                ** in this case.
                */
                } else if (RegValIsKnown (In->RegY)) {
                    if (In->RegY == In->RegA) {
                        CE_ReplaceOPC (E, OP65_STA);
                    } else if (In->RegY == In->RegX   &&
                               E->AM != AM65_ABSX     &&
                               E->AM != AM65_ZPX) {
                        CE_ReplaceOPC (E, OP65_STX);
                    }
                }
                break;

            case OP65_STZ:
                /* If we store into a known zero page location, and this
                ** location does already contain the value to be stored,
                ** remove the store.
                */
                if ((CPUIsets[CPU] & CPU_ISET_65SC02) != 0 && E->AM == AM65_ZP) {
                    if (ZPRegVal (E->Chg, In) == 0) {
                        Delete = 1;
                    }
                }
                break;

            case OP65_TAX:
                if (RegValIsKnown (In->RegA)          &&
                    In->RegA == In->RegX              &&
                    (N = CS_GetNextEntry (S, I)) != 0 &&
                    !CE_UseLoadFlags (N)) {
                    /* Value is identical and not followed by a branch */
                    Delete = 1;
                }
                break;

            case OP65_TAY:
                if (RegValIsKnown (In->RegA)            &&
                    In->RegA == In->RegY                &&
                    (N = CS_GetNextEntry (S, I)) != 0   &&
                    !CE_UseLoadFlags (N)) {
                    /* Value is identical and not followed by a branch */
                    Delete = 1;
                }
                break;

            case OP65_TXA:
                if (RegValIsKnown (In->RegX)            &&
                    In->RegX == In->RegA                &&
                    (N = CS_GetNextEntry (S, I)) != 0   &&
                    !CE_UseLoadFlags (N)) {
                    /* Value is identical and not followed by a branch */
                    Delete = 1;
                }
                break;

            case OP65_TYA:
                if (RegValIsKnown (In->RegY)            &&
                    In->RegY == In->RegA                &&
                    (N = CS_GetNextEntry (S, I)) != 0   &&
                    !CE_UseLoadFlags (N)) {
                    /* Value is identical and not followed by a branch */
                    Delete = 1;
                }
                break;

            default:
                break;

        }

        /* Delete the entry if requested */
        if (Delete) {

            /* Register value is not used, remove the load */
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



unsigned OptStoreLoad (CodeSeg* S)
/* Remove a store followed by a load from the same location. */
{
    unsigned Changes = 0;

    /* Walk over the entries */
    unsigned I = 0;
    while (I < CS_GetEntryCount (S)) {

        CodeEntry* N;
        CodeEntry* X;

        /* Get next entry */
        CodeEntry* E = CS_GetEntry (S, I);

        /* Check if it is a store instruction followed by a load from the
        ** same address which is itself not followed by a conditional branch.
        */
        if ((E->Info & OF_STORE) != 0                       &&
            (N = CS_GetNextEntry (S, I)) != 0               &&
            !CE_HasLabel (N)                                &&
            E->AM == N->AM                                  &&
            ((E->OPC == OP65_STA && N->OPC == OP65_LDA) ||
             (E->OPC == OP65_STX && N->OPC == OP65_LDX) ||
             (E->OPC == OP65_STY && N->OPC == OP65_LDY))    &&
            strcmp (E->Arg, N->Arg) == 0                    &&
            (X = CS_GetNextEntry (S, I+1)) != 0             &&
            !CE_UseLoadFlags (X)) {

            /* Register has already the correct value, remove the load */
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



unsigned OptTransfers1 (CodeSeg* S)
/* Remove transfers from one register to another and back */
{
    unsigned Changes = 0;

    /* Walk over the entries */
    unsigned I = 0;
    while (I < CS_GetEntryCount (S)) {

        CodeEntry* N;
        CodeEntry* X;
        CodeEntry* P;

        /* Get next entry */
        CodeEntry* E = CS_GetEntry (S, I);

        /* Check if we have two transfer instructions */
        if ((E->Info & OF_XFR) != 0                 &&
            (N = CS_GetNextEntry (S, I)) != 0       &&
            !CE_HasLabel (N)                        &&
            (N->Info & OF_XFR) != 0) {

            /* Check if it's a transfer and back */
            if ((E->OPC == OP65_TAX && N->OPC == OP65_TXA && !RegXUsed (S, I+2)) ||
                (E->OPC == OP65_TAY && N->OPC == OP65_TYA && !RegYUsed (S, I+2)) ||
                (E->OPC == OP65_TXA && N->OPC == OP65_TAX && !RegAUsed (S, I+2)) ||
                (E->OPC == OP65_TYA && N->OPC == OP65_TAY && !RegAUsed (S, I+2))) {

                /* If the next insn is a conditional branch, check if the insn
                ** preceeding the first xfr will set the flags right, otherwise we
                ** may not remove the sequence.
                */
                if ((X = CS_GetNextEntry (S, I+1)) == 0) {
                    goto NextEntry;
                }
                if (CE_UseLoadFlags (X)) {
                    if (I == 0) {
                        /* No preceeding entry */
                        goto NextEntry;
                    }
                    P = CS_GetEntry (S, I-1);
                    if ((P->Info & OF_SETF) == 0) {
                        /* Does not set the flags */
                        goto NextEntry;
                    }
                }

                /* Remove both transfers */
                CS_DelEntry (S, I+1);
                CS_DelEntry (S, I);

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



unsigned OptTransfers2 (CodeSeg* S)
/* Replace loads followed by a register transfer by a load with the second
** register if possible.
*/
{
    unsigned Changes = 0;

    /* Walk over the entries */
    unsigned I = 0;
    while (I < CS_GetEntryCount (S)) {

        CodeEntry* N;

        /* Get next entry */
        CodeEntry* E = CS_GetEntry (S, I);

        /* Check if we have a load followed by a transfer where the loaded
        ** register is not used later.
        */
        if ((E->Info & OF_LOAD) != 0                &&
            (N = CS_GetNextEntry (S, I)) != 0       &&
            !CE_HasLabel (N)                        &&
            (N->Info & OF_XFR) != 0                 &&
            GetRegInfo (S, I+2, E->Chg) != E->Chg) {

            CodeEntry* X = 0;

            if (E->OPC == OP65_LDA && N->OPC == OP65_TAX) {
                /* LDA/TAX - check for the right addressing modes */
                if (E->AM == AM65_IMM ||
                    E->AM == AM65_ZP  ||
                    E->AM == AM65_ABS ||
                    E->AM == AM65_ABSY) {
                    /* Replace */
                    X = NewCodeEntry (OP65_LDX, E->AM, E->Arg, 0, N->LI);
                }
            } else if (E->OPC == OP65_LDA && N->OPC == OP65_TAY) {
                /* LDA/TAY - check for the right addressing modes */
                if (E->AM == AM65_IMM ||
                    E->AM == AM65_ZP  ||
                    E->AM == AM65_ZPX ||
                    E->AM == AM65_ABS ||
                    E->AM == AM65_ABSX) {
                    /* Replace */
                    X = NewCodeEntry (OP65_LDY, E->AM, E->Arg, 0, N->LI);
                }
            } else if (E->OPC == OP65_LDY && N->OPC == OP65_TYA) {
                /* LDY/TYA. LDA supports all addressing modes LDY does */
                X = NewCodeEntry (OP65_LDA, E->AM, E->Arg, 0, N->LI);
            } else if (E->OPC == OP65_LDX && N->OPC == OP65_TXA) {
                /* LDX/TXA. LDA doesn't support zp,y, so we must map it to
                ** abs,y instead.
                */
                am_t AM = (E->AM == AM65_ZPY)? AM65_ABSY : E->AM;
                X = NewCodeEntry (OP65_LDA, AM, E->Arg, 0, N->LI);
            }

            /* If we have a load entry, add it and remove the old stuff */
            if (X) {
                CS_InsertEntry (S, X, I+2);
                CS_DelEntries (S, I, 2);
                ++Changes;
                --I;    /* Correct for one entry less */
            }
        }

        /* Next entry */
        ++I;
    }

    /* Return the number of changes made */
    return Changes;
}



unsigned OptTransfers3 (CodeSeg* S)
/* Replace a register transfer followed by a store of the second register by a
** store of the first register if this is possible.
*/
{
    unsigned Changes      = 0;
    unsigned UsedRegs     = REG_NONE;   /* Track used registers */
    unsigned Xfer         = 0;          /* Index of transfer insn */
    unsigned Store        = 0;          /* Index of store insn */
    CodeEntry* XferEntry  = 0;          /* Pointer to xfer insn */
    CodeEntry* StoreEntry = 0;          /* Pointer to store insn */

    enum {
        Initialize,
        Search,
        FoundXfer,
        FoundStore
    } State = Initialize;

    /* Walk over the entries. Look for a xfer instruction that is followed by
    ** a store later, where the value of the register is not used later.
    */
    unsigned I = 0;
    while (I < CS_GetEntryCount (S)) {

        /* Get next entry */
        CodeEntry* E = CS_GetEntry (S, I);

        switch (State) {

            case Initialize:
                /* Clear the list of used registers */
                UsedRegs = REG_NONE;
                /* FALLTHROUGH */

            case Search:
                if (E->Info & OF_XFR) {
                    /* Found start of sequence */
                    Xfer = I;
                    XferEntry = E;
                    State = FoundXfer;
                }
                break;

            case FoundXfer:
                /* If we find a conditional jump, abort the sequence, since
                ** handling them makes things really complicated.
                */
                if (E->Info & OF_CBRA) {

                    /* Switch back to searching */
                    I = Xfer;
                    State = Initialize;

                /* Does this insn use the target register of the transfer? */
                } else if ((E->Use & XferEntry->Chg) != 0) {

                    /* It it's a store instruction, and the block is a basic
                    ** block, proceed. Otherwise restart
                    */
                    if ((E->Info & OF_STORE) != 0       &&
                        CS_IsBasicBlock (S, Xfer, I)) {
                        Store = I;
                        StoreEntry = E;
                        State = FoundStore;
                    } else {
                        I = Xfer;
                        State = Initialize;
                    }

                /* Does this insn change the target register of the transfer? */
                } else if (E->Chg & XferEntry->Chg) {

                    /* We *may* add code here to remove the transfer, but I'm
                    ** currently not sure about the consequences, so I won't
                    ** do that and bail out instead.
                    */
                    I = Xfer;
                    State = Initialize;

                /* Does this insn have a label? */
                } else if (CE_HasLabel (E)) {

                    /* Too complex to handle - bail out */
                    I = Xfer;
                    State = Initialize;

                } else {
                    /* Track used registers */
                    UsedRegs |= E->Use;
                }
                break;

            case FoundStore:
                /* We are at the instruction behind the store. If the register
                ** isn't used later, and we have an address mode match, we can
                ** replace the transfer by a store and remove the store here.
                */
                if ((GetRegInfo (S, I, XferEntry->Chg) & XferEntry->Chg) == 0   &&
                    (StoreEntry->AM == AM65_ABS         ||
                     StoreEntry->AM == AM65_ZP)                                 &&
                    (StoreEntry->AM != AM65_ZP ||
                     (StoreEntry->Chg & UsedRegs) == 0)                         &&
                    !MemAccess (S, Xfer+1, Store-1, StoreEntry)) {

                    /* Generate the replacement store insn */
                    CodeEntry* X = 0;
                    switch (XferEntry->OPC) {

                        case OP65_TXA:
                            X = NewCodeEntry (OP65_STX,
                                              StoreEntry->AM,
                                              StoreEntry->Arg,
                                              0,
                                              StoreEntry->LI);
                            break;

                        case OP65_TAX:
                            X = NewCodeEntry (OP65_STA,
                                              StoreEntry->AM,
                                              StoreEntry->Arg,
                                              0,
                                              StoreEntry->LI);
                            break;

                        case OP65_TYA:
                            X = NewCodeEntry (OP65_STY,
                                              StoreEntry->AM,
                                              StoreEntry->Arg,
                                              0,
                                              StoreEntry->LI);
                            break;

                        case OP65_TAY:
                            X = NewCodeEntry (OP65_STA,
                                              StoreEntry->AM,
                                              StoreEntry->Arg,
                                              0,
                                              StoreEntry->LI);
                            break;

                        default:
                            break;
                    }

                    /* If we have a replacement store, change the code */
                    if (X) {
                        /* Insert after the xfer insn */
                        CS_InsertEntry (S, X, Xfer+1);

                        /* Remove the xfer instead */
                        CS_DelEntry (S, Xfer);

                        /* Remove the final store */
                        CS_DelEntry (S, Store);

                        /* Correct I so we continue with the next insn */
                        I -= 2;

                        /* Remember we had changes */
                        ++Changes;
                    } else {
                        /* Restart after last xfer insn */
                        I = Xfer;
                    }
                } else {
                    /* Restart after last xfer insn */
                    I = Xfer;
                }
                State = Initialize;
                break;

        }

        /* Next entry */
        ++I;
    }

    /* Return the number of changes made */
    return Changes;
}



unsigned OptTransfers4 (CodeSeg* S)
/* Replace a load of a register followed by a transfer insn of the same register
** by a load of the second register if possible.
*/
{
    unsigned Changes      = 0;
    unsigned Load         = 0;  /* Index of load insn */
    unsigned Xfer         = 0;  /* Index of transfer insn */
    CodeEntry* LoadEntry  = 0;  /* Pointer to load insn */
    CodeEntry* XferEntry  = 0;  /* Pointer to xfer insn */

    enum {
        Search,
        FoundLoad,
        FoundXfer
    } State = Search;

    /* Walk over the entries. Look for a load instruction that is followed by
    ** a load later.
    */
    unsigned I = 0;
    while (I < CS_GetEntryCount (S)) {

        /* Get next entry */
        CodeEntry* E = CS_GetEntry (S, I);

        switch (State) {

            case Search:
                if (E->Info & OF_LOAD) {
                    /* Found start of sequence */
                    Load = I;
                    LoadEntry = E;
                    State = FoundLoad;
                }
                break;

            case FoundLoad:
                /* If we find a conditional jump, abort the sequence, since
                ** handling them makes things really complicated.
                */
                if (E->Info & OF_CBRA) {

                    /* Switch back to searching */
                    I = Load;
                    State = Search;

                /* Does this insn use the target register of the load? */
                } else if ((E->Use & LoadEntry->Chg) != 0) {

                    /* It it's a xfer instruction, and the block is a basic
                    ** block, proceed. Otherwise restart
                    */
                    if ((E->Info & OF_XFR) != 0       &&
                        CS_IsBasicBlock (S, Load, I)) {
                        Xfer = I;
                        XferEntry = E;
                        State = FoundXfer;
                    } else {
                        I = Load;
                        State = Search;
                    }

                /* Does this insn change the target register of the load? */
                } else if (E->Chg & LoadEntry->Chg) {

                    /* We *may* add code here to remove the load, but I'm
                    ** currently not sure about the consequences, so I won't
                    ** do that and bail out instead.
                    */
                    I = Load;
                    State = Search;
                }
                break;

            case FoundXfer:
                /* We are at the instruction behind the xfer. If the register
                ** isn't used later, and we have an address mode match, we can
                ** replace the transfer by a load and remove the initial load.
                */
                if ((GetRegInfo (S, I, LoadEntry->Chg) & LoadEntry->Chg) == 0   &&
                    (LoadEntry->AM == AM65_ABS          ||
                     LoadEntry->AM == AM65_ZP           ||
                     LoadEntry->AM == AM65_IMM)                                 &&
                    !MemAccess (S, Load+1, Xfer-1, LoadEntry)) {

                    /* Generate the replacement load insn */
                    CodeEntry* X = 0;
                    switch (XferEntry->OPC) {

                        case OP65_TXA:
                        case OP65_TYA:
                            X = NewCodeEntry (OP65_LDA,
                                              LoadEntry->AM,
                                              LoadEntry->Arg,
                                              0,
                                              LoadEntry->LI);
                            break;

                        case OP65_TAX:
                            X = NewCodeEntry (OP65_LDX,
                                              LoadEntry->AM,
                                              LoadEntry->Arg,
                                              0,
                                              LoadEntry->LI);
                            break;

                        case OP65_TAY:
                            X = NewCodeEntry (OP65_LDY,
                                              LoadEntry->AM,
                                              LoadEntry->Arg,
                                              0,
                                              LoadEntry->LI);
                            break;

                        default:
                            break;
                    }

                    /* If we have a replacement load, change the code */
                    if (X) {
                        /* Insert after the xfer insn */
                        CS_InsertEntry (S, X, Xfer+1);

                        /* Remove the xfer instead */
                        CS_DelEntry (S, Xfer);

                        /* Remove the initial load */
                        CS_DelEntry (S, Load);

                        /* Correct I so we continue with the next insn */
                        I -= 2;

                        /* Remember we had changes */
                        ++Changes;
                    } else {
                        /* Restart after last xfer insn */
                        I = Xfer;
                    }
                } else {
                    /* Restart after last xfer insn */
                    I = Xfer;
                }
                State = Search;
                break;

        }

        /* Next entry */
        ++I;
    }

    /* Return the number of changes made */
    return Changes;
}



unsigned OptPushPop (CodeSeg* S)
/* Remove a PHA/PLA sequence were A is not used later */
{
    unsigned Changes = 0;
    unsigned Push    = 0;       /* Index of push insn */
    unsigned Pop     = 0;       /* Index of pop insn */
    unsigned ChgA    = 0;       /* Flag for A changed */
    enum {
        Searching,
        FoundPush,
        FoundPop
    } State = Searching;

    /* Walk over the entries. Look for a push instruction that is followed by
    ** a pop later, where the pop is not followed by an conditional branch,
    ** and where the value of the A register is not used later on.
    ** Look out for the following problems:
    **
    **  - There may be another PHA/PLA inside the sequence: Restart it.
    **  - If the PLA has a label, all jumps to this label must be inside
    **    the sequence, otherwise we cannot remove the PHA/PLA.
    */
    unsigned I = 0;
    while (I < CS_GetEntryCount (S)) {

        CodeEntry* X;

        /* Get next entry */
        CodeEntry* E = CS_GetEntry (S, I);

        switch (State) {

            case Searching:
                if (E->OPC == OP65_PHA) {
                    /* Found start of sequence */
                    Push  = I;
                    ChgA  = 0;
                    State = FoundPush;
                }
                break;

            case FoundPush:
                if (E->OPC == OP65_PHA) {
                    /* Inner push/pop, restart */
                    Push = I;
                    ChgA = 0;
                } else if (E->OPC == OP65_PLA) {
                    /* Found a matching pop */
                    Pop = I;
                    /* Check that the block between Push and Pop is a basic
                    ** block (one entry, one exit). Otherwise ignore it.
                    */
                    if (CS_IsBasicBlock (S, Push, Pop)) {
                        State = FoundPop;
                    } else {
                        /* Go into searching mode again */
                        State = Searching;
                    }
                } else if (E->Chg & REG_A) {
                    ChgA = 1;
                }
                break;

            case FoundPop:
                /* We're at the instruction after the PLA.
                ** Check for the following conditions:
                **   - If this instruction is a store of A that doesn't use
                **     another register, if the instruction does not have a
                **     label, and A is not used later, we may replace the PHA
                **     by the store and remove pla if several other conditions
                **     are met.
                **   - If this instruction is not a conditional branch, and A
                **     is either unused later, or not changed by the code
                **     between push and pop, we may remove PHA and PLA.
                */
                if (E->OPC == OP65_STA                          &&
                    (E->AM == AM65_ABS || E->AM == AM65_ZP)     &&
                    !CE_HasLabel (E)                            &&
                    !RegAUsed (S, I+1)                          &&
                    !MemAccess (S, Push+1, Pop-1, E)) {

                    /* Insert a STA after the PHA */
                    X = NewCodeEntry (E->OPC, E->AM, E->Arg, E->JumpTo, E->LI);
                    CS_InsertEntry (S, X, Push+1);

                    /* Remove the PHA instead */
                    CS_DelEntry (S, Push);

                    /* Remove the PLA/STA sequence */
                    CS_DelEntries (S, Pop, 2);

                    /* Correct I so we continue with the next insn */
                    I -= 2;

                    /* Remember we had changes */
                    ++Changes;

                } else if ((E->Info & OF_CBRA) == 0     &&
                           (!RegAUsed (S, I) || !ChgA)) {

                    /* We can remove the PHA and PLA instructions */
                    CS_DelEntry (S, Pop);
                    CS_DelEntry (S, Push);

                    /* Correct I so we continue with the next insn */
                    I -= 2;

                    /* Remember we had changes */
                    ++Changes;

                }
                /* Go into search mode again */
                State = Searching;
                break;

        }

        /* Next entry */
        ++I;
    }

    /* Return the number of changes made */
    return Changes;
}



unsigned OptPrecalc (CodeSeg* S)
/* Replace immediate operations with the accu where the current contents are
** known by a load of the final value.
*/
{
    unsigned Changes = 0;
    unsigned I;

    /* Walk over the entries */
    I = 0;
    while (I < CS_GetEntryCount (S)) {

        /* Get next entry */
        CodeEntry* E = CS_GetEntry (S, I);

        /* Get pointers to the input and output registers of the insn */
        const RegContents* Out = &E->RI->Out;
        const RegContents* In  = &E->RI->In;

        /* Argument for LDn and flag */
        const char* Arg = 0;
        opc_t OPC = OP65_LDA;

        /* Handle the different instructions */
        switch (E->OPC) {

            case OP65_LDA:
                if (E->AM != AM65_IMM && RegValIsKnown (Out->RegA)) {
                    /* Result of load is known */
                    Arg = MakeHexArg (Out->RegA);
                }
                break;

            case OP65_LDX:
                if (E->AM != AM65_IMM && RegValIsKnown (Out->RegX)) {
                    /* Result of load is known but register is X */
                    Arg = MakeHexArg (Out->RegX);
                    OPC = OP65_LDX;
                }
                break;

            case OP65_LDY:
                if (E->AM != AM65_IMM && RegValIsKnown (Out->RegY)) {
                    /* Result of load is known but register is Y */
                    Arg = MakeHexArg (Out->RegY);
                    OPC = OP65_LDY;
                }
                break;

            case OP65_EOR:
                if (RegValIsKnown (Out->RegA)) {
                    /* Accu op zp with known contents */
                    Arg = MakeHexArg (Out->RegA);
                }
                break;

            case OP65_ADC:
            case OP65_SBC:
                /* If this is an operation with an immediate operand of zero,
                ** and the register is zero, the operation won't give us any
                ** results we don't already have (including the flags), so
                ** remove it. Something like this is generated as a result of
                ** a compare where parts of the values are known to be zero.
                */
                if (In->RegA == 0 && CE_IsKnownImm (E, 0x00)) {
                    /* 0-0 or 0+0 -> remove */
                    CS_DelEntry (S, I);
                    ++Changes;
                }
                break;

            case OP65_AND:
                if (CE_IsKnownImm (E, 0xFF)) {
                    /* AND with 0xFF, remove */
                    CS_DelEntry (S, I);
                    ++Changes;
                } else if (CE_IsKnownImm (E, 0x00)) {
                    /* AND with 0x00, replace by lda #$00 */
                    Arg = MakeHexArg (0x00);
                } else if (RegValIsKnown (Out->RegA)) {
                    /* Accu AND zp with known contents */
                    Arg = MakeHexArg (Out->RegA);
                } else if (In->RegA == 0xFF) {
                    /* AND but A contains 0xFF - replace by lda */
                    CE_ReplaceOPC (E, OP65_LDA);
                    ++Changes;
                }
                break;

            case OP65_ORA:
                if (CE_IsKnownImm (E, 0x00)) {
                    /* ORA with zero, remove */
                    CS_DelEntry (S, I);
                    ++Changes;
                } else if (CE_IsKnownImm (E, 0xFF)) {
                    /* ORA with 0xFF, replace by lda #$ff */
                    Arg = MakeHexArg (0xFF);
                } else if (RegValIsKnown (Out->RegA)) {
                    /* Accu AND zp with known contents */
                    Arg = MakeHexArg (Out->RegA);
                } else if (In->RegA == 0) {
                    /* ORA but A contains 0x00 - replace by lda */
                    CE_ReplaceOPC (E, OP65_LDA);
                    ++Changes;
                }
                break;

            default:
                break;

        }

        /* Check if we have to replace the insn by LDA */
        if (Arg) {
            CodeEntry* X = NewCodeEntry (OPC, AM65_IMM, Arg, 0, E->LI);
            CS_InsertEntry (S, X, I+1);
            CS_DelEntry (S, I);
            ++Changes;
        }

        /* Next entry */
        ++I;
    }

    /* Return the number of changes made */
    return Changes;
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

        } else if ((CPUIsets[CPU] & CPU_ISET_65SC02) != 0 &&
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



/*****************************************************************************/
/*                          Optimize indirect loads                          */
/*****************************************************************************/



unsigned OptIndLoads1 (CodeSeg* S)
/* Change
**
**     lda      (zp),y
**
** into
**
**     lda      (zp,x)
**
** provided that x and y are both zero.
*/
{
    unsigned Changes = 0;
    unsigned I;

    /* Walk over the entries */
    I = 0;
    while (I < CS_GetEntryCount (S)) {

        /* Get next entry */
        CodeEntry* E = CS_GetEntry (S, I);

        /* Check if it's what we're looking for */
        if (E->OPC == OP65_LDA          &&
            E->AM == AM65_ZP_INDY       &&
            E->RI->In.RegY == 0         &&
            E->RI->In.RegX == 0) {

            /* Replace by the same insn with other addressing mode */
            CodeEntry* X = NewCodeEntry (E->OPC, AM65_ZPX_IND, E->Arg, 0, E->LI);
            CS_InsertEntry (S, X, I+1);

            /* Remove the old insn */
            CS_DelEntry (S, I);
            ++Changes;
        }

        /* Next entry */
        ++I;

    }

    /* Return the number of changes made */
    return Changes;
}



unsigned OptIndLoads2 (CodeSeg* S)
/* Change
**
**     lda      (zp,x)
**
** into
**
**     lda      (zp),y
**
** provided that x and y are both zero.
*/
{
    unsigned Changes = 0;
    unsigned I;

    /* Walk over the entries */
    I = 0;
    while (I < CS_GetEntryCount (S)) {

        /* Get next entry */
        CodeEntry* E = CS_GetEntry (S, I);

        /* Check if it's what we're looking for */
        if (E->OPC == OP65_LDA          &&
            E->AM == AM65_ZPX_IND       &&
            E->RI->In.RegY == 0         &&
            E->RI->In.RegX == 0) {

            /* Replace by the same insn with other addressing mode */
            CodeEntry* X = NewCodeEntry (E->OPC, AM65_ZP_INDY, E->Arg, 0, E->LI);
            CS_InsertEntry (S, X, I+1);

            /* Remove the old insn */
            CS_DelEntry (S, I);
            ++Changes;
        }

        /* Next entry */
        ++I;

    }

    /* Return the number of changes made */
    return Changes;
}
