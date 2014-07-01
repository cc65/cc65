/*****************************************************************************/
/*                                                                           */
/*                                 coptc02.h                                 */
/*                                                                           */
/*                       65C02 specific optimizations                        */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2001-2012, Ullrich von Bassewitz                                      */
/*                Roeerstrasse 52                                            */
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



#include <string.h>

/* cc65 */
#include "codeent.h"
#include "codeinfo.h"
#include "error.h"
#include "coptc02.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/*****************************************************************************/
/*                             Helper functions                              */
/*****************************************************************************/



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



unsigned Opt65C02Ind (CodeSeg* S)
/* Try to use the indirect addressing mode where possible */
{
    unsigned Changes = 0;
    unsigned I;

    /* Walk over the entries */
    I = 0;
    while (I < CS_GetEntryCount (S)) {

        /* Get next entry */
        CodeEntry* E = CS_GetEntry (S, I);

        /* Check for addressing mode indirect indexed Y where Y is zero.
        ** Note: All opcodes that are available as (zp),y are also available
        ** as (zp), so we can ignore the actual opcode here.
        */
        if (E->AM == AM65_ZP_INDY && E->RI->In.RegY == 0) {

            /* Replace it by indirect addressing mode */
            CodeEntry* X = NewCodeEntry (E->OPC, AM65_ZP_IND, E->Arg, 0, E->LI);
            CS_InsertEntry (S, X, I+1);
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



unsigned Opt65C02BitOps (CodeSeg* S)
/* Use special bit op instructions of the C02 */
{
    unsigned Changes = 0;
    unsigned I;

    /* Walk over the entries */
    I = 0;
    while (I < CS_GetEntryCount (S)) {

        CodeEntry* L[3];

        /* Get next entry */
        L[0] = CS_GetEntry (S, I);

        /* Check for the sequence */
        if (L[0]->OPC == OP65_LDA                               &&
            (L[0]->AM == AM65_ZP || L[0]->AM == AM65_ABS)       &&
            !CS_RangeHasLabel (S, I+1, 2)                       &&
            CS_GetEntries (S, L+1, I+1, 2)                      &&
            (L[1]->OPC == OP65_AND || L[1]->OPC == OP65_ORA)    &&
            CE_IsConstImm (L[1])                                &&
            L[2]->OPC == OP65_STA                               &&
            L[2]->AM == L[0]->AM                                &&
            strcmp (L[2]->Arg, L[0]->Arg) == 0                  &&
            !RegAUsed (S, I+3)) {

            char Buf[32];
            CodeEntry* X;

            /* Use TRB for AND and TSB for ORA */
            if (L[1]->OPC == OP65_AND) {

                /* LDA #XX */
                sprintf (Buf, "$%02X", (int) ((~L[1]->Num) & 0xFF));
                X = NewCodeEntry (OP65_LDA, AM65_IMM, Buf, 0, L[1]->LI);
                CS_InsertEntry (S, X, I+3);

                /* TRB */
                X = NewCodeEntry (OP65_TRB, L[0]->AM, L[0]->Arg, 0, L[0]->LI);
                CS_InsertEntry (S, X, I+4);

            } else {

                /* LDA #XX */
                sprintf (Buf, "$%02X", (int) L[1]->Num);
                X = NewCodeEntry (OP65_LDA, AM65_IMM, Buf, 0, L[1]->LI);
                CS_InsertEntry (S, X, I+3);

                /* TSB */
                X = NewCodeEntry (OP65_TSB, L[0]->AM, L[0]->Arg, 0, L[0]->LI);
                CS_InsertEntry (S, X, I+4);
            }

            /* Delete the old stuff */
            CS_DelEntries (S, I, 3);

            /* We had changes */
            ++Changes;
        }

        /* Next entry */
        ++I;

    }

    /* Return the number of changes made */
    return Changes;
}



unsigned Opt65C02Stores (CodeSeg* S)
/* Use STZ where possible */
{
    unsigned Changes = 0;
    unsigned I;

    /* Walk over the entries */
    I = 0;
    while (I < CS_GetEntryCount (S)) {

        /* Get next entry */
        CodeEntry* E = CS_GetEntry (S, I);

        /* Check for a store with a register value of zero and an addressing
        ** mode available with STZ.
        */
        if (((E->OPC == OP65_STA && E->RI->In.RegA == 0) ||
             (E->OPC == OP65_STX && E->RI->In.RegX == 0) ||
             (E->OPC == OP65_STY && E->RI->In.RegY == 0))       &&
            (E->AM == AM65_ZP  || E->AM == AM65_ABS ||
             E->AM == AM65_ZPX || E->AM == AM65_ABSX)) {

            /* Replace by STZ */
            CodeEntry* X = NewCodeEntry (OP65_STZ, E->AM, E->Arg, 0, E->LI);
            CS_InsertEntry (S, X, I+1);

            /* Delete the old stuff */
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
