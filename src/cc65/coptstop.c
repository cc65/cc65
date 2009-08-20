/*****************************************************************************/
/*                                                                           */
/*				   coptstop.c				     */
/*                                                                           */
/*	     Optimize operations that take operands via the stack            */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2001-2009 Ullrich von Bassewitz                                       */
/*               Roemerstrasse 52                                            */
/*               D-70794 Filderstadt                                         */
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

/* cc65 */
#include "codeent.h"
#include "codeinfo.h"
#include "coptstop.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Flags for the functions */
typedef enum {
    STOP_NONE       = 0x00,     /* Nothing special */
    STOP_A_KNOWN    = 0x01,     /* Call only if A is known */
    STOP_X_ZERO     = 0x02      /* Call only if X is zero */
} STOP_FLAGS;

/* Structure forward decl */
typedef struct StackOpData StackOpData;

/* Structure that describes an optimizer subfunction for a specific op */
typedef unsigned (*OptFunc) (StackOpData* D);
typedef struct OptFuncDesc OptFuncDesc;
struct OptFuncDesc {
    const char*         Name;           /* Name of the replaced runtime function */
    OptFunc             Func;           /* Function pointer */
    unsigned            UnusedRegs;     /* Regs that must not be used later */
    STOP_FLAGS          Flags;          /* Flags */
};

/* LoadData flags set by DirectOp */
#define LD_DIRECT       0x01            /* Direct op may be used */
#define LD_RELOAD_Y     0x02            /* Reload index register Y */
#define LD_REMOVE       0x04            /* Load may be removed */

/* Structure that tells us how to load the lhs values */
typedef struct LoadData LoadData;
struct LoadData {
    unsigned char       Flags;          /* Tells us how to load */
    unsigned char       Offs;           /* Stack offset if data is on stack */
};

/* Structure that holds the needed data */
struct StackOpData {
    CodeSeg*            Code;           /* Pointer to code segment */
    unsigned            Flags;          /* Flags to remember things */

    /* Pointer to optimizer subfunction description */
    const OptFuncDesc*  OptFunc;

    /* ZP register usage inside the sequence */
    unsigned            UsedRegs;

    /* Several indices of insns in the code segment */
    int                 LoadAIndex;     /* Index of load insns, -1 = invalid */
    int                 LoadXIndex;
    int                 LoadYIndex;
    int                 PushIndex;      /* Index of call to pushax in codeseg */
    int                 OpIndex;        /* Index of actual operation */

    /* Pointers to insns in the code segment */
    CodeEntry*          LoadAEntry;     /* Entry that loads A or NULL */
    CodeEntry*          LoadXEntry;     /* Entry that loads X or NULL */
    CodeEntry*          PrevEntry;      /* Entry before the call to pushax */
    CodeEntry*          PushEntry;      /* Pointer to entry with call to pushax */
    CodeEntry*          OpEntry;        /* Pointer to entry with op */
    CodeEntry*          NextEntry;      /* Entry after the op */

    /* Stack offsets if the lhs is loaded from stack */
    LoadData            AData;
    LoadData            XData;


    const char*         ZPLo;           /* Lo byte of zero page loc to use */
    const char*         ZPHi;           /* Hi byte of zero page loc to use */
    unsigned            IP;             /* Insertion point used by some routines */
};



/*****************************************************************************/
/*     	   	      	       	    Helpers                                  */
/*****************************************************************************/



static void AdjustStackOffset (StackOpData* D, unsigned Offs)
/* Adjust the offset for all stack accesses in the range PushIndex to OpIndex.
 * OpIndex is adjusted according to the insertions.
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
     	     * value.
     	     */
     	    CodeEntry* P = CS_GetPrevEntry (D->Code, I);
     	    if (P && P->OPC == OP65_LDY && CE_IsConstImm (P)) {

     	      	/* The Y load is just before the stack access, adjust it */
     	      	CE_SetNumArg (P, P->Num - Offs);

     	    } else {

     	      	/* Insert a new load instruction before the stack access */
     	      	const char* Arg = MakeHexArg (E->RI->In.RegY - Offs);
     	      	CodeEntry* X = NewCodeEntry (OP65_LDY, AM65_IMM, Arg, 0, E->LI);
     	   	CS_InsertEntry (D->Code, X, I++);

     	   	/* One more inserted entries */
     	   	++D->OpIndex;

     	    }

            /* If we need the value of Y later, be sure to reload it */
            if (RegYUsed (D->Code, I+1)) {
     	    	const char* Arg = MakeHexArg (E->RI->In.RegY);
     	   	CodeEntry* X = NewCodeEntry (OP65_LDY, AM65_IMM, Arg, 0, E->LI);
     	   	CS_InsertEntry (D->Code, X, I+1);

     	   	/* One more inserted entries */
     	   	++D->OpIndex;

     	   	/* Skip this instruction in the next round */
     	   	++I;
            }
     	}

     	/* Next entry */
     	++I;
    }
}



static void InsertEntry (StackOpData* D, CodeEntry* E, int Index)
/* Insert a new entry. Depending on Index, D->PushIndex and D->OpIndex will
 * be adjusted by this function.
 */
{
    /* Insert the entry into the code segment */
    CS_InsertEntry (D->Code, E, Index);

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
 * adjusted by this function, and PushEntry/OpEntry may get invalidated.
 */
{
    /* Delete the entry from the code segment */
    CS_DelEntry (D->Code, Index);

    /* Adjust the indices if necessary */
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



static void CheckOneDirectOp (CodeEntry* E, LoadData* L, unsigned char Offs)
/* Check if the given entry is a lda instruction with an addressing mode
 * that allows us to replace it by another operation (like ora). If so, we may
 * use this location for the or and must not save the value in the zero
 * page location.
 */
{
    /* Check the load entry */
    if (E) {
        /* Must check the call first since addressing mode is ABS, so second
         * "if" will catch otherwise.
         */
        if (CE_IsCallTo (E, "ldaxysp")) {
            /* Same as single loads from stack. Since we must distinguish
             * between A and X here, the necessary offset is passed to the
             * function as a parameter.
             */
            L->Offs = (unsigned char) E->RI->In.RegY - Offs;
            L->Flags |= (LD_DIRECT | LD_RELOAD_Y);
        } else if (E->AM == AM65_IMM || E->AM == AM65_ZP || E->AM == AM65_ABS) {
            /* These insns are all ok and replaceable */
            L->Flags |= LD_DIRECT;
        } else if (E->AM == AM65_ZP_INDY &&
                   RegValIsKnown (E->RI->In.RegY) &&
                   strcmp (E->Arg, "sp") == 0) {
            /* A load from the stack with known offset is also ok, but in this
             * case we must reload the index register later. Please note that
             * a load indirect via other zero page locations is not ok, since
             * these locations may change between the push and the actual
             * operation.
             */
            L->Offs  = (unsigned char) E->RI->In.RegY;
            L->Flags |= (LD_DIRECT | LD_RELOAD_Y);
        }
    }
}



static void CheckDirectOp (StackOpData* D)
/* Check if the given entry is a lda instruction with an addressing mode
 * that allows us to replace it by another operation (like ora). If so, we may
 * use this location for the or and must not save the value in the zero
 * page location.
 */
{
    /* Check flags for A and X load instructions */
    CheckOneDirectOp (D->LoadAEntry, &D->AData, 1);
    CheckOneDirectOp (D->LoadXEntry, &D->XData, 0);
}



static void ReplacePushByStore (StackOpData* D)
/* Replace the call to the push subroutine by a store into the zero page
 * location (actually, the push is not replaced, because we need it for
 * later, but the name is still ok since the push will get removed at the
 * end of each routine).
 */
{
    CodeEntry* X;

    /* Store the value into the zeropage instead of pushing it. Check high
     * byte first so that the store is later in A/X order.
     */
    if ((D->XData.Flags & LD_DIRECT) == 0) {
        X = NewCodeEntry (OP65_STX, AM65_ZP, D->ZPHi, 0, D->PushEntry->LI);
        InsertEntry (D, X, D->PushIndex+1);
    }
    if ((D->AData.Flags & LD_DIRECT) == 0) {
     	X = NewCodeEntry (OP65_STA, AM65_ZP, D->ZPLo, 0, D->PushEntry->LI);
       	InsertEntry (D, X, D->PushIndex+1);
    }
}



static void AddOpLow (StackOpData* D, opc_t OPC)
/* Add an op for the low byte of an operator. This function honours the
 * OP_DIRECT and OP_RELOAD_Y flags and generates the necessary instructions.
 * All code is inserted at the current insertion point.
 */
{
    CodeEntry* X;

    if ((D->AData.Flags & LD_DIRECT) != 0) {
       	/* Op with a variable location. If the location is on the stack, we
         * need to reload the Y register.
         */
        if ((D->AData.Flags & LD_RELOAD_Y) == 0) {

            /* opc ... */
            CodeEntry* LoadA = D->LoadAEntry;
            X = NewCodeEntry (OPC, LoadA->AM, LoadA->Arg, 0, D->OpEntry->LI);
            InsertEntry (D, X, D->IP++);

        } else {

            /* ldy #offs */
            const char* Arg = MakeHexArg (D->AData.Offs);
            X = NewCodeEntry (OP65_LDY, AM65_IMM, Arg, 0, D->OpEntry->LI);
            InsertEntry (D, X, D->IP++);

            /* opc (sp),y */
            X = NewCodeEntry (OPC, AM65_ZP_INDY, "sp", 0, D->OpEntry->LI);
            InsertEntry (D, X, D->IP++);

        }

        /* In both cases, we can remove the load */
        D->AData.Flags |= LD_REMOVE;

    } else {

   	/* Op with temp storage */
   	X = NewCodeEntry (OPC, AM65_ZP, D->ZPLo, 0, D->OpEntry->LI);
        InsertEntry (D, X, D->IP++);

    }
}



static void AddOpHigh (StackOpData* D, opc_t OPC)
/* Add an op for the high byte of an operator. Special cases (constant values
 * or similar) have to be checked separately, the function covers only the
 * generic case. Code is inserted at the insertion point.
 */
{
    CodeEntry* X;

    /* pha */
    X = NewCodeEntry (OP65_PHA, AM65_IMP, 0, 0, D->OpEntry->LI);
    InsertEntry (D, X, D->IP++);

    /* txa */
    X = NewCodeEntry (OP65_TXA, AM65_IMP, 0, 0, D->OpEntry->LI);
    InsertEntry (D, X, D->IP++);

    if ((D->XData.Flags & LD_DIRECT) != 0) {

        if ((D->XData.Flags & LD_RELOAD_Y) == 0) {

            /* opc xxx */
            CodeEntry* LoadX = D->LoadXEntry;
   	    X = NewCodeEntry (OPC, LoadX->AM, LoadX->Arg, 0, D->OpEntry->LI);
            InsertEntry (D, X, D->IP++);

        } else {

            /* ldy #const */
            const char* Arg = MakeHexArg (D->XData.Offs);
            X = NewCodeEntry (OP65_LDY, AM65_IMM, Arg, 0, D->OpEntry->LI);
            InsertEntry (D, X, D->IP++);

            /* opc (sp),y */
            X = NewCodeEntry (OPC, AM65_ZP_INDY, "sp", 0, D->OpEntry->LI);
            InsertEntry (D, X, D->IP++);
        }

        /* In both cases, we can remove the load */
        D->XData.Flags |= LD_REMOVE;

    } else {
        /* opc zphi */
        X = NewCodeEntry (OPC, AM65_ZP, D->ZPHi, 0, D->OpEntry->LI);
        InsertEntry (D, X, D->IP++);
    }

    /* tax */
    X = NewCodeEntry (OP65_TAX, AM65_IMP, 0, 0, D->OpEntry->LI);
    InsertEntry (D, X, D->IP++);

    /* pla */
    X = NewCodeEntry (OP65_PLA, AM65_IMP, 0, 0, D->OpEntry->LI);
    InsertEntry (D, X, D->IP++);
}



static void RemoveRemainders (StackOpData* D)
/* Remove the code that is unnecessary after translation of the sequence */
{
    /* Remove the push and the operator routine */
    DelEntry (D, D->OpIndex);
    DelEntry (D, D->PushIndex);

    /* Remove the register loads before the push. Beware: There may only be
     * one!
     */
    if (D->LoadAIndex >= 0 && D->LoadAIndex == D->LoadXIndex) {
        /* Common load routine */
        if ((D->AData.Flags & D->XData.Flags) & LD_REMOVE) {
            /* Both say: remove */
            DelEntry (D, D->LoadAIndex);
        }
    } else if (D->LoadAIndex >= 0 && (D->AData.Flags & LD_REMOVE)) {
        DelEntry (D, D->LoadAIndex);
    } else if (D->LoadXIndex >= 0 && (D->XData.Flags & LD_REMOVE)) {
        DelEntry (D, D->LoadXIndex);
    }
}



static int IsRegVar (StackOpData* D)
/* If the value pushed is that of a zeropage variable, replace ZPLo and ZPHi
 * in the given StackOpData struct by the variable and return true. Otherwise
 * leave D untouched and return false.
 */
{
    CodeEntry*  LoadA = D->LoadAEntry;
    CodeEntry*  LoadX = D->LoadXEntry;
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
/*   	       		 Actual optimization functions                       */
/*****************************************************************************/



static unsigned Opt___bzero (StackOpData* D)
/* Optimize the __bzero sequence if possible */
{
    CodeEntry*  X;
    const char* Arg;
    CodeLabel*  L;

    /* Check if we're using a register variable */
    if (!IsRegVar (D)) {
        /* Store the value into the zeropage instead of pushing it */
        ReplacePushByStore (D);
    }

    /* If the return value of __bzero is used, we have to add code to reload
     * a/x from the pointer variable.
     */
    if (RegAXUsed (D->Code, D->OpIndex+1)) {
        X = NewCodeEntry (OP65_LDA, AM65_ZP, D->ZPLo, 0, D->OpEntry->LI);
        InsertEntry (D, X, D->OpIndex+1);
        X = NewCodeEntry (OP65_LDX, AM65_ZP, D->ZPHi, 0, D->OpEntry->LI);
        InsertEntry (D, X, D->OpIndex+2);
    }

    /* X is always zero, A contains the size of the data area to zero.
     * Note: A may be zero, in which case the operation is null op.
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
/* Optimize the staspidx sequence if possible */
{
    CodeEntry* X;

    /* Check if we're using a register variable */
    if (!IsRegVar (D)) {
        /* Store the value into the zeropage instead of pushing it */
        ReplacePushByStore (D);
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
/* Optimize the staxspidx sequence if possible */
{
    CodeEntry* X;

    /* Check if we're using a register variable */
    if (!IsRegVar (D)) {
        /* Store the value into the zeropage instead of pushing it */
        ReplacePushByStore (D);
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
     * function would return.
     */
    X = NewCodeEntry (OP65_LDY, AM65_IMM, "$00", 0, D->OpEntry->LI);
    InsertEntry (D, X, D->OpIndex+5);

    /* Remove the push and the call to the staxspidx function */
    RemoveRemainders (D);

    /* We changed the sequence */
    return 1;
}



static unsigned Opt_tosaddax (StackOpData* D)
/* Optimize the tosaddax sequence if possible */
{
    CodeEntry*  X;
    CodeEntry*  N;

    /* We need the entry behind the add */
    CHECK (D->NextEntry != 0);

    /* Check if the X register is known and zero when the add is done, and
     * if the add is followed by
     *
     *  ldy     #$00
     *  jsr     ldauidx         ; or ldaidx
     *
     * If this is true, the addition does actually add an offset to a pointer
     * before it is dereferenced. Since both subroutines take an offset in Y,
     * we can pass the offset (instead of #$00) and remove the addition
     * alltogether.
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
        ReplacePushByStore (D);

        /* Replace the ldy by a tay. Be sure to create the new entry before
         * deleting the ldy, since we will reference the line info from this
         * insn.
         */
        X = NewCodeEntry (OP65_TAY, AM65_IMP, 0, 0, D->NextEntry->LI);
        DelEntry (D, D->OpIndex + 1);
        InsertEntry (D, X, D->OpIndex + 1);

        /* Replace the call to ldaidx/ldauidx. Since X is already zero, and
         * the ptr is in the zero page location, we just need to load from
         * the pointer, and fix X in case of ldaidx.
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

        /* Check the entry before the push. If it's a lda instruction with an
         * addressing mode that allows us to replace it, we may use this
         * location for the op and must not save the value in the zero page
         * location.
         */
        CheckDirectOp (D);

        /* Store the value into the zeropage instead of pushing it */
        ReplacePushByStore (D);

        /* Inline the add */
        D->IP = D->OpIndex+1;

        /* clc */
        X = NewCodeEntry (OP65_CLC, AM65_IMP, 0, 0, D->OpEntry->LI);
        InsertEntry (D, X, D->IP++);

        /* Low byte */
        AddOpLow (D, OP65_ADC);

        /* High byte */
        if (D->PushEntry->RI->In.RegX == 0) {
            /* The high byte is the value in X plus the carry */
            CodeLabel* L = CS_GenLabel (D->Code, D->NextEntry);
            X = NewCodeEntry (OP65_BCC, AM65_BRA, L->Name, L, D->OpEntry->LI);
            InsertEntry (D, X, D->IP++);
            X = NewCodeEntry (OP65_INX, AM65_IMP, 0, 0, D->OpEntry->LI);
            InsertEntry (D, X, D->IP++);
        } else if (D->OpEntry->RI->In.RegX == 0) {
            /* The high byte is that of the first operand plus carry */
            CodeLabel* L;
            if (RegValIsKnown (D->PushEntry->RI->In.RegX)) {
                /* Value of first op high byte is known */
                const char* Arg = MakeHexArg (D->PushEntry->RI->In.RegX);
                X = NewCodeEntry (OP65_LDX, AM65_IMM, Arg, 0, D->OpEntry->LI);
            } else {
                /* Value of first op high byte is unknown */
                X = NewCodeEntry (OP65_LDX, AM65_ZP, D->ZPHi, 0, D->OpEntry->LI);
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
            AddOpHigh (D, OP65_ADC);
        }
    }

    /* Remove the push and the call to the tosaddax function */
    RemoveRemainders (D);

    /* We changed the sequence */
    return 1;
}



static unsigned Opt_tosandax (StackOpData* D)
/* Optimize the tosandax sequence if possible */
{
    CodeEntry*  X;

    /* Check the entry before the push. If it's a lda instruction with an
     * addressing mode that allows us to replace it, we may use this
     * location for the op and must not save the value in the zero page
     * location.
     */
    CheckDirectOp (D);

    /* Store the value into the zeropage instead of pushing it */
    ReplacePushByStore (D);

    /* Inline the and, low byte */
    D->IP = D->OpIndex + 1;
    AddOpLow (D, OP65_AND);

    /* High byte */
    if (D->PushEntry->RI->In.RegX == 0 || D->OpEntry->RI->In.RegX == 0) {
     	/* The high byte is zero */
       	X = NewCodeEntry (OP65_LDX, AM65_IMM, "$00", 0, D->OpEntry->LI);
	InsertEntry (D, X, D->IP++);
    } else {
     	/* High byte is unknown */
        AddOpHigh (D, OP65_AND);
    }

    /* Remove the push and the call to the tosandax function */
    RemoveRemainders (D);

    /* We changed the sequence */
    return 1;
}



static unsigned Opt_tosorax (StackOpData* D)
/* Optimize the tosorax sequence if possible */
{
    CodeEntry*  X;

    /* Check the entry before the push. If it's a lda instruction with an
     * addressing mode that allows us to replace it, we may use this
     * location for the op and must not save the value in the zero page
     * location.
     */
    CheckDirectOp (D);

    /* Store the value into the zeropage instead of pushing it */
    ReplacePushByStore (D);

    /* Inline the or, low byte */
    D->IP = D->OpIndex + 1;
    AddOpLow (D, OP65_ORA);

    /* High byte */
    if (RegValIsKnown (D->PushEntry->RI->In.RegX) &&
        RegValIsKnown (D->OpEntry->RI->In.RegX)) {
     	/* Both values known, precalculate the result */
        unsigned char Result = D->PushEntry->RI->In.RegX | D->OpEntry->RI->In.RegX;
        const char* Arg = MakeHexArg (Result);
        X = NewCodeEntry (OP65_LDX, AM65_IMM, Arg, 0, D->OpEntry->LI);
        InsertEntry (D, X, D->IP++);
    } else if (D->PushEntry->RI->In.RegX != 0) {
     	/* High byte is unknown */
        AddOpHigh (D, OP65_ORA);
    }

    /* Remove the push and the call to the tosorax function */
    RemoveRemainders (D);

    /* We changed the sequence */
    return 1;
}



static unsigned Opt_tossubax (StackOpData* D)
/* Optimize the tossubax sequence if possible */
{
    CodeEntry*  X;

    /* Check the load entry before the push. If it's a lda instruction with an
     * addressing mode that allows us to replace it, we may use this
     * location for the op and must not save the value in the zero page
     * location.
     */
    CheckDirectOp (D);

    /* Store the value into the zeropage instead of pushing it */
    ReplacePushByStore (D);

    /* Inline the sbc */
    D->IP = D->OpIndex+1;

    /* sec */
    X = NewCodeEntry (OP65_SEC, AM65_IMP, 0, 0, D->OpEntry->LI);
    InsertEntry (D, X, D->IP++);

    /* Low byte */
    AddOpLow (D, OP65_SBC);

    /* High byte */
    AddOpHigh (D, OP65_SBC);

    /* Remove the push and the call to the tosaddax function */
    RemoveRemainders (D);

    /* We changed the sequence */
    return 1;
}



static unsigned Opt_tosxorax (StackOpData* D)
/* Optimize the tosxorax sequence if possible */
{
    CodeEntry*  X;

    /* Check the entry before the push. If it's a lda instruction with an
     * addressing mode that allows us to replace it, we may use this
     * location for the op and must not save the value in the zero page
     * location.
     */
    CheckDirectOp (D);

    /* Store the value into the zeropage instead of pushing it */
    ReplacePushByStore (D);

    /* Inline the xor, low byte */
    D->IP = D->OpIndex + 1;
    AddOpLow (D, OP65_EOR);

    /* High byte */
    if (RegValIsKnown (D->PushEntry->RI->In.RegX) &&
        RegValIsKnown (D->OpEntry->RI->In.RegX)) {
     	/* Both values known, precalculate the result */
     	const char* Arg = MakeHexArg (D->PushEntry->RI->In.RegX ^ D->OpEntry->RI->In.RegX);
       	X = NewCodeEntry (OP65_LDX, AM65_IMM, Arg, 0, D->OpEntry->LI);
     	InsertEntry (D, X, D->IP++);
    } else if (D->PushEntry->RI->In.RegX != 0) {
     	/* High byte is unknown */
        AddOpHigh (D, OP65_EOR);
    }

    /* Remove the push and the call to the tosandax function */
    RemoveRemainders (D);

    /* We changed the sequence */
    return 1;
}



/*****************************************************************************/
/*   		      		     Code                                    */
/*****************************************************************************/



static const OptFuncDesc FuncTable[] = {
    { "__bzero",    Opt___bzero,   REG_NONE, STOP_X_ZERO | STOP_A_KNOWN   },
    { "staspidx",   Opt_staspidx,  REG_NONE, STOP_NONE                    },
    { "staxspidx",  Opt_staxspidx, REG_AX,   STOP_NONE                    },
    { "tosaddax",   Opt_tosaddax,  REG_NONE, STOP_NONE                    },
    { "tosandax",   Opt_tosandax,  REG_NONE, STOP_NONE                    },
    { "tosorax",    Opt_tosorax,   REG_NONE, STOP_NONE                    },
/*    { "tossubax",   Opt_tossubax,  REG_NONE, STOP_NONE                    }, */
    { "tosxorax",   Opt_tosxorax,  REG_NONE, STOP_NONE                    },
};
#define FUNC_COUNT (sizeof(FuncTable) / sizeof(FuncTable[0]))



static int CmpFunc (const void* Key, const void* Func)
/* Compare function for bsearch */
{
    return strcmp (Key, ((const	OptFuncDesc*) Func)->Name);
}



static const OptFuncDesc* FindFunc (const char* Name)
/* Find the function with the given name. Return a pointer to the table entry
 * or NULL if the function was not found.
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
 * the pushax/op sequence when encountered.
 */
{
    static const char* Tab[] = {
        "aslax1",
        "aslax2",
        "asrax1",
        "asrax2",
        "ldaxidx",
        "ldaxysp",
        "negax",
        "shlax1",
        "shlax2",
        "shrax1",
        "shrax2",
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
    Data->AData.Flags   = 0;
    Data->XData.Flags   = 0;
    Data->OptFunc       = 0;

    Data->LoadAIndex    = -1;
    Data->LoadXIndex    = -1;
    Data->LoadYIndex    = -1;
    Data->PushIndex     = -1;
    Data->OpIndex       = -1;

    Data->LoadAEntry    = 0;
    Data->LoadXEntry    = 0;

    Data->UsedRegs      = REG_NONE;
}



static int PreCondOk (StackOpData* D)
/* Check if the preconditions for a call to the optimizer subfunction are
 * satisfied. As a side effect, this function will also choose the zero page
 * register to use.
 */
{
    /* Check the flags */
    unsigned UnusedRegs = D->OptFunc->UnusedRegs;
    if (UnusedRegs != REG_NONE &&
        (GetRegInfo (D->Code, D->OpIndex+1, UnusedRegs) & UnusedRegs) != 0) {
        /* Cannot optimize */
        return 0;
    }
    if ((D->OptFunc->Flags & STOP_A_KNOWN) != 0 &&
        RegValIsUnknown (D->OpEntry->RI->In.RegA)) {
        /* Cannot optimize */
        return 0;
    }
    if ((D->OptFunc->Flags & STOP_X_ZERO) != 0 &&
        D->OpEntry->RI->In.RegX != 0) {
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
    } else if ((D->UsedRegs & REG_PTR1) == REG_NONE) {
        D->ZPLo = "ptr1";
        D->ZPHi = "ptr1+1";
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
/*  	      	      	     	     Code                                    */
/*****************************************************************************/



unsigned OptStackOps (CodeSeg* S)
/* Optimize operations that take operands via the stack */
{
    unsigned            Changes = 0;    /* Number of changes in one run */
    StackOpData         Data;
    unsigned            I;

    enum {
        Searching,
        FoundPush,
        FoundOp
    } State = Searching;


    /* Generate register info */
    CS_GenRegInfo (S);

    /* Clear Data */
    Data.Code = S;
    ResetStackOpData (&Data);

    /* Look for a call to pushax followed by a call to some other function
     * that takes it's first argument on the stack, and the second argument
     * in the primary register.
     * It depends on the code between the two if we can handle/transform the
     * sequence, so check this code for the following list of things:
     *
     *  - the range must be a basic block (one entry, one exit)
     *  - there may not be accesses to local variables with unknown
     *    offsets (because we have to adjust these offsets).
     *  - no subroutine calls
     *  - no jump labels
     *
     * Since we need a zero page register later, do also check the
     * intermediate code for zero page use.
     */
    I = 0;
    while (I < CS_GetEntryCount (S)) {

    	/* Get the next entry */
    	CodeEntry* E = CS_GetEntry (S, I);

        /* Actions depend on state */
        switch (State) {

            case Searching:
                /* While searching, track register load insns, so we can tell
                 * what is in a register once pushax is encountered.
                 */
                if (CE_IsCallTo (E, "pushax")) {
                    Data.PushIndex = I;
                    State = FoundPush;
                } else if (E->Info & OF_LOAD) {
                    if (E->Chg & REG_A) {
                        Data.LoadAIndex = I;
                    }
                    if (E->Chg & REG_X) {
                        Data.LoadXIndex = I;
                    }
                    if (E->Chg & REG_Y) {
                        Data.LoadYIndex = I;
                    }
                } else if (E->Info & OF_XFR) {
                    switch (E->OPC) {
                        case OP65_TAX: Data.LoadXIndex = Data.LoadAIndex; break;
                        case OP65_TAY: Data.LoadYIndex = Data.LoadAIndex; break;
                        case OP65_TXA: Data.LoadAIndex = Data.LoadXIndex; break;
                        case OP65_TYA: Data.LoadAIndex = Data.LoadYIndex; break;
                        default:                                          break;
                    }
                } else if (CE_IsCallTo (E, "ldaxysp")) {
                    /* Both registers set */
                    Data.LoadAIndex = I;
                    Data.LoadXIndex = I;
                } else {
                    if (E->Chg & REG_A) {
                        Data.LoadAIndex = -1;
                    }
                    if (E->Chg & REG_X) {
                        Data.LoadXIndex = -1;
                    }
                    if (E->Chg & REG_Y) {
                        Data.LoadYIndex = -1;
                    }
                }
                break;

            case FoundPush:
                /* We' found a pushax before. Search for a stack op that may
                 * follow and in the meantime, track zeropage usage and check
                 * for code that will disable us from translating the sequence.
                 */
                if (E->OPC == OP65_JSR) {

                    /* Subroutine call: Check if this is one of the functions,
                     * we're going to replace.
                     */
                    Data.OptFunc = FindFunc (E->Arg);
                    if (Data.OptFunc) {
                        /* Remember the op index and go on */
                        Data.OpIndex = I;
                        Data.OpEntry = E;
                        State = FoundOp;
                        break;
                    } else if (HarmlessCall (E->Arg)) {
                        /* Track zeropage register usage */
                        Data.UsedRegs |= (E->Use | E->Chg);
                    } else {
                        /* A call to an unkown subroutine: We need to start
                         * over after the last pushax. Note: This will also
                         * happen if we encounter a call to pushax!
                         */
                        I = Data.PushIndex;
                        ResetStackOpData (&Data);
                        State = Searching;
                        break;
                    }

                } else if ((E->Use & REG_SP) != 0 &&
                    (E->AM != AM65_ZP_INDY || RegValIsUnknown (E->RI->In.RegY) ||
                     E->RI->In.RegY < 2)) {

                    /* If we are using the stack, and we don't have "indirect Y"
                     * addressing mode, or the value of Y is unknown, or less
                     * than two, we cannot cope with this piece of code. Having
                     * an unknown value of Y means that we cannot correct the
                     * stack offset, while having an offset less than two means
                     * that the code works with the value on stack which is to
                     * be removed.
                     */
                    I = Data.PushIndex;
                    ResetStackOpData (&Data);
                    State = Searching;
                    break;

                } else {
                    /* Other stuff: Track zeropage register usage */
                    Data.UsedRegs |= (E->Use | E->Chg);
                }
                break;

            case FoundOp:
                /* Track zero page location usage beyond this point */
                Data.UsedRegs |= GetRegInfo (S, I, REG_SREG | REG_PTR1 | REG_PTR2);

                /* Get the entry pointers to the load insns. If these insns
                 * load from zero page, we have to include them into UsedRegs
                 * registers used.
                 */
                if (Data.LoadAIndex >= 0) {
                    Data.LoadAEntry = CS_GetEntry (S, Data.LoadAIndex);
                    if (Data.LoadAEntry->AM == AM65_ZP) {
                        Data.UsedRegs |= Data.LoadAEntry->Use;
                    }
                }
                if (Data.LoadXIndex >= 0) {
                    Data.LoadXEntry = CS_GetEntry (S, Data.LoadXIndex);
                    if (Data.LoadXEntry->AM == AM65_ZP) {
                        Data.UsedRegs |= Data.LoadXEntry->Use;
                    }
                }

                /* Check the preconditions. If they aren't ok, reset the insn
                 * pointer to the pushax and start over. We will loose part of
                 * load tracking but at least a/x has probably lost between
                 * pushax and here and will be tracked again when restarting.
                 */
                if (!PreCondOk (&Data)) {
                    I = Data.PushIndex;
                    ResetStackOpData (&Data);
                    State = Searching;
                    break;
                }

                /* Adjust stack offsets to account for the upcoming removal */
                AdjustStackOffset (&Data, 2);

                /* Prepare the remainder of the data structure. */
                Data.PrevEntry = CS_GetPrevEntry (S, Data.PushIndex);
                Data.PushEntry = CS_GetEntry (S, Data.PushIndex);
                Data.OpEntry   = CS_GetEntry (S, Data.OpIndex);
                Data.NextEntry = CS_GetNextEntry (S, Data.OpIndex);

                /* Call the optimizer function */
                Changes += Data.OptFunc->Func (&Data);

                /* Regenerate register info */
                CS_GenRegInfo (S);

                /* Done */
                ResetStackOpData (&Data);
                State = Searching;
                break;

	}

	/* Next entry */
	++I;

    }

    /* Free the register info */
    CS_FreeRegInfo (S);

    /* Return the number of changes made */
    return Changes;
}



