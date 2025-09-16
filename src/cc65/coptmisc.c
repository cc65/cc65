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



static signed IsShift (const CodeEntry* E, const char* dec, const char* inc, const char* sub, const char* add)
/* Check if this is an insn that increments/decrements the stack pointer or AX.
** If so, return the value (negative for dec, positive for inc). If not, return zero.
*/
{
    if (E->OPC != OP65_JSR && E->OPC != OP65_JMP) {
        return 0;
    }

    if (strncmp (E->Arg, dec, 5) == 0) {
        if (E->Arg[5] >= '1' && E->Arg[5] <= '8') {
            return -(E->Arg[5] - '0');
        }
    } else if (strcmp (E->Arg, sub) == 0 && RegValIsKnown (E->RI->In.RegY)) {
        return -(E->RI->In.RegY);
    } else if (strncmp (E->Arg, inc, 5) == 0) {
        if (E->Arg[5] >= '1' && E->Arg[5] <= '8') {
            return (E->Arg[5] - '0');
        }
    } else if (strcmp (E->Arg, add) == 0 && RegValIsKnown (E->RI->In.RegY)) {
        return (E->RI->In.RegY);
    }

    /* If we come here, it's not a dec/inc op */
    return 0;
}



static unsigned OptIncDecOps (CodeSeg* S, const char* dec, const char* inc, const char* sub, const char* add)
/* Merge adjacent calls to inc/dec/add/sub into one. NOTE: This function won't merge all
** known cases!
*/
{
    unsigned Changes = 0;
    unsigned I;

    /* Walk over the entries */
    I = 0;
    while (I < CS_GetEntryCount (S)) {

        signed Val1;
        signed Val2;
        const CodeEntry* N;

        /* Get the next entry */
        const CodeEntry* E = CS_GetEntry (S, I);

        /* Check for decspn, incspn, subysp or addysp */
        if (E->OPC == OP65_JSR                            &&
            (Val1 = IsShift (E, dec, inc, sub, add)) != 0 &&
            (N = CS_GetNextEntry (S, I)) != 0             &&
            (N->OPC == OP65_JSR || N->OPC == OP65_JMP)    &&
            (Val2 = IsShift (N, dec, inc, sub, add)) != 0 &&
            abs (Val1 += Val2) <= 255                     &&
            !CE_HasLabel (N)) {

            CodeEntry* X;
            char Buf[20];

            if (Val1 != 0) {
                /* We can combine the two */
                if (abs (Val1) <= 8) {
                    /* Insert a call to inc/dec using the last OPC */
                    xsprintf (Buf, sizeof (Buf), "%s%u", Val1 < 0 ? dec:inc, abs (Val1));
                    X = NewCodeEntry (N->OPC, AM65_ABS, Buf, 0, N->LI);
                    CS_InsertEntry (S, X, I+2);
                } else {
                    /* Insert a call to add/sub */
                    const char* Arg = MakeHexArg (abs (Val1));
                    X = NewCodeEntry (OP65_LDY, AM65_IMM, Arg, 0, N->LI);
                    CS_InsertEntry (S, X, I+2);
                    if (Val1 < 0) {
                        X = NewCodeEntry (N->OPC, AM65_ABS, sub, 0, N->LI);
                    } else {
                        X = NewCodeEntry (N->OPC, AM65_ABS, add, 0, N->LI);
                    }
                    CS_InsertEntry (S, X, I+3);
                }
            } /* If total shift == 0, just drop the old code. */

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



unsigned OptStackPtrOps (CodeSeg* S)
{
    return OptIncDecOps (S, "decsp", "incsp", "subysp", "addysp");
}



unsigned OptAXOps (CodeSeg* S)
{
    return OptIncDecOps (S, "decax", "incax", "decaxy", "incaxy");
}



unsigned OptAXLoad (CodeSeg* S)
/* Merge jsr incax[1-8]/jsr ldaxi into ldy/jsr ldaxidx */
{
    unsigned Changes = 0;
    unsigned I;

    /* Walk over the entries */
    I = 0;
    while (I < CS_GetEntryCount (S)) {

        signed Val;
        const CodeEntry* N;

        /* Get the next entry */
        const CodeEntry* E = CS_GetEntry (S, I);

        /* Check for incax[1-8] followed by jsr/jmp ldaxi */
        if (E->OPC == OP65_JSR                            &&
            strncmp (E->Arg, "incax", 5) == 0             &&
            E->Arg[5] >= '1' && E->Arg[5] <= '8'          &&
            E->Arg[6]  == '\0'                            &&
            (N = CS_GetNextEntry (S, I)) != 0             &&
            (N->OPC == OP65_JSR || N->OPC == OP65_JMP)    &&
            strcmp (N->Arg, "ldaxi") == 0                 &&
            !CE_HasLabel (N)) {

            CodeEntry* X;
            const char* Arg;

            Val = E->Arg[5] - '0';
            Arg = MakeHexArg (Val + 1);
            X = NewCodeEntry (OP65_LDY, AM65_IMM, Arg, 0, N->LI);
            CS_InsertEntry (S, X, I+2);
            X = NewCodeEntry (N->OPC, AM65_ABS, "ldaxidx", 0, N->LI);
            CS_InsertEntry (S, X, I+3);

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



unsigned OptTosLoadPop (CodeSeg* S)
/* Merge jsr ldax0sp / jsr|jmp incsp2 into jsr|jmp popax */
{
    unsigned Changes = 0;
    unsigned I;

    /* Walk over the entries */
    I = 0;
    while (I < CS_GetEntryCount (S)) {

        const CodeEntry* N;

        /* Get the next entry */
        const CodeEntry* E = CS_GetEntry (S, I);

        /* Check for the sequence */
        if (CE_IsCallTo (E, "ldax0sp")                               &&
            (N = CS_GetNextEntry (S, I)) != 0                        &&
            (CE_IsCallTo (N, "incsp2") || CE_IsJumpTo (N, "incsp2")) &&
            !CE_HasLabel (N)) {

            CodeEntry* X;

            X = NewCodeEntry (N->OPC, AM65_ABS, "popax", 0, N->LI);
            CS_InsertEntry (S, X, I+2);

            /* Delete the old code */
            CS_DelEntries (S, I, 2);

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



unsigned OptAXLoad2 (CodeSeg* S)
/* Merge ldy/jsr incaxy/jsr ldaxi into ldy/jsr ldaxidx */
{
    unsigned Changes = 0;
    unsigned I;

    /* Walk over the entries */
    I = 0;
    while (I < CS_GetEntryCount (S)) {

        signed Val;
        CodeEntry* E[3];
        CodeEntry* X;

        /* Get the next entry */
        E[0] = CS_GetEntry (S, I);

        /* Check for ldy followed by incaxy followed by jsr/jmp ldaxi */
        if (E[0]->OPC == OP65_LDY                            &&
            CE_IsConstImm (E[0])                             &&
            CS_GetEntries (S, E+1, I+1, 2)                   &&
            E[1]->OPC == OP65_JSR                            &&
            strcmp (E[1]->Arg, "incaxy") == 0                &&
            (E[2]->OPC == OP65_JSR || E[2]->OPC == OP65_JMP) &&
            strcmp (E[2]->Arg, "ldaxi") == 0                 &&
            !CS_RangeHasLabel (S, I, 3)) {

            /* Replace with ldy (y+1) / jsr ldaxidx */
            Val = E[0]->Num + 1;

            X = NewCodeEntry (OP65_LDY, AM65_IMM, MakeHexArg (Val), 0, E[0]->LI);
            CS_InsertEntry (S, X, I+3);
            X = NewCodeEntry (E[2]->OPC, AM65_ABS, "ldaxidx", 0, E[0]->LI);
            CS_InsertEntry (S, X, I+4);

            CS_DelEntries (S, I, 3);

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
            strcmp (L[3]->Arg, "c_sp") == 0    &&
            L[6]->OPC == OP65_ADC            &&
            strcmp (L[6]->Arg, "c_sp+1") == 0  &&
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
/*                       Optimize stack load/store ops                       */
/*****************************************************************************/



unsigned OptLoadStore2 (CodeSeg* S)
/* Remove 16 bit stack loads followed by a store into the same location. */
{
    unsigned Changes = 0;

    /* Walk over the entries */
    unsigned I = 0;
    while (I < CS_GetEntryCount (S)) {

        CodeEntry* N;

        /* Get next entry */
        CodeEntry* E = CS_GetEntry (S, I);

        /* Check if this is a 16 bit load followed by a store into the same
        ** address.
        */
        if (CE_IsCallTo (E, "ldaxysp")          &&  /* Stack load ... */
            RegValIsKnown (E->RI->In.RegY)      &&  /* ... with known offs */
            (N = CS_GetNextEntry (S, I)) != 0   &&  /* Next insn ... */
            !CE_HasLabel (N)                    &&  /* ... without label ... */
            N->OPC == OP65_LDY                  &&  /* ... is LDY */
            CE_IsKnownImm (N, E->RI->In.RegY-1) &&  /* Same offset as load */
            (N = CS_GetNextEntry (S, I+1)) != 0 &&  /* Next insn ... */
            !CE_HasLabel (N)                    &&  /* ... without label ... */
            CE_IsCallTo (N, "staxysp")) {           /* ... is store */

            /* Found - remove it. Leave the load in place. If it's unused, it
            ** will get removed by later steps.
            */
            CS_DelEntries (S, I+1, 2);

            /* Remember, we had changes */
            ++Changes;
        }

        /* Next entry */
        ++I;
    }

    /* Return the number of changes made */
    return Changes;
}



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
            X = NewCodeEntry (OP65_LDA, AM65_ZP_INDY, "c_sp", 0, E->LI);
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

                /* lda (c_sp),y */
                X = NewCodeEntry (OP65_LDA, AM65_ZP_INDY, "c_sp", 0, L[0]->LI);
                CS_InsertEntry (S, X, I+3);

                /* sta abs */
                X = NewCodeEntry (OP65_STA, L[2]->AM, L[2]->Arg, 0, L[2]->LI);
                CS_InsertEntry (S, X, I+4);

                /* dey */
                X = NewCodeEntry (OP65_DEY, AM65_IMP, 0, 0, L[0]->LI);
                CS_InsertEntry (S, X, I+5);

                /* lda (c_sp),y */
                X = NewCodeEntry (OP65_LDA, AM65_ZP_INDY, "c_sp", 0, L[0]->LI);
                CS_InsertEntry (S, X, I+6);

                /* sta abs */
                X = NewCodeEntry (OP65_STA, L[1]->AM, L[1]->Arg, 0, L[1]->LI);
                CS_InsertEntry (S, X, I+7);

                /* Now remove the call to the subroutine and the sta/stx */
                CS_DelEntries (S, I, 3);

            } else {

                /* Standard replacement */

                /* lda (c_sp),y */
                X = NewCodeEntry (OP65_LDA, AM65_ZP_INDY, "c_sp", 0, L[0]->LI);
                CS_InsertEntry (S, X, I+1);

                /* tax */
                X = NewCodeEntry (OP65_TAX, AM65_IMP, 0, 0, L[0]->LI);
                CS_InsertEntry (S, X, I+2);

                /* dey */
                X = NewCodeEntry (OP65_DEY, AM65_IMP, 0, 0, L[0]->LI);
                CS_InsertEntry (S, X, I+3);

                /* lda (c_sp),y */
                X = NewCodeEntry (OP65_LDA, AM65_ZP_INDY, "c_sp", 0, L[0]->LI);
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



unsigned OptBinOps1 (CodeSeg* S)
/* Search for an AND/EOR/ORA where the value of A or the operand is known and
** replace it by something simpler.
*/
{
    unsigned Changes = 0;

    /* Walk over the entries */
    unsigned I = 0;
    while (I < CS_GetEntryCount (S)) {

        /* Get next entry */
        CodeEntry* E = CS_GetEntry (S, I);

        /* Get a pointer to the input registers of the insn */
        const RegContents* In  = &E->RI->In;

        /* Check for AND/EOR/ORA and a known value in A */
        int Delete = 0;
        CodeEntry* X = 0;
        switch (E->OPC) {

            case OP65_AND:
                if (In->RegA == 0x00) {
                    /* Zero AND anything gives zero. The instruction can be
                    ** replaced by an immediate load of zero.
                    */
                    X = NewCodeEntry (OP65_LDA, AM65_IMM, "$00", 0, E->LI);
                } else if (In->RegA == 0xFF) {
                    /* 0xFF AND anything equals the operand. The instruction
                    ** can be replaced by a simple load of the operand.
                    */
                    X = NewCodeEntry (OP65_LDA, E->AM, E->Arg, 0, E->LI);
                } else if (E->AM == AM65_ZP) {
                    short Operand = -1;
                    switch (GetKnownReg (E->Use & REG_ZP, In)) {
                        case REG_TMP1:      Operand = In->Tmp1;     break;
                        case REG_PTR1_LO:   Operand = In->Ptr1Lo;   break;
                        case REG_PTR1_HI:   Operand = In->Ptr1Hi;   break;
                        case REG_SREG_LO:   Operand = In->SRegLo;   break;
                        case REG_SREG_HI:   Operand = In->SRegHi;   break;
                    }
                    if (Operand == 0x00) {
                        /* AND with zero gives zero. The instruction can be
                        ** replaced by an immediate load of zero.
                        */
                        X = NewCodeEntry (OP65_LDA, AM65_IMM, "$00", 0, E->LI);
                    } else if (Operand == 0xFF) {
                        /* AND with 0xFF is a no-op besides setting the flags.
                        ** The instruction can be removed if the flags aren't
                        ** used later.
                        */
                        if (!LoadFlagsUsed (S, I+1)) {
                            Delete = 1;
                        }
                    } else if (Operand >= 0) {
                        /* The instruction can be replaced by an immediate
                        ** AND.
                        */
                        const char* Arg = MakeHexArg (Operand);
                        X = NewCodeEntry (OP65_AND, AM65_IMM, Arg, 0, E->LI);
                    }
                }
                break;

            case OP65_EOR:
                if (In->RegA == 0x00) {
                    /* Zero EOR anything equals the operand. The instruction
                    ** can be replaced by a simple load.
                    */
                    X = NewCodeEntry (OP65_LDA, E->AM, E->Arg, 0, E->LI);
                } else if (E->AM == AM65_ZP) {
                    short Operand = -1;
                    switch (GetKnownReg (E->Use & REG_ZP, In)) {
                        case REG_TMP1:      Operand = In->Tmp1;     break;
                        case REG_PTR1_LO:   Operand = In->Ptr1Lo;   break;
                        case REG_PTR1_HI:   Operand = In->Ptr1Hi;   break;
                        case REG_SREG_LO:   Operand = In->SRegLo;   break;
                        case REG_SREG_HI:   Operand = In->SRegHi;   break;
                    }
                    if (Operand == 0x00) {
                        /* EOR with 0x00 is a no-op besides setting the flags.
                        ** The instruction can be removed if the flags aren't
                        ** used later.
                        */
                        if (!LoadFlagsUsed (S, I+1)) {
                            Delete = 1;
                        }
                    } else if (Operand >= 0) {
                        /* The instruction can be replaced by an immediate
                        ** EOR.
                        */
                        const char* Arg = MakeHexArg (Operand);
                        X = NewCodeEntry (OP65_EOR, AM65_IMM, Arg, 0, E->LI);
                    }
                }
                break;

            case OP65_ORA:
                if (In->RegA == 0x00) {
                    /* ORA with 0x00 is a no-op. The instruction can be
                    ** replaced by a simple load.
                    */
                    X = NewCodeEntry (OP65_LDA, E->AM, E->Arg, 0, E->LI);
                } else if (In->RegA == 0xFF) {
                    /* ORA with 0xFF gives 0xFF. The instruction can be replaced
                    ** by an immediate load of 0xFF.
                    */
                    X = NewCodeEntry (OP65_LDA, AM65_IMM, "$FF", 0, E->LI);
                } else if (E->AM == AM65_ZP) {
                    short Operand = -1;
                    switch (GetKnownReg (E->Use & REG_ZP, In)) {
                        case REG_TMP1:      Operand = In->Tmp1;     break;
                        case REG_PTR1_LO:   Operand = In->Ptr1Lo;   break;
                        case REG_PTR1_HI:   Operand = In->Ptr1Hi;   break;
                        case REG_SREG_LO:   Operand = In->SRegLo;   break;
                        case REG_SREG_HI:   Operand = In->SRegHi;   break;
                    }
                    if (Operand == 0x00) {
                        /* ORA with 0x00 is a no-op besides setting the flags.
                        ** The instruction can be removed if the flags aren't
                        ** used later.
                        */
                        if (!LoadFlagsUsed (S, I+1)) {
                            Delete = 1;
                        }
                    } else if (Operand == 0xFF) {
                        /* ORA with 0xFF results in 0xFF. The instruction can
                        ** be replaced by a simple load.
                        */
                        X = NewCodeEntry (OP65_LDA, AM65_IMM, "$FF", 0, E->LI);
                    } else if (Operand >= 0) {
                        /* The instruction can be replaced by an immediate
                        ** ORA.
                        */
                        const char* Arg = MakeHexArg (Operand);
                        X = NewCodeEntry (OP65_ORA, AM65_IMM, Arg, 0, E->LI);
                    }
                }
                break;

            default:
                break;

        }

        /* If we must delete the instruction, do that. If we have a replacement
        ** entry, place it and remove the old one.
        */
        if (X) {
            CS_InsertEntry (S, X, I+1);
            Delete = 1;
        }
        if (Delete) {
            CS_DelEntry (S, I);
            ++Changes;
        }

        /* Next entry */
        ++I;

    }

    /* Return the number of changes made */
    return Changes;
}



unsigned OptBinOps2 (CodeSeg* S)
/* Search for an AND/EOR/ORA for identical memory locations and replace it
** by something simpler.
*/
{
    unsigned Changes = 0;

    /* Walk over the entries */
    unsigned I = 0;
    while (I < CS_GetEntryCount (S)) {

        CodeEntry* N;

        /* Get next entry */
        CodeEntry* E = CS_GetEntry (S, I);

        /* Check if this is an 8 bit load followed by a bit operation with the
        ** same memory cell.
        */
        if (E->OPC == OP65_LDA                  &&
            (N = CS_GetNextEntry (S, I)) != 0   &&  /* Next insn ... */
            !CE_HasLabel (N)                    &&  /* ... without label ... */
            (N->OPC == OP65_AND ||                  /* ... is AND/EOR/ORA ... */
             N->OPC == OP65_EOR ||
             N->OPC == OP65_ORA)                &&
            E->AM == N->AM                      && /* ... with same addr mode ... */
            strcmp (E->Arg, N->Arg) == 0) {        /* ... and same argument */

            /* For an EOR, the result is zero. For the other instructions, the
            ** result doesn't change so they can be removed.
            */
            if (N->OPC == OP65_EOR) {
                /* Simply insert a load of the now known value. The flags will
                ** be correct because of the load and the preceeding
                ** instructions will be removed by later steps.
                */
                CodeEntry* X = NewCodeEntry (OP65_LDA, AM65_IMM, "$00", 0, N->LI);
                CS_InsertEntry (S, X, I+2);
            } else {
                CS_DelEntry (S, I+1);
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


unsigned OptTosPushPop (CodeSeg* S)
/* Merge jsr pushax/j?? popax */
{
    unsigned Changes = 0;
    unsigned I;

    /* Walk over the entries */
    I = 0;
    while (I < CS_GetEntryCount (S)) {

        const CodeEntry* N;

        /* Get the next entry */
        const CodeEntry* E = CS_GetEntry (S, I);

        /* Check for decspn, incspn, subysp or addysp */
        if (CE_IsCallTo (E, "pushax")                              &&
            (N = CS_GetNextEntry (S, I)) != 0                      &&
            (CE_IsCallTo (N, "popax") || CE_IsJumpTo (N, "popax")) &&
            !CE_HasLabel (N)) {

            /* Insert an rts if jmp popax */
            if (N->OPC == OP65_JMP) {
              CodeEntry* X = NewCodeEntry (OP65_RTS, AM65_IMP, 0, 0, E->LI);
              CS_InsertEntry (S, X, I);
            }

            /* Delete the old code */
            CS_DelEntries (S, I+1, 2);

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
