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
/*                                   Data                                    */
/*****************************************************************************/



/* Structure that holds the needed data */
typedef struct StackOpData StackOpData;
struct StackOpData {
    CodeSeg*    Code;                   /* Pointer to code segment */
    unsigned    Flags;                  /* Flags to remember things */
    unsigned    PushIndex;              /* Index of call to pushax in codeseg */
    unsigned    OpIndex;                /* Index of actual operation */
    CodeEntry*  PrevEntry;              /* Entry before the call to pushax */
    CodeEntry*  PushEntry;              /* Pointer to entry with call to pushax */
    CodeEntry*  OpEntry;                /* Pointer to entry with op */
    CodeEntry*  NextEntry;              /* Entry after the op */
    const char* ZPLo;                   /* Lo byte of zero page loc to use */
    const char* ZPHi;                   /* Hi byte of zero page loc to use */
    unsigned    IP;                     /* Insertion point used by some routines */
};

/* Flags returned by DirectOp */
#define OP_DIRECT       0x01            /* Direct op may be used */
#define OP_ONSTACK      0x02            /* Operand is on stack */



/*****************************************************************************/
/*     		      	       	    Helpers                                  */
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



static void InsertEntry (StackOpData* D, CodeEntry* E, unsigned Index)
/* Insert a new entry. Depending on Index, D->PushIndex and D->OpIndex will
 * be adjusted by this function.
 */
{
    /* Insert the entry into the code segment */
    CS_InsertEntry (D->Code, E, Index);

    /* Adjust the indices if necessary */
    if (D->PushEntry && Index <= D->PushIndex) {
        ++D->PushIndex;
    }
    if (D->OpEntry && Index <= D->OpIndex) {
        ++D->OpIndex;
    }
}



static void DelEntry (StackOpData* D, unsigned Index)
/* Delete an entry. Depending on Index, D->PushIndex and D->OpIndex will be
 * adjusted by this function, and PushEntry/OpEntry may get invalidated.
 */
{
    /* Delete the entry from the code segment */
    CS_DelEntry (D->Code, Index);

    /* Adjust the indices if necessary */
    if (Index < D->PushIndex) {
        --D->PushIndex;
    } else if (Index == D->PushIndex) {
        D->PushEntry = 0;
    }
    if (Index < D->OpIndex) {
        --D->OpIndex;
    } else if (Index == D->OpIndex) {
        D->OpEntry = 0;
    }
}



static void CheckDirectOp (StackOpData* D)
/* Check if the given entry is a lda instruction with an addressing mode
 * that allows us to replace it by another operation (like ora). If so, we may
 * use this location for the or and must not save the value in the zero
 * page location.
 */
{
    /* We need the entry before the push */
    CodeEntry* E;
    CHECK ((E = D->PrevEntry) != 0);

    if (E->OPC == OP65_LDA) {
        if (E->AM == AM65_IMM || E->AM == AM65_ZP || E->AM == AM65_ABS) {
            /* These insns are all ok and replaceable */
            D->Flags |= OP_DIRECT;
        } else if (E->AM == AM65_ZP_INDY &&
                   E->RI->In.RegY >= 0   &&
       	       	   (E->Use & REG_SP) != 0) {
            /* Load from stack with known offset is also ok */
            D->Flags |= (OP_DIRECT | OP_ONSTACK);
        }
    }
}



static void ReplacePushByStore (StackOpData* D)
/* Replace the call to the push subroutine by a store into the zero page
 * location (actually, the push is not replaced, because we need it for
 * later, but the name is still ok since the push will get removed at the
 * end of each routine.
 */
{
    CodeEntry* X;

    /* Store the value into the zeropage instead of pushing it */
    X = NewCodeEntry (OP65_STX, AM65_ZP, D->ZPHi, 0, D->PushEntry->LI);
    InsertEntry (D, X, D->PushIndex+1);
    if ((D->Flags & OP_DIRECT) == 0) {
     	X = NewCodeEntry (OP65_STA, AM65_ZP, D->ZPLo, 0, D->PushEntry->LI);
       	InsertEntry (D, X, D->PushIndex+1);
    }
}



static void AddOpLow (StackOpData* D, opc_t OPC)
/* Add an op for the low byte of an operator. This function honours the
 * OP_DIRECT and OP_ONSTACK flags and generates the necessary instructions.
 * All code is inserted at the current insertion point.
 */
{
    CodeEntry* X;

    if ((D->Flags & OP_DIRECT) != 0) {
       	/* Op with a variable location. If the location is on the stack, we
         * need to reload the Y register.
         */
        if ((D->Flags & OP_ONSTACK) != 0) {
            const char* Arg = MakeHexArg (D->PrevEntry->RI->In.RegY);
            X = NewCodeEntry (OP65_LDY, AM65_IMM, Arg, 0, D->OpEntry->LI);
            InsertEntry (D, X, D->IP++);
        }
	X = NewCodeEntry (OPC, D->PrevEntry->AM, D->PrevEntry->Arg, 0, D->OpEntry->LI);
    } else {
	/* Op with temp storage */
	X = NewCodeEntry (OPC, AM65_ZP, D->ZPLo, 0, D->OpEntry->LI);
    }
    InsertEntry (D, X, D->IP++);
}



static void AddOpHigh (StackOpData* D, opc_t OPC)
/* Add an op for the high byte of an operator. Special cases (constant values
 * or similar have to be checked separately, the function covers only the
 * generic case. Code is inserted at the insertion point.
 */
{
    CodeEntry* X;

    /* High byte is unknown */
    X = NewCodeEntry (OP65_STA, AM65_ZP, D->ZPLo, 0, D->OpEntry->LI);
    InsertEntry (D, X, D->IP++);
    X = NewCodeEntry (OP65_TXA, AM65_IMP, 0, 0, D->OpEntry->LI);
    InsertEntry (D, X, D->IP++);
    X = NewCodeEntry (OPC, AM65_ZP, D->ZPHi, 0, D->OpEntry->LI);
    InsertEntry (D, X, D->IP++);
    X = NewCodeEntry (OP65_TAX, AM65_IMP, 0, 0, D->OpEntry->LI);
    InsertEntry (D, X, D->IP++);
    X = NewCodeEntry (OP65_LDA, AM65_ZP, D->ZPLo, 0, D->OpEntry->LI);
    InsertEntry (D, X, D->IP++);
}



static void RemovePushAndOp (StackOpData* D)
/* Remove the call to pushax and the call to the operator subroutine */
{
    DelEntry (D, D->OpIndex);
    DelEntry (D, D->PushIndex);
}



/*****************************************************************************/
/*   	       		 Actual optimization functions                       */
/*****************************************************************************/



static unsigned Opt_staspidx (StackOpData* D)
/* Optimize the staspidx sequence if possible */
{
    CodeEntry* X;

    /* Store the value into the zeropage instead of pushing it */
    ReplacePushByStore (D);

    /* Replace the store subroutine call by a direct op */
    X = NewCodeEntry (OP65_STA, AM65_ZP_INDY, D->ZPLo, 0, D->OpEntry->LI);
    InsertEntry (D, X, D->OpIndex+1);

    /* Remove the push and the call to the staspidx function */
    RemovePushAndOp (D);

    /* We changed the sequence */
    return 1;
}



static unsigned Opt_staxspidx (StackOpData* D)
/* Optimize the staxspidx sequence if possible */
{
    CodeEntry* X;

    /* Store the value into the zeropage instead of pushing it */
    ReplacePushByStore (D);

    /* Inline the store */
    X = NewCodeEntry (OP65_STA, AM65_ZP_INDY, D->ZPLo, 0, D->OpEntry->LI);
    InsertEntry (D, X, D->OpIndex+1);
    X = NewCodeEntry (OP65_INY, AM65_IMP, 0, 0, D->OpEntry->LI);
    InsertEntry (D, X, D->OpIndex+2);
    if (D->OpEntry->RI->In.RegX >= 0) {
	/* Value of X is known */
	const char* Arg = MakeHexArg (D->OpEntry->RI->In.RegX);
       	X = NewCodeEntry (OP65_LDA, AM65_IMM, Arg, 0, D->OpEntry->LI);
    } else {
     	/* Value unknown */
     	X = NewCodeEntry (OP65_TXA, AM65_IMP, 0, 0, D->OpEntry->LI);
    }
    InsertEntry (D, X, D->OpIndex+3);
    X = NewCodeEntry (OP65_STA, AM65_ZP_INDY, D->ZPLo, 0, D->OpEntry->LI);
    InsertEntry (D, X, D->OpIndex+4);

    /* Remove the push and the call to the staspidx function */
    RemovePushAndOp (D);

    /* We changed the sequence */
    return 1;
}



static unsigned Opt_tosaddax (StackOpData* D)
/* Optimize the tosaddax sequence if possible */
{
    CodeEntry*  X;


    /* We need the entry behind the add */
    CHECK (D->NextEntry != 0);

    /* Check the entry before the push. If it's a lda instruction with an
     * addressing mode that allows us to replace it, we may use this
     * location for the op and must not save the value in the zero page
     * location.
     */
    CheckDirectOp (D);

    /* Store the value into the zeropage instead of pushing it */
    ReplacePushByStore (D);

    /* Inline the add */
    D->IP = D->OpIndex+1;
    X = NewCodeEntry (OP65_CLC, AM65_IMP, 0, 0, D->OpEntry->LI);
    InsertEntry (D, X, D->IP++);

    /* Low byte */
    AddOpLow (D, OP65_ADC);

    /* High byte */
    if (D->PushEntry->RI->In.RegX == 0) {
     	/* The high byte is the value in X plus the carry */
     	CodeLabel* L = CS_GenLabel (D->Code, D->NextEntry);
     	X = NewCodeEntry (OP65_BCC, AM65_BRA, L->Name, L, D->OpEntry->LI);
     	InsertEntry (D, X, D->IP++);
     	X = NewCodeEntry (OP65_INX, AM65_IMP, 0, 0, D->OpEntry->LI);
     	InsertEntry (D, X, D->IP++);
    } else if (D->OpEntry->RI->In.RegX == 0) {
       	/* The high byte is that of the first operand plus carry */
     	CodeLabel* L;
     	if (D->PushEntry->RI->In.RegX >= 0) {
     	    /* Value of first op high byte is known */
	    const char* Arg = MakeHexArg (D->PushEntry->RI->In.RegX);
	    X = NewCodeEntry (OP65_LDX, AM65_IMM, Arg, 0, D->OpEntry->LI);
	} else {
	    /* Value of first op high byte is unknown */
	    X = NewCodeEntry (OP65_LDX, AM65_ZP, D->ZPHi, 0, D->OpEntry->LI);
	}
	InsertEntry (D, X, D->IP++);
	L = CS_GenLabel (D->Code, D->NextEntry);
	X = NewCodeEntry (OP65_BCC, AM65_BRA, L->Name, L, D->OpEntry->LI);
	InsertEntry (D, X, D->IP++);
	X = NewCodeEntry (OP65_INX, AM65_IMP, 0, 0, D->OpEntry->LI);
	InsertEntry (D, X, D->IP++);
    } else {
     	/* High byte is unknown */
        AddOpHigh (D, OP65_ADC);
    }

    /* Remove the push and the call to the tosaddax function */
    RemovePushAndOp (D);

    /* We changed the sequence */
    return 1;
}



static unsigned Opt_tosandax (StackOpData* D)
/* Optimize the tosandax sequence if possible */
{
    CodeEntry*  X;

    /* Check the entry before the push. If it's a lda instruction with an
     * addressing mode that allows us to replace it, we may use this
     * location for the op and must not save the value in the zero page
     * location.
     */
    CheckDirectOp (D);

    /* Store the value into the zeropage instead of pushing it */
    ReplacePushByStore (D);

    /* Inline the and, low byte */
    D->IP = D->OpIndex + 1;
    AddOpLow (D, OP65_AND);

    /* High byte */
    if (D->PushEntry->RI->In.RegX == 0 || D->OpEntry->RI->In.RegX == 0) {
     	/* The high byte is zero */
       	X = NewCodeEntry (OP65_LDX, AM65_IMM, "$00", 0, D->OpEntry->LI);
	InsertEntry (D, X, D->IP++);
    } else {
     	/* High byte is unknown */
        AddOpHigh (D, OP65_AND);
    }

    /* Remove the push and the call to the tosandax function */
    RemovePushAndOp (D);

    /* We changed the sequence */
    return 1;
}



static unsigned Opt_tosorax (StackOpData* D)
/* Optimize the tosorax sequence if possible */
{
    CodeEntry*  X;

    /* Check the entry before the push. If it's a lda instruction with an
     * addressing mode that allows us to replace it, we may use this
     * location for the op and must not save the value in the zero page
     * location.
     */
    CheckDirectOp (D);

    /* Store the value into the zeropage instead of pushing it */
    ReplacePushByStore (D);

    /* Inline the or, low byte */
    D->IP = D->OpIndex + 1;
    AddOpLow (D, OP65_ORA);

    /* High byte */
    if (D->PushEntry->RI->In.RegX >= 0 && D->OpEntry->RI->In.RegX >= 0) {
     	/* Both values known, precalculate the result */
	const char* Arg = MakeHexArg (D->PushEntry->RI->In.RegX | D->OpEntry->RI->In.RegX);
       	X = NewCodeEntry (OP65_LDX, AM65_IMM, Arg, 0, D->OpEntry->LI);
	InsertEntry (D, X, D->IP++);
    } else if (D->PushEntry->RI->In.RegX != 0) {
     	/* High byte is unknown */
        AddOpHigh (D, OP65_ORA);
    }

    /* Remove the push and the call to the tosorax function */
    RemovePushAndOp (D);

    /* We changed the sequence */
    return 1;
}



static unsigned Opt_tosxorax (StackOpData* D)
/* Optimize the tosxorax sequence if possible */
{
    CodeEntry*  X;

    /* Check the entry before the push. If it's a lda instruction with an
     * addressing mode that allows us to replace it, we may use this
     * location for the op and must not save the value in the zero page
     * location.
     */
    CheckDirectOp (D);

    /* Store the value into the zeropage instead of pushing it */
    ReplacePushByStore (D);

    /* Inline the xor, low byte */
    D->IP = D->OpIndex + 1;
    AddOpLow (D, OP65_EOR);

    /* High byte */
    if (D->PushEntry->RI->In.RegX >= 0 && D->OpEntry->RI->In.RegX >= 0) {
     	/* Both values known, precalculate the result */
     	const char* Arg = MakeHexArg (D->PushEntry->RI->In.RegX ^ D->OpEntry->RI->In.RegX);
       	X = NewCodeEntry (OP65_LDX, AM65_IMM, Arg, 0, D->OpEntry->LI);
     	InsertEntry (D, X, D->IP++);
    } else if (D->PushEntry->RI->In.RegX != 0) {
     	/* High byte is unknown */
        AddOpHigh (D, OP65_EOR);
    }

    /* Remove the push and the call to the tosandax function */
    RemovePushAndOp (D);

    /* We changed the sequence */
    return 1;
}



/*****************************************************************************/
/*   		      		     Code                                    */
/*****************************************************************************/



/* Flags for the functions */
typedef enum {
    STOP_NONE,	     	    /* Nothing special */
    STOP_A_UNUSED           /* Call only if a unused later */
} STOP_FLAGS;


typedef unsigned (*OptFunc) (StackOpData* D);
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



static int CmpHarmless (const void* Key, const void* Entry)
/* Compare function for bsearch */
{
    return strcmp (Key, *(const char**)Entry);
}



static int HarmlessCall (const char* Name)
/* Check if this is a call to a harmless subroutine that will not interrupt
 * the pushax/op sequence when encountered.
 */
{
    static const char* Tab[] = {
        "ldaxysp",
    };

    void* R = bsearch (Name,
                       Tab,
                       sizeof (Tab) / sizeof (Tab[0]),
                       sizeof (Tab[0]),
                       CmpHarmless);
    return (R != 0);
}



/*****************************************************************************/
/*  		      	     	     Code                                    */
/*****************************************************************************/



unsigned OptStackOps (CodeSeg* S)
/* Optimize operations that take operands via the stack */
{
    unsigned    Changes = 0;    /* Number of changes in one run */
    int         InSeq = 0;      /* Inside a sequence */
    unsigned    Push = 0; 	/* Index of pushax */
    unsigned    UsedRegs = 0;   /* Zeropage registers used in sequence */
    unsigned    I;


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

                    StackOpData Data;
		    int PreCondOk = 1;

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
		     	    Data.ZPLo = "sreg";
		     	    Data.ZPHi = "sreg+1";
		     	} else if ((UsedRegs & REG_PTR1) == REG_NONE) {
		     	    Data.ZPLo = "ptr1";
		     	    Data.ZPHi = "ptr1+1";
		     	} else if ((UsedRegs & REG_PTR2) == REG_NONE) {
		     	    Data.ZPLo = "ptr2";
		     	    Data.ZPHi = "ptr2+1";
		     	} else {
		     	    /* No registers available */
		     	    PreCondOk = 0;
		     	}
		    }

	    	    /* If preconditions are ok, call the optimizer function */
	    	    if (PreCondOk) {

	    		/* Adjust stack offsets */
	    		Data.OpIndex = I + AdjustStackOffset (S, Push, I, 2);

                        /* Prepare the remainder of the data structure */
                        Data.Code      = S;
                        Data.Flags     = 0;
                        Data.PushIndex = Push;
                        Data.PrevEntry = CS_GetPrevEntry (S, Data.PushIndex);
                        Data.PushEntry = CS_GetEntry (S, Data.PushIndex);
                        Data.OpEntry   = E;
                        Data.NextEntry = CS_GetNextEntry (S, Data.OpIndex);

       	    	     	/* Call the optimizer function */
	    	     	Changes += F->Func (&Data);

	    	     	/* Regenerate register info */
	    	     	CS_GenRegInfo (S);
	    	    }

	    	    /* End of sequence */
	    	    InSeq = 0;

	    	} else if (strcmp (E->Arg, "pushax") == 0) {
	    	    /* Restart the sequence */
	    	    Push     = I;
	    	    UsedRegs = REG_NONE;
	    	} else if (!HarmlessCall (E->Arg)) {
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



