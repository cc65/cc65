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



void ClearLoadRegInfo (LoadRegInfo* LRI)
/* Clear a LoadRegInfo struct */
{
    LRI->Flags      = LI_NONE;
    LRI->LoadIndex  = -1;
    LRI->LoadEntry  = 0;
    LRI->LoadYIndex = -1;
    LRI->LoadYEntry = 0;
    LRI->ChgIndex   = -1;
    LRI->ChgEntry   = 0;
    LRI->Offs       = 0;
}



void CopyLoadRegInfo (LoadRegInfo* To, LoadRegInfo* From)
/* Copy a LoadRegInfo struct */
{
    To->Flags      = From->Flags;
    To->LoadIndex  = From->LoadIndex;
    To->LoadEntry  = From->LoadEntry;
    To->LoadYIndex = From->LoadYIndex;
    To->LoadYEntry = From->LoadYEntry;
    To->ChgIndex   = From->ChgIndex;
    To->ChgEntry   = From->ChgEntry;
    To->Offs       = From->Offs;
}



void FinalizeLoadRegInfo (LoadRegInfo* LRI, CodeSeg* S)
/* Prepare a LoadRegInfo struct for use */
{
    /* Get the entries */
    if (LRI->LoadIndex >= 0) {
        LRI->LoadEntry = CS_GetEntry (S, LRI->LoadIndex);
    } else {
        LRI->LoadEntry = 0;
    }
    if (LRI->LoadYIndex >= 0) {
        LRI->LoadYEntry = CS_GetEntry (S, LRI->LoadYIndex);
    } else {
        LRI->LoadYEntry = 0;
    }
    if (LRI->ChgIndex >= 0) {
        LRI->ChgEntry = CS_GetEntry (S, LRI->ChgIndex);
    } else {
        LRI->ChgEntry = 0;
    }

    /* Load from src not modified before op can be treated as direct */
    if ((LRI->Flags & (LI_SRC_CHG | LI_Y_SRC_CHG)) == 0 &&
        (LRI->Flags & (LI_CHECK_ARG | LI_CHECK_Y)) != 0) {
        LRI->Flags |= LI_DIRECT;
        if ((LRI->Flags & LI_CHECK_Y) != 0) {
            LRI->Flags |= LI_RELOAD_Y;
        }
    }
    /* We cannot ldy src,y or reload unknown Y */
    if ((LRI->Flags & (LI_CHECK_Y | LI_RELOAD_Y)) == (LI_CHECK_Y | LI_RELOAD_Y) &&
        (LRI->LoadYEntry == 0 ||
         (LRI->LoadYEntry->Use & REG_Y) == REG_Y)) {
        LRI->Flags &= ~LI_DIRECT;
    }
}



void AdjustLoadRegInfo (LoadRegInfo* LRI, int Index, int Change)
/* Adjust a load register info struct after deleting or inserting an entry
** with a given index
*/
{
    CHECK (abs (Change) == 1);
    if (Change < 0) {
        /* Deletion */
        if (Index < LRI->LoadIndex) {
            --LRI->LoadIndex;
        } else if (Index == LRI->LoadIndex) {
            /* Has been removed */
            LRI->LoadIndex = -1;
            LRI->LoadEntry = 0;
        }
        if (Index < LRI->LoadYIndex) {
            --LRI->LoadIndex;
        } else if (Index == LRI->LoadYIndex) {
            /* Has been removed */
            LRI->LoadYIndex = -1;
            LRI->LoadYEntry = 0;
        }
        if (Index < LRI->ChgIndex) {
            --LRI->ChgIndex;
        } else if (Index == LRI->ChgIndex) {
            /* Has been removed */
            LRI->ChgIndex = -1;
            LRI->ChgEntry = 0;
        }
    } else {
        /* Insertion */
        if (Index <= LRI->LoadIndex) {
            ++LRI->LoadIndex;
        }
        if (Index <= LRI->LoadYIndex) {
            ++LRI->LoadYIndex;
        }
        if (Index <= LRI->ChgIndex) {
            ++LRI->ChgIndex;
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
/* Get RegInfo of the last insn entry that changed the reg */
{
    CodeEntry* E;

    if (Reg->ChgIndex >= 0 && (E = CS_GetEntry (D->Code, Reg->ChgIndex)) != 0) {
        return E->RI;
    }

    return 0;
}



static int Affected (LoadRegInfo* LRI, const CodeEntry* E)
/* Check if the result of the same loading code as in LRI may be changed by E.
** If any part of the arg is used, it could be unsafe to add such a store before E.
** If any part of the arg is changed, it could be unsafe to add such a load after E.
*/
{
    fncls_t         fncls;
    unsigned int    Use;
    unsigned int    Chg;
    unsigned int    UseToCheck = 0;
    unsigned int    ChgToCheck = 0;
    const ZPInfo*   ZI  = 0;
    unsigned        Res = 0;
    CodeEntry*      AE  = 0;
    CodeEntry*      YE  = 0;

    if ((LRI->Flags & (LI_CHECK_ARG | LI_CHECK_Y | LI_RELOAD_Y)) == 0) {
        /* Nothing to check */
        return 0;
    }

    if (E->AM == AM65_ACC || E->AM == AM65_BRA || E->AM == AM65_IMM || E->AM == AM65_IMP) {
        goto L_Result;
    }
    CHECK ((LRI->Flags & LI_CHECK_ARG) == 0 || LRI->LoadIndex < 0 || LRI->LoadEntry != 0);
    CHECK ((LRI->Flags & (LI_CHECK_Y | LI_RELOAD_Y)) == 0 || LRI->LoadYIndex < 0 || LRI->LoadYEntry != 0);

    if ((LRI->Flags & LI_CHECK_ARG) != 0) {
        AE = LRI->LoadEntry;
        if (AE != 0) {
            /* We ignore processor flags for loading args.
            ** Further more, Reg A can't be used as the index.
            */
            UseToCheck |= AE->Use & ~REG_A & REG_ALL;
            ChgToCheck |= AE->Chg & ~REG_A & REG_ALL;

            /* Check if the argument has been parsed successfully */
            if (!CE_IsArgStrParsed (AE)) {
                /* Bail out and play it safe*/
                goto L_Affected;
            }
            /* We have to manually set up the use/chg flags for builtin functions */
            ZI = GetZPInfo (AE->ArgBase);
            if (ZI != 0) {
                UseToCheck |= ZI->ByteUse;
                ChgToCheck |= ZI->ByteUse;
            }
        } else {
            /* We don't know what regs could have been used for the src.
            ** So we just assume all.
            */
            UseToCheck |= ~REG_A & REG_ALL;
            ChgToCheck |= ~REG_A & REG_ALL;
        }
    }

    if ((LRI->Flags & LI_CHECK_Y) != 0) {
        YE = LRI->LoadYEntry;
        if (YE != 0) {
            UseToCheck |= YE->Use;

            /* Check if the argument has been parsed successfully */
            if (!CE_IsArgStrParsed (YE)) {
                /* Bail out and play it safe*/
                goto L_Affected;
            }
            /* We have to manually set up the use/chg flags for builtin functions */
            ZI = GetZPInfo (YE->ArgBase);
            if (ZI != 0) {
                UseToCheck |= ZI->ByteUse;
                ChgToCheck |= ZI->ByteUse;
            }
        } else {
            /* We don't know what regs could have been used by Y.
            ** So we just assume all.
            */
            UseToCheck |= ~REG_A & REG_ALL;
            ChgToCheck |= ~REG_A & REG_ALL;
        }
    }

    if (E->OPC == OP65_JSR) {
        /* Try to know about the function */
        fncls = GetFuncInfo (E->Arg, &Use, &Chg);
        if (fncls == FNCLS_BUILTIN) {
            /* Builtin functions are usually harmless */
            if ((ChgToCheck & Use & REG_ALL) != 0) {
                Res |= LI_SRC_USE;
            }
            if ((UseToCheck & Chg & REG_ALL) != 0) {
                Res |= LI_SRC_CHG;
            }
            goto L_Result;
        }
        /* Otherwise play it safe */
        goto L_Affected;

    } else {
        if ((E->Info & (OF_READ | OF_WRITE)) != 0) {

            /* Check if the argument has been parsed successfully */
            if (!CE_IsArgStrParsed (E)) {
                /* Bail out and play it safe*/
                goto L_Affected;
            }

            /* These opc may operate on memory locations. In some cases we can
            ** be sure that the src is unaffected as E doesn't overlap with it.
            ** However, if we don't know what memory locations could have been
            ** used for the src, we just assume all.
            */
            if (E->AM == AM65_ABS       ||
                E->AM == AM65_ZP        ||
                (E->AM == AM65_ZP_INDY && strcmp (E->ArgBase, "sp") == 0)
                ) {
                if ((LRI->Flags & LI_CHECK_ARG) != 0) {
                    if (AE == 0                             ||
                        (AE->AM != AM65_ABS &&
                         AE->AM != AM65_ZP  &&
                         (AE->AM != AM65_ZP_INDY ||
                          strcmp (AE->ArgBase, "sp") != 0)) ||
                         (AE->ArgOff == E->ArgOff &&
                          strcmp (AE->ArgBase, E->ArgBase) == 0)) {

                        if ((E->Info & OF_READ) != 0) {
                            /* Used */
                            Res |= LI_SRC_USE;
                        }
                        if ((E->Info & OF_WRITE) != 0) {
                            /* Changed */
                            Res |= LI_SRC_CHG;
                        }
                    }
                }

                if ((LRI->Flags & LI_CHECK_Y) != 0) {
                    /* If we don't know what memory location could have been
                    ** used by Y, we just assume all.
                    */
                    if (YE == 0 ||
                        (YE->ArgOff == E->ArgOff && strcmp (YE->ArgBase, E->ArgBase) == 0)) {

                        if ((E->Info & OF_READ) != 0) {
                            /* Used */
                            Res |= LI_Y_SRC_USE;
                        }
                        if ((E->Info & OF_WRITE) != 0) {
                            /* Changed */
                            Res |= LI_Y_SRC_CHG;
                        }
                    }
                }

                /* Otherwise unaffected */
                goto L_Result;
            }
            /* We could've check further for more cases where the load target
            ** isn't modified, but for now let's save the trouble and just play
            ** it safe.
            */
            goto L_Affected;
        }
    }

L_Affected:
    if ((E->Info & OF_READ) != 0) {
        /* Used */
        Res |= LI_SRC_USE;
        if ((LRI->Flags & LI_CHECK_Y) != 0) {
            Res |= LI_Y_SRC_USE;
        }
    }
    if ((E->Info & OF_WRITE) != 0) {
        /* Changed */
        Res |= LI_SRC_CHG;
        if ((LRI->Flags & LI_CHECK_Y) != 0) {
            Res |= LI_Y_SRC_CHG;
        }
    }

L_Result:
    if ((LRI->Flags & LI_RELOAD_Y) != 0 &&
        (E->Use & REG_Y) != 0) {
        Res |= LI_Y_USE;
    }
    if ((LRI->Flags & LI_CHECK_Y) != 0 &&
        (E->Chg & REG_Y) != 0) {
        Res |= LI_Y_CHG;
    }

    return Res;
}



static void HonourUseAndChg (LoadRegInfo* LRI, unsigned Reg, const CodeEntry* E, int I)
/* Honour use and change flags for an instruction */
{
    if ((E->Chg & Reg) != 0) {
        /* This changes the content of the reg */
        ClearLoadRegInfo (LRI);
        LRI->ChgIndex = I;
        LRI->Flags = 0;
    } else {
        LRI->Flags |= Affected (LRI, E);
    }
}



void PrepairLoadRegInfoForArgCheck (CodeSeg* S, LoadRegInfo* LRI, CodeEntry* E)
/* Set the load src flags and remember to check for load src change if necessary.
** Note: this doesn't assume reloading Y.
*/
{
    if (E->AM == AM65_IMM) {
        /* These insns are all ok and replaceable */
        LRI->Flags |= LI_DIRECT;
    } else if (E->AM == AM65_ZP || E->AM == AM65_ABS) {
        /* These insns are replaceable only if they are not modified later */
        LRI->Flags |= LI_CHECK_ARG;
    } else if (E->AM == AM65_ZPY || E->AM == AM65_ABSY) {
        /* These insns are replaceable only if they are not modified later */
        LRI->Flags |= LI_CHECK_ARG | LI_CHECK_Y;
    } else if ((E->AM == AM65_ZP_INDY) &&
                strcmp (E->Arg, "sp") == 0) {
        /* A load from the stack with known offset is also ok, but in this
        ** case we must reload the index register later. Please note that
        ** a load indirect via other zero page locations is not ok, since
        ** these locations may change between the push and the actual
        ** operation.
        */
        LRI->Flags |= LI_CHECK_ARG | LI_CHECK_Y;
    }

    /* If the load offset has a known value, we can just remember and reload
    ** it into the index register later.
    */
    if ((LRI->Flags & LI_CHECK_Y) != 0) {
        if (RegValIsKnown (E->RI->In.RegY)) {
            LRI->Offs = (unsigned char)E->RI->In.RegY;
            LRI->Flags &= ~LI_CHECK_Y;
            LRI->Flags |= LI_RELOAD_Y;
        }
    }

    /* Watch for any change of the load target */
    if ((LRI->Flags & LI_CHECK_ARG) != 0) {
        LRI->LoadIndex = CS_GetEntryIndex (S, E);
        LRI->LoadEntry = E;
    }

    /* We need to check if the src of Y is changed */
    if (LRI->LoadYIndex >= 0) {
        LRI->LoadYEntry = CS_GetEntry (S, LRI->LoadYIndex);
    } else {
        LRI->LoadYEntry = 0;
    }
}



void SetIfOperandSrcAffected (LoadInfo* LLI, CodeEntry* E)
/* Check and flag operand src that may be affected */
{
    LLI->A.Flags |= Affected (&LLI->A, E);
    LLI->X.Flags |= Affected (&LLI->X, E);
    LLI->Y.Flags |= Affected (&LLI->Y, E);
}



void SetIfOperandLoadUnremovable (LoadInfo* LI, unsigned Used)
/* Check and flag operand load that may be unremovable */
{
    /* Disallow removing the loads if the registers are used */
    if ((Used & REG_A) != 0) {
        LI->A.Flags |= LI_DONT_REMOVE;
    }
    if ((Used & REG_X) != 0) {
        LI->X.Flags |= LI_DONT_REMOVE;
    }
    if ((Used & REG_Y) != 0) {
        LI->Y.Flags |= LI_DONT_REMOVE;
    }
}



unsigned int TrackLoads (LoadInfo* LI, CodeSeg* S, int I)
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

        LoadRegInfo* LRI = 0;

        /* Determine, which register was loaded */
        if (E->Chg & REG_A) {
            LRI = &LI->A;
        } else if (E->Chg & REG_X) {
            LRI = &LI->X;
        } else if (E->Chg & REG_Y) {
            LRI = &LI->Y;
        }
        CHECK (LRI != 0);

        /* Remember the load */
        LRI->LoadIndex  = I;
        LRI->ChgIndex   = I;
        LRI->LoadYIndex = -1;

        /* Set load flags */
        LRI->Flags = LI_LOAD_INSN;
        if (E->AM == AM65_IMM) {
            /* These insns are all ok and replaceable */
            LRI->Flags |= LI_DIRECT;
        } else if (E->AM == AM65_ZP || E->AM == AM65_ABS) {
            /* These insns are replaceable only if they are not modified later */
            LRI->Flags |= LI_CHECK_ARG;
        } else if (E->AM == AM65_ZPY || E->AM == AM65_ABSY) {
            /* These insns are replaceable only if they are not modified later */
            LRI->Flags |= LI_CHECK_ARG | LI_CHECK_Y;
        } else if (E->AM == AM65_ZP_INDY &&
                   strcmp (E->Arg, "sp") == 0) {
            /* A load from the stack with known offset is also ok, but in this
            ** case we must reload the index register later. Please note that
            ** a load indirect via other zero page locations is not ok, since
            ** these locations may change between the push and the actual
            ** operation.
            */
            LRI->Flags |= LI_CHECK_ARG | LI_CHECK_Y | LI_SP;

            /* Reg Y can be regarded as unused if this load is removed */
            Used &= ~REG_Y;
            if (LRI == &LI->A) {
                LI->Y.Flags |= LI_USED_BY_A;
            } else {
                LI->Y.Flags |= LI_USED_BY_X;
            }
        }

        /* If the load offset has a known value, we can just remember and reload
        ** it into the index register later.
        */
        if ((LRI->Flags & LI_CHECK_Y) != 0) {
            if (RegValIsKnown (E->RI->In.RegY)) {
                LRI->Offs = (unsigned char)E->RI->In.RegY;
                LRI->Flags &= ~LI_CHECK_Y;
                LRI->Flags |= LI_RELOAD_Y;
            } else {
                /* We need to check if the src of Y is changed */
                LRI->LoadYIndex = LI->Y.LoadIndex;
            }
        }

        /* Watch for any change of the load target */
        if ((LRI->Flags & LI_CHECK_ARG) != 0) {
            LRI->LoadEntry = CS_GetEntry (S, I);
        }

        if (LRI->LoadYIndex >= 0) {
            LRI->LoadYEntry = CS_GetEntry (S, LRI->LoadYIndex);
        } else {
            LRI->LoadYEntry = 0;
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
        Tgt->ChgIndex   = I;
        Tgt->Offs       = Src->Offs;
        Tgt->Flags      = Src->Flags;

    } else if (CE_IsCallTo (E, "ldaxysp") && RegValIsKnown (E->RI->In.RegY)) {

        /* Both registers set, Y changed */
        LI->A.LoadIndex = I;
        LI->A.ChgIndex  = I;
        LI->A.Flags     = (LI_LOAD_INSN | LI_DIRECT | LI_RELOAD_Y | LI_SP);
        LI->A.Offs      = (unsigned char) E->RI->In.RegY - 1;

        LI->X.LoadIndex = I;
        LI->X.ChgIndex  = I;
        LI->X.Flags     = (LI_LOAD_INSN | LI_DIRECT | LI_RELOAD_Y | LI_SP);
        LI->X.Offs      = (unsigned char) E->RI->In.RegY;

        /* Reg Y can be regarded as unused if this load is removed */
        Used &= ~REG_Y;
        LI->Y.Flags |= LI_USED_BY_A | LI_USED_BY_X;

    } else {
        HonourUseAndChg (&LI->A, REG_A, E, I);
        HonourUseAndChg (&LI->X, REG_X, E, I);
        HonourUseAndChg (&LI->Y, REG_Y, E, I);
    }

    return Used;
}



void SetDontRemoveEntryFlag (LoadRegInfo* LRI)
/* Flag the entry as non-removable according to register flags */
{
    if (LRI->Flags & LI_DONT_REMOVE) {
        if (LRI->LoadEntry != 0) {
            LRI->LoadEntry->Flags |= CEF_DONT_REMOVE;

            /* If the load requires Y, then Y shouldn't be removed either */
            if (LRI->LoadYEntry != 0) {
                LRI->LoadYEntry->Flags |= CEF_DONT_REMOVE;
            }
        }
    }
}



void ResetDontRemoveEntryFlag (LoadRegInfo* LRI)
/* Unflag the entry as non-removable according to register flags */
{
    if (LRI->LoadEntry != 0) {
        LRI->LoadEntry->Flags &= ~CEF_DONT_REMOVE;
    }

    if (LRI->LoadYEntry != 0) {
        LRI->LoadYEntry->Flags &= ~CEF_DONT_REMOVE;
    }

    if (LRI->ChgEntry != 0) {
        LRI->ChgEntry->Flags &= ~CEF_DONT_REMOVE;
    }
}



void SetDontRemoveEntryFlags (StackOpData* D)
/* Flag the entries as non-removable according to register flags */
{
    SetDontRemoveEntryFlag (&D->Lhs.A);
    SetDontRemoveEntryFlag (&D->Lhs.X);
    SetDontRemoveEntryFlag (&D->Lhs.Y);
    SetDontRemoveEntryFlag (&D->Rhs.A);
    SetDontRemoveEntryFlag (&D->Rhs.X);
    SetDontRemoveEntryFlag (&D->Rhs.Y);
    SetDontRemoveEntryFlag (&D->Rv.A);
    SetDontRemoveEntryFlag (&D->Rv.X);
    SetDontRemoveEntryFlag (&D->Rv.Y);
}



void ResetDontRemoveEntryFlags (StackOpData* D)
/* Unflag the entries as non-removable according to register flags */
{
    ResetDontRemoveEntryFlag (&D->Lhs.A);
    ResetDontRemoveEntryFlag (&D->Lhs.X);
    ResetDontRemoveEntryFlag (&D->Lhs.Y);
    ResetDontRemoveEntryFlag (&D->Rhs.A);
    ResetDontRemoveEntryFlag (&D->Rhs.X);
    ResetDontRemoveEntryFlag (&D->Rhs.Y);
    ResetDontRemoveEntryFlag (&D->Rv.A);
    ResetDontRemoveEntryFlag (&D->Rv.X);
    ResetDontRemoveEntryFlag (&D->Rv.Y);
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
    ClearLoadInfo (&Data->Rv);

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

            if ((LI->X.Flags & LI_CHECK_Y) == 0) {
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
                X = NewCodeEntry (OPC, LI->X.LoadEntry->AM, LI->X.LoadEntry->Arg, 0, D->OpEntry->LI);
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
    if ((LI->A.Flags & LI_REMOVE) == LI_REMOVE) {
        if (LI->A.LoadIndex >= 0 &&
            (LI->A.LoadEntry->Flags & CEF_DONT_REMOVE) == 0) {
            DelEntry (D, LI->A.LoadIndex);
            LI->A.LoadEntry = 0;
        }
        if (LI->A.LoadYIndex >= 0 &&
            (LI->A.LoadYEntry->Flags & CEF_DONT_REMOVE) == 0) {
            DelEntry (D, LI->A.LoadYIndex);
        }
        if (LI->A.ChgIndex >= 0 &&
            (LI->A.ChgEntry->Flags & CEF_DONT_REMOVE) == 0) {
            DelEntry (D, LI->A.ChgIndex);
        }
    }

    if (LI->A.LoadEntry != 0                &&
        (LI->A.Flags & LI_RELOAD_Y) != 0    &&
        LI->A.LoadYIndex >= 0) {
        /* If an entry is using Y and not removed, then its Y load mustn't be removed */
        LI->A.LoadYEntry->Flags |= CEF_DONT_REMOVE;
    }

    if ((LI->X.Flags & LI_REMOVE) == LI_REMOVE) {
        if (LI->X.LoadIndex >= 0 &&
            (LI->X.LoadEntry->Flags & CEF_DONT_REMOVE) == 0) {
            DelEntry (D, LI->X.LoadIndex);
            LI->X.LoadEntry = 0;
        }
        if (LI->X.LoadYIndex >= 0 &&
            (LI->X.LoadYEntry->Flags & CEF_DONT_REMOVE) == 0) {
            DelEntry (D, LI->X.LoadYIndex);
        }
        if (LI->X.ChgIndex >= 0 &&
            (LI->X.ChgEntry->Flags & CEF_DONT_REMOVE) == 0) {
            DelEntry (D, LI->X.ChgIndex);
        }
    }

    if (LI->X.LoadEntry != 0                &&
        (LI->X.Flags & LI_RELOAD_Y) != 0    &&
        LI->X.LoadYIndex >= 0) {
        /* If an entry is using Y and not removed, then its Y load mustn't be removed */
        LI->X.LoadYEntry->Flags |= CEF_DONT_REMOVE;
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
    static const char* const Tab[] = {
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
        "ldaidx",
        "ldauidx",
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



/*****************************************************************************/
/*                            Load tracking code                             */
/*****************************************************************************/



/*****************************************************************************/
/*                                  Helpers                                  */
/*****************************************************************************/



const char* GetZPName (unsigned ZPLoc)
/* Get the name strings of certain known ZP Regs */
{
    if ((ZPLoc & REG_TMP1) != 0) {
        return "tmp1";
    }
    if ((ZPLoc & REG_PTR1_LO) != 0) {
        return "ptr1";
    }
    if ((ZPLoc & REG_PTR1_HI) != 0) {
        return "ptr1+1";
    }
    if ((ZPLoc & REG_PTR2_LO) != 0) {
        return "ptr2";
    }
    if ((ZPLoc & REG_PTR2_HI) != 0) {
        return "ptr2+1";
    }
    if ((ZPLoc & REG_SREG_LO) != 0) {
        return "sreg";
    }
    if ((ZPLoc & REG_SREG_HI) != 0) {
        return "sreg+1";
    }
    if ((ZPLoc & REG_SAVE_LO) != 0) {
        return "save";
    }
    if ((ZPLoc & REG_SAVE_HI) != 0) {
        return "save+1";
    }
    if ((ZPLoc & REG_SP_LO) != 0) {
        return "sp";
    }
    if ((ZPLoc & REG_SP_HI) != 0) {
        return "sp+1";
    }

    return 0;
}

unsigned FindAvailableBackupLoc (BackupInfo* B, unsigned Type)
/* Find a ZP loc for storing the backup and fill in the info.
** The allowed types are specified with the Type parameter.
** For convenience, all types are aloowed if none is specified.
** Return the type of the found loc.
*/
{
    unsigned SizeType = Type & BU_SIZE_MASK;
    Type &= BU_TYPE_MASK;
    if (Type == 0) {
        Type = BU_TYPE_MASK;
    }

    if (SizeType == BU_B8 && (Type & BU_REG) != 0 && (B->ZPUsage & REG_Y) == 0) {
        /* Use the Y Reg only */
        B->Type = BU_REG | SizeType;
        B->Where = REG_Y;
        B->ZPUsage |= REG_Y;
        return B->Type;
    }

    if (SizeType == BU_B8 && (Type & BU_ZP) != 0) {
        /* For now we only check for tmp1 and sreg */
        if ((B->ZPUsage & REG_TMP1) == 0) {
            B->Type = BU_ZP | BU_B8;
            B->Where = REG_TMP1;
            B->ZPUsage |= REG_TMP1;
            return B->Type;
        }
        if ((B->ZPUsage & REG_SREG_LO) == 0) {
            B->Type = BU_ZP | BU_B8;
            B->Where = REG_SREG_LO;
            B->ZPUsage |= REG_SREG_LO;
            return B->Type;
        }
        if ((B->ZPUsage & REG_SREG_HI) == 0) {
            B->Type = BU_ZP | BU_B8;
            B->Where = REG_SREG_HI;
            B->ZPUsage |= REG_SREG_HI;
            return B->Type;
        }
    }

    if (SizeType == BU_B16 && (Type & BU_ZP) != 0) {
        /* For now we only check for ptr1, sreg and ptr2 */
        if ((B->ZPUsage & REG_PTR1) == 0) {
            B->Type = BU_ZP | BU_B16;
            B->Where = REG_PTR1;
            B->ZPUsage |= REG_PTR1;
            return B->Type;
        }
        if ((B->ZPUsage & REG_SREG) == 0) {
            B->Type = BU_ZP | BU_B16;
            B->Where = REG_SREG;
            B->ZPUsage |= REG_SREG;
            return B->Type;
        }
        if ((B->ZPUsage & REG_PTR2) == 0) {
            B->Type = BU_ZP | BU_B16;
            B->Where = REG_PTR2;
            B->ZPUsage |= REG_PTR2;
            return B->Type;
        }
    }

    if (SizeType == BU_B24 && (Type & BU_ZP) != 0) {
        /* For now we only check for certain combinations of
        ** tmp1 + (ptr1, sreg or ptr2).
        */
        if ((B->ZPUsage & (REG_TMP1 | REG_PTR1)) == 0) {
            B->Type = BU_ZP | BU_B24;
            B->Where = REG_TMP1 | REG_PTR1;
            B->ZPUsage |= REG_TMP1 | REG_PTR1;
            return B->Type;
        }
        if ((B->ZPUsage & (REG_TMP1 | REG_SREG)) == 0) {
            B->Type = BU_ZP | BU_B24;
            B->Where = REG_TMP1 | REG_SREG;
            B->ZPUsage |= REG_TMP1 | REG_SREG;
            return B->Type;
        }
        if ((B->ZPUsage & (REG_TMP1 | REG_PTR2)) == 0) {
            B->Type = BU_ZP | BU_B24;
            B->Where = REG_TMP1 | REG_PTR2;
            B->ZPUsage |= REG_TMP1 | REG_PTR2;
            return B->Type;
        }
    }

    if (SizeType < BU_B32 && (Type & BU_SP6502) != 0) {
        /* Even for BU_B24, we just push/pop all 3 of AXY */
        B->Type = BU_SP6502 | BU_B16;
        B->Where = 0;
        return B->Type;
    }

    if (SizeType != BU_B24 && SizeType <= BU_B32 && (Type & BU_SP) != 0) {
        /* We may also use pusha/popa, pushax/popax and pusheax/popeax */
        B->Type = BU_SP | SizeType;
        B->Where = 0;
        return B->Type;
    }

    /* No available */
    return BU_UNKNOWN;
}



void AdjustEntryIndices (Collection* Indices, int Index, int Change)
/* Adjust a load register info struct after deleting or inserting successive
** entries with a given index.
*/
{
    int I;
    int* IndexPtr;

    if (Change > 0) {
        /* Insertion */
        for (I = 0; I < (int)CollCount (Indices); ++I) {
            IndexPtr = CollAtUnchecked (Indices, I);
            if (Index <= *IndexPtr) {
                *IndexPtr += Change;
            }
        }
    } else if (Change < 0) {
        /* Deletion */
        for (I = 0; I < (int)CollCount (Indices); ++I) {
            IndexPtr = CollAtUnchecked (Indices, I);
            if (Index <= *IndexPtr + Change) {
                *IndexPtr += Change;
            } else if (Index <= *IndexPtr) {
                /* Has been removed */
                *IndexPtr = -1;
                //CollDelete (Indices, I);
                --I;
            }
        }
    }
}



void DelEntryIdx (CodeSeg* S, int Idx, Collection* Indices)
/* Delete an entry and adjust Indices if necessary */
{
    CS_DelEntry (S, Idx);
    AdjustEntryIndices (Indices, Idx, -1);
}



void DelEntriesIdx (CodeSeg* S, int Idx, int Count, Collection* Indices)
/* Delete entries and adjust Indices if necessary */
{
    CS_DelEntries (S, Idx, Count);
    AdjustEntryIndices (Indices, Idx, -Count);
}



void RemoveFlaggedRegLoads (CodeSeg* S, LoadRegInfo* LRI, Collection* Indices)
/* Remove flagged register load insns */
{
    if ((LRI->Flags & LI_REMOVE) == LI_REMOVE) {
        if (LRI->LoadIndex >= 0 &&
            (LRI->LoadEntry->Flags & CEF_DONT_REMOVE) == 0) {
            DelEntryIdx (S, LRI->LoadIndex, Indices);
            LRI->LoadEntry = 0;
        }
        if (LRI->LoadYIndex >= 0 &&
            (LRI->LoadYEntry->Flags & CEF_DONT_REMOVE) == 0) {
            DelEntryIdx (S, LRI->LoadYIndex, Indices);
        }
        if (LRI->ChgIndex >= 0 &&
            (LRI->ChgEntry->Flags & CEF_DONT_REMOVE) == 0) {
            DelEntryIdx (S, LRI->ChgIndex, Indices);
        }
    }

    if (LRI->LoadEntry != 0 &&
        (LRI->Flags & LI_RELOAD_Y) != 0 &&
        LRI->LoadYIndex >= 0) {
        /* If an entry is using Y and not removed, then its Y load mustn't be removed */
        LRI->LoadYEntry->Flags |= CEF_DONT_REMOVE;
    }
}


void RemoveFlaggedLoads (CodeSeg* S, LoadInfo* LI, Collection* Indices)
/* Remove flagged load insns */
{
    RemoveFlaggedRegLoads (S, &LI->A, Indices);
    RemoveFlaggedRegLoads (S, &LI->X, Indices);
    RemoveFlaggedRegLoads (S, &LI->Y, Indices);
}



static int BackupAAt (CodeSeg* S, BackupInfo* B, int Idx, Collection* Indices, int After)
/* Backup the content of A Before or After the specified index Idx depending on the After param */
{
    CodeEntry* E;
    CodeEntry* X;
    int OldIdx;

    /* Adjust the insertion point if necessary */
    if (After) {
        ++Idx;
    }
    OldIdx = Idx;

    /* Cannot insert after the last insn */
    CHECK ((unsigned)Idx < CollCount (&S->Entries));

    /* Get the entry at Idx */
    E = CS_GetEntry (S, Idx);

    if (E->RI != 0 && RegValIsKnown (E->RI->In.RegA)) {
        /* Just memorize the value */
        B->Type = BU_IMM | BU_B8;
        B->Imm = E->RI->In.RegA;

    } else {
        FindAvailableBackupLoc (B, BU_B8);
        switch (B->Type & BU_TYPE_MASK) {
        case BU_REG:
            if ((B->Where & REG_X) != 0) {
                X = NewCodeEntry (OP65_TAX, AM65_IMP, 0, 0, E->LI);
                CS_InsertEntry (S, X, Idx++);
                break;
            } else if ((B->Where & REG_Y) != 0) {
                X = NewCodeEntry (OP65_TAY, AM65_IMP, 0, 0, E->LI);
                CS_InsertEntry (S, X, Idx++);
                break;
            }
            break;

        case BU_ZP:
            X = NewCodeEntry (OP65_STA, AM65_ZP, GetZPName (B->Where), 0, E->LI);
            CS_InsertEntry (S, X, Idx++);
           break;

        case BU_SP6502:
            X = NewCodeEntry (OP65_PHA, AM65_IMP, 0, 0, E->LI);
            CS_InsertEntry (S, X, Idx++);
            break;

        case BU_SP:
            if ((B->ZPUsage & REG_Y) == 0) {
                X = NewCodeEntry (OP65_JSR, AM65_ABS, "pusha", 0, E->LI);
                CS_InsertEntry (S, X, Idx++);
                break;
            }

            /* Unable to do backup */
            return 0;

        default:
            /* Unable to do backup */
            return 0;
        }
    }

    /* Adjust all indices at once */
    AdjustEntryIndices (Indices, OldIdx, Idx - OldIdx);

    /* Move labels if it was an insertion before Idx */
    if (!After) {
        CS_MoveLabels (S, E, CS_GetEntry (S, OldIdx));
    }

    /* Done */
    return 1;
}



static int BackupXAt (CodeSeg* S, BackupInfo* B, int Idx, Collection* Indices, int After)
/* Backup the content of X before or after the specified index Idx depending on the param After */
{
    CodeEntry* E;
    CodeEntry* X;
    int OldIdx;

    /* Adjust the insertion point if necessary */
    if (After) {
        ++Idx;
    }
    OldIdx = Idx;

    /* Cannot insert after the last insn */
    CHECK ((unsigned)Idx < CollCount (&S->Entries));

    /* Get the entry at Idx */
    E = CS_GetEntry (S, Idx);

    if (E->RI != 0 && RegValIsKnown (E->RI->In.RegX)) {
        /* Just memorize the value */
        B->Type = BU_IMM | BU_B8;
        B->Imm = E->RI->In.RegX;

    } else {
        FindAvailableBackupLoc (B, BU_B8);
        switch (B->Type & BU_TYPE_MASK) {
        case BU_ZP:
            X = NewCodeEntry (OP65_STX, AM65_ZP, GetZPName(B->Where), 0, E->LI);
            CS_InsertEntry (S, X, Idx++);
           break;

        case BU_SP6502:
            if ((B->ZPUsage & REG_A) == 0) {
                X = NewCodeEntry (OP65_TXA, AM65_IMP, 0, 0, E->LI);
                CS_InsertEntry (S, X, Idx++);
                X = NewCodeEntry (OP65_PHA, AM65_IMP, 0, 0, E->LI);
                CS_InsertEntry (S, X, Idx++);
                break;
            }

            /* Unable to do backup */
            return 0;

        case BU_SP:
            if ((B->ZPUsage & REG_A) == 0) {
                X = NewCodeEntry (OP65_TXA, AM65_IMP, 0, 0, E->LI);
                CS_InsertEntry (S, X, Idx++);
                X = NewCodeEntry (OP65_JSR, AM65_ABS, "pusha", 0, E->LI);
                CS_InsertEntry (S, X, Idx++);
                break;
            }

            /* Unable to do backup */
            return 0;

        case BU_REG:
            /* Fallthrough */
        default:

            /* Unable to do backup */
            return 0;
        }
    }

    /* Adjust all indices at once */
    AdjustEntryIndices (Indices, OldIdx, Idx - OldIdx);

    /* Move labels if it was an insertion before Idx */
    if (!After) {
        CS_MoveLabels (S, E, CS_GetEntry (S, OldIdx));
    }

    /* Done */
    return 1;
}



static int BackupYAt (CodeSeg* S, BackupInfo* B, int Idx, Collection* Indices, int After)
/* Backup the content of Y before or after the specified index Idx depending on the param After */
{
    CodeEntry* E;
    CodeEntry* X;
    int OldIdx;

    /* Adjust the insertion point if necessary */
    if (After) {
        ++Idx;
    }
    OldIdx = Idx;

    /* Cannot insert after the last insn */
    CHECK ((unsigned)Idx < CollCount (&S->Entries));

    /* Get the entry at Idx */
    E = CS_GetEntry (S, Idx);

    if (E->RI != 0 && RegValIsKnown (E->RI->In.RegY)) {
        /* Just memorize the value */
        B->Type = BU_IMM | BU_B8;
        B->Imm = E->RI->In.RegY;

    } else {
        FindAvailableBackupLoc (B, BU_B8);
        switch (B->Type & BU_TYPE_MASK) {
        case BU_ZP:
            X = NewCodeEntry (OP65_STY, AM65_ZP, GetZPName(B->Where), 0, E->LI);
            CS_InsertEntry (S, X, Idx++);
           break;

        case BU_SP6502:
            if ((B->ZPUsage & REG_A) == 0) {
                X = NewCodeEntry (OP65_TYA, AM65_IMP, 0, 0, E->LI);
                CS_InsertEntry (S, X, Idx++);
                X = NewCodeEntry (OP65_PHA, AM65_IMP, 0, 0, E->LI);
                CS_InsertEntry (S, X, Idx++);
                break;
            }

            /* Unable to do backup */
            return 0;

        case BU_SP:
            if ((B->ZPUsage & REG_A) == 0) {
                X = NewCodeEntry (OP65_TYA, AM65_IMP, 0, 0, E->LI);
                CS_InsertEntry (S, X, Idx++);
                X = NewCodeEntry (OP65_JSR, AM65_ABS, "pusha", 0, E->LI);
                CS_InsertEntry (S, X, Idx++);
                break;
            }

            /* Unable to do backup */
            return 0;

        case BU_REG:
            /* Fallthrough */
        default:

            /* Unable to do backup */
            return 0;
        }
    }

    /* Adjust all indices at once */
    AdjustEntryIndices (Indices, OldIdx, Idx - OldIdx);

    /* Move labels if it was an insertion before Idx */
    if (!After) {
        CS_MoveLabels (S, E, CS_GetEntry (S, OldIdx));
    }

    /* Done */
    return 1;
}



static int BackupAXAt (CodeSeg* S, BackupInfo* B, int Idx, Collection* Indices, int After)
/* Backup the content of AX Before or After the specified index Idx depending on the After param */
{
    CodeEntry* E;
    CodeEntry* X;
    int OldIdx;
    StrBuf Arg;

    SB_Init (&Arg);

    /* Adjust the insertion point if necessary */
    if (After) {
        ++Idx;
    }
    OldIdx = Idx;

    /* Cannot insert after the last insn */
    CHECK ((unsigned)Idx < CollCount (&S->Entries));

    /* Get the entry at Idx */
    E = CS_GetEntry (S, Idx);

    if (E->RI != 0 && RegValIsKnown (E->RI->In.RegA) && RegValIsKnown (E->RI->In.RegX)) {
        /* Just memorize the value */
        B->Type = BU_IMM | BU_B16;
        B->Imm = E->RI->In.RegA | (E->RI->In.RegX << 8);

    } else {
        FindAvailableBackupLoc (B, BU_B16);
        switch (B->Type & BU_TYPE_MASK) {
        case BU_ZP:
            SB_AppendStr (&Arg, GetZPName (B->Where));
            SB_Terminate (&Arg);
            X = NewCodeEntry (OP65_STA, AM65_ZP, SB_GetConstBuf (&Arg), 0, E->LI);
            CS_InsertEntry (S, X, Idx++);
            SB_AppendStr (&Arg, "+1");
            SB_Terminate (&Arg);
            X = NewCodeEntry (OP65_STX, AM65_ZP, SB_GetConstBuf (&Arg), 0, E->LI);
            CS_InsertEntry (S, X, Idx++);
            break;

        case BU_SP6502:
            if ((B->ZPUsage & REG_A) == 0) {
                X = NewCodeEntry (OP65_PHA, AM65_IMP, 0, 0, E->LI);
                CS_InsertEntry (S, X, Idx++);
                X = NewCodeEntry (OP65_TXA, AM65_IMP, 0, 0, E->LI);
                CS_InsertEntry (S, X, Idx++);
                X = NewCodeEntry (OP65_PHA, AM65_IMP, 0, 0, E->LI);
                CS_InsertEntry (S, X, Idx++);
                break;
            }

            /* Unable to do backup */
            return 0;

        case BU_SP:
            if ((B->ZPUsage & REG_Y) == 0) {
                X = NewCodeEntry (OP65_JSR, AM65_ABS, "pushax", 0, E->LI);
                CS_InsertEntry (S, X, Idx++);
                break;
            }

            /* Unable to do backup */
            return 0;

        case BU_REG:
            /* Fallthrough */

        default:
            /* Unable to do backup */
            return 0;
        }
    }

    /* Adjust all indices at once */
    AdjustEntryIndices (Indices, OldIdx, Idx - OldIdx);

    /* Move labels if it was an insertion before Idx */
    if (!After) {
        CS_MoveLabels (S, E, CS_GetEntry (S, OldIdx));
    }

    SB_Done (&Arg);

    /* Done */
    return 1;
}



static int BackupAXYAt (CodeSeg* S, BackupInfo* B, int Idx, Collection* Indices, int After)
/* Backup the content of AXY before or after the specified index Idx depending on the param After.
** This doesn't allow separating the backup of Y from that of AX for now.
*/
{
    CodeEntry* E;
    CodeEntry* X;
    int OldIdx;
    StrBuf Arg;

    SB_Init (&Arg);

    /* Adjust the insertion point if necessary */
    if (After) {
        ++Idx;
    }
    OldIdx = Idx;

    /* Cannot insert after the last insn */
    CHECK ((unsigned)Idx < CollCount (&S->Entries));

    /* Get the entry at Idx */
    E = CS_GetEntry (S, Idx);

    if (E->RI != 0 &&
        RegValIsKnown (E->RI->In.RegA) &&
        RegValIsKnown (E->RI->In.RegX) &&
        RegValIsKnown (E->RI->In.RegY)) {
        /* Just memorize the value */
        B->Type = BU_IMM | BU_B24;
        B->Imm = E->RI->In.RegA | (E->RI->In.RegX << 8) | (E->RI->In.RegY << 16);

    } else {
        FindAvailableBackupLoc (B, BU_B24);
        switch (B->Type & BU_TYPE_MASK) {
        case BU_ZP:
            CHECK ((B->Where & REG_TMP1) != 0);
            SB_AppendStr (&Arg, GetZPName (B->Where & ~REG_TMP1));
            SB_Terminate (&Arg);
            X = NewCodeEntry (OP65_STA, AM65_ZP, SB_GetConstBuf (&Arg), 0, E->LI);
            CS_InsertEntry (S, X, Idx++);
            SB_AppendStr (&Arg, "+1");
            SB_Terminate (&Arg);
            X = NewCodeEntry (OP65_STX, AM65_ZP, SB_GetConstBuf (&Arg), 0, E->LI);
            CS_InsertEntry (S, X, Idx++);
            X = NewCodeEntry (OP65_STY, AM65_ZP, GetZPName (B->Where & REG_TMP1), 0, E->LI);
            CS_InsertEntry(S, X, Idx++);
            break;

        case BU_SP6502:
            if ((B->ZPUsage & REG_A) == 0) {
                X = NewCodeEntry (OP65_PHA, AM65_IMP, 0, 0, E->LI);
                CS_InsertEntry (S, X, Idx++);
                X = NewCodeEntry (OP65_TXA, AM65_IMP, 0, 0, E->LI);
                CS_InsertEntry (S, X, Idx++);
                X = NewCodeEntry (OP65_PHA, AM65_IMP, 0, 0, E->LI);
                CS_InsertEntry (S, X, Idx++);
                X = NewCodeEntry (OP65_TYA, AM65_IMP, 0, 0, E->LI);
                CS_InsertEntry (S, X, Idx++);
                X = NewCodeEntry (OP65_PHA, AM65_IMP, 0, 0, E->LI);
                CS_InsertEntry (S, X, Idx++);
                break;
            }

            /* Unable to do backup */
            return 0;

        case BU_SP:
            if ((B->ZPUsage & REG_AY) == 0) {
                X = NewCodeEntry (OP65_JSR, AM65_ABS, "pushax", 0, E->LI);
                CS_InsertEntry (S, X, Idx++);
                X = NewCodeEntry (OP65_TYA, AM65_IMP, 0, 0, E->LI);
                CS_InsertEntry (S, X, Idx++);
                X = NewCodeEntry (OP65_JSR, AM65_ABS, "pusha", 0, E->LI);
                CS_InsertEntry (S, X, Idx++);
                break;
            }

            /* Unable to do backup */
            return 0;

        case BU_REG:
            /* Fallthrough */

        default:
            /* Unable to do backup */
            return 0;
        }
    }

    /* Adjust all indices at once */
    AdjustEntryIndices (Indices, OldIdx, Idx - OldIdx);

    /* Move labels if it was an insertion before Idx */
    if (!After) {
        CS_MoveLabels (S, E, CS_GetEntry (S, OldIdx));
    }

    SB_Done (&Arg);

    /* Done */
    return 1;
}



int BackupABefore (CodeSeg* S, BackupInfo* B, int Idx, Collection* Indices)
/* Backup the content of A Before the specified index Idx */
{
    return BackupAAt (S, B, Idx, Indices, 0);
}



int BackupXBefore (CodeSeg* S, BackupInfo* B, int Idx, Collection* Indices)
/* Backup the content of X before the specified index Idx */
{
    return BackupXAt (S, B, Idx, Indices, 0);
}



int BackupYBefore (CodeSeg* S, BackupInfo* B, int Idx, Collection* Indices)
/* Backup the content of Y before the specified index Idx */
{
    return BackupYAt (S, B, Idx, Indices, 0);
}



int BackupAXBefore (CodeSeg* S, BackupInfo* B, int Idx, Collection* Indices)
/* Backup the content of AX before the specified index Idx */
{
    return BackupAXAt (S, B, Idx, Indices, 0);
}



int BackupAXYBefore (CodeSeg* S, BackupInfo* B, int Idx, Collection* Indices)
/* Backup the content of AXY before the specified index Idx.
** This doesn't allow separating the backup of Y from that of AX for now.
*/
{
    return BackupAXYAt (S, B, Idx, Indices, 0);
}



int BackupAAfter (CodeSeg* S, BackupInfo* B, int Idx, Collection* Indices)
/* Backup the content of A after the specified index Idx */
{
    return BackupAAt (S, B, Idx, Indices, 1);
}



int BackupXAfter (CodeSeg* S, BackupInfo* B, int Idx, Collection* Indices)
/* Backup the content of X after the specified index Idx */
{
    return BackupXAt (S, B, Idx, Indices, 1);
}



int BackupYAfter (CodeSeg* S, BackupInfo* B, int Idx, Collection* Indices)
/* Backup the content of Y after the specified index Idx */
{
    return BackupYAt (S, B, Idx, Indices, 1);
}



int BackupAXAfter (CodeSeg* S, BackupInfo* B, int Idx, Collection* Indices)
/* Backup the content of AX after the specified index Idx */
{
    return BackupAXAt (S, B, Idx, Indices, 1);
}



int BackupAXYAfter (CodeSeg* S, BackupInfo* B, int Idx, Collection* Indices)
/* Backup the content of AXY after the specified index Idx.
** This doesn't allow separating the backup of Y from that of AX for now.
*/
{
    return BackupAXYAt (S, B, Idx, Indices, 1);
}



int RestoreABefore (CodeSeg* S, BackupInfo* B, int Idx, Collection* Indices)
/* Restore the content of Y before the specified index Idx */
{
    CodeEntry* E;
    CodeEntry* X;
    int OldIdx = Idx;

    /* Get the entry at Idx */
    E = CS_GetEntry (S, Idx);

    switch (B->Type & BU_TYPE_MASK) {
    case BU_IMM:
        /* Just use the memorized value */
        X = NewCodeEntry (OP65_LDA, AM65_IMM, MakeHexArg (B->Imm & 0xFF), 0, E->LI);
        CS_InsertEntry (S, X, Idx++);
        break;

    case BU_REG:
        if ((B->Where & REG_X) != 0) {
            X = NewCodeEntry (OP65_TXA, AM65_IMP, 0, 0, E->LI);
            CS_InsertEntry (S, X, Idx++);
        } else if ((B->Where & REG_Y) != 0) {
            X = NewCodeEntry (OP65_TYA, AM65_IMP, 0, 0, E->LI);
            CS_InsertEntry (S, X, Idx++);
        }
        break;

    case BU_ZP:
        X = NewCodeEntry (OP65_LDA, AM65_ZP, GetZPName (B->Where), 0, E->LI);
        CS_InsertEntry (S, X, Idx++);
        break;

    case BU_SP6502:
        X = NewCodeEntry (OP65_PLA, AM65_IMP, 0, 0, E->LI);
        CS_InsertEntry (S, X, Idx++);
        break;

    case BU_SP:
        if ((B->ZPUsage & REG_Y) == 0) {
            X = NewCodeEntry (OP65_JSR, AM65_ABS, "popa", 0, E->LI);
            CS_InsertEntry (S, X, Idx++);
            break;
        }

        /* Unable to restore */
        return 0;

    default:
        /* Unable to restore */
        return 0;
    }

    /* Adjust all indices at once */
    AdjustEntryIndices (Indices, OldIdx, Idx - OldIdx);

    /* Done */
    return 1;
}



int RestoreXBefore (CodeSeg* S, BackupInfo* B, int Idx, Collection* Indices)
/* Restore the content of X before the specified index Idx */
{
    CodeEntry* E;
    CodeEntry* X;
    int OldIdx = Idx;

    /* Get the entry at Idx */
    E = CS_GetEntry (S, Idx);

    switch (B->Type & BU_TYPE_MASK) {
    case BU_IMM:
        /* Just use the memorized value */
        X = NewCodeEntry (OP65_LDX, AM65_IMM, MakeHexArg (B->Imm & 0xFF), 0, E->LI);
        CS_InsertEntry (S, X, Idx++);
        break;

    case BU_REG:
        if ((B->Where & REG_A) != 0) {
            X = NewCodeEntry (OP65_TAX, AM65_IMP, 0, 0, E->LI);
            CS_InsertEntry (S, X, Idx++);
        } else if ((B->Where & REG_Y) != 0) {
            if ((B->ZPUsage & REG_A) == 0) {
                X = NewCodeEntry (OP65_PHA, AM65_IMP, 0, 0, E->LI);
                CS_InsertEntry (S, X, Idx++);
            }
            X = NewCodeEntry (OP65_TYA, AM65_IMP, 0, 0, E->LI);
            CS_InsertEntry (S, X, Idx++);
            X = NewCodeEntry (OP65_TAX, AM65_IMP, 0, 0, E->LI);
            CS_InsertEntry (S, X, Idx++);
            if ((B->ZPUsage & REG_A) == 0) {
                X = NewCodeEntry (OP65_PLA, AM65_IMP, 0, 0, E->LI);
                CS_InsertEntry (S, X, Idx++);
            }
        }
        break;

    case BU_ZP:
        X = NewCodeEntry (OP65_LDX, AM65_ZP, GetZPName (B->Where), 0, E->LI);
        CS_InsertEntry (S, X, Idx++);
        break;

    case BU_SP6502:
        if ((B->ZPUsage & REG_A) == 0) {
            X = NewCodeEntry (OP65_PLA, AM65_IMP, 0, 0, E->LI);
            CS_InsertEntry (S, X, Idx++);
            X = NewCodeEntry (OP65_TAX, AM65_IMP, 0, 0, E->LI);
            CS_InsertEntry (S, X, Idx++);
            break;
        }

        /* Unable to restore */
        return 0;

    case BU_SP:
        if ((B->ZPUsage & REG_A) == 0) {
            X = NewCodeEntry (OP65_JSR, AM65_ABS, "popa", 0, E->LI);
            CS_InsertEntry (S, X, Idx++);
            X = NewCodeEntry (OP65_TAX, AM65_IMP, 0, 0, E->LI);
            CS_InsertEntry (S, X, Idx++);
            break;
        }

        /* Unable to restore */
        return 0;

    default:
        /* Unable to restore */
        return 0;
    }

    /* Adjust all indices at once */
    AdjustEntryIndices (Indices, OldIdx, Idx - OldIdx);

    /* Done */
    return 1;
}



int RestoreYBefore (CodeSeg* S, BackupInfo* B, int Idx, Collection* Indices)
/* Restore the content of Y before the specified index Idx */
{
    CodeEntry* E;
    CodeEntry* X;
    int OldIdx = Idx;

    /* Get the entry at Idx */
    E = CS_GetEntry (S, Idx);

    switch (B->Type & BU_TYPE_MASK) {
    case BU_IMM:
        /* Just use the memorized value */
        X = NewCodeEntry (OP65_LDY, AM65_IMM, MakeHexArg (B->Imm & 0xFF), 0, E->LI);
        CS_InsertEntry (S, X, Idx++);
        break;

    case BU_REG:
        if ((B->Where & REG_A) != 0) {
            X = NewCodeEntry (OP65_TAX, AM65_IMP, 0, 0, E->LI);
            CS_InsertEntry (S, X, Idx++);
        } else if ((B->Where & REG_X) != 0) {
            if ((B->ZPUsage & REG_A) == 0) {
                X = NewCodeEntry (OP65_PHA, AM65_IMP, 0, 0, E->LI);
                CS_InsertEntry (S, X, Idx++);
            }
            X = NewCodeEntry (OP65_TXA, AM65_IMP, 0, 0, E->LI);
            CS_InsertEntry (S, X, Idx++);
            X = NewCodeEntry (OP65_TAY, AM65_IMP, 0, 0, E->LI);
            CS_InsertEntry (S, X, Idx++);
            if ((B->ZPUsage & REG_A) == 0) {
                X = NewCodeEntry (OP65_PLA, AM65_IMP, 0, 0, E->LI);
                CS_InsertEntry (S, X, Idx++);
            }
        }
        break;

    case BU_ZP:
        X = NewCodeEntry (OP65_LDY, AM65_ZP, GetZPName (B->Where), 0, E->LI);
        CS_InsertEntry (S, X, Idx++);
        break;

    case BU_SP6502:
        if ((B->ZPUsage & REG_A) == 0) {
            X = NewCodeEntry (OP65_PLA, AM65_IMP, 0, 0, E->LI);
            CS_InsertEntry (S, X, Idx++);
            X = NewCodeEntry (OP65_TAY, AM65_IMP, 0, 0, E->LI);
            CS_InsertEntry (S, X, Idx++);
            break;
        }

        /* Unable to restore */
        return 0;

    case BU_SP:
        if ((B->ZPUsage & REG_A) == 0) {
            X = NewCodeEntry (OP65_JSR, AM65_ABS, "popa", 0, E->LI);
            CS_InsertEntry (S, X, Idx++);
            X = NewCodeEntry (OP65_TAY, AM65_IMP, 0, 0, E->LI);
            CS_InsertEntry (S, X, Idx++);
            break;
        }

        /* Unable to restore */
        return 0;

    default:
        /* Unable to restore */
        return 0;
    }

    /* Adjust all indices at once */
    AdjustEntryIndices (Indices, OldIdx, Idx - OldIdx);

    /* Done */
    return 1;
}



int RestoreAXBefore (CodeSeg* S, BackupInfo* B, int Idx, Collection* Indices)
/* Restore the content of AX before the specified index Idx */
{
    CodeEntry* E;
    CodeEntry* X;
    StrBuf Arg;
    int OldIdx = Idx;

    SB_Init (&Arg);

    /* Get the entry at Idx */
    E = CS_GetEntry (S, Idx);

    switch (B->Type & BU_TYPE_MASK) {
    case BU_REG:
        /* Just use the memorized value */
        X = NewCodeEntry (OP65_LDA, AM65_IMM, MakeHexArg (B->Imm & 0xFF), 0, E->LI);
        CS_InsertEntry (S, X, Idx++);
        X = NewCodeEntry (OP65_LDX, AM65_IMM, MakeHexArg ((B->Imm >> 8) & 0xFF), 0, E->LI);
        CS_InsertEntry (S, X, Idx++);
        break;

    case BU_ZP:
        SB_AppendStr (&Arg, GetZPName (B->Where));
        SB_Terminate (&Arg);
        X = NewCodeEntry (OP65_LDA, AM65_ZP, SB_GetConstBuf (&Arg), 0, E->LI);
        CS_InsertEntry (S, X, Idx++);
        SB_AppendStr (&Arg, "+1");
        SB_Terminate (&Arg);
        X = NewCodeEntry (OP65_LDX, AM65_ZP, SB_GetConstBuf (&Arg), 0, E->LI);
        CS_InsertEntry (S, X, Idx++);
        break;

    case BU_SP6502:
        if ((B->ZPUsage & REG_A) == 0) {
            X = NewCodeEntry (OP65_PLA, AM65_IMP, 0, 0, E->LI);
            CS_InsertEntry (S, X, Idx++);
            X = NewCodeEntry (OP65_TAX, AM65_IMP, 0, 0, E->LI);
            CS_InsertEntry (S, X, Idx++);
            X = NewCodeEntry (OP65_PLA, AM65_IMP, 0, 0, E->LI);
            CS_InsertEntry (S, X, Idx++);
            break;
        }

        /* Unable to restore */
        return 0;

    case BU_SP:
        if ((B->ZPUsage & REG_Y) == 0) {
            X = NewCodeEntry (OP65_JSR, AM65_ABS, "popax", 0, E->LI);
            CS_InsertEntry (S, X, Idx++);
            break;
        }

        /* Unable to restore */
        return 0;

    default:
        /* Unable to restore */
        return 0;
    }

    /* Adjust all indices at once */
    AdjustEntryIndices (Indices, OldIdx, Idx - OldIdx);

    SB_Done (&Arg);

    return 1;
}



int RestoreAXYBefore (CodeSeg* S, BackupInfo* B, int Idx, Collection* Indices)
/* Restore the content of AXY before the specified index Idx.
** This only allows restore from compacted AXY backup for now.
*/
{
    CodeEntry* E;
    CodeEntry* X;
    int OldIdx = Idx;
    StrBuf Arg;

    SB_Init (&Arg);

    /* Get the entry at Idx */
    E = CS_GetEntry (S, Idx);

    switch (B->Type & BU_TYPE_MASK) {
    case BU_IMM:
        /* Just use memorized value */
        X = NewCodeEntry (OP65_LDA, AM65_IMM, MakeHexArg (B->Imm & 0xFF), 0, E->LI);
        CS_InsertEntry (S, X, Idx++);
        X = NewCodeEntry (OP65_LDX, AM65_IMM, MakeHexArg ((B->Imm >> 8) & 0xFF), 0, E->LI);
        CS_InsertEntry (S, X, Idx++);
        X = NewCodeEntry (OP65_LDY, AM65_IMM, MakeHexArg ((B->Imm >> 16) & 0xFF), 0, E->LI);
        CS_InsertEntry (S, X, Idx++);
        break;

    case BU_ZP:
        CHECK ((B->Where & REG_TMP1) != 0);
        SB_AppendStr (&Arg, GetZPName (B->Where & ~REG_TMP1));
        SB_Terminate (&Arg);
        X = NewCodeEntry (OP65_LDA, AM65_ZP, SB_GetConstBuf (&Arg), 0, E->LI);
        CS_InsertEntry (S, X, Idx++);
        SB_AppendStr (&Arg, "+1");
        SB_Terminate (&Arg);
        X = NewCodeEntry (OP65_LDX, AM65_ZP, SB_GetConstBuf (&Arg), 0, E->LI);
        CS_InsertEntry (S, X, Idx++);
        X = NewCodeEntry (OP65_LDY, AM65_ZP, GetZPName (B->Where & REG_TMP1), 0, E->LI);
        CS_InsertEntry (S, X, Idx++);
        break;

    case BU_SP6502:
        X = NewCodeEntry (OP65_PLA, AM65_IMP, 0, 0, E->LI);
        CS_InsertEntry (S, X, Idx++);
        X = NewCodeEntry (OP65_TAY, AM65_IMP, 0, 0, E->LI);
        CS_InsertEntry (S, X, Idx++);
        X = NewCodeEntry (OP65_PLA, AM65_IMP, 0, 0, E->LI);
        CS_InsertEntry (S, X, Idx++);
        X = NewCodeEntry (OP65_TAX, AM65_IMP, 0, 0, E->LI);
        CS_InsertEntry (S, X, Idx++);
        X = NewCodeEntry (OP65_PLA, AM65_IMP, 0, 0, E->LI);
        CS_InsertEntry (S, X, Idx++);
        break;

    case BU_SP:
        X = NewCodeEntry (OP65_JSR, AM65_ABS, "popa", 0, E->LI);
        CS_InsertEntry (S, X, Idx++);
        X = NewCodeEntry (OP65_TAY, AM65_IMP, 0, 0, E->LI);
        CS_InsertEntry (S, X, Idx++);
        X = NewCodeEntry (OP65_JSR, AM65_ABS, "popax", 0, E->LI);
        CS_InsertEntry (S, X, Idx++);
        break;

    default:
        /* Unable to restorep */
        return 0;
    }

    /* Adjust all indices at once */
    AdjustEntryIndices (Indices, OldIdx, Idx - OldIdx);

    SB_Done (&Arg);

    /* Done */
    return 1;
}



int BackupArgAfter (CodeSeg* S, BackupInfo* B, int Idx, const CodeEntry* E, Collection* Indices)
/* Backup the content of the opc arg of the entry E after the specified index Idx.
** Reg A/Y will be used to transfer the content from a memory location to another
** regardless of whether it is in use.
*/
{
    CodeEntry* X;
    int OldIdx = Idx;
    unsigned ArgSize;
    unsigned Use, Chg;
    StrBuf SrcArg;
    StrBuf DstArg;

    SB_Init (&SrcArg);
    SB_Init (&DstArg);

    /* We only recognize opc with an arg for now, as well as a special case for ldaxysp */
    if ((E->OPC != OP65_JSR || strcmp (E->Arg, "ldaxysp") == 0) &&
        E->AM != AM65_BRA) {
        /* Get size of the arg */
        if ((E->Info & OF_LBRA) != 0 || strcmp (E->Arg, "ldaxysp") == 0) {
            ArgSize = BU_B16;
        } else {
            ArgSize = BU_B8;
        }

        if (E->AM == AM65_IMM && CE_HasNumArg (E)) {
            /* Just memorize the value */
            B->Type = BU_IMM | ArgSize;
            B->Imm = E->Num;

            /* Adjust all indices at once */
            AdjustEntryIndices (Indices, OldIdx + 1, Idx - OldIdx);

            /* Done */
            return 1;

        }

        if (E->Size != 1 && E->AM != AM65_IMP) {

            /* We only recognize opc with an arg for now */
            FindAvailableBackupLoc (B, ArgSize);
            switch (B->Type & BU_TYPE_MASK) {
            case BU_ZP:
                X = NewCodeEntry (OP65_LDA, E->AM, E->Arg, 0, E->LI);
                CS_InsertEntry (S, X, ++Idx);
                SB_AppendStr (&DstArg, GetZPName (B->Where));
                SB_Terminate (&DstArg);
                X = NewCodeEntry (OP65_STA, AM65_ZP, SB_GetConstBuf (&DstArg), 0, E->LI);
                CS_InsertEntry (S, X, ++Idx);
                if (ArgSize == BU_B16) {
                    SB_AppendStr (&SrcArg, E->Arg);
                    SB_AppendStr (&SrcArg, "+1");
                    SB_Terminate (&SrcArg);
                    X = NewCodeEntry (OP65_LDA, E->AM, SB_GetConstBuf (&SrcArg), 0, E->LI);
                    CS_InsertEntry (S, X, ++Idx);
                    SB_AppendStr (&DstArg, "+1");
                    SB_Terminate (&DstArg);
                    X = NewCodeEntry (OP65_STA, AM65_ZP, SB_GetConstBuf (&DstArg), 0, E->LI);
                    CS_InsertEntry (S, X, ++Idx);
                }
                break;

            case BU_REG:
                CHECK (ArgSize == BU_B8 && B->Where == REG_Y);
                if (E->AM == AM65_ZP || E->AM == AM65_ABS) {
                    X = NewCodeEntry (OP65_LDY, E->AM, E->Arg, 0, E->LI);
                    CS_InsertEntry (S, X, ++Idx);
                } else {
                    X = NewCodeEntry (OP65_LDA, E->AM, E->Arg, 0, E->LI);
                    CS_InsertEntry (S, X, ++Idx);
                    X = NewCodeEntry (OP65_TAY, AM65_IMP, 0, 0, E->LI);
                    CS_InsertEntry (S, X, ++Idx);
                }
                break;

            case BU_SP6502:
                X = NewCodeEntry (OP65_LDA, E->AM, E->Arg, 0, E->LI);
                CS_InsertEntry (S, X, ++Idx);
                X = NewCodeEntry (OP65_PHA, AM65_IMP, 0, 0, E->LI);
                CS_InsertEntry (S, X, ++Idx);
                if (ArgSize == BU_B16) {
                    SB_AppendStr (&SrcArg, E->Arg);
                    SB_AppendStr (&SrcArg, "+1");
                    SB_Terminate (&SrcArg);
                    X = NewCodeEntry (OP65_LDA, E->AM, SB_GetConstBuf (&SrcArg), 0, E->LI);
                    CS_InsertEntry (S, X, ++Idx);
                    X = NewCodeEntry (OP65_PHA, AM65_IMP, 0, 0, E->LI);
                    CS_InsertEntry (S, X, ++Idx);
                }
                break;

            case BU_SP:
                X = NewCodeEntry (OP65_LDA, E->AM, E->Arg, 0, E->LI);
                CS_InsertEntry (S, X, ++Idx);
                if (ArgSize != BU_B16) {
                    X = NewCodeEntry (OP65_JSR, AM65_ABS, "pusha", 0, E->LI);
                    CS_InsertEntry (S, X, ++Idx);
                } else {
                    SB_AppendStr (&SrcArg, E->Arg);
                    SB_AppendStr (&SrcArg, "+1");
                    SB_Terminate (&SrcArg);
                    if ((B->ZPUsage & REG_X) == 0) {
                        if (E->AM == AM65_ZP) {
                            X = NewCodeEntry (OP65_LDX, E->AM, SB_GetConstBuf (&SrcArg), 0, E->LI);
                            CS_InsertEntry (S, X, ++Idx);
                            X = NewCodeEntry (OP65_JSR, AM65_ABS, "pushax", 0, E->LI);
                            CS_InsertEntry (S, X, ++Idx);
                        } else {
                            X = NewCodeEntry (OP65_LDA, E->AM, SB_GetConstBuf (&SrcArg), 0, E->LI);
                            CS_InsertEntry (S, X, ++Idx);
                            X = NewCodeEntry (OP65_TAX, AM65_IMP, 0, 0, E->LI);
                            CS_InsertEntry (S, X, ++Idx);
                            X = NewCodeEntry (OP65_JSR, AM65_ABS, "pushax", 0, E->LI);
                            CS_InsertEntry (S, X, ++Idx);
                        }
                    } else {
                        X = NewCodeEntry (OP65_JSR, AM65_ABS, "pusha", 0, E->LI);
                        CS_InsertEntry (S, X, ++Idx);
                        X = NewCodeEntry (OP65_LDA, AM65_ZP, SB_GetConstBuf (&DstArg), 0, E->LI);
                        CS_InsertEntry (S, X, ++Idx);
                        X = NewCodeEntry (OP65_JSR, AM65_ABS, "pusha", 0, E->LI);
                        CS_InsertEntry (S, X, ++Idx);
                    }
                }
                break;
            }

            /* Adjust all indices at once */
            AdjustEntryIndices (Indices, OldIdx + 1, Idx - OldIdx);

            /* Done */
            return 1;
        }
    } else if (E->OPC == OP65_JSR) {
        /* For function calls we load their arguments instead */
        GetFuncInfo (E->Arg, &Use, &Chg);
        if ((Use & ~REG_AXY) == 0) {
            if (Use == REG_A) {
                ArgSize = BU_B8;
                return BackupAAfter (S, B, Idx, Indices);
            } else if (Use == REG_AX) {
                ArgSize = BU_B16;
                return BackupAXAfter (S, B, Idx, Indices);
            } else if (Use == REG_AXY) {
                /* This is actually a 16-bit word plus a 8-bit byte */
                ArgSize = BU_B24;
                return BackupAXYAfter (S, B, Idx, Indices);
            }

            /* We don't recognize other usage patterns for now */
        }
    }

    SB_Done (&SrcArg);
    SB_Done (&DstArg);

    /* Unable to do backup */
    return 0;
}

static int LoadAAt (CodeSeg* S, int Idx, const LoadRegInfo* LRI, Collection* Indices, int After)
/* Reload into A the same arg according to LoadRegInfo before or after Idx
** depending on the After param.
*/
{
    CodeEntry* E;
    CodeEntry* O;       /* Old entry at Idx */
    CodeEntry* X;
    int Success = 0;
    int OldIdx;
    unsigned Use, Chg;

    /* Adjust the insertion point if necessary */
    if (After) {
        ++Idx;
    }
    OldIdx = Idx;

    E = LRI->LoadEntry;
    CHECK (E != 0);

    O = CS_GetEntry (S, OldIdx);

    /* We only recognize opc with an arg for now, as well as a special case for ldaxysp */
    if ((E->OPC != OP65_JSR || strcmp (E->Arg, "ldaxysp") == 0) &&
        E->AM != AM65_BRA && E->AM != AM65_IMP) {
        if (E->Size != 1 && E->AM != AM65_IMP) {

            /* FIXME: The load flags only reflect the situation by the time it reaches the range end */
            if ((LRI->Flags & (LI_DIRECT | LI_CHECK_ARG | LI_CHECK_Y)) != 0) {
                if ((LRI->Flags & LI_RELOAD_Y) != 0) {
                    if ((LRI->Flags & LI_CHECK_Y) == 0) {
                        X = NewCodeEntry (OP65_LDY, AM65_IMM, MakeHexArg (LRI->Offs), 0, E->LI);
                    } else {
                        X = NewCodeEntry (OP65_LDY, LRI->LoadYEntry->AM, LRI->LoadYEntry->Arg, 0, E->LI);
                    }
                    CS_InsertEntry (S, X, Idx++);
                }
                X = NewCodeEntry (OP65_LDA, E->AM, E->Arg, 0, E->LI);
                CS_InsertEntry (S, X, Idx++);

                Success = 1;
            }
        }
    } else if (E->OPC == OP65_JSR) {

        /* For other function calls we load their arguments instead */
        GetFuncInfo (E->Arg, &Use, &Chg);
        if ((Use & ~REG_AXY) == 0) {
            if (Use == REG_X) {
                X = NewCodeEntry (OP65_TXA, AM65_IMP, 0, 0, E->LI);
                CS_InsertEntry (S, X, Idx++);
            } else if (Use == REG_Y) {
                X = NewCodeEntry (OP65_TYA, AM65_IMP, 0, 0, E->LI);
                CS_InsertEntry (S, X, Idx++);
            } else if (Use == REG_A) {
                /* nothing to do */
            } else {
                /* We don't recognize other usage patterns for now */
                return 0;
            }

            Success = 1;
        }
    }

    if (Success) {
        /* Adjust all indices at once */
        AdjustEntryIndices (Indices, OldIdx, Idx - OldIdx);

        /* Move labels if it was an insertion before Idx */
        CS_MoveLabels (S, O, CS_GetEntry (S, OldIdx));

        /* Done */
        return 1;
    }

    /* Unable to load */
    return 0;
}



static int LoadXAt (CodeSeg* S, int Idx, const LoadRegInfo* LRI, Collection* Indices, int After)
/* Reload into X the same arg according to LoadRegInfo before or after Idx
** depending on the After param.
*/
{
    CodeEntry* E;
    CodeEntry* O;       /* Old entry at Idx */
    CodeEntry* X;
    int Success = 0;
    int OldIdx;
    unsigned Use, Chg;

    /* Adjust the insertion point if necessary */
    if (After) {
        ++Idx;
    }
    OldIdx = Idx;

    E = LRI->LoadEntry;
    CHECK (E != 0);

    O = CS_GetEntry (S, OldIdx);

    /* We only recognize opc with an arg for now, as well as a special case for ldaxysp */
    if ((E->OPC != OP65_JSR || strcmp (E->Arg, "ldaxysp") == 0) &&
        E->AM != AM65_BRA && E->AM != AM65_IMP) {
        if (E->Size != 1 && E->AM != AM65_IMP) {

            /* FIXME: The load flags only reflect the situation by the time it reaches the range end */
            if ((LRI->Flags & (LI_DIRECT | LI_CHECK_ARG | LI_CHECK_Y)) != 0) {
                if ((LRI->Flags & LI_RELOAD_Y) != 0) {
                    if ((LRI->Flags & LI_CHECK_Y) == 0) {
                        X = NewCodeEntry (OP65_LDY, AM65_IMM, MakeHexArg (LRI->Offs), 0, E->LI);
                    } else {
                        X = NewCodeEntry (OP65_LDY, LRI->LoadYEntry->AM, LRI->LoadYEntry->Arg, 0, E->LI);
                    }
                    CS_InsertEntry (S, X, Idx++);

                    /* ldx does support AM65_ZPY and AM65_ABSY */
                    if (E->AM == AM65_ZPY || E->AM == AM65_ABSY) {
                        X = NewCodeEntry (OP65_LDX, E->AM, E->Arg, 0, E->LI);
                    } else {
                        X = NewCodeEntry (OP65_LDA, E->AM, E->Arg, 0, E->LI);
                        CS_InsertEntry (S, X, Idx++);
                        X = NewCodeEntry (OP65_TAX, AM65_IMP, 0, 0, E->LI);
                    }
                    CS_InsertEntry (S, X, Idx++);
                } else {
                    X = NewCodeEntry (OP65_LDX, E->AM, E->Arg, 0, E->LI);
                    CS_InsertEntry (S, X, Idx++);
                }

                Success = 1;
            }
        }
    } else if (E->OPC == OP65_JSR) {
        /* For function calls we load their arguments instead */
        GetFuncInfo (E->Arg, &Use, &Chg);
        if ((Use & ~REG_AXY) == 0) {
            if (Use == REG_A) {
                X = NewCodeEntry (OP65_TAY, AM65_IMP, 0, 0, E->LI);
                CS_InsertEntry (S, X, Idx++);
            } else if (Use == REG_Y) {
                X = NewCodeEntry (OP65_PHA, AM65_IMP, 0, 0, E->LI);
                CS_InsertEntry (S, X, Idx++);
                X = NewCodeEntry (OP65_TYA, AM65_IMP, 0, 0, E->LI);
                CS_InsertEntry (S, X, Idx++);
                X = NewCodeEntry (OP65_TAX, AM65_IMP, 0, 0, E->LI);
                CS_InsertEntry (S, X, Idx++);
                X = NewCodeEntry (OP65_PLA, AM65_IMP, 0, 0, E->LI);
                CS_InsertEntry (S, X, Idx++);
            } else if (Use == REG_X) {
                /* nothing to do */
            } else {
                /* We don't recognize other usage patterns for now */
                return 0;
            }

            Success = 1;
        }
    }

    if (Success) {
        /* Adjust all indices at once */
        AdjustEntryIndices (Indices, OldIdx, Idx - OldIdx);

        /* Move labels if it was an insertion before Idx */
        CS_MoveLabels (S, O, CS_GetEntry (S, OldIdx));

        /* Done */
        return 1;
    }

    /* Unable to load */
    return 0;
}



static int LoadYAt (CodeSeg* S, int Idx, const LoadRegInfo* LRI, Collection* Indices, int After)
/* Reload into Y the same arg according to LoadRegInfo before or after Idx
** depending on the After param.
*/
{
    CodeEntry* E;
    CodeEntry* O;       /* Old entry at Idx */
    CodeEntry* X;
    int Success = 0;
    int OldIdx;
    unsigned Use, Chg;

    /* Adjust the insertion point if necessary */
    if (After) {
        ++Idx;
    }
    OldIdx = Idx;

    E = LRI->LoadEntry;
    CHECK (E != 0);

    O = CS_GetEntry (S, OldIdx);

    /* We only recognize opc with an arg for now, as well as a special case for ldaxysp */
    if ((E->OPC != OP65_JSR || strcmp (E->Arg, "ldaxysp") == 0) &&
        E->AM != AM65_BRA && E->AM != AM65_IMP) {
        if (E->Size != 1 && E->AM != AM65_IMP) {

            /* FIXME: The load flags only reflect the situation by the time it reaches the range end */
            if ((LRI->Flags & (LI_DIRECT | LI_CHECK_ARG | LI_CHECK_Y)) != 0) {
                if ((LRI->Flags & LI_RELOAD_Y) != 0) {
                    if ((LRI->Flags & LI_CHECK_Y) == 0) {
                        X = NewCodeEntry (OP65_LDY, AM65_IMM, MakeHexArg (LRI->Offs), 0, E->LI);
                    } else {
                        X = NewCodeEntry (OP65_LDY, LRI->LoadYEntry->AM, LRI->LoadYEntry->Arg, 0, E->LI);
                    }
                    CS_InsertEntry (S, X, Idx++);
                    X = NewCodeEntry (OP65_LDA, E->AM, E->Arg, 0, E->LI);
                    CS_InsertEntry (S, X, Idx++);
                    X = NewCodeEntry (OP65_TAY, AM65_IMP, 0, 0, E->LI);
                    CS_InsertEntry (S, X, Idx++);
                } else {
                    X = NewCodeEntry (OP65_LDY, E->AM, E->Arg, 0, E->LI);
                    CS_InsertEntry (S, X, Idx++);
                }

                Success = 1;
            }
        }
    } else if (E->OPC == OP65_JSR) {
        /* For function calls we load their arguments instead */
        GetFuncInfo (E->Arg, &Use, &Chg);
        if ((Use & ~REG_AXY) == 0) {
            if (Use == REG_A) {
                X = NewCodeEntry (OP65_TAY, AM65_IMP, 0, 0, E->LI);
                CS_InsertEntry (S, X, Idx++);
            } else if (Use == REG_X) {
                X = NewCodeEntry (OP65_PHA, AM65_IMP, 0, 0, E->LI);
                CS_InsertEntry (S, X, Idx++);
                X = NewCodeEntry (OP65_TXA, AM65_IMP, 0, 0, E->LI);
                CS_InsertEntry (S, X, Idx++);
                X = NewCodeEntry (OP65_TAY, AM65_IMP, 0, 0, E->LI);
                CS_InsertEntry (S, X, Idx++);
                X = NewCodeEntry (OP65_PLA, AM65_IMP, 0, 0, E->LI);
                CS_InsertEntry (S, X, Idx++);
            } else if (Use == REG_Y) {
                /* nothing to do */
            } else {
                /* We don't recognize other usage patterns for now */
                return 0;
            }

            Success = 1;
        }
    }

    if (Success) {
        /* Adjust all indices at once */
        AdjustEntryIndices (Indices, OldIdx, Idx - OldIdx);

        /* Move labels if it was an insertion before Idx */
        CS_MoveLabels (S, O, CS_GetEntry (S, OldIdx));

        /* Done */
        return 1;
    }

    /* Unable to load */
    return 0;
}



int LoadABefore (CodeSeg* S, int Idx, const LoadRegInfo* LRI, Collection* Indices)
/* Reload into A the same arg according to LoadRegInfo at Idx */
{
    return LoadAAt (S, Idx, LRI, Indices, 0);
}



int LoadXBefore (CodeSeg* S, int Idx, const LoadRegInfo* LRI, Collection* Indices)
/* Reload into X the same arg according to LoadRegInfo at Idx */
{
    return LoadXAt (S, Idx, LRI, Indices, 0);
}



int LoadYBefore (CodeSeg* S, int Idx, const LoadRegInfo* LRI, Collection* Indices)
/* Reload into Y the same arg according to LoadRegInfo at Idx */
{
    return LoadYAt (S, Idx, LRI, Indices, 0);
}



int LoadAAfter (CodeSeg* S, int Idx, const LoadRegInfo* LRI, Collection* Indices)
/* Reload into A the same arg according to LoadRegInfo after Idx */
{
    return LoadAAt (S, Idx, LRI, Indices, 1);
}



int LoadXAfter (CodeSeg* S, int Idx, const LoadRegInfo* LRI, Collection* Indices)
/* Reload into X the same arg according to LoadRegInfo after Idx */
{
    return LoadXAt (S, Idx, LRI, Indices, 1);
}



int LoadYAfter (CodeSeg* S, int Idx, const LoadRegInfo* LRI, Collection* Indices)
/* Reload into Y the same arg according to LoadRegInfo after Idx */
{
    return LoadYAt (S, Idx, LRI, Indices, 1);
}



unsigned GetRegAccessedInOpenRange (CodeSeg* S, int First, int Last)
/* Get what ZPs, registers or processor states are used or changed in the range
** (First, Last).
** The code block must be basic without any jump backwards.
*/
{
    CodeEntry* X;
    unsigned ZPAccessed = 0;

    CHECK (Last <= (int)CollCount (&S->Entries));

    while (++First < Last) {
        X = CS_GetEntry (S, First);
        ZPAccessed |= X->Use | X->Chg;
    }

    return ZPAccessed;
}



unsigned GetRegUsageInOpenRange (CodeSeg* S, int First, int Last, unsigned* Use, unsigned* Chg)
/* Get what ZPs, registers or processor states are used or changed in the range
** (First, Last) in output parameters Use and Chg.
** Return what ZP regs are used before changed in this range.
** The code block must be basic without any jump backwards.
*/
{
    CodeEntry* X;
    unsigned U = 0;
    unsigned C = 0;

    CHECK (Last <= (int)CollCount (&S->Entries));

    /* Clear the output flags first */
    if (Use != 0) {
        *Use = 0;
    }
    if (Chg != 0) {
        *Chg = 0;
    }

    while (++First < Last) {
        X = CS_GetEntry (S, First);
        if (Use != 0) {
            *Use |= X->Use;
        }
        if (Chg != 0) {
            *Chg |= X->Chg;
        }
        /* Used before changed */
        U |= ~C & X->Use;
        C |= X->Chg;
    }

    return U;
}



int IsArgSameInOpenRange (CodeSeg* S, int First, int Last, CodeEntry* E)
/* Check if the loading the opc arg gives the same result everywhere between (First, Last).
** The code block in the range must be basic without any jump backwards.
** Note: this always checks Y if any of the LI_CHECK_Y / LI_RELOAD_Y flags is set.
*/
{
    LoadRegInfo LRI;
    CodeEntry*  X;
    unsigned CheckedFlags = LI_SRC_CHG;

    CHECK (Last <= (int)CollCount (&S->Entries));

    /* TODO: We'll currently give up finding the src of Y */
    ClearLoadRegInfo (&LRI);
    PrepairLoadRegInfoForArgCheck (S, &LRI, E);

    /* TODO: We don't currently check for all cases */
    if ((LRI.Flags & (LI_DIRECT | LI_CHECK_ARG | LI_CHECK_Y | LI_RELOAD_Y)) == 0) {
        /* Just bail out as if the src would change right away */
        return 0;
    }

    /* If there's no need to check */
    if ((LRI.Flags & (LI_CHECK_ARG | LI_CHECK_Y | LI_RELOAD_Y)) == 0) {
        return 1;
    }

    /* This always checks Y */
    if ((LRI.Flags & (LI_CHECK_Y | LI_RELOAD_Y)) != 0) {
        LRI.Flags |= LI_CHECK_Y;
        LRI.Flags &= ~LI_RELOAD_Y;
        CheckedFlags |= LI_Y_CHG;
    }

    while (++First < Last) {
        X = CS_GetEntry (S, First);
        if ((Affected (&LRI, X) & CheckedFlags) != 0) {
            return 0;
        }
    }

    /* No change found */
    return 1;
}



int FindArgFirstChangeInOpenRange (CodeSeg* S, int First, int Last, CodeEntry* E)
/* Find the first possible spot where the loaded arg of E might be changed in
** the range (First, Last). The code block in the range must be basic without
** any jump backwards.
** Return the index of the found entry, or Last if not found.
** Note: changes of Y are always ignored even if the LI_RELOAD_Y flag is not set.
*/
{
    LoadRegInfo LRI;
    CodeEntry* X;
    unsigned CheckedFlags = LI_SRC_CHG;

    CHECK (Last <= (int)CollCount (&S->Entries));

    /* TODO: We'll currently give up finding the src of Y */
    ClearLoadRegInfo (&LRI);
    PrepairLoadRegInfoForArgCheck (S, &LRI, E);

    /* TODO: We don't currently check for all cases */
    if ((LRI.Flags & (LI_DIRECT | LI_CHECK_ARG | LI_CHECK_Y)) == 0) {
        /* Just bail out as if the src would change right away */
        return First + 1;
    }

    /* If there's no need to check */
    if ((LRI.Flags & (LI_CHECK_ARG | LI_CHECK_Y)) == 0) {
        return Last;
    }

    while (++First < Last) {
        X = CS_GetEntry (S, First);
        if ((Affected (&LRI, X) & CheckedFlags) != 0) {
            return First;
        }
    }

    /* Not found */
    return Last;
}



int FindArgLastUsageInOpenRange (CodeSeg* S, int First, int Last, CodeEntry* E, int ReloadY)
/* Find the last index where the arg of E might be used or changed in the range (First, Last).
** ReloadY indicates whether Y is supposed to be reloaded.
** The code block in the range must be basic without any jump backwards.
** Return the index of the found entry, or First if not found.
*/
{
    LoadRegInfo LRI;
    CodeEntry* X;
    unsigned CheckedFlags = LI_SRC_USE | LI_SRC_CHG;
    int Found = First;

    CHECK (Last <= (int)CollCount (&S->Entries));

    /* TODO: We'll currently give up finding the src of Y */
    ClearLoadRegInfo (&LRI);
    PrepairLoadRegInfoForArgCheck (S, &LRI, E);

    /* Whether Y is to be reloaded */
    if (ReloadY) {
        /* Always reload Y */
        if ((LRI.Flags & LI_CHECK_Y) != 0) {
            LRI.Flags |= LI_RELOAD_Y;
        }
    } else if ((LRI.Flags & LI_RELOAD_Y) != 0) {
        /* Always check Y */
        LRI.Flags |= LI_CHECK_Y;
        LRI.Flags &= ~LI_RELOAD_Y;
    }

    /* TODO: We don't currently check for all cases */
    if ((LRI.Flags & (LI_DIRECT | LI_CHECK_ARG | LI_CHECK_Y | LI_RELOAD_Y)) == 0) {
        /* Just bail out as if the src would change everywhere */
        return First < Last ? Last - 1 : First;
    }

    if ((LRI.Flags & LI_CHECK_Y) != 0) {
        CheckedFlags |= LI_Y_SRC_USE | LI_Y_SRC_CHG;
    }

    if ((LRI.Flags & LI_RELOAD_Y) != 0) {
        CheckedFlags |= LI_Y_USE;
    } else if ((LRI.Flags & LI_CHECK_Y) != 0) {
        CheckedFlags |= LI_Y_CHG;
    }

    while (++First < Last) {
        X = CS_GetEntry (S, First);
        if ((Affected (&LRI, X) & CheckedFlags) != 0) {
            Found = First;
        }
    }

    /* Result */
    return Found;
}



int FindRegFirstChangeInOpenRange (CodeSeg* S, int First, int Last, unsigned what)
/* Find the first possible spot where the queried ZPs, registers and/or processor
** states might be changed in the range (First, Last). The code block in the
** range must be basic without any jump backwards.
** Return the index of the found entry, or Last if not found.
*/
{
    CodeEntry* X;

    CHECK (Last <= (int)CollCount (&S->Entries));

    while (++First < Last) {
        X = CS_GetEntry (S, First);
        if ((X->Chg & what) != 0) {
            return First;
        }
    }

    /* Not found */
    return Last;
}



int FindRegFirstUseInOpenRange (CodeSeg* S, int First, int Last, unsigned what)
/* Find the first possible spot where the queried ZPs, registers and/or processor
** states might be used in the range (First, Last). The code block in the range
** must be basic without any jump backwards.
** Return the index of the found entry, or Last if not found.
*/
{
    CodeEntry* X;

    CHECK (Last <= (int)CollCount (&S->Entries));

    while (++First < Last) {
        X = CS_GetEntry (S, First);
        if ((X->Use & what) != 0) {
            return First;
        }
    }

    /* Not found */
    return Last;
}



int FindRegLastChangeInOpenRange (CodeSeg* S, int First, int Last, unsigned what)
/* Find the last possible spot where the queried ZPs, registers and/or processor
** states might be changed in the range (First, Last). The code block in the
** range must be basic without any jump backwards.
** Return the index of the found entry, or First if not found.
*/
{
    CodeEntry* X;
    int Found = First;

    CHECK (Last <= (int)CollCount (&S->Entries));

    while (++First < Last) {
        X = CS_GetEntry (S, First);
        if ((X->Chg & what) != 0) {
            Found = First;
        }
    }

    return Found;
}



int FindRegLastUseInOpenRange (CodeSeg* S, int First, int Last, unsigned what)
/* Find the last possible spot where the queried ZPs, registers and/or processor
** states might be used in the range (First, Last). The code block in the range
** must be basic without any jump backwards.
** Return the index of the found entry, or First if not found.
*/
{
    CodeEntry* X;
    int Found = First;

    CHECK (Last <= (int)CollCount (&S->Entries));

    while (++First < Last) {
        X = CS_GetEntry (S, First);
        if ((X->Use & what) != 0) {
            Found = First;
        }
    }

    return Found;
}
