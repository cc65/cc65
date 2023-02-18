/*****************************************************************************/
/*                                                                           */
/*                                 coptstop.c                                */
/*                                                                           */
/*           Optimize operations that take operands via the stack            */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2001-2019, Ullrich von Bassewitz                                      */
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
#include "codeoptutil.h"
#include "coptstop.h"
#include "error.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Flags for the functions */
typedef enum {
    OP_NONE              = 0x00,         /* Nothing special */
    OP_A_KNOWN           = 0x01,         /* Value of A must be known */
    OP_X_ZERO            = 0x02,         /* X must be zero */
    OP_LHS_LOAD          = 0x04,         /* Must have load insns for LHS */
    OP_LHS_SAME_BY_OP    = 0x08,         /* Load result of LHS must be the same if relocated to op */
    OP_LHS_LOAD_DIRECT   = 0x0C,         /* Must have direct load insn for LHS */
    OP_RHS_LOAD          = 0x10,         /* Must have load insns for RHS */
    OP_RHS_SAME_BY_OP    = 0x20,         /* Load result of RHS must be the same if relocated to op */
    OP_RHS_LOAD_DIRECT   = 0x30,         /* Must have direct load insn for RHS */
    OP_AX_INTERCHANGE    = 0x40,         /* Preconditions of A/X may be interchanged */
    OP_LR_INTERCHANGE    = 0x80,         /* Preconditions of LHS/RHS may be interchanged */
    OP_LHS_SAME_BY_PUSH  = 0x0100,       /* LHS must load the same content if relocated to push */
    OP_RHS_SAME_BY_PUSH  = 0x0200,       /* RHS must load the same content if relocated to push */
    OP_LHS_SAME_BY_RHS   = 0x0400,       /* LHS must load the same content if relocated to RHS */
    OP_RHS_SAME_BY_LHS   = 0x0800,       /* RHS must load the same content if relocated to LHS */
    OP_LHS_REMOVE        = 0x1000,       /* LHS must be removable or RHS may use ZP store/load */
    OP_LHS_REMOVE_DIRECT = 0x3000,       /* LHS must be directly removable */
    OP_RHS_REMOVE        = 0x4000,       /* RHS must be removable or LHS may use ZP store/load */
    OP_RHS_REMOVE_DIRECT = 0xC000,       /* RHS must be directly removable */
} OP_FLAGS;

/* Structure that describes an optimizer subfunction for a specific op */
typedef unsigned (*OptFunc) (StackOpData* D);
typedef struct OptFuncDesc OptFuncDesc;
struct OptFuncDesc {
    const char*         Name;           /* Name of the replaced runtime function */
    OptFunc             Func;           /* Function pointer */
    unsigned            UnusedRegs;     /* Regs that must not be used later */
    OP_FLAGS            Flags;          /* Flags */
};



/*****************************************************************************/
/*                                  Helpers                                  */
/*****************************************************************************/



static int SameRegAValue (StackOpData* D)
/* Check if Rhs Reg A == Lhs Reg A */
{
    RegInfo* LRI = GetLastChangedRegInfo (D, &D->Lhs.A);
    RegInfo* RRI = GetLastChangedRegInfo (D, &D->Rhs.A);

    /* RHS can have a -1 ChgIndex only if it is carried over from LHS */
    if (RRI == 0                                    ||
        (D->Rhs.A.ChgIndex >= 0         &&
         D->Rhs.A.ChgIndex == D->Lhs.A.ChgIndex)    ||
        (LRI != 0                       &&
         RegValIsKnown (LRI->Out.RegA)  &&
         RegValIsKnown (RRI->Out.RegA)  &&
         (LRI->Out.RegA & 0xFF) == (RRI->Out.RegA & 0xFF))) {

        return 1;
    }

    return 0;

}



static int SameRegXValue (StackOpData* D)
/* Check if Rhs Reg X == Lhs Reg X */
{
    RegInfo* LRI = GetLastChangedRegInfo (D, &D->Lhs.X);
    RegInfo* RRI = GetLastChangedRegInfo (D, &D->Rhs.X);

    if (RRI == 0                                    ||
        (D->Rhs.X.ChgIndex >= 0         &&
         D->Rhs.X.ChgIndex == D->Lhs.X.ChgIndex)    ||
        (LRI != 0                       &&
         RegValIsKnown (LRI->Out.RegX)  &&
         RegValIsKnown (RRI->Out.RegX)  &&
         (LRI->Out.RegX & 0xFF) == (RRI->Out.RegX & 0xFF))) {

        return 1;
    }

    return 0;

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

        /* Lhs load entries can be removed if not used later */
        D->Lhs.X.Flags |= LI_REMOVE;
        D->Lhs.A.Flags |= LI_REMOVE;

    } else if ((D->Rhs.A.Flags & (LI_DIRECT | LI_RELOAD_Y)) == LI_DIRECT &&
               (D->Rhs.X.Flags & (LI_DIRECT | LI_RELOAD_Y)) == LI_DIRECT &&
               D->RhsMultiChg == 0) {

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

        /* Rhs load entries must be removed */
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
        AddStoreLhsX (D);
        AddStoreLhsA (D);

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

        /* Lhs load entries can be removed if not used later */
        D->Lhs.X.Flags |= LI_REMOVE;
        D->Lhs.A.Flags |= LI_REMOVE;

    } else {

        /* Save lhs into zeropage and reload later */
        AddStoreLhsX (D);
        AddStoreLhsA (D);

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
        AddStoreLhsX (D);
        AddStoreLhsA (D);
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
        AddStoreLhsX (D);
        AddStoreLhsA (D);
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
    const char* Arg = 0;

    /* Check if we're using a register variable */
    if (!IsRegVar (D)) {
        /* Store the value into the zeropage instead of pushing it */
        AddStoreLhsX (D);
        AddStoreLhsA (D);
    }

    /* Inline the store */

    /* sta (zp),y */
    X = NewCodeEntry (OP65_STA, AM65_ZP_INDY, D->ZPLo, 0, D->OpEntry->LI);
    InsertEntry (D, X, D->OpIndex+1);

    if (RegValIsKnown (D->OpEntry->RI->In.RegY)) {
        /* Value of Y is known */
        Arg = MakeHexArg (D->OpEntry->RI->In.RegY + 1);
        X = NewCodeEntry (OP65_LDY, AM65_IMM, Arg, 0, D->OpEntry->LI);
    } else {
        X = NewCodeEntry (OP65_INY, AM65_IMP, 0, 0, D->OpEntry->LI);
    }
    InsertEntry (D, X, D->OpIndex+2);

    if (RegValIsKnown (D->OpEntry->RI->In.RegX)) {
        /* Value of X is known */
        Arg = MakeHexArg (D->OpEntry->RI->In.RegX);
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
    if (RegValIsKnown (D->OpEntry->RI->In.RegY)) {
        Arg = MakeHexArg (D->OpEntry->RI->In.RegY);
        X = NewCodeEntry (OP65_LDY, AM65_IMM, Arg, 0, D->OpEntry->LI);
    } else {
        X = NewCodeEntry (OP65_DEY, AM65_IMP, 0, 0, D->OpEntry->LI);
    }
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
        AddStoreLhsX (D);
        AddStoreLhsA (D);

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

    /* Rhs load entries must be removed */
    D->Rhs.X.Flags |= LI_REMOVE;
    D->Rhs.A.Flags |= LI_REMOVE;

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

    /* Rhs load entries must be removed */
    D->Rhs.X.Flags |= LI_REMOVE;
    D->Rhs.A.Flags |= LI_REMOVE;

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

    /* Rhs load entries must be removed */
    D->Rhs.X.Flags |= LI_REMOVE;
    D->Rhs.A.Flags |= LI_REMOVE;

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

    /* Rhs load entries must be removed */
    D->Rhs.X.Flags |= LI_REMOVE;
    D->Rhs.A.Flags |= LI_REMOVE;

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

    /* Rhs load entries must be removed */
    D->Rhs.X.Flags |= LI_REMOVE;
    D->Rhs.A.Flags |= LI_REMOVE;

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

    /* Rhs load entries must be removed */
    D->Rhs.X.Flags |= LI_REMOVE;
    D->Rhs.A.Flags |= LI_REMOVE;

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

    /* Rhs load entries must be removed */
    D->Rhs.X.Flags |= LI_REMOVE;
    D->Rhs.A.Flags |= LI_REMOVE;

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
/*            Optimization functions when hi-bytes can be ignored            */
/*****************************************************************************/



static unsigned Opt_a_toscmpbool (StackOpData* D, const char* BoolTransformer)
/* Optimize the TOS compare sequence with a bool transformer */
{
    CodeEntry* X;
    cmp_t      Cond;

    D->IP = D->OpIndex + 1;

    if (!D->RhsMultiChg                     &&
        (D->Rhs.A.Flags & LI_DIRECT) != 0   &&
        (D->Rhs.A.LoadEntry->Flags & CEF_DONT_REMOVE) == 0) {

        /* cmp */
        AddOpLow (D, OP65_CMP, &D->Rhs);

        /* Rhs low-byte load must be removed and hi-byte load may be removed */
        D->Rhs.X.Flags |= LI_REMOVE;
        D->Rhs.A.Flags |= LI_REMOVE;

    } else if ((D->Lhs.A.Flags & LI_DIRECT) != 0) {
        /* If the lhs is direct (but not stack relative), encode compares with lhs,
        ** effectively reversing the order (which doesn't matter for == and !=).
        */
        Cond = FindBoolCmpCond (BoolTransformer);
        Cond = GetRevertedCond (Cond);
        BoolTransformer = GetBoolTransformer (Cond);

        /* This shouldn't fail */
        CHECK (BoolTransformer);

        /* cmp */
        AddOpLow (D, OP65_CMP, &D->Lhs);

        /* Lhs load entries can be removed if not used later */
        D->Lhs.X.Flags |= LI_REMOVE;
        D->Lhs.A.Flags |= LI_REMOVE;

    } else {
        /* We'll do reverse-compare */
        Cond = FindBoolCmpCond (BoolTransformer);
        Cond = GetRevertedCond (Cond);
        BoolTransformer = GetBoolTransformer (Cond);

        /* This shouldn't fail */
        CHECK (BoolTransformer);

        /* Save lhs into zeropage */
        AddStoreLhsA (D);
        /* AddStoreLhsA may have moved the OpIndex, recalculate insertion point to prevent label migration. */
        D->IP = D->OpIndex + 1;

        /* cmp */
        X = NewCodeEntry (OP65_CMP, AM65_ZP, D->ZPLo, 0, D->OpEntry->LI);
        InsertEntry (D, X, D->IP++);

    }

    /* Create a call to the boolean transformer function. This is needed for all
    ** variants.
    */
    X = NewCodeEntry (OP65_JSR, AM65_ABS, BoolTransformer, 0, D->OpEntry->LI);
    InsertEntry (D, X, D->IP++);

    /* Remove the push and the call to the TOS function */
    RemoveRemainders (D);

    /* We changed the sequence */
    return 1;
}



static unsigned Opt_a_toseq (StackOpData* D)
/* Optimize the toseqax sequence */
{
    return Opt_a_toscmpbool (D, "booleq");
}



static unsigned Opt_a_tosicmp (StackOpData* D)
/* Replace tosicmp with CMP */
{
    CodeEntry*  X;
    RegInfo*    RI;
    const char* Arg;

    if (!SameRegAValue (D)) {
        /* Because of SameRegAValue */
        CHECK (D->Rhs.A.ChgIndex >= 0);

        /* Store LHS in ZP and reload it before op */
        X = NewCodeEntry (OP65_STA, AM65_ZP, D->ZPLo, 0, D->PushEntry->LI);
        InsertEntry (D, X, D->PushIndex + 1);
        X = NewCodeEntry (OP65_LDA, AM65_ZP, D->ZPLo, 0, D->PushEntry->LI);
        InsertEntry (D, X, D->OpIndex);

        D->IP = D->OpIndex + 1;

        if ((D->Rhs.A.Flags & LI_DIRECT) == 0) {
            /* RHS src is not directly comparable */
            X = NewCodeEntry (OP65_STA, AM65_ZP, D->ZPHi, 0, D->OpEntry->LI);
            InsertEntry (D, X, D->Rhs.A.ChgIndex + 1);
            /* RHS insertion may have moved the OpIndex, recalculate insertion point to prevent label migration. */
            D->IP = D->OpIndex + 1;

            /* Cmp with stored RHS */
            X = NewCodeEntry (OP65_CMP, AM65_ZP, D->ZPHi, 0, D->OpEntry->LI);
            InsertEntry (D, X, D->IP++);
        } else {
            if ((D->Rhs.A.Flags & LI_RELOAD_Y) == 0) {
                /* Cmp directly with RHS src */
                X = NewCodeEntry (OP65_CMP, AM65_ZP, D->Rhs.A.LoadEntry->Arg, 0, D->OpEntry->LI);
                InsertEntry (D, X, D->IP++);
            } else {
                /* ldy #offs */
                if ((D->Rhs.A.Flags & LI_CHECK_Y) == 0) {
                    X = NewCodeEntry (OP65_LDY, AM65_IMM, MakeHexArg (D->Rhs.A.Offs), 0, D->OpEntry->LI);
                } else {
                    X = NewCodeEntry (OP65_LDY, D->Rhs.A.LoadYEntry->AM, D->Rhs.A.LoadYEntry->Arg, 0, D->OpEntry->LI);
                }
                InsertEntry (D, X, D->IP++);

                /* cmp src,y OR cmp (sp),y */
                if (D->Rhs.A.LoadEntry->OPC == OP65_JSR) {
                    /* opc (sp),y */
                    X = NewCodeEntry (OP65_CMP, AM65_ZP_INDY, "sp", 0, D->OpEntry->LI);
                } else {
                    /* opc src,y */
                    X = NewCodeEntry (OP65_CMP, D->Rhs.A.LoadEntry->AM, D->Rhs.A.LoadEntry->Arg, 0, D->OpEntry->LI);
                }
                InsertEntry (D, X, D->IP++);
            }

            /* RHS may be removed */
            D->Rhs.A.Flags |= LI_REMOVE;
            D->Rhs.X.Flags |= LI_REMOVE;
        }

        /* Fix up the N/V flags: N = ~C, V = 0 */
        Arg = MakeHexArg (0);
        X = NewCodeEntry (OP65_LDA, AM65_IMM, Arg, 0, D->OpEntry->LI);
        InsertEntry (D, X, D->IP++);
        X = NewCodeEntry (OP65_SBC, AM65_IMM, Arg, 0, D->OpEntry->LI);
        InsertEntry (D, X, D->IP++);
        Arg = MakeHexArg (0x01);
        X = NewCodeEntry (OP65_ORA, AM65_IMM, Arg, 0, D->OpEntry->LI);
        InsertEntry (D, X, D->IP++);

        /* jeq L1 */
        CodeLabel* Label = CS_GenLabel (D->Code, CS_GetEntry (D->Code, D->IP));
        X = NewCodeEntry (OP65_JEQ, AM65_BRA, Label->Name, Label, X->LI);
        InsertEntry (D, X, D->IP-3);

    } else {
        /* Just clear A,Z,N; and set C */
        Arg = MakeHexArg (0);
        if ((RI = GetLastChangedRegInfo (D, &D->Lhs.A)) != 0 &&
            RegValIsKnown (RI->Out.RegA)                     &&
            (RI->Out.RegA & 0xFF) == 0) {

            X = NewCodeEntry (OP65_CMP, AM65_IMM, Arg, 0, D->OpEntry->LI);
            InsertEntry (D, X, D->OpIndex + 1);
        } else {
            X = NewCodeEntry (OP65_LDA, AM65_IMM, Arg, 0, D->OpEntry->LI);
            InsertEntry (D, X, D->OpIndex + 1);
            X = NewCodeEntry (OP65_CMP, AM65_IMM, Arg, 0, D->OpEntry->LI);
            InsertEntry (D, X, D->OpIndex + 2);
        }
    }

    /* Remove the push and the call to the operator function */
    RemoveRemainders (D);

    return 1;
}



static unsigned Opt_a_tosne (StackOpData* D)
/* Optimize the tosneax sequence */
{
    return Opt_a_toscmpbool (D, "boolne");
}



static unsigned Opt_a_tosuge (StackOpData* D)
/* Optimize the tosgeax and tosugeax sequences */
{
    return Opt_a_toscmpbool (D, "booluge");
}



static unsigned Opt_a_tosugt (StackOpData* D)
/* Optimize the tosgtax and tosugtax sequences */
{
    return Opt_a_toscmpbool (D, "boolugt");
}



static unsigned Opt_a_tosule (StackOpData* D)
/* Optimize the tosleax and tosuleax sequences */
{
    return Opt_a_toscmpbool (D, "boolule");
}



static unsigned Opt_a_tosult (StackOpData* D)
/* Optimize the tosltax and tosultax sequences */
{
    return Opt_a_toscmpbool (D, "boolult");
}



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



/* The first column of these two tables must be sorted in lexical order */

static const OptFuncDesc FuncTable[] = {
    { "___bzero",   Opt___bzero,   REG_NONE, OP_X_ZERO | OP_A_KNOWN                    },
    { "staspidx",   Opt_staspidx,  REG_NONE, OP_NONE                                   },
    { "staxspidx",  Opt_staxspidx, REG_AX,   OP_NONE                                   },
    { "tosaddax",   Opt_tosaddax,  REG_NONE, OP_NONE                                   },
    { "tosandax",   Opt_tosandax,  REG_NONE, OP_NONE                                   },
    { "tosaslax",   Opt_tosaslax,  REG_NONE, OP_NONE                                   },
    { "tosasrax",   Opt_tosasrax,  REG_NONE, OP_NONE                                   },
    { "toseqax",    Opt_toseqax,   REG_NONE, OP_LR_INTERCHANGE | OP_RHS_REMOVE_DIRECT  },
    { "tosgeax",    Opt_tosgeax,   REG_NONE, OP_RHS_REMOVE_DIRECT | OP_RHS_LOAD_DIRECT },
    { "tosltax",    Opt_tosltax,   REG_NONE, OP_RHS_REMOVE_DIRECT | OP_RHS_LOAD_DIRECT },
    { "tosneax",    Opt_tosneax,   REG_NONE, OP_LR_INTERCHANGE | OP_RHS_REMOVE_DIRECT  },
    { "tosorax",    Opt_tosorax,   REG_NONE, OP_NONE                                   },
    { "tosshlax",   Opt_tosshlax,  REG_NONE, OP_NONE                                   },
    { "tosshrax",   Opt_tosshrax,  REG_NONE, OP_NONE                                   },
    { "tossubax",   Opt_tossubax,  REG_NONE, OP_RHS_REMOVE_DIRECT | OP_RHS_LOAD_DIRECT },
    { "tosugeax",   Opt_tosugeax,  REG_NONE, OP_RHS_REMOVE_DIRECT | OP_RHS_LOAD_DIRECT },
    { "tosugtax",   Opt_tosugtax,  REG_NONE, OP_RHS_REMOVE_DIRECT | OP_RHS_LOAD_DIRECT },
    { "tosuleax",   Opt_tosuleax,  REG_NONE, OP_RHS_REMOVE_DIRECT | OP_RHS_LOAD_DIRECT },
    { "tosultax",   Opt_tosultax,  REG_NONE, OP_RHS_REMOVE_DIRECT | OP_RHS_LOAD_DIRECT },
    { "tosxorax",   Opt_tosxorax,  REG_NONE, OP_NONE                                   },
};

static const OptFuncDesc FuncRegATable[] = {
    { "toseqax",    Opt_a_toseq,   REG_NONE, OP_NONE                                   },
    { "tosgeax",    Opt_a_tosuge,  REG_NONE, OP_NONE                                   },
    { "tosgtax",    Opt_a_tosugt,  REG_NONE, OP_NONE                                   },
    { "tosicmp",    Opt_a_tosicmp, REG_NONE, OP_NONE                                   },
    { "tosleax",    Opt_a_tosule,  REG_NONE, OP_NONE                                   },
    { "tosltax",    Opt_a_tosult,  REG_NONE, OP_NONE                                   },
    { "tosneax",    Opt_a_tosne,   REG_NONE, OP_NONE                                   },
    { "tosugeax",   Opt_a_tosuge,  REG_NONE, OP_NONE                                   },
    { "tosugtax",   Opt_a_tosugt,  REG_NONE, OP_NONE                                   },
    { "tosuleax",   Opt_a_tosule,  REG_NONE, OP_NONE                                   },
    { "tosultax",   Opt_a_tosult,  REG_NONE, OP_NONE                                   },
};

#define FUNC_COUNT(Table) (sizeof(Table) / sizeof(Table[0]))



static int CmpFunc (const void* Key, const void* Func)
/* Compare function for bsearch */
{
    return strcmp (Key, ((const OptFuncDesc*) Func)->Name);
}



static const OptFuncDesc* FindFunc (const OptFuncDesc FuncTable[], size_t Count, const char* Name)
/* Find the function with the given name. Return a pointer to the table entry
** or NULL if the function was not found.
*/
{
    return bsearch (Name, FuncTable, Count, sizeof(OptFuncDesc), CmpFunc);
}



static int PreCondOk (StackOpData* D)
/* Check if the preconditions for a call to the optimizer subfunction are
** satisfied. As a side effect, this function will also choose the zero page
** register to use for temporary storage.
*/
{
    LoadInfo* Lhs;
    LoadInfo* Rhs;
    LoadRegInfo* LhsLo;
    LoadRegInfo* LhsHi;
    LoadRegInfo* RhsLo;
    LoadRegInfo* RhsHi;
    short LoVal;
    short HiVal;
    int I;
    int Passed = 0;

    /* Check the flags */
    const OptFuncDesc* Desc = D->OptFunc;
    unsigned UnusedRegs = Desc->UnusedRegs;
    if (UnusedRegs != REG_NONE &&
        (GetRegInfo (D->Code, D->OpIndex+1, UnusedRegs) & UnusedRegs) != 0) {
        /* Cannot optimize */
        return 0;
    }

    Passed = 0;
    LoVal = D->OpEntry->RI->In.RegA;
    HiVal = D->OpEntry->RI->In.RegX;
    /* Check normally first, then interchange A/X and check again if necessary */
    for (I = (Desc->Flags & OP_AX_INTERCHANGE ? 0 : 1); !Passed && I < 2; ++I) {

        do {
            if ((Desc->Flags & OP_A_KNOWN) != 0 &&
                RegValIsUnknown (LoVal)) {
                /* Cannot optimize */
                break;
            }
            if ((Desc->Flags & OP_X_ZERO) != 0 &&
                HiVal != 0) {
                /* Cannot optimize */
                break;
            }
            Passed = 1;
        } while (0);

        /* Interchange A/X */
        LoVal = D->OpEntry->RI->In.RegX;
        HiVal = D->OpEntry->RI->In.RegA;
    }
    if (!Passed) {
        /* Cannot optimize */
        return 0;
    }

    Passed = 0;
    Lhs = &D->Lhs;
    Rhs = &D->Rhs;
    /* Check normally first, then interchange LHS/RHS and check again if necessary */
    for (I = (Desc->Flags & OP_LR_INTERCHANGE ? 0 : 1); !Passed && I < 2; ++I) {

        do {
            LhsLo = &Lhs->A;
            LhsHi = &Lhs->X;
            RhsLo = &Rhs->A;
            RhsHi = &Rhs->X;
            /* Currently we have only LHS/RHS checks with identical requirements for A/X,
            ** so we don't need to check twice for now.
            */

            if ((Desc->Flags & OP_LHS_LOAD) != 0) {
                if ((LhsLo->Flags & LhsHi->Flags & LI_LOAD_INSN) == 0) {
                    /* Cannot optimize */
                    break;
                } else if ((Desc->Flags & OP_LHS_LOAD_DIRECT) != 0) {
                    if ((LhsLo->Flags & LhsHi->Flags & LI_DIRECT) == 0) {
                        /* Cannot optimize */
                        break;
                    }
                }
            }
            if ((Desc->Flags & OP_RHS_LOAD) != 0) {
                if ((RhsLo->Flags & RhsHi->Flags & LI_LOAD_INSN) == 0) {
                    /* Cannot optimize */
                    break;
                } else if ((Desc->Flags & OP_RHS_LOAD_DIRECT) != 0) {
                    if ((RhsLo->Flags & RhsHi->Flags & LI_DIRECT) == 0) {
                        /* Cannot optimize */
                        break;
                    }
                }
            }
            if ((Desc->Flags & OP_LHS_REMOVE) != 0) {
                /* Check if the load entries cannot be removed */
                if ((LhsLo->LoadEntry != 0 && (LhsLo->LoadEntry->Flags & CEF_DONT_REMOVE) != 0) ||
                    (LhsHi->LoadEntry != 0 && (LhsHi->LoadEntry->Flags & CEF_DONT_REMOVE) != 0)) {
                    if ((Desc->Flags & OP_LHS_REMOVE_DIRECT) != 0) {
                        /* Cannot optimize */
                        break;
                    }
                }
            }
            if ((Desc->Flags & OP_RHS_REMOVE) != 0) {
                if ((RhsLo->LoadEntry != 0 && (RhsLo->LoadEntry->Flags & CEF_DONT_REMOVE) != 0) ||
                    (RhsHi->LoadEntry != 0 && (RhsHi->LoadEntry->Flags & CEF_DONT_REMOVE) != 0)) {
                    if ((Desc->Flags & OP_RHS_REMOVE_DIRECT) != 0) {
                        /* Cannot optimize */
                        break;
                    }
                }
            }
            if (D->RhsMultiChg && (Desc->Flags & OP_RHS_REMOVE_DIRECT) != 0) {
                /* Cannot optimize */
                break;
            }
            Passed = 1;
        } while (0);

        /* Interchange LHS/RHS for next round */
        Lhs = &D->Rhs;
        Rhs = &D->Lhs;
    }
    if (!Passed) {
        /* Cannot optimize */
        return 0;
    }

    /* Determine the zero page locations to use. We've tracked the used
    ** ZP locations, so try to find some for us that are unused.
    */
    if ((D->ZPUsage & REG_PTR1) == REG_NONE) {
        D->ZPLo = "ptr1";
        D->ZPHi = "ptr1+1";
    } else if ((D->ZPUsage & REG_SREG) == REG_NONE) {
        D->ZPLo = "sreg";
        D->ZPHi = "sreg+1";
    } else if ((D->ZPUsage & REG_PTR2) == REG_NONE) {
        D->ZPLo = "ptr2";
        D->ZPHi = "ptr2+1";
    } else {
        /* No registers available */
        return 0;
    }

    /* Determine if we have a basic block */
    return CS_IsBasicBlock (D->Code, D->PushIndex, D->OpIndex);
}



static int RegAPreCondOk (StackOpData* D)
/* Check if the preconditions for a call to the RegA-only optimizer subfunction
** are satisfied. As a side effect, this function will also choose the zero page
** register to use for temporary storage.
*/
{
    LoadInfo* Lhs;
    LoadInfo* Rhs;
    LoadRegInfo* LhsLo;
    LoadRegInfo* RhsLo;
    short LhsLoVal, LhsHiVal;
    short RhsLoVal, RhsHiVal;
    int I;
    int Passed = 0;

    /* Check the flags */
    const OptFuncDesc* Desc = D->OptFunc;
    unsigned UnusedRegs = Desc->UnusedRegs;
    if (UnusedRegs != REG_NONE &&
        (GetRegInfo (D->Code, D->OpIndex+1, UnusedRegs) & UnusedRegs) != 0) {
        /* Cannot optimize */
        return 0;
    }

    Passed = 0;
    LhsLoVal = D->PushEntry->RI->In.RegA;
    LhsHiVal = D->PushEntry->RI->In.RegX;
    RhsLoVal = D->OpEntry->RI->In.RegA;
    RhsHiVal = D->OpEntry->RI->In.RegX;
    /* Check normally first, then interchange A/X and check again if necessary */
    for (I = (Desc->Flags & OP_AX_INTERCHANGE ? 0 : 1); !Passed && I < 2; ++I) {

        do {
            if (LhsHiVal != RhsHiVal) {
                /* Cannot optimize */
                break;
            }
            if ((Desc->Flags & OP_A_KNOWN) != 0 &&
                RegValIsUnknown (LhsLoVal)) {
                /* Cannot optimize */
                break;
            }
            if ((Desc->Flags & OP_X_ZERO) != 0 &&
                LhsHiVal != 0) {
                /* Cannot optimize */
                break;
            }
            Passed = 1;
        } while (0);

        /* Suppress warning about unused assignment in GCC */
        (void)RhsLoVal;

        /* Interchange A/X */
        LhsLoVal = D->PushEntry->RI->In.RegX;
        LhsHiVal = D->PushEntry->RI->In.RegA;
        RhsLoVal = D->OpEntry->RI->In.RegX;
        RhsHiVal = D->OpEntry->RI->In.RegA;
    }
    if (!Passed) {
        /* Cannot optimize */
        return 0;
    }

    Passed = 0;
    Lhs = &D->Lhs;
    Rhs = &D->Rhs;
    /* Check normally first, then interchange LHS/RHS and check again if necessary */
    for (I = (Desc->Flags & OP_LR_INTERCHANGE ? 0 : 1); !Passed && I < 2; ++I) {

        do {
            LhsLo = &Lhs->A;
            RhsLo = &Rhs->A;
            /* Currently we have only LHS/RHS checks with identical requirements for A/X,
            ** so we don't need to check twice for now.
            */

            if ((Desc->Flags & OP_LHS_LOAD) != 0) {
                if ((LhsLo->Flags & LI_LOAD_INSN) == 0) {
                    /* Cannot optimize */
                    break;
                } else if ((Desc->Flags & OP_LHS_LOAD_DIRECT) != 0) {
                    if ((LhsLo->Flags & LI_DIRECT) == 0) {
                        /* Cannot optimize */
                        break;
                    }
                }
            }
            if ((Desc->Flags & OP_RHS_LOAD) != 0) {
                if ((RhsLo->Flags & LI_LOAD_INSN) == 0) {
                    /* Cannot optimize */
                    break;
                } else if ((Desc->Flags & OP_RHS_LOAD_DIRECT) != 0) {
                    if ((RhsLo->Flags & LI_DIRECT) == 0) {
                        /* Cannot optimize */
                        break;
                    }
                }
            }
            if ((Desc->Flags & OP_LHS_REMOVE) != 0) {
                /* Check if the load entries cannot be removed */
                if ((LhsLo->LoadEntry != 0 && (LhsLo->LoadEntry->Flags & CEF_DONT_REMOVE) != 0)) {
                    if ((Desc->Flags & OP_LHS_REMOVE_DIRECT) != 0) {
                        /* Cannot optimize */
                        break;
                    }
                }
            }
            if ((Desc->Flags & OP_RHS_REMOVE) != 0) {
                if ((RhsLo->LoadEntry != 0 && (RhsLo->LoadEntry->Flags & CEF_DONT_REMOVE) != 0)) {
                    if ((Desc->Flags & OP_RHS_REMOVE_DIRECT) != 0) {
                        /* Cannot optimize */
                        break;
                    }
                }
            }
            if (D->RhsMultiChg && (Desc->Flags & OP_RHS_REMOVE_DIRECT) != 0) {
                /* Cannot optimize */
                break;
            }
            Passed = 1;
        } while (0);

        /* Interchange LHS/RHS for next round */
        Lhs = &D->Rhs;
        Rhs = &D->Lhs;
    }
    if (!Passed) {
        /* Cannot optimize */
        return 0;
    }

    /* Determine the zero page locations to use. We've tracked the used
    ** ZP locations, so try to find some for us that are unused.
    */
    if ((D->ZPUsage & REG_PTR1) == REG_NONE) {
        D->ZPLo = "ptr1";
        D->ZPHi = "ptr1+1";
    } else if ((D->ZPUsage & REG_SREG) == REG_NONE) {
        D->ZPLo = "sreg";
        D->ZPHi = "sreg+1";
    } else if ((D->ZPUsage & REG_PTR2) == REG_NONE) {
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
    unsigned        Changes = 0;        /* Number of changes in one run */
    StackOpData     Data;
    int             I;
    int             OldEntryCount;      /* Old number of entries */
    unsigned        Used;               /* What registers would be used */
    unsigned        PushedRegs = 0;     /* Track if the same regs are used after the push */
    int             RhsAChgIndex;       /* Track if rhs is changed more than once */
    int             RhsXChgIndex;       /* Track if rhs is changed more than once */
    int             IsRegAOptFunc = 0;  /* Whether to use the RegA-only optimizations */

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
    ** When hibytes of both oprands are equal, we may have more specialized
    ** optimization for the op.
    */
    I = 0;
    while (I < (int)CS_GetEntryCount (S)) {

        /* Get the next entry */
        CodeEntry* E = CS_GetEntry (S, I);

        /* Actions depend on state */
        switch (State) {

            case Initialize:
                ResetStackOpData (&Data);
                State = Search;
                /* FALLTHROUGH */

            case Search:
                /* While searching, track register load insns, so we can tell
                ** what is in a register once pushax is encountered.
                */
                if (CE_HasLabel (E)) {
                    /* Currently we don't track across branches.
                    ** Treat this as a change to all regs.
                    */
                    ClearLoadInfo (&Data.Lhs);
                    Data.Lhs.A.ChgIndex = I;
                    Data.Lhs.X.ChgIndex = I;
                    Data.Lhs.Y.ChgIndex = I;
                }
                if (CE_IsCallTo (E, "pushax")) {
                    /* Disallow removing Lhs loads if the registers are used */
                    SetIfOperandLoadUnremovable (&Data.Lhs, Data.UsedRegs);

                    /* The Lhs regs are also used as the default Rhs until changed */
                    PushedRegs = REG_AXY;
                    CopyLoadInfo (&Data.Rhs, &Data.Lhs);

                    Data.PushIndex = I;
                    Data.PushEntry = E;
                    State = FoundPush;
                } else {
                    /* Track load insns */
                    Used = TrackLoads (&Data.Lhs, S, I);
                    Data.UsedRegs &= ~E->Chg;
                    Data.UsedRegs |= Used;
                }
                break;

            case FoundPush:
                /* We' found a pushax before. Search for a stack op that may
                ** follow and in the meantime, track zeropage usage and check
                ** for code that will disable us from translating the sequence.
                */
                if (CE_HasLabel (E)) {
                    /* Currently we don't track across branches.
                    ** Treat this as a change to all regs.
                    */
                    ClearLoadInfo (&Data.Rhs);
                    Data.Rhs.A.ChgIndex = I;
                    Data.Rhs.X.ChgIndex = I;
                    Data.Rhs.Y.ChgIndex = I;
                }
                if (E->OPC == OP65_JSR) {
                    /* Subroutine call: Check if this is one of the functions,
                    ** we're going to replace.
                    */
                    if (SameRegXValue (&Data)) {
                        Data.OptFunc = FindFunc (FuncRegATable, FUNC_COUNT (FuncRegATable), E->Arg);
                        IsRegAOptFunc = 1;
                    }
                    if (Data.OptFunc == 0) {
                        Data.OptFunc = FindFunc (FuncTable, FUNC_COUNT (FuncTable), E->Arg);
                        IsRegAOptFunc = 0;
                    }
                    if (Data.OptFunc) {
                        /* Disallow removing Rhs loads if the registers are used */
                        SetIfOperandLoadUnremovable (&Data.Rhs, Data.UsedRegs);

                        /* Remember the op index and go on */
                        Data.OpIndex = I;
                        Data.OpEntry = E;
                        State = FoundOp;
                        break;
                    } else if (!HarmlessCall (E, 2)) {
                        /* The call might use or change the content that we are
                        ** going to access later via the stack pointer. In any
                        ** case, we need to start over after the last pushax.
                        ** Note: This will also happen if we encounter a call
                        ** to pushax!
                        */
                        I = Data.PushIndex;
                        State = Initialize;
                        break;
                    }
                } else if (((E->Chg | E->Use) & REG_SP) != 0) {

                    /* If we are using the stack, and we don't have "indirect Y"
                    ** addressing mode, or the value of Y is unknown, or less
                    ** than two, we cannot cope with this piece of code. Having
                    ** an unknown value of Y means that we cannot correct the
                    ** stack offset, while having an offset less than two means
                    ** that the code works with the value on stack which is to
                    ** be removed.
                    */
                    if (E->AM == AM65_ZPX_IND               ||
                        ((E->Chg | E->Use) & SLV_IND) == 0  ||
                        (RegValIsUnknown (E->RI->In.RegY)   ||
                         E->RI->In.RegY < 2)) {
                        I = Data.PushIndex;
                        State = Initialize;
                        break;
                    }

                }

                /* Memorize the old rhs load indices before refreshing them */
                RhsAChgIndex = Data.Rhs.A.ChgIndex;
                RhsXChgIndex = Data.Rhs.X.ChgIndex;

                /* Keep tracking Lhs src if necessary */
                SetIfOperandSrcAffected (&Data.Lhs, E);

                /* Track register usage */
                Used = TrackLoads (&Data.Rhs, S, I);

                Data.ZPUsage   |= (E->Use | E->Chg);
                /* The changes could depend on the use */
                Data.UsedRegs  &= ~E->Chg;
                Data.UsedRegs  |= Used;
                Data.ZPChanged |= E->Chg;

                /* Check if any parts of Lhs are used again before overwritten */
                if (PushedRegs != 0) {
                    if ((PushedRegs & E->Use) != 0) {
                        SetIfOperandLoadUnremovable (&Data.Lhs, PushedRegs & E->Use);
                    }
                    PushedRegs &= ~E->Chg;
                }
                /* Check if rhs is changed again after the push */
                if ((RhsAChgIndex != Data.Lhs.A.ChgIndex &&
                     RhsAChgIndex != Data.Rhs.A.ChgIndex)       ||
                    (RhsXChgIndex != Data.Lhs.X.ChgIndex &&
                     RhsXChgIndex != Data.Rhs.X.ChgIndex)) {
                    /* This will disable those sub-opts that require removing
                    ** the rhs as they can't handle such cases correctly.
                    */
                    Data.RhsMultiChg = 1;
                }
                break;

            case FoundOp:
                /* Track zero page location usage beyond this point */
                Data.ZPUsage |= GetRegInfo (S, I, REG_SREG | REG_PTR1 | REG_PTR2);

                /* Finalize the load info */
                FinalizeLoadInfo (&Data.Lhs, S);
                FinalizeLoadInfo (&Data.Rhs, S);

                /* Check if the lhs loads from zeropage. If this is true, these
                ** zero page locations have to be added to ZPUsage, because
                ** they cannot be used for intermediate storage. In addition,
                ** if one of these zero page locations is destroyed between
                ** pushing the lhs and the actual operation, we cannot use the
                ** original zero page locations for the final op, but must
                ** use another ZP location to save them.
                */
                Data.ZPChanged &= REG_ZP;
                if (Data.Lhs.A.LoadEntry && Data.Lhs.A.LoadEntry->AM == AM65_ZP) {
                    Data.ZPUsage |= Data.Lhs.A.LoadEntry->Use;
                    if ((Data.Lhs.A.LoadEntry->Use & Data.ZPChanged) != 0) {
                        Data.Lhs.A.Flags &= ~(LI_DIRECT | LI_RELOAD_Y);
                    }
                }
                if (Data.Lhs.X.LoadEntry && Data.Lhs.X.LoadEntry->AM == AM65_ZP) {
                    Data.ZPUsage |= Data.Lhs.X.LoadEntry->Use;
                    if ((Data.Lhs.X.LoadEntry->Use & Data.ZPChanged) != 0) {
                        Data.Lhs.X.Flags &= ~(LI_DIRECT | LI_RELOAD_Y);
                    }
                }

                /* Flag entries that can't be removed */
                SetDontRemoveEntryFlags (&Data);

                /* Check the preconditions. If they aren't ok, reset the insn
                ** pointer to the pushax and start over. We will lose part of
                ** load tracking but at least a/x has probably lost between
                ** pushax and here and will be tracked again when restarting.
                */
                if (IsRegAOptFunc ? !RegAPreCondOk (&Data) : !PreCondOk (&Data)) {
                    /* Unflag entries that can't be removed */
                    ResetDontRemoveEntryFlags (&Data);
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
                const OptFuncDesc* Desc = Data.OptFunc;
                Changes += Desc->Func (&Data);

                /* Unflag entries that can't be removed */
                ResetDontRemoveEntryFlags (&Data);

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
