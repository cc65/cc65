/*****************************************************************************/
/*                                                                           */
/*				   coptneg.c                                 */
/*                                                                           */
/*			  Optimize negation sequences                        */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2001      Ullrich von Bassewitz                                       */
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
#include "coptneg.h"



/*****************************************************************************/
/*		  	      nega optimizations			     */
/*****************************************************************************/



unsigned OptNegA1 (CodeSeg* S)
/* Check for
 *
 *	ldx 	#$00
 *	lda 	..
 * 	jsr 	bnega
 *
 * Remove the ldx if the lda does not use it.
 */
{
    unsigned Changes = 0;

    /* Walk over the entries */
    unsigned I = 0;
    while (I < CS_GetEntryCount (S)) {

	CodeEntry* L[2];

      	/* Get next entry */
       	CodeEntry* E = CS_GetEntry (S, I);

     	/* Check for a ldx */
       	if (E->OPC == OP65_LDX 			&&
	    E->AM == AM65_IMM	    		&&
	    (E->Flags & CEF_NUMARG) != 0	&&
	    E->Num == 0	   			&&
  	    CS_GetEntries (S, L, I+1, 2)	&&
	    L[0]->OPC == OP65_LDA		&&
	    (L[0]->Use & REG_X) == 0	    	&&
	    !CE_HasLabel (L[0])                 &&
	    CE_IsCall (L[1], "bnega")           &&
	    !CE_HasLabel (L[1])) {

	    /* Remove the ldx instruction */
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



unsigned OptNegA2 (CodeSeg* S)
/* Check for
 *
 *	lda 	..
 * 	jsr 	bnega
 *	jeq/jne	..
 *
 * Adjust the conditional branch and remove the call to the subroutine.
 */
{
    unsigned Changes = 0;

    /* Walk over the entries */
    unsigned I = 0;
    while (I < CS_GetEntryCount (S)) {

	CodeEntry* L[2];

      	/* Get next entry */
       	CodeEntry* E = CS_GetEntry (S, I);

     	/* Check for the sequence */
	if ((E->OPC == OP65_ADC ||
	     E->OPC == OP65_AND ||
	     E->OPC == OP65_DEA ||
	     E->OPC == OP65_EOR ||
	     E->OPC == OP65_INA ||
       	     E->OPC == OP65_LDA ||
	     E->OPC == OP65_ORA	||
	     E->OPC == OP65_PLA ||
	     E->OPC == OP65_SBC ||
	     E->OPC == OP65_TXA ||
	     E->OPC == OP65_TYA)                &&
	    CS_GetEntries (S, L, I+1, 2)	&&
       	    CE_IsCall (L[0], "bnega")           &&
	    !CE_HasLabel (L[0])	  	        &&
	    (L[1]->Info & OF_ZBRA) != 0         &&
	    !CE_HasLabel (L[1])) {

	    /* Invert the branch */
	    CE_ReplaceOPC (L[1], GetInverseBranch (L[1]->OPC));

	    /* Delete the subroutine call */
	    CS_DelEntry (S, I+1);

	    /* Remember, we had changes */
	    ++Changes;

	}

	/* Next entry */
	++I;

    }

    /* Return the number of changes made */
    return Changes;
}



/*****************************************************************************/
/*		   	      negax optimizations			     */
/*****************************************************************************/



unsigned OptNegAX1 (CodeSeg* S)
/* On a call to bnegax, if X is zero, the result depends only on the value in
 * A, so change the call to a call to bnega. This will get further optimized
 * later if possible.
 */
{
    unsigned Changes = 0;
    unsigned I;

    /* Generate register info for this step */
    CS_GenRegInfo (S);

    /* Walk over the entries */
    I = 0;
    while (I < CS_GetEntryCount (S)) {

      	/* Get next entry */
       	CodeEntry* E = CS_GetEntry (S, I);

	/* Check if this is a call to bnegax, and if X is known and zero */
	if (E->RI->In.RegX == 0             &&
       	    CE_IsCall (E, "bnegax")) {

	    /* We're cheating somewhat here ... */
	    E->Arg[5] = '\0';
	    E->Use &= ~REG_X;

	    /* We had changes */
	    ++Changes;
	}

	/* Next entry */
	++I;

    }

    /* Free register info */
    CS_FreeRegInfo (S);

    /* Return the number of changes made */
    return Changes;
}



unsigned OptNegAX2 (CodeSeg* S)
/* Search for the sequence:
 *
 *  	lda	(xx),y
 *  	tax
 *  	dey
 *  	lda	(xx),y
 *  	jsr	bnegax
 *  	jne/jeq	...
 *
 * and replace it by
 *
 *  	lda    	(xx),y
 *  	dey
 *  	ora    	(xx),y
 *	jeq/jne	...
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
       	if (E->OPC == OP65_LDA  	      	&&
	    E->AM == AM65_ZP_INDY	      	&&
	    CS_GetEntries (S, L, I+1, 5)	&&
	    L[0]->OPC == OP65_TAX    		&&
	    L[1]->OPC == OP65_DEY    		&&
      	    L[2]->OPC == OP65_LDA    		&&
	    L[2]->AM == AM65_ZP_INDY  		&&
	    strcmp (L[2]->Arg, E->Arg) == 0	&&
	    !CE_HasLabel (L[2])		        &&
       	    CE_IsCall (L[3], "bnegax")          &&
	    !CE_HasLabel (L[3])		        &&
       	    (L[4]->Info & OF_ZBRA) != 0         &&
	    !CE_HasLabel (L[4])) {

	    /* lda --> ora */
	    CE_ReplaceOPC (L[2], OP65_ORA);

  	    /* Invert the branch */
	    CE_ReplaceOPC (L[4], GetInverseBranch (L[4]->OPC));

      	    /* Delete the entries no longer needed. Beware: Deleting entries
	     * will change the indices.
	     */
       	    CS_DelEntry (S, I+4);	    	/* jsr bnegax */
	    CS_DelEntry (S, I+1);	    	/* tax */

	    /* Remember, we had changes */
	    ++Changes;

	}

	/* Next entry */
	++I;

    }

    /* Return the number of changes made */
    return Changes;
}



unsigned OptNegAX3 (CodeSeg* S)
/* Search for the sequence:
 *
 *  	lda	xx
 *  	ldx	yy
 *  	jsr	bnegax
 *    	jne/jeq	...
 *
 * and replace it by
 *
 *  	lda    	xx
 *	ora	xx+1
 *	jeq/jne	...
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
       	if (E->OPC == OP65_LDA  	      	&&
       	    CS_GetEntries (S, L, I+1, 3)	&&
	    L[0]->OPC == OP65_LDX       	&&
	    !CE_HasLabel (L[0]) 		&&
       	    CE_IsCall (L[1], "bnegax")          &&
	    !CE_HasLabel (L[1]) 		&&
       	    (L[2]->Info & OF_ZBRA) != 0         &&
	    !CE_HasLabel (L[2])) {

	    /* ldx --> ora */
	    CE_ReplaceOPC (L[0], OP65_ORA);

	    /* Invert the branch */
       	    CE_ReplaceOPC (L[2], GetInverseBranch (L[2]->OPC));

	    /* Delete the subroutine call */
       	    CS_DelEntry (S, I+2);

	    /* Remember, we had changes */
	    ++Changes;

	}

	/* Next entry */
	++I;

    }

    /* Return the number of changes made */
    return Changes;
}



unsigned OptNegAX4 (CodeSeg* S)
/* Search for the sequence:
 *
 *    	jsr   	xxx
 *  	jsr   	bnega(x)
 *  	jeq/jne	...
 *
 * and replace it by:
 *
 *      jsr	xxx
 *  	<boolean test>
 *  	jne/jeq	...
 */
{
    unsigned Changes = 0;

    /* Walk over the entries */
    unsigned I = 0;
    while (I < CS_GetEntryCount (S)) {

	CodeEntry* L[2];

      	/* Get next entry */
       	CodeEntry* E = CS_GetEntry (S, I);

     	/* Check for the sequence */
       	if (E->OPC == OP65_JSR  	      	&&
       	    CS_GetEntries (S, L, I+1, 2)   	&&
       	    L[0]->OPC == OP65_JSR              	&&
	    strncmp (L[0]->Arg,"bnega",5) == 0 	&&
	    !CE_HasLabel (L[0]) 	       	&&
       	    (L[1]->Info & OF_ZBRA) != 0         &&
	    !CE_HasLabel (L[1])) {

	    CodeEntry* X;

	    /* Check if we're calling bnega or bnegax */
	    int ByteSized = (strcmp (L[0]->Arg, "bnega") == 0);

	    /* Insert apropriate test code */
	    if (ByteSized) {
	     	/* Test bytes */
	    	X = NewCodeEntry (OP65_TAX, AM65_IMP, 0, 0, L[0]->LI);
  	    	CS_InsertEntry (S, X, I+2);
	    } else {
	    	/* Test words */
	    	X = NewCodeEntry (OP65_STX, AM65_ZP, "tmp1", 0, L[0]->LI);
      	    	CS_InsertEntry (S, X, I+2);
	    	X = NewCodeEntry (OP65_ORA, AM65_ZP, "tmp1", 0, L[0]->LI);
	    	CS_InsertEntry (S, X, I+3);
	    }

	    /* Delete the subroutine call */
	    CS_DelEntry (S, I+1);

	    /* Invert the branch */
       	    CE_ReplaceOPC (L[1], GetInverseBranch (L[1]->OPC));

	    /* Remember, we had changes */
	    ++Changes;

	}

	/* Next entry */
	++I;

    }

    /* Return the number of changes made */
    return Changes;
}



