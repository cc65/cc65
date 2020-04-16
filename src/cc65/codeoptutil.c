/*****************************************************************************/
/*                                                                           */
/*                               codeoptutil.c                               */
/*                                                                           */
/*           Optimize operations that take operands via the stack            */
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

/* common */
#include "chartype.h"
#include "xmalloc.h"

/* cc65 */
#include "codeinfo.h"
#include "codeoptutil.h"
#include "error.h"



/*****************************************************************************/
/*                            Load tracking code                             */
/*****************************************************************************/



void ClearLoadRegInfo (LoadRegInfo* RI)
/* Clear a LoadRegInfo struct */
{
    RI->Flags      = LI_NONE;
    RI->LoadIndex  = -1;
    RI->LoadEntry  = 0;
    RI->LoadYIndex = -1;
    RI->LoadYEntry = 0;
    RI->XferIndex  = -1;
    RI->XferEntry  = 0;
    RI->Offs       = 0;
}



void CopyLoadRegInfo (LoadRegInfo* To, LoadRegInfo* From)
/* Copy a LoadRegInfo struct */
{
    To->Flags      = From->Flags;
    To->LoadIndex  = From->LoadIndex;
    To->LoadEntry  = From->LoadEntry;
    To->LoadYIndex = From->LoadYIndex;
    To->LoadYEntry = From->LoadYEntry;
    To->XferIndex  = From->XferIndex;
    To->XferEntry  = From->XferEntry;
    To->Offs       = From->Offs;
}



void FinalizeLoadRegInfo (LoadRegInfo* RI, CodeSeg* S)
/* Prepare a LoadRegInfo struct for use */
{
    /* Get the entries */
    if (RI->LoadIndex >= 0) {
        RI->LoadEntry = CS_GetEntry (S, RI->LoadIndex);
    } else {
        RI->LoadEntry = 0;
    }
    if (RI->XferIndex >= 0) {
        RI->XferEntry = CS_GetEntry (S, RI->XferIndex);
    } else {
        RI->XferEntry = 0;
    }
    /* Load from src not modified before op can be treated as direct */
    if ((RI->Flags & LI_SRC_CHG) == 0 &&
        (RI->Flags & (LI_CHECK_ARG | LI_CHECK_Y)) != 0) {
        RI->Flags |= LI_DIRECT;
        if ((RI->Flags & LI_CHECK_Y) != 0) {
            RI->Flags |= LI_RELOAD_Y;
        }
    }
    /* We cannot ldy src,y */
    if ((RI->Flags & LI_RELOAD_Y) != 0          &&
        RI->LoadYEntry != 0                     &&
        (RI->LoadYEntry->Use & REG_Y) == REG_Y) {
        RI->Flags &= ~LI_DIRECT;
    }
}



void AdjustLoadRegInfo (LoadRegInfo* RI, int Index, int Change)
/* Adjust a load register info struct after deleting or inserting an entry
** with a given index
*/
{
    CHECK (abs (Change) == 1);
    if (Change < 0) {
        /* Deletion */
        if (Index < RI->LoadIndex) {
            --RI->LoadIndex;
        } else if (Index == RI->LoadIndex) {
            /* Has been removed */
            RI->LoadIndex = -1;
            RI->LoadEntry = 0;
        }
        if (Index < RI->XferIndex) {
            --RI->XferIndex;
        } else if (Index == RI->XferIndex) {
            /* Has been removed */
            RI->XferIndex = -1;
            RI->XferEntry = 0;
        }
    } else {
        /* Insertion */
        if (Index <= RI->LoadIndex) {
            ++RI->LoadIndex;
        }
        if (Index <= RI->XferIndex) {
            ++RI->XferIndex;
        }
    }
}



void ClearLoadInfo (LoadInfo* LI)
/* Clear a LoadInfo struct */
{
    ClearLoadRegInfo (&LI->A);
    ClearLoadRegInfo (&LI->X);
    ClearLoadRegInfo (&LI->Y);
}



void CopyLoadInfo (LoadInfo* To, LoadInfo* From)
/* Copy a LoadInfo struct */
{
    CopyLoadRegInfo (&To->A, &From->A);
    CopyLoadRegInfo (&To->X, &From->X);
    CopyLoadRegInfo (&To->Y, &From->Y);
}



void FinalizeLoadInfo (LoadInfo* LI, CodeSeg* S)
/* Prepare a LoadInfo struct for use */
{
    /* Get the entries */
    FinalizeLoadRegInfo (&LI->A, S);
    FinalizeLoadRegInfo (&LI->X, S);
    FinalizeLoadRegInfo (&LI->Y, S);
}



void AdjustLoadInfo (LoadInfo* LI, int Index, int Change)
/* Adjust a load info struct after deleting entry with a given index */
{
    AdjustLoadRegInfo (&LI->A, Index, Change);
    AdjustLoadRegInfo (&LI->X, Index, Change);
    AdjustLoadRegInfo (&LI->Y, Index, Change);
}



RegInfo* GetLastChangedRegInfo (StackOpData* D, LoadRegInfo* Reg)
/* Get RegInfo of the last load insn entry */
{
    CodeEntry* E;

    if (Reg->LoadIndex >= 0 && (E = CS_GetEntry (D->Code, Reg->LoadIndex)) != 0) {
        return E->RI;
    }

    return 0;
}



static int Affected (LoadRegInfo* RI, const CodeEntry* E)
/* Check if the load src may be modified between the pushax and op */
{
    fncls_t      fncls;
    unsigned int Use;
    unsigned int Chg;
    unsigned int UseToCheck = 0;

    if ((RI->Flags & (LI_CHECK_ARG | LI_CHECK_Y)) != 0) {
        if (E->AM == AM65_IMM || E->AM == AM65_ACC || E->AM == AM65_IMP || E->AM == AM65_BRA) {
            return 0;
        }
        CHECK ((RI->Flags & LI_CHECK_ARG) == 0 || RI->LoadEntry != 0);
        CHECK ((RI->Flags & LI_CHECK_Y) == 0   || RI->LoadYEntry != 0);

        if ((RI->Flags & LI_CHECK_ARG) != 0) {
            UseToCheck |= RI->LoadEntry->Use;
        }

        if ((RI->Flags & LI_CHECK_Y) != 0) {
            UseToCheck |= RI->LoadYEntry->Use;
        }

        if (E->OPC == OP65_JSR) {
            /* Try to know about the function */
            fncls = GetFuncInfo (E->Arg, &Use, &Chg);           
            if ((UseToCheck & Chg & REG_ALL) == 0 &&
                fncls == FNCLS_BUILTIN) {
                /* Builtin functions are known to be harmless */
                return 0;
            }
            /* Otherwise play it safe */
            return 1;
        } else if (E->OPC == OP65_DEC || E->OPC == OP65_INC || 
                   E->OPC == OP65_ASL || E->OPC == OP65_LSR ||
                   E->OPC == OP65_ROL || E->OPC == OP65_ROR ||
                   E->OPC == OP65_TRB || E->OPC == OP65_TSB ||
                   E->OPC == OP65_STA || E->OPC == OP65_STX || E->OPC == OP65_STY) {
            if ((E->AM == AM65_ABS || E->AM == AM65_ZP)) {
                if ((RI->Flags & LI_CHECK_ARG) != 0 && 
                    strcmp (RI->LoadEntry->Arg, E->Arg) == 0) {
                    return 1;
                }
                if ((RI->Flags & LI_CHECK_Y) != 0 &&
                    strcmp (RI->LoadYEntry->Arg, E->Arg) == 0) {
                    return 1;
                }
                return 0;
            }
            /* We could've check further for more cases where the load target isn't modified,
            ** But for now let's save the trouble and just play it safe. */
            return 1;
        }
    }
    return 0;
}



static void HonourUseAndChg (LoadRegInfo* RI, unsigned Reg, const CodeEntry* E, int I)
/* Honour use and change flags for an instruction */
{
    if ((E->Chg & Reg) != 0) {
        /* Remember this as an indirect load */
        ClearLoadRegInfo (RI);
        RI->LoadIndex = I;
        RI->XferIndex = -1;
        RI->Flags = 0;
    } else if (Affected (RI, E)) {
        RI->Flags |= LI_SRC_CHG;
    }
}



unsigned int TrackLoads (LoadInfo* LI, LoadInfo* LLI, CodeSeg* S, int I)
/* Track loads for a code entry.
** Return used registers.
*/
{
    unsigned Used;
    CodeEntry* E = CS_GetEntry (S, I);
    CHECK (E != 0);

    /* By default */
    Used = E->Use;

    /* Whether we had a load or xfer op before or not, the newly loaded value
    ** will be the real one used for the pushax/op unless it's overwritten,
    ** so we can just reset the flags about it in such cases.
    */
    if (E->Info & OF_LOAD) {

        LoadRegInfo* RI = 0;

        /* Determine, which register was loaded */
        if (E->Chg & REG_A) {
            RI = &LI->A;
        } else if (E->Chg & REG_X) {
            RI = &LI->X;
        } else if (E->Chg & REG_Y) {
            RI = &LI->Y;
        }
        CHECK (RI != 0);

        /* Remember the load */
        RI->LoadIndex = I;
        RI->XferIndex = -1;

        /* Set load flags */
        RI->Flags = LI_LOAD_INSN;
        if (E->AM == AM65_IMM) {
            /* These insns are all ok and replaceable */
            RI->Flags |= LI_DIRECT;
        } else if (E->AM == AM65_ZP || E->AM == AM65_ABS) {
            /* These insns are replaceable only if they are not modified later */
            RI->Flags |= LI_CHECK_ARG;
        } else if (E->AM == AM65_ZPY || E->AM == AM65_ABSY) {
            /* These insns are replaceable only if they are not modified later */
            RI->Flags |= LI_CHECK_ARG | LI_CHECK_Y;
        } else if (E->AM == AM65_ZP_INDY &&
                   strcmp (E->Arg, "sp") == 0) {
            /* A load from the stack with known offset is also ok, but in this
            ** case we must reload the index register later. Please note that
            ** a load indirect via other zero page locations is not ok, since
            ** these locations may change between the push and the actual
            ** operation.
            */
            RI->Flags |= LI_DIRECT | LI_CHECK_Y | LI_SP;

            /* Reg Y can be regarded as unused if this load is removed */
            Used &= ~REG_Y;
            if (RI == &LI->A) {
                LI->Y.Flags |= LI_USED_BY_A;
            } else {
                LI->Y.Flags |= LI_USED_BY_X;
            }
        }

        /* If the load offset has a known value, we can just remember and reload
        ** it into the index register later.
        */
        if ((RI->Flags & LI_CHECK_Y) != 0) {
            if (RegValIsKnown (E->RI->In.RegY)) {
                RI->Offs = (unsigned char)E->RI->In.RegY;
                RI->Flags &= ~LI_CHECK_Y;
                RI->Flags |= LI_RELOAD_Y;
            } else {
                /* We need to check if the src of Y is changed */
                RI->LoadYIndex = LI->Y.LoadIndex;
                RI->LoadYEntry = CS_GetEntry (S, RI->LoadYIndex);
            }
        }

        /* Watch for any change of the load target */
        if ((RI->Flags & LI_CHECK_ARG) != 0) {
            RI->LoadEntry = CS_GetEntry (S, I);
        }

    } else if (E->Info & OF_XFR) {

        /* Determine source and target of the transfer and handle the TSX insn */
        LoadRegInfo* Src;
        LoadRegInfo* Tgt;
        switch (E->OPC) {
            case OP65_TAX:
                Src = &LI->A;
                Tgt = &LI->X;
                Used &= ~REG_A;
                Src->Flags |= LI_USED_BY_X;
                break;
            case OP65_TAY:
                Src = &LI->A; 
                Tgt = &LI->Y;
                Used &= ~REG_A; 
                Src->Flags |= LI_USED_BY_Y;
                break;
            case OP65_TXA:
                Src = &LI->X;
                Tgt = &LI->A;
                Used &= ~REG_X;
                Src->Flags |= LI_USED_BY_A;
                break;
            case OP65_TYA:
                Src = &LI->Y;
                Tgt = &LI->A;
                Used &= ~REG_Y;
                Src->Flags |= LI_USED_BY_A;
                break;
            case OP65_TSX:
                ClearLoadRegInfo (&LI->X);
                return Used;
            case OP65_TXS:
                return Used;
            default:            Internal ("Unknown XFR insn in TrackLoads");
        }

        /* Transfer the data */
        Tgt->LoadIndex  = Src->LoadIndex;
        Tgt->LoadEntry  = Src->LoadEntry;
        Tgt->LoadYIndex = Src->LoadYIndex;
        Tgt->LoadYEntry = Src->LoadYEntry;
        Tgt->XferIndex  = I;
        Tgt->Offs       = Src->Offs;
        Tgt->Flags      = Src->Flags;

    } else if (CE_IsCallTo (E, "ldaxysp") && RegValIsKnown (E->RI->In.RegY)) {

        /* Both registers set, Y changed */
        LI->A.LoadIndex = I;
        LI->A.XferIndex = -1;
        LI->A.Flags     = (LI_LOAD_INSN | LI_DIRECT | LI_RELOAD_Y | LI_SP);
        LI->A.Offs      = (unsigned char) E->RI->In.RegY - 1;

        LI->X.LoadIndex = I;
        LI->X.XferIndex = -1;
        LI->X.Flags     = (LI_LOAD_INSN | LI_DIRECT | LI_RELOAD_Y | LI_SP);
        LI->X.Offs      = (unsigned char) E->RI->In.RegY;

        /* Reg Y can be regarded as unused if this load is removed */
        Used &= ~REG_Y;
        LI->Y.Flags |= LI_USED_BY_A | LI_USED_BY_X;

    } else {
        HonourUseAndChg (&LI->A, REG_A, E, I);
        HonourUseAndChg (&LI->X, REG_X, E, I);
        HonourUseAndChg (&LI->Y, REG_Y, E, I);

        /* The other operand may be affected too */
        if (LLI != 0) {
            if (Affected (&LLI->A, E)) {
                LLI->A.Flags |= LI_SRC_CHG;
            }
            if (Affected (&LLI->X, E)) {
                LLI->X.Flags |= LI_SRC_CHG;
            }
            if (Affected (&LLI->Y, E)) {
                LLI->Y.Flags |= LI_SRC_CHG;
            }
        }
    }

    return Used;
}



void SetDontRemoveEntryFlag (LoadRegInfo* RI)
/* Flag the entry as non-removable according to register flags */
{
    if (RI->Flags & LI_DONT_REMOVE) {
        if (RI->LoadEntry != 0) {
            RI->LoadEntry->Flags |= CEF_DONT_REMOVE;
        }
    }
}



void ResetDontRemoveEntryFlag (LoadRegInfo* RI)
/* Unflag the entry as non-removable according to register flags */
{
    if (RI->Flags & LI_DONT_REMOVE) {
        if (RI->LoadEntry != 0) {
            RI->LoadEntry->Flags &= ~CEF_DONT_REMOVE;
        }
    }
}



void SetDontRemoveEntryFlags (StackOpData* D)
/* Flag the entries as non-removable according to register flags */
{
    SetDontRemoveEntryFlag (&D->Lhs.A);
    SetDontRemoveEntryFlag (&D->Lhs.X);
    SetDontRemoveEntryFlag (&D->Rhs.A);
    SetDontRemoveEntryFlag (&D->Rhs.X);
}



void ResetDontRemoveEntryFlags (StackOpData* D)
/* Unflag the entries as non-removable according to register flags */
{
    ResetDontRemoveEntryFlag (&D->Lhs.A);
    ResetDontRemoveEntryFlag (&D->Lhs.X);
    ResetDontRemoveEntryFlag (&D->Rhs.A);
    ResetDontRemoveEntryFlag (&D->Rhs.X);
}



void ResetStackOpData (StackOpData* Data)
/* Reset the given data structure */
{
    Data->OptFunc       = 0;
    Data->ZPUsage       = REG_NONE;
    Data->ZPChanged     = REG_NONE;
    Data->UsedRegs      = REG_NONE;
    Data->RhsMultiChg   = 0;

    ClearLoadInfo (&Data->Lhs);
    ClearLoadInfo (&Data->Rhs);

    Data->PushIndex     = -1;
    Data->OpIndex       = -1;
}



/*****************************************************************************/
/*                                  Helpers                                  */
/*****************************************************************************/



void InsertEntry (StackOpData* D, CodeEntry* E, int Index)
/* Insert a new entry. Depending on Index, D->PushIndex and D->OpIndex will
** be adjusted by this function.
*/
{
    /* Insert the entry into the code segment */
    CS_InsertEntry (D->Code, E, Index);

    /* Adjust register loads if necessary */
    AdjustLoadInfo (&D->Lhs, Index, 1);
    AdjustLoadInfo (&D->Rhs, Index, 1);

    /* Adjust the indices if necessary */
    if (D->PushEntry && Index <= D->PushIndex) {
        ++D->PushIndex;
    }
    if (D->OpEntry && Index <= D->OpIndex) {
        ++D->OpIndex;
    }
}



void DelEntry (StackOpData* D, int Index)
/* Delete an entry. Depending on Index, D->PushIndex and D->OpIndex will be
** adjusted by this function, and PushEntry/OpEntry may get invalidated.
*/
{
    /* Delete the entry from the code segment */
    CS_DelEntry (D->Code, Index);

    /* Adjust register loads if necessary */
    AdjustLoadInfo (&D->Lhs, Index, -1);
    AdjustLoadInfo (&D->Rhs, Index, -1);

    /* Adjust the other indices if necessary */
    if (Index < D->PushIndex) {
        --D->PushIndex;
    } else if (Index == D->PushIndex) {
        D->PushEntry = 0;
    }
    if (Index < D->OpIndex) {
        --D->OpIndex;
    } else if (Index == D->OpIndex) {
        D->OpEntry = 0;
    }
}



void AdjustStackOffset (StackOpData* D, unsigned Offs)
/* Adjust the offset for all stack accesses in the range PushIndex to OpIndex.
** OpIndex is adjusted according to the insertions.
*/
{
    /* Walk over all entries */
    int I = D->PushIndex + 1;
    while (I < D->OpIndex) {

        CodeEntry* E = CS_GetEntry (D->Code, I);

        /* Check if this entry does a stack access, and if so, if it's a plain
        ** load from stack, since this is needed later.
        */
        int Correction = 0;
        if ((E->Use & REG_SP) != 0) {

            /* Check for some things that should not happen */
            CHECK (E->AM == AM65_ZP_INDY || E->RI->In.RegY >= (short) Offs);
            CHECK (strcmp (E->Arg, "sp") == 0);
            /* We need to correct this one */
            Correction = (E->OPC == OP65_LDA)? 2 : 1;

        } else if (CE_IsCallTo (E, "ldaxysp")) {
            /* We need to correct this one */
            Correction = 1;
        }

        if (Correction) {
            /* Get the code entry before this one. If it's a LDY, adjust the
            ** value.
            */
            CodeEntry* P = CS_GetPrevEntry (D->Code, I);
            if (P && P->OPC == OP65_LDY && CE_IsConstImm (P)) {
                /* The Y load is just before the stack access, adjust it */
                CE_SetNumArg (P, P->Num - Offs);
            } else {
                /* Insert a new load instruction before the stack access */
                const char* Arg = MakeHexArg (E->RI->In.RegY - Offs);
                CodeEntry* X = NewCodeEntry (OP65_LDY, AM65_IMM, Arg, 0, E->LI);
                InsertEntry (D, X, I++);
            }

            /* If we need the value of Y later, be sure to reload it */
            if (RegYUsed (D->Code, I+1)) {
                CodeEntry* N;
                const char* Arg = MakeHexArg (E->RI->In.RegY);
                if (Correction == 2 && (N = CS_GetNextEntry(D->Code, I)) != 0 &&
                    ((N->Info & OF_ZBRA) != 0) && N->JumpTo != 0) {
                    /* The Y register is used but the load instruction loads A
                    ** and is followed by a branch that evaluates the zero flag.
                    ** This means that we cannot just insert the load insn
                    ** for the Y register at this place, because it would
                    ** destroy the Z flag. Instead place load insns at the
                    ** target of the branch and after it.
                    ** Note: There is a chance that this code won't work. The
                    ** jump may be a backwards jump (in which case the stack
                    ** offset has already been adjusted) or there may be other
                    ** instructions between the load and the conditional jump.
                    ** Currently the compiler does not generate such code, but
                    ** it is possible to force the optimizer into something
                    ** invalid by use of inline assembler.
                    */

                    /* Add load insn after the branch */
                    CodeEntry* X = NewCodeEntry (OP65_LDY, AM65_IMM, Arg, 0, E->LI);
                    InsertEntry (D, X, I+2);

                    /* Add load insn before branch target */
                    CodeEntry* Y = NewCodeEntry (OP65_LDY, AM65_IMM, Arg, 0, E->LI);
                    int J = CS_GetEntryIndex (D->Code, N->JumpTo->Owner);
                    CHECK (J > I);      /* Must not happen */
                    InsertEntry (D, Y, J);

                    /* Move the label to the new insn */
                    CodeLabel* L = CS_GenLabel (D->Code, Y);
                    CS_MoveLabelRef (D->Code, N, L);
                } else {
                    CodeEntry* X = NewCodeEntry (OP65_LDY, AM65_IMM, Arg, 0, E->LI);
                    InsertEntry (D, X, I+1);
                    /* Skip this instruction in the next round */
                    ++I;
                }
            }
        }

        /* Next entry */
        ++I;
    }

    /* If we have rhs load insns that load from stack, we'll have to adjust
    ** the offsets for these also.
    */
    if ((D->Rhs.A.Flags & (LI_RELOAD_Y | LI_SP | LI_CHECK_Y)) == (LI_RELOAD_Y | LI_SP)) {
        D->Rhs.A.Offs -= Offs;
    }
    if ((D->Rhs.X.Flags & (LI_RELOAD_Y | LI_SP | LI_CHECK_Y)) == (LI_RELOAD_Y | LI_SP)) {
        D->Rhs.X.Offs -= Offs;
    }
}



int IsRegVar (StackOpData* D)
/* If the value pushed is that of a zeropage variable that is unchanged until Op,
** replace ZPLo and ZPHi in the given StackOpData struct by the variable and return true.
** Otherwise leave D untouched and return false.
*/
{
    CodeEntry*  LoadA = D->Lhs.A.LoadEntry;
    CodeEntry*  LoadX = D->Lhs.X.LoadEntry;
    unsigned    Len;

    /* Must be unchanged till Op */
    if ((D->Lhs.A.Flags & (LI_DIRECT | LI_RELOAD_Y)) != LI_DIRECT ||
        (D->Lhs.X.Flags & (LI_DIRECT | LI_RELOAD_Y)) != LI_DIRECT) {
        return 0;
    }

    /* Must have both load insns */
    if (LoadA == 0 || LoadX == 0) {
        return 0;
    }

    /* Must be loads from zp */
    if (LoadA->AM != AM65_ZP || LoadX->AM != AM65_ZP) {
        return 0;
    }

    /* Must be the same zp loc with high byte in X */
    Len = strlen (LoadA->Arg);
    if (strncmp (LoadA->Arg, LoadX->Arg, Len) != 0      ||
        strcmp (LoadX->Arg + Len, "+1") != 0) {
        return 0;
    }

    /* Use the zero page location directly */
    D->ZPLo = LoadA->Arg;
    D->ZPHi = LoadX->Arg;
    return 1;
}



void AddStoreLhsA (StackOpData* D)
/* Add a store to zero page after the push insn */
{
    CodeEntry* X = NewCodeEntry (OP65_STA, AM65_ZP, D->ZPLo, 0, D->PushEntry->LI);
    InsertEntry (D, X, D->PushIndex+1);
}



void AddStoreLhsX (StackOpData* D)
/* Add a store to zero page after the push insn */
{
    CodeEntry* X = NewCodeEntry (OP65_STX, AM65_ZP, D->ZPHi, 0, D->PushEntry->LI);
    InsertEntry (D, X, D->PushIndex+1);
}



void ReplacePushByStore (StackOpData* D)
/* Replace the call to the push subroutine by a store into the zero page
** location (actually, the push is not replaced, because we need it for
** later, but the name is still ok since the push will get removed at the
** end of each routine).
*/
{
    /* Store the value into the zeropage instead of pushing it. Check high
    ** byte first so that the store is later in A/X order.
    */
    if ((D->Lhs.X.Flags & LI_DIRECT) == 0) {
        AddStoreLhsX (D);
    }
    if ((D->Lhs.A.Flags & LI_DIRECT) == 0) {
        AddStoreLhsA (D);
    }
}



void AddOpLow (StackOpData* D, opc_t OPC, LoadInfo* LI)
/* Add an op for the low byte of an operator. This function honours the
** OP_DIRECT and OP_RELOAD_Y flags and generates the necessary instructions.
** All code is inserted at the current insertion point.
*/
{
    CodeEntry* X;

    if ((LI->A.Flags & LI_DIRECT) != 0) {
        /* Op with a variable location. If the location is on the stack, we
        ** need to reload the Y register.
        */
        if ((LI->A.Flags & LI_RELOAD_Y) == 0) {

            /* opc ... */
            CodeEntry* LoadA = LI->A.LoadEntry;
            X = NewCodeEntry (OPC, LoadA->AM, LoadA->Arg, 0, D->OpEntry->LI);
            InsertEntry (D, X, D->IP++);

        } else {

            if ((LI->A.Flags & LI_CHECK_Y) == 0) {
                /* ldy #offs */
                X = NewCodeEntry (OP65_LDY, AM65_IMM, MakeHexArg (LI->A.Offs), 0, D->OpEntry->LI);
            } else {
                /* ldy src */
                X = NewCodeEntry (OP65_LDY, LI->A.LoadYEntry->AM, LI->A.LoadYEntry->Arg, 0, D->OpEntry->LI);
            }
            InsertEntry (D, X, D->IP++);

            if (LI->A.LoadEntry->OPC == OP65_JSR) {
                /* opc (sp),y */
                X = NewCodeEntry (OPC, AM65_ZP_INDY, "sp", 0, D->OpEntry->LI);
            } else {
                /* opc src,y */
                X = NewCodeEntry (OPC, LI->A.LoadEntry->AM, LI->A.LoadEntry->Arg, 0, D->OpEntry->LI);
            }
            InsertEntry (D, X, D->IP++);

        }

        /* In both cases, we can remove the load */
        LI->A.Flags |= LI_REMOVE;

    } else {

        /* Op with temp storage */
        X = NewCodeEntry (OPC, AM65_ZP, D->ZPLo, 0, D->OpEntry->LI);
        InsertEntry (D, X, D->IP++);

    }
}



void AddOpHigh (StackOpData* D, opc_t OPC, LoadInfo* LI, int KeepResult)
/* Add an op for the high byte of an operator. Special cases (constant values
** or similar) have to be checked separately, the function covers only the
** generic case. Code is inserted at the insertion point.
*/
{
    CodeEntry* X;

    if (KeepResult) {
        /* pha */
        X = NewCodeEntry (OP65_PHA, AM65_IMP, 0, 0, D->OpEntry->LI);
        InsertEntry (D, X, D->IP++);
    }

    /* txa */
    X = NewCodeEntry (OP65_TXA, AM65_IMP, 0, 0, D->OpEntry->LI);
    InsertEntry (D, X, D->IP++);

    if ((LI->X.Flags & LI_DIRECT) != 0) {

        if ((LI->X.Flags & LI_RELOAD_Y) == 0) {

            /* opc xxx */
            CodeEntry* LoadX = LI->X.LoadEntry;
            X = NewCodeEntry (OPC, LoadX->AM, LoadX->Arg, 0, D->OpEntry->LI);
            InsertEntry (D, X, D->IP++);

        } else {

            if ((LI->A.Flags & LI_CHECK_Y) == 0) {
                /* ldy #const */
                X = NewCodeEntry (OP65_LDY, AM65_IMM, MakeHexArg (LI->X.Offs), 0, D->OpEntry->LI);
            } else {
                /* ldy src */
                X = NewCodeEntry (OP65_LDY, LI->X.LoadYEntry->AM, LI->X.LoadYEntry->Arg, 0, D->OpEntry->LI);
            }
            InsertEntry (D, X, D->IP++);

            if (LI->X.LoadEntry->OPC == OP65_JSR) {
                /* opc (sp),y */
                X = NewCodeEntry (OPC, AM65_ZP_INDY, "sp", 0, D->OpEntry->LI);
            } else {
                /* opc src,y */
                X = NewCodeEntry (OPC, LI->A.LoadEntry->AM, LI->A.LoadEntry->Arg, 0, D->OpEntry->LI);
            }
            InsertEntry (D, X, D->IP++);
        }

        /* In both cases, we can remove the load */
        LI->X.Flags |= LI_REMOVE;

    } else {
        /* opc zphi */
        X = NewCodeEntry (OPC, AM65_ZP, D->ZPHi, 0, D->OpEntry->LI);
        InsertEntry (D, X, D->IP++);
    }

    if (KeepResult) {
        /* tax */
        X = NewCodeEntry (OP65_TAX, AM65_IMP, 0, 0, D->OpEntry->LI);
        InsertEntry (D, X, D->IP++);

        /* pla */
        X = NewCodeEntry (OP65_PLA, AM65_IMP, 0, 0, D->OpEntry->LI);
        InsertEntry (D, X, D->IP++);
    }
}



void RemoveRegLoads (StackOpData* D, LoadInfo* LI)
/* Remove register load insns */
{
    /* Both registers may be loaded with one insn, but DelEntry will in this
    ** case clear the other one.
    */
    if ((LI->A.Flags & LI_REMOVE) == LI_REMOVE) {
        if (LI->A.LoadIndex >= 0 &&
            (LI->A.LoadEntry->Flags & CEF_DONT_REMOVE) == 0) {
            DelEntry (D, LI->A.LoadIndex);
        }
        if (LI->A.XferIndex >= 0 &&
            (LI->A.XferEntry->Flags & CEF_DONT_REMOVE) == 0) {
            DelEntry (D, LI->A.XferIndex);
        }
    }
    if ((LI->X.Flags & LI_REMOVE) == LI_REMOVE) {
        if (LI->X.LoadIndex >= 0 &&
            (LI->X.LoadEntry->Flags & CEF_DONT_REMOVE) == 0) {
            DelEntry (D, LI->X.LoadIndex);
        }
        if (LI->X.XferIndex >= 0 &&
            (LI->X.XferEntry->Flags & CEF_DONT_REMOVE) == 0) {
            DelEntry (D, LI->X.XferIndex);
        }
    }
}



void RemoveRemainders (StackOpData* D)
/* Remove the code that is unnecessary after translation of the sequence */
{
    /* Remove the register loads for lhs and rhs if nothing prevents that */
    RemoveRegLoads (D, &D->Lhs);
    RemoveRegLoads (D, &D->Rhs);

    /* Remove the push and the operator routine */
    DelEntry (D, D->OpIndex);
    DelEntry (D, D->PushIndex);
}



static int CmpHarmless (const void* Key, const void* Entry)
/* Compare function for bsearch */
{
    return strcmp (Key, *(const char**)Entry);
}



int HarmlessCall (const char* Name)
/* Check if this is a call to a harmless subroutine that will not interrupt
** the pushax/op sequence when encountered.
*/
{
    const char* const Tab[] = {
        "aslax1",
        "aslax2",
        "aslax3",
        "aslax4",
        "aslaxy",
        "asrax1",
        "asrax2",
        "asrax3",
        "asrax4",
        "asraxy",
        "bcastax",
        "bnegax",
        "complax",
        "decax1",
        "decax2",
        "decax3",
        "decax4",
        "decax5",
        "decax6",
        "decax7",
        "decax8",
        "decaxy",
        "incax1",
        "incax2",
        "incax3",
        "incax4",
        "incax5",
        "incax6",
        "incax7",
        "incax8",
        "incaxy",
        "ldaxidx",
        "ldaxysp",
        "negax",
        "shlax1",
        "shlax2",
        "shlax3",
        "shlax4",
        "shlaxy",
        "shrax1",
        "shrax2",
        "shrax3",
        "shrax4",
        "shraxy",
    };

    void* R = bsearch (Name,
                       Tab,
                       sizeof (Tab) / sizeof (Tab[0]),
                       sizeof (Tab[0]),
                       CmpHarmless);
    return (R != 0);
}


