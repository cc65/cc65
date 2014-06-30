/*****************************************************************************/
/*                                                                           */
/*                                copttest.c                                 */
/*                                                                           */
/*                          Optimize test sequences                          */
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
#include "copttest.h"



/*****************************************************************************/
/*                              Optimize tests                               */
/*****************************************************************************/



unsigned OptTest1 (CodeSeg* S)
/* Given a sequence
**
**     stx     xxx
**     ora     xxx
**     beq/bne ...
**
** If X is zero, the sequence may be changed to
**
**     cmp     #$00
**     beq/bne ...
**
** which may be optimized further by another step.
**
** If A is zero, the sequence may be changed to
**
**     txa
**     beq/bne ...
*/
{
    unsigned Changes = 0;
    unsigned I;

    /* Walk over the entries */
    I = 0;
    while (I < CS_GetEntryCount (S)) {

        CodeEntry* L[3];

        /* Get next entry */
        L[0] = CS_GetEntry (S, I);

        /* Check if it's the sequence we're searching for */
        if (L[0]->OPC == OP65_STX              &&
            CS_GetEntries (S, L+1, I+1, 2)     &&
            !CE_HasLabel (L[1])                &&
            L[1]->OPC == OP65_ORA              &&
            strcmp (L[0]->Arg, L[1]->Arg) == 0 &&
            !CE_HasLabel (L[2])                &&
            (L[2]->Info & OF_ZBRA) != 0) {

            /* Check if X is zero */
            if (L[0]->RI->In.RegX == 0) {

                /* Insert the compare */
                CodeEntry* N = NewCodeEntry (OP65_CMP, AM65_IMM, "$00", 0, L[0]->LI);
                CS_InsertEntry (S, N, I+2);

                /* Remove the two other insns */
                CS_DelEntry (S, I+1);
                CS_DelEntry (S, I);

                /* We had changes */
                ++Changes;

            /* Check if A is zero */
            } else if (L[1]->RI->In.RegA == 0) {

                /* Insert the txa */
                CodeEntry* N = NewCodeEntry (OP65_TXA, AM65_IMP, 0, 0, L[1]->LI);
                CS_InsertEntry (S, N, I+2);

                /* Remove the two other insns */
                CS_DelEntry (S, I+1);
                CS_DelEntry (S, I);

                /* We had changes */
                ++Changes;
            }
        }

        /* Next entry */
        ++I;

    }

    /* Return the number of changes made */
    return Changes;
}



unsigned OptTest2 (CodeSeg* S)
/* Search for an inc/dec operation followed by a load and a conditional
** branch based on the flags from the load. Remove the load if the insn
** isn't used later.
*/
{
    unsigned Changes = 0;

    /* Walk over the entries */
    unsigned I = 0;
    while (I < CS_GetEntryCount (S)) {

        CodeEntry* L[3];

        /* Get next entry */
        L[0] = CS_GetEntry (S, I);

        /* Check if it's the sequence we're searching for */
        if ((L[0]->OPC == OP65_INC || L[0]->OPC == OP65_DEC)    &&
            CS_GetEntries (S, L+1, I+1, 2)                      &&
            !CE_HasLabel (L[1])                                 &&
            (L[1]->Info & OF_LOAD) != 0                         &&
            (L[2]->Info & OF_FBRA) != 0                         &&
            L[1]->AM == L[0]->AM                                &&
            strcmp (L[0]->Arg, L[1]->Arg) == 0                  &&
            (GetRegInfo (S, I+2, L[1]->Chg) & L[1]->Chg) == 0) {

            /* Remove the load */
            CS_DelEntry (S, I+1);
             ++Changes;
        }

        /* Next entry */
        ++I;

    }

    /* Return the number of changes made */
    return Changes;
}
