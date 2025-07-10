/*****************************************************************************/
/*                                                                           */
/*                                coptunary.c                                */
/*                                                                           */
/*                     Optimize bitwise unary sequences                      */
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



/* cc65 */
#include "codeent.h"
#include "codeinfo.h"
#include "coptbool.h"



/*****************************************************************************/
/*                            negax optimizations                            */
/*****************************************************************************/



unsigned OptNegAX1 (CodeSeg* S)
/* Search for a call to negax and replace it by
**
**      eor     #$FF
**      clc
**      adc     #$01
**
** if X isn't used later.
*/
{
    unsigned Changes = 0;
    unsigned I;

    /* Walk over the entries */
    I = 0;
    while (I < CS_GetEntryCount (S)) {

        /* Get next entry */
        CodeEntry* E = CS_GetEntry (S, I);

        /* Check if this is a call to negax, and if X isn't used later */
        if (CE_IsCallTo (E, "negax") && !RegXUsed (S, I+1)) {

            CodeEntry* X;

            /* Add replacement code behind */
            X = NewCodeEntry (OP65_EOR, AM65_IMM, "$FF", 0, E->LI);
            CS_InsertEntry (S, X, I+1);

            X = NewCodeEntry (OP65_CLC, AM65_IMP, 0, 0, E->LI);
            CS_InsertEntry (S, X, I+2);

            X = NewCodeEntry (OP65_ADC, AM65_IMM, "$01", 0, E->LI);
            CS_InsertEntry (S, X, I+3);

            /* Delete the call to negax */
            CS_DelEntry (S, I);

            /* Skip the generated code */
            I += 2;

            /* We had changes */
            ++Changes;
        }

        /* Next entry */
        ++I;

    }

    /* Return the number of changes made */
    return Changes;
}



unsigned OptNegAX2 (CodeSeg* S)
/* Search for a call to negax and replace it by
**
**      ldx     #$FF
**      eor     #$FF
**      clc
**      adc     #$01
**      bcc     L1
**      inx
** L1:
**
** if X is known and zero on entry.
*/
{
    unsigned Changes = 0;
    unsigned I;

    /* Walk over the entries */
    I = 0;
    while (I < CS_GetEntryCount (S)) {

        CodeEntry* P;

        /* Get next entry */
        CodeEntry* E = CS_GetEntry (S, I);

        /* Check if this is a call to negax, and if X is known and zero */
        if (E->RI->In.RegX == 0                 &&
            CE_IsCallTo (E, "negax")            &&
            (P = CS_GetNextEntry (S, I)) != 0) {

            CodeEntry* X;
            CodeLabel* L;

            /* Add replacement code behind */

            /* ldx #$FF */
            X = NewCodeEntry (OP65_LDX, AM65_IMM, "$FF", 0, E->LI);
            CS_InsertEntry (S, X, I+1);

            /* eor #$FF */
            X = NewCodeEntry (OP65_EOR, AM65_IMM, "$FF", 0, E->LI);
            CS_InsertEntry (S, X, I+2);

            /* clc */
            X = NewCodeEntry (OP65_CLC, AM65_IMP, 0, 0, E->LI);
            CS_InsertEntry (S, X, I+3);

            /* adc #$01 */
            X = NewCodeEntry (OP65_ADC, AM65_IMM, "$01", 0, E->LI);
            CS_InsertEntry (S, X, I+4);

            /* Get the label attached to the insn following the call */
            L = CS_GenLabel (S, P);

            /* bcc L */
            X = NewCodeEntry (OP65_BCC, AM65_BRA, L->Name, L, E->LI);
            CS_InsertEntry (S, X, I+5);

            /* inx */
            X = NewCodeEntry (OP65_INX, AM65_IMP, 0, 0, E->LI);
            CS_InsertEntry (S, X, I+6);

            /* Delete the call to negax */
            CS_DelEntry (S, I);

            /* Skip the generated code */
            I += 5;

            /* We had changes */
            ++Changes;
        }

        /* Next entry */
        ++I;

    }

    /* Return the number of changes made */
    return Changes;
}



/*****************************************************************************/
/*                           complax optimizations                           */
/*****************************************************************************/



unsigned OptComplAX1 (CodeSeg* S)
/* Search for a call to complax and replace it by
**
**      eor     #$FF
**
** if X isn't used later.
*/
{
    unsigned Changes = 0;
    unsigned I;

    /* Walk over the entries */
    I = 0;
    while (I < CS_GetEntryCount (S)) {

        /* Get next entry */
        CodeEntry* E = CS_GetEntry (S, I);

        /* Check if this is a call to negax, and if X isn't used later */
        if (CE_IsCallTo (E, "complax") && !RegXUsed (S, I+1)) {

            CodeEntry* X;

            /* Add replacement code behind */
            X = NewCodeEntry (OP65_EOR, AM65_IMM, "$FF", 0, E->LI);
            CS_InsertEntry (S, X, I+1);

            /* Delete the call to negax */
            CS_DelEntry (S, I);

            /* We had changes */
            ++Changes;
        }

        /* Next entry */
        ++I;

    }

    /* Return the number of changes made */
    return Changes;
}
