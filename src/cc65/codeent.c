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
#include "codeinfo.h"
#include "error.h"
#include "funcinfo.h"
#include "global.h"
#include "codelab.h"
#include "opcodes.h"
#include "codeent.h"



/*****************************************************************************/
/*  	       	    	   	     Data				     */
/*****************************************************************************/



/*****************************************************************************/
/*     	       	      	   	     Code				     */
/*****************************************************************************/



CodeEntry* NewCodeEntry (const OPCDesc* D, am_t AM, const char* Arg, CodeLabel* JumpTo)
/* Create a new code entry, initialize and return it */
{
    /* Allocate memory */
    CodeEntry* E = xmalloc (sizeof (CodeEntry));

    /* Initialize the fields */
    E->OPC  	= D->OPC;
    E->AM   	= AM;
    E->Size 	= GetInsnSize (E->OPC, E->AM);
    E->Hints	= 0;
    E->Arg  	= (Arg && Arg[0] != '\0')? xstrdup (Arg) : 0;
    E->Num  	= 0;
    E->Flags	= 0;
    E->Info    	= D->Info;
    if (E->OPC == OPC_JSR && E->Arg) {
	/* A subroutine call */
     	E->Info |= GetFuncInfo (E->Arg);
    } else {
	/* Some other instruction */
     	E->Info |= GetAMUseInfo (AM);
    }
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
    /* Free the string argument if we have one */
    xfree (E->Arg);

    /* Cleanup the collection */
    DoneCollection (&E->Labels);

    /* Free the entry */
    xfree (E);
}



int CodeEntryHasLabel (const CodeEntry* E)
/* Check if the given code entry has labels attached */
{
    return (CollCount (&E->Labels) > 0);
}



void OutputCodeEntry (const CodeEntry* E, FILE* F)
/* Output the code entry to a file */
{
    const OPCDesc* D;
    unsigned Chars;

    /* If we have a label, print that */
    unsigned LabelCount = CollCount (&E->Labels);
    unsigned I;
    for (I = 0; I < LabelCount; ++I) {
    	OutputCodeLabel (CollConstAt (&E->Labels, I), F);
    }

    /* Get the opcode description */
    D = GetOPCDesc (E->OPC);

    /* Print the mnemonic */
    Chars = fprintf (F, "\t%s", D->Mnemo);

    /* Print the operand */
    switch (E->AM) {

    	case AM_IMP:
    	    /* implicit */
    	    break;

    	case AM_ACC:
    	    /* accumulator */
    	    Chars += fprintf (F, "%*sa", 9-Chars, "");
    	    break;

    	case AM_IMM:
    	    /* immidiate */
    	    Chars += fprintf (F, "%*s#%s", 9-Chars, "", E->Arg);
    	    break;

    	case AM_ZP:
    	case AM_ABS:
	    /* zeropage and absolute */
	    Chars += fprintf (F, "%*s%s", 9-Chars, "", E->Arg);
	    break;

	case AM_ZPX:
	case AM_ABSX:
	    /* zeropage,X and absolute,X */
	    Chars += fprintf (F, "%*s%s,x", 9-Chars, "", E->Arg);
	    break;

	case AM_ABSY:
	    /* absolute,Y */
	    Chars += fprintf (F, "%*s%s,y", 9-Chars, "", E->Arg);
	    break;

	case AM_ZPX_IND:
	    /* (zeropage,x) */
       	    Chars += fprintf (F, "%*s(%s,x)", 9-Chars, "", E->Arg);
	    break;

	case AM_ZP_INDY:
	    /* (zeropage),y */
       	    Chars += fprintf (F, "%*s(%s),y", 9-Chars, "", E->Arg);
	    break;

	case AM_ZP_IND:
	    /* (zeropage) */
       	    Chars += fprintf (F, "%*s(%s)", 9-Chars, "", E->Arg);
	    break;

	case AM_BRA:
	    /* branch */
	    CHECK (E->JumpTo != 0);
	    Chars += fprintf (F, "%*s%s", 9-Chars, "", E->JumpTo->Name);
	    break;

	default:
	    Internal ("Invalid addressing mode");

    }

    /* Print usage info if requested by the debugging flag */
//    if (Debug) {
  	Chars += fprintf (F,
  			  "%*s; USE: %c%c%c CHG: %c%c%c",
  			  30-Chars, "",
  			  (E->Info & CI_USE_A)? 'A' : '_',
  			  (E->Info & CI_USE_X)? 'X' : '_',
  			  (E->Info & CI_USE_Y)? 'Y' : '_',
  			  (E->Info & CI_CHG_A)? 'A' : '_',
  			  (E->Info & CI_CHG_X)? 'X' : '_',
  			  (E->Info & CI_CHG_Y)? 'Y' : '_');
//    }

    /* Terminate the line */
    fprintf (F, "\n");
}




