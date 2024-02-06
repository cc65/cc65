/*****************************************************************************/
/*                                                                           */
/*                                 codeseg.h                                 */
/*                                                                           */
/*                          Code segment structure                           */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2001-2009, Ullrich von Bassewitz                                      */
/*                Roemerstrasse 52                                           */
/*                D-70794 Filderstadt                                        */
/* EMail:         uz@cc65.org                                                */
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

/* common */
#include "attrib.h"
#include "coll.h"
#include "inline.h"

/* cc65 */
#include "codelab.h"
#include "lineinfo.h"
#include "symentry.h"



/*****************************************************************************/
/*                                 Forwards                                  */
/*****************************************************************************/



struct CodeEntry;



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Size of the label hash table */
#define CS_LABEL_HASH_SIZE      29

/* Code segment structure */
typedef struct CodeSeg CodeSeg;
struct CodeSeg {
    char*           SegName;                    /* Segment name */
    SymEntry*       Func;                       /* Owner function */
    Collection      Entries;                    /* List of code entries */
    Collection      Labels;                     /* Labels for next insn */
    CodeLabel*      LabelHash[CS_LABEL_HASH_SIZE]; /* Label hash table */
    unsigned short  ExitRegs;                   /* Register use on exit */

    /* Optimization settings for this segment */
    unsigned char   Optimize;                   /* On/off switch */
    unsigned        CodeSizeFactor;
};



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



CodeSeg* NewCodeSeg (const char* SegName, SymEntry* Func);
/* Create a new code segment, initialize and return it */

void CS_AddEntry (CodeSeg* S, struct CodeEntry* E);
/* Add an entry to the given code segment */

void CS_AddVLine (CodeSeg* S, LineInfo* LI, const char* Format, va_list ap) attribute ((format(printf,3,0)));
/* Add a line to the given code segment */

void CS_AddLine (CodeSeg* S, LineInfo* LI, const char* Format, ...) attribute ((format(printf,3,4)));
/* Add a line to the given code segment */

#if defined(HAVE_INLINE)
INLINE unsigned CS_GetEntryCount (const CodeSeg* S)
/* Return the number of entries for the given code segment */
{
    return CollCount (&S->Entries);
}
#else
#  define CS_GetEntryCount(S)   CollCount (&(S)->Entries)
#endif

void CS_InsertEntry (CodeSeg* S, struct CodeEntry* E, unsigned Index);
/* Insert the code entry at the index given. Following code entries will be
** moved to slots with higher indices.
*/

void CS_DelEntry (CodeSeg* S, unsigned Index);
/* Delete an entry from the code segment. This includes moving any associated
** labels, removing references to labels and even removing the referenced labels
** if the reference count drops to zero.
** Note: Labels are moved forward if possible, that is, they are moved to the
** next insn (not the preceeding one).
*/

void CS_DelEntries (CodeSeg* S, unsigned Start, unsigned Count);
/* Delete a range of code entries. This includes removing references to labels,
** labels attached to the entries and so on.
*/

void CS_MoveEntries (CodeSeg* S, unsigned Start, unsigned Count, unsigned NewPos);
/* Move a range of entries from one position to another. Start is the index
** of the first entry to move, Count is the number of entries and NewPos is
** the index of the target entry. The entry with the index Start will later
** have the index NewPos. All entries with indices NewPos and above are
** moved to higher indices. If the code block is moved to the end of the
** current code, and if pending labels exist, these labels will get attached
** to the first instruction of the moved block (the first one after the
** current code end)
*/

#if defined(HAVE_INLINE)
INLINE void CS_MoveEntry (CodeSeg* S, unsigned OldPos, unsigned NewPos)
/* Move an entry from one position to another. OldPos is the current position
** of the entry, NewPos is the new position of the entry.
*/
{
    CollMove (&S->Entries, OldPos, NewPos);
}
#else
#  define CS_MoveEntry(S, OldPos, NewPos)       CollMove (&(S)->Entries, OldPos, NewPos)
#endif

#if defined(HAVE_INLINE)
INLINE struct CodeEntry* CS_GetEntry (CodeSeg* S, unsigned Index)
/* Get an entry from the given code segment */
{
    return CollAt (&S->Entries, Index);
}
#else
#  define CS_GetEntry(S, Index) ((struct CodeEntry*) CollAt(&(S)->Entries, (Index)))
#endif

struct CodeEntry* CS_GetPrevEntry (CodeSeg* S, unsigned Index);
/* Get the code entry preceeding the one with the index Index. If there is no
** preceeding code entry, return NULL.
*/

struct CodeEntry* CS_GetNextEntry (CodeSeg* S, unsigned Index);
/* Get the code entry following the one with the index Index. If there is no
** following code entry, return NULL.
*/

int CS_GetEntries (CodeSeg* S, struct CodeEntry** List,
                   unsigned Start, unsigned Count);
/* Get Count code entries into List starting at index start. Return true if
** we got the lines, return false if not enough lines were available.
*/

unsigned CS_GetEntryIndex (CodeSeg* S, struct CodeEntry* E);
/* Return the index of a code entry */

int CS_RangeHasLabel (CodeSeg* S, unsigned Start, unsigned Count);
/* Return true if any of the code entries in the given range has a label
** attached. If the code segment does not span the given range, check the
** possible span instead.
*/

#if defined(HAVE_INLINE)
INLINE int CS_HavePendingLabel (const CodeSeg* S)
/* Return true if there are open labels that will get attached to the next
** instruction that is added.
*/
{
    return (CollCount (&S->Labels) > 0);
}
#else
#  define CS_HavePendingLabel(S)        (CollCount (&(S)->Labels) > 0)
#endif

CodeLabel* CS_AddLabel (CodeSeg* S, const char* Name);
/* Add a code label for the next instruction to follow */

CodeLabel* CS_GenLabel (CodeSeg* S, struct CodeEntry* E);
/* If the code entry E does already have a label, return it. Otherwise
** create a new label, attach it to E and return it.
*/

void CS_DelLabel (CodeSeg* S, CodeLabel* L);
/* Remove references from this label and delete it. */

void CS_MergeLabels (CodeSeg* S);
/* Merge code labels. That means: For each instruction, remove all labels but
** one and adjust references accordingly.
*/

void CS_MoveLabels (CodeSeg* S, struct CodeEntry* Old, struct CodeEntry* New);
/* Move all labels from Old to New. The routine will move the labels itself
** if New does not have any labels, and move references if there is at least
** a label for new. If references are moved, the old label is deleted
** afterwards.
*/

void CS_RemoveLabelRef (CodeSeg* S, struct CodeEntry* E);
/* Remove the reference between E and the label it jumps to. The reference
** will be removed on both sides and E->JumpTo will be 0 after that. If
** the reference was the only one for the label, the label will get
** deleted.
*/

void CS_MoveLabelRef (CodeSeg* S, struct CodeEntry* E, CodeLabel* L);
/* Change the reference of E to L instead of the current one. If this
** was the only reference to the old label, the old label will get
** deleted.
*/

void CS_DelCodeRange (CodeSeg* S, unsigned First, unsigned Last);
/* Delete all entries between first and last, both inclusive. The function
** can only handle basic blocks (First is the only entry, Last the only exit)
** and no open labels. It will call FAIL if any of these preconditions are
** violated.
*/

void CS_DelCodeAfter (CodeSeg* S, unsigned Last);
/* Delete all entries including the given one */

void CS_ResetMarks (CodeSeg* S, unsigned First, unsigned Last);
/* Remove all user marks from the entries in the given range */

#if defined(HAVE_INLINE)
INLINE void CS_ResetAllMarks (CodeSeg* S)
/* Remove all user marks from the code segment */
{
    if (CS_GetEntryCount (S) > 0) {
        CS_ResetMarks (S, 0, CS_GetEntryCount (S));
    }
}
#else
#  define CS_ResetAllMarks(S) \
        ((CS_GetEntryCount (S) > 0)? CS_ResetMarks (S, 0, CS_GetEntryCount (S)) : (void) 0)
#endif

int CS_IsBasicBlock (CodeSeg* S, unsigned First, unsigned Last);
/* Check if the given code segment range is a basic block. That is, check if
** First is the only entrance and Last is the only exit. This means that no
** jump/branch inside the block may jump to an insn below First or after(!)
** Last, and that no insn may jump into this block from the outside.
*/

void CS_OutputPrologue (const CodeSeg* S);
/* If the given code segment is a code segment for a function, output the
** assembler prologue into the file. That is: Output a comment header, switch
** to the correct segment and enter the local function scope. If the code
** segment is global, do nothing.
*/

void CS_OutputEpilogue (const CodeSeg* S);
/* If the given code segment is a code segment for a function, output the
** assembler epilogue into the file. That is: Close the local function scope.
*/

void CS_Output (CodeSeg* S);
/* Output the code segment data to a file */

void CS_FreeRegInfo (CodeSeg* S);
/* Free register infos for all instructions */

void CS_GenRegInfo (CodeSeg* S);
/* Generate register infos for all instructions */



/* End of codeseg.h */

#endif
