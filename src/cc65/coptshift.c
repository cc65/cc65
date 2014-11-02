/*****************************************************************************/
/*                                                                           */
/*                                coptshift.c                                */
/*                                                                           */
/*                              Optimize shifts                              */
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



/* common */
#include "chartype.h"

/* cc65 */
#include "codeent.h"
#include "codeinfo.h"
#include "coptshift.h"



/*****************************************************************************/
/*                                     Data                                  */
/*****************************************************************************/



/* Shift types. Shift type is in the first byte, shift count in the second */
enum {
    SHIFT_NONE          = 0x0000,

    /* Masks */
    SHIFT_MASK_COUNT    = 0x00FF,
    SHIFT_MASK_DIR      = 0x0F00,
    SHIFT_MASK_MODE     = 0xF000,       /* Arithmetic or logical */
    SHIFT_MASK_TYPE     = SHIFT_MASK_DIR | SHIFT_MASK_MODE,

    /* Shift counts */
    SHIFT_COUNT_Y       = 0x0000,       /* Count is in Y register */
    SHIFT_COUNT_1       = 0x0001,
    SHIFT_COUNT_2       = 0x0002,
    SHIFT_COUNT_3       = 0x0003,
    SHIFT_COUNT_4       = 0x0004,
    SHIFT_COUNT_5       = 0x0005,
    SHIFT_COUNT_6       = 0x0006,
    SHIFT_COUNT_7       = 0x0007,

    /* Shift directions */
    SHIFT_DIR_LEFT      = 0x0100,
    SHIFT_DIR_RIGHT     = 0x0200,

    /* Shift modes */
    SHIFT_MODE_ARITH    = 0x1000,
    SHIFT_MODE_LOGICAL  = 0x2000,

    /* Shift types */
    SHIFT_TYPE_ASL      = SHIFT_DIR_LEFT  | SHIFT_MODE_ARITH,
    SHIFT_TYPE_ASR      = SHIFT_DIR_RIGHT | SHIFT_MODE_ARITH,
    SHIFT_TYPE_LSL      = SHIFT_DIR_LEFT  | SHIFT_MODE_LOGICAL,
    SHIFT_TYPE_LSR      = SHIFT_DIR_RIGHT | SHIFT_MODE_LOGICAL,

    /* Complete specs */
    SHIFT_ASL_Y         = SHIFT_TYPE_ASL | SHIFT_COUNT_Y,
    SHIFT_ASR_Y         = SHIFT_TYPE_ASR | SHIFT_COUNT_Y,
    SHIFT_LSL_Y         = SHIFT_TYPE_LSL | SHIFT_COUNT_Y,
    SHIFT_LSR_Y         = SHIFT_TYPE_LSR | SHIFT_COUNT_Y,

    SHIFT_ASL_1         = SHIFT_TYPE_ASL | SHIFT_COUNT_1,
    SHIFT_ASR_1         = SHIFT_TYPE_ASR | SHIFT_COUNT_1,
    SHIFT_LSL_1         = SHIFT_TYPE_LSL | SHIFT_COUNT_1,
    SHIFT_LSR_1         = SHIFT_TYPE_LSR | SHIFT_COUNT_1,

    SHIFT_ASL_2         = SHIFT_TYPE_ASL | SHIFT_COUNT_2,
    SHIFT_ASR_2         = SHIFT_TYPE_ASR | SHIFT_COUNT_2,
    SHIFT_LSL_2         = SHIFT_TYPE_LSL | SHIFT_COUNT_2,
    SHIFT_LSR_2         = SHIFT_TYPE_LSR | SHIFT_COUNT_2,

    SHIFT_ASL_3         = SHIFT_TYPE_ASL | SHIFT_COUNT_3,
    SHIFT_ASR_3         = SHIFT_TYPE_ASR | SHIFT_COUNT_3,
    SHIFT_LSL_3         = SHIFT_TYPE_LSL | SHIFT_COUNT_3,
    SHIFT_LSR_3         = SHIFT_TYPE_LSR | SHIFT_COUNT_3,

    SHIFT_ASL_4         = SHIFT_TYPE_ASL | SHIFT_COUNT_4,
    SHIFT_ASR_4         = SHIFT_TYPE_ASR | SHIFT_COUNT_4,
    SHIFT_LSL_4         = SHIFT_TYPE_LSL | SHIFT_COUNT_4,
    SHIFT_LSR_4         = SHIFT_TYPE_LSR | SHIFT_COUNT_4,

    SHIFT_ASL_5         = SHIFT_TYPE_ASL | SHIFT_COUNT_5,
    SHIFT_ASR_5         = SHIFT_TYPE_ASR | SHIFT_COUNT_5,
    SHIFT_LSL_5         = SHIFT_TYPE_LSL | SHIFT_COUNT_5,
    SHIFT_LSR_5         = SHIFT_TYPE_LSR | SHIFT_COUNT_5,

    SHIFT_ASL_6         = SHIFT_TYPE_ASL | SHIFT_COUNT_6,
    SHIFT_ASR_6         = SHIFT_TYPE_ASR | SHIFT_COUNT_6,
    SHIFT_LSL_6         = SHIFT_TYPE_LSL | SHIFT_COUNT_6,
    SHIFT_LSR_6         = SHIFT_TYPE_LSR | SHIFT_COUNT_6,

    SHIFT_ASL_7         = SHIFT_TYPE_ASL | SHIFT_COUNT_7,
    SHIFT_ASR_7         = SHIFT_TYPE_ASR | SHIFT_COUNT_7,
    SHIFT_LSL_7         = SHIFT_TYPE_LSL | SHIFT_COUNT_7,
    SHIFT_LSR_7         = SHIFT_TYPE_LSR | SHIFT_COUNT_7,
};



/* Macros to extract values from a shift type */
#define SHIFT_COUNT(S)          ((S) & SHIFT_MASK_COUNT)
#define SHIFT_DIR(S)            ((S) & SHIFT_MASK_DIR)
#define SHIFT_MODE(S)           ((S) & SHIFT_MASK_MODE)
#define SHIFT_TYPE(S)           ((S) & SHIFT_MASK_TYPE)



/*****************************************************************************/
/*                              Helper routines                              */
/*****************************************************************************/



static unsigned GetShift (const char* Name)
/* Determine the shift from the name of the subroutine */
{
    unsigned Type;

    if (strncmp (Name, "aslax", 5) == 0) {
        Type = SHIFT_TYPE_ASL;
    } else if (strncmp (Name, "asrax", 5) == 0) {
        Type = SHIFT_TYPE_ASR;
    } else if (strncmp (Name, "shlax", 5) == 0) {
        Type = SHIFT_TYPE_LSL;
    } else if (strncmp (Name, "shrax", 5) == 0) {
        Type = SHIFT_TYPE_LSR;
    } else {
        /* Nothing we know */
        return SHIFT_NONE;
    }

    /* Get the count */
    switch (Name[5]) {
        case 'y':       Type |= SHIFT_COUNT_Y;  break;
        case '1':       Type |= SHIFT_COUNT_1;  break;
        case '2':       Type |= SHIFT_COUNT_2;  break;
        case '3':       Type |= SHIFT_COUNT_3;  break;
        case '4':       Type |= SHIFT_COUNT_4;  break;
        case '5':       Type |= SHIFT_COUNT_5;  break;
        case '6':       Type |= SHIFT_COUNT_6;  break;
        case '7':       Type |= SHIFT_COUNT_7;  break;
        default:        return SHIFT_NONE;
    }

    /* Make sure nothing follows */
    if (Name[6] == '\0') {
        return Type;
    } else {
        return SHIFT_NONE;
    }
}



/*****************************************************************************/
/*                              Optimize shifts                              */
/*****************************************************************************/



unsigned OptShift1 (CodeSeg* S)
/* A call to the shlaxN routine may get replaced by one or more asl insns
** if the value of X is not used later. If X is used later, but it is zero
** on entry and it's a shift by one, it may get replaced by:
**
**      asl     a
**      bcc     L1
**      inx
**  L1:
*/
{
    unsigned Changes = 0;
    unsigned I;

    /* Walk over the entries */
    I = 0;
    while (I < CS_GetEntryCount (S)) {

        unsigned   Shift;
        CodeEntry* N;
        CodeEntry* X;
        CodeLabel* L;

        /* Get next entry */
        CodeEntry* E = CS_GetEntry (S, I);

        /* Check for the sequence */
        if (E->OPC == OP65_JSR                          &&
            (Shift = GetShift (E->Arg)) != SHIFT_NONE   &&
            SHIFT_DIR (Shift) == SHIFT_DIR_LEFT) {


            unsigned Count = SHIFT_COUNT (Shift);
            if (!RegXUsed (S, I+1)) {

                if (Count == SHIFT_COUNT_Y) {

                    CodeLabel* L;

                    if (S->CodeSizeFactor < 200) {
                        goto NextEntry;
                    }

                    /* Change into
                    **
                    ** L1:  asl     a
                    **      dey
                    **      bpl     L1
                    **      ror     a
                    */

                    /* asl a */
                    X = NewCodeEntry (OP65_ASL, AM65_ACC, "a", 0, E->LI);
                    CS_InsertEntry (S, X, I+1);
                    L = CS_GenLabel (S, X);

                    /* dey */
                    X = NewCodeEntry (OP65_DEY, AM65_IMP, 0, 0, E->LI);
                    CS_InsertEntry (S, X, I+2);

                    /* bpl L1 */
                    X = NewCodeEntry (OP65_BPL, AM65_BRA, L->Name, L, E->LI);
                    CS_InsertEntry (S, X, I+3);

                    /* ror a */
                    X = NewCodeEntry (OP65_ROR, AM65_ACC, "a", 0, E->LI);
                    CS_InsertEntry (S, X, I+4);

                } else {
                    /* Insert shift insns */
                    while (Count--) {
                        X = NewCodeEntry (OP65_ASL, AM65_ACC, "a", 0, E->LI);
                        CS_InsertEntry (S, X, I+1);
                    }
                }

            } else if (E->RI->In.RegX == 0              &&
                       Count == 1                       &&
                       (N = CS_GetNextEntry (S, I)) != 0) {

                /* asl a */
                X = NewCodeEntry (OP65_ASL, AM65_ACC, "a", 0, E->LI);
                CS_InsertEntry (S, X, I+1);

                /* bcc L1 */
                L = CS_GenLabel (S, N);
                X = NewCodeEntry (OP65_BCC, AM65_BRA, L->Name, L, E->LI);
                CS_InsertEntry (S, X, I+2);

                /* inx */
                X = NewCodeEntry (OP65_INX, AM65_IMP, 0, 0, E->LI);
                CS_InsertEntry (S, X, I+3);

            } else {

                /* We won't handle this one */
                goto NextEntry;

            }

            /* Delete the call to shlax */
            CS_DelEntry (S, I);

            /* Remember, we had changes */
            ++Changes;
        }

NextEntry:
        /* Next entry */
        ++I;

    }

    /* Return the number of changes made */
    return Changes;
}



unsigned OptShift2(CodeSeg* S)
/* A call to the asrax1 routines may get replaced by something simpler, if
** X is not used later:
**
**      cmp     #$80
**      ror     a
*/
{
    unsigned Changes = 0;
    unsigned I;

    /* Walk over the entries */
    I = 0;
    while (I < CS_GetEntryCount (S)) {

        unsigned Shift;
        unsigned Count;

        /* Get next entry */
        CodeEntry* E = CS_GetEntry (S, I);

        /* Check for the sequence */
        if (E->OPC == OP65_JSR                          &&
            (Shift = GetShift (E->Arg)) != SHIFT_NONE   &&
            SHIFT_TYPE (Shift) == SHIFT_TYPE_ASR        &&
            (Count = SHIFT_COUNT (Shift)) > 0           &&
            Count * 100 <= S->CodeSizeFactor    &&
            !RegXUsed (S, I+1)) {

            CodeEntry* X;
            unsigned J = I+1;

            /* Generate the replacement sequence */
            while (Count--) {
                /* cmp #$80 */
                X = NewCodeEntry (OP65_CMP, AM65_IMM, "$80", 0, E->LI);
                CS_InsertEntry (S, X, J++);

                /* ror a */
                X = NewCodeEntry (OP65_ROR, AM65_ACC, "a", 0, E->LI);
                CS_InsertEntry (S, X, J++);
            }

            /* Delete the call to asrax */
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



unsigned OptShift3 (CodeSeg* S)
/* The sequence
**
**      bcc     L
**      inx
** L:   jsr     shrax1
**
** may get replaced by
**
**      ror     a
**
** if X is zero on entry. For shift counts > 1, more
**
**      shr     a
**
** must be added.
*/
{
    unsigned Changes = 0;
    unsigned I;

    /* Walk over the entries */
    I = 0;
    while (I < CS_GetEntryCount (S)) {

        unsigned   Shift;
        unsigned   Count;
        CodeEntry* L[3];

        /* Get next entry */
        L[0] = CS_GetEntry (S, I);

        /* Check for the sequence */
        if ((L[0]->OPC == OP65_BCC || L[0]->OPC == OP65_JCC)    &&
            L[0]->JumpTo != 0                                   &&
            L[0]->RI->In.RegX == 0                              &&
            CS_GetEntries (S, L+1, I+1, 2)                      &&
            L[1]->OPC == OP65_INX                               &&
            L[0]->JumpTo->Owner == L[2]                         &&
            !CS_RangeHasLabel (S, I, 2)                         &&
            L[2]->OPC == OP65_JSR                               &&
            (Shift = GetShift (L[2]->Arg)) != SHIFT_NONE        &&
            SHIFT_DIR (Shift) == SHIFT_DIR_RIGHT                &&
            (Count = SHIFT_COUNT (Shift)) > 0) {
                                                
            /* Add the replacement insn instead */
            CodeEntry* X = NewCodeEntry (OP65_ROR, AM65_ACC, "a", 0, L[2]->LI);
            CS_InsertEntry (S, X, I+3);
            while (--Count) {
                X = NewCodeEntry (OP65_LSR, AM65_ACC, "a", 0, L[2]->LI);
                CS_InsertEntry (S, X, I+4);
            }

            /* Remove the bcs/dex/jsr */
            CS_DelEntries (S, I, 3);

            /* Remember, we had changes */
            ++Changes;

        }

        /* Next entry */
        ++I;

    }

    /* Return the number of changes made */
    return Changes;
}



unsigned OptShift4 (CodeSeg* S)
/* Calls to the asraxN or shraxN routines may get replaced by one or more lsr
** insns if the value of X is zero.
*/
{
    unsigned Changes = 0;
    unsigned I;

    /* Walk over the entries */
    I = 0;
    while (I < CS_GetEntryCount (S)) {

        unsigned Shift;
        unsigned Count;

        /* Get next entry */
        CodeEntry* E = CS_GetEntry (S, I);

        /* Check for the sequence */
        if (E->OPC == OP65_JSR                          &&
            (Shift = GetShift (E->Arg)) != SHIFT_NONE   &&
            SHIFT_DIR (Shift) == SHIFT_DIR_RIGHT        &&
            E->RI->In.RegX == 0) {

            CodeEntry* X;

            /* Shift count may be in Y */
            Count = SHIFT_COUNT (Shift);
            if (Count == SHIFT_COUNT_Y) {

                CodeLabel* L;

                if (S->CodeSizeFactor < 200) {
                    /* Not acceptable */
                    goto NextEntry;
                }

                /* Generate:
                **
                ** L1: lsr     a
                **     dey
                **     bpl     L1
                **     rol     a
                **
                ** A negative shift count or one that is greater or equal than
                ** the bit width of the left operand (which is promoted to
                ** integer before the operation) causes undefined behaviour, so
                ** above transformation is safe.
                */

                /* lsr a */
                X = NewCodeEntry (OP65_LSR, AM65_ACC, "a", 0, E->LI);
                CS_InsertEntry (S, X, I+1);
                L = CS_GenLabel (S, X);

                /* dey */
                X = NewCodeEntry (OP65_DEY, AM65_IMP, 0, 0, E->LI);
                CS_InsertEntry (S, X, I+2);

                /* bpl L1 */
                X = NewCodeEntry (OP65_BPL, AM65_BRA, L->Name, L, E->LI);
                CS_InsertEntry (S, X, I+3);

                /* rol a */
                X = NewCodeEntry (OP65_ROL, AM65_ACC, "a", 0, E->LI);
                CS_InsertEntry (S, X, I+4);

            } else {
                /* Insert shift insns */
                while (Count--) {
                    X = NewCodeEntry (OP65_LSR, AM65_ACC, "a", 0, E->LI);
                    CS_InsertEntry (S, X, I+1);
                }

            }

            /* Delete the call to shrax */
            CS_DelEntry (S, I);

            /* Remember, we had changes */
            ++Changes;

        }

NextEntry:
        /* Next entry */
        ++I;

    }

    /* Return the number of changes made */
    return Changes;
}



unsigned OptShift5 (CodeSeg* S)
/* Search for the sequence
**
**      lda     xxx
**      ldx     yyy
**      jsr     aslax1/asrax1/shlax1/shrax1
**      sta     aaa
**      stx     bbb
**
** and replace it by
**
**      lda     xxx
**      asl     a
**      sta     aaa
**      lda     yyy
**      rol     a
**      sta     bbb
**
** or similar, provided that a/x is not used later
*/
{
    unsigned Changes = 0;

    /* Walk over the entries */
    unsigned I = 0;
    while (I < CS_GetEntryCount (S)) {

        unsigned ShiftType;
        CodeEntry* L[5];

        /* Get next entry */
        L[0] = CS_GetEntry (S, I);

        /* Check for the sequence */
        if (L[0]->OPC == OP65_LDA                               &&
            (L[0]->AM == AM65_ABS || L[0]->AM == AM65_ZP)       &&
            CS_GetEntries (S, L+1, I+1, 4)                      &&
            !CS_RangeHasLabel (S, I+1, 4)                       &&
            L[1]->OPC == OP65_LDX                               &&
            (L[1]->AM == AM65_ABS || L[1]->AM == AM65_ZP)       &&
            L[2]->OPC == OP65_JSR                               &&
            (ShiftType = GetShift (L[2]->Arg)) != SHIFT_NONE    &&
            SHIFT_COUNT(ShiftType) == 1                         &&
            L[3]->OPC == OP65_STA                               &&
            (L[3]->AM == AM65_ABS || L[3]->AM == AM65_ZP)       &&
            L[4]->OPC == OP65_STX                               &&
            (L[4]->AM == AM65_ABS || L[4]->AM == AM65_ZP)       &&
            !RegAXUsed (S, I+5)) {

            CodeEntry* X;

            /* Handle the four shift types differently */
            switch (ShiftType) {

                case SHIFT_ASR_1:
                    X = NewCodeEntry (OP65_LDA, L[1]->AM, L[1]->Arg, 0, L[1]->LI);
                    CS_InsertEntry (S, X, I+5);
                    X = NewCodeEntry (OP65_CMP, AM65_IMM, "$80", 0, L[2]->LI);
                    CS_InsertEntry (S, X, I+6);
                    X = NewCodeEntry (OP65_ROR, AM65_ACC, "a", 0, L[2]->LI);
                    CS_InsertEntry (S, X, I+7);
                    X = NewCodeEntry (OP65_STA, L[4]->AM, L[4]->Arg, 0, L[4]->LI);
                    CS_InsertEntry (S, X, I+8);
                    X = NewCodeEntry (OP65_LDA, L[0]->AM, L[0]->Arg, 0, L[0]->LI);
                    CS_InsertEntry (S, X, I+9);
                    X = NewCodeEntry (OP65_ROR, AM65_ACC, "a", 0, L[2]->LI);
                    CS_InsertEntry (S, X, I+10);
                    X = NewCodeEntry (OP65_STA, L[3]->AM, L[3]->Arg, 0, L[3]->LI);
                    CS_InsertEntry (S, X, I+11);
                    CS_DelEntries (S, I, 5);
                    break;

                case SHIFT_LSR_1:
                    X = NewCodeEntry (OP65_LDA, L[1]->AM, L[1]->Arg, 0, L[1]->LI);
                    CS_InsertEntry (S, X, I+5);
                    X = NewCodeEntry (OP65_LSR, AM65_ACC, "a", 0, L[2]->LI);
                    CS_InsertEntry (S, X, I+6);
                    X = NewCodeEntry (OP65_STA, L[4]->AM, L[4]->Arg, 0, L[4]->LI);
                    CS_InsertEntry (S, X, I+7);
                    X = NewCodeEntry (OP65_LDA, L[0]->AM, L[0]->Arg, 0, L[0]->LI);
                    CS_InsertEntry (S, X, I+8);
                    X = NewCodeEntry (OP65_ROR, AM65_ACC, "a", 0, L[2]->LI);
                    CS_InsertEntry (S, X, I+9);
                    X = NewCodeEntry (OP65_STA, L[3]->AM, L[3]->Arg, 0, L[3]->LI);
                    CS_InsertEntry (S, X, I+10);
                    CS_DelEntries (S, I, 5);
                    break;

                case SHIFT_LSL_1:
                case SHIFT_ASL_1:
                    /* These two are identical */
                    X = NewCodeEntry (OP65_ASL, AM65_ACC, "a", 0, L[2]->LI);
                    CS_InsertEntry (S, X, I+1);
                    X = NewCodeEntry (OP65_STA, L[3]->AM, L[3]->Arg, 0, L[3]->LI);
                    CS_InsertEntry (S, X, I+2);
                    X = NewCodeEntry (OP65_LDA, L[1]->AM, L[1]->Arg, 0, L[1]->LI);
                    CS_InsertEntry (S, X, I+3);
                    X = NewCodeEntry (OP65_ROL, AM65_ACC, "a", 0, L[2]->LI);
                    CS_InsertEntry (S, X, I+4);
                    X = NewCodeEntry (OP65_STA, L[4]->AM, L[4]->Arg, 0, L[4]->LI);
                    CS_InsertEntry (S, X, I+5);
                    CS_DelEntries (S, I+6, 4);
                    break;

            }

            /* Remember, we had changes */
            ++Changes;

        }

        /* Next entry */
        ++I;

    }

    /* Return the number of changes made */
    return Changes;
}



unsigned OptShift6 (CodeSeg* S)
/* Inline the shift subroutines. */
{
    unsigned Changes = 0;

    /* Walk over the entries */
    unsigned I = 0;
    while (I < CS_GetEntryCount (S)) {

        unsigned   Shift;
        unsigned   Count;
        CodeEntry* X;
        unsigned   IP;

        /* Get next entry */
        CodeEntry* E = CS_GetEntry (S, I);

        /* Check for a call to one of the shift routine */
        if (E->OPC == OP65_JSR                          &&
            (Shift = GetShift (E->Arg)) != SHIFT_NONE   &&
            SHIFT_DIR (Shift) == SHIFT_DIR_LEFT         &&
            (Count = SHIFT_COUNT (Shift)) > 0) {

            /* Code is:
            **
            **      stx     tmp1
            **      asl     a
            **      rol     tmp1
            **      (repeat ShiftCount-1 times)
            **      ldx     tmp1
            **
            ** which makes 4 + 3 * ShiftCount bytes, compared to the original
            ** 3 bytes for the subroutine call. However, in most cases, the
            ** final load of the X register gets merged with some other insn
            ** and replaces a txa, so for a shift count of 1, we get a factor
            ** of 200, which matches nicely the CodeSizeFactor enabled with -Oi
            */
            if (Count > 1 || S->CodeSizeFactor > 200) {
                unsigned Size = 4 + 3 * Count;
                if ((Size * 100 / 3) > S->CodeSizeFactor) {
                    /* Not acceptable */
                    goto NextEntry;
                }
            }

            /* Inline the code. Insertion point is behind the subroutine call */
            IP = (I + 1);

            /* stx tmp1 */
            X = NewCodeEntry (OP65_STX, AM65_ZP, "tmp1", 0, E->LI);
            CS_InsertEntry (S, X, IP++);

            while (Count--) {
                /* asl a */
                X = NewCodeEntry (OP65_ASL, AM65_ACC, "a", 0, E->LI);
                CS_InsertEntry (S, X, IP++);

                /* rol tmp1 */
                X = NewCodeEntry (OP65_ROL, AM65_ZP, "tmp1", 0, E->LI);
                CS_InsertEntry (S, X, IP++);
            }

            /* ldx tmp1 */
            X = NewCodeEntry (OP65_LDX, AM65_ZP, "tmp1", 0, E->LI);
            CS_InsertEntry (S, X, IP++);

            /* Remove the subroutine call */
            CS_DelEntry (S, I);

            /* Remember, we had changes */
            ++Changes;
        }

NextEntry:
        /* Next entry */
        ++I;

    }

    /* Return the number of changes made */
    return Changes;
}
