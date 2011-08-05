/*****************************************************************************/
/*                                                                           */
/*				  segments.c				     */
/*                                                                           */
/*		     Segment handling for the ld65 linker		     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2011, Ullrich von Bassewitz                                      */
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



#include <stdlib.h>
#include <string.h>

/* common */
#include "check.h"
#include "exprdefs.h"
#include "fragdefs.h"
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
#include "lineinfo.h"
#include "segments.h"
#include "spool.h"



/*****************************************************************************/
/*     	       	     	       	     Data				     */
/*****************************************************************************/



/* Hash table */
#define HASHTAB_MASK    0x3FU
#define HASHTAB_SIZE 	(HASHTAB_MASK + 1)
static Segment*        	HashTab [HASHTAB_SIZE];

static unsigned	       	SegCount = 0; 	/* Segment count */
static Segment*	     	SegRoot = 0;  	/* List of all segments */



/*****************************************************************************/
/*     	       	     	   	     Code  	      	  	  	     */
/*****************************************************************************/



static Segment* NewSegment (unsigned Name, unsigned char AddrSize)
/* Create a new segment and initialize it */
{
    unsigned Hash;

    /* Allocate memory */
    Segment* S = xmalloc (sizeof (Segment));

    /* Initialize the fields */
    S->Name        = Name;
    S->Next	   = 0;
    S->SecRoot	   = 0;
    S->SecLast	   = 0;
    S->PC   	   = 0;
    S->Size    	   = 0;
    S->AlignObj	   = 0;
    S->OutputName  = 0;
    S->OutputOffs  = 0;
    S->Align       = 0;
    S->FillVal	   = 0;
    S->AddrSize    = AddrSize;
    S->ReadOnly    = 0;
    S->Relocatable = 0;
    S->Placed      = 0;
    S->Dumped      = 0;

    /* Insert the segment into the segment list and assign the segment id */
    if (SegRoot == 0) {
        S->Id = 0;
    } else {
        S->Id = SegRoot->Id + 1;
    }
    S->List = SegRoot;
    SegRoot = S;
    ++SegCount;

    /* Insert the segment into the segment hash list */
    Hash = (S->Name & HASHTAB_MASK);
    S->Next = HashTab[Hash];
    HashTab[Hash] = S;

    /* Return the new entry */
    return S;
}



Segment* GetSegment (unsigned Name, unsigned char AddrSize, const char* ObjName)
/* Search for a segment and return an existing one. If the segment does not
 * exist, create a new one and return that. ObjName is only used for the error
 * message and may be NULL if the segment is linker generated.
 */
{
    /* Try to locate the segment in the table */
    Segment* S = SegFind (Name);

    /* If we don't have that segment already, allocate it using the type of
     * the first section.
     */
    if (S == 0) {
      	/* Create a new segment */
      	S = NewSegment (Name, AddrSize);
    } else {
       	/* Check if the existing segment has the requested address size */
       	if (S->AddrSize != AddrSize) {
     	    /* Allow an empty object name */
	    if (ObjName == 0) {
		ObjName = "[linker generated]";
	    }
	    Error ("Module `%s': Type mismatch for segment `%s'", ObjName,
                   GetString (Name));
     	}
    }

    /* Return the segment */
    return S;
}



Section* NewSection (Segment* Seg, unsigned char Align, unsigned char AddrSize)
/* Create a new section for the given segment */
{
    unsigned long V;


    /* Allocate memory */
    Section* S = xmalloc (sizeof (Section));

    /* Initialize the data */
    S->Next	= 0;
    S->Seg   	= Seg;
    S->Obj      = 0;
    S->FragRoot = 0;
    S->FragLast = 0;
    S->Size  	= 0;
    S->Align    = Align;
    S->AddrSize = AddrSize;

    /* Calculate the alignment bytes needed for the section */
    V = (0x01UL << S->Align) - 1;
    S->Fill = (((Seg->Size + V) & ~V) - Seg->Size);

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
    unsigned      Name;
    unsigned      Size;
    unsigned char Align;
    unsigned char Type;
    unsigned      FragCount;
    Segment*      S;
    Section*      Sec;

    /* Read the segment data */
    (void) Read32 (F);            /* File size of data */
    Name      = MakeGlobalStringId (O, ReadVar (F));    /* Segment name */
    Size      = Read32 (F);       /* Size of data */
    Align     = Read8 (F);        /* Alignment */
    Type      = Read8 (F);        /* Segment type */
    FragCount = ReadVar (F);      /* Number of fragments */


    /* Print some data */
    Print (stdout, 2, "Module `%s': Found segment `%s', size = %u, align = %u, type = %u\n",
	   GetObjFileName (O), GetString (Name), Size, Align, Type);

    /* Get the segment for this section */
    S = GetSegment (Name, Type, GetObjFileName (O));

    /* Allocate the section we will return later */
    Sec = NewSection (S, Align, Type);

    /* Remember the object file this section was from */
    Sec->Obj = O;

    /* Set up the minimum segment alignment */
    if (Sec->Align > S->Align) {
	/* Section needs larger alignment, use this one */
	S->Align    = Sec->Align;
	S->AlignObj = O;
    }

    /* Start reading fragments from the file and insert them into the section . */
    while (FragCount--) {

    	Fragment* Frag;

    	/* Read the fragment type */
    	unsigned char Type = Read8 (F);

        /* Extract the check mask from the type */
        unsigned char Bytes = Type & FRAG_BYTEMASK;
        Type &= FRAG_TYPEMASK;

    	/* Handle the different fragment types */
	switch (Type) {

	    case FRAG_LITERAL:
	       	Frag = NewFragment (Type, ReadVar (F), Sec);
		ReadData (F, Frag->LitBuf, Frag->Size);
	       	break;

	    case FRAG_EXPR:
	    case FRAG_SEXPR:
       	       	Frag = NewFragment (Type, Bytes, Sec);
		Frag->Expr = ReadExpr (F, O);
	  	break;

	    case FRAG_FILL:
	  	/* Will allocate memory, but we don't care... */
	  	Frag = NewFragment (Type, ReadVar (F), Sec);
	  	break;

	    default:
	  	Error ("Unknown fragment type in module `%s', segment `%s': %02X",
	  	       GetObjFileName (O), GetString (S->Name), Type);
	  	/* NOTREACHED */
	 	return 0;
       	}

        /* Read the line infos into the list of the fragment */
        ReadLineInfoList (F, O, &Frag->LineInfos);

        /* Resolve the back pointers */
        FragResolveLineInfos (Frag);

	/* Remember the module we had this fragment from */
	Frag->Obj = O;
    }

    /* Return the section */
    return Sec;
}



Segment* SegFind (unsigned Name)
/* Return the given segment or NULL if not found. */
{
    Segment* S = HashTab[Name & HASHTAB_MASK];
    while (S) {
       	if (Name == S->Name) {
	    /* Found */
	    break;
	}
	S = S->Next;
    }
    /* Not found */
    return S;
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
       	printf ("Segment: %s (%lu)\n", GetString (Seg->Name), Seg->Size);
	while (S) {
	    Fragment* F = S->FragRoot;
	    printf ("  Section:\n");
	    while (F) {
		switch (F->Type) {

		    case FRAG_LITERAL:
			printf ("    Literal (%u bytes):", F->Size);
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
			printf ("    Expression (%u bytes):\n", F->Size);
			printf ("    ");
			DumpExpr (F->Expr, 0);
			break;

		    case FRAG_SEXPR:
			printf ("    Signed expression (%u bytes):\n", F->Size);
			printf ("      ");
			DumpExpr (F->Expr, 0);
			break;

		    case FRAG_FILL:
			printf ("    Empty space (%u bytes)\n", F->Size);
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



void SegWrite (const char* TgtName, FILE* Tgt, Segment* S, SegWriteFunc F, void* Data)
/* Write the data from the given segment to a file. For expressions, F is
 * called (see description of SegWriteFunc above).
 */
{
    Section*      Sec;
    int           Sign;
    unsigned long Offs = 0;


    /* Remember the output file and offset for the segment */
    S->OutputName = TgtName;
    S->OutputOffs = (unsigned long) ftell (Tgt);

    /* Loop over all sections in this segment */
    Sec = S->SecRoot;
    while (Sec) {
     	Fragment* Frag;

        /* Output were this section is from */
        Print (stdout, 2, "      Section from \"%s\"\n", GetObjFileName (Sec->Obj));

     	/* If we have fill bytes, write them now */
       	Print (stdout, 2, "        Filling 0x%lx bytes with 0x%02x\n",
               Sec->Fill, S->FillVal);
     	WriteMult (Tgt, S->FillVal, Sec->Fill);
     	Offs += Sec->Fill;

     	/* Loop over all fragments in this section */
     	Frag = Sec->FragRoot;
     	while (Frag) {

            /* Do fragment alignment checks */



            /* Output fragment data */
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
		   	    	   GetFragmentSourceName (Frag),
		   	 	   GetFragmentSourceLine (Frag));
		   	    break;

		   	case SEG_EXPR_TOO_COMPLEX:
     		   	    Error ("Expression too complex in module `%s', line %lu",
		   	    	   GetFragmentSourceName (Frag),
		   	 	   GetFragmentSourceLine (Frag));
			    break;

			case SEG_EXPR_INVALID:
			    Error ("Invalid expression in module `%s', line %lu",
		   	    	   GetFragmentSourceName (Frag),
		   	 	   GetFragmentSourceLine (Frag));
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
            Print (stdout, 2, "        Fragment with 0x%x bytes\n",
                   Frag->Size);
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
     	return strcmp (GetString (S1->Name), GetString (S2->Name));
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
     	    long End = S->PC + S->Size;
     	    if (S->Size > 0) {
     		/* Point to last element addressed */
     	     	--End;
     	    }
     	    fprintf (F, "%-20s  %06lX  %06lX  %06lX\n",
       	       	     GetString (S->Name), S->PC, End, S->Size);
     	}
    }

    /* Free the segment pool */
    xfree (SegPool);
}



void PrintDbgSegments (FILE* F)
/* Output the segments to the debug file */
{
    /* Walk over all segments */
    Segment* S = SegRoot;
    while (S) {

        /* Print the segment data */
        fprintf (F,
                 "seg\tid=%u,name=\"%s\",start=0x%06lX,size=0x%04lX,addrsize=%s,type=%s",
                 S->Id, GetString (S->Name), S->PC, S->Size,
                 AddrSizeToStr (S->AddrSize),
                 S->ReadOnly? "ro" : "rw");
        if (S->OutputName) {
            fprintf (F, ",oname=\"%s\",ooffs=%lu",
                     S->OutputName, S->OutputOffs);
        }
        fputc ('\n', F);

     	/* Follow the linked list */
     	S = S->List;
    }                                   
}



void CheckSegments (void)
/* Walk through the segment list and check if there are segments that were
 * not written to the output file. Output an error if this is the case.
 */
{
    Segment* S = SegRoot;
    while (S) {
     	if (S->Size > 0 && S->Dumped == 0) {
       	    Error ("Missing memory area assignment for segment `%s'",
                   GetString (S->Name));
	}
	S = S->List;
    }
}



