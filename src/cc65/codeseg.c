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

/* b6502 */
#include "codeent.h"
#include "error.h"
#include "codeseg.h"



/*****************************************************************************/
/*     	       	      	       	     Code				     */
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



static CodeEntry* ParseInsn (const char* L)
/* Parse an instruction nnd generate a code entry from it. If the line contains
 * errors, output an error message and return NULL.
 * For simplicity, we don't accept the broad range of input a "real" assembler
 * does. The instruction and the argument are expected to be separated by
 * white space, for example.
 */
{
    char   		Mnemo[16];
    const OPCDesc*	OPC;
    am_t     		AM = 0;		/* Initialize to keep gcc silent */
    char     		Expr[64];
    char     	   	Reg;
    CodeEntry*		E;

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
    Expr[0] = '\0';
    switch (*L) {

	case '#':
	    /* Immidiate */
	    StrCopy (Expr, sizeof (Expr), L+1);
	    AM = AM_IMM;
	    break;

	case '(':
	    /* Indirect */
	    L = ReadToken (L+1, ",)", Expr, sizeof (Expr));

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
	     	AM = AM_IMP;
	     	break;
	    }
	    /* FALLTHROUGH */

	default:
	    /* Absolute, maybe indexed */
	    L = ReadToken (L, ",", Expr, sizeof (Expr));
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

    /* We do now have the addressing mode in AM. Allocate a new CodeEntry
     * structure and initialize it.
     */
    E = NewCodeEntry (OPC, AM);
    if (Expr[0] != '\0') {
	/* We have an additional expression */
	E->Arg.Expr = xstrdup (Expr);
    }

    /* Return the new code entry */
    return E;
}



CodeSeg* NewCodeSeg (const char* Name)
/* Create a new code segment, initialize and return it */
{
    unsigned I;

    /* Allocate memory */
    CodeSeg* S = xmalloc (sizeof (CodeSeg));

    /* Initialize the fields */
    S->Name = xstrdup (Name);
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
    unsigned I, Count;

    /* Free the name */
    xfree (S->Name);

    /* Free the entries */
    Count = CollCount (&S->Entries);
    for (I = 0; I < Count; ++I) {
	FreeCodeEntry (CollAt (&S->Entries, I));
    }

    /* Free the collections */
    DoneCollection (&S->Entries);
    DoneCollection (&S->Labels);

    /* Free all labels */
    for (I = 0; I < sizeof(S->LabelHash) / sizeof(S->LabelHash[0]); ++I) {
	CodeLabel* L = S->LabelHash[I];
	while (L) {
	    CodeLabel* Tmp = L;
	    L = L->Next;
	    FreeCodeLabel (Tmp);
	}
    }

    /* Free the struct */
    xfree (S);
}



void AddCodeSegLine (CodeSeg* S, const char* Format, ...)
/* Add a line to the given code segment */
{
    const char* L;
    CodeEntry*  E;

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
     	    Error ("ASM code error: Pseudo instructions not supported");
	    break;

	default:
	    E = ParseInsn (L);
	    break;
    }

    /* If we have a code entry, transfer the labels and insert it */
    if (E) {

	/* Transfer the labels if we have any */
	unsigned LabelCount = CollCount (&S->Labels);
	unsigned I;
	for (I = 0; I < LabelCount; ++I) {
	    CollAppend (&E->Labels, CollAt (&S->Labels, I));
	}
	CollDeleteAll (&S->Labels);

	/* Add the entry to the list of code entries in this segment */
	CollAppend (&S->Entries, E);

    }
}



void AddCodeSegLabel (CodeSeg* S, const char* Name)
/* Add a label for the next instruction to follow */
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
    	L = NewCodeLabel (Name, Hash);

    	/* Enter the label into the hash table */
    	L->Next = S->LabelHash[L->Hash];
    	S->LabelHash[L->Hash] = L;
    }

    /* We do now have a valid label. Remember it for later */
    CollAppend (&S->Labels, L);
}



void OutputCodeSeg (FILE* F, const CodeSeg* S)
/* Output the code segment data to a file */
{
    unsigned I;

    /* Get the number of entries in this segment */
    unsigned Count = CollCount (&S->Entries);

    /* Output all entries */
    for (I = 0; I < Count; ++I) {
	OutputCodeEntry (F, CollConstAt (&S->Entries, I));
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

	/* If this entry has one or zero labels, continue with the next one */
	unsigned LabelCount = CollCount (&E->Labels);
	if (LabelCount <= 1) {
	    continue;
	}

	/* We have more than one label. Use the first one as reference label */
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

		/* Get the next instrcuction that references this label */
	 	CodeEntry* E = CollAt (&L->JumpFrom, K);

		/* Change the reference */
		CHECK (E->JumpTo == L);
		E->JumpTo = RefLab;
		CollAppend (&RefLab->JumpFrom, E);

	    }

	    /* Delete the label */
	    FreeCodeLabel (L);

	    /* Remove it from the list */
	    CollDelete (&E->Labels, J);

     	}
    }
}



