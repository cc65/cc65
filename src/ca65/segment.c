/*****************************************************************************/
/*                                                                           */
/*				   segment.c				     */
/*                                                                           */
/*                   Segments for the ca65 macroassembler                    */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2003 Ullrich von Bassewitz                                       */
/*               Römerstraße 52                                              */
/*               D-70794 Filderstadt                                         */
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
#include <errno.h>

/* common */
#include "segnames.h"
#include "xmalloc.h"

/* cc65 */
#include "error.h"
#include "fragment.h"
#include "global.h"
#include "lineinfo.h"
#include "listing.h"
#include "objcode.h"
#include "objfile.h"
#include "segment.h"
#include "spool.h"
#include "symtab.h"



/*****************************************************************************/
/*     	      	    		     Data				     */
/*****************************************************************************/



/* Are we in absolute mode or in relocatable mode? */
int 		RelocMode = 1;
unsigned long	AbsPC	  = 0;		/* PC if in absolute mode */

/* Segment initializer macro */
#define SEG(segdef, num, prev)      \
    { prev, 0, 0, 0, num, 0, 0, segdef }

/* Definitions for predefined segments */
SegDef NullSegDef     = STATIC_SEGDEF_INITIALIZER (SEGNAME_NULL,     ADDR_SIZE_ABS);
SegDef ZeropageSegDef = STATIC_SEGDEF_INITIALIZER (SEGNAME_ZEROPAGE, ADDR_SIZE_ZP);
SegDef DataSegDef     = STATIC_SEGDEF_INITIALIZER (SEGNAME_DATA,     ADDR_SIZE_ABS);
SegDef BssSegDef      = STATIC_SEGDEF_INITIALIZER (SEGNAME_BSS,      ADDR_SIZE_ABS);
SegDef RODataSegDef   = STATIC_SEGDEF_INITIALIZER (SEGNAME_RODATA,   ADDR_SIZE_ABS);
SegDef CodeSegDef     = STATIC_SEGDEF_INITIALIZER (SEGNAME_CODE,     ADDR_SIZE_ABS);

/* Predefined segments */
static Segment NullSeg     = SEG (&NullSegDef,     5, NULL);
static Segment ZeropageSeg = SEG (&ZeropageSegDef, 4, &NullSeg);
static Segment DataSeg     = SEG (&DataSegDef,     3, &ZeropageSeg);
static Segment BssSeg      = SEG (&BssSegDef,      2, &DataSeg);
static Segment RODataSeg   = SEG (&RODataSegDef,   1, &BssSeg);
static Segment CodeSeg     = SEG (&CodeSegDef,     0, &RODataSeg);

/* Number of segments */
static unsigned SegmentCount = 6;

/* List of all segments */
static Segment* SegmentList = &CodeSeg;
static Segment* SegmentLast = &NullSeg;

/* Currently active segment */
Segment* ActiveSeg = &CodeSeg;



/*****************************************************************************/
/*     	      	      	   	     Code				     */
/*****************************************************************************/



static Segment* NewSegment (const char* Name, unsigned char AddrSize)
/* Create a new segment, insert it into the global list and return it */
{
    Segment* S;

    /* Check for too many segments */
    if (SegmentCount >= 256) {
     	Fatal ("Too many segments");
    }

    /* Check the segment name for invalid names */
    if (!ValidSegName (Name)) {
     	Error ("Illegal segment name: `%s'", Name);
    }

    /* Create a new segment */
    S = xmalloc (sizeof (*S));

    /* Initialize it */
    S->List      = 0;
    S->Root      = 0;
    S->Last      = 0;
    S->FragCount = 0;
    S->Num       = SegmentCount++;
    S->Align     = 0;
    S->PC        = 0;
    S->Def       = NewSegDef (Name, AddrSize);

    /* Insert it into the segment list */
    SegmentLast->List = S;
    SegmentLast = S;

    /* And return it... */
    return S;
}



Fragment* GenFragment (unsigned char Type, unsigned short Len)
/* Generate a new fragment, add it to the current segment and return it. */
{
    /* Create the new fragment */
    Fragment* F = NewFragment (Type, Len);

    /* Insert the fragment into the current segment */
    if (ActiveSeg->Root) {
    	ActiveSeg->Last->Next = F;
    	ActiveSeg->Last = F;
    } else {
    	ActiveSeg->Root = ActiveSeg->Last = F;
    }
    ++ActiveSeg->FragCount;

    /* Add this fragment to the current listing line */
    if (LineCur) {
	if (LineCur->FragList == 0) {
	    LineCur->FragList = F;
	} else {
       	    LineCur->FragLast->LineList = F;
     	}
     	LineCur->FragLast = F;
    }

    /* Increment the program counter */
    ActiveSeg->PC += F->Len;
    if (!RelocMode) {
	AbsPC += F->Len;
    }

    /* Return the fragment */
    return F;
}



void UseSeg (const SegDef* D)
/* Use the segment with the given name */
{
    Segment* Seg = SegmentList;
    while (Seg) {
       	if (strcmp (Seg->Def->Name, D->Name) == 0) {
     	    /* We found this segment. Check if the type is identical */
	    if (D->AddrSize != ADDR_SIZE_DEFAULT &&
                Seg->Def->AddrSize != D->AddrSize) {
		Error ("Segment attribute mismatch");
		/* Use the new attribute to avoid errors */
	        Seg->Def->AddrSize = D->AddrSize;
       	    }
       	    ActiveSeg = Seg;
     	    return;
     	}
     	/* Check next segment */
     	Seg = Seg->List;
    }

    /* Segment is not in list, create a new one */
    if (D->AddrSize == ADDR_SIZE_DEFAULT) {
        Seg = NewSegment (D->Name, ADDR_SIZE_ABS);
    } else {
        Seg = NewSegment (D->Name, D->AddrSize);
    }
    ActiveSeg = Seg;
}



unsigned long GetPC (void)
/* Get the program counter of the current segment */
{
    return RelocMode? ActiveSeg->PC : AbsPC;
}



void SetAbsPC (unsigned long PC)
/* Set the program counter in absolute mode */
{
    RelocMode = 0;
    AbsPC = PC;
}



void SegAlign (unsigned Power, int Val)
/* Align the PC segment to 2^Power. If Val is -1, emit fill fragments (the
 * actual fill value will be determined by the linker), otherwise use the
 * given value.
 */
{
    unsigned char Data [4];
    unsigned long Align = (1UL << Power) - 1;
    unsigned long NewPC = (ActiveSeg->PC + Align) & ~Align;
    unsigned long Count = NewPC - ActiveSeg->PC;

    if (Val != -1) {
	/* User defined fill value */
	memset (Data, Val, sizeof (Data));
	while (Count) {
	    if (Count > sizeof (Data)) {
	    	EmitData (Data, sizeof (Data));
	    	Count -= sizeof (Data);
	    } else {
	    	EmitData (Data, Count);
	    	Count = 0;
	    }
	}
    } else {
	/* Linker defined fill value */
	EmitFill (Count);
    }

    /* Remember the alignment in the header */
    if (ActiveSeg->Align < Power) {
     	ActiveSeg->Align = Power;
    }
}



unsigned char GetSegAddrSize (unsigned SegNum)
/* Return the address size of the segment with the given number */
{
    /* Search for the segment */
    Segment* S = SegmentList;
    while (S && SegNum) {
    	--SegNum;
    	S = S->List;
    }

    /* Did we find it? */
    if (S == 0) {
    	FAIL ("Invalid segment number");
    }

    /* Return the address size */
    return S->Def->AddrSize;
}



void SegCheck (void)
/* Check the segments for range and other errors */
{
    Segment* S = SegmentList;
    while (S) {
     	Fragment* F = S->Root;
     	while (F) {
       	    if (F->Type == FRAG_EXPR || F->Type == FRAG_SEXPR) {
                long Val;
       	       	if (IsConstExpr (F->V.Expr, &Val)) {
     	       	    /* We are able to evaluate the expression. Check for
     	       	     * range errors.
     	       	     */
     	       	    unsigned I;
     	       	    int Abs = (F->Type != FRAG_SEXPR);

     	       	    if (F->Len == 1) {
     	       		if (Abs) {
     	       		    /* Absolute value */
     	       		    if (Val > 255) {
     	       	       	     	PError (&F->Pos, "Range error");
     	       		    }
     	       		} else {
     	     	 	    /* PC relative value */
     	     		    if (Val < -128 || Val > 127) {
     	     	       	     	PError (&F->Pos, "Range error");
     	     		    }
     	     		}
     	       	    } else if (F->Len == 2) {
     	     	    	if (Abs) {
     	     		    /* Absolute value */
     	     		    if (Val > 65535) {
     	       	       	     	PError (&F->Pos, "Range error");
     	     		    }
     	     		} else {
     	     		    /* PC relative value */
     	     		    if (Val < -32768 || Val > 32767) {
     	     	       	     	PError (&F->Pos, "Range error");
     	       		    }
     	     		}
     	     	    }

                    /* We don't need the expression tree any longer */
                    FreeExpr (F->V.Expr);

     	     	    /* Convert the fragment into a literal fragment */
     	     	    for (I = 0; I < F->Len; ++I) {
     	     	       	F->V.Data [I] = Val & 0xFF;
     	     	       	Val >>= 8;
     	     	    }
     	     	    F->Type = FRAG_LITERAL;
     	     	} else {
     	     	    /* We cannot evaluate the expression now, leave the job for
     	     	     * the linker. However, we are able to check for explicit
     	     	     * byte expressions and we will do so.
     		     */
     	       	    if (F->Type == FRAG_EXPR && F->Len == 1 && !IsByteExpr (F->V.Expr)) {
     	       	        PError (&F->Pos, "Range error");
     	     	    }
     		}
     	    }
     	    F = F->Next;
     	}
     	S = S->List;
    }
}



void SegDump (void)
/* Dump the contents of all segments */
{
    unsigned X = 0;
    Segment* S = SegmentList;
    printf ("\n");
    while (S) {
	unsigned I;
	Fragment* F;
	int State = -1;
       	printf ("New segment: %s", S->Def->Name);
	F = S->Root;
	while (F) {
    	    if (F->Type == FRAG_LITERAL) {
	       	if (State != 0) {
	       	    printf ("\n  Literal:");
		    X = 15;
	       	    State = 0;
		}
		for (I = 0; I < F->Len; ++I) {
		    printf (" %02X", F->V.Data [I]);
		    X += 3;
		}
     	    } else if (F->Type == FRAG_EXPR || F->Type == FRAG_SEXPR) {
       	       	State = 1;
	      	printf ("\n  Expression (%u): ", F->Len);
		DumpExpr (F->V.Expr, SymResolve);
	    } else if (F->Type == FRAG_FILL) {
		State = 1;
		printf ("\n  Fill bytes (%u)", F->Len);
	    } else {
		Internal ("Unknown fragment type: %u", F->Type);
	    }
       	    if (X > 65) {
	       	State = -1;
	    }
	    F = F->Next;
	}
	printf ("\n  End PC = $%04X\n", (unsigned)(S->PC & 0xFFFF));
	S = S->List;
    }
    printf ("\n");
}



static void WriteOneSeg (Segment* Seg)
/* Write one segment to the object file */
{
    Fragment* Frag;
    unsigned LineInfoIndex;
    unsigned long DataSize;
    unsigned long EndPos;

    /* Remember the file position, then write a dummy for the size of the
     * following data
     */
    unsigned long SizePos = ObjGetFilePos ();
    ObjWrite32 (0);

    /* Write the segment data */
    ObjWriteVar (GetStringId (Seg->Def->Name)); /* Name of the segment */
    ObjWrite32 (Seg->PC);                       /* Size */
    ObjWrite8 (Seg->Align);                     /* Segment alignment */
    ObjWrite8 (Seg->Def->AddrSize);             /* Address size of the segment */
    ObjWriteVar (Seg->FragCount);               /* Number of fragments */

    /* Now walk through the fragment list for this segment and write the
     * fragments.
     */
    Frag = Seg->Root;
    while (Frag) {

    	/* Write data depending on the type */
       	switch (Frag->Type) {

    	    case FRAG_LITERAL:
    	    	ObjWrite8 (FRAG_LITERAL);
    	    	ObjWriteVar (Frag->Len);
    	        ObjWriteData (Frag->V.Data, Frag->Len);
    	    	break;

    	    case FRAG_EXPR:
    	    	switch (Frag->Len) {
    	    	    case 1:   ObjWrite8 (FRAG_EXPR8);   break;
    	    	    case 2:   ObjWrite8 (FRAG_EXPR16);	break;
    	    	    case 3:   ObjWrite8 (FRAG_EXPR24);	break;
    	    	    case 4:   ObjWrite8 (FRAG_EXPR32);	break;
    	    	    default:  Internal ("Invalid fragment size: %u", Frag->Len);
     	    	}
    	    	WriteExpr (Frag->V.Expr);
    	    	break;

    	    case FRAG_SEXPR:
    	    	switch (Frag->Len) {
    	    	    case 1:   ObjWrite8 (FRAG_SEXPR8);  break;
    	    	    case 2:   ObjWrite8 (FRAG_SEXPR16);	break;
    	    	    case 3:   ObjWrite8 (FRAG_SEXPR24);	break;
    	    	    case 4:   ObjWrite8 (FRAG_SEXPR32);	break;
    	    	    default:  Internal ("Invalid fragment size: %u", Frag->Len);
    	    	}
    	    	WriteExpr (Frag->V.Expr);
    	    	break;

	    case FRAG_FILL:
	    	ObjWrite8 (FRAG_FILL);
       	       	ObjWriteVar (Frag->Len);
	    	break;

    	    default:
    	    	Internal ("Invalid fragment type: %u", Frag->Type);

    	}

       	/* Write the file position of this fragment */
	ObjWritePos (&Frag->Pos);

	/* Write extra line info for this fragment. Zero is considered
	 * "no line info", so add one to the value.
	 */
	LineInfoIndex = Frag->LI? Frag->LI->Index + 1 : 0;
	ObjWriteVar (LineInfoIndex);

	/* Next fragment */
	Frag = Frag->Next;
    }

    /* Calculate the size of the data, seek back and write it */
    EndPos = ObjGetFilePos ();          /* Remember where we are */
    DataSize = EndPos - SizePos - 4;    /* Don't count size itself */
    ObjSetFilePos (SizePos);            /* Seek back to the size */
    ObjWrite32 (DataSize);              /* Write the size */
    ObjSetFilePos (EndPos);             /* Seek back to the end */
}



void WriteSegments (void)
/* Write the segment data to the object file */
{
    Segment* Seg;

    /* Tell the object file module that we're about to start the seg list */
    ObjStartSegments ();

    /* First thing is segment count */
    ObjWriteVar (SegmentCount);

    /* Now walk through all segments and write them to the object file */
    Seg = SegmentList;
    while (Seg) {
	/* Write one segment */
	WriteOneSeg (Seg);
	/* Next segment */
      	Seg = Seg->List;
    }

    /* Done writing segments */
    ObjEndSegments ();
}



