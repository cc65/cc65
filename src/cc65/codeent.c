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



#include <stdlib.h>
#include <string.h>

/* common */
#include "chartype.h"
#include "check.h"
#include "xmalloc.h"

/* cc65 */
#include "codeinfo.h"
#include "error.h"
#include "global.h"
#include "codelab.h"
#include "opcodes.h"
#include "codeent.h"



/*****************************************************************************/
/*  	       	    	   	     Data				     */
/*****************************************************************************/



/* Empty argument */
static char EmptyArg[] = "";



/*****************************************************************************/
/*			       Helper functions				     */
/*****************************************************************************/



static void FreeArg (char* Arg)
/* Free a code entry argument */
{
    if (Arg != EmptyArg) {
	xfree (Arg);
    }
}



static char* GetArgCopy (const char* Arg)
/* Create an argument copy for assignment */
{
    if (Arg && Arg[0] != '\0') {
	/* Create a copy */
	return xstrdup (Arg);
    } else {
	/* Use the empty argument string */
	return EmptyArg;
    }
}



static int NumArg (const char* Arg, unsigned long* Num)
/* If the given argument is numerical, convert it and return true. Otherwise
 * set Num to zero and return false.
 */
{
    char* End;
    unsigned long Val;

    /* Determine the base */
    int Base = 10;
    if (*Arg == '$') {
    	++Arg;
    	Base = 16;
    } else if (*Arg == '%') {
    	++Arg;
    	Base = 2;
    }

    /* Convert the value. strtol is not exactly what we want here, but it's
     * cheap and may be replaced by something fancier later.
     */
    Val = strtoul (Arg, &End, Base);

    /* Check if the conversion was successful */
    if (*End != '\0') {

	/* Could not convert */
	*Num = 0;
	return 0;

    } else {

	/* Conversion ok */
	*Num = Val;
	return 1;

    }
}



static void SetUseChgInfo (CodeEntry* E, const OPCDesc* D)
/* Set the Use and Chg in E */
{
    /* If this is a subroutine call, or a jump to an external function,
     * lookup the information about this function and use it. The jump itself
     * does not change any registers, so we don't need to use the data from D.
     */
    if ((E->Info & (OF_BRA | OF_CALL)) != 0 && E->JumpTo == 0) {
     	/* A subroutine call or jump to external symbol (function exit) */
     	GetFuncInfo (E->Arg, &E->Use, &E->Chg);
    } else {
     	/* Some other instruction. Use the values from the opcode description
	 * plus addressing mode info
	 */
     	E->Use = D->Use | GetAMUseInfo (E->AM);
	E->Chg = D->Chg;
    }
}



/*****************************************************************************/
/*     	       	      	   	     Code				     */
/*****************************************************************************/



CodeEntry* NewCodeEntry (opc_t OPC, am_t AM, const char* Arg,
			 CodeLabel* JumpTo, LineInfo* LI)
/* Create a new code entry, initialize and return it */
{
    /* Get the opcode description */
    const OPCDesc* D = GetOPCDesc (OPC);

    /* Allocate memory */
    CodeEntry* E = xmalloc (sizeof (CodeEntry));

    /* Initialize the fields */
    E->OPC    = D->OPC;
    E->AM     = AM;
    E->Arg    = GetArgCopy (Arg);
    E->Flags  = NumArg (E->Arg, &E->Num)? CEF_NUMARG : 0;
    E->Info   = D->Info;
    E->Size   = GetInsnSize (E->OPC, E->AM);
    E->JumpTo = JumpTo;
    E->LI     = UseLineInfo (LI);
    SetUseChgInfo (E, D);
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
    FreeArg (E->Arg);

    /* Cleanup the collection */
    DoneCollection (&E->Labels);

    /* Release the line info */
    ReleaseLineInfo (E->LI);

    /* Free the entry */
    xfree (E);
}



void ReplaceOPC (CodeEntry* E, opc_t OPC)
/* Replace the opcode of the instruction. This will also replace related info,
 * Size, Use and Chg, but it will NOT update any arguments or labels.
 */
{
    /* Get the opcode descriptor */
    const OPCDesc* D = GetOPCDesc (OPC);

    /* Replace the opcode */
    E->OPC  = OPC;
    E->Info = D->Info;
    E->Size = GetInsnSize (E->OPC, E->AM);
    SetUseChgInfo (E, D);
}



int CodeEntriesAreEqual (const CodeEntry* E1, const CodeEntry* E2)
/* Check if both code entries are equal */
{
    return E1->OPC == E2->OPC && E1->AM == E2->AM && strcmp (E1->Arg, E2->Arg) == 0;
}



void AttachCodeLabel (CodeEntry* E, CodeLabel* L)
/* Attach the label to the entry */
{
    /* Add it to the entries label list */
    CollAppend (&E->Labels, L);

    /* Tell the label about it's owner */
    L->Owner = E;
}



void MoveCodeLabel (CodeLabel* L, CodeEntry* E)
/* Move the code label L from it's former owner to the code entry E. */
{
    /* Delete the label from the owner */
    CollDeleteItem (&L->Owner->Labels, L);

    /* Set the new owner */
    CollAppend (&E->Labels, L);
    L->Owner = E;
}



void CodeEntrySetArg (CodeEntry* E, const char* Arg)
/* Set a new argument for the given code entry. An old string is deleted. */
{
    /* Free the old argument */
    FreeArg (E->Arg);

    /* Assign the new one */
    E->Arg = GetArgCopy (Arg);
}



void OutputCodeEntry (const CodeEntry* E, FILE* F)
/* Output the code entry to a file */
{
    const OPCDesc* D;
    unsigned Chars;
    const char* Target;

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

    	case AM65_IMP:
    	    /* implicit */
    	    break;

    	case AM65_ACC:
    	    /* accumulator */
    	    Chars += fprintf (F, "%*sa", 9-Chars, "");
    	    break;

    	case AM65_IMM:
    	    /* immidiate */
    	    Chars += fprintf (F, "%*s#%s", 9-Chars, "", E->Arg);
    	    break;

    	case AM65_ZP:
    	case AM65_ABS:
	    /* zeropage and absolute */
	    Chars += fprintf (F, "%*s%s", 9-Chars, "", E->Arg);
	    break;

	case AM65_ZPX:
	case AM65_ABSX:
	    /* zeropage,X and absolute,X */
	    Chars += fprintf (F, "%*s%s,x", 9-Chars, "", E->Arg);
	    break;

	case AM65_ABSY:
	    /* absolute,Y */
	    Chars += fprintf (F, "%*s%s,y", 9-Chars, "", E->Arg);
	    break;

	case AM65_ZPX_IND:
	    /* (zeropage,x) */
       	    Chars += fprintf (F, "%*s(%s,x)", 9-Chars, "", E->Arg);
	    break;

	case AM65_ZP_INDY:
	    /* (zeropage),y */
       	    Chars += fprintf (F, "%*s(%s),y", 9-Chars, "", E->Arg);
	    break;

	case AM65_ZP_IND:
	    /* (zeropage) */
       	    Chars += fprintf (F, "%*s(%s)", 9-Chars, "", E->Arg);
	    break;

	case AM65_BRA:
	    /* branch */
	    Target = E->JumpTo? E->JumpTo->Name : E->Arg;
	    Chars += fprintf (F, "%*s%s", 9-Chars, "", Target);
	    break;

	default:
	    Internal ("Invalid addressing mode");

    }

    /* Print usage info if requested by the debugging flag */
    if (Debug) {
  	fprintf (F,
		 "%*s; USE: %c%c%c CHG: %c%c%c SIZE: %u\n",
		 30-Chars, "",
		 (E->Use & REG_A)? 'A' : '_',
		 (E->Use & REG_X)? 'X' : '_',
		 (E->Use & REG_Y)? 'Y' : '_',
		 (E->Chg & REG_A)? 'A' : '_',
		 (E->Chg & REG_X)? 'X' : '_',
		 (E->Chg & REG_Y)? 'Y' : '_',
		 E->Size);
    } else {
	/* Terminate the line */
	fprintf (F, "\n");
    }
}





