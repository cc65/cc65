/*****************************************************************************/
/*                                                                           */
/*				   coptsize.c				     */
/*                                                                           */
/*                              Size optimizations                           */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2002      Ullrich von Bassewitz                                       */
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
#include "cpu.h"
#include "coptsize.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



typedef struct CallDesc CallDesc;
struct CallDesc {
    const char*     LongFunc;       /* Long function name */
    short           A, X, Y;        /* Register contents */
    const char*     ShortFunc;      /* Short function name */
};

/* Note: The table is sorted. If there is more than one entry with the same
 * name, entries are sorted best match first, so when searching linear for
 * a match, the first one can be used because it is also the best one (or
 * at least none of the following ones are better).
 */
static const CallDesc CallTable [] = {
    { "addeqysp",   -1,   -1,    0, "addeq0sp"      },
    { "laddeqysp",  -1,   -1,    0, "laddeq0sp"     },
    { "ldaxidx",    -1,   -1,    1, "ldaxi"         },
    { "ldaxysp",    -1,   -1,    1, "ldax0sp"       },
    { "ldeaxidx",   -1,   -1,    3, "ldeaxi"        },
    { "ldeaxysp",   -1,   -1,    3, "ldeax0sp"      },
    { "pusha",       0,   -1,   -1, "pushc0"        },
    { "pusha",       1,   -1,   -1, "pushc1"        },
    { "pusha",       2,   -1,   -1, "pushc2"        },
    { "pushax",      0,    0,   -1, "push0"         },
    { "pushax",      1,    0,   -1, "push1"         },
    { "pushax",      2,    0,   -1, "push2"         },
    { "pushax",      3,    0,   -1, "push3"         },
    { "pushax",      4,    0,   -1, "push4"         },
    { "pushax",      5,    0,   -1, "push5"         },
    { "pushax",      6,    0,   -1, "push6"         },
    { "pushax",      7,    0,   -1, "push7"         },
    { "pushax",     -1,    0,   -1, "pusha0"        },
    { "pushax",     -1, 0xFF,   -1, "pushaFF"       },
    { "pushaysp",   -1,   -1,    0, "pusha0sp"      },
    { "staxysp",    -1,   -1,    0, "stax0sp"       },
    { "tosaddax",   -1,    0,   -1, "tosadda0"      },
    { "tosandax",   -1,    0,   -1, "tosanda0"      },
    { "tosdivax",   -1,    0,   -1, "tosdiva0"      },
    { "toseqax",    -1,    0,   -1, "toseqa0"       },
    { "tosgeax",    -1,    0,   -1, "tosgea0"       },
    { "tosgtax",    -1,    0,   -1, "tosgta0"       },
    { "tosleax",    -1,    0,   -1, "toslea0"       },
    { "tosorax",    -1,    0,   -1, "tosora0"       },
    { "lsubeqysp",  -1,   -1,    0, "lsubeq0sp"     },
    { "steaxysp",   -1,   -1,    0, "steax0sp"      },
    { "subeqysp",   -1,   -1,    0, "subeq0sp"      },
    { "tosaslax",   -1,    0,   -1, "tosasla0"      },
    { "tosasrax",   -1,    0,   -1, "tosasra0"      },
    { "tosltax",    -1,    0,   -1, "toslta0"       },
    { "tosmodax",   -1,    0,   -1, "tosmoda0"      },
    { "tosmulax",   -1,    0,   -1, "tosmula0"      },
    { "tosneax",    -1,    0,   -1, "tosnea0"       },
    { "tosrsubax",  -1,    0,   -1, "tosrsuba0"     },
    { "tosshlax",   -1,    0,   -1, "tosshla0"      },
    { "tosshrax",   -1,    0,   -1, "tosshra0"      },
    { "tossubax",   -1,    0,   -1, "tossuba0"      },
    { "tosudivax",  -1,    0,   -1, "tosudiva0"     },
    { "tosugeax",   -1,    0,   -1, "tosugea0"      },
    { "tosugtax",   -1,    0,   -1, "tosugta0"      },
    { "tosuleax",   -1,    0,   -1, "tosulea0"      },
    { "tosultax",   -1,    0,   -1, "tosulta0"      },
    { "tosumodax",  -1,    0,   -1, "tosumoda0"     },
    { "tosumulax",  -1,    0,   -1, "tosumula0"     },
    { "tosxorax",   -1,    0,   -1, "tosxora0"      },

#if 0
    "tosadd0ax",         /* tosaddeax, sreg = 0 */
    "laddeqa",           /* laddeq, sreg = 0, x = 0 */
    "laddeq1",           /* laddeq, sreg = 0, x = 0, a = 1 */
    "tosand0ax",         /* tosandeax, sreg = 0 */
    "tosdiv0ax",         /* tosdiveax, sreg = 0 */
    "tosmod0ax",         /* tosmodeax, sreg = 0 */
    "tosmul0ax",         /* tosmuleax, sreg = 0 */
    "tosumul0ax",        /* tosumuleax, sreg = 0 */
    "tosor0ax",          /* tosoreax, sreg = 0 */
    "push0ax",           /* pusheax, sreg = 0 */
    "tosrsub0ax",        /* tosrsubeax, sreg = 0 */
    "tosshl0ax",         /* tosshleax, sreg = 0 */
    "tosasl0ax",         /* tosasleax, sreg = 0 */
    "tosshr0ax",         /* tosshreax, sreg = 0 */
    "tosasr0ax",         /* tosasreax, sreg = 0 */
    "tossub0ax",         /* tossubeax, sreg = 0 */
    "lsubeqa",           /* lsubeq, sreg = 0, x = 0 */
    "lsubeq1",           /* lsubeq, sreg = 0, x = 0, a = 1 */
    "tosudiv0ax",        /* tosudiveax, sreg = 0 */
    "tosumod0ax",        /* tosumodeax, sreg = 0 */
    "tosxor0ax",         /* tosxoreax, sreg = 0 */
#endif
};
#define CALL_COUNT (sizeof(CallTable) / sizeof(CallTable[0]))



/*****************************************************************************/
/*     		      	       	    Helpers                                  */
/*****************************************************************************/



static const CallDesc* FindCall (const char* Name)
/* Find the function with the given name. Return a pointer to the table entry
 * or NULL if the function was not found.
 */
{
    /* Do a binary search */
    int First = 0;
    int Last = CALL_COUNT - 1;
    int Found = 0;

    while (First <= Last) {

       	/* Set current to mid of range */
	int Current = (Last + First) / 2;

       	/* Do a compare */
       	int Result = strcmp (CallTable[Current].LongFunc, Name);
	if (Result < 0) {
	    First = Current + 1;
	} else {
	    Last = Current - 1;
	    if (Result == 0) {
       	       	/* Found. Repeat the procedure until the first of all entries
                 * with the same name is found.
                 */
	       	Found = 1;
	    }
	}
    }

    /* Return the first entry if found, or NULL otherwise */
    return Found? &CallTable[First] : 0;
}



/*****************************************************************************/
/*  		      	     	     Code                                    */
/*****************************************************************************/



unsigned OptSize1 (CodeSeg* S)
/* Do size optimization by calling special subroutines that preload registers.
 * This routine does not work standalone, it needs a following register load
 * removal pass.
 */
{
    CodeEntry* E;
    unsigned Changes = 0;
    unsigned I;

    /* Generate register info for the following step */
    CS_GenRegInfo (S);

    /* Walk over the entries */
    I = 0;
    while (I < CS_GetEntryCount (S)) {

        const CallDesc* D;

      	/* Get next entry */
       	E = CS_GetEntry (S, I);

     	/* Check if it's a subroutine call */
     	if (E->OPC == OP65_JSR && (D = FindCall (E->Arg)) != 0) {

     	    /* Check for any of the known functions. */
            while (1) {

                /* Check the registers */
                if ((D->A < 0 || D->A == E->RI->In.RegA) &&
                    (D->X < 0 || D->X == E->RI->In.RegX) &&
                    (D->Y < 0 || D->Y == E->RI->In.RegY)) {
                    /* Ok, match for all registers */
                    CodeEntry* X;
                    X = NewCodeEntry (E->OPC, E->AM, D->ShortFunc, 0, E->LI);
                    CS_InsertEntry (S, X, I+1);
                    CS_DelEntry (S, I);

                    /* Remember that we had changes */
                    ++Changes;
                }

                /* Next table entry, bail out if next entry not valid */
                if (++D >= CallTable + CALL_COUNT ||
                    strcmp (D->LongFunc, E->Arg) != 0) {
                    /* End of table or entries reached */
                    break;
                }
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



unsigned OptSize2 (CodeSeg* S)
/* Do size optimization by using shorter code sequences, even if this
 * introduces relations between instructions. This step must be one of the
 * last steps, because it makes further work much more difficult.
 */
{
    unsigned Changes = 0;
    unsigned I;

    /* Generate register info for the following step */
    CS_GenRegInfo (S);

    /* Walk over the entries */
    I = 0;
    while (I < CS_GetEntryCount (S)) {

      	/* Get next entry */
       	CodeEntry* E = CS_GetEntry (S, I);

        /* Get the input registers */
        const RegContents* In = &E->RI->In;

	/* Assume we have no replacement */
	CodeEntry* X = 0;

	/* Check the instruction */
	switch (E->OPC) {

	    case OP65_LDA:
	        if (CE_KnownImm (E)) {
		    short Val = (short) E->Num;
		    if (Val == In->RegX) {
		    	X = NewCodeEntry (OP65_TXA, AM65_IMP, 0, 0, E->LI);
		    } else if (Val == In->RegY) {
		    	X = NewCodeEntry (OP65_TYA, AM65_IMP, 0, 0, E->LI);
		    } else if (RegValIsKnown (In->RegA) && CPU >= CPU_65C02) {
		    	if (Val == ((In->RegA - 1) & 0xFF)) {
		     	    X = NewCodeEntry (OP65_DEA, AM65_IMP, 0, 0, E->LI);
		    	} else if (Val == ((In->RegA + 1) & 0xFF)) {
		    	    X = NewCodeEntry (OP65_INA, AM65_IMP, 0, 0, E->LI);
	      	    	}
		    }
	      	}
	        break;

	    case OP65_LDX:
	        if (CE_KnownImm (E)) {
		    short Val = (short) E->Num;
		    if (RegValIsKnown (In->RegX) && Val == ((In->RegX - 1) & 0xFF)) {
			X = NewCodeEntry (OP65_DEX, AM65_IMP, 0, 0, E->LI);
       	       	    } else if (RegValIsKnown (In->RegX) && Val == ((In->RegX + 1) & 0xFF)) {
			X = NewCodeEntry (OP65_INX, AM65_IMP, 0, 0, E->LI);
		    } else if (Val == In->RegA) {
			X = NewCodeEntry (OP65_TAX, AM65_IMP, 0, 0, E->LI);
                    }
		}
	        break;

       	    case OP65_LDY:
	        if (CE_KnownImm (E)) {
		    short Val = (short) E->Num;
		    if (RegValIsKnown (In->RegY) && Val == ((In->RegY - 1) & 0xFF)) {
			X = NewCodeEntry (OP65_DEY, AM65_IMP, 0, 0, E->LI);
		    } else if (RegValIsKnown (In->RegY) && Val == ((In->RegY + 1) & 0xFF)) {
			X = NewCodeEntry (OP65_INY, AM65_IMP, 0, 0, E->LI);
		    } else if (Val == In->RegA) {
			X = NewCodeEntry (OP65_TAY, AM65_IMP, 0, 0, E->LI);
		    }
		}
	        break;

	    default:
	        /* Avoid gcc warnings */
	        break;

	}

	/* Insert the replacement if we have one */
	if (X) {
	    CS_InsertEntry (S, X, I+1);
	    CS_DelEntry (S, I);
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



