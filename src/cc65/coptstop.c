/*****************************************************************************/
/*                                                                           */
/*				   coptstop.c				     */
/*                                                                           */
/*	     Optimize operations that take operands via the stack            */
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



#include <stdlib.h>

/* cc65 */
#include "codeent.h"
#include "codeinfo.h"
#include "codeopt.h"
#include "error.h"
#include "coptstop.h"



/*****************************************************************************/
/*			       	    Helpers                                  */
/*****************************************************************************/



static unsigned AdjustStackOffset (CodeSeg* S, unsigned Start, unsigned Stop,
				   unsigned Offs)
/* Adjust the offset for all stack accesses in the range Start to Stop, both
 * inclusive. The function returns the number of instructions that have been
 * inserted.
 */
{
    /* Number of inserted instructions */
    unsigned Inserted = 0;

    /* Walk over all entries */
    unsigned I = Start;
    while (I <= Stop) {

     	CodeEntry* E = CS_GetEntry (S, I);

     	if (E->Use & REG_SP) {

     	    CodeEntry* P;

     	    /* Check for some things that should not happen */
     	    CHECK (E->AM == AM65_ZP_INDY || E->RI->In.RegY >= (short) Offs);
	    CHECK (strcmp (E->Arg, "sp") == 0);

     	    /* Get the code entry before this one. If it's a LDY, adjust the
     	     * value.
     	     */
     	    P = CS_GetPrevEntry (S, I);
     	    if (P && P->OPC == OP65_LDY && CE_KnownImm (P)) {

     		/* The Y load is just before the stack access, adjust it */
     		CE_SetNumArg (P, P->Num - Offs);

     	    } else {

     		/* Insert a new load instruction before the stack access */
     		const char* Arg = MakeHexArg (E->RI->In.RegY - Offs);
     		CodeEntry* X = NewCodeEntry (OP65_LDY, AM65_IMM, Arg, 0, E->LI);
     		CS_InsertEntry (S, X, I);

     		/* One more inserted entries */
     		++Inserted;
     		++Stop;

     		/* Be sure to skip the stack access for the next round */
     		++I;

     	    }

     	}

     	/* Next entry */
     	++I;
    }

    /* Return the number of inserted entries */
    return Inserted;
}



/*****************************************************************************/
/*   			 Actual optimization functions                       */
/*****************************************************************************/



static unsigned Opt_staspidx (CodeSeg* S, unsigned Push, unsigned Store,
     			      const char* ZPLo, const char* ZPHi)
/* Optimize the staspidx sequence if possible */
{
    CodeEntry* X;
    CodeEntry* PushEntry;
    CodeEntry* StoreEntry;

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

    /* We changed the sequence */
    return 1;
}



static unsigned Opt_staxspidx (CodeSeg* S, unsigned Push, unsigned Store,
     		       	       const char* ZPLo, const char* ZPHi)
/* Optimize the staxspidx sequence if possible */
{
    CodeEntry* X;
    CodeEntry* PushEntry;
    CodeEntry* StoreEntry;

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
    X = NewCodeEntry (OP65_INY, AM65_IMP, 0, 0, StoreEntry->LI);
    CS_InsertEntry (S, X, Store+2);
    if (StoreEntry->RI->In.RegX >= 0) {
	/* Value of X is known */
	const char* Arg = MakeHexArg (StoreEntry->RI->In.RegX);
       	X = NewCodeEntry (OP65_LDA, AM65_IMM, Arg, 0, StoreEntry->LI);
    } else {
     	/* Value unknown */
     	X = NewCodeEntry (OP65_TXA, AM65_IMP, 0, 0, StoreEntry->LI);
    }
    CS_InsertEntry (S, X, Store+3);
    X = NewCodeEntry (OP65_STA, AM65_ZP_INDY, ZPLo, 0, StoreEntry->LI);
    CS_InsertEntry (S, X, Store+4);

    /* Remove the push and the call to the staspidx function */
    CS_DelEntry (S, Store);
    CS_DelEntry (S, Push);

    /* We changed the sequence */
    return 1;
}



static unsigned Opt_tosaddax (CodeSeg* S, unsigned Push, unsigned Add,
			      const char* ZPLo, const char* ZPHi)
/* Optimize the tosaddax sequence if possible */
{
    CodeEntry* P;
    CodeEntry* N;
    CodeEntry* X;
    CodeEntry* PushEntry;
    CodeEntry* AddEntry;
    int        DirectAdd;


    /* We need the entry behind the add */
    CHECK ((N = CS_GetNextEntry (S, Add)) != 0);

    /* And the entry before the push */
    CHECK ((P = CS_GetPrevEntry (S, Push)) != 0);

    /* Get the push entry */
    PushEntry = CS_GetEntry (S, Push);

    /* Check the entry before the push, if it's a lda instruction with an
     * addressing mode that does not use an additional index register. If
     * so, we may use this location for the add and must not save the
     * value in the zero page location.
     */
    DirectAdd = (P->OPC == OP65_LDA &&
		 (P->AM == AM65_IMM || P->AM == AM65_ZP || P->AM == AM65_ABS));

    /* Store the value into the zeropage instead of pushing it */
    X = NewCodeEntry (OP65_STX, AM65_ZP, ZPHi, 0, PushEntry->LI);
    CS_InsertEntry (S, X, Push+1);
    ++Add;      /* Correct the index */
    if (!DirectAdd) {
     	X = NewCodeEntry (OP65_STA, AM65_ZP, ZPLo, 0, PushEntry->LI);
     	CS_InsertEntry (S, X, Push+1);
	++Add;	/* Correct the index */
    }

    /* Get a pointer to the add entry */
    AddEntry = CS_GetEntry (S, Add);

    /* Inline the add */
    X = NewCodeEntry (OP65_CLC, AM65_IMP, 0, 0, AddEntry->LI);
    CS_InsertEntry (S, X, Add+1);
    if (DirectAdd) {
	/* Add from variable location */
	X = NewCodeEntry (OP65_ADC, P->AM, P->Arg, 0, AddEntry->LI);
    } else {
	/* Add from temp storage */
	X = NewCodeEntry (OP65_ADC, AM65_ZP, ZPLo, 0, AddEntry->LI);
    }
    CS_InsertEntry (S, X, Add+2);
    if (PushEntry->RI->In.RegX == 0) {
     	/* The high byte is the value in X plus the carry */
     	CodeLabel* L = CS_GenLabel (S, N);
     	X = NewCodeEntry (OP65_BCC, AM65_BRA, L->Name, L, AddEntry->LI);
     	CS_InsertEntry (S, X, Add+3);
     	X = NewCodeEntry (OP65_INX, AM65_IMP, 0, 0, AddEntry->LI);
     	CS_InsertEntry (S, X, Add+4);
    } else if (AddEntry->RI->In.RegX == 0) {
     	/* The high byte is that of the first operand plus carry */
     	CodeLabel* L;
     	if (PushEntry->RI->In.RegX >= 0) {
     	    /* Value of first op high byte is known */
	    const char* Arg = MakeHexArg (PushEntry->RI->In.RegX);
	    X = NewCodeEntry (OP65_LDX, AM65_IMM, Arg, 0, AddEntry->LI);
	} else {
	    /* Value of first op high byte is unknown */
	    X = NewCodeEntry (OP65_LDX, AM65_ZP, ZPHi, 0, AddEntry->LI);
	}
	CS_InsertEntry (S, X, Add+3);
	L = CS_GenLabel (S, N);
	X = NewCodeEntry (OP65_BCC, AM65_BRA, L->Name, L, AddEntry->LI);
	CS_InsertEntry (S, X, Add+4);
	X = NewCodeEntry (OP65_INX, AM65_IMP, 0, 0, AddEntry->LI);
	CS_InsertEntry (S, X, Add+5);
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

    /* We changed the sequence */
    return 1;
}



static unsigned Opt_tosandax (CodeSeg* S, unsigned Push, unsigned And,
	    	     	      const char* ZPLo, const char* ZPHi)
/* Optimize the tosandax sequence if possible */
{
    CodeEntry* P;
    CodeEntry* X;
    CodeEntry* PushEntry;
    CodeEntry* AndEntry;
    int        DirectAnd;

    /* Get the entry before the push */
    CHECK ((P = CS_GetPrevEntry (S, Push)) != 0);

    /* Get the push entry */
    PushEntry = CS_GetEntry (S, Push);

    /* Check the entry before the push, if it's a lda instruction with an
     * addressing mode that does not use an additional index register. If
     * so, we may use this location for the and and must not save the
     * value in the zero page location.
     */
    DirectAnd = (P->OPC == OP65_LDA &&
	    	 (P->AM == AM65_IMM || P->AM == AM65_ZP || P->AM == AM65_ABS));

    /* Store the value into the zeropage instead of pushing it */
    X = NewCodeEntry (OP65_STX, AM65_ZP, ZPHi, 0, PushEntry->LI);
    CS_InsertEntry (S, X, Push+1);
    ++And;      /* Correct the index */
    if (!DirectAnd) {
	X = NewCodeEntry (OP65_STA, AM65_ZP, ZPLo, 0, PushEntry->LI);
	CS_InsertEntry (S, X, Push+1);
	++And;  /* Correct the index */
    }

    /* Get a pointer to the and entry */
    AndEntry = CS_GetEntry (S, And);

    /* Inline the and */
    if (DirectAnd) {
     	/* And with variable location */
	X = NewCodeEntry (OP65_AND, P->AM, P->Arg, 0, AndEntry->LI);
    } else {
     	/* And with temp storage */
     	X = NewCodeEntry (OP65_AND, AM65_ZP, ZPLo, 0, AndEntry->LI);
    }
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

    /* We changed the sequence */
    return 1;
}



static unsigned Opt_tosorax (CodeSeg* S, unsigned Push, unsigned Or,
     		     	     const char* ZPLo, const char* ZPHi)
/* Optimize the tosorax sequence if possible */
{
    CodeEntry* P;
    CodeEntry* X;
    CodeEntry* PushEntry;
    CodeEntry* OrEntry;
    int        DirectOr;

    /* Get the entry before the push */
    CHECK ((P = CS_GetPrevEntry (S, Push)) != 0);

    /* Get the push entry */
    PushEntry = CS_GetEntry (S, Push);

    /* Check the entry before the push, if it's a lda instruction with an
     * addressing mode that does not use an additional index register. If
     * so, we may use this location for the or and must not save the
     * value in the zero page location.
     */
    DirectOr = (P->OPC == OP65_LDA &&
     		(P->AM == AM65_IMM || P->AM == AM65_ZP || P->AM == AM65_ABS));

    /* Store the value into the zeropage instead of pushing it */
    X = NewCodeEntry (OP65_STX, AM65_ZP, ZPHi, 0, PushEntry->LI);
    CS_InsertEntry (S, X, Push+1);
    ++Or;  /* Correct the index */
    if (!DirectOr) {
     	X = NewCodeEntry (OP65_STA, AM65_ZP, ZPLo, 0, PushEntry->LI);
     	CS_InsertEntry (S, X, Push+1);
     	++Or;  /* Correct the index */
    }

    /* Get a pointer to the or entry */
    OrEntry = CS_GetEntry (S, Or);

    /* Inline the or */
    if (DirectOr) {
     	/* Or with variable location */
     	X = NewCodeEntry (OP65_ORA, P->AM, P->Arg, 0, OrEntry->LI);
    } else {
     	X = NewCodeEntry (OP65_ORA, AM65_ZP, ZPLo, 0, OrEntry->LI);
    }
    CS_InsertEntry (S, X, Or+1);
    if (PushEntry->RI->In.RegX >= 0 && OrEntry->RI->In.RegX >= 0) {
     	/* Both values known, precalculate the result */
	const char* Arg = MakeHexArg (PushEntry->RI->In.RegX | OrEntry->RI->In.RegX);
       	X = NewCodeEntry (OP65_LDX, AM65_IMM, Arg, 0, OrEntry->LI);
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

    /* We changed the sequence */
    return 1;
}



static unsigned Opt_tosxorax (CodeSeg* S, unsigned Push, unsigned Xor,
			      const char* ZPLo, const char* ZPHi)
/* Optimize the tosxorax sequence if possible */
{
    CodeEntry* P;
    CodeEntry* X;
    CodeEntry* PushEntry;
    CodeEntry* XorEntry;
    int        DirectXor;

    /* Get the entry before the push */
    CHECK ((P = CS_GetPrevEntry (S, Push)) != 0);

    /* Get the push entry */
    PushEntry = CS_GetEntry (S, Push);

    /* Check the entry before the push, if it's a lda instruction with an
     * addressing mode that does not use an additional index register. If
     * so, we may use this location for the xor and must not save the
     * value in the zero page location.
     */
    DirectXor = (P->OPC == OP65_LDA &&
	 	 (P->AM == AM65_IMM || P->AM == AM65_ZP || P->AM == AM65_ABS));

    /* Store the value into the zeropage instead of pushing it */
    X = NewCodeEntry (OP65_STX, AM65_ZP, ZPHi, 0, PushEntry->LI);
    CS_InsertEntry (S, X, Push+1);
    ++Xor;  /* Correct the index */
    if (!DirectXor) {
	X = NewCodeEntry (OP65_STA, AM65_ZP, ZPLo, 0, PushEntry->LI);
	CS_InsertEntry (S, X, Push+1);
	++Xor;  /* Correct the index */
    }

    /* Get a pointer to the entry */
    XorEntry = CS_GetEntry (S, Xor);

    /* Inline the xor */
    if (DirectXor) {
	/* Xor with variable location */
	X = NewCodeEntry (OP65_EOR, P->AM, P->Arg, 0, XorEntry->LI);
    } else {
	/* Xor with temp storage */
	X = NewCodeEntry (OP65_EOR, AM65_ZP, ZPLo, 0, XorEntry->LI);
    }
    CS_InsertEntry (S, X, Xor+1);
    if (PushEntry->RI->In.RegX >= 0 && XorEntry->RI->In.RegX >= 0) {
     	/* Both values known, precalculate the result */
     	const char* Arg = MakeHexArg (PushEntry->RI->In.RegX ^ XorEntry->RI->In.RegX);
       	X = NewCodeEntry (OP65_LDX, AM65_IMM, Arg, 0, XorEntry->LI);
     	CS_InsertEntry (S, X, Xor+2);
    } else if (PushEntry->RI->In.RegX != 0) {
     	/* High byte is unknown */
       	X = NewCodeEntry (OP65_STA, AM65_ZP, ZPLo, 0, XorEntry->LI);
     	CS_InsertEntry (S, X, Xor+2);
     	X = NewCodeEntry (OP65_TXA, AM65_IMP, 0, 0, XorEntry->LI);
     	CS_InsertEntry (S, X, Xor+3);
       	X = NewCodeEntry (OP65_EOR, AM65_ZP, ZPHi, 0, XorEntry->LI);
     	CS_InsertEntry (S, X, Xor+4);
     	X = NewCodeEntry (OP65_TAX, AM65_IMP, 0, 0, XorEntry->LI);
     	CS_InsertEntry (S, X, Xor+5);
     	X = NewCodeEntry (OP65_LDA, AM65_ZP, ZPLo, 0, XorEntry->LI);
     	CS_InsertEntry (S, X, Xor+6);
    }

    /* Remove the push and the call to the tosandax function */
    CS_DelEntry (S, Xor);
    CS_DelEntry (S, Push);

    /* We changed the sequence */
    return 1;
}



/*****************************************************************************/
/*   		      		     Code                                    */
/*****************************************************************************/



/* Flags for the functions */
typedef enum {
    STOP_NONE,		    /* Nothing special */
    STOP_A_UNUSED           /* Call only if a unused later */
} STOP_FLAGS;


typedef unsigned (*OptFunc) (CodeSeg* S, unsigned Push, unsigned Store,
     	    		     const char* ZPLo, const char* ZPHi);
typedef struct OptFuncDesc OptFuncDesc;
struct OptFuncDesc {
    const char*     Name;   /* Name of the replaced runtime function */
    OptFunc         Func;   /* Function pointer */
    STOP_FLAGS      Flags;  /* Flags */
};

static const OptFuncDesc FuncTable[] = {
    { "staspidx",   Opt_staspidx,  STOP_NONE },
    { "staxspidx",  Opt_staxspidx, STOP_A_UNUSED },
    { "tosaddax",   Opt_tosaddax,  STOP_NONE },
    { "tosandax",   Opt_tosandax,  STOP_NONE },
    { "tosorax",    Opt_tosorax,   STOP_NONE },
    { "tosxorax",   Opt_tosxorax,  STOP_NONE },
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
    unsigned I;

    /* Generate register info */
    CS_GenRegInfo (S);

    /* Look for a call to pushax followed by a call to some other function
     * that takes it's first argument on the stack, and the second argument
     * in the primary register.
     * It depends on the code between the two if we can handle/transform the
     * sequence, so check this code for the following list of things:
     *
     *  - there must not be a jump or conditional branch (this may
     *    get relaxed later).
     *  - there may not be accesses to local variables with unknown
     *    offsets (because we have to adjust these offsets).
     *  - no subroutine calls
     *  - no jump labels
     *
     * Since we need a zero page register later, do also check the
     * intermediate code for zero page use.
     */
    I = 0;
    while (I < CS_GetEntryCount (S)) {

	/* Get the next entry */
	CodeEntry* E = CS_GetEntry (S, I);

	/* Handling depends if we're inside a sequence or not */
	if (InSeq) {

	    if ((E->Info & OF_BRA) != 0                              ||
		((E->Use & REG_SP) != 0                         &&
		 (E->AM != AM65_ZP_INDY || E->RI->In.RegY < 0))      ||
		CE_HasLabel (E)) {

	    	/* All this stuff is not allowed in a sequence */
	    	InSeq = 0;

	    } else if (E->OPC == OP65_JSR) {

       	       	/* Subroutine call: Check if this is one of our functions */
	    	const OptFuncDesc* F = FindFunc (E->Arg);
	    	if (F) {

	    	    const char* ZPLo = 0;
	    	    const char* ZPHi = 0;
		    int PreCondOk    = 1;

		    /* Check the flags */
		    if (F->Flags & STOP_A_UNUSED) {
			/* a must be unused later */
			if (RegAUsed (S, I+1)) {
			    /* Cannot optimize */
			    PreCondOk = 0;
			}
		    }

	    	    /* Determine the zero page locations to use */
		    if (PreCondOk) {
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
		     	    PreCondOk = 0;
		     	}
		    }

	    	    /* If preconditions are ok, call the optimizer function */
	    	    if (PreCondOk) {

	    		/* Adjust stack offsets */
	    		unsigned Op = I + AdjustStackOffset (S, Push, I, 2);

	    		/* Call the optimizer function */
	    	     	Changes += F->Func (S, Push, Op, ZPLo, ZPHi);

	    		/* Regenerate register info */
	    		CS_GenRegInfo (S);
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

    /* Free the register info */
    CS_FreeRegInfo (S);

    /* Return the number of changes made */
    return Changes;
}



