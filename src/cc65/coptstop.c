/*****************************************************************************/
/*                                                                           */
/*				   coptstop.c				     */
/*                                                                           */
/*	     Optimize operations that take operands via the stack            */
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
#include "codeopt.h"
#include "error.h"
#include "coptstop.h"



/*****************************************************************************/
/*			       	    Helpers                                  */
/*****************************************************************************/



static unsigned Opt_tosaddax (CodeSeg* S, unsigned Push, unsigned Add)
/* Optimize the tosaddax sequence if possible */
{
    unsigned I;
    CodeEntry* N;
    CodeEntry* X;
    CodeEntry* PushEntry;
    CodeEntry* AddEntry;
    const char* ZPLo;
    const char* ZPHi;

    /* Check if the sequence is safe. This means that there may not be any
     * jumps between the two data points, and no usage of the stack. Handling
     * these conditions is possible and may be done later.
     */
    unsigned UsedRegs = REG_NONE;
    for (I = Push + 1; I < Add; ++I) {
    	CodeEntry* E = CS_GetEntry (S, I);
    	if ((E->Info & OF_BRA) != 0 ||
	    E->OPC == OP65_JSR      ||
    	    (E->Use & REG_SP) != 0  ||
    	    CE_HasLabel (E)) {
    	    /* A jump or stack pointer usage - bail out */
    	    return 0;
    	}
	UsedRegs |= (E->Use | E->Chg);
    }

    /* We prefer usage of sreg for the intermediate value, since sreg is
     * tracked and optimized.
     */
    UsedRegs |= GetRegInfo (S, Push+1, REG_ALL);
    if ((UsedRegs & REG_SREG) == REG_NONE) {
     	/* SREG is available */
	ZPLo = "sreg";
	ZPHi = "sreg+1";
    } else if ((UsedRegs & REG_PTR1) == REG_NONE) {
	ZPLo = "ptr1";
	ZPHi = "ptr1+1";
    } else if ((UsedRegs & REG_PTR2) == REG_NONE) {
	ZPLo = "ptr2";
	ZPHi = "ptr2+1";
    } else {
	/* No registers available */
     	return 0;
    }

    /* We need the entry behind the add */
    if ((N = CS_GetNextEntry (S, Add)) == 0) {
	/* Unavailable */
	return 0;
    }

    /* Generate register info */
    CS_GenRegInfo (S);

    /* Get the push entry */
    PushEntry = CS_GetEntry (S, Push);

    /* Store the value into sreg instead of pushing it */
    X = NewCodeEntry (OP65_STA, AM65_ZP, ZPLo, 0, PushEntry->LI);
    CS_InsertEntry (S, X, Push+1);
    X = NewCodeEntry (OP65_STX, AM65_ZP, ZPHi, 0, PushEntry->LI);
    CS_InsertEntry (S, X, Push+2);

    /* Correct the index of the add and get a pointer to the entry */
    Add += 2;
    AddEntry = CS_GetEntry (S, Add);

    /* Inline the add */
    X = NewCodeEntry (OP65_CLC, AM65_IMP, 0, 0, AddEntry->LI);
    CS_InsertEntry (S, X, Add+1);
    X = NewCodeEntry (OP65_ADC, AM65_ZP, ZPLo, 0, AddEntry->LI);
    CS_InsertEntry (S, X, Add+2);
    if (PushEntry->RI->In.RegX == 0 && AddEntry->RI->In.RegX == 0) {
     	/* The high byte is zero on entry */
	CodeLabel* L = CS_GenLabel (S, N);
	X = NewCodeEntry (OP65_BCC, AM65_BRA, L->Name, L, AddEntry->LI);
	CS_InsertEntry (S, X, Add+3);
	X = NewCodeEntry (OP65_INX, AM65_IMP, 0, 0, AddEntry->LI);
	CS_InsertEntry (S, X, Add+4);
    } else {
     	/* High byte is unknown */
     	X = NewCodeEntry (OP65_STA, AM65_ZP, ZPLo, 0, AddEntry->LI);
     	CS_InsertEntry (S, X, Add+3);
     	X = NewCodeEntry (OP65_TXA, AM65_IMP, 0, 0, AddEntry->LI);
     	CS_InsertEntry (S, X, Add+4);
     	X = NewCodeEntry (OP65_ADC, AM65_ZP, ZPHi, 0, AddEntry->LI);
     	CS_InsertEntry (S, X, Add+5);
     	X = NewCodeEntry (OP65_TAX, AM65_IMP, 0, 0, AddEntry->LI);
     	CS_InsertEntry (S, X, Add+6);
     	X = NewCodeEntry (OP65_LDA, AM65_ZP, ZPLo, 0, AddEntry->LI);
     	CS_InsertEntry (S, X, Add+7);
    }

    /* Remove the push and the call to the tosaddax function */
    CS_DelEntry (S, Add);
    CS_DelEntry (S, Push);

    /* Free the register info */
    CS_FreeRegInfo (S);

    /* We changed the sequence */
    return 1;
}



static unsigned Opt_staspidx (CodeSeg* S, unsigned Push, unsigned Store)
/* Optimize the staspidx sequence if possible */
{
    unsigned I;
    CodeEntry* N;
    CodeEntry* X;
    CodeEntry* PushEntry;
    CodeEntry* StoreEntry;
    const char* ZPLo;
    const char* ZPHi;

    /* Check if the sequence is safe. This means that there may not be any
     * jumps between the two data points, and no usage of the stack. Handling
     * these conditions is possible and may be done later.
     */
    unsigned UsedRegs = REG_NONE;
    for (I = Push + 1; I < Store; ++I) {
    	CodeEntry* E = CS_GetEntry (S, I);
    	if ((E->Info & OF_BRA) != 0 ||
	    E->OPC == OP65_JSR      ||
    	    (E->Use & REG_SP) != 0  ||
    	    CE_HasLabel (E)) {
    	    /* A jump or stack pointer usage - bail out */
    	    return 0;
    	}
	UsedRegs |= (E->Use | E->Chg);
    }

    /* We prefer usage of sreg for the intermediate value, since sreg is
     * tracked and optimized.
     */
    UsedRegs |= GetRegInfo (S, Push+1, REG_SREG | REG_PTR1 | REG_PTR2);
    if ((UsedRegs & REG_SREG) == REG_NONE) {
     	/* SREG is available */
	ZPLo = "sreg";
	ZPHi = "sreg+1";
    } else if ((UsedRegs & REG_PTR1) == REG_NONE) {
	ZPLo = "ptr1";
	ZPHi = "ptr1+1";
    } else if ((UsedRegs & REG_PTR2) == REG_NONE) {
	ZPLo = "ptr2";
	ZPHi = "ptr2+1";
    } else {
	/* No registers available */
     	return 0;
    }

    /* We need the entry behind the store */
    if ((N = CS_GetNextEntry (S, Store)) == 0) {
	/* Unavailable */
	return 0;
    }

    /* Generate register info */
    CS_GenRegInfo (S);

    /* Get the push entry */
    PushEntry = CS_GetEntry (S, Push);

    /* Store the value into sreg instead of pushing it */
    X = NewCodeEntry (OP65_STA, AM65_ZP, ZPLo, 0, PushEntry->LI);
    CS_InsertEntry (S, X, Push+1);
    X = NewCodeEntry (OP65_STX, AM65_ZP, ZPHi, 0, PushEntry->LI);
    CS_InsertEntry (S, X, Push+2);

    /* Correct the index of the store and get a pointer to the entry */
    Store += 2;
    StoreEntry = CS_GetEntry (S, Store);

    /* Inline the store */
    X = NewCodeEntry (OP65_STA, AM65_ZP_INDY, ZPLo, 0, StoreEntry->LI);
    CS_InsertEntry (S, X, Store+1);

    /* Remove the push and the call to the staspidx function */
    CS_DelEntry (S, Store);
    CS_DelEntry (S, Push);

    /* Free the register info */
    CS_FreeRegInfo (S);

    /* We changed the sequence */
    return 1;
}



/*****************************************************************************/
/*  		      		     Code                                    */
/*****************************************************************************/



unsigned OptStackOps (CodeSeg* S)
/* Optimize operations that take operands via the stack */
{
    unsigned Changes = 0;          /* Number of changes in one run */
    int LastPush = -1;             /* Last call to pushax */

    /* Walk over all entries */
    unsigned I = 0;
    while (I < CS_GetEntryCount (S)) {

	/* Get the next entry */
	CodeEntry* E = CS_GetEntry (S, I);

	/* Check for a subroutine call */
	if (E->OPC == OP65_JSR) {

	    /* We look for two things: A call to pushax, and a call to one
	     * of the known functions we're going to replace. We're only
	     * interested in the latter ones, if we had a push before.
	     */
	    if (strcmp (E->Arg, "pushax") == 0) {

		/* Just remember it */
		LastPush = I;

	    } else if (LastPush >= 0) {

		if (strcmp (E->Arg, "tosaddax") == 0) {
  		    Changes += Opt_tosaddax (S, LastPush, I);
		    LastPush = -1;
  		} else if (strcmp (E->Arg, "staspidx") == 0) {
		    Changes += Opt_staspidx (S, LastPush, I);
		    LastPush = -1;
		}

	    }
	}

	/* Next entry */
	++I;

    }

    /* Return the number of changes made */
    return Changes;
}



