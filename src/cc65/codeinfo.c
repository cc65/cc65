/*****************************************************************************/
/*                                                                           */
/*				  codeinfo.c				     */
/*                                                                           */
/*		    Additional information about 6502 code		     */
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
#include <string.h>

/* common */
#include "coll.h"

/* cc65 */
#include "codeinfo.h"



/*****************************************************************************/
/*     	       	      	  	     Data				     */
/*****************************************************************************/



/* Table listing the function names and code info values for known internally
 * used functions. This table should get auto-generated in the future.
 */
typedef struct FuncInfo FuncInfo;
struct FuncInfo {
    const char*	    Name;	/* Function name */
    unsigned char   Use;	/* Register usage */
    unsigned char   Chg;	/* Changed/destroyed registers */
};

static const FuncInfo FuncInfoTable[] = {
    { "booleq",		REG_NONE,    	REG_AX	},
    { "boolge",		REG_NONE,	REG_AX	},
    { "boolgt",		REG_NONE,	REG_AX	},
    { "boolle",		REG_NONE,	REG_AX	},
    { "boollt",		REG_NONE,	REG_AX	},
    { "boolne",		REG_NONE,	REG_AX	},
    { "booluge",   	REG_NONE,	REG_AX	},
    { "boolugt",   	REG_NONE,	REG_AX	},
    { "boolule",   	REG_NONE,	REG_AX	},
    { "boolult",   	REG_NONE,	REG_AX	},
    { "decax1",     	REG_AX,		REG_AX  },
    { "decax2",     	REG_AX,		REG_AX  },
    { "decax3",     	REG_AX,		REG_AX  },
    { "decax4",     	REG_AX,		REG_AX  },
    { "decax5",     	REG_AX,		REG_AX  },
    { "decax6",     	REG_AX,		REG_AX  },
    { "decax7",     	REG_AX,		REG_AX  },
    { "decax8",     	REG_AX,		REG_AX  },
    { "decaxy",	   	REG_AXY,	REG_AX	},
    { "decsp2",    	REG_NONE,	REG_A	},
    { "decsp3",    	REG_NONE,	REG_A	},
    { "decsp4",    	REG_NONE,	REG_A	},
    { "decsp5",    	REG_NONE,	REG_A	},
    { "decsp6",    	REG_NONE,	REG_A	},
    { "decsp7",    	REG_NONE,	REG_A	},
    { "decsp8",    	REG_NONE,	REG_A	},
    { "ldax0sp",   	REG_Y,		REG_AX	},
    { "ldaxysp",   	REG_Y,		REG_AX 	},
    { "pusha",	   	REG_A,		REG_Y	},
    { "pusha0",	       	REG_A,		REG_XY	},
    { "pushax",	   	REG_AX,		REG_Y	},
    { "pushw0sp",  	REG_NONE,	REG_AXY	},
    { "pushwysp",  	REG_Y,		REG_AXY	},
    { "tosicmp",   	REG_AX,		REG_AXY },
};
#define FuncInfoCount	(sizeof(FuncInfoTable) / sizeof(FuncInfoTable[0]))

/* Structure used to pass information to the RegValUsedInt1 and 2 functions */
typedef struct RVUInfo RVUInfo;
struct RVUInfo {
    Collection	VisitedLines;		/* Lines already visited */
};



/*****************************************************************************/
/*     	       	      	  	     Code 		 		     */
/*****************************************************************************/



static int CompareFuncInfo (const void* Key, const void* Info)
/* Compare function for bsearch */
{
    return strcmp (Key, ((const	FuncInfo*) Info)->Name);
}



void GetFuncInfo (const char* Name, unsigned char* Use, unsigned char* Chg)
/* For the given function, lookup register information and combine it with
 * the information already in place. If the function is unknown, assume it
 * will use all registers and load all registers.
 * See codeinfo.h for possible flags.
 */
{
    /* Search for the function */
    const FuncInfo* Info = bsearch (Name, FuncInfoTable, FuncInfoCount,
			  	    sizeof(FuncInfo), CompareFuncInfo);

    /* Do we know the function? */
    if (Info) {
	/* Use the information we have */
	*Use |= Info->Use;
	*Chg |= Info->Chg;
    } else {
	*Use |= REG_AXY;
	*Chg |= REG_AXY;
    }
}

					
#if 0

static unsigned RVUInt2 (Line* L,
    		       	 LineColl* LC, 	    /* To remember visited lines */
    		   	 unsigned Used,     /* Definitely used registers */
    		   	 unsigned Unused)   /* Definitely unused registers */
/* Subfunction for RegValUsed. Will be called recursively in case of branches. */
{
    int I;

    /* Check the following instructions. We classifiy them into primary
     * loads (register value not used), neutral (check next instruction),
     * and unknown (assume register was used).
     */
    while (1) {

    	unsigned R;

    	/* Get the next line and follow jumps */
    	do {

    	    /* Handle jumps to local labels (continue there) */
       	    if (LineMatch (L, "\tjmp\tL") || LineMatch (L, "\tbra\tL")) {
    	     	/* Get the target of the jump */
    	     	L = GetTargetLine (L->Line+5);
    	    }

    	    /* Get the next line, skip local labels */
    	    do {
       	    	L = NextCodeSegLine (L);
    	    } while (L && (IsLocalLabel (L) || L->Line[0] == '\0'));

    	    /* Bail out if we're done */
    	    if (L == 0 || IsExtLabel (L)) {
    	    	/* End of function reached */
    	    	goto ExitPoint;
    	    }

    	    /* Check if we had this line already. If so, bail out, if not,
    	     * add it to the list of known lines.
    	     */
    	    if (LCHasLine (LC, L) || !LCAddLine (LC, L)) {
    	    	goto ExitPoint;
    	    }

     	} while (LineMatch (L, "\tjmp\tL") || LineMatch (L, "\tbra\tL"));

    	/* Special handling of code hints */
       	if (IsHintLine (L)) {

    	    if (IsHint (L, "a:-") && (Used & REG_A) == 0) {
    		Unused |= REG_A;
    	    } else if (IsHint (L, "x:-") && (Used & REG_X) == 0) {
    		Unused |= REG_X;
    	    } else if (IsHint (L, "y:-") && (Used & REG_Y) == 0) {
    		Unused |= REG_Y;
    	    }

    	/* Special handling for branches */
    	} else if (LineMatchX (L, ShortBranches) >= 0 ||
    	    LineMatchX (L, LongBranches) >= 0) {
    	    const char* Target = L->Line+5;
    	    if (Target[0] == 'L') {
    	       	/* Jump to local label. Check the register usage starting at
    	       	 * the branch target and at the code following the branch.
    	       	 * All registers that are unused in both execution flows are
    	       	 * returned as unused.
    	       	 */
    	       	unsigned U1, U2;
       	       	U2 = RVUInt1 (GetTargetLine (Target), LC, Used, Unused);
    	       	U1 = RVUInt1 (L, LC, Used, Unused);
    	       	return U1 | U2;		/* Used in any of the branches */
    	    }
    	} else {

    	    /* Search for the instruction in this line */
    	    I = FindCmd (L);

    	    /* If we don't find it, assume all other registers are used */
    	    if (I < 0) {
    		break;
    	    }

    	    /* Evaluate the use flags, check for addressing modes */
    	    R = CmdDesc[I].Use;
    	    if (IsXAddrMode (L)) {
    		R |= REG_X;
    	    } else if (IsYAddrMode (L)) {
    		R |= REG_Y;
    	    }
     	    if (R) {
    		/* Remove registers that were already new loaded */
    		R &= ~Unused;

    		/* Remember the remaining registers */
    		Used |= R;
    	    }

    	    /* Evaluate the load flags */
    	    R = CmdDesc[I].Load;
    	    if (R) {
    		/* Remove registers that were already used */
    		R &= ~Used;

    		/* Remember the remaining registers */
    		Unused |= R;
    	    }

    	}

       	/* If we know about all registers, bail out */
       	if ((Used | Unused) == REG_ALL) {
    	    break;
    	}
    }

ExitPoint:
    /* Return to the caller the complement of all unused registers */
    return ~Unused & REG_ALL;
}



static unsigned RVUInt1 (Line* L,
    		       	 LineColl* LC, 	    /* To remember visited lines */
    		       	 unsigned Used,     /* Definitely used registers */
    		       	 unsigned Unused)   /* Definitely unused registers */
/* Subfunction for RegValUsed. Will be called recursively in case of branches. */
{
    /* Remember the current count of the line collection */
    unsigned Count = LC->Count;

    /* Call the worker routine */
    unsigned R = RVUInt2 (L, LC, Used, Unused);

    /* Restore the old count */
    LC->Count = Count;

    /* Return the result */
    return R;
}



static unsigned RegValUsed (Line* Start)
/* Check the next instructions after the one in L for register usage. If
 * a register is used as an index, or in a store or other instruction, it
 * is assumed to be used. If a register is loaded with a value, before it
 * was used by one of the actions described above, it is assumed unused.
 * If the end of the lookahead is reached, all registers that are uncertain
 * are marked as used.
 * The result of the search is returned.
 */
{
    unsigned R;

    /* Create a new line collection and enter the start line */
    LineColl* LC = NewLineColl (256);
    LCAddLine (LC, Start);

    /* Call the recursive subfunction */
    R = RVUInt1 (Start, LC, REG_NONE, REG_NONE);

    /* Delete the line collection */
    FreeLineColl (LC);

    /* Return the registers used */
    return R;
}



static int RegAUsed (Line* Start)
/* Check if the value in A is used. */
{
    return (RegValUsed (Start) & REG_A) != 0;
}



static int RegXUsed (Line* Start)
/* Check if the value in X is used. */
{
    return (RegValUsed (Start) & REG_X) != 0;
}



static int RegYUsed (Line* Start)
/* Check if the value in Y is used. */
{
    return (RegValUsed (Start) & REG_Y) != 0;
}



#endif     
