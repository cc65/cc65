/*****************************************************************************/
/*                                                                           */
/*				   codeent.c				     */
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



/* common */
#include "check.h"
#include "xmalloc.h"

/* cc65 */
#include "error.h"

/* b6502 */
#include "codeinfo.h"
#include "label.h"
#include "opcodes.h"
#include "codeent.h"



/*****************************************************************************/
/*  	       	    	   	     Data				     */
/*****************************************************************************/



/*****************************************************************************/
/*     	       	      	   	     Code				     */
/*****************************************************************************/



CodeEntry* NewCodeEntry (const OPCDesc* D, am_t AM, CodeLabel* JumpTo)
/* Create a new code entry, initialize and return it */
{
    /* Allocate memory */
    CodeEntry* E = xmalloc (sizeof (CodeEntry));

    /* Initialize the fields */
    E->OPC	= D->OPC;
    E->AM	= AM;
    E->Size	= GetInsnSize (E->OPC, E->AM);
    E->Hints	= 0;
    E->Arg.Num	= 0;
    E->Flags	= 0;
    E->Usage	= D->Info & (CI_MASK_USE | CI_MASK_CHG);
    E->JumpTo	= JumpTo;
    InitCollection (&E->Labels);

    /* If we have a label given, add this entry to the label */
    if (JumpTo) {
	CollAppend (&JumpTo->JumpFrom, E);
    }

    /* Return the initialized struct */
    return E;
}



void FreeCodeEntry (CodeEntry* E)
/* Free the given code entry */
{
    /* ## Free the string argument if we have one */

    /* Cleanup the collection */
    DoneCollection (&E->Labels);

    /* Free the entry */
    xfree (E);
}



void OutputCodeEntry (FILE* F, const CodeEntry* E)
/* Output the code entry to a file */
{
    const OPCDesc* D;

    /* If we have a label, print that */
    unsigned LabelCount = CollCount (&E->Labels);
    unsigned I;
    for (I = 0; I < LabelCount; ++I) {
	OutputCodeLabel (F, CollConstAt (&E->Labels, I));
    }

    /* Get the opcode description */
    D = GetOPCDesc (E->OPC);

    /* Print the mnemonic */
    fprintf (F, "\t%s", D->Mnemo);

    /* Print the operand */
    switch (E->AM) {

	case AM_IMP:
	    /* implicit */
	    break;

	case AM_ACC:
	    /* accumulator */
	    fprintf (F, "\ta");
	    break;

	case AM_IMM:
	    /* immidiate */
	    fprintf (F, "\t#%s", E->Arg.Expr);
	    break;

	case AM_ZP:
	case AM_ABS:
	    /* zeropage and absolute */
	    fprintf (F, "\t%s", E->Arg.Expr);
	    break;

	case AM_ZPX:
	case AM_ABSX:
	    /* zeropage,X and absolute,X */
	    fprintf (F, "\t%s,x", E->Arg.Expr);
	    break;

	case AM_ABSY:
	    /* absolute,Y */
	    fprintf (F, "\t%s,y", E->Arg.Expr);
	    break;

	case AM_ZPX_IND:
	    /* (zeropage,x) */
       	    fprintf (F, "\t(%s,x)", E->Arg.Expr);
	    break;

	case AM_ZP_INDY:
	    /* (zeropage),y */
       	    fprintf (F, "\t(%s),y", E->Arg.Expr);
	    break;

	case AM_ZP_IND:
	    /* (zeropage) */
       	    fprintf (F, "\t(%s)", E->Arg.Expr);
	    break;

	case AM_BRA:
	    /* branch */
	    CHECK (E->JumpTo != 0);
	    fprintf (F, "\t%s", E->JumpTo->Name);
	    break;

	default:
	    Internal ("Invalid addressing mode");

    }

    /* Terminate the line */
    fprintf (F, "\n");
}




