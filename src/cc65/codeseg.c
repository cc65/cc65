/*****************************************************************************/
/*                                                                           */
/*				   codeseg.c				     */
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



#include <string.h>
#include <ctype.h>

/* common */
#include "chartype.h"
#include "check.h"
#include "hashstr.h"
#include "strutil.h"
#include "xmalloc.h"
#include "xsprintf.h"

/* cc65 */
#include "asmlabel.h"
#include "codeent.h"
#include "codeinfo.h"
#include "datatype.h"
#include "error.h"
#include "symentry.h"
#include "codeseg.h"



/*****************************************************************************/
/*	    		       Helper functions				     */
/*****************************************************************************/



static void MoveLabelsToPool (CodeSeg* S, CodeEntry* E)
/* Move the labels of the code entry E to the label pool of the code segment */
{
    unsigned LabelCount = GetCodeLabelCount (E);
    while (LabelCount--) {
	CodeLabel* L = GetCodeLabel (E, LabelCount);
	L->Flags &= ~LF_DEF;
	L->Owner = 0;
	CollAppend (&S->Labels, L);
    }
    CollDeleteAll (&E->Labels);
}



static CodeLabel* FindCodeLabel (CodeSeg* S, const char* Name, unsigned Hash)
/* Find the label with the given name. Return the label or NULL if not found */
{
    /* Get the first hash chain entry */
    CodeLabel* L = S->LabelHash[Hash];

    /* Search the list */
    while (L) {
	if (strcmp (Name, L->Name) == 0) {
	    /* Found */
	    break;
      	}
	L = L->Next;
    }
    return L;
}



static CodeLabel* NewCodeSegLabel (CodeSeg* S, const char* Name, unsigned Hash)
/* Create a new label and insert it into the label hash table */
{
    /* Not found - create a new one */
    CodeLabel* L = NewCodeLabel (Name, Hash);

    /* Enter the label into the hash table */
    L->Next = S->LabelHash[L->Hash];
    S->LabelHash[L->Hash] = L;

    /* Return the new label */
    return L;
}



static void RemoveLabelFromHash (CodeSeg* S, CodeLabel* L)
/* Remove the given code label from the hash list */
{
    /* Get the first entry in the hash chain */
    CodeLabel* List = S->LabelHash[L->Hash];
    CHECK (List != 0);

    /* First, remove the label from the hash chain */
    if (List == L) {
       	/* First entry in hash chain */
       	S->LabelHash[L->Hash] = L->Next;
    } else {
       	/* Must search through the chain */
       	while (List->Next != L) {
       	    /* If we've reached the end of the chain, something is *really* wrong */
       	    CHECK (List->Next != 0);
       	    /* Next entry */
       	    List = List->Next;
       	}
       	/* The next entry is the one, we have been searching for */
       	List->Next = L->Next;
    }
}



/*****************************************************************************/
/*		      Functions for parsing instructions		     */
/*****************************************************************************/



static const char* SkipSpace (const char* S)
/* Skip white space and return an updated pointer */
{
    while (IsSpace (*S)) {
	++S;
    }
    return S;
}



static const char* ReadToken (const char* L, const char* Term,
		     	      char* Buf, unsigned BufSize)
/* Read the next token into Buf, return the updated line pointer. The
 * token is terminated by one of the characters given in term.
 */
{
    /* Read/copy the token */
    unsigned I = 0;
    unsigned ParenCount = 0;
    while (*L && (ParenCount > 0 || strchr (Term, *L) == 0)) {
	if (I < BufSize-1) {
	    Buf[I++] = *L;
	}
	if (*L == ')') {
	    --ParenCount;
	} else if (*L == '(') {
	    ++ParenCount;
	}
	++L;
    }

    /* Terminate the buffer contents */
    Buf[I] = '\0';

    /* Return the updated line pointer */
    return L;
}



static CodeEntry* ParseInsn (CodeSeg* S, const char* L)
/* Parse an instruction nnd generate a code entry from it. If the line contains
 * errors, output an error message and return NULL.
 * For simplicity, we don't accept the broad range of input a "real" assembler
 * does. The instruction and the argument are expected to be separated by
 * white space, for example.
 */
{
    char       		Mnemo[16];
    const OPCDesc*	OPC;
    am_t     		AM = 0;		/* Initialize to keep gcc silent */
    char     		Arg[64];
    char     	   	Reg;
    CodeEntry*	     	E;
    CodeLabel*		Label;

    /* Mnemonic */
    L = ReadToken (L, " \t", Mnemo, sizeof (Mnemo));

    /* Try to find the opcode description for the mnemonic */
    OPC = FindOpcode (Mnemo);

    /* If we didn't find the opcode, print an error and bail out */
    if (OPC == 0) {
	Error ("ASM code error: %s is not a valid mnemonic", Mnemo);
	return 0;
    }

    /* Skip separator white space */
    L = SkipSpace (L);

    /* Get the addressing mode */
    Arg[0] = '\0';
    switch (*L) {

	case '\0':
	    /* Implicit */
	    AM = AM_IMP;
	    break;

	case '#':
	    /* Immidiate */
	    StrCopy (Arg, sizeof (Arg), L+1);
	    AM = AM_IMM;
	    break;

	case '(':
	    /* Indirect */
	    L = ReadToken (L+1, ",)", Arg, sizeof (Arg));

	    /* Check for errors */
	    if (*L == '\0') {
	     	Error ("ASM code error: syntax error");
	     	return 0;
	    }

	    /* Check the different indirect modes */
       	    if (*L == ',') {
	     	/* Expect zp x indirect */
	     	L = SkipSpace (L+1);
	     	if (toupper (*L) != 'X') {
	     	    Error ("ASM code error: `X' expected");
	     	    return 0;
	     	}
	     	L = SkipSpace (L+1);
	     	if (*L != ')') {
	     	    Error ("ASM code error: `)' expected");
	     	    return 0;
	     	}
	     	L = SkipSpace (L+1);
	     	if (*L != '\0') {
	     	    Error ("ASM code error: syntax error");
	     	    return 0;
	     	}
	     	AM = AM_ZPX_IND;
	    } else if (*L == ')') {
	     	/* zp indirect or zp indirect, y */
	     	L = SkipSpace (L+1);
	      	if (*L == ',') {
	     	    L = SkipSpace (L+1);
	     	    if (toupper (*L) != 'Y') {
	     		Error ("ASM code error: `Y' expected");
	     		return 0;
	     	    }
	     	    L = SkipSpace (L+1);
	     	    if (*L != '\0') {
	       	    	Error ("ASM code error: syntax error");
	     	    	return 0;
	     	    }
	     	    AM = AM_ZP_INDY;
	     	} else if (*L == '\0') {
	     	    AM = AM_ZP_IND;
	     	} else {
	     	    Error ("ASM code error: syntax error");
	     	    return 0;
	     	}
	    }
	    break;

	case 'a':
       	case 'A':
	    /* Accumulator? */
	    if (L[1] == '\0') {
	     	AM = AM_ACC;
	     	break;
	    }
	    /* FALLTHROUGH */

	default:
	    /* Absolute, maybe indexed */
	    L = ReadToken (L, ",", Arg, sizeof (Arg));
	    if (*L == '\0') {
	     	/* Absolute, zeropage or branch */
		if ((OPC->Info & OF_BRA) != 0) {
		    /* Branch */
		    AM = AM_BRA;
		} else if (IsZPName (Arg)) {
		    AM = AM_ZP;
		} else {
		    AM = AM_ABS;
		}
	    } else if (*L == ',') {
		/* Indexed */
		L = SkipSpace (L+1);
		if (*L == '\0') {
      		    Error ("ASM code error: syntax error");
		    return 0;
		} else {
	      	    Reg = toupper (*L);
		    L = SkipSpace (L+1);
		    if (Reg == 'X') {
			if (IsZPName (Arg)) {
			    AM = AM_ZPX;
			} else {
			    AM = AM_ABSX;
			}
		    } else if (Reg == 'Y') {
		     	AM = AM_ABSY;
		    } else {
		     	Error ("ASM code error: syntax error");
	       	     	return 0;
		    }
		    if (*L != '\0') {
	    	     	Error ("ASM code error: syntax error");
	    	     	return 0;
	    	    }
	    	}
	    }
	    break;

    }

    /* If the instruction is a branch, check for the label and generate it
     * if it does not exist. Ignore anything but local labels here.
     */
    Label = 0;
    if (AM == AM_BRA && Arg[0] == 'L') {

	/* Generate the hash over the label, then search for the label */
	unsigned Hash = HashStr (Arg) % CS_LABEL_HASH_SIZE;
	Label = FindCodeLabel (S, Arg, Hash);

	/* If we don't have the label, it's a forward ref - create it */
	if (Label == 0) {
	    /* Generate a new label */
	    Label = NewCodeSegLabel (S, Arg, Hash);
	}
    }

    /* We do now have the addressing mode in AM. Allocate a new CodeEntry
     * structure and initialize it.
     */
    E = NewCodeEntry (OPC->OPC, AM, Arg, Label);

    /* Return the new code entry */
    return E;
}



/*****************************************************************************/
/*     	       	      	       	     Code				     */
/*****************************************************************************/



CodeSeg* NewCodeSeg (const char* SegName, SymEntry* Func)
/* Create a new code segment, initialize and return it */
{
    unsigned I;

    /* Allocate memory */
    CodeSeg* S = xmalloc (sizeof (CodeSeg));

    /* Initialize the fields */
    S->SegName  = xstrdup (SegName);
    S->Func	= Func;
    InitCollection (&S->Entries);
    InitCollection (&S->Labels);
    for (I = 0; I < sizeof(S->LabelHash) / sizeof(S->LabelHash[0]); ++I) {
	S->LabelHash[I] = 0;
    }

    /* If we have a function given, get the return type of the function.
     * Assume ANY return type besides void will use the A and X registers.
     */
    if (S->Func && !IsTypeVoid (GetFuncReturn (Func->Type))) {
	S->ExitRegs = REG_AX;
    } else {
	S->ExitRegs = REG_NONE;
    }

    /* Return the new struct */
    return S;
}



void AddCodeEntry (CodeSeg* S, const char* Format, va_list ap)
/* Add a line to the given code segment */
{
    const char* L;
    CodeEntry*  E;
    char	Token[64];

    /* Format the line */
    char Buf [256];
    xvsprintf (Buf, sizeof (Buf), Format, ap);

    /* Skip whitespace */
    L = SkipSpace (Buf);

    /* Check which type of instruction we have */
    E = 0;	/* Assume no insn created */
    switch (*L) {

    	case '\0':
	    /* Empty line, just ignore it */
	    break;

	case ';':
	    /* Comment or hint, ignore it for now */
     	    break;

	case '.':
	    /* Control instruction */
	    ReadToken (L, " \t", Token, sizeof (Token));
     	    Error ("ASM code error: Pseudo instruction `%s' not supported", Token);
	    break;

	default:
	    E = ParseInsn (S, L);
	    break;
    }

    /* If we have a code entry, transfer the labels and insert it */
    if (E) {

	/* Transfer the labels if we have any */
	unsigned I;
	unsigned LabelCount = CollCount (&S->Labels);
	for (I = 0; I < LabelCount; ++I) {

	    /* Get the label */
	    CodeLabel* L = CollAt (&S->Labels, I);

	    /* Attach it to the entry */
	    AttachCodeLabel (E, L);
	}

	/* Delete the transfered labels */
	CollDeleteAll (&S->Labels);

	/* Add the entry to the list of code entries in this segment */
      	CollAppend (&S->Entries, E);

    }
}



void InsertCodeEntry (CodeSeg* S, struct CodeEntry* E, unsigned Index)
/* Insert the code entry at the index given. Following code entries will be
 * moved to slots with higher indices.
 */
{
    /* Insert the entry into the collection */
    CollInsert (&S->Entries, E, Index);
}



void DelCodeEntry (CodeSeg* S, unsigned Index)
/* Delete an entry from the code segment. This includes moving any associated
 * labels, removing references to labels and even removing the referenced labels
 * if the reference count drops to zero.
 */
{
    /* Get the code entry for the given index */
    CodeEntry* E = GetCodeEntry (S, Index);

    /* If the entry has a labels, we have to move this label to the next insn.
     * If there is no next insn, move the label into the code segement label
     * pool. The operation is further complicated by the fact that the next
     * insn may already have a label. In that case change all reference to
     * this label and delete the label instead of moving it.
     */
    unsigned Count = GetCodeLabelCount (E);
    if (Count > 0) {

     	/* The instruction has labels attached. Check if there is a next
     	 * instruction.
     	 */
     	if (Index == GetCodeEntryCount (S)-1) {

     	    /* No next instruction, move to the codeseg label pool */
     	    MoveLabelsToPool (S, E);

     	} else {

     	    /* There is a next insn, get it */
     	    CodeEntry* N = GetCodeEntry (S, Index+1);

     	    /* Move labels to the next entry */
     	    MoveCodeLabels (S, E, N);

     	}
    }

    /* If this insn references a label, remove the reference. And, if the
     * the reference count for this label drops to zero, remove this label.
     */
    if (E->JumpTo) {
       	/* Remove the reference */
       	RemoveCodeLabelRef (S, E);
    }

    /* Delete the pointer to the insn */
    CollDelete (&S->Entries, Index);

    /* Delete the instruction itself */
    FreeCodeEntry (E);
}



void DelCodeEntries (CodeSeg* S, unsigned Start, unsigned Count)
/* Delete a range of code entries. This includes removing references to labels,
 * labels attached to the entries and so on.
 */
{
    /* Start deleting the entries from the rear, because this involves less
     * memory moving.
     */
    while (Count--) {
	DelCodeEntry (S, Start + Count);
    }
}



void MoveCodeEntry (CodeSeg* S, unsigned OldPos, unsigned NewPos)
/* Move an entry from one position to another. OldPos is the current position
 * of the entry, NewPos is the new position of the entry.
 */
{
    /* Get the code entry and remove it from the collection */
    CodeEntry* E = GetCodeEntry (S, OldPos);
    CollDelete (&S->Entries, OldPos);

    /* Correct NewPos if needed */
    if (NewPos >= OldPos) {
	/* Position has changed with removal */
	--NewPos;
    }

    /* Now insert it at the new position */
    CollInsert (&S->Entries, E, NewPos);
}



struct CodeEntry* GetNextCodeEntry (CodeSeg* S, unsigned Index)
/* Get the code entry following the one with the index Index. If there is no
 * following code entry, return NULL.
 */
{
    if (Index >= CollCount (&S->Entries)-1) {
	/* This is the last entry */
	return 0;
    } else {
	/* Code entries left */
       	return CollAtUnchecked (&S->Entries, Index+1);
    }
}



int GetCodeEntries (CodeSeg* S, struct CodeEntry** List,
       		    unsigned Start, unsigned Count)
/* Get Count code entries into List starting at index start. Return true if
 * we got the lines, return false if not enough lines were available.
 */
{
    /* Check if enough entries are available */
    if (Start + Count > CollCount (&S->Entries)) {
	return 0;
    }

    /* Copy the entries */
    while (Count--) {
	*List++ = CollAtUnchecked (&S->Entries, Start++);
    }

    /* We have the entries */
    return 1;
}



unsigned GetCodeEntryIndex (CodeSeg* S, struct CodeEntry* E)
/* Return the index of a code entry */
{
    int Index = CollIndex (&S->Entries, E);
    CHECK (Index >= 0);
    return Index;
}



void AddCodeLabel (CodeSeg* S, const char* Name)
/* Add a code label for the next instruction to follow */
{
    /* Calculate the hash from the name */
    unsigned Hash = HashStr (Name) % CS_LABEL_HASH_SIZE;

    /* Try to find the code label if it does already exist */
    CodeLabel* L = FindCodeLabel (S, Name, Hash);

    /* Did we find it? */
    if (L) {
     	/* We found it - be sure it does not already have an owner */
     	CHECK (L->Owner == 0);
    } else {
     	/* Not found - create a new one */
     	L = NewCodeSegLabel (S, Name, Hash);
    }

    /* Safety. This call is quite costly, but safety is better */
    if (CollIndex (&S->Labels, L) >= 0) {
     	Internal ("AddCodeLabel: Label `%s' already defined", Name);
    }

    /* We do now have a valid label. Remember it for later */
    CollAppend (&S->Labels, L);
}



CodeLabel* GenCodeLabel (CodeSeg* S, struct CodeEntry* E)
/* If the code entry E does already have a label, return it. Otherwise
 * create a new label, attach it to E and return it.
 */
{
    CodeLabel* L;

    if (CodeEntryHasLabel (E)) {

	/* Get the label from this entry */
	L = GetCodeLabel (E, 0);

    } else {

	/* Get a new name */
	const char* Name = LocalLabelName (GetLocalLabel ());

	/* Generate the hash over the name */
	unsigned Hash = HashStr (Name) % CS_LABEL_HASH_SIZE;

	/* Create a new label */
	L = NewCodeSegLabel (S, Name, Hash);

	/* Attach this label to the code entry */
	AttachCodeLabel (E, L);

    }

    /* Return the label */
    return L;
}



void DelCodeLabel (CodeSeg* S, CodeLabel* L)
/* Remove references from this label and delete it. */
{
    unsigned Count, I;

    /* First, remove the label from the hash chain */
    RemoveLabelFromHash (S, L);

    /* Remove references from insns jumping to this label */
    Count = CollCount (&L->JumpFrom);
    for (I = 0; I < Count; ++I) {
       	/* Get the insn referencing this label */
       	CodeEntry* E = CollAt (&L->JumpFrom, I);
       	/* Remove the reference */
       	E->JumpTo = 0;
    }
    CollDeleteAll (&L->JumpFrom);

    /* Remove the reference to the owning instruction if it has one. The
     * function may be called for a label without an owner when deleting
     * unfinished parts of the code. This is unfortunate since it allows
     * errors to slip through.
     */
    if (L->Owner) {
     	CollDeleteItem (&L->Owner->Labels, L);
    }

    /* All references removed, delete the label itself */
    FreeCodeLabel (L);
}



void MergeCodeLabels (CodeSeg* S)
/* Merge code labels. That means: For each instruction, remove all labels but
 * one and adjust references accordingly.
 */
{
    unsigned I;

    /* Walk over all code entries */
    unsigned EntryCount = GetCodeEntryCount (S);
    for (I = 0; I < EntryCount; ++I) {

       	CodeLabel* RefLab;
	unsigned   J;

	/* Get a pointer to the next entry */
	CodeEntry* E = GetCodeEntry (S, I);

     	/* If this entry has zero labels, continue with the next one */
    	unsigned LabelCount = GetCodeLabelCount (E);
    	if (LabelCount == 0) {
    	    continue;
    	}

    	/* We have at least one label. Use the first one as reference label. */
    	RefLab = GetCodeLabel (E, 0);

    	/* Walk through the remaining labels and change references to these
    	 * labels to a reference to the one and only label. Delete the labels
    	 * that are no longer used. To increase performance, walk backwards
    	 * through the list.
    	 */
      	for (J = LabelCount-1; J >= 1; --J) {

    	    /* Get the next label */
    	    CodeLabel* L = GetCodeLabel (E, J);

	    /* Move all references from this label to the reference label */
     	    MoveLabelRefs (L, RefLab);

       	    /* Remove the label completely. */
       	    DelCodeLabel (S, L);
     	}

    	/* The reference label is the only remaining label. Check if there
	 * are any references to this label, and delete it if this is not
	 * the case.
	 */
       	if (CollCount (&RefLab->JumpFrom) == 0) {
     	    /* Delete the label */
       	    DelCodeLabel (S, RefLab);
     	}
    }
}



void MoveCodeLabels (CodeSeg* S, struct CodeEntry* Old, struct CodeEntry* New)
/* Move all labels from Old to New. The routine will move the labels itself
 * if New does not have any labels, and move references if there is at least
 * a label for new. If references are moved, the old label is deleted
 * afterwards.
 */
{
    /* Get the number of labels to move */
    unsigned OldLabelCount = GetCodeLabelCount (Old);

    /* Does the new entry have itself a label? */
    if (CodeEntryHasLabel (New)) {

	/* The new entry does already have a label - move references */
	CodeLabel* NewLabel = GetCodeLabel (New, 0);
	while (OldLabelCount--) {

	    /* Get the next label */
	    CodeLabel* OldLabel = GetCodeLabel (Old, OldLabelCount);

	    /* Move references */
	    MoveLabelRefs (OldLabel, NewLabel);

	    /* Delete the label */
	    DelCodeLabel (S, OldLabel);

     	}

    } else {

	/* The new entry does not have a label, just move them */
	while (OldLabelCount--) {

	    /* Move the label to the new entry */
	    MoveCodeLabel (GetCodeLabel (Old, OldLabelCount), New);

	}

    }
}



void RemoveCodeLabelRef (CodeSeg* S, struct CodeEntry* E)
/* Remove the reference between E and the label it jumps to. The reference
 * will be removed on both sides and E->JumpTo will be 0 after that. If
 * the reference was the only one for the label, the label will get
 * deleted.
 */
{
    /* Get a pointer to the label and make sure it exists */
    CodeLabel* L = E->JumpTo;
    CHECK (L != 0);

    /* Delete the entry from the label */
    CollDeleteItem (&L->JumpFrom, E);

    /* The entry jumps no longer to L */
    E->JumpTo = 0;

    /* If there are no more references, delete the label */
    if (CollCount (&L->JumpFrom) == 0) {
       	DelCodeLabel (S, L);
    }
}



void MoveCodeLabelRef (CodeSeg* S, struct CodeEntry* E, CodeLabel* L)
/* Change the reference of E to L instead of the current one. If this
 * was the only reference to the old label, the old label will get
 * deleted.
 */
{
    /* Get the old label */
    CodeLabel* OldLabel = E->JumpTo;

    /* Be sure that code entry references a label */
    PRECONDITION (OldLabel != 0);

    /* Remove the reference to our label */
    RemoveCodeLabelRef (S, E);

    /* Use the new label */
    AddLabelRef (L, E);
}



void AddCodeSegHint (CodeSeg* S, unsigned Hint)
/* Add a hint for the preceeding instruction */
{
    CodeEntry* E;

    /* Get the number of entries in this segment */
    unsigned EntryCount = GetCodeEntryCount (S);

    /* Must have at least one entry */
    CHECK (EntryCount > 0);

    /* Get the last entry */
    E = GetCodeEntry (S, EntryCount-1);

    /* Add the hint */
    E->Hints |= Hint;
}



void DelCodeSegAfter (CodeSeg* S, unsigned Last)
/* Delete all entries including the given one */
{
    /* Get the number of entries in this segment */
    unsigned Count = GetCodeEntryCount (S);

    /* First pass: Delete all references to labels. If the reference count
     * for a label drops to zero, delete it.
     */
    unsigned C = Count;
    while (Last < C--) {

	/* Get the next entry */
	CodeEntry* E = GetCodeEntry (S, C);

	/* Check if this entry has a label reference */
	if (E->JumpTo) {
	    /* If the label is a label in the label pool and this is the last
	     * reference to the label, remove the label from the pool.
	     */
	    CodeLabel* L = E->JumpTo;
	    int Index = CollIndex (&S->Labels, L);
	    if (Index >= 0 && CollCount (&L->JumpFrom) == 1) {
		/* Delete it from the pool */
		CollDelete (&S->Labels, Index);
	    }

	    /* Remove the reference to the label */
	    RemoveCodeLabelRef (S, E);
	}

    }

    /* Second pass: Delete the instructions. If a label attached to an
     * instruction still has references, it must be references from outside
     * the deleted area. Don't delete the label in this case, just make it
     * ownerless and move it to the label pool.
     */
    C = Count;
    while (Last < C--) {

    	/* Get the next entry */
    	CodeEntry* E = GetCodeEntry (S, C);

    	/* Check if this entry has a label attached */
    	if (CodeEntryHasLabel (E)) {
	    /* Move the labels to the pool and clear the owner pointer */
	    MoveLabelsToPool (S, E);
	}

	/* Delete the pointer to the entry */
	CollDelete (&S->Entries, C);

	/* Delete the entry itself */
	FreeCodeEntry (E);
    }
}



void OutputCodeSeg (const CodeSeg* S, FILE* F)
/* Output the code segment data to a file */
{
    unsigned I;

    /* Get the number of entries in this segment */
    unsigned Count = GetCodeEntryCount (S);

    /* If the code segment is empty, bail out here */
    if (Count == 0) {
	return;
    }

    /* Output the segment directive */
    fprintf (F, ".segment\t\"%s\"\n\n", S->SegName);

    /* If this is a segment for a function, enter a function */
    if (S->Func) {
	fprintf (F, ".proc\t_%s\n\n", S->Func->Name);
    }

    /* Output all entries */
    for (I = 0; I < Count; ++I) {

	unsigned char Use;

	OutputCodeEntry (CollConstAt (&S->Entries, I), F);
			      
#if 0
     	/* Print usage info */
     	Use = GetRegInfo ((CodeSeg*) S, I+1);
     	fprintf (F,
     		 "  Use: %c%c%c\n",
     		 (Use & REG_A)? 'A' : '_',
     		 (Use & REG_X)? 'X' : '_',
      		 (Use & REG_Y)? 'Y' : '_');
#else
	fprintf (F, "\n");
#endif
    }

    /* If this is a segment for a function, leave the function */
    if (S->Func) {
	fprintf (F, "\n.endproc\n\n");
    }
}







