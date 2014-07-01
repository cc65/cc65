/*****************************************************************************/
/*                                                                           */
/*                                 coptsub.c                                 */
/*                                                                           */
/*                      Optimize subtraction sequences                       */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2001-2006, Ullrich von Bassewitz                                      */
/*                Römerstrasse 52                                            */
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
#include "coptsub.h"



/*****************************************************************************/
/*                           Optimize subtractions                           */
/*****************************************************************************/



unsigned OptSub1 (CodeSeg* S)
/* Search for the sequence
**
**      sbc     ...
**      bcs     L
**      dex
** L:
**
** and remove the handling of the high byte if X is not used later.
*/
{
    unsigned Changes = 0;

    /* Walk over the entries */
    unsigned I = 0;
    while (I < CS_GetEntryCount (S)) {

        CodeEntry* L[3];

        /* Get next entry */
        CodeEntry* E = CS_GetEntry (S, I);

        /* Check for the sequence */
        if (E->OPC == OP65_SBC                               &&
            CS_GetEntries (S, L, I+1, 3)                     &&
            (L[0]->OPC == OP65_BCS || L[0]->OPC == OP65_JCS) &&
            L[0]->JumpTo != 0                                &&
            !CE_HasLabel (L[0])                              &&
            L[1]->OPC == OP65_DEX                            &&
            !CE_HasLabel (L[1])                              &&
            L[0]->JumpTo->Owner == L[2]                      &&
            !RegXUsed (S, I+3)) {

            /* Remove the bcs/dex */
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



unsigned OptSub2 (CodeSeg* S)
/* Search for the sequence
**
**      lda     xx
**      sec
**      sta     tmp1
**      lda     yy
**      sbc     tmp1
**      sta     yy
**
** and replace it by
**
**      sec
**      lda     yy
**      sbc     xx
**      sta     yy
*/
{
    unsigned Changes = 0;

    /* Walk over the entries */
    unsigned I = 0;
    while (I < CS_GetEntryCount (S)) {

        CodeEntry* L[5];

        /* Get next entry */
        CodeEntry* E = CS_GetEntry (S, I);

        /* Check for the sequence */
        if (E->OPC == OP65_LDA                             &&
            !CS_RangeHasLabel (S, I+1, 5)                  &&
            CS_GetEntries (S, L, I+1, 5)                   &&
            L[0]->OPC == OP65_SEC                          &&
            L[1]->OPC == OP65_STA                          &&
            strcmp (L[1]->Arg, "tmp1") == 0                &&
            L[2]->OPC == OP65_LDA                          &&
            L[3]->OPC == OP65_SBC                          &&
            strcmp (L[3]->Arg, "tmp1") == 0                &&
            L[4]->OPC == OP65_STA                          &&
            strcmp (L[4]->Arg, L[2]->Arg) == 0) {

            /* Remove the store to tmp1 */
            CS_DelEntry (S, I+2);

            /* Remove the subtraction */
            CS_DelEntry (S, I+3);

            /* Move the lda to the position of the subtraction and change the
            ** op to SBC.
            */
            CS_MoveEntry (S, I, I+3);
            CE_ReplaceOPC (E, OP65_SBC);

            /* If the sequence head had a label, move this label back to the
            ** head.
            */
            if (CE_HasLabel (E)) {
                CS_MoveLabels (S, E, L[0]);
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



unsigned OptSub3 (CodeSeg* S)
/* Search for a call to decaxn and replace it by an 8 bit sub if the X register
** is not used later.
*/
{
    unsigned Changes = 0;

    /* Walk over the entries */
    unsigned I = 0;
    while (I < CS_GetEntryCount (S)) {

        CodeEntry* E;

        /* Get next entry */
        E = CS_GetEntry (S, I);

        /* Check for the sequence */
        if (E->OPC == OP65_JSR                          &&
            strncmp (E->Arg, "decax", 5) == 0           &&
            IsDigit (E->Arg[5])                         &&
            E->Arg[6] == '\0'                           &&
            !RegXUsed (S, I+1)) {

            CodeEntry* X;
            const char* Arg;

            /* Insert new code behind the sequence */
            X = NewCodeEntry (OP65_SEC, AM65_IMP, 0, 0, E->LI);
            CS_InsertEntry (S, X, I+1);

            Arg = MakeHexArg (E->Arg[5] - '0');
            X = NewCodeEntry (OP65_SBC, AM65_IMM, Arg, 0, E->LI);
            CS_InsertEntry (S, X, I+2);

            /* Delete the old code */
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
