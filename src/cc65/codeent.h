/*****************************************************************************/
/*                                                                           */
/*				   codeent.h				     */
/*                                                                           */
/*			      Code segment entry			     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2001-2002 Ullrich von Bassewitz                                       */
/*               Wacholderweg 14                                             */
/*               D-70597 Stuttgart                                           */
/* EMail:        uz@musoftware.de                                            */
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
#include <string.h>

/* common */
#include "coll.h"
#include "inline.h"

/* cc65 */
#include "codelab.h"
#include "lineinfo.h"
#include "opcodes.h"
#include "reginfo.h"



/*****************************************************************************/
/*  	       	     	  	     Data				     */
/*****************************************************************************/



/* Flags used */
#define CEF_USERMARK 	0x0001U		/* Generic mark by user functions */
#define CEF_NUMARG   	0x0002U		/* Insn has numerical argument */

/* Code entry structure */
typedef struct CodeEntry CodeEntry;
struct CodeEntry {
    opc_t	     	OPC;		/* Opcode */
    am_t	     	AM;		/* Adressing mode */
    char*      	       	Arg;   	       	/* Argument as string */
    unsigned long    	Num;		/* Numeric argument */
    unsigned short   	Flags;		/* Flags */
    unsigned short      Info;		/* Additional code info */
    unsigned short      Use;		/* Registers used */
    unsigned short      Chg;		/* Registers changed/destroyed */
    unsigned char    	Size;		/* Estimated size */
    CodeLabel*	     	JumpTo;		/* Jump label */
    Collection	     	Labels;		/* Labels for this instruction */
    LineInfo*           LI;             /* Source line info for this insn */
    RegInfo*            RI;             /* Register info for this insn */
};



/*****************************************************************************/
/*     	       	      	  	     Code				     */
/*****************************************************************************/



const char* MakeHexArg (unsigned Num);
/* Convert Num into a string in the form $XY, suitable for passing it as an
 * argument to NewCodeEntry, and return a pointer to the string.
 * BEWARE: The function returns a pointer to a static buffer, so the value is
 * gone if you call it twice (and apart from that it's not thread and signal
 * safe).
 */

CodeEntry* NewCodeEntry (opc_t OPC, am_t AM, const char* Arg,
   	       	    	 CodeLabel* JumpTo, LineInfo* LI);
/* Create a new code entry, initialize and return it */

void FreeCodeEntry (CodeEntry* E);
/* Free the given code entry */

void CE_ReplaceOPC (CodeEntry* E, opc_t OPC);
/* Replace the opcode of the instruction. This will also replace related info,
 * Size, Use and Chg, but it will NOT update any arguments or labels.
 */

int CodeEntriesAreEqual (const CodeEntry* E1, const CodeEntry* E2);
/* Check if both code entries are equal */

void CE_AttachLabel (CodeEntry* E, CodeLabel* L);
/* Attach the label to the entry */

#if defined(HAVE_INLINE)
INLINE int CE_HasLabel (const CodeEntry* E)
/* Check if the given code entry has labels attached */
{
    return (CollCount (&E->Labels) > 0);
}
#else
#  define CE_HasLabel(E)	(CollCount (&(E)->Labels) > 0)
#endif

#if defined(HAVE_INLINE)
INLINE unsigned CE_GetLabelCount (const CodeEntry* E)
/* Get the number of labels attached to this entry */
{
    return CollCount (&E->Labels);
}
#else
#  define CE_GetLabelCount(E)	CollCount (&(E)->Labels)
#endif

#if defined(HAVE_INLINE)
INLINE CodeLabel* CE_GetLabel (CodeEntry* E, unsigned Index)
/* Get a label from this code entry */
{
    return CollAt (&E->Labels, Index);
}
#else
#  define CE_GetLabel(E, Index)	CollAt (&(E)->Labels, (Index))
#endif

void CE_MoveLabel (CodeLabel* L, CodeEntry* E);
/* Move the code label L from it's former owner to the code entry E. */

#if defined(HAVE_INLINE)
INLINE int CE_HasMark (const CodeEntry* E)
/* Return true if the given code entry has the CEF_USERMARK flag set */
{
    return (E->Flags & CEF_USERMARK) != 0;
}
#else
#  define CE_HasMark(E)	(((E)->Flags & CEF_USERMARK) != 0)
#endif

#if defined(HAVE_INLINE)
INLINE void CE_SetMark (CodeEntry* E)
/* Set the CEF_USERMARK flag for the given entry */
{
    E->Flags |= CEF_USERMARK;
}
#else
#  define CE_SetMark(E)	((E)->Flags |= CEF_USERMARK)
#endif

#if defined(HAVE_INLINE)
INLINE void CE_ResetMark (CodeEntry* E)
/* Reset the CEF_USERMARK flag for the given entry */
{
    E->Flags &= ~CEF_USERMARK;
}
#else
#  define CE_ResetMark(E)	((E)->Flags &= ~CEF_USERMARK)
#endif

void CE_SetNumArg (CodeEntry* E, long Num);
/* Set a new numeric argument for the given code entry that must already
 * have a numeric argument.
 */

int CE_KnownImm (const CodeEntry* E);
/* Return true if the argument of E is a known immediate value */

#if defined(HAVE_INLINE)
INLINE int CE_IsCallTo (const CodeEntry* E, const char* Name)
/* Check if this is a call to the given function */
{
    return (E->OPC == OP65_JSR && strcmp (E->Arg, Name) == 0);
}
#else
#  define CE_IsCallTo(E, Name) ((E)->OPC == OP65_JSR && strcmp ((E)->Arg, (Name)) == 0)
#endif

int CE_UseLoadFlags (const CodeEntry* E);
/* Return true if the instruction uses any flags that are set by a load of
 * a register (N and Z).
 */

void CE_FreeRegInfo (CodeEntry* E);
/* Free an existing register info struct */

void CE_GenRegInfo (CodeEntry* E, RegContents* InputRegs);
/* Generate register info for this instruction. If an old info exists, it is
 * overwritten.
 */

void CE_Output (const CodeEntry* E, FILE* F);
/* Output the code entry to a file */



/* End of codeent.h */
#endif




