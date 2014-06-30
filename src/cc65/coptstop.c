/*****************************************************************************/
/*                                                                           */
/*                                 coptstop.c                                */
/*                                                                           */
/*           Optimize operations that take operands via the stack            */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2001-2013, Ullrich von Bassewitz                                      */
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



#include <stdlib.h>

/* common */
#include "chartype.h"

/* cc65 */
#include "codeent.h"
#include "codeinfo.h"
#include "coptstop.h"
#include "error.h"



/*****************************************************************************/
/*                            Load tracking data                             */
/*****************************************************************************/



/* LoadRegInfo flags set by DirectOp */
typedef enum {
  LI_NONE               = 0x00,
  LI_DIRECT             = 0x01,         /* Direct op may be used */
  LI_RELOAD_Y           = 0x02,         /* Reload index register Y */
  LI_REMOVE             = 0x04,         /* Load may be removed */
  LI_DONT_REMOVE        = 0x08,         /* Load may not be removed */
  LI_DUP_LOAD           = 0x10,         /* Duplicate load */
} LI_FLAGS;

/* Structure that tells us how to load the lhs values */
typedef struct LoadRegInfo LoadRegInfo;
struct LoadRegInfo {
    LI_FLAGS            Flags;          /* Tells us how to load */
    int                 LoadIndex;      /* Index of load insn, -1 if invalid */
    CodeEntry*          LoadEntry;      /* The actual entry, 0 if invalid */
    int                 XferIndex;      /* Index of transfer insn  */
    CodeEntry*          XferEntry;      /* The actual transfer entry */
    int                 Offs;           /* Stack offset if data is on stack */
};

/* Now combined for both registers */
typedef struct LoadInfo LoadInfo;
struct LoadInfo {
    LoadRegInfo         A;              /* Info for A register */
    LoadRegInfo         X;              /* Info for X register */
    LoadRegInfo         Y;              /* Info for Y register */
};



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Flags for the functions */
typedef enum {
    OP_NONE             = 0x00,         /* Nothing special */
    OP_A_KNOWN          = 0x01,         /* Value of A must be known */
    OP_X_ZERO           = 0x02,         /* X must be zero */
    OP_LHS_LOAD         = 0x04,         /* Must have load insns for LHS */
    OP_LHS_LOAD_DIRECT  = 0x0C,         /* Must have direct load insn for LHS */
    OP_RHS_LOAD         = 0x10,         /* Must have load insns for RHS */
    OP_RHS_LOAD_DIRECT  = 0x30,         /* Must have direct load insn for RHS */
} OP_FLAGS;

/* Structure forward decl */
typedef struct StackOpData StackOpData;

/* Structure that describes an optimizer subfunction for a specific op */
typedef unsigned (*OptFunc) (StackOpData* D);
typedef struct OptFuncDesc OptFuncDesc;
struct OptFuncDesc {
    const char*         Name;           /* Name of the replaced runtime function */
    OptFunc             Func;           /* Function pointer */
    unsigned            UnusedRegs;     /* Regs that must not be used later */
    OP_FLAGS            Flags;          /* Flags */
};

/* Structure that holds the needed data */
struct StackOpData {
    CodeSeg*            Code;           /* Pointer to code segment */
    unsigned            Flags;          /* Flags to remember things */

    /* Pointer to optimizer subfunction description */
    const OptFuncDesc*  OptFunc;

    /* ZP register usage inside the sequence */
    unsigned            UsedRegs;

    /* Register load information for lhs and rhs */
    LoadInfo            Lhs;
    LoadInfo            Rhs;

    /* Several indices of insns in the code segment */
    int                 PushIndex;      /* Index of call to pushax in codeseg */
    int                 OpIndex;        /* Index of actual operation */

    /* Pointers to insns in the code segment */
    CodeEntry*          PrevEntry;      /* Entry before the call to pushax */
    CodeEntry*          PushEntry;      /* Pointer to entry with call to pushax */
    CodeEntry*          OpEntry;        /* Pointer to entry with op */
    CodeEntry*          NextEntry;      /* Entry after the op */

    const char*         ZPLo;           /* Lo byte of zero page loc to use */
    const char*         ZPHi;           /* Hi byte of zero page loc to use */
    unsigned            IP;             /* Insertion point used by some routines */
};



/*****************************************************************************/
/*                            Load tracking code                             */
/*****************************************************************************/



static void ClearLoadRegInfo (LoadRegInfo* RI)
/* Clear a LoadRegInfo struct */
{
    RI->Flags     = LI_NONE;
    RI->LoadIndex = -1;
    RI->XferIndex = -1;
    RI->Offs      = 0;
}



static void FinalizeLoadRegInfo (LoadRegInfo* RI, CodeSeg* S)
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
}



static void ClearLoadInfo (LoadInfo* LI)
/* Clear a LoadInfo struct */
{
    ClearLoadRegInfo (&LI->A);
    ClearLoadRegInfo (&LI->X);
    ClearLoadRegInfo (&LI->Y);
}



static void AdjustLoadRegInfo (LoadRegInfo* RI, int Index, int Change)
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



static void FinalizeLoadInfo (LoadInfo* LI, CodeSeg* S)
/* Prepare a LoadInfo struct for use */
{
    /* Get the entries */
    FinalizeLoadRegInfo (&LI->A, S);
    FinalizeLoadRegInfo (&LI->X, S);
    FinalizeLoadRegInfo (&LI->Y, S);
}



static void AdjustLoadInfo (LoadInfo* LI, int Index, int Change)
/* Adjust a load info struct after deleting entry with a given index */
{
    AdjustLoadRegInfo (&LI->A, Index, Change);
    AdjustLoadRegInfo (&LI->X, Index, Change);
    AdjustLoadRegInfo (&LI->Y, Index, Change);
}



static void HonourUseAndChg (LoadRegInfo* RI, unsigned Reg, const CodeEntry* E)
/* Honour use and change flags for an instruction */
{
    if (E->Chg & Reg) {
        ClearLoadRegInfo (RI);
    } else if ((E->Use & Reg) && RI->LoadIndex >= 0) {
        RI->Flags |= LI_DONT_REMOVE;
    }
}



static void TrackLoads (LoadInfo* LI, CodeEntry* E, int I)
/* Track loads for a code entry */
{
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

        /* If we had a load or xfer op before, this is a duplicate load which
        ** can cause problems if it encountered between the pushax and the op,
        ** so remember it.
        */
        if (RI->LoadIndex >= 0 || RI->XferIndex >= 0) {
            RI->Flags |= LI_DUP_LOAD;
        }

        /* Remember the load */
        RI->LoadIndex = I;
        RI->XferIndex = -1;

        /* Set load flags */
        RI->Flags    &= ~(LI_DIRECT | LI_RELOAD_Y);
        if (E->AM == AM65_IMM || E->AM == AM65_ZP || E->AM == AM65_ABS) {
            /* These insns are all ok and replaceable */
            RI->Flags |= LI_DIRECT;
        } else if (E->AM == AM65_ZP_INDY &&
                   RegValIsKnown (E->RI->In.RegY) &&
                   strcmp (E->Arg, "sp") == 0) {
            /* A load from the stack with known offset is also ok, but in this
            ** case we must reload the index register later. Please note that
            ** a load indirect via other zero page locations is not ok, since
            ** these locations may change between the push and the actual
            ** operation.
            */
            RI->Offs  = (unsigned char) E->RI->In.RegY;
            RI->Flags |= (LI_DIRECT | LI_RELOAD_Y);
        }


    } else if (E->Info & OF_XFR) {

        /* Determine source and target of the transfer and handle the TSX insn */
        LoadRegInfo* Src;
        LoadRegInfo* Tgt;
        switch (E->OPC) {
            case OP65_TAX:      Src = &LI->A; Tgt = &LI->X; break;
            case OP65_TAY:      Src = &LI->A; Tgt = &LI->Y; break;
            case OP65_TXA:      Src = &LI->X; Tgt = &LI->A; break;
            case OP65_TYA:      Src = &LI->Y; Tgt = &LI->A; break;
            case OP65_TSX:      ClearLoadRegInfo (&LI->X);  return;
            case OP65_TXS:                                  return;
            default:            Internal ("Unknown XFR insn in TrackLoads");
        }

        /* If we had a load or xfer op before, this is a duplicate load which
        ** can cause problems if it encountered between the pushax and the op,
        ** so remember it.
        */
        if (Tgt->LoadIndex >= 0 || Tgt->XferIndex >= 0) {
            Tgt->Flags |= LI_DUP_LOAD;
        }

        /* Transfer the data */
        Tgt->LoadIndex  = Src->LoadIndex;
        Tgt->XferIndex  = I;
        Tgt->Offs       = Src->Offs;
        Tgt->Flags     &= ~(LI_DIRECT | LI_RELOAD_Y);
        Tgt->Flags     |= Src->Flags & (LI_DIRECT | LI_RELOAD_Y);

    } else if (CE_IsCallTo (E, "ldaxysp") && RegValIsKnown (E->RI->In.RegY)) {

        /* If we had a load or xfer op before, this is a duplicate load which
        ** can cause problems if it encountered between the pushax and the op,
        ** so remember it for both registers involved.
        */
        if (LI->A.LoadIndex >= 0 || LI->A.XferIndex >= 0) {
            LI->A.Flags |= LI_DUP_LOAD;
        }
        if (LI->X.LoadIndex >= 0 || LI->X.XferIndex >= 0) {
            LI->X.Flags |= LI_DUP_LOAD;
        }

        /* Both registers set, Y changed */
        LI->A.LoadIndex = I;
        LI->A.XferIndex = -1;
        LI->A.Flags    |= (LI_DIRECT | LI_RELOAD_Y);
        LI->A.Offs      = (unsigned char) E->RI->In.RegY - 1;

        LI->X.LoadIndex = I;
        LI->X.XferIndex = -1;
        LI->X.Flags    |= (LI_DIRECT | LI_RELOAD_Y);
        LI->X.Offs      = (unsigned char) E->RI->In.RegY;

        ClearLoadRegInfo (&LI->Y);
    } else {
        HonourUseAndChg (&LI->A, REG_A, E);
        HonourUseAndChg (&LI->X, REG_X, E);
        HonourUseAndChg (&LI->Y, REG_Y, E);
    }
}



/*****************************************************************************/
/*                                  Helpers                                  */
/*****************************************************************************/



static void InsertEntry (StackOpData* D, CodeEntry* E, int Index)
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



static void DelEntry (StackOpData* D, int Index)
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



static void AdjustStackOffset (StackOpData* D, unsigned Offs)
/* Adjust the offset for all stack accesses in the range PushIndex to OpIndex.
** OpIndex is adjusted according to the insertions.
*/
{
    /* Walk over all entries */
    int I = D->PushIndex + 1;
    while (I < D->OpIndex) {

        CodeEntry* E = CS_GetEntry (D->Code, I);

        int NeedCorrection = 0;
        if ((E->Use & REG_SP) != 0) {

            /* Check for some things that should not happen */
            CHECK (E->AM == AM65_ZP_INDY || E->RI->In.RegY >= (short) Offs);
            CHECK (strcmp (E->Arg, "sp") == 0);

            /* We need to correct this one */
            NeedCorrection = 1;

        } else if (CE_IsCallTo (E, "ldaxysp")) {

            /* We need to correct this one */
            NeedCorrection = 1;

        }

        if (NeedCorrection) {

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
                const char* Arg = MakeHexArg (E->RI->In.RegY);
                CodeEntry* X = NewCodeEntry (OP65_LDY, AM65_IMM, Arg, 0, E->LI);
                InsertEntry (D, X, I+1);

                /* Skip this instruction in the next round */
                ++I;
            }
        }

        /* Next entry */
        ++I;
    }

    /* If we have rhs load insns that load from stack, we'll have to adjust
    ** the offsets for these also.
    */
    if (D->Rhs.A.Flags & LI_RELOAD_Y) {
        D->Rhs.A.Offs -= Offs;
    }
    if (D->Rhs.X.Flags & LI_RELOAD_Y) {
        D->Rhs.X.Offs -= Offs;
    }
}



static void AddStoreA (StackOpData* D)
/* Add a store to zero page after the push insn */
{
    CodeEntry* X = NewCodeEntry (OP65_STA, AM65_ZP, D->ZPLo, 0, D->PushEntry->LI);
    InsertEntry (D, X, D->PushIndex+1);
}



static void AddStoreX (StackOpData* D)
/* Add a store to zero page after the push insn */
{
    CodeEntry* X = NewCodeEntry (OP65_STX, AM65_ZP, D->ZPHi, 0, D->PushEntry->LI);
    InsertEntry (D, X, D->PushIndex+1);
}



static void ReplacePushByStore (StackOpData* D)
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
        AddStoreX (D);
    }
    if ((D->Lhs.A.Flags & LI_DIRECT) == 0) {
        AddStoreA (D);
    }
}



static void AddOpLow (StackOpData* D, opc_t OPC, LoadInfo* LI)
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

            /* ldy #offs */
            const char* Arg = MakeHexArg (LI->A.Offs);
            X = NewCodeEntry (OP65_LDY, AM65_IMM, Arg, 0, D->OpEntry->LI);
            InsertEntry (D, X, D->IP++);

            /* opc (sp),y */
            X = NewCodeEntry (OPC, AM65_ZP_INDY, "sp", 0, D->OpEntry->LI);
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



static void AddOpHigh (StackOpData* D, opc_t OPC, LoadInfo* LI, int KeepResult)
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

            /* ldy #const */
            const char* Arg = MakeHexArg (LI->X.Offs);
            X = NewCodeEntry (OP65_LDY, AM65_IMM, Arg, 0, D->OpEntry->LI);
            InsertEntry (D, X, D->IP++);

            /* opc (sp),y */
            X = NewCodeEntry (OPC, AM65_ZP_INDY, "sp", 0, D->OpEntry->LI);
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



static void RemoveRegLoads (StackOpData* D, LoadInfo* LI)
/* Remove register load insns */
{
    /* Both registers may be loaded with one insn, but DelEntry will in this
    ** case clear the other one.
    */
    if ((LI->A.Flags & (LI_REMOVE | LI_DONT_REMOVE)) == LI_REMOVE) {
        if (LI->A.LoadIndex >= 0) {
            DelEntry (D, LI->A.LoadIndex);
        }
        if (LI->A.XferIndex >= 0) {
            DelEntry (D, LI->A.XferIndex);
        }
    }
    if ((LI->X.Flags & (LI_REMOVE | LI_DONT_REMOVE)) == LI_REMOVE) {
        if (LI->X.LoadIndex >= 0) {
            DelEntry (D, LI->X.LoadIndex);
        }
        if (LI->X.XferIndex >= 0) {
            DelEntry (D, LI->X.XferIndex);
        }
    }
}



static void RemoveRemainders (StackOpData* D)
/* Remove the code that is unnecessary after translation of the sequence */
{
    /* Remove the register loads for lhs and rhs */
    RemoveRegLoads (D, &D->Lhs);
    RemoveRegLoads (D, &D->Rhs);

    /* Remove the push and the operator routine */
    DelEntry (D, D->OpIndex);
    DelEntry (D, D->PushIndex);
}



static int IsRegVar (StackOpData* D)
/* If the value pushed is that of a zeropage variable, replace ZPLo and ZPHi
** in the given StackOpData struct by the variable and return true. Otherwise
** leave D untouched and return false.
*/
{
    CodeEntry*  LoadA = D->Lhs.A.LoadEntry;
    CodeEntry*  LoadX = D->Lhs.X.LoadEntry;
    unsigned    Len;

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



/*****************************************************************************/
/*                       Actual optimization functions                       */
/*****************************************************************************/



static unsigned Opt_toseqax_tosneax (StackOpData* D, const char* BoolTransformer)
/* Optimize the toseqax and tosneax sequences. */
{
    CodeEntry*  X;
    CodeLabel* L;

    /* Create a call to the boolean transformer function and a label for this
    ** insn. This is needed for all variants. Other insns are inserted *before*
    ** the call.
    */
    X = NewCodeEntry (OP65_JSR, AM65_ABS, BoolTransformer, 0, D->OpEntry->LI);
    InsertEntry (D, X, D->OpIndex + 1);
    L = CS_GenLabel (D->Code, X);

    /* If the lhs is direct (but not stack relative), encode compares with lhs
    ** effectively reverting the order (which doesn't matter for ==).
    */
    if ((D->Lhs.A.Flags & (LI_DIRECT | LI_RELOAD_Y)) == LI_DIRECT &&
        (D->Lhs.X.Flags & (LI_DIRECT | LI_RELOAD_Y)) == LI_DIRECT) {

        CodeEntry* LoadX = D->Lhs.X.LoadEntry;
        CodeEntry* LoadA = D->Lhs.A.LoadEntry;

        D->IP = D->OpIndex+1;

        /* cpx */
        X = NewCodeEntry (OP65_CPX, LoadX->AM, LoadX->Arg, 0, D->OpEntry->LI);
        InsertEntry (D, X, D->IP++);

        /* bne L */
        X = NewCodeEntry (OP65_BNE, AM65_BRA, L->Name, L, D->OpEntry->LI);
        InsertEntry (D, X, D->IP++);

        /* cmp */
        X = NewCodeEntry (OP65_CMP, LoadA->AM, LoadA->Arg, 0, D->OpEntry->LI);
        InsertEntry (D, X, D->IP++);

        /* Lhs load entries can be removed */
        D->Lhs.X.Flags |= LI_REMOVE;
        D->Lhs.A.Flags |= LI_REMOVE;

    } else if ((D->Rhs.A.Flags & (LI_DIRECT | LI_RELOAD_Y)) == LI_DIRECT &&
               (D->Rhs.X.Flags & (LI_DIRECT | LI_RELOAD_Y)) == LI_DIRECT) {

        CodeEntry* LoadX = D->Rhs.X.LoadEntry;
        CodeEntry* LoadA = D->Rhs.A.LoadEntry;

        D->IP = D->OpIndex+1;

        /* cpx */
        X = NewCodeEntry (OP65_CPX, LoadX->AM, LoadX->Arg, 0, D->OpEntry->LI);
        InsertEntry (D, X, D->IP++);

        /* bne L */
        X = NewCodeEntry (OP65_BNE, AM65_BRA, L->Name, L, D->OpEntry->LI);
        InsertEntry (D, X, D->IP++);

        /* cmp */
        X = NewCodeEntry (OP65_CMP, LoadA->AM, LoadA->Arg, 0, D->OpEntry->LI);
        InsertEntry (D, X, D->IP++);

        /* Rhs load entries can be removed */
        D->Rhs.X.Flags |= LI_REMOVE;
        D->Rhs.A.Flags |= LI_REMOVE;

    } else if ((D->Rhs.A.Flags & LI_DIRECT) != 0 &&
               (D->Rhs.X.Flags & LI_DIRECT) != 0) {

        D->IP = D->OpIndex+1;

        /* Add operand for low byte */
        AddOpLow (D, OP65_CMP, &D->Rhs);

        /* bne L */
        X = NewCodeEntry (OP65_BNE, AM65_BRA, L->Name, L, D->OpEntry->LI);
        InsertEntry (D, X, D->IP++);

        /* Add operand for high byte */
        AddOpHigh (D, OP65_CMP, &D->Rhs, 0);

    } else {

        /* Save lhs into zeropage, then compare */
        AddStoreX (D);
        AddStoreA (D);

        D->IP = D->OpIndex+1;

        /* cpx */
        X = NewCodeEntry (OP65_CPX, AM65_ZP, D->ZPHi, 0, D->OpEntry->LI);
        InsertEntry (D, X, D->IP++);

        /* bne L */
        X = NewCodeEntry (OP65_BNE, AM65_BRA, L->Name, L, D->OpEntry->LI);
        InsertEntry (D, X, D->IP++);

        /* cmp */
        X = NewCodeEntry (OP65_CMP, AM65_ZP, D->ZPLo, 0, D->OpEntry->LI);
        InsertEntry (D, X, D->IP++);

    }

    /* Remove the push and the call to the tosgeax function */
    RemoveRemainders (D);

    /* We changed the sequence */
    return 1;
}



static unsigned Opt_tosshift (StackOpData* D, const char* Name)
/* Optimize shift sequences. */
{
    CodeEntry*  X;

    /* Store the value into the zeropage instead of pushing it */
    ReplacePushByStore (D);

    /* If the lhs is direct (but not stack relative), we can just reload the
    ** data later.
    */
    if ((D->Lhs.A.Flags & (LI_DIRECT | LI_RELOAD_Y)) == LI_DIRECT &&
        (D->Lhs.X.Flags & (LI_DIRECT | LI_RELOAD_Y)) == LI_DIRECT) {

        CodeEntry* LoadX = D->Lhs.X.LoadEntry;
        CodeEntry* LoadA = D->Lhs.A.LoadEntry;

        /* Inline the shift */
        D->IP = D->OpIndex+1;

        /* tay */
        X = NewCodeEntry (OP65_TAY, AM65_IMP, 0, 0, D->OpEntry->LI);
        InsertEntry (D, X, D->IP++);

        /* lda */
        X = NewCodeEntry (OP65_LDA, LoadA->AM, LoadA->Arg, 0, D->OpEntry->LI);
        InsertEntry (D, X, D->IP++);

        /* ldx */
        X = NewCodeEntry (OP65_LDX, LoadX->AM, LoadX->Arg, 0, D->OpEntry->LI);
        InsertEntry (D, X, D->IP++);

        /* Lhs load entries can be removed */
        D->Lhs.X.Flags |= LI_REMOVE;
        D->Lhs.A.Flags |= LI_REMOVE;

    } else {

        /* Save lhs into zeropage and reload later */
        AddStoreX (D);
        AddStoreA (D);

        /* Be sure to setup IP after adding the stores, otherwise it will get
        ** messed up.
        */
        D->IP = D->OpIndex+1;

        /* tay */
        X = NewCodeEntry (OP65_TAY, AM65_IMP, 0, 0, D->OpEntry->LI);
        InsertEntry (D, X, D->IP++);

        /* lda zp */
        X = NewCodeEntry (OP65_LDA, AM65_ZP, D->ZPLo, 0, D->OpEntry->LI);
        InsertEntry (D, X, D->IP++);

        /* ldx zp+1 */
        X = NewCodeEntry (OP65_LDX, AM65_ZP, D->ZPHi, 0, D->OpEntry->LI);
        InsertEntry (D, X, D->IP++);

    }

    /* jsr shlaxy/aslaxy/whatever */
    X = NewCodeEntry (OP65_JSR, AM65_ABS, Name, 0, D->OpEntry->LI);
    InsertEntry (D, X, D->IP++);

    /* Remove the push and the call to the shift function */
    RemoveRemainders (D);

    /* We changed the sequence */
    return 1;
}



static unsigned Opt___bzero (StackOpData* D)
/* Optimize the __bzero sequence */
{
    CodeEntry*  X;
    const char* Arg;
    CodeLabel*  L;

    /* Check if we're using a register variable */
    if (!IsRegVar (D)) {
        /* Store the value into the zeropage instead of pushing it */
        AddStoreX (D);
        AddStoreA (D);
    }

    /* If the return value of __bzero is used, we have to add code to reload
    ** a/x from the pointer variable.
    */
    if (RegAXUsed (D->Code, D->OpIndex+1)) {
        X = NewCodeEntry (OP65_LDA, AM65_ZP, D->ZPLo, 0, D->OpEntry->LI);
        InsertEntry (D, X, D->OpIndex+1);
        X = NewCodeEntry (OP65_LDX, AM65_ZP, D->ZPHi, 0, D->OpEntry->LI);
        InsertEntry (D, X, D->OpIndex+2);
    }

    /* X is always zero, A contains the size of the data area to zero.
    ** Note: A may be zero, in which case the operation is null op.
    */
    if (D->OpEntry->RI->In.RegA != 0) {

        /* lda #$00 */
        X = NewCodeEntry (OP65_LDA, AM65_IMM, "$00", 0, D->OpEntry->LI);
        InsertEntry (D, X, D->OpIndex+1);

        /* The value of A is known */
        if (D->OpEntry->RI->In.RegA <= 0x81) {

            /* Loop using the sign bit */

            /* ldy #count-1 */
            Arg = MakeHexArg (D->OpEntry->RI->In.RegA - 1);
            X = NewCodeEntry (OP65_LDY, AM65_IMM, Arg, 0, D->OpEntry->LI);
            InsertEntry (D, X, D->OpIndex+2);

            /* L: sta (zp),y */
            X = NewCodeEntry (OP65_STA, AM65_ZP_INDY, D->ZPLo, 0, D->OpEntry->LI);
            InsertEntry (D, X, D->OpIndex+3);
            L = CS_GenLabel (D->Code, X);

            /* dey */
            X = NewCodeEntry (OP65_DEY, AM65_IMP, 0, 0, D->OpEntry->LI);
            InsertEntry (D, X, D->OpIndex+4);

            /* bpl L */
            X = NewCodeEntry (OP65_BPL, AM65_BRA, L->Name, L, D->OpEntry->LI);
            InsertEntry (D, X, D->OpIndex+5);

        } else {

            /* Loop using an explicit compare */

            /* ldy #$00 */
            X = NewCodeEntry (OP65_LDY, AM65_IMM, "$00", 0, D->OpEntry->LI);
            InsertEntry (D, X, D->OpIndex+2);

            /* L: sta (zp),y */
            X = NewCodeEntry (OP65_STA, AM65_ZP_INDY, D->ZPLo, 0, D->OpEntry->LI);
            InsertEntry (D, X, D->OpIndex+3);
            L = CS_GenLabel (D->Code, X);

            /* iny */
            X = NewCodeEntry (OP65_INY, AM65_IMP, 0, 0, D->OpEntry->LI);
            InsertEntry (D, X, D->OpIndex+4);

            /* cpy #count */
            Arg = MakeHexArg (D->OpEntry->RI->In.RegA);
            X = NewCodeEntry (OP65_CPY, AM65_IMM, Arg, 0, D->OpEntry->LI);
            InsertEntry (D, X, D->OpIndex+5);

            /* bne L */
            X = NewCodeEntry (OP65_BNE, AM65_BRA, L->Name, L, D->OpEntry->LI);
            InsertEntry (D, X, D->OpIndex+6);
        }

    }

    /* Remove the push and the call to the __bzero function */
    RemoveRemainders (D);

    /* We changed the sequence */
    return 1;
}



static unsigned Opt_staspidx (StackOpData* D)
/* Optimize the staspidx sequence */
{
    CodeEntry* X;

    /* Check if we're using a register variable */
    if (!IsRegVar (D)) {
        /* Store the value into the zeropage instead of pushing it */
        AddStoreX (D);
        AddStoreA (D);
    }

    /* Replace the store subroutine call by a direct op */
    X = NewCodeEntry (OP65_STA, AM65_ZP_INDY, D->ZPLo, 0, D->OpEntry->LI);
    InsertEntry (D, X, D->OpIndex+1);

    /* Remove the push and the call to the staspidx function */
    RemoveRemainders (D);

    /* We changed the sequence */
    return 1;
}



static unsigned Opt_staxspidx (StackOpData* D)
/* Optimize the staxspidx sequence */
{
    CodeEntry* X;

    /* Check if we're using a register variable */
    if (!IsRegVar (D)) {
        /* Store the value into the zeropage instead of pushing it */
        AddStoreX (D);
        AddStoreA (D);
    }

    /* Inline the store */

    /* sta (zp),y */
    X = NewCodeEntry (OP65_STA, AM65_ZP_INDY, D->ZPLo, 0, D->OpEntry->LI);
    InsertEntry (D, X, D->OpIndex+1);

    if (RegValIsKnown (D->OpEntry->RI->In.RegY)) {
        /* Value of Y is known */
        const char* Arg = MakeHexArg (D->OpEntry->RI->In.RegY + 1);
        X = NewCodeEntry (OP65_LDY, AM65_IMM, Arg, 0, D->OpEntry->LI);
    } else {
        X = NewCodeEntry (OP65_INY, AM65_IMP, 0, 0, D->OpEntry->LI);
    }
    InsertEntry (D, X, D->OpIndex+2);

    if (RegValIsKnown (D->OpEntry->RI->In.RegX)) {
        /* Value of X is known */
        const char* Arg = MakeHexArg (D->OpEntry->RI->In.RegX);
        X = NewCodeEntry (OP65_LDA, AM65_IMM, Arg, 0, D->OpEntry->LI);
    } else {
        /* Value unknown */
        X = NewCodeEntry (OP65_TXA, AM65_IMP, 0, 0, D->OpEntry->LI);
    }
    InsertEntry (D, X, D->OpIndex+3);

    /* sta (zp),y */
    X = NewCodeEntry (OP65_STA, AM65_ZP_INDY, D->ZPLo, 0, D->OpEntry->LI);
    InsertEntry (D, X, D->OpIndex+4);

    /* If we remove staxspidx, we must restore the Y register to what the
    ** function would return.
    */
    X = NewCodeEntry (OP65_LDY, AM65_IMM, "$00", 0, D->OpEntry->LI);
    InsertEntry (D, X, D->OpIndex+5);

    /* Remove the push and the call to the staxspidx function */
    RemoveRemainders (D);

    /* We changed the sequence */
    return 1;
}



static unsigned Opt_tosaddax (StackOpData* D)
/* Optimize the tosaddax sequence */
{
    CodeEntry*  X;
    CodeEntry*  N;

    /* We need the entry behind the add */
    CHECK (D->NextEntry != 0);

    /* Check if the X register is known and zero when the add is done, and
    ** if the add is followed by
    **
    **  ldy     #$00
    **  jsr     ldauidx         ; or ldaidx
    **
    ** If this is true, the addition does actually add an offset to a pointer
    ** before it is dereferenced. Since both subroutines take an offset in Y,
    ** we can pass the offset (instead of #$00) and remove the addition
    ** alltogether.
    */
    if (D->OpEntry->RI->In.RegX == 0                            &&
        D->NextEntry->OPC == OP65_LDY                           &&
        CE_IsKnownImm (D->NextEntry, 0)                         &&
        !CE_HasLabel (D->NextEntry)                             &&
        (N = CS_GetNextEntry (D->Code, D->OpIndex + 1)) != 0    &&
        (CE_IsCallTo (N, "ldauidx")                     ||
         CE_IsCallTo (N, "ldaidx"))) {

        int Signed = (strcmp (N->Arg, "ldaidx") == 0);

        /* Store the value into the zeropage instead of pushing it */
        AddStoreX (D);
        AddStoreA (D);

        /* Replace the ldy by a tay. Be sure to create the new entry before
        ** deleting the ldy, since we will reference the line info from this
        ** insn.
        */
        X = NewCodeEntry (OP65_TAY, AM65_IMP, 0, 0, D->NextEntry->LI);
        DelEntry (D, D->OpIndex + 1);
        InsertEntry (D, X, D->OpIndex + 1);

        /* Replace the call to ldaidx/ldauidx. Since X is already zero, and
        ** the ptr is in the zero page location, we just need to load from
        ** the pointer, and fix X in case of ldaidx.
        */
        X = NewCodeEntry (OP65_LDA, AM65_ZP_INDY, D->ZPLo, 0, N->LI);
        DelEntry (D, D->OpIndex + 2);
        InsertEntry (D, X, D->OpIndex + 2);
        if (Signed) {

            CodeLabel* L;

            /* Add sign extension - N is unused now */
            N = CS_GetNextEntry (D->Code, D->OpIndex + 2);
            CHECK (N != 0);
            L = CS_GenLabel (D->Code, N);

            X = NewCodeEntry (OP65_BPL, AM65_BRA, L->Name, L, X->LI);
            InsertEntry (D, X, D->OpIndex + 3);

            X = NewCodeEntry (OP65_DEX, AM65_IMP, 0, 0, X->LI);
            InsertEntry (D, X, D->OpIndex + 4);
        }

    } else {

        /* Store the value into the zeropage instead of pushing it */
        ReplacePushByStore (D);

        /* Inline the add */
        D->IP = D->OpIndex+1;

        /* clc */
        X = NewCodeEntry (OP65_CLC, AM65_IMP, 0, 0, D->OpEntry->LI);
        InsertEntry (D, X, D->IP++);

        /* Low byte */
        AddOpLow (D, OP65_ADC, &D->Lhs);

        /* High byte */
        if (D->PushEntry->RI->In.RegX == 0) {

            /* The high byte is the value in X plus the carry */
            CodeLabel* L = CS_GenLabel (D->Code, D->NextEntry);

            /* bcc L */
            X = NewCodeEntry (OP65_BCC, AM65_BRA, L->Name, L, D->OpEntry->LI);
            InsertEntry (D, X, D->IP++);

            /* inx */
            X = NewCodeEntry (OP65_INX, AM65_IMP, 0, 0, D->OpEntry->LI);
            InsertEntry (D, X, D->IP++);

        } else if (D->OpEntry->RI->In.RegX == 0                         &&
                   (RegValIsKnown (D->PushEntry->RI->In.RegX)   ||
                    (D->Lhs.X.Flags & LI_RELOAD_Y) == 0)) {

            /* The high byte is that of the first operand plus carry */
            CodeLabel* L;
            if (RegValIsKnown (D->PushEntry->RI->In.RegX)) {
                /* Value of first op high byte is known */
                const char* Arg = MakeHexArg (D->PushEntry->RI->In.RegX);
                X = NewCodeEntry (OP65_LDX, AM65_IMM, Arg, 0, D->OpEntry->LI);
            } else {
                /* Value of first op high byte is unknown. Load from ZP or
                ** original storage.
                */
                if (D->Lhs.X.Flags & LI_DIRECT) {
                    CodeEntry* LoadX = D->Lhs.X.LoadEntry;
                    X = NewCodeEntry (OP65_LDX, LoadX->AM, LoadX->Arg, 0, D->OpEntry->LI);
                } else {
                    X = NewCodeEntry (OP65_LDX, AM65_ZP, D->ZPHi, 0, D->OpEntry->LI);
                }
            }
            InsertEntry (D, X, D->IP++);

            /* bcc label */
            L = CS_GenLabel (D->Code, D->NextEntry);
            X = NewCodeEntry (OP65_BCC, AM65_BRA, L->Name, L, D->OpEntry->LI);
            InsertEntry (D, X, D->IP++);

            /* inx */
            X = NewCodeEntry (OP65_INX, AM65_IMP, 0, 0, D->OpEntry->LI);
            InsertEntry (D, X, D->IP++);
        } else {
            /* High byte is unknown */
            AddOpHigh (D, OP65_ADC, &D->Lhs, 1);
        }
    }

    /* Remove the push and the call to the tosaddax function */
    RemoveRemainders (D);

    /* We changed the sequence */
    return 1;
}



static unsigned Opt_tosandax (StackOpData* D)
/* Optimize the tosandax sequence */
{
    /* Store the value into the zeropage instead of pushing it */
    ReplacePushByStore (D);

    /* Inline the and, low byte */
    D->IP = D->OpIndex + 1;
    AddOpLow (D, OP65_AND, &D->Lhs);

    /* High byte */
    AddOpHigh (D, OP65_AND, &D->Lhs, 1);

    /* Remove the push and the call to the tosandax function */
    RemoveRemainders (D);

    /* We changed the sequence */
    return 1;
}



static unsigned Opt_tosaslax (StackOpData* D)
/* Optimize the tosaslax sequence */
{
    return Opt_tosshift (D, "aslaxy");
}



static unsigned Opt_tosasrax (StackOpData* D)
/* Optimize the tosasrax sequence */
{
    return Opt_tosshift (D, "asraxy");
}



static unsigned Opt_toseqax (StackOpData* D)
/* Optimize the toseqax sequence */
{
    return Opt_toseqax_tosneax (D, "booleq");
}



static unsigned Opt_tosgeax (StackOpData* D)
/* Optimize the tosgeax sequence */
{
    CodeEntry*  X;
    CodeLabel* L;

    /* Inline the sbc */
    D->IP = D->OpIndex+1;

    /* Must be true because of OP_RHS_LOAD */
    CHECK ((D->Rhs.A.Flags & D->Rhs.X.Flags & LI_DIRECT) != 0);

    /* Add code for low operand */
    AddOpLow (D, OP65_CMP, &D->Rhs);

    /* Add code for high operand */
    AddOpHigh (D, OP65_SBC, &D->Rhs, 0);

    /* eor #$80 */
    X = NewCodeEntry (OP65_EOR, AM65_IMM, "$80", 0, D->OpEntry->LI);
    InsertEntry (D, X, D->IP++);

    /* asl a */
    X = NewCodeEntry (OP65_ASL, AM65_ACC, "a", 0, D->OpEntry->LI);
    InsertEntry (D, X, D->IP++);
    L = CS_GenLabel (D->Code, X);

    /* Insert a bvs L before the eor insn */
    X = NewCodeEntry (OP65_BVS, AM65_BRA, L->Name, L, D->OpEntry->LI);
    InsertEntry (D, X, D->IP - 2);
    ++D->IP;

    /* lda #$00 */
    X = NewCodeEntry (OP65_LDA, AM65_IMM, "$00", 0, D->OpEntry->LI);
    InsertEntry (D, X, D->IP++);

    /* ldx #$00 */
    X = NewCodeEntry (OP65_LDX, AM65_IMM, "$00", 0, D->OpEntry->LI);
    InsertEntry (D, X, D->IP++);

    /* rol a */
    X = NewCodeEntry (OP65_ROL, AM65_ACC, "a", 0, D->OpEntry->LI);
    InsertEntry (D, X, D->IP++);

    /* Remove the push and the call to the tosgeax function */
    RemoveRemainders (D);

    /* We changed the sequence */
    return 1;
}



static unsigned Opt_tosltax (StackOpData* D)
/* Optimize the tosltax sequence */
{
    CodeEntry*  X;
    CodeLabel* L;


    /* Inline the compare */
    D->IP = D->OpIndex+1;

    /* Must be true because of OP_RHS_LOAD */
    CHECK ((D->Rhs.A.Flags & D->Rhs.X.Flags & LI_DIRECT) != 0);

    /* Add code for low operand */
    AddOpLow (D, OP65_CMP, &D->Rhs);

    /* Add code for high operand */
    AddOpHigh (D, OP65_SBC, &D->Rhs, 0);

    /* eor #$80 */
    X = NewCodeEntry (OP65_EOR, AM65_IMM, "$80", 0, D->OpEntry->LI);
    InsertEntry (D, X, D->IP++);

    /* asl a */
    X = NewCodeEntry (OP65_ASL, AM65_ACC, "a", 0, D->OpEntry->LI);
    InsertEntry (D, X, D->IP++);
    L = CS_GenLabel (D->Code, X);

    /* Insert a bvc L before the eor insn */
    X = NewCodeEntry (OP65_BVC, AM65_BRA, L->Name, L, D->OpEntry->LI);
    InsertEntry (D, X, D->IP - 2);
    ++D->IP;

    /* lda #$00 */
    X = NewCodeEntry (OP65_LDA, AM65_IMM, "$00", 0, D->OpEntry->LI);
    InsertEntry (D, X, D->IP++);

    /* ldx #$00 */
    X = NewCodeEntry (OP65_LDX, AM65_IMM, "$00", 0, D->OpEntry->LI);
    InsertEntry (D, X, D->IP++);

    /* rol a */
    X = NewCodeEntry (OP65_ROL, AM65_ACC, "a", 0, D->OpEntry->LI);
    InsertEntry (D, X, D->IP++);

    /* Remove the push and the call to the tosltax function */
    RemoveRemainders (D);

    /* We changed the sequence */
    return 1;
}



static unsigned Opt_tosneax (StackOpData* D)
/* Optimize the tosneax sequence */
{
    return Opt_toseqax_tosneax (D, "boolne");
}



static unsigned Opt_tosorax (StackOpData* D)
/* Optimize the tosorax sequence */
{
    /* Store the value into the zeropage instead of pushing it */
    ReplacePushByStore (D);

    /* Inline the or, low byte */
    D->IP = D->OpIndex + 1;
    AddOpLow (D, OP65_ORA, &D->Lhs);

    /* High byte */
    AddOpHigh (D, OP65_ORA, &D->Lhs, 1);

    /* Remove the push and the call to the tosorax function */
    RemoveRemainders (D);

    /* We changed the sequence */
    return 1;
}



static unsigned Opt_tosshlax (StackOpData* D)
/* Optimize the tosshlax sequence */
{
    return Opt_tosshift (D, "shlaxy");
}



static unsigned Opt_tosshrax (StackOpData* D)
/* Optimize the tosshrax sequence */
{
    return Opt_tosshift (D, "shraxy");
}



static unsigned Opt_tossubax (StackOpData* D)
/* Optimize the tossubax sequence. Note: subtraction is not commutative! */
{
    CodeEntry*  X;


    /* Inline the sbc */
    D->IP = D->OpIndex+1;

    /* sec */
    X = NewCodeEntry (OP65_SEC, AM65_IMP, 0, 0, D->OpEntry->LI);
    InsertEntry (D, X, D->IP++);

    /* Must be true because of OP_RHS_LOAD */
    CHECK ((D->Rhs.A.Flags & D->Rhs.X.Flags & LI_DIRECT) != 0);

    /* Add code for low operand */
    AddOpLow (D, OP65_SBC, &D->Rhs);

    /* Add code for high operand */
    AddOpHigh (D, OP65_SBC, &D->Rhs, 1);

    /* Remove the push and the call to the tossubax function */
    RemoveRemainders (D);

    /* We changed the sequence */
    return 1;
}



static unsigned Opt_tosugeax (StackOpData* D)
/* Optimize the tosugeax sequence */
{
    CodeEntry*  X;


    /* Inline the sbc */
    D->IP = D->OpIndex+1;

    /* Must be true because of OP_RHS_LOAD */
    CHECK ((D->Rhs.A.Flags & D->Rhs.X.Flags & LI_DIRECT) != 0);

    /* Add code for low operand */
    AddOpLow (D, OP65_CMP, &D->Rhs);

    /* Add code for high operand */
    AddOpHigh (D, OP65_SBC, &D->Rhs, 0);

    /* lda #$00 */
    X = NewCodeEntry (OP65_LDA, AM65_IMM, "$00", 0, D->OpEntry->LI);
    InsertEntry (D, X, D->IP++);

    /* ldx #$00 */
    X = NewCodeEntry (OP65_LDX, AM65_IMM, "$00", 0, D->OpEntry->LI);
    InsertEntry (D, X, D->IP++);

    /* rol a */
    X = NewCodeEntry (OP65_ROL, AM65_ACC, "a", 0, D->OpEntry->LI);
    InsertEntry (D, X, D->IP++);

    /* Remove the push and the call to the tosugeax function */
    RemoveRemainders (D);

    /* We changed the sequence */
    return 1;
}



static unsigned Opt_tosugtax (StackOpData* D)
/* Optimize the tosugtax sequence */
{
    CodeEntry*  X;


    /* Inline the sbc */
    D->IP = D->OpIndex+1;

    /* Must be true because of OP_RHS_LOAD */
    CHECK ((D->Rhs.A.Flags & D->Rhs.X.Flags & LI_DIRECT) != 0);

    /* sec */
    X = NewCodeEntry (OP65_SEC, AM65_IMP, 0, 0, D->OpEntry->LI);
    InsertEntry (D, X, D->IP++);

    /* Add code for low operand */
    AddOpLow (D, OP65_SBC, &D->Rhs);

    /* We need the zero flag, so remember the immediate result */
    X = NewCodeEntry (OP65_STA, AM65_ZP, "tmp1", 0, D->OpEntry->LI);
    InsertEntry (D, X, D->IP++);

    /* Add code for high operand */
    AddOpHigh (D, OP65_SBC, &D->Rhs, 0);

    /* Set Z flag */
    X = NewCodeEntry (OP65_ORA, AM65_ZP, "tmp1", 0, D->OpEntry->LI);
    InsertEntry (D, X, D->IP++);

    /* Transform to boolean */
    X = NewCodeEntry (OP65_JSR, AM65_ABS, "boolugt", 0, D->OpEntry->LI);
    InsertEntry (D, X, D->IP++);

    /* Remove the push and the call to the operator function */
    RemoveRemainders (D);

    /* We changed the sequence */
    return 1;
}



static unsigned Opt_tosuleax (StackOpData* D)
/* Optimize the tosuleax sequence */
{
    CodeEntry*  X;


    /* Inline the sbc */
    D->IP = D->OpIndex+1;

    /* Must be true because of OP_RHS_LOAD */
    CHECK ((D->Rhs.A.Flags & D->Rhs.X.Flags & LI_DIRECT) != 0);

    /* sec */
    X = NewCodeEntry (OP65_SEC, AM65_IMP, 0, 0, D->OpEntry->LI);
    InsertEntry (D, X, D->IP++);

    /* Add code for low operand */
    AddOpLow (D, OP65_SBC, &D->Rhs);

    /* We need the zero flag, so remember the immediate result */
    X = NewCodeEntry (OP65_STA, AM65_ZP, "tmp1", 0, D->OpEntry->LI);
    InsertEntry (D, X, D->IP++);

    /* Add code for high operand */
    AddOpHigh (D, OP65_SBC, &D->Rhs, 0);

    /* Set Z flag */
    X = NewCodeEntry (OP65_ORA, AM65_ZP, "tmp1", 0, D->OpEntry->LI);
    InsertEntry (D, X, D->IP++);

    /* Transform to boolean */
    X = NewCodeEntry (OP65_JSR, AM65_ABS, "boolule", 0, D->OpEntry->LI);
    InsertEntry (D, X, D->IP++);

    /* Remove the push and the call to the operator function */
    RemoveRemainders (D);

    /* We changed the sequence */
    return 1;
}



static unsigned Opt_tosultax (StackOpData* D)
/* Optimize the tosultax sequence */
{
    CodeEntry*  X;


    /* Inline the sbc */
    D->IP = D->OpIndex+1;

    /* Must be true because of OP_RHS_LOAD */
    CHECK ((D->Rhs.A.Flags & D->Rhs.X.Flags & LI_DIRECT) != 0);

    /* Add code for low operand */
    AddOpLow (D, OP65_CMP, &D->Rhs);

    /* Add code for high operand */
    AddOpHigh (D, OP65_SBC, &D->Rhs, 0);

    /* Transform to boolean */
    X = NewCodeEntry (OP65_JSR, AM65_ABS, "boolult", 0, D->OpEntry->LI);
    InsertEntry (D, X, D->IP++);

    /* Remove the push and the call to the operator function */
    RemoveRemainders (D);

    /* We changed the sequence */
    return 1;
}



static unsigned Opt_tosxorax (StackOpData* D)
/* Optimize the tosxorax sequence */
{
    CodeEntry*  X;


    /* Store the value into the zeropage instead of pushing it */
    ReplacePushByStore (D);

    /* Inline the xor, low byte */
    D->IP = D->OpIndex + 1;
    AddOpLow (D, OP65_EOR, &D->Lhs);

    /* High byte */
    if (RegValIsKnown (D->PushEntry->RI->In.RegX) &&
        RegValIsKnown (D->OpEntry->RI->In.RegX)) {
        /* Both values known, precalculate the result */
        const char* Arg = MakeHexArg (D->PushEntry->RI->In.RegX ^ D->OpEntry->RI->In.RegX);
        X = NewCodeEntry (OP65_LDX, AM65_IMM, Arg, 0, D->OpEntry->LI);
        InsertEntry (D, X, D->IP++);
    } else if (D->PushEntry->RI->In.RegX != 0) {
        /* High byte is unknown */
        AddOpHigh (D, OP65_EOR, &D->Lhs, 1);
    }

    /* Remove the push and the call to the tosandax function */
    RemoveRemainders (D);

    /* We changed the sequence */
    return 1;
}



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



static const OptFuncDesc FuncTable[] = {
    { "__bzero",    Opt___bzero,   REG_NONE, OP_X_ZERO | OP_A_KNOWN     },
    { "staspidx",   Opt_staspidx,  REG_NONE, OP_NONE                    },
    { "staxspidx",  Opt_staxspidx, REG_AX,   OP_NONE                    },
    { "tosaddax",   Opt_tosaddax,  REG_NONE, OP_NONE                    },
    { "tosandax",   Opt_tosandax,  REG_NONE, OP_NONE                    },
    { "tosaslax",   Opt_tosaslax,  REG_NONE, OP_NONE                    },
    { "tosasrax",   Opt_tosasrax,  REG_NONE, OP_NONE                    },
    { "toseqax",    Opt_toseqax,   REG_NONE, OP_NONE                    },
    { "tosgeax",    Opt_tosgeax,   REG_NONE, OP_RHS_LOAD_DIRECT         },
    { "tosltax",    Opt_tosltax,   REG_NONE, OP_RHS_LOAD_DIRECT         },
    { "tosneax",    Opt_tosneax,   REG_NONE, OP_NONE                    },
    { "tosorax",    Opt_tosorax,   REG_NONE, OP_NONE                    },
    { "tosshlax",   Opt_tosshlax,  REG_NONE, OP_NONE                    },
    { "tosshrax",   Opt_tosshrax,  REG_NONE, OP_NONE                    },
    { "tossubax",   Opt_tossubax,  REG_NONE, OP_RHS_LOAD_DIRECT         },
    { "tosugeax",   Opt_tosugeax,  REG_NONE, OP_RHS_LOAD_DIRECT         },
    { "tosugtax",   Opt_tosugtax,  REG_NONE, OP_RHS_LOAD_DIRECT         },
    { "tosuleax",   Opt_tosuleax,  REG_NONE, OP_RHS_LOAD_DIRECT         },
    { "tosultax",   Opt_tosultax,  REG_NONE, OP_RHS_LOAD_DIRECT         },
    { "tosxorax",   Opt_tosxorax,  REG_NONE, OP_NONE                    },
};
#define FUNC_COUNT (sizeof(FuncTable) / sizeof(FuncTable[0]))



static int CmpFunc (const void* Key, const void* Func)
/* Compare function for bsearch */
{
    return strcmp (Key, ((const OptFuncDesc*) Func)->Name);
}



static const OptFuncDesc* FindFunc (const char* Name)
/* Find the function with the given name. Return a pointer to the table entry
** or NULL if the function was not found.
*/
{
    return bsearch (Name, FuncTable, FUNC_COUNT, sizeof(OptFuncDesc), CmpFunc);
}



static int CmpHarmless (const void* Key, const void* Entry)
/* Compare function for bsearch */
{
    return strcmp (Key, *(const char**)Entry);
}



static int HarmlessCall (const char* Name)
/* Check if this is a call to a harmless subroutine that will not interrupt
** the pushax/op sequence when encountered.
*/
{
    static const char* Tab[] = {
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



static void ResetStackOpData (StackOpData* Data)
/* Reset the given data structure */
{
    Data->OptFunc       = 0;
    Data->UsedRegs      = REG_NONE;

    ClearLoadInfo (&Data->Lhs);
    ClearLoadInfo (&Data->Rhs);

    Data->PushIndex     = -1;
    Data->OpIndex       = -1;
}



static int PreCondOk (StackOpData* D)
/* Check if the preconditions for a call to the optimizer subfunction are
** satisfied. As a side effect, this function will also choose the zero page
** register to use.
*/
{
    /* Check the flags */
    unsigned UnusedRegs = D->OptFunc->UnusedRegs;
    if (UnusedRegs != REG_NONE &&
        (GetRegInfo (D->Code, D->OpIndex+1, UnusedRegs) & UnusedRegs) != 0) {
        /* Cannot optimize */
        return 0;
    }
    if ((D->OptFunc->Flags & OP_A_KNOWN) != 0 &&
        RegValIsUnknown (D->OpEntry->RI->In.RegA)) {
        /* Cannot optimize */
        return 0;
    }
    if ((D->OptFunc->Flags & OP_X_ZERO) != 0 &&
        D->OpEntry->RI->In.RegX != 0) {
        /* Cannot optimize */
        return 0;
    }
    if ((D->OptFunc->Flags & OP_LHS_LOAD) != 0) {
        if (D->Lhs.A.LoadIndex < 0 || D->Lhs.X.LoadIndex < 0) {
            /* Cannot optimize */
            return 0;
        } else if ((D->OptFunc->Flags & OP_LHS_LOAD_DIRECT) != 0) {
            if ((D->Lhs.A.Flags & D->Lhs.X.Flags & LI_DIRECT) == 0) {
                /* Cannot optimize */
                return 0;
            }
        }
    }
    if ((D->OptFunc->Flags & OP_RHS_LOAD) != 0) {
        if (D->Rhs.A.LoadIndex < 0 || D->Rhs.X.LoadIndex < 0) {
            /* Cannot optimize */
            return 0;
        } else if ((D->OptFunc->Flags & OP_RHS_LOAD_DIRECT) != 0) {
            if ((D->Rhs.A.Flags & D->Rhs.X.Flags & LI_DIRECT) == 0) {
                /* Cannot optimize */
                return 0;
            }
        }
    }
    if ((D->Rhs.A.Flags | D->Rhs.X.Flags) & LI_DUP_LOAD) {
        /* Cannot optimize */
        return 0;
    }

    /* Determine the zero page locations to use */
    if ((D->UsedRegs & REG_PTR1) == REG_NONE) {
        D->ZPLo = "ptr1";
        D->ZPHi = "ptr1+1";
    } else if ((D->UsedRegs & REG_SREG) == REG_NONE) {
        D->ZPLo = "sreg";
        D->ZPHi = "sreg+1";
    } else if ((D->UsedRegs & REG_PTR2) == REG_NONE) {
        D->ZPLo = "ptr2";
        D->ZPHi = "ptr2+1";
    } else {
        /* No registers available */
        return 0;
    }

    /* Determine if we have a basic block */
    return CS_IsBasicBlock (D->Code, D->PushIndex, D->OpIndex);
}



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



unsigned OptStackOps (CodeSeg* S)
/* Optimize operations that take operands via the stack */
{
    unsigned            Changes = 0;    /* Number of changes in one run */
    StackOpData         Data;
    int                 I;
    int                 OldEntryCount;  /* Old number of entries */
    unsigned            UsedRegs = 0;   /* Registers used */
    unsigned            ChangedRegs = 0;/* Registers changed */


    enum {
        Initialize,
        Search,
        FoundPush,
        FoundOp
    } State = Initialize;


    /* Remember the code segment in the info struct */
    Data.Code = S;

    /* Look for a call to pushax followed by a call to some other function
    ** that takes it's first argument on the stack, and the second argument
    ** in the primary register.
    ** It depends on the code between the two if we can handle/transform the
    ** sequence, so check this code for the following list of things:
    **
    **  - the range must be a basic block (one entry, one exit)
    **  - there may not be accesses to local variables with unknown
    **    offsets (because we have to adjust these offsets).
    **  - no subroutine calls
    **  - no jump labels
    **
    ** Since we need a zero page register later, do also check the
    ** intermediate code for zero page use.
    */
    I = 0;
    while (I < (int)CS_GetEntryCount (S)) {

        /* Get the next entry */
        CodeEntry* E = CS_GetEntry (S, I);

        /* Actions depend on state */
        switch (State) {

            case Initialize:
                ResetStackOpData (&Data);
                UsedRegs = ChangedRegs = REG_NONE;
                State = Search;
                /* FALLTHROUGH */

            case Search:
                /* While searching, track register load insns, so we can tell
                ** what is in a register once pushax is encountered.
                */
                if (CE_HasLabel (E)) {
                    /* Currently we don't track across branches */
                    ClearLoadInfo (&Data.Lhs);
                }
                if (CE_IsCallTo (E, "pushax")) {
                    Data.PushIndex = I;
                    State = FoundPush;
                } else {
                    /* Track load insns */
                    TrackLoads (&Data.Lhs, E, I);
                }
                break;

            case FoundPush:
                /* We' found a pushax before. Search for a stack op that may
                ** follow and in the meantime, track zeropage usage and check
                ** for code that will disable us from translating the sequence.
                */
                if (CE_HasLabel (E)) {
                    /* Currently we don't track across branches */
                    ClearLoadInfo (&Data.Rhs);
                }
                if (E->OPC == OP65_JSR) {

                    /* Subroutine call: Check if this is one of the functions,
                    ** we're going to replace.
                    */
                    Data.OptFunc = FindFunc (E->Arg);
                    if (Data.OptFunc) {
                        /* Remember the op index and go on */
                        Data.OpIndex = I;
                        Data.OpEntry = E;
                        State = FoundOp;
                        break;
                    } else if (!HarmlessCall (E->Arg)) {
                        /* A call to an unkown subroutine: We need to start
                        ** over after the last pushax. Note: This will also
                        ** happen if we encounter a call to pushax!
                        */
                        I = Data.PushIndex;
                        State = Initialize;
                        break;
                    } else {
                        /* Track register usage */
                        Data.UsedRegs |= (E->Use | E->Chg);
                        TrackLoads (&Data.Rhs, E, I);
                    }

                } else if (E->Info & OF_STORE && (E->Chg & REG_ZP) == 0) {

                    /* Too dangerous - there may be a change of a variable
                    ** within the sequence.
                    */
                    I = Data.PushIndex;
                    State = Initialize;
                    break;

                } else if ((E->Use & REG_SP) != 0                       &&
                           (E->AM != AM65_ZP_INDY               ||
                            RegValIsUnknown (E->RI->In.RegY)    ||
                            E->RI->In.RegY < 2)) {

                    /* If we are using the stack, and we don't have "indirect Y"
                    ** addressing mode, or the value of Y is unknown, or less
                    ** than two, we cannot cope with this piece of code. Having
                    ** an unknown value of Y means that we cannot correct the
                    ** stack offset, while having an offset less than two means
                    ** that the code works with the value on stack which is to
                    ** be removed.
                    */
                    I = Data.PushIndex;
                    State = Initialize;
                    break;

                } else {
                    /* Other stuff: Track register usage */
                    Data.UsedRegs |= (E->Use | E->Chg);
                    TrackLoads (&Data.Rhs, E, I);
                }
                /* If the registers from the push (A/X) are used before they're
                ** changed, we cannot change the sequence, because this would
                ** with a high probability change the register contents.
                */
                UsedRegs |= E->Use;
                if ((UsedRegs & ~ChangedRegs) & REG_AX) {
                    I = Data.PushIndex;
                    State = Initialize;
                    break;
                }
                ChangedRegs |= E->Chg;
                break;

            case FoundOp:
                /* Track zero page location usage beyond this point */
                Data.UsedRegs |= GetRegInfo (S, I, REG_SREG | REG_PTR1 | REG_PTR2);

                /* Finalize the load info */
                FinalizeLoadInfo (&Data.Lhs, S);
                FinalizeLoadInfo (&Data.Rhs, S);

                /* If the Lhs loads do load from zeropage, we have to include
                ** them into UsedRegs registers used. The Rhs loads have already
                ** been tracked.
                */
                if (Data.Lhs.A.LoadEntry && Data.Lhs.A.LoadEntry->AM == AM65_ZP) {
                    Data.UsedRegs |= Data.Lhs.A.LoadEntry->Use;
                }
                if (Data.Lhs.X.LoadEntry && Data.Lhs.X.LoadEntry->AM == AM65_ZP) {
                    Data.UsedRegs |= Data.Lhs.X.LoadEntry->Use;
                }

                /* Check the preconditions. If they aren't ok, reset the insn
                ** pointer to the pushax and start over. We will loose part of
                ** load tracking but at least a/x has probably lost between
                ** pushax and here and will be tracked again when restarting.
                */
                if (!PreCondOk (&Data)) {
                    I = Data.PushIndex;
                    State = Initialize;
                    break;
                }

                /* Prepare the remainder of the data structure. */
                Data.PrevEntry = CS_GetPrevEntry (S, Data.PushIndex);
                Data.PushEntry = CS_GetEntry (S, Data.PushIndex);
                Data.OpEntry   = CS_GetEntry (S, Data.OpIndex);
                Data.NextEntry = CS_GetNextEntry (S, Data.OpIndex);

                /* Remember the current number of code lines */
                OldEntryCount = CS_GetEntryCount (S);

                /* Adjust stack offsets to account for the upcoming removal */
                AdjustStackOffset (&Data, 2);

                /* Regenerate register info, since AdjustStackOffset changed
                ** the code
                */
                CS_GenRegInfo (S);

                /* Call the optimizer function */
                Changes += Data.OptFunc->Func (&Data);

                /* Since the function may have added or deleted entries,
                ** correct the index.
                */
                I += CS_GetEntryCount (S) - OldEntryCount;

                /* Regenerate register info */
                CS_GenRegInfo (S);

                /* Done */
                State = Initialize;
                continue;

        }

        /* Next entry */
        ++I;

    }

    /* Return the number of changes made */
    return Changes;
}
