/*****************************************************************************/
/*                                                                           */
/*				  coptpush.c                                 */
/*                                                                           */
/*			    Optimize push sequences                          */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2001-2002 Ullrich von Bassewitz                                       */
/*               Wacholderweg 14                                             */
/*               D-70597 Stuttgart                                           */
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



/* cc65 */
#include "codeent.h"
#include "codeinfo.h"
#include "coptpush.h"



/*****************************************************************************/
/*				     Code                                    */
/*****************************************************************************/



unsigned OptPush1 (CodeSeg* S)
/* Given a sequence
 *
 *     ldy     #xx
 *     jsr     ldaxysp
 *     jsr     pushax
 *
 * If a/x are not used later, replace that by
 *
 *     ldy     #xx+2
 *     jsr     pushwysp
 *
 * saving 3 bytes and several cycles.
 */
{
    unsigned Changes = 0;

    /* Walk over the entries */
    unsigned I = 0;
    while (I < CS_GetEntryCount (S)) {

     	CodeEntry* L[3];

      	/* Get next entry */
       	L[0] = CS_GetEntry (S, I);

     	/* Check for the sequence */
	if (L[0]->OPC == OP65_LDY               &&
	    CE_KnownImm (L[0])                  &&
	    L[0]->Num < 0xFE                    &&
	    !CS_RangeHasLabel (S, I+1, 2)       &&
       	    CS_GetEntries (S, L+1, I+1, 2)   	&&
	    CE_IsCallTo (L[1], "ldaxysp")       &&
       	    CE_IsCallTo (L[2], "pushax")        &&
       	    (GetRegInfo (S, I+3, REG_AX) & REG_AX) == 0) {

	    /* Insert new code behind the pushax */
	    const char* Arg;
	    CodeEntry* X;

	    /* ldy     #xx+1 */
	    Arg = MakeHexArg (L[0]->Num+2);
	    X = NewCodeEntry (OP65_LDY, AM65_IMM, Arg, 0, L[0]->LI);
	    CS_InsertEntry (S, X, I+3);

	    /* jsr pushwysp */
	    X = NewCodeEntry (OP65_JSR, AM65_ABS, "pushwysp", 0, L[2]->LI);
	    CS_InsertEntry (S, X, I+4);

	    /* Delete the old code */
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



