/*****************************************************************************/
/*                                                                           */
/*				   codeseg.h				     */
/*                                                                           */
/*			    Code segment structure			     */
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



#ifndef CODESEG_H
#define CODESEG_H



#include <stdarg.h>
#include <stdio.h>

/* common */
#include "attrib.h"
#include "coll.h"

/* cc65 */
#include "codelab.h"
#include "symentry.h"



/*****************************************************************************/
/*  	       	     	  	     Data				     */
/*****************************************************************************/



/* Size of the label hash table */
#define CS_LABEL_HASH_SIZE	29

/* Code segment structure */
typedef struct CodeSeg CodeSeg;
struct CodeSeg {
    char*	SegName;  			/* Segment name */
    SymEntry*	Func;				/* Owner function */
    Collection	Entries;			/* List of code entries */
    Collection	Labels;				/* Labels for next insn */
    CodeLabel* 	LabelHash [CS_LABEL_HASH_SIZE];	/* Label hash table */
};



/*****************************************************************************/
/*     	       	      	  	     Code      		  		     */
/*****************************************************************************/



CodeSeg* NewCodeSeg (const char* SegName, SymEntry* Func);
/* Create a new code segment, initialize and return it */

void AddCodeEntry (CodeSeg* S, const char* Format, va_list ap) attribute ((format(printf,2,0)));
/* Add a line to the given code segment */

void DelCodeEntry (CodeSeg* S, unsigned Index);
/* Delete an entry from the code segment. This includes deleting any associated
 * labels, removing references to labels and even removing the referenced labels
 * if the reference count drops to zero.
 */

void AddCodeLabel (CodeSeg* S, const char* Name);
/* Add a code label for the next instruction to follow */

void DelCodeLabel (CodeSeg* S, CodeLabel* L);
/* Remove references from this label and delete it. */

void AddCodeSegHint (CodeSeg* S, unsigned Hint);
/* Add a hint for the preceeding instruction */

void DelCodeSegAfter (CodeSeg* S, unsigned Last);
/* Delete all entries including the given one */

void OutputCodeSeg (const CodeSeg* S, FILE* F);
/* Output the code segment data to a file */

CodeLabel* FindCodeLabel (CodeSeg* S, const char* Name, unsigned Hash);
/* Find the label with the given name. Return the label or NULL if not found */

void MergeCodeLabels (CodeSeg* S);
/* Merge code labels. That means: For each instruction, remove all labels but
 * one and adjust the code entries accordingly.
 */

unsigned GetCodeSegEntries (const CodeSeg* S);
/* Return the number of entries for the given code segment */



/* End of codeseg.h */
#endif



