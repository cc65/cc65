/*****************************************************************************/
/*                                                                           */
/*				   codeopt.c				     */
/*                                                                           */
/*			     Optimizer subroutines			     */
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



#include <string.h>

/* common */
#include "abend.h"
#include "chartype.h"
#include "print.h"
#include "xmalloc.h"
#include "xsprintf.h"

/* cc65 */
#include "asmlabel.h"
#include "codeent.h"
#include "codeinfo.h"
#include "coptadd.h"
#include "coptcmp.h"
#include "coptind.h"
#include "coptstop.h"
#include "coptsub.h"
#include "error.h"
#include "global.h"
#include "codeopt.h"



/*****************************************************************************/
/*  	     	   	       Helper functions	    			     */
/*****************************************************************************/



/*****************************************************************************/
/*				Optimize shifts                              */
/*****************************************************************************/



static unsigned OptShift1 (CodeSeg* S)
/* A call to the shlaxN routine may get replaced by one or more asl insns
 * if the value of X is not used later.
 */
{
    unsigned Changes = 0;

    /* Walk over the entries */
    unsigned I = 0;
    while (I < CS_GetEntryCount (S)) {

      	/* Get next entry */
       	CodeEntry* E = CS_GetEntry (S, I);

     	/* Check for the sequence */
	if (E->OPC == OP65_JSR                       &&
       	    (strncmp (E->Arg, "shlax", 5) == 0 ||
	     strncmp (E->Arg, "aslax", 5) == 0)	     &&
	    strlen (E->Arg) == 6                     &&
	    IsDigit (E->Arg[5])                      &&
	    !RegXUsed (S, I+1)) {

	    /* Insert shift insns */
	    unsigned Count = E->Arg[5] - '0';
	    while (Count--) {
	    	CodeEntry* X = NewCodeEntry (OP65_ASL, AM65_ACC, "a", 0, E->LI);
	    	CS_InsertEntry (S, X, I+1);
	    }

	    /* Delete the call to shlax */
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



/*****************************************************************************/
/*	    			Optimize tests                               */
/*****************************************************************************/



static unsigned OptTest1 (CodeSeg* S)
/* On a sequence
 *
 *     stx     xxx
 *     ora     xxx
 *     beq/bne ...
 *
 * if X is zero, the sequence may be changed to
 *
 *     cmp     #$00
 *     beq/bne ...
 *
 * which may be optimized further by another step.
 *
 * If A is zero, the sequence may be changed to
 *
 *     txa
 *     beq/bne ...
 *
 */
{
    unsigned Changes = 0;
    unsigned I;

    /* Generate register info for this step */
    CS_GenRegInfo (S);

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

    /* Free register info */
    CS_FreeRegInfo (S);

    /* Return the number of changes made */
    return Changes;
}



/*****************************************************************************/
/*		  	      nega optimizations			     */
/*****************************************************************************/



static unsigned OptNegA1 (CodeSeg* S)
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
	    L[1]->OPC == OP65_JSR	    	&&
	    strcmp (L[1]->Arg, "bnega") == 0    &&
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



static unsigned OptNegA2 (CodeSeg* S)
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
       	    L[0]->OPC == OP65_JSR  	    	&&
	    strcmp (L[0]->Arg, "bnega") == 0	&&
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



static unsigned OptNegAX1 (CodeSeg* S)
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
	if (E->OPC == OP65_JSR              &&
	    E->RI->In.RegX == 0             &&
	    strcmp (E->Arg, "bnegax") == 0) {

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



static unsigned OptNegAX2 (CodeSeg* S)
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
	    L[3]->OPC == OP65_JSR    		&&
	    strcmp (L[3]->Arg, "bnegax") == 0	&&
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



static unsigned OptNegAX3 (CodeSeg* S)
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
       	    L[1]->OPC == OP65_JSR      		&&
	    strcmp (L[1]->Arg, "bnegax") == 0	&&
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



static unsigned OptNegAX4 (CodeSeg* S)
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



/*****************************************************************************/
/*		       Optimize stores through pointers                      */
/*****************************************************************************/



static unsigned OptPtrStore1Sub (CodeSeg* S, unsigned I, CodeEntry** const L)
/* Check if this is one of the allowed suboperation for OptPtrStore1 */
{
    /* Check for a label attached to the entry */
    if (CE_HasLabel (L[0])) {
	return 0;
    }

    /* Check for single insn sub ops */
    if (L[0]->OPC == OP65_AND                                           ||
    	L[0]->OPC == OP65_EOR                                           ||
    	L[0]->OPC == OP65_ORA                                           ||
    	(L[0]->OPC == OP65_JSR && strncmp (L[0]->Arg, "shlax", 5) == 0) ||
       	(L[0]->OPC == OP65_JSR && strncmp (L[0]->Arg, "shrax", 5) == 0)) {

    	/* One insn */
    	return 1;

    } else if (L[0]->OPC == OP65_CLC                      &&
       	       (L[1] = CS_GetNextEntry (S, I)) != 0       &&
	       L[1]->OPC == OP65_ADC                      &&
	       !CE_HasLabel (L[1])) {
	return 2;
    } else if (L[0]->OPC == OP65_SEC                      &&
	       (L[1] = CS_GetNextEntry (S, I)) != 0       &&
	       L[1]->OPC == OP65_SBC                      &&
	       !CE_HasLabel (L[1])) {
	return 2;
    }



    /* Not found */
    return 0;
}



static unsigned OptPtrStore1 (CodeSeg* S)
/* Search for the sequence:
 *
 *    	jsr   	pushax
 *      ldy     xxx
 *      jsr     ldauidx
 *      subop
 *      ldy     yyy
 *  	jsr   	staspidx
 *
 * and replace it by:
 *
 *      sta     ptr1
 *      stx     ptr1+1
 *      ldy     xxx
 *      ldx     #$00
 *      lda     (ptr1),y
 *	subop
 *      ldy     yyy
 *      sta     (ptr1),y
 */
{
    unsigned Changes = 0;

    /* Walk over the entries */
    unsigned I = 0;
    while (I < CS_GetEntryCount (S)) {

	unsigned K;
     	CodeEntry* L[10];

      	/* Get next entry */
       	L[0] = CS_GetEntry (S, I);

     	/* Check for the sequence */
       	if (L[0]->OPC == OP65_JSR    	      	    &&
     	    strcmp (L[0]->Arg, "pushax") == 0       &&
       	    CS_GetEntries (S, L+1, I+1, 3)     	    &&
       	    L[1]->OPC == OP65_LDY              	    &&
     	    CE_KnownImm (L[1])                      &&
     	    !CE_HasLabel (L[1])      	       	    &&
     	    L[2]->OPC == OP65_JSR                   &&
     	    strcmp (L[2]->Arg, "ldauidx") == 0      &&
     	    !CE_HasLabel (L[2])                     &&
       	    (K = OptPtrStore1Sub (S, I+3, L+3)) > 0 &&
	    CS_GetEntries (S, L+3+K, I+3+K, 2)      &&
       	    L[3+K]->OPC == OP65_LDY                 &&
     	    CE_KnownImm (L[3+K])                    &&
	    !CE_HasLabel (L[3+K])                   &&
	    L[4+K]->OPC == OP65_JSR                 &&
	    strcmp (L[4+K]->Arg, "staspidx") == 0   &&
	    !CE_HasLabel (L[4+K])) {

	    CodeEntry* X;

	    /* Create and insert the stores */
       	    X = NewCodeEntry (OP65_STA, AM65_ZP, "ptr1", 0, L[0]->LI);
	    CS_InsertEntry (S, X, I+1);

	    X = NewCodeEntry (OP65_STX, AM65_ZP, "ptr1+1", 0, L[0]->LI);
	    CS_InsertEntry (S, X, I+2);

	    /* Delete the call to pushax */
	    CS_DelEntry (S, I);

	    /* Delete the call to ldauidx */
	    CS_DelEntry (S, I+3);

	    /* Insert the load from ptr1 */
	    X = NewCodeEntry (OP65_LDX, AM65_IMM, "$00", 0, L[3]->LI);
	    CS_InsertEntry (S, X, I+3);
	    X = NewCodeEntry (OP65_LDA, AM65_ZP_INDY, "ptr1", 0, L[2]->LI);
	    CS_InsertEntry (S, X, I+4);

	    /* Insert the store through ptr1 */
	    X = NewCodeEntry (OP65_STA, AM65_ZP_INDY, "ptr1", 0, L[3]->LI);
	    CS_InsertEntry (S, X, I+6+K);

	    /* Delete the call to staspidx */
	    CS_DelEntry (S, I+7+K);

	    /* Remember, we had changes */
	    ++Changes;

	}

	/* Next entry */
	++I;

    }

    /* Return the number of changes made */
    return Changes;
}



static unsigned OptPtrStore2 (CodeSeg* S)
/* Search for the sequence:
 *
 *    	jsr   	pushax
 *      lda     xxx
 *      ldy     yyy
 *  	jsr   	staspidx
 *
 * and replace it by:
 *
 *      sta     ptr1
 *      stx     ptr1+1
 *      lda     xxx
 *      ldy     yyy
 *      sta     (ptr1),y
 */
{
    unsigned Changes = 0;

    /* Walk over the entries */
    unsigned I = 0;
    while (I < CS_GetEntryCount (S)) {

	CodeEntry* L[4];

      	/* Get next entry */
       	L[0] = CS_GetEntry (S, I);

     	/* Check for the sequence */
       	if (L[0]->OPC == OP65_JSR  	      	&&
	    strcmp (L[0]->Arg, "pushax") == 0   &&
       	    CS_GetEntries (S, L+1, I+1, 3)   	&&
       	    L[1]->OPC == OP65_LDA              	&&
	    !CE_HasLabel (L[1])    	       	&&
	    L[2]->OPC == OP65_LDY               &&
	    !CE_HasLabel (L[2])                 &&
	    L[3]->OPC == OP65_JSR               &&
	    strcmp (L[3]->Arg, "staspidx") == 0 &&
	    !CE_HasLabel (L[3])) {

	    CodeEntry* X;

	    /* Create and insert the stores */
       	    X = NewCodeEntry (OP65_STA, AM65_ZP, "ptr1", 0, L[0]->LI);
	    CS_InsertEntry (S, X, I+1);

	    X = NewCodeEntry (OP65_STX, AM65_ZP, "ptr1+1", 0, L[0]->LI);
	    CS_InsertEntry (S, X, I+2);

	    /* Delete the call to pushax */
	    CS_DelEntry (S, I);

	    /* Insert the store through ptr1 */
	    X = NewCodeEntry (OP65_STA, AM65_ZP_INDY, "ptr1", 0, L[3]->LI);
	    CS_InsertEntry (S, X, I+4);

	    /* Delete the call to staspidx */
	    CS_DelEntry (S, I+5);

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
/*			Optimize loads through pointers                      */
/*****************************************************************************/



static unsigned OptPtrLoad1 (CodeSeg* S)
/* Search for the sequence:
 *
 *      tax
 *      dey
 *      lda     (sp),y             # May be any destination
 *      ldy     ...
 *  	jsr    	ldauidx
 *
 * and replace it by:
 *
 *      sta     ptr1+1
 *      dey
 *      lda     (sp),y
 *      sta     ptr1
 *      ldy     ...
 *      ldx     #$00
 *      lda     (ptr1),y
 */
{
    unsigned Changes = 0;

    /* Walk over the entries */
    unsigned I = 0;
    while (I < CS_GetEntryCount (S)) {

	CodeEntry* L[5];

      	/* Get next entry */
       	L[0] = CS_GetEntry (S, I);

     	/* Check for the sequence */
       	if (L[0]->OPC == OP65_TAX      	       	&&
       	    CS_GetEntries (S, L+1, I+1, 4)     	&&
       	    L[1]->OPC == OP65_DEY              	&&
	    !CE_HasLabel (L[1])        	       	&&
	    L[2]->OPC == OP65_LDA               &&
	    !CE_HasLabel (L[2])                 &&
	    L[3]->OPC == OP65_LDY               &&
	    !CE_HasLabel (L[3])                 &&
	    L[4]->OPC == OP65_JSR               &&
       	    strcmp (L[4]->Arg, "ldauidx") == 0  &&
	    !CE_HasLabel (L[4])) {

	    CodeEntry* X;

       	    /* Store the high byte and remove the TAX instead */
	    X = NewCodeEntry (OP65_STA, AM65_ZP, "ptr1+1", 0, L[0]->LI);
	    CS_InsertEntry (S, X, I+1);
	    CS_DelEntry (S, I);

	    /* Store the low byte */
	    X = NewCodeEntry (OP65_STA, AM65_ZP, "ptr1", 0, L[2]->LI);
	    CS_InsertEntry (S, X, I+3);

	    /* Delete the call to ldauidx */
	    CS_DelEntry (S, I+5);

	    /* Load high and low byte */
	    X = NewCodeEntry (OP65_LDX, AM65_IMM, "$00", 0, L[3]->LI);
	    CS_InsertEntry (S, X, I+5);
	    X = NewCodeEntry (OP65_LDA, AM65_ZP_INDY, "ptr1", 0, L[3]->LI);
	    CS_InsertEntry (S, X, I+6);

	    /* Remember, we had changes */
	    ++Changes;

	}

	/* Next entry */
	++I;

    }

    /* Return the number of changes made */
    return Changes;
}



static unsigned OptPtrLoad2 (CodeSeg* S)
/* Search for the sequence:
 *
 *      adc    	xxx
 *      tay
 *      txa
 *      adc     yyy
 *      tax
 *      tya
 *      ldy
 *  	jsr    	ldauidx
 *
 * and replace it by:
 *
 *      adc    	xxx
 *      sta     ptr1
 *      txa
 *      adc     yyy
 *      sta     ptr1+1
 *      ldy
 *     	ldx     #$00
 *      lda     (ptr1),y
 */
{
    unsigned Changes = 0;

    /* Walk over the entries */
    unsigned I = 0;
    while (I < CS_GetEntryCount (S)) {

	CodeEntry* L[8];

      	/* Get next entry */
       	L[0] = CS_GetEntry (S, I);

     	/* Check for the sequence */
       	if (L[0]->OPC == OP65_ADC      	       	&&
       	    CS_GetEntries (S, L+1, I+1, 7)     	&&
       	    L[1]->OPC == OP65_TAY              	&&
	    !CE_HasLabel (L[1])        	       	&&
	    L[2]->OPC == OP65_TXA               &&
	    !CE_HasLabel (L[2])                 &&
	    L[3]->OPC == OP65_ADC               &&
	    !CE_HasLabel (L[3])                 &&
	    L[4]->OPC == OP65_TAX               &&
	    !CE_HasLabel (L[4])                 &&
	    L[5]->OPC == OP65_TYA               &&
	    !CE_HasLabel (L[5])                 &&
	    L[6]->OPC == OP65_LDY               &&
	    !CE_HasLabel (L[6])                 &&
	    L[7]->OPC == OP65_JSR               &&
       	    strcmp (L[7]->Arg, "ldauidx") == 0  &&
	    !CE_HasLabel (L[7])) {

	    CodeEntry* X;

       	    /* Store the low byte and remove the TAY instead */
	    X = NewCodeEntry (OP65_STA, AM65_ZP, "ptr1", 0, L[0]->LI);
	    CS_InsertEntry (S, X, I+1);
	    CS_DelEntry (S, I+2);

	    /* Store the high byte */
	    X = NewCodeEntry (OP65_STA, AM65_ZP, "ptr1+1", 0, L[3]->LI);
	    CS_InsertEntry (S, X, I+4);

	    /* Delete more transfer insns */
	    CS_DelEntry (S, I+6);
	    CS_DelEntry (S, I+5);

	    /* Delete the call to ldauidx */
	    CS_DelEntry (S, I+6);

	    /* Load high and low byte */
	    X = NewCodeEntry (OP65_LDX, AM65_IMM, "$00", 0, L[6]->LI);
	    CS_InsertEntry (S, X, I+6);
	    X = NewCodeEntry (OP65_LDA, AM65_ZP_INDY, "ptr1", 0, L[6]->LI);
	    CS_InsertEntry (S, X, I+7);

	    /* Remember, we had changes */
	    ++Changes;

	}

	/* Next entry */
	++I;

    }

    /* Return the number of changes made */
    return Changes;
}



static unsigned OptPtrLoad3 (CodeSeg* S)
/* Search for the sequence:
 *
 *      adc    	xxx
 *      pha
 *      txa
 *      iny
 *      adc     yyy
 *      tax
 *      pla
 *      ldy
 *  	jsr    	ldauidx
 *
 * and replace it by:
 *
 *      adc    	xxx
 *      sta     ptr1
 *      txa
 *      iny
 *      adc     yyy
 *      sta     ptr1+1
 *      ldy
 *     	ldx     #$00
 *      lda     (ptr1),y
 */
{
    unsigned Changes = 0;

    /* Walk over the entries */
    unsigned I = 0;
    while (I < CS_GetEntryCount (S)) {

	CodeEntry* L[9];

      	/* Get next entry */
       	L[0] = CS_GetEntry (S, I);

     	/* Check for the sequence */
       	if (L[0]->OPC == OP65_ADC      	       	&&
       	    CS_GetEntries (S, L+1, I+1, 8)     	&&
       	    L[1]->OPC == OP65_PHA              	&&
	    !CE_HasLabel (L[1])        	       	&&
	    L[2]->OPC == OP65_TXA               &&
	    !CE_HasLabel (L[2])                 &&
	    L[3]->OPC == OP65_INY               &&
	    !CE_HasLabel (L[3])                 &&
       	    L[4]->OPC == OP65_ADC               &&
	    !CE_HasLabel (L[4])                 &&
	    L[5]->OPC == OP65_TAX               &&
	    !CE_HasLabel (L[5])                 &&
	    L[6]->OPC == OP65_PLA               &&
	    !CE_HasLabel (L[6])                 &&
	    L[7]->OPC == OP65_LDY               &&
	    !CE_HasLabel (L[7])                 &&
	    L[8]->OPC == OP65_JSR               &&
       	    strcmp (L[8]->Arg, "ldauidx") == 0  &&
	    !CE_HasLabel (L[8])) {

	    CodeEntry* X;

       	    /* Store the low byte and remove the PHA instead */
	    X = NewCodeEntry (OP65_STA, AM65_ZP, "ptr1", 0, L[0]->LI);
	    CS_InsertEntry (S, X, I+1);
	    CS_DelEntry (S, I+2);

	    /* Store the high byte */
	    X = NewCodeEntry (OP65_STA, AM65_ZP, "ptr1+1", 0, L[4]->LI);
	    CS_InsertEntry (S, X, I+5);

	    /* Delete more transfer and PLA insns */
	    CS_DelEntry (S, I+7);
	    CS_DelEntry (S, I+6);

	    /* Delete the call to ldauidx */
	    CS_DelEntry (S, I+7);

	    /* Load high and low byte */
	    X = NewCodeEntry (OP65_LDX, AM65_IMM, "$00", 0, L[6]->LI);
	    CS_InsertEntry (S, X, I+7);
	    X = NewCodeEntry (OP65_LDA, AM65_ZP_INDY, "ptr1", 0, L[6]->LI);
	    CS_InsertEntry (S, X, I+8);

	    /* Remember, we had changes */
	    ++Changes;

	}

	/* Next entry */
	++I;

    }

    /* Return the number of changes made */
    return Changes;
}



static unsigned OptPtrLoad4 (CodeSeg* S)
/* Search for the sequence:
 *
 *      lda     #<(label+0)
 *      ldx     #>(label+0)
 *      clc
 *      adc     xxx
 *      bcc     L
 *      inx
 * L:   ldy     #$00
 *      jsr     ldauidx
 *
 * and replace it by:
 *
 *      ldy     xxx
 *      ldx     #$00
 *      lda     label,y
 */
{
    unsigned Changes = 0;

    /* Walk over the entries */
    unsigned I = 0;
    while (I < CS_GetEntryCount (S)) {

	CodeEntry* L[8];
	unsigned Len;

      	/* Get next entry */
       	L[0] = CS_GetEntry (S, I);

     	/* Check for the sequence */
       	if (L[0]->OPC == OP65_LDA      	       	             &&
	    L[0]->AM == AM65_IMM                             &&
       	    CS_GetEntries (S, L+1, I+1, 7)     	             &&
       	    L[1]->OPC == OP65_LDX              	             &&
	    L[1]->AM == AM65_IMM                             &&
	    !CE_HasLabel (L[1])        	       	             &&
	    L[2]->OPC == OP65_CLC                            &&
	    !CE_HasLabel (L[2])                              &&
	    L[3]->OPC == OP65_ADC                            &&
	    (L[3]->AM == AM65_ABS || L[3]->AM == AM65_ZP)    &&
	    !CE_HasLabel (L[3])                              &&
       	    (L[4]->OPC == OP65_BCC || L[4]->OPC == OP65_JCC) &&
       	    L[4]->JumpTo != 0                                &&
       	    L[4]->JumpTo->Owner == L[6]                      &&
	    !CE_HasLabel (L[4])                              &&
	    L[5]->OPC == OP65_INX                            &&
	    !CE_HasLabel (L[5])                              &&
	    L[6]->OPC == OP65_LDY                            &&
	    CE_KnownImm (L[6])                               &&
	    L[6]->Num == 0                                   &&
       	    L[7]->OPC == OP65_JSR                            &&
       	    strcmp (L[7]->Arg, "ldauidx") == 0               &&
	    !CE_HasLabel (L[7])                              &&
	    /* Check the label last because this is quite costly */
	    (Len = strlen (L[0]->Arg)) > 3                   &&
	    L[0]->Arg[0] == '<'                              &&
	    L[0]->Arg[1] == '('                              &&
	    strlen (L[1]->Arg) == Len                        &&
	    L[1]->Arg[0] == '>'                              &&
       	    memcmp (L[0]->Arg+1, L[1]->Arg+1, Len-1) == 0) {

	    CodeEntry* X;
	    char* Label;

	    /* We will create all the new stuff behind the current one so
	     * we keep the line references.
	     */
	    X = NewCodeEntry (OP65_LDY,	L[3]->AM, L[3]->Arg, 0, L[0]->LI);
	    CS_InsertEntry (S, X, I+8);

	    X = NewCodeEntry (OP65_LDX, AM65_IMM, "$00", 0, L[0]->LI);
	    CS_InsertEntry (S, X, I+9);

	    Label = memcpy (xmalloc (Len-2), L[0]->Arg+2, Len-3);
	    Label[Len-3] = '\0';
	    X = NewCodeEntry (OP65_LDA, AM65_ABSY, Label, 0, L[0]->LI);
	    CS_InsertEntry (S, X, I+10);
	    xfree (Label);

	    /* Remove the old code */
	    CS_DelEntries (S, I, 8);

	    /* Remember, we had changes */
	    ++Changes;

	}

	/* Next entry */
	++I;

    }

    /* Return the number of changes made */
    return Changes;
}



static unsigned OptPtrLoad5 (CodeSeg* S)
/* Search for the sequence:
 *
 *      lda     #<(label+0)
 *      ldx     #>(label+0)
 *      ldy     #$xx
 *      clc
 *      adc     (sp),y
 *      bcc     L
 *      inx
 * L:   ldy     #$00
 *      jsr     ldauidx
 *
 * and replace it by:
 *
 *      ldy     #$xx
 *      lda     (sp),y
 *      tay
 *      ldx     #$00
 *      lda     label,y
 */
{
    unsigned Changes = 0;

    /* Walk over the entries */
    unsigned I = 0;
    while (I < CS_GetEntryCount (S)) {

	CodeEntry* L[9];
	unsigned Len;

      	/* Get next entry */
       	L[0] = CS_GetEntry (S, I);

     	/* Check for the sequence */
       	if (L[0]->OPC == OP65_LDA      	       	             &&
	    L[0]->AM == AM65_IMM                             &&
       	    CS_GetEntries (S, L+1, I+1, 8)     	             &&
       	    L[1]->OPC == OP65_LDX              	             &&
	    L[1]->AM == AM65_IMM                             &&
	    !CE_HasLabel (L[1])        	       	             &&
	    L[2]->OPC == OP65_LDY                            &&
	    CE_KnownImm (L[2])                               &&
	    !CE_HasLabel (L[2])                              &&
	    L[3]->OPC == OP65_CLC                            &&
	    !CE_HasLabel (L[3])                              &&
	    L[4]->OPC == OP65_ADC                            &&
	    L[4]->AM == AM65_ZP_INDY                         &&
	    !CE_HasLabel (L[4])                              &&
       	    (L[5]->OPC == OP65_BCC || L[5]->OPC == OP65_JCC) &&
       	    L[5]->JumpTo != 0                                &&
       	    L[5]->JumpTo->Owner == L[7]                      &&
	    !CE_HasLabel (L[5])                              &&
	    L[6]->OPC == OP65_INX                            &&
	    !CE_HasLabel (L[6])                              &&
	    L[7]->OPC == OP65_LDY                            &&
	    CE_KnownImm (L[7])                               &&
	    L[7]->Num == 0                                   &&
       	    L[8]->OPC == OP65_JSR                            &&
       	    strcmp (L[8]->Arg, "ldauidx") == 0               &&
	    !CE_HasLabel (L[8])                              &&
	    /* Check the label last because this is quite costly */
	    (Len = strlen (L[0]->Arg)) > 3                   &&
	    L[0]->Arg[0] == '<'                              &&
	    L[0]->Arg[1] == '('                              &&
	    strlen (L[1]->Arg) == Len                        &&
	    L[1]->Arg[0] == '>'                              &&
       	    memcmp (L[0]->Arg+1, L[1]->Arg+1, Len-1) == 0) {

	    CodeEntry* X;
	    char* Label;

	    /* Add the lda */
	    X = NewCodeEntry (OP65_LDA, AM65_ZP_INDY, L[4]->Arg, 0, L[0]->LI);
	    CS_InsertEntry (S, X, I+3);

	    /* Add the tay */
	    X = NewCodeEntry (OP65_TAY, AM65_IMP, 0, 0, L[0]->LI);
	    CS_InsertEntry (S, X, I+4);

	    /* Add the ldx */
	    X = NewCodeEntry (OP65_LDX, AM65_IMM, "$00", 0, L[0]->LI);
	    CS_InsertEntry (S, X, I+5);

	    /* Add the lda */
	    Label = memcpy (xmalloc (Len-2), L[0]->Arg+2, Len-3);
	    Label[Len-3] = '\0';
	    X = NewCodeEntry (OP65_LDA, AM65_ABSY, Label, 0, L[0]->LI);
	    CS_InsertEntry (S, X, I+6);
	    xfree (Label);

	    /* Remove the old code */
	    CS_DelEntries (S, I, 2);
	    CS_DelEntries (S, I+5, 6);

	    /* Remember, we had changes */
	    ++Changes;

	}

	/* Next entry */
	++I;

    }

    /* Return the number of changes made */
    return Changes;
}



static unsigned OptPtrLoad6 (CodeSeg* S)
/* Search for the sequence
 *
 *      ldy     ...
 *      jsr     ldauidx
 *
 * and replace it by:
 *
 *      ldy     ...
 *      stx     ptr1+1
 *      sta     ptr1
 *      ldx     #$00
 *      lda     (ptr1),y
 *
 * This step must be execute *after* OptPtrLoad1!
 */
{
    unsigned Changes = 0;

    /* Walk over the entries */
    unsigned I = 0;
    while (I < CS_GetEntryCount (S)) {

	CodeEntry* L[2];

      	/* Get next entry */
       	L[0] = CS_GetEntry (S, I);

     	/* Check for the sequence */
       	if (L[0]->OPC == OP65_LDY      	       	&&
       	    CS_GetEntries (S, L+1, I+1, 1)     	&&
       	    L[1]->OPC == OP65_JSR               &&
       	    strcmp (L[1]->Arg, "ldauidx") == 0  &&
	    !CE_HasLabel (L[1])) {

	    CodeEntry* X;

       	    /* Store the high byte */
       	    X = NewCodeEntry (OP65_STA, AM65_ZP, "ptr1", 0, L[0]->LI);
	    CS_InsertEntry (S, X, I+1);

	    /* Store the low byte */
	    X = NewCodeEntry (OP65_STX, AM65_ZP, "ptr1+1", 0, L[0]->LI);
	    CS_InsertEntry (S, X, I+2);

	    /* Delete the call to ldauidx */
	    CS_DelEntry (S, I+3);

	    /* Load the high and low byte */
	    X = NewCodeEntry (OP65_LDX, AM65_IMM, "$00", 0, L[0]->LI);
	    CS_InsertEntry (S, X, I+3);
	    X = NewCodeEntry (OP65_LDA, AM65_ZP_INDY, "ptr1", 0, L[0]->LI);
	    CS_InsertEntry (S, X, I+4);

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
/*  				struct OptFunc                               */
/*****************************************************************************/



typedef struct OptFunc OptFunc;
struct OptFunc {
    unsigned     (*Func) (CodeSeg*);    /* Optimizer function */
    const char*  Name;                  /* Name of the function/group */
    char         Disabled;              /* True if function disabled */
};



/*****************************************************************************/
/*     	       	      	     	     Code	   	  		     */
/*****************************************************************************/



/* Macro that builds a function description */
#define OptFuncEntry(func) static OptFuncDesc D##func = { func, #func, 0 }

/* A list of all the function descriptions */
static OptFunc DOptPtrStore1   	= { OptPtrStore1,    "OptPtrStore1",    0 };
static OptFunc DOptPtrStore2   	= { OptPtrStore2,    "OptPtrStore2",    0 };
static OptFunc DOptPtrLoad1    	= { OptPtrLoad1,     "OptPtrLoad1",  	0 };
static OptFunc DOptPtrLoad2    	= { OptPtrLoad2,     "OptPtrLoad2",  	0 };
static OptFunc DOptPtrLoad3    	= { OptPtrLoad3,     "OptPtrLoad3",  	0 };
static OptFunc DOptPtrLoad4    	= { OptPtrLoad4,     "OptPtrLoad4",  	0 };
static OptFunc DOptPtrLoad5    	= { OptPtrLoad5,     "OptPtrLoad5",  	0 };
static OptFunc DOptPtrLoad6    	= { OptPtrLoad6,     "OptPtrLoad6",  	0 };
static OptFunc DOptNegA1       	= { OptNegA1,  	     "OptNegA1",     	0 };
static OptFunc DOptNegA2       	= { OptNegA2,  	     "OptNegA2",     	0 };
static OptFunc DOptNegAX1      	= { OptNegAX1,       "OptNegAX1",    	0 };
static OptFunc DOptNegAX2      	= { OptNegAX2,       "OptNegAX2",    	0 };
static OptFunc DOptNegAX3      	= { OptNegAX3,       "OptNegAX3",    	0 };
static OptFunc DOptNegAX4      	= { OptNegAX4,       "OptNegAX4",    	0 };
static OptFunc DOptSub1	       	= { OptSub1,   	     "OptSub1",      	0 };
static OptFunc DOptSub2	       	= { OptSub2,   	     "OptSub2",      	0 };
static OptFunc DOptAdd1	       	= { OptAdd1,   	     "OptAdd1",      	0 };
static OptFunc DOptAdd2	       	= { OptAdd2,   	     "OptAdd2",      	0 };
static OptFunc DOptAdd3	       	= { OptAdd3,   	     "OptAdd3",      	0 };
static OptFunc DOptShift1      	= { OptShift1,       "OptShift1",    	0 };
static OptFunc DOptJumpCascades	= { OptJumpCascades, "OptJumpCascades", 0 };
static OptFunc DOptDeadJumps   	= { OptDeadJumps,    "OptDeadJumps",    0 };
static OptFunc DOptRTS 	       	= { OptRTS,    	     "OptRTS",       	0 };
static OptFunc DOptDeadCode    	= { OptDeadCode,     "OptDeadCode",  	0 };
static OptFunc DOptJumpTarget  	= { OptJumpTarget,   "OptJumpTarget",   0 };
static OptFunc DOptCondBranches	= { OptCondBranches, "OptCondBranches", 0 };
static OptFunc DOptRTSJumps    	= { OptRTSJumps,     "OptRTSJumps",  	0 };
static OptFunc DOptBoolTrans    = { OptBoolTrans,    "OptBoolTrans",    0 };
static OptFunc DOptCmp1	       	= { OptCmp1,   	     "OptCmp1",      	0 };
static OptFunc DOptCmp2	       	= { OptCmp2,   	     "OptCmp2",      	0 };
static OptFunc DOptCmp3	       	= { OptCmp3,   	     "OptCmp3",      	0 };
static OptFunc DOptCmp4	       	= { OptCmp4,   	     "OptCmp4",      	0 };
static OptFunc DOptCmp5	       	= { OptCmp5,   	     "OptCmp5",      	0 };
static OptFunc DOptCmp6	       	= { OptCmp6,   	     "OptCmp6",      	0 };
static OptFunc DOptCmp7	       	= { OptCmp7,   	     "OptCmp7",      	0 };
static OptFunc DOptTest1       	= { OptTest1,  	     "OptTest1",     	0 };
static OptFunc DOptUnusedLoads 	= { OptUnusedLoads,  "OptUnusedLoads",  0 };
static OptFunc DOptUnusedStores	= { OptUnusedStores, "OptUnusedStores", 0 };
static OptFunc DOptDupLoads     = { OptDupLoads,     "OptDupLoads",     0 };
static OptFunc DOptStoreLoad   	= { OptStoreLoad,    "OptStoreLoad",    0 };
static OptFunc DOptTransfers   	= { OptTransfers,    "OptTransfers",    0 };
static OptFunc DOptStackOps    	= { OptStackOps,     "OptStackOps",  	0 };
static OptFunc DOptBranchDist  	= { OptBranchDist,   "OptBranchDist",   0 };

/* Table containing all the steps in alphabetical order */
static OptFunc* OptFuncs[] = {
    &DOptAdd1,
    &DOptAdd2,
    &DOptAdd3,
    &DOptBoolTrans,
    &DOptBranchDist,
    &DOptCmp1,
    &DOptCmp2,
    &DOptCmp3,
    &DOptCmp4,
    &DOptCmp5,
    &DOptCmp6,
    &DOptCmp7,
    &DOptCondBranches,
    &DOptDeadCode,
    &DOptDeadJumps,
    &DOptDupLoads,
    &DOptJumpCascades,
    &DOptJumpTarget,
    &DOptNegA1,
    &DOptNegA2,
    &DOptNegAX1,
    &DOptNegAX2,
    &DOptNegAX3,
    &DOptNegAX4,
    &DOptPtrStore1,
    &DOptPtrStore2,
    &DOptPtrLoad1,
    &DOptPtrLoad2,
    &DOptPtrLoad3,
    &DOptPtrLoad4,
    &DOptPtrLoad5,
    &DOptPtrLoad6,
    &DOptRTS,
    &DOptRTSJumps,
    &DOptSub1,
    &DOptSub2,
    &DOptShift1,
    &DOptStackOps,
    &DOptStoreLoad,
    &DOptTest1,
    &DOptTransfers,
    &DOptUnusedLoads,
    &DOptUnusedStores,
};
#define OPTFUNC_COUNT  (sizeof(OptFuncs) / sizeof(OptFuncs[0]))



static unsigned RunOptStep (CodeSeg* S, OptFunc* O, unsigned Max)
/* Run one optimizer function Max times or until there are no more changes */
{
    unsigned Changes, C;

    /* Don't run the function if it is disabled */
    if (O->Disabled) {
    	return 0;
    }

    /* Run this until there are no more changes */
    Changes = 0;
    do {
    	C = O->Func (S);
	Changes += C;
    } while (--Max && C > 0);

    /* Return the number of changes */
    return Changes;
}



static OptFunc* FindOptStep (const char* Name)
/* Find an optimizer step by name in the table and return a pointer. Print an
 * error and call AbEnd if not found.
 */
{
    unsigned I;

    /* Run all optimization steps */
    for (I = 0; I < OPTFUNC_COUNT; ++I) {
       	if (strcmp (OptFuncs[I]->Name, Name) == 0) {
    	    /* Found */
    	    return OptFuncs[I];
    	}
    }

    /* Not found */
    AbEnd ("Optimization step `%s' not found", Name);
    return 0;
}



void DisableOpt (const char* Name)
/* Disable the optimization with the given name */
{
    if (strcmp (Name, "any") == 0) {
	unsigned I;
       	for (I = 0; I < OPTFUNC_COUNT; ++I) {
       	    OptFuncs[I]->Disabled = 1;
    	}
    } else {
     	OptFunc* F = FindOptStep (Name);
     	F->Disabled = 1;
    }
}



void EnableOpt (const char* Name)
/* Enable the optimization with the given name */
{
    if (strcmp (Name, "any") == 0) {
	unsigned I;
       	for (I = 0; I < OPTFUNC_COUNT; ++I) {
       	    OptFuncs[I]->Disabled = 0;
	}
    } else {
     	OptFunc* F = FindOptStep (Name);
     	F->Disabled = 0;
    }
}



void ListOptSteps (FILE* F)
/* List all optimization steps */
{
    unsigned I;
    for (I = 0; I < OPTFUNC_COUNT; ++I) {
    	fprintf (F, "%s\n", OptFuncs[I]->Name);
    }
}



static void RunOptGroup1 (CodeSeg* S)
/* Run the first group of optimization steps. These steps translate known
 * patterns emitted by the code generator into more optimal patterns. Order
 * of the steps is important, because some of the steps done earlier cover
 * the same patterns as later steps as subpatterns.
 */
{
    RunOptStep (S, &DOptPtrStore1, 1);
    RunOptStep (S, &DOptPtrStore2, 1);
    RunOptStep (S, &DOptPtrLoad1, 1);
    RunOptStep (S, &DOptPtrLoad2, 1);
    RunOptStep (S, &DOptPtrLoad3, 1);
    RunOptStep (S, &DOptPtrLoad4, 1);
    RunOptStep (S, &DOptPtrLoad5, 1);
    RunOptStep (S, &DOptNegAX1, 1);
    RunOptStep (S, &DOptNegAX2, 1);
    RunOptStep (S, &DOptNegAX3, 1);
    RunOptStep (S, &DOptNegAX4, 1);
    RunOptStep (S, &DOptAdd1, 1);
    RunOptStep (S, &DOptAdd2, 1);
    RunOptStep (S, &DOptShift1, 1);
}



static void RunOptGroup2 (CodeSeg* S)
/* Run one group of optimization steps. These steps depend on each other,
 * that means that one step may allow another step to do additional work,
 * so we will repeat the steps as long as we see any changes.
 */
{
    unsigned Changes;

    do {
	Changes = 0;

	Changes += RunOptStep (S, &DOptPtrLoad6, 1);
	Changes += RunOptStep (S, &DOptNegA1, 1);
	Changes += RunOptStep (S, &DOptNegA2, 1);
	Changes += RunOptStep (S, &DOptSub1, 1);
	Changes += RunOptStep (S, &DOptSub2, 1);
	Changes += RunOptStep (S, &DOptAdd3, 1);
	Changes += RunOptStep (S, &DOptJumpCascades, 1);
	Changes += RunOptStep (S, &DOptDeadJumps, 1);
	Changes += RunOptStep (S, &DOptRTS, 1);
	Changes += RunOptStep (S, &DOptDeadCode, 1);
	Changes += RunOptStep (S, &DOptJumpTarget, 1);
	Changes += RunOptStep (S, &DOptCondBranches, 1);
	Changes += RunOptStep (S, &DOptRTSJumps, 1);
	Changes += RunOptStep (S, &DOptBoolTrans, 1);
	Changes += RunOptStep (S, &DOptCmp1, 1);
	Changes += RunOptStep (S, &DOptCmp2, 1);
	Changes += RunOptStep (S, &DOptCmp3, 1);
	Changes += RunOptStep (S, &DOptCmp4, 1);
	Changes += RunOptStep (S, &DOptCmp5, 1);
	Changes += RunOptStep (S, &DOptCmp6, 1);
	Changes += RunOptStep (S, &DOptCmp7, 1);
	Changes += RunOptStep (S, &DOptTest1, 1);
	Changes += RunOptStep (S, &DOptUnusedLoads, 1);
	Changes += RunOptStep (S, &DOptUnusedStores, 1);
	Changes += RunOptStep (S, &DOptDupLoads, 1);
	Changes += RunOptStep (S, &DOptStoreLoad, 1);
	Changes += RunOptStep (S, &DOptTransfers, 1);
	Changes += RunOptStep (S, &DOptStackOps, 1);

    } while (Changes);
}



static void RunOptGroup3 (CodeSeg* S)
/* The last group of optimization steps. Adjust branches.
 */
{
    RunOptStep (S, &DOptBranchDist, 3);
}



void RunOpt (CodeSeg* S)
/* Run the optimizer */
{

    /* If we shouldn't run the optimizer, bail out */
    if (!Optimize) {
    	return;
    }

    /* Print the name of the function we are working on */
    if (S->Func) {
     	Print (stdout, 1, "Running optimizer for function `%s'\n", S->Func->Name);
    } else {
     	Print (stdout, 1, "Running optimizer for global code segment\n");
    }

    /* Run groups of optimizations */
    RunOptGroup1 (S);
    RunOptGroup2 (S);
    RunOptGroup3 (S);
}



