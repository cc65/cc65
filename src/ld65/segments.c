/*****************************************************************************/
/*                                                                           */
/*				  segments.c				     */
/*                                                                           */
/*		     Segment handling for the ld65 linker		     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2000 Ullrich von Bassewitz                                       */
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



#include <stdlib.h>
#include <string.h>

/* common */
#include "check.h"
#include "exprdefs.h"
#include "hashstr.h"
#include "print.h"
#include "segdefs.h"
#include "symdefs.h"
#include "xmalloc.h"

/* ld65 */
#include "error.h"
#include "expr.h"
#include "fileio.h"
#include "fragment.h"
#include "global.h"
#include "segments.h"



/*****************************************************************************/
/*     	       	     	       	     Data				     */
/*****************************************************************************/



/* Hash table */
#define HASHTAB_SIZE 	253
static Segment*        	HashTab [HASHTAB_SIZE];

static unsigned	       	SegCount = 0; 	/* Segment count */
static Segment*	     	SegRoot = 0;	/* List of all segments */



/*****************************************************************************/
/*     	       	     	   	     Code  	      	  	  	     */
/*****************************************************************************/



static Segment* SegFindInternal (const char* Name, unsigned HashVal)
/* Try to find the segment with the given name, return a pointer to the
 * segment structure, or 0 if not found.
 */
{
    Segment* S = HashTab [HashVal];
    while (S) {
	if (strcmp (Name, S->Name) == 0) {
	    /* Found */
	    break;
	}
	S = S->Next;
    }
    /* Not found */
    return S;
}



static Segment* NewSegment (const char* Name, unsigned HashVal, unsigned char Type)
/* Create a new segment and initialize it */
{
    /* Get the length of the symbol name */
    unsigned Len = strlen (Name);

    /* Allocate memory */
    Segment* S = xmalloc (sizeof (Segment) + Len);

    /* Initialize the fields */
    S->Next	= 0;
    S->SecRoot	= 0;
    S->SecLast	= 0;
    S->PC   	= 0;
    S->Size    	= 0;
    S->AlignObj	= 0;
    S->Align    = 0;
    S->FillVal	= 0;
    S->Type     = Type;
    S->Dumped   = 0;
    memcpy (S->Name, Name, Len);
    S->Name [Len] = '\0';

    /* Insert the segment into the segment list */
    S->List = SegRoot;
    SegRoot = S;
    ++SegCount;

    /* Insert the segment into the segment hash list */
    S->Next = HashTab [HashVal];
    HashTab [HashVal] = S;

    /* Return the new entry */
    return S;
}



Segment* GetSegment (const char* Name, unsigned char Type, const char* ObjName)
/* Search for a segment and return an existing one. If the segment does not
 * exist, create a new one and return that. ObjName is only used for the error
 * message and may be NULL if the segment is linker generated.
 */
{
    /* Create a hash over the name and try to locate the segment in the table */
    unsigned HashVal = HashStr (Name) % HASHTAB_SIZE;
    Segment* S = SegFindInternal (Name, HashVal);

    /* If we don't have that segment already, allocate it using the type of
     * the first section.
     */
    if (S == 0) {
      	/* Create a new segment */
      	S = NewSegment (Name, HashVal, Type);
    } else {
       	/* Check if the existing segment has the requested type */
       	if (S->Type != Type) {
     	    /* Allow an empty object name */
	    if (ObjName == 0) {
		ObjName = "[linker generated]";
	    }
	    Error ("Module `%s': Type mismatch for segment `%s'", ObjName, Name);
     	}
    }

    /* Return the segment */
    return S;
}



Section* NewSection (Segment* Seg, unsigned char Align, unsigned char Type)
/* Create a new section for the given segment */
{
    unsigned long V;


    /* Allocate memory */
    Section* S = xmalloc (sizeof (Segment));

    /* Initialize the data */
    S->Next	= 0;
    S->Seg   	= Seg;
    S->FragRoot = 0;
    S->FragLast = 0;
    S->Size  	= 0;
    S->Align    = Align;
    S->Type     = Type;

    /* Calculate the alignment bytes needed for the section */
    V = (0x01UL << S->Align) - 1;
    S->Fill = (unsigned char) (((Seg->Size + V) & ~V) - Seg->Size);

    /* Adjust the segment size and set the section offset */
    Seg->Size  += S->Fill;
    S->Offs  	= Seg->Size;  	/* Current size is offset */

    /* Insert the section into the segment */
    if (Seg->SecRoot == 0) {
	/* First section in this segment */
       	Seg->SecRoot = S;
    } else {
	Seg->SecLast->Next = S;
    }
    Seg->SecLast = S;

    /* Return the struct */
    return S;
}



Section* ReadSection (FILE* F, ObjData* O)
/* Read a section from a file */
{
    char* Name;
    unsigned long Size;
    unsigned char Align;
    unsigned char Type;
    Segment* S;
    Section* Sec;

    /* Read the name */
    Name = ReadStr (F);

    /* Read the size */
    Size = Read32 (F);

    /* Read the alignment */
    Align = Read8 (F);

    /* Read the segment type */
    Type = Read8 (F);

    /* Print some data */
    Print (stdout, 2, "Module `%s': Found segment `%s', size = %lu, align = %u, type = %u\n",
	   GetObjFileName (O), Name, Size, Align, Type);

    /* Get the segment for this section */
    S = GetSegment (Name, Type, GetObjFileName (O));

    /* We have the segment and don't need the name any longer */
    xfree (Name);

    /* Allocate the section we will return later */
    Sec = NewSection (S, Align, Type);

    /* Set up the minimum segment alignment */
    if (Sec->Align > S->Align) {
	/* Section needs larger alignment, use this one */
	S->Align    = Sec->Align;
	S->AlignObj = O;
    }

    /* Start reading fragments from the file and insert them into the section . */
    while (Size) {

    	Fragment* Frag;

    	/* Read the fragment type */
    	unsigned char Type = Read8 (F);

    	/* Handle the different fragment types */
	switch (Type) {

	    case FRAG_LITERAL:
	       	Frag = NewFragment (Type, ReadVar (F), Sec);
	       	break;

	    case FRAG_EXPR8:
    	    case FRAG_EXPR16:
       	    case FRAG_EXPR24:
	    case FRAG_EXPR32:
	    case FRAG_SEXPR8:
	    case FRAG_SEXPR16:
	    case FRAG_SEXPR24:
	    case FRAG_SEXPR32:
       	       	Frag = NewFragment (Type & FRAG_TYPEMASK, Type & FRAG_BYTEMASK, Sec);
	  	break;

	    case FRAG_FILL:
	  	/* Will allocate memory, but we don't care... */
	  	Frag = NewFragment (Type, ReadVar (F), Sec);
	  	break;

	    default:
	  	Error ("Unknown fragment type in module `%s', segment `%s': %02X",
	  	       GetObjFileName (O), S->Name, Type);
	  	/* NOTREACHED */
	 	return 0;
       	}

	/* Now read the fragment data */
	switch (Frag->Type) {

	    case FRAG_LITERAL:
		/* Literal data */
		ReadData (F, Frag->LitBuf, Frag->Size);
		break;

	    case FRAG_EXPR:
	    case FRAG_SEXPR:
		/* An expression */
		Frag->Expr = ReadExpr (F, O);
		break;

	}

	/* Read the file position of the fragment */
	ReadFilePos (F, &Frag->Pos);

	/* Remember the module we had this fragment from */
	Frag->Obj = O;

     	/* Next one */
	CHECK (Size >= Frag->Size);
     	Size -= Frag->Size;
    }

    /* Return the section */
    return Sec;
}



Segment* SegFind (const char* Name)
/* Return the given segment or NULL if not found. */
{
    return SegFindInternal (Name, HashStr (Name) % HASHTAB_SIZE);
}



int IsBSSType (Segment* S)
/* Check if the given segment is a BSS style segment, that is, it does not
 * contain non-zero data.
 */
{
    /* Loop over all sections */
    Section* Sec = S->SecRoot;
    while (Sec) {
	/* Loop over all fragments */
	Fragment* F = Sec->FragRoot;
	while (F) {
	    if (F->Type == FRAG_LITERAL) {
		unsigned char* Data = F->LitBuf;
		unsigned long Count = F->Size;
		while (Count--) {
		    if (*Data++ != 0) {
			return 0;
		    }
		}
	    } else if (F->Type == FRAG_EXPR || F->Type == FRAG_SEXPR) {
		if (GetExprVal (F->Expr) != 0) {
		    return 0;
		}
	    }
	    F = F->Next;
	}
	Sec = Sec->Next;
    }
    return 1;
}



void SegDump (void)
/* Dump the segments and it's contents */
{
    unsigned I;
    unsigned long Count;
    unsigned char* Data;

    Segment* Seg = SegRoot;
    while (Seg) {
	Section* S = Seg->SecRoot;
       	printf ("Segment: %s (%lu)\n", Seg->Name, Seg->Size);
	while (S) {
	    Fragment* F = S->FragRoot;
	    printf ("  Section:\n");
	    while (F) {
		switch (F->Type) {

		    case FRAG_LITERAL:
			printf ("    Literal (%lu bytes):", F->Size);
			Count = F->Size;
			Data  = F->LitBuf;
			I = 100;
			while (Count--) {
			    if (I > 75) {
				printf ("\n   ");
		     		I = 3;
			    }
			    printf (" %02X", *Data++);
			    I += 3;
			}
			printf ("\n");
			break;

		    case FRAG_EXPR:
			printf ("    Expression (%lu bytes):\n", F->Size);
			printf ("    ");
			DumpExpr (F->Expr);
			break;

		    case FRAG_SEXPR:
			printf ("    Signed expression (%lu bytes):\n", F->Size);
			printf ("      ");
			DumpExpr (F->Expr);
			break;

		    case FRAG_FILL:
			printf ("    Empty space (%lu bytes)\n", F->Size);
		     	break;

		    default:
			Internal ("Invalid fragment type: %02X", F->Type);
		}
		F = F->Next;
	    }
	    S = S->Next;
    	}
	Seg = Seg->List;
    }
}



unsigned SegWriteConstExpr (FILE* F, ExprNode* E, int Signed, unsigned Size)
/* Write a supposedly constant expression to the target file. Do a range
 * check and return one of the SEG_EXPR_xxx codes.
 */
{
    static const unsigned long U_HighRange [4] = {
       	0x000000FF, 0x0000FFFF, 0x00FFFFFF, 0xFFFFFFFF
    };
    static const long S_HighRange [4] = {
       	0x0000007F, 0x00007FFF, 0x007FFFFF, 0x7FFFFFFF
    };
    static const long S_LowRange [4] = {
       	0xFFFFFF80, 0xFFFF8000, 0xFF800000, 0x80000000
    };


    /* Get the expression value */
    long Val = GetExprVal (E);

    /* Check the size */
    CHECK (Size >= 1 && Size <= 4);

    /* Check for a range error */
    if (Signed) {
	if (Val > S_HighRange [Size-1] || Val < S_LowRange [Size-1]) {
	    /* Range error */
	    return SEG_EXPR_RANGE_ERROR;
	}
    } else {
	if (((unsigned long)Val) > U_HighRange [Size-1]) {
	    /* Range error */
	    return SEG_EXPR_RANGE_ERROR;
	}
    }

    /* Write the value to the file */
    WriteVal (F, Val, Size);

    /* Success */
    return SEG_EXPR_OK;
}



void SegWrite (FILE* Tgt, Segment* S, SegWriteFunc F, void* Data)
/* Write the data from the given segment to a file. For expressions, F is
 * called (see description of SegWriteFunc above).
 */
{
    int Sign;
    unsigned long Offs = 0;

    /* Loop over all sections in this segment */
    Section* Sec = S->SecRoot;
    while (Sec) {
	Fragment* Frag;

	/* If we have fill bytes, write them now */
	WriteMult (Tgt, S->FillVal, Sec->Fill);

	/* Loop over all fragments in this section */
	Frag = Sec->FragRoot;
	while (Frag) {

	    switch (Frag->Type) {

		case FRAG_LITERAL:
		    WriteData (Tgt, Frag->LitBuf, Frag->Size);
		    break;

		case FRAG_EXPR:
		case FRAG_SEXPR:
		    Sign = (Frag->Type == FRAG_SEXPR);
		    /* Call the users function and evaluate the result */
		    switch (F (Frag->Expr, Sign, Frag->Size, Offs, Data)) {

			case SEG_EXPR_OK:
			    break;

			case SEG_EXPR_RANGE_ERROR:
			    Error ("Range error in module `%s', line %lu",
			    	   GetSourceFileName (Frag->Obj, Frag->Pos.Name),
				   Frag->Pos.Line);
			    break;

			case SEG_EXPR_TOO_COMPLEX:
			    Error ("Expression too complex in module `%s', line %lu",
			    	   GetSourceFileName (Frag->Obj, Frag->Pos.Name),
				   Frag->Pos.Line);
			    break;

			default:
			    Internal ("Invalid return code from SegWriteFunc");
		    }
		    break;

		case FRAG_FILL:
		    WriteMult (Tgt, S->FillVal,	Frag->Size);
		    break;

		default:
		    Internal ("Invalid fragment type: %02X", Frag->Type);
	    }

	    /* Update the offset */
	    Offs += Frag->Size;

	    /* Next fragment */
	    Frag = Frag->Next;
     	}

	/* Next section */
	Sec = Sec->Next;
    }
}



static int CmpSegStart (const void* K1, const void* K2)
/* Compare function for qsort */
{
    /* Get the real segment pointers */
    const Segment* S1 = *(const Segment**)K1;
    const Segment* S2 = *(const Segment**)K2;

    /* Compare the start addresses */
    if (S1->PC > S2->PC) {
	return 1;
    } else if (S1->PC < S2->PC) {
	return -1;
    } else {
	/* Sort segments with equal starts by name */
     	return strcmp (S1->Name, S2->Name);
    }
}



void PrintSegmentMap (FILE* F)
/* Print a segment map to the given file */
{
    unsigned I;
    Segment* S;
    Segment** SegPool;

    /* Allocate memory for the segment pool */
    SegPool = xmalloc (SegCount * sizeof (Segment*));

    /* Collect pointers to the segments */
    I = 0;
    S = SegRoot;
    while (S) {

     	/* Check the count for safety */
     	CHECK (I < SegCount);

     	/* Remember the pointer */
     	SegPool [I] = S;

     	/* Follow the linked list */
     	S = S->List;

     	/* Next array index */
     	++I;
    }
    CHECK (I == SegCount);

    /* Sort the array by increasing start addresses */
    qsort (SegPool, SegCount, sizeof (Segment*), CmpSegStart);

    /* Print a header */
    fprintf (F, "Name                  Start   End     Size\n"
     	       	"--------------------------------------------\n");

    /* Print the segments */
    for (I = 0; I < SegCount; ++I) {

     	/* Get a pointer to the segment */
     	S = SegPool [I];

	/* Print empty segments only if explicitly requested */
	if (VerboseMap || S->Size > 0) {
	    /* Print the segment data */
	    fprintf (F, "%-20s  %06lX  %06lX  %06lX\n",
		     S->Name, S->PC, S->PC + S->Size, S->Size);
     	}
    }

    /* Free the segment pool */
    xfree (SegPool);
}



void CheckSegments (void)
/* Walk through the segment list and check if there are segments that were
 * not written to the output file. Output an error if this is the case.
 */
{
    Segment* S = SegRoot;
    while (S) {
	if (S->Size > 0 && S->Dumped == 0) {
       	    Error ("Missing memory area assignment for segment `%s'", S->Name);
	}
	S = S->List;
    }
}



