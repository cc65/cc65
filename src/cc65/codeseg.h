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
#include "inline.h"

/* cc65 */
#include "codelab.h"
#include "symentry.h"



/*****************************************************************************/
/*				   Forwards				     */
/*****************************************************************************/



struct CodeEntry;



/*****************************************************************************/
/*  	       	     	  	     Data				     */
/*****************************************************************************/



/* Size of the label hash table */
#define CS_LABEL_HASH_SIZE	29

/* Code segment structure */
typedef struct CodeSeg CodeSeg;
struct CodeSeg {
    char*	    SegName;  	  		/* Segment name */
    SymEntry*	    Func;	  		/* Owner function */
    Collection	    Entries;	  		/* List of code entries */
    Collection	    Labels;	  		/* Labels for next insn */
    CodeLabel* 	    LabelHash [CS_LABEL_HASH_SIZE]; /* Label hash table */
    unsigned char   ExitRegs;			/* Register use on exit */
};



/*****************************************************************************/
/*     	       	      	  	     Code      		  		     */
/*****************************************************************************/



CodeSeg* NewCodeSeg (const char* SegName, SymEntry* Func);
/* Create a new code segment, initialize and return it */

void AddCodeEntry (CodeSeg* S, const char* Format, va_list ap) attribute ((format(printf,2,0)));
/* Add a line to the given code segment */

void InsertCodeEntry (CodeSeg* S, struct CodeEntry* E, unsigned Index);
/* Insert the code entry at the index given. Following code entries will be
 * moved to slots with higher indices.
 */

void DelCodeEntry (CodeSeg* S, unsigned Index);
/* Delete an entry from the code segment. This includes moving any associated
 * labels, removing references to labels and even removing the referenced labels
 * if the reference count drops to zero.
 */

void DelCodeEntries (CodeSeg* S, unsigned Start, unsigned Count);
/* Delete a range of code entries. This includes removing references to labels,
 * labels attached to the entries and so on.
 */

struct CodeEntry* RetrieveCodeEntry (CodeSeg* S, unsigned Index);
/* Retrieve a code entry. This means, the code entry is removed from the
 * entry collection, but not deleted and returned instead. The entry may
 * then be inserted again at another position.
 */

#if defined(HAVE_INLINE)
INLINE struct CodeEntry* GetCodeEntry (CodeSeg* S, unsigned Index)
/* Get an entry from the given code segment */
{
    return CollAt (&S->Entries, Index);
}
#else
#  define GetCodeEntry(S, Index)	CollAt(&(S)->Entries, (Index))
#endif

struct CodeEntry* GetNextCodeEntry (CodeSeg* S, unsigned Index);
/* Get the code entry following the one with the index Index. If there is no
 * following code entry, return NULL.
 */

int GetCodeEntries (CodeSeg* S, struct CodeEntry** List,
       	 	    unsigned Start, unsigned Count);
/* Get Count code entries into List starting at index start. Return true if
 * we got the lines, return false if not enough lines were available.
 */

unsigned GetCodeEntryIndex (CodeSeg* S, struct CodeEntry* E);
/* Return the index of a code entry */

void AddCodeLabel (CodeSeg* S, const char* Name);
/* Add a code label for the next instruction to follow */

CodeLabel* GenCodeLabel (CodeSeg* S, struct CodeEntry* E);
/* If the code entry E does already have a label, return it. Otherwise
 * create a new label, attach it to E and return it.
 */

void DelCodeLabel (CodeSeg* S, CodeLabel* L);
/* Remove references from this label and delete it. */

void MergeCodeLabels (CodeSeg* S);
/* Merge code labels. That means: For each instruction, remove all labels but
 * one and adjust references accordingly.
 */

void MoveCodeLabels (CodeSeg* S, struct CodeEntry* Old, struct CodeEntry* New);
/* Move all labels from Old to New. The routine will move the labels itself
 * if New does not have any labels, and move references if there is at least
 * a label for new. If references are moved, the old label is deleted
 * afterwards.
 */

void RemoveCodeLabelRef (CodeSeg* S, struct CodeEntry* E);
/* Remove the reference between E and the label it jumps to. The reference
 * will be removed on both sides and E->JumpTo will be 0 after that. If
 * the reference was the only one for the label, the label will get
 * deleted.
 */

void MoveCodeLabelRef (CodeSeg* S, struct CodeEntry* E, CodeLabel* L);
/* Change the reference of E to L instead of the current one. If this
 * was the only reference to the old label, the old label will get
 * deleted.
 */

void AddCodeSegHint (CodeSeg* S, unsigned Hint);
/* Add a hint for the preceeding instruction */

void DelCodeSegAfter (CodeSeg* S, unsigned Last);
/* Delete all entries including the given one */

void OutputCodeSeg (const CodeSeg* S, FILE* F);
/* Output the code segment data to a file */

#if defined(HAVE_INLINE)
INLINE unsigned GetCodeEntryCount (const CodeSeg* S)
/* Return the number of entries for the given code segment */
{
    return CollCount (&S->Entries);
}
#else
#  define GetCodeEntryCount(S)	CollCount (&(S)->Entries)
#endif



/* End of codeseg.h */
#endif



