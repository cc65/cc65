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




