/*****************************************************************************/
/*                                                                           */
/*				   codeent.h				     */
/*                                                                           */
/*			      Code segment entry			     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2001     Ullrich von Bassewitz                                        */
/*              Wacholderweg 14                                              */
/*              D-70597 Stuttgart                                            */
/* EMail:       uz@musoftware.de                                             */
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



#ifndef CODEENT_H
#define CODEENT_H



#include <stdio.h>

/* common */
#include "coll.h"

/* b6502 */
#include "label.h"
#include "opcodes.h"



/*****************************************************************************/
/*  	       	    	  	     Data				     */
/*****************************************************************************/



/* Code entry structure */
typedef struct CodeEntry CodeEntry;
struct CodeEntry {
    opc_t	    	OPC;		/* Opcode */
    am_t	    	AM;		/* Adressing mode */
    unsigned char   	Size;		/* Estimated size */
    unsigned char   	Hints;		/* Hints for this entry */
    union {
	unsigned    	Num;		/* Numeric argument */
	char*	    	Expr;		/* Textual argument */
    } Arg;
    unsigned short  	Flags;		/* Flags */
    unsigned short  	Usage;		/* Register usage for this entry */
    CodeLabel*	  	JumpTo;		/* Jump label */
    Collection	  	Labels;		/* Labels for this instruction */
};



/*****************************************************************************/
/*     	       	      	  	     Code				     */
/*****************************************************************************/



CodeEntry* NewCodeEntry (const OPCDesc* D, am_t AM, CodeLabel* JumpTo);
/* Create a new code entry, initialize and return it */

void FreeCodeEntry (CodeEntry* E);
/* Free the given code entry */

void OutputCodeEntry (FILE* F, const CodeEntry* E);
/* Output the code entry to a file */



/* End of codeent.h */
#endif



