/*****************************************************************************/
/*                                                                           */
/*                                coptpush.c                                 */
/*                                                                           */
/*                          Optimize push sequences                          */
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
#include "coptpush.h"



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



unsigned OptPush1 (CodeSeg* S)
/* Given a sequence
**
**     jsr     ldaxysp
**     jsr     pushax
**
** If a/x are not used later, and Y is known, replace that by
**
**     ldy     #xx+2
**     jsr     pushwysp
**
** saving 3 bytes and several cycles.
*/
{
    unsigned I;
    unsigned Changes = 0;

    /* Walk over the entries */
    I = 0;
    while (I < CS_GetEntryCount (S)) {

        CodeEntry* L[2];

        /* Get next entry */
        L[0] = CS_GetEntry (S, I);

        /* Check for the sequence */
        if (CE_IsCallTo (L[0], "ldaxysp")               &&
            RegValIsKnown (L[0]->RI->In.RegY)           &&
            L[0]->RI->In.RegY < 0xFE                    &&
            (L[1] = CS_GetNextEntry (S, I)) != 0        &&
            !CE_HasLabel (L[1])                         &&
            CE_IsCallTo (L[1], "pushax")                &&
            !RegAXUsed (S, I+2)) {

            /* Insert new code behind the pushax */
            const char* Arg;
            CodeEntry* X;

            /* ldy     #xx+1 */
            Arg = MakeHexArg (L[0]->RI->In.RegY+2);
            X = NewCodeEntry (OP65_LDY, AM65_IMM, Arg, 0, L[0]->LI);
            CS_InsertEntry (S, X, I+2);

            /* jsr pushwysp */
            X = NewCodeEntry (OP65_JSR, AM65_ABS, "pushwysp", 0, L[1]->LI);
            CS_InsertEntry (S, X, I+3);

            /* Delete the old code */
            CS_DelEntries (S, I, 2);

            /* Remember, we had changes */
            ++Changes;

        }

        /* Next entry */
        ++I;

    }

    /* Return the number of changes made */
    return Changes;
}



unsigned OptPush2 (CodeSeg* S)
/* A sequence
**
**     jsr     ldaxidx
**     jsr     pushax
**
** may get replaced by
**
**     jsr     pushwidx
*/
{
    unsigned I;
    unsigned Changes = 0;

    /* Walk over the entries */
    I = 0;
    while (I < CS_GetEntryCount (S)) {

        CodeEntry* L[2];

        /* Get next entry */
        L[0] = CS_GetEntry (S, I);

        /* Check for the sequence */
        if (CE_IsCallTo (L[0], "ldaxidx")               &&
            (L[1] = CS_GetNextEntry (S, I)) != 0        &&
            !CE_HasLabel (L[1])                         &&
            CE_IsCallTo (L[1], "pushax")) {

            /* Insert new code behind the pushax */
            CodeEntry* X;

            /* jsr pushwidx */
            X = NewCodeEntry (OP65_JSR, AM65_ABS, "pushwidx", 0, L[1]->LI);
            CS_InsertEntry (S, X, I+2);

            /* Delete the old code */
            CS_DelEntries (S, I, 2);

            /* Remember, we had changes */
            ++Changes;

        }

        /* Next entry */
        ++I;

    }

    /* Return the number of changes made */
    return Changes;
}
