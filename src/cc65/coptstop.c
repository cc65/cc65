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



#include <stdlib.h>

/* common */
#include "xsprintf.h"

/* cc65 */
#include "codeent.h"
#include "codeinfo.h"
#include "codeopt.h"
#include "error.h"
#include "coptstop.h"



/*****************************************************************************/
/*			       	    Helpers                                  */
/*****************************************************************************/



static unsigned Opt_staspidx (CodeSeg* S, unsigned Push, unsigned Store,
			      const char* ZPLo, const char* ZPHi)
/* Optimize the staspidx sequence if possible */
{
    CodeEntry* X;
    CodeEntry* PushEntry;
    CodeEntry* StoreEntry;

    /* Generate register info */
    CS_GenRegInfo (S);

    /* Get the push entry */
    PushEntry = CS_GetEntry (S, Push);

    /* Store the value into the zeropage instead of pushing it */
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



static unsigned Opt_tosaddax (CodeSeg* S, unsigned Push, unsigned Add,
			      const char* ZPLo, const char* ZPHi)
/* Optimize the tosaddax sequence if possible */
{
    CodeEntry* N;
    CodeEntry* X;
    CodeEntry* PushEntry;
    CodeEntry* AddEntry;

    /* We need the entry behind the add */
    if ((N = CS_GetNextEntry (S, Add)) == 0) {
	/* Unavailable */
	return 0;
    }

    /* Generate register info */
    CS_GenRegInfo (S);

    /* Get the push entry */
    PushEntry = CS_GetEntry (S, Push);

    /* Store the value into the zeropage instead of pushing it */
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



static unsigned Opt_tosandax (CodeSeg* S, unsigned Push, unsigned And,
		     	      const char* ZPLo, const char* ZPHi)
/* Optimize the tosandax sequence if possible */
{
    CodeEntry* X;
    CodeEntry* PushEntry;
    CodeEntry* AndEntry;

    /* Generate register info */
    CS_GenRegInfo (S);

    /* Get the push entry */
    PushEntry = CS_GetEntry (S, Push);

    /* Store the value into the zeropage instead of pushing it */
    X = NewCodeEntry (OP65_STA, AM65_ZP, ZPLo, 0, PushEntry->LI);
    CS_InsertEntry (S, X, Push+1);
    X = NewCodeEntry (OP65_STX, AM65_ZP, ZPHi, 0, PushEntry->LI);
    CS_InsertEntry (S, X, Push+2);

    /* Correct the index of the add and get a pointer to the entry */
    And += 2;
    AndEntry = CS_GetEntry (S, And);

    /* Inline the and */
    X = NewCodeEntry (OP65_AND, AM65_ZP, ZPLo, 0, AndEntry->LI);
    CS_InsertEntry (S, X, And+1);
    if (PushEntry->RI->In.RegX == 0 || AndEntry->RI->In.RegX == 0) {
     	/* The high byte is zero */
       	X = NewCodeEntry (OP65_LDX, AM65_IMM, "$00", 0, AndEntry->LI);
	CS_InsertEntry (S, X, And+2);
    } else {
     	/* High byte is unknown */
       	X = NewCodeEntry (OP65_STA, AM65_ZP, ZPLo, 0, AndEntry->LI);
     	CS_InsertEntry (S, X, And+2);
     	X = NewCodeEntry (OP65_TXA, AM65_IMP, 0, 0, AndEntry->LI);
     	CS_InsertEntry (S, X, And+3);
     	X = NewCodeEntry (OP65_AND, AM65_ZP, ZPHi, 0, AndEntry->LI);
     	CS_InsertEntry (S, X, And+4);
     	X = NewCodeEntry (OP65_TAX, AM65_IMP, 0, 0, AndEntry->LI);
     	CS_InsertEntry (S, X, And+5);
     	X = NewCodeEntry (OP65_LDA, AM65_ZP, ZPLo, 0, AndEntry->LI);
     	CS_InsertEntry (S, X, And+6);
    }

    /* Remove the push and the call to the tosandax function */
    CS_DelEntry (S, And);
    CS_DelEntry (S, Push);

    /* Free the register info */
    CS_FreeRegInfo (S);

    /* We changed the sequence */
    return 1;
}



static unsigned Opt_tosorax (CodeSeg* S, unsigned Push, unsigned Or,
		     	     const char* ZPLo, const char* ZPHi)
/* Optimize the tosorax sequence if possible */
{
    CodeEntry* X;
    CodeEntry* PushEntry;
    CodeEntry* OrEntry;

    /* Generate register info */
    CS_GenRegInfo (S);

    /* Get the push entry */
    PushEntry = CS_GetEntry (S, Push);

    /* Store the value into the zeropage instead of pushing it */
    X = NewCodeEntry (OP65_STA, AM65_ZP, ZPLo, 0, PushEntry->LI);
    CS_InsertEntry (S, X, Push+1);
    X = NewCodeEntry (OP65_STX, AM65_ZP, ZPHi, 0, PushEntry->LI);
    CS_InsertEntry (S, X, Push+2);

    /* Correct the index of the add and get a pointer to the entry */
    Or += 2;
    OrEntry = CS_GetEntry (S, Or);

    /* Inline the or */
    X = NewCodeEntry (OP65_ORA, AM65_ZP, ZPLo, 0, OrEntry->LI);
    CS_InsertEntry (S, X, Or+1);
    if (PushEntry->RI->In.RegX >= 0 && OrEntry->RI->In.RegX == 0) {
     	/* Value of X will be that of the first operand */
	char Buf [16];
	xsprintf (Buf, sizeof (Buf), "$%02X", PushEntry->RI->In.RegX);
       	X = NewCodeEntry (OP65_LDX, AM65_IMM, Buf, 0, OrEntry->LI);
	CS_InsertEntry (S, X, Or+2);
    } else if (PushEntry->RI->In.RegX != 0) {
     	/* High byte is unknown */
       	X = NewCodeEntry (OP65_STA, AM65_ZP, ZPLo, 0, OrEntry->LI);
     	CS_InsertEntry (S, X, Or+2);
     	X = NewCodeEntry (OP65_TXA, AM65_IMP, 0, 0, OrEntry->LI);
     	CS_InsertEntry (S, X, Or+3);
       	X = NewCodeEntry (OP65_ORA, AM65_ZP, ZPHi, 0, OrEntry->LI);
     	CS_InsertEntry (S, X, Or+4);
     	X = NewCodeEntry (OP65_TAX, AM65_IMP, 0, 0, OrEntry->LI);
     	CS_InsertEntry (S, X, Or+5);
     	X = NewCodeEntry (OP65_LDA, AM65_ZP, ZPLo, 0, OrEntry->LI);
     	CS_InsertEntry (S, X, Or+6);
    }

    /* Remove the push and the call to the tosandax function */
    CS_DelEntry (S, Or);
    CS_DelEntry (S, Push);

    /* Free the register info */
    CS_FreeRegInfo (S);

    /* We changed the sequence */
    return 1;
}



/*****************************************************************************/
/*  		      		     Code                                    */
/*****************************************************************************/



typedef unsigned (*OptFunc) (CodeSeg* S, unsigned Push, unsigned Store,
			     const char* ZPLo, const char* ZPHi);
typedef struct OptFuncDesc OptFuncDesc;
struct OptFuncDesc {
    const char*         Name;   /* Name of the replaced runtime function */
    OptFunc             Func;   /* Function pointer */
};

static const OptFuncDesc FuncTable[] = {
    { "staspidx",       Opt_staspidx    },
    { "tosaddax",       Opt_tosaddax    },
    { "tosandax",       Opt_tosandax    },
    { "tosorax",        Opt_tosorax     },
};
#define FUNC_COUNT (sizeof(FuncTable) / sizeof(FuncTable[0]))



static int CmpFunc (const void* Key, const void* Func)
/* Compare function for bsearch */
{
    return strcmp (Key, ((const	OptFuncDesc*) Func)->Name);
}



static const OptFuncDesc* FindFunc (const char* Name)
/* Find the function with the given name. Return a pointer to the table entry
 * or NULL if the function was not found.
 */
{
    return bsearch (Name, FuncTable, FUNC_COUNT, sizeof(OptFuncDesc), CmpFunc);
}



/*****************************************************************************/
/*  		      	     	     Code                                    */
/*****************************************************************************/



unsigned OptStackOps (CodeSeg* S)
/* Optimize operations that take operands via the stack */
{
    unsigned Changes = 0;     /* Number of changes in one run */
    int      InSeq = 0;       /* Inside a sequence */
    unsigned Push = 0; 	      /* Index of pushax */
    unsigned UsedRegs = 0;    /* Zeropage registers used in sequence */


    /* Look for a call to pushax followed by a call to some other function
     * that takes it's first argument on the stack, and the second argument
     * in the primary register.
     * It depends on the code between the two if we can handle/transform the
     * sequence, so check this code for the following list of things:
     *
     *  - there must not be a jump or conditional branch (this may
     *    get relaxed later).
     *  - there may not be accesses to local variables (may also be
     *    relaxed later)
     *  - no subroutine calls
     *  - no jump labels
     *
     * Since we need a zero page register later, do also check the
     * intermediate code for zero page use.
     */
    unsigned I = 0;
    while (I < CS_GetEntryCount (S)) {

	/* Get the next entry */
	CodeEntry* E = CS_GetEntry (S, I);

	/* Handling depends if we're inside a sequence or not */
	if (InSeq) {

	    /* Subroutine call? */
	    if (E->OPC == OP65_JSR) {

		/* Check if this is one of our functions */
		const OptFuncDesc* F = FindFunc (E->Arg);
		if (F) {

		    /* Determine the register to use */
		    const char* ZPLo;
		    const char* ZPHi;
       	       	    UsedRegs |= GetRegInfo (S, I+1, REG_SREG | REG_PTR1 | REG_PTR2);
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
		     	ZPLo = 0;
		     	ZPHi = 0;
		    }

		    /* If we have a register, call the optimizer function */
		    if (ZPLo && ZPHi) {
		     	Changes += F->Func (S, Push, I, ZPLo, ZPHi);
		    }

		    /* End of sequence */
		    InSeq = 0;

		} else if (strcmp (E->Arg, "pushax") == 0) {
		    /* Restart the sequence */
		    Push     = I;
		    UsedRegs = REG_NONE;
		} else {
		    /* A call to an unkown subroutine ends the sequence */
		    InSeq = 0;
		}

	    } else if ((E->Info & OF_BRA) != 0 ||
		       (E->Use & REG_SP) != 0  ||
		       CE_HasLabel (E)) {

		/* All this stuff is not allowed in a sequence */
		InSeq = 0;

	    } else {

		/* Other stuff: Track zeropage register usage */
		UsedRegs |= (E->Use | E->Chg);

	    }

	} else if (CE_IsCall (E, "pushax")) {

	    /* This starts a sequence */
	    Push     = I;
	    UsedRegs = REG_NONE;
	    InSeq    = 1;

	}

	/* Next entry */
	++I;

    }

    /* Return the number of changes made */
    return Changes;
}



