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
#include "print.h"

/* cc65 */
#include "asmlabel.h"
#include "codeent.h"
#include "codeinfo.h"
#include "coptind.h"
#include "error.h"
#include "global.h"
#include "codeopt.h"



/*****************************************************************************/
/*  	       	  	  	     Data				     */
/*****************************************************************************/



/* Defines for the conditions in a compare */
typedef enum {
    CMP_INV = -1,
    CMP_EQ,
    CMP_NE,
    CMP_GT,
    CMP_GE,
    CMP_LT,
    CMP_LE,
    CMP_UGT,
    CMP_UGE,
    CMP_ULT,
    CMP_ULE
} cmp_t;

/* Table with the compare suffixes */
static const char CmpSuffixTab [][4] = {
    "eq", "ne", "gt", "ge", "lt", "le", "ugt", "uge", "ult", "ule"
};

/* Table used to invert a condition, indexed by condition */
static const unsigned char CmpInvertTab [] = {
    CMP_NE, CMP_EQ,
    CMP_LE, CMP_LT, CMP_GE, CMP_GT,
    CMP_ULE, CMP_ULT, CMP_UGE, CMP_UGT
};

/* Table to show which compares are signed (use the N flag) */
static const char CmpSignedTab [] = {
    0, 0, 1, 1, 1, 1, 0, 0, 0, 0
};



/*****************************************************************************/
/*	     	 	       Helper functions				     */
/*****************************************************************************/



static cmp_t FindCmpCond (const char* Suffix)
/* Map a condition suffix to a code. Return the code or CMP_INV on failure */
{
    int I;

    /* Linear search */
    for (I = 0; I < sizeof (CmpSuffixTab) / sizeof (CmpSuffixTab [0]); ++I) {
       	if (strcmp (Suffix, CmpSuffixTab [I]) == 0) {
	    /* Found */
     	    return I;
     	}
    }

    /* Not found */
    return CMP_INV;
}



/*****************************************************************************/
/*	       Remove calls to the bool transformer subroutines		     */
/*****************************************************************************/



static unsigned OptBoolTransforms (CodeSeg* S)
/* Try to remove the call to boolean transformer routines where the call is
 * not really needed.
 */
{
    unsigned Changes = 0;
    unsigned I;

    /* Get the number of entries, bail out if we have not enough */
    unsigned Count = GetCodeEntryCount (S);
    if (Count < 2) {
     	return 0;
    }

    /* Walk over the entries */
    I = 0;
    while (I < Count-1) {

      	/* Get next entry */
       	CodeEntry* E = GetCodeEntry (S, I);

	/* Check for a boolean transformer */
	if (E->OPC == OPC_JSR && strncmp (E->Arg, "bool", 4) == 0) {

	    cmp_t Cond;

	    /* Get the next entry */
	    CodeEntry* N = GetCodeEntry (S, I+1);

	    /* Check if this is a conditional branch */
	    if ((N->Info & OF_CBRA) == 0) {
		/* No conditional branch, bail out */
		goto NextEntry;
	    }

	    /* Make the boolean transformer unnecessary by changing the
	     * the conditional jump to evaluate the condition flags that
	     * are set after the compare directly. Note: jeq jumps if
	     * the condition is not met, jne jumps if the condition is met.
	     */
	    Cond = FindCmpCond (E->Arg + 4);
	    if (Cond == CMP_INV) {
		/* Unknown function */
		goto NextEntry;
	    }

     	    /* Invert the code if we jump on condition not met. */
       	    if (GetBranchCond (N->OPC) == BC_EQ) {
	     	/* Jumps if condition false, invert condition */
	     	Cond = CmpInvertTab [Cond];
  	    }

	    /* Check if we can replace the code by something better */
 	    switch (Cond) {

	    	case CMP_EQ:
		    ReplaceOPC (N, OPC_JEQ);
	    	    break;

	    	case CMP_NE:
		    ReplaceOPC (N, OPC_JNE);
	    	    break;

	       	case CMP_GT:
		    /* Not now ### */
		    goto NextEntry;

	    	case CMP_GE:
		    ReplaceOPC (N, OPC_JPL);
	    	    break;

	    	case CMP_LT:
		    ReplaceOPC (N, OPC_JMI);
	    	    break;

	    	case CMP_LE:
		    /* Not now ### */
	    	    goto NextEntry;

     	    	case CMP_UGT:
		    /* Not now ### */
		    goto NextEntry;

	    	case CMP_UGE:
       	       	    ReplaceOPC (N, OPC_JCS);
	    	    break;

	    	case CMP_ULT:
		    ReplaceOPC (N, OPC_JCC);
	    	    break;

	    	case CMP_ULE:
		    /* Not now ### */
	    	    goto NextEntry;

     	    	default:
	    	    Internal ("Unknown jump condition: %d", Cond);

	    }

	    /* Remove the call to the bool transformer */
	    DelCodeEntry (S, I);
	    --Count;

	    /* Remember, we had changes */
	    ++Changes;

	}

NextEntry:
	/* Next entry */
	++I;

    }

    /* Return the number of changes made */
    return Changes;
}



/*****************************************************************************/
/*     	       	      	  	     Code	   			     */
/*****************************************************************************/



/* Table with all the optimization functions */
typedef struct OptFunc OptFunc;
struct OptFunc {
    unsigned (*Func) (CodeSeg*);/* Optimizer function */
    const char*	Name;		/* Name of optimizer step */
    char	Disabled;	/* True if pass disabled */
};



/* Table with optimizer steps -  are called in this order */
static OptFunc OptFuncs [] = {
    /* Optimize jump cascades */
    { OptJumpCascades, 	    "OptJumpCascades",		0      	},
    /* Remove dead jumps */
    { OptDeadJumps,    	    "OptDeadJumps",		0      	},
    /* Change jsr/rts to jmp */
    { OptRTS,  	       	    "OptRTS",			0      	},
    /* Remove dead code */
    { OptDeadCode,     	    "OptDeadCode",		0      	},
    /* Optimize jump targets */
    { OptJumpTarget,   	    "OptJumpTarget",		0      	},
    /* Remove dead conditional branches */
    { OptDeadCondBranches,  "OptDeadCondBranches",	0    	},
    /* Remove calls to the bool transformer subroutines	*/
    { OptBoolTransforms,    "OptBoolTransforms",	0	},
};



static OptFunc* FindOptStep (const char* Name)
/* Find an optimizer step by name in the table and return a pointer. Print an
 * error and call AbEnd if not found.
 */
{
    unsigned I;

    /* Run all optimization steps */
    for (I = 0; I < sizeof(OptFuncs)/sizeof(OptFuncs[0]); ++I) {
    	if (strcmp (OptFuncs[I].Name, Name) == 0) {
    	    /* Found */
    	    return OptFuncs+I;
    	}
    }

    /* Not found */
    AbEnd ("Optimization step `%s' not found", Name);
    return 0;
}



void DisableOpt (const char* Name)
/* Disable the optimization with the given name */
{
    OptFunc* F  = FindOptStep (Name);
    F->Disabled = 1;
}



void EnableOpt (const char* Name)
/* Enable the optimization with the given name */
{
    OptFunc* F  = FindOptStep (Name);
    F->Disabled = 0;
}



void RunOpt (CodeSeg* S)
/* Run the optimizer */
{
    unsigned I;
    unsigned Pass = 0;
    unsigned OptChanges;

    /* Print the name of the function we are working on */
    if (S->Func) {
     	Print (stdout, 1, "Running optimizer for function `%s'\n", S->Func->Name);
    } else {
     	Print (stdout, 1, "Running optimizer for global code segment\n");
    }

    /* Repeat all steps until there are no more changes */
    do {
     	/* Reset the number of changes */
     	OptChanges = 0;

	/* Keep the user hapy */
	Print (stdout, 1, "  Optimizer pass %u:\n", ++Pass);

       	/* Run all optimization steps */
       	for (I = 0; I < sizeof(OptFuncs)/sizeof(OptFuncs[0]); ++I) {

	    /* Print the name of the following optimizer step */
	    Print (stdout, 1, "    %s:%*s", OptFuncs[I].Name,
	       	   (int) (30-strlen(OptFuncs[I].Name)), "");

	    /* Check if the step is disabled */
       	    if (OptFuncs[I].Disabled) {
	       	Print (stdout, 1, "Disabled\n");
	    } else {
	       	unsigned Changes = OptFuncs[I].Func (S);
		OptChanges += Changes;
		Print (stdout, 1, "%u Changes\n", Changes);
	    }
	}

    } while (OptChanges > 0);
}



