/*****************************************************************************/
/*                                                                           */
/*				   codeseg.c				     */
/*                                                                           */
/*			    Code segment structure			     */
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
#include "codeent.h"
#include "codeinfo.h"
#include "error.h"
#include "codeseg.h"



/*****************************************************************************/
/*  	       	 	  	     Data				     */
/*****************************************************************************/



/* Pointer to current code segment */
CodeSeg* CS = 0;



/*****************************************************************************/
/*		      Functions for parsing instructions		     */
/*****************************************************************************/



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
    CodeEntry*		E;
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
	     	/* Assume absolute */
		AM = AM_ABS;
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
		     	AM = AM_ABSX;
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
    if ((OPC->Info & CI_MASK_BRA) == CI_BRA && Arg[0] == 'L') {

	unsigned Hash;

	/* Addressing mode must be alsobute or something is really wrong */
	CHECK (AM == AM_ABS);

	/* Addressing mode is a branch/jump */
	AM = AM_BRA;

	/* Generate the hash over the label, then search for the label */
	Hash = HashStr (Arg) % CS_LABEL_HASH_SIZE;
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
    E = NewCodeEntry (OPC, AM, Arg, Label);

    /* Return the new code entry */
    return E;
}



/*****************************************************************************/
/*     	       	      	       	     Code				     */
/*****************************************************************************/



CodeSeg* NewCodeSeg (const char* SegName, const char* FuncName)
/* Create a new code segment, initialize and return it */
{
    unsigned I;

    /* Allocate memory */
    CodeSeg* S = xmalloc (sizeof (CodeSeg));

    /* Initialize the fields */
    S->Next     = 0;
    S->SegName  = xstrdup (SegName);
    S->FuncName	= xstrdup (FuncName);
    InitCollection (&S->Entries);
    InitCollection (&S->Labels);
    for (I = 0; I < sizeof(S->LabelHash) / sizeof(S->LabelHash[0]); ++I) {
	S->LabelHash[I] = 0;
    }

    /* Return the new struct */
    return S;
}



void FreeCodeSeg (CodeSeg* S)
/* Free a code segment including all code entries */
{
    FAIL ("Not implemented");
}



void PushCodeSeg (CodeSeg* S)
/* Push the given code segment onto the stack */
{
    S->Next = CS;
    CS	    = S;
}



CodeSeg* PopCodeSeg (void)
/* Remove the current code segment from the stack and return it */
{
    /* Remember the current code segment */
    CodeSeg* S = CS;

    /* Cannot pop on empty stack */
    PRECONDITION (S != 0);

    /* Pop */
    CS = S->Next;

    /* Return the popped code segment */
    return S;
}



void AddCodeSegLine (CodeSeg* S, const char* Format, ...)
/* Add a line to the given code segment */
{
    const char* L;
    CodeEntry*  E;
    char	Token[64];

    /* Format the line */
    va_list ap;
    char Buf [256];
    va_start (ap, Format);
    xvsprintf (Buf, sizeof (Buf), Format, ap);
    va_end (ap);

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
	    /* Mark it as defined */
	    L->Flags |= LF_DEF;
	    /* Move it to the code entry */
	    CollAppend (&E->Labels, L);
	    /* Tell the label about it's owner */
	    L->Owner = E;
	}

	/* Delete the transfered labels */
	CollDeleteAll (&S->Labels);

	/* Add the entry to the list of code entries in this segment */
      	CollAppend (&S->Entries, E);

    }
}



void DelCodeSegLine (CodeSeg* S, unsigned Index)
/* Delete an entry from the code segment. This includes deleting any associated
 * labels, removing references to labels and even removing the referenced labels
 * if the reference count drops to zero.
 */
{
    /* Get the code entry for the given index */
    CodeEntry* E = CollAt (&S->Entries, Index);

    /* Remove any labels associated with this entry */
    unsigned Count;
    while ((Count = CollCount (&E->Labels)) > 0) {
	DelCodeLabel (S, CollAt (&E->Labels, Count-1));
    }

    /* If this insn references a label, remove the reference. And, if the
     * the reference count for this label drops to zero, remove this label.
     */
    if (E->JumpTo) {

	/* Remove the reference */
       	if (RemoveLabelRef (E->JumpTo, E) == 0) {
	    /* No references remaining, remove the label */
	    DelCodeLabel (S, E->JumpTo);
	}

	/* Reset the label pointer to avoid problems later */
	E->JumpTo = 0;
    }

    /* Delete the pointer to the insn */
    CollDelete (&S->Entries, Index);

    /* Delete the instruction itself */
    FreeCodeEntry (E);
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

    /* We do now have a valid label. Remember it for later */
    CollAppend (&S->Labels, L);
}



void DelCodeLabel (CodeSeg* S, CodeLabel* L)
/* Remove references from this label and delete it. */
{
    unsigned Count, I;

    /* Get the first entry in the hash chain */
    CodeLabel* List = S->LabelHash[L->Hash];

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

    /* Remove references from insns jumping to this label */
    Count = CollCount (&L->JumpFrom);
    for (I = 0; I < Count; ++I) {
       	/* Get the insn referencing this label */
       	CodeEntry* E = CollAt (&L->JumpFrom, I);
       	/* Remove the reference */
       	E->JumpTo = 0;
    }
    CollDeleteAll (&L->JumpFrom);

    /* Remove the reference to the owning instruction */
    CollDeleteItem (&L->Owner->Labels, L);

    /* All references removed, delete the label itself */
    FreeCodeLabel (L);
}



void AddCodeSegHint (CodeSeg* S, unsigned Hint)
/* Add a hint for the preceeding instruction */
{
    CodeEntry* E;

    /* Get the number of entries in this segment */
    unsigned EntryCount = CollCount (&S->Entries);

    /* Must have at least one entry */
    CHECK (EntryCount > 0);

    /* Get the last entry */
    E = CollAt (&S->Entries, EntryCount-1);

    /* Add the hint */
    E->Hints |= Hint;
}



void DelCodeSegAfter (CodeSeg* S, unsigned Last)
/* Delete all entries including the given one */
{
    /* Get the number of entries in this segment */
    unsigned Count = CollCount (&S->Entries);

    /* Remove all entries after the given one */
    while (Last < Count) {

	/* Get the next entry */
	CodeEntry* E = CollAt (&S->Entries, Count-1);

	/* We have to transfer all labels to the code segment label pool */
	unsigned LabelCount = CollCount (&E->Labels);
	while (LabelCount--) {
	    CodeLabel* L = CollAt (&E->Labels, LabelCount);
	    L->Flags &= ~LF_DEF;
	    CollAppend (&S->Labels, L);
	}
	CollDeleteAll (&E->Labels);

	/* Remove the code entry */
      	FreeCodeEntry (CollAt (&S->Entries, Count-1));
      	CollDelete (&S->Entries, Count-1);
      	--Count;
    }
}



void OutputCodeSeg (FILE* F, const CodeSeg* S)
/* Output the code segment data to a file */
{
    unsigned I;

    /* Get the number of entries in this segment */
    unsigned Count = CollCount (&S->Entries);

    fprintf (F, "; Labels: ");
    for (I = 0; I < CS_LABEL_HASH_SIZE; ++I) {
    	const CodeLabel* L = S->LabelHash[I];
    	while (L) {
    	    fprintf (F, "%s ", L->Name);
    	    L = L->Next;
    	}
    }
    fprintf (F, "\n");

    /* Output the segment directive */
    fprintf (F, ".segment\t\"%s\"\n\n", S->SegName);

    /* If this is a segment for a function, enter a function */
    if (S->FuncName[0] != '\0') {
	fprintf (F, ".proc\t_%s\n\n", S->FuncName);
    }

    /* Output all entries */
    for (I = 0; I < Count; ++I) {
	OutputCodeEntry (F, CollConstAt (&S->Entries, I));
    }

    /* If this is a segment for a function, leave the function */
    if (S->FuncName[0] != '\0') {
	fprintf (F, "\n.endproc\n\n");
    }
}



CodeLabel* FindCodeLabel (CodeSeg* S, const char* Name, unsigned Hash)
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



void MergeCodeLabels (CodeSeg* S)
/* Merge code labels. That means: For each instruction, remove all labels but
 * one and adjust the code entries accordingly.
 */
{
    unsigned I;

    /* Walk over all code entries */
    unsigned EntryCount = CollCount (&S->Entries);
    for (I = 0; I < EntryCount; ++I) {

       	CodeLabel* RefLab;
	unsigned   J;

	/* Get a pointer to the next entry */
	CodeEntry* E = CollAt (&S->Entries, I);

    	/* If this entry has zero labels, continue with the next one */
    	unsigned LabelCount = CollCount (&E->Labels);
    	if (LabelCount == 0) {
    	    continue;
    	}

    	/* We have at least one label. Use the first one as reference label. */
    	RefLab = CollAt (&E->Labels, 0);

    	/* Walk through the remaining labels and change references to these
    	 * labels to a reference to the one and only label. Delete the labels
    	 * that are no longer used. To increase performance, walk backwards
    	 * through the list.
    	 */
      	for (J = LabelCount-1; J >= 1; --J) {

    	    unsigned K;

    	    /* Get the next label */
    	    CodeLabel* L = CollAt (&E->Labels, J);

    	    /* Walk through all instructions referencing this label */
    	    unsigned RefCount = CollCount (&L->JumpFrom);
    	    for (K = 0; K < RefCount; ++K) {

    	       	/* Get the next instruction that references this label */
    	 	CodeEntry* E = CollAt (&L->JumpFrom, K);

    	 	/* Change the reference */
    	 	CHECK (E->JumpTo == L);
		AddLabelRef (RefLab, E);

    	    }

	    /* There are no more instructions jumping to this label now */
	    CollDeleteAll (&L->JumpFrom);

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



unsigned GetCodeSegEntries (const CodeSeg* S)
/* Return the number of entries for the given code segment */
{
    return CollCount (&S->Entries);
}




