/*****************************************************************************/
/*                                                                           */
/*                                codemisc.c                                 */
/*                                                                           */
/*                   Miscellaneous optimization operations                   */
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



#include <stdlib.h>

/* common */
#include "chartype.h"
#include "xsprintf.h"

/* cc65 */
#include "codeent.h"
#include "codeinfo.h"
#include "coptmisc.h"
#include "error.h"
#include "symtab.h"



/*****************************************************************************/
/*                            Decouple operations                            */
/*****************************************************************************/



unsigned OptDecouple (CodeSeg* S)
/* Decouple operations, that is, do the following replacements:
**
**   dex        -> ldx #imm
**   inx        -> ldx #imm
**   dey        -> ldy #imm
**   iny        -> ldy #imm
**   tax        -> ldx #imm
**   txa        -> lda #imm
**   tay        -> ldy #imm
**   tya        -> lda #imm
**   lda zp     -> lda #imm
**   ldx zp     -> ldx #imm
**   ldy zp     -> ldy #imm
**
** Provided that the register values are known of course.
*/
{
    unsigned Changes = 0;
    unsigned I;

    /* Walk over the entries */
    I = 0;
    while (I < CS_GetEntryCount (S)) {

        const char* Arg;

        /* Get next entry and it's input register values */
        CodeEntry* E = CS_GetEntry (S, I);
        const RegContents* In = &E->RI->In;

        /* Assume we have no replacement */
        CodeEntry* X = 0;

        /* Check the instruction */
        switch (E->OPC) {

            case OP65_DEA:
                if (RegValIsKnown (In->RegA)) {
                    Arg = MakeHexArg ((In->RegA - 1) & 0xFF);
                    X = NewCodeEntry (OP65_LDA, AM65_IMM, Arg, 0, E->LI);
                }
                break;

            case OP65_DEX:
                if (RegValIsKnown (In->RegX)) {
                    Arg = MakeHexArg ((In->RegX - 1) & 0xFF);
                    X = NewCodeEntry (OP65_LDX, AM65_IMM, Arg, 0, E->LI);
                }
                break;

            case OP65_DEY:
                if (RegValIsKnown (In->RegY)) {
                    Arg = MakeHexArg ((In->RegY - 1) & 0xFF);
                    X = NewCodeEntry (OP65_LDY, AM65_IMM, Arg, 0, E->LI);
                }
                break;

            case OP65_INA:
                if (RegValIsKnown (In->RegA)) {
                    Arg = MakeHexArg ((In->RegA + 1) & 0xFF);
                    X = NewCodeEntry (OP65_LDA, AM65_IMM, Arg, 0, E->LI);
                }
                break;

            case OP65_INX:
                if (RegValIsKnown (In->RegX)) {
                    Arg = MakeHexArg ((In->RegX + 1) & 0xFF);
                    X = NewCodeEntry (OP65_LDX, AM65_IMM, Arg, 0, E->LI);
                }
                break;

            case OP65_INY:
                if (RegValIsKnown (In->RegY)) {
                    Arg = MakeHexArg ((In->RegY + 1) & 0xFF);
                    X = NewCodeEntry (OP65_LDY, AM65_IMM, Arg, 0, E->LI);
                }
                break;

            case OP65_LDA:
                if (E->AM == AM65_ZP) {
                    switch (GetKnownReg (E->Use & REG_ZP, In)) {
                        case REG_TMP1:
                            Arg = MakeHexArg (In->Tmp1);
                            X = NewCodeEntry (OP65_LDA, AM65_IMM, Arg, 0, E->LI);
                            break;

                        case REG_PTR1_LO:
                            Arg = MakeHexArg (In->Ptr1Lo);
                            X = NewCodeEntry (OP65_LDA, AM65_IMM, Arg, 0, E->LI);
                            break;

                        case REG_PTR1_HI:
                            Arg = MakeHexArg (In->Ptr1Hi);
                            X = NewCodeEntry (OP65_LDA, AM65_IMM, Arg, 0, E->LI);
                            break;

                        case REG_SREG_LO:
                            Arg = MakeHexArg (In->SRegLo);
                            X = NewCodeEntry (OP65_LDA, AM65_IMM, Arg, 0, E->LI);
                            break;

                        case REG_SREG_HI:
                            Arg = MakeHexArg (In->SRegHi);
                            X = NewCodeEntry (OP65_LDA, AM65_IMM, Arg, 0, E->LI);
                            break;
                    }
                }
                break;

            case OP65_LDX:
                if (E->AM == AM65_ZP) {
                    switch (GetKnownReg (E->Use & REG_ZP, In)) {
                        case REG_TMP1:
                            Arg = MakeHexArg (In->Tmp1);
                            X = NewCodeEntry (OP65_LDX, AM65_IMM, Arg, 0, E->LI);
                            break;

                        case REG_PTR1_LO:
                            Arg = MakeHexArg (In->Ptr1Lo);
                            X = NewCodeEntry (OP65_LDX, AM65_IMM, Arg, 0, E->LI);
                            break;

                        case REG_PTR1_HI:
                            Arg = MakeHexArg (In->Ptr1Hi);
                            X = NewCodeEntry (OP65_LDX, AM65_IMM, Arg, 0, E->LI);
                            break;

                        case REG_SREG_LO:
                            Arg = MakeHexArg (In->SRegLo);
                            X = NewCodeEntry (OP65_LDX, AM65_IMM, Arg, 0, E->LI);
                            break;

                        case REG_SREG_HI:
                            Arg = MakeHexArg (In->SRegHi);
                            X = NewCodeEntry (OP65_LDX, AM65_IMM, Arg, 0, E->LI);
                            break;
                    }
                }
                break;

            case OP65_LDY:
                if (E->AM == AM65_ZP) {
                    switch (GetKnownReg (E->Use, In)) {
                        case REG_TMP1:
                            Arg = MakeHexArg (In->Tmp1);
                            X = NewCodeEntry (OP65_LDY, AM65_IMM, Arg, 0, E->LI);
                            break;

                        case REG_PTR1_LO:
                            Arg = MakeHexArg (In->Ptr1Lo);
                            X = NewCodeEntry (OP65_LDY, AM65_IMM, Arg, 0, E->LI);
                            break;

                        case REG_PTR1_HI:
                            Arg = MakeHexArg (In->Ptr1Hi);
                            X = NewCodeEntry (OP65_LDY, AM65_IMM, Arg, 0, E->LI);
                            break;

                        case REG_SREG_LO:
                            Arg = MakeHexArg (In->SRegLo);
                            X = NewCodeEntry (OP65_LDY, AM65_IMM, Arg, 0, E->LI);
                            break;

                        case REG_SREG_HI:
                            Arg = MakeHexArg (In->SRegHi);
                            X = NewCodeEntry (OP65_LDY, AM65_IMM, Arg, 0, E->LI);
                            break;
                    }
                }
                break;

            case OP65_TAX:
                if (E->RI->In.RegA >= 0) {
                    Arg = MakeHexArg (In->RegA);
                    X = NewCodeEntry (OP65_LDX, AM65_IMM, Arg, 0, E->LI);
                }
                break;

            case OP65_TAY:
                if (E->RI->In.RegA >= 0) {
                    Arg = MakeHexArg (In->RegA);
                    X = NewCodeEntry (OP65_LDY, AM65_IMM, Arg, 0, E->LI);
                }
                break;

            case OP65_TXA:
                if (E->RI->In.RegX >= 0) {
                    Arg = MakeHexArg (In->RegX);
                    X = NewCodeEntry (OP65_LDA, AM65_IMM, Arg, 0, E->LI);
                }
                break;

            case OP65_TYA:
                if (E->RI->In.RegY >= 0) {
                    Arg = MakeHexArg (In->RegY);
                    X = NewCodeEntry (OP65_LDA, AM65_IMM, Arg, 0, E->LI);
                }
                break;

            default:
                /* Avoid gcc warnings */
                break;

        }

        /* Insert the replacement if we have one */
        if (X) {
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



/*****************************************************************************/
/*                        Optimize stack pointer ops                         */
/*****************************************************************************/



static unsigned IsDecSP (const CodeEntry* E)
/* Check if this is an insn that decrements the stack pointer. If so, return
** the decrement. If not, return zero.
** The function expects E to be a subroutine call.
*/
{
    if (strncmp (E->Arg, "decsp", 5) == 0) {
        if (E->Arg[5] >= '1' && E->Arg[5] <= '8') {
            return (E->Arg[5] - '0');
        }
    } else if (strcmp (E->Arg, "subysp") == 0 && RegValIsKnown (E->RI->In.RegY)) {
        return E->RI->In.RegY;
    }

    /* If we come here, it's not a decsp op */
    return 0;
}



unsigned OptStackPtrOps (CodeSeg* S)
/* Merge adjacent calls to decsp into one. NOTE: This function won't merge all
** known cases!
*/
{
    unsigned Changes = 0;
    unsigned I;

    /* Walk over the entries */
    I = 0;
    while (I < CS_GetEntryCount (S)) {

        unsigned Dec1;
        unsigned Dec2;
        const CodeEntry* N;

        /* Get the next entry */
        const CodeEntry* E = CS_GetEntry (S, I);

        /* Check for decspn or subysp */
        if (E->OPC == OP65_JSR                          &&
            (Dec1 = IsDecSP (E)) > 0                    &&
            (N = CS_GetNextEntry (S, I)) != 0           &&
            (Dec2 = IsDecSP (N)) > 0                    &&
            (Dec1 += Dec2) <= 255                       &&
            !CE_HasLabel (N)) {

            CodeEntry* X;
            char Buf[20];

            /* We can combine the two */
            if (Dec1 <= 8) {
                /* Insert a call to decsp */
                xsprintf (Buf, sizeof (Buf), "decsp%u", Dec1);
                X = NewCodeEntry (OP65_JSR, AM65_ABS, Buf, 0, N->LI);
                CS_InsertEntry (S, X, I+2);
            } else {
                /* Insert a call to subysp */
                const char* Arg = MakeHexArg (Dec1);
                X = NewCodeEntry (OP65_LDY, AM65_IMM, Arg, 0, N->LI);
                CS_InsertEntry (S, X, I+2);
                X = NewCodeEntry (OP65_JSR, AM65_ABS, "subysp", 0, N->LI);
                CS_InsertEntry (S, X, I+3);
            }

            /* Delete the old code */
            CS_DelEntries (S, I, 2);

            /* Regenerate register info */
            CS_GenRegInfo (S);

            /* Remember we had changes */
            ++Changes;

        } else {

            /* Next entry */
            ++I;
        }

    }

    /* Return the number of changes made */
    return Changes;
}



unsigned OptGotoSPAdj (CodeSeg* S)
/* Optimize SP adjustment for forward 'goto' */
{
    unsigned Changes = 0;
    unsigned I;

    /* Walk over the entries */
    I = 0;
    while (I < CS_GetEntryCount (S)) {

        CodeEntry* L[10], *X;
        unsigned short adjustment;
        const char* Arg;

        /* Get next entry */
        L[0] = CS_GetEntry (S, I);

        /* Check for the sequence generated by g_lateadjustSP */
        if (L[0]->OPC == OP65_PHA            &&
            CS_GetEntries (S, L+1, I+1, 9)   &&
            L[1]->OPC == OP65_LDA            &&
            L[1]->AM == AM65_ABS             &&
            L[2]->OPC == OP65_CLC            &&
            L[3]->OPC == OP65_ADC            &&
            strcmp (L[3]->Arg, "sp") == 0    &&
            L[6]->OPC == OP65_ADC            &&
            strcmp (L[6]->Arg, "sp+1") == 0  &&
            L[9]->OPC == OP65_JMP) {
            adjustment = FindSPAdjustment (L[1]->Arg);

            if (adjustment == 0) {
                /* No SP adjustment needed, remove the whole sequence */
                CS_DelEntries (S, I, 9);
            }
            else if (adjustment >= 65536 - 8) {
                /* If adjustment is in range [-8, 0) we use decsp* calls */
                char Buf[20];
                adjustment = 65536 - adjustment;
                xsprintf (Buf, sizeof (Buf), "decsp%u", adjustment);
                X = NewCodeEntry (OP65_JSR, AM65_ABS, Buf, 0, L[1]->LI);
                CS_InsertEntry (S, X, I + 9);

                /* Delete the old code */
                CS_DelEntries (S, I, 9);
            }
            else if (adjustment >= 65536 - 255) {
                /* For range [-255, -8) we have ldy #, jsr subysp */
                adjustment = 65536 - adjustment;
                Arg = MakeHexArg (adjustment);
                X = NewCodeEntry (OP65_LDY, AM65_IMM, Arg, 0, L[1]->LI);
                CS_InsertEntry (S, X, I + 9);
                X = NewCodeEntry (OP65_JSR, AM65_ABS, "subysp", 0, L[1]->LI);
                CS_InsertEntry (S, X, I + 10);

                /* Delete the old code */
                CS_DelEntries (S, I, 9);
            }
            else if (adjustment > 255) {
                /* For ranges [-32768, 255) and (255, 32767) the only modification
                ** is to replace the absolute with immediate addressing
                */
                Arg = MakeHexArg (adjustment & 0xff);
                X = NewCodeEntry (OP65_LDA, AM65_IMM, Arg, 0, L[1]->LI);
                CS_InsertEntry (S, X, I + 1);
                Arg = MakeHexArg (adjustment >> 8);
                X = NewCodeEntry (OP65_LDA, AM65_IMM, Arg, 0, L[5]->LI);
                CS_InsertEntry (S, X, I + 6);

                /* Delete the old code */
                CS_DelEntry (S, I + 2);
                CS_DelEntry (S, I + 6);
            }
            else if (adjustment > 8) {
                /* For range (8, 255] we have ldy #, jsr addysp */
                Arg = MakeHexArg (adjustment & 0xff);
                X = NewCodeEntry (OP65_LDY, AM65_IMM, Arg, 0, L[1]->LI);
                CS_InsertEntry (S, X, I + 9);
                X = NewCodeEntry (OP65_JSR, AM65_ABS, "addysp", 0, L[1]->LI);
                CS_InsertEntry (S, X, I + 10);

                /* Delete the old code */
                CS_DelEntries (S, I, 9);
            }
            else {
                /* If adjustment is in range (0, 8] we use incsp* calls */
                char Buf[20];
                xsprintf (Buf, sizeof (Buf), "incsp%u", adjustment);
                X = NewCodeEntry (OP65_JSR, AM65_ABS, Buf, 0, L[1]->LI);
                CS_InsertEntry (S, X, I + 9);

                /* Delete the old code */
                CS_DelEntries (S, I, 9);
            }
            /* Regenerate register info */
            CS_GenRegInfo (S);

            /* Remember we had changes */
            Changes++;

        } else {

            /* Next entry */
            ++I;
        }

    }

    /* Return the number of changes made */
    return Changes;
}



/*****************************************************************************/
/*                         Optimize stack load ops                           */
/*****************************************************************************/



unsigned OptLoad1 (CodeSeg* S)
/* Search for a call to ldaxysp where X is not used later and replace it by
** a load of just the A register.
*/
{
    unsigned I;
    unsigned Changes = 0;

    /* Walk over the entries */
    I = 0;
    while (I < CS_GetEntryCount (S)) {

        CodeEntry* E;

        /* Get next entry */
        E = CS_GetEntry (S, I);

        /* Check for the sequence */
        if (CE_IsCallTo (E, "ldaxysp")          &&
            RegValIsKnown (E->RI->In.RegY)      &&
            !RegXUsed (S, I+1)) {

            CodeEntry* X;

            /* Reload the Y register */
            const char* Arg = MakeHexArg (E->RI->In.RegY - 1);
            X = NewCodeEntry (OP65_LDY, AM65_IMM, Arg, 0, E->LI);
            CS_InsertEntry (S, X, I+1);

            /* Load from stack */
            X = NewCodeEntry (OP65_LDA, AM65_ZP_INDY, "sp", 0, E->LI);
            CS_InsertEntry (S, X, I+2);

            /* Now remove the call to the subroutine */
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



unsigned OptLoad2 (CodeSeg* S)
/* Replace calls to ldaxysp by inline code */
{
    unsigned I;
    unsigned Changes = 0;

    /* Walk over the entries */
    I = 0;
    while (I < CS_GetEntryCount (S)) {

        CodeEntry* L[3];

        /* Get next entry */
        L[0] = CS_GetEntry (S, I);

        /* Check for the sequence */
        if (CE_IsCallTo (L[0], "ldaxysp")) {

            CodeEntry* X;

            /* Followed by sta abs/stx abs? */
            if (CS_GetEntries (S, L+1, I+1, 2)                  &&
                L[1]->OPC == OP65_STA                           &&
                L[2]->OPC == OP65_STX                           &&
                (L[1]->Arg == 0                         ||
                 L[2]->Arg == 0                         ||
                 strcmp (L[1]->Arg, L[2]->Arg) != 0)            &&
                !CS_RangeHasLabel (S, I+1, 2)                   &&
                !RegXUsed (S, I+3)) {

                /* A/X are stored into memory somewhere and X is not used
                ** later
                */

                /* lda (sp),y */
                X = NewCodeEntry (OP65_LDA, AM65_ZP_INDY, "sp", 0, L[0]->LI);
                CS_InsertEntry (S, X, I+3);

                /* sta abs */
                X = NewCodeEntry (OP65_STA, L[2]->AM, L[2]->Arg, 0, L[2]->LI);
                CS_InsertEntry (S, X, I+4);

                /* dey */
                X = NewCodeEntry (OP65_DEY, AM65_IMP, 0, 0, L[0]->LI);
                CS_InsertEntry (S, X, I+5);

                /* lda (sp),y */
                X = NewCodeEntry (OP65_LDA, AM65_ZP_INDY, "sp", 0, L[0]->LI);
                CS_InsertEntry (S, X, I+6);

                /* sta abs */
                X = NewCodeEntry (OP65_STA, L[1]->AM, L[1]->Arg, 0, L[1]->LI);
                CS_InsertEntry (S, X, I+7);

                /* Now remove the call to the subroutine and the sta/stx */
                CS_DelEntries (S, I, 3);

            } else {

                /* Standard replacement */

                /* lda (sp),y */
                X = NewCodeEntry (OP65_LDA, AM65_ZP_INDY, "sp", 0, L[0]->LI);
                CS_InsertEntry (S, X, I+1);

                /* tax */
                X = NewCodeEntry (OP65_TAX, AM65_IMP, 0, 0, L[0]->LI);
                CS_InsertEntry (S, X, I+2);

                /* dey */
                X = NewCodeEntry (OP65_DEY, AM65_IMP, 0, 0, L[0]->LI);
                CS_InsertEntry (S, X, I+3);

                /* lda (sp),y */
                X = NewCodeEntry (OP65_LDA, AM65_ZP_INDY, "sp", 0, L[0]->LI);
                CS_InsertEntry (S, X, I+4);

                /* Now remove the call to the subroutine */
                CS_DelEntry (S, I);

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
