/*****************************************************************************/
/*                                                                           */
/*				   objcode.c				     */
/*                                                                           */
/*	       Objectcode management for the ca65 macroassembler	     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2003 Ullrich von Bassewitz                                       */
/*               Römerstrasse 52                                             */
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
#include "chartype.h"
#include "check.h"
#include "segdefs.h"
#include "segnames.h"
#include "xmalloc.h"

/* cc65 */
#include "error.h"
#include "fragment.h"
#include "global.h"
#include "lineinfo.h"
#include "listing.h"
#include "objfile.h"
#include "scanner.h"
#include "symtab.h"
#include "objcode.h"



/*****************************************************************************/
/*     	      	    		     Data				     */
/*****************************************************************************/



/* Are we in absolute mode or in relocatable mode? */
int 		RelocMode = 1;
unsigned long	AbsPC	  = 0;		/* PC if in absolute mode */



typedef struct Segment Segment;
struct Segment {
    Segment*   	    List;      	       	/* List of all segments */
    Fragment*  	    Root;  	  	/* Root of fragment list */
    Fragment*  	    Last;  	  	/* Pointer to last fragment */
    unsigned long   FragCount;          /* Number of fragments */
    unsigned        Num;   		/* Segment number */
    unsigned        Align; 		/* Segment alignment */
    unsigned long   PC;
    SegDef*         Def;                /* Segment definition (name and type) */
};


#define SEG(segdef, num, prev)      \
    { prev, 0, 0, 0, num, 0, 0, segdef }

/* Definitions for predefined segments */
SegDef NullSegDef     = STATIC_SEGDEF_INITIALIZER (SEGNAME_NULL,     SEGTYPE_ABS);
SegDef ZeropageSegDef = STATIC_SEGDEF_INITIALIZER (SEGNAME_ZEROPAGE, SEGTYPE_ZP);
SegDef DataSegDef     = STATIC_SEGDEF_INITIALIZER (SEGNAME_DATA,     SEGTYPE_ABS);
SegDef BssSegDef      = STATIC_SEGDEF_INITIALIZER (SEGNAME_BSS,      SEGTYPE_ABS);
SegDef RODataSegDef   = STATIC_SEGDEF_INITIALIZER (SEGNAME_RODATA,   SEGTYPE_ABS);
SegDef CodeSegDef     = STATIC_SEGDEF_INITIALIZER (SEGNAME_CODE,     SEGTYPE_ABS);

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
static Segment*	ActiveSeg = &CodeSeg;



/*****************************************************************************/
/*     	       	   	      Segment management			     */
/*****************************************************************************/



static Segment* NewSegment (const char* Name, unsigned SegType)
/* Create a new segment, insert it into the global list and return it */
{
    Segment* S;

    /* Check for too many segments */
    if (SegmentCount >= 256) {
     	Fatal (FAT_TOO_MANY_SEGMENTS);
    }

    /* Check the segment name for invalid names */
    if (!ValidSegName (Name)) {
     	Error (ERR_ILLEGAL_SEGMENT, Name);
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
    S->Def       = NewSegDef (Name, SegType);

    /* Insert it into the segment list */
    SegmentLast->List = S;
    SegmentLast = S;

    /* And return it... */
    return S;
}



void UseSeg (const SegDef* D)
/* Use the segment with the given name */
{
    Segment* Seg = SegmentList;
    while (Seg) {
       	if (strcmp (Seg->Def->Name, D->Name) == 0) {
     	    /* We found this segment. Check if the type is identical */
	    if (D->Type != SEGTYPE_DEFAULT && Seg->Def->Type != D->Type) {
		Error (ERR_SEG_ATTR_MISMATCH);
		/* Use the new attribute to avoid errors */
	        Seg->Def->Type = D->Type;
       	    }
       	    ActiveSeg = Seg;
     	    return;
     	}
     	/* Check next segment */
     	Seg = Seg->List;
    }

    /* Segment is not in list, create a new one */
    if (D->Type == SEGTYPE_DEFAULT) {
        Seg = NewSegment (D->Name, SEGTYPE_ABS);
    } else {
        Seg = NewSegment (D->Name, D->Type);
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



const SegDef* GetCurrentSeg (void)
/* Get a pointer to the segment defininition of the current segment */
{
    return ActiveSeg->Def;
}



unsigned GetSegNum (void)
/* Get the number of the current segment */
{
    return ActiveSeg->Num;
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



int IsZPSeg (void)
/* Return true if the current segment is a zeropage segment */
{
    return (ActiveSeg->Def->Type == SEGTYPE_ZP);
}



int IsFarSeg (void)
/* Return true if the current segment is a far segment */
{
    return (ActiveSeg->Def->Type == SEGTYPE_FAR);
}



unsigned GetSegType (unsigned SegNum)
/* Return the type of the segment with the given number */
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

    /* Return the segment type */
    return S->Def->Type;
}



void SegCheck (void)
/* Check the segments for range and other errors */
{
    Segment* S = SegmentList;
    while (S) {
     	Fragment* F = S->Root;
     	while (F) {
       	    if (F->Type == FRAG_EXPR || F->Type == FRAG_SEXPR) {
       	       	F->V.Expr = FinalizeExpr (F->V.Expr);
       	       	if (IsConstExpr (F->V.Expr)) {
     	       	    /* We are able to evaluate the expression. Get the value
     	       	     * and check for range errors.
     	       	     */
     	       	    unsigned I;
     	       	    long Val = GetExprVal (F->V.Expr);
     	       	    int Abs = (F->Type != FRAG_SEXPR);

     	       	    if (F->Len == 1) {
     	       		if (Abs) {
     	       		    /* Absolute value */
     	       		    if (Val > 255) {
     	       	       	     	PError (&F->Pos, ERR_RANGE);
     	       		    }
     	       		} else {
     	     	 	    /* PC relative value */
     	     		    if (Val < -128 || Val > 127) {
     	     	       	     	PError (&F->Pos, ERR_RANGE);
     	     		    }
     	     		}
     	       	    } else if (F->Len == 2) {
     	     	    	if (Abs) {
     	     		    /* Absolute value */
     	     		    if (Val > 65535) {
     	       	       	     	PError (&F->Pos, ERR_RANGE);
     	     		    }
    	     		} else {
	     		    /* PC relative value */
	     		    if (Val < -32768 || Val > 32767) {
	     	       	     	PError (&F->Pos, ERR_RANGE);
	       		    }
	     		}
	     	    }

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
	       	        PError (&F->Pos, ERR_RANGE);
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
		DumpExpr (F->V.Expr);
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
    ObjWriteStr (Seg->Def->Name);       /* Name of the segment */
    ObjWrite32 (Seg->PC);               /* Size */
    ObjWrite8 (Seg->Align);             /* Segment alignment */
    ObjWrite8 (Seg->Def->Type);         /* Type of the segment */
    ObjWriteVar (Seg->FragCount);       /* Number of fragments that follow */

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



/*****************************************************************************/
/*     	      	      	   	     Code				     */
/*****************************************************************************/



static void IncPC (unsigned Value)
/* Increment the PC counter */
{
    ActiveSeg->PC += Value;
    if (!RelocMode) {
	AbsPC += Value;
    }
}



static Fragment* NewFragment (unsigned char Type, unsigned short Len)
/* Create, initialize and return a new fragment. The fragment will be inserted
 * into the current segment.
 */
{
    Fragment* F;

    /* Create a new fragment */
    F = xmalloc (sizeof (*F));

    /* Initialize it */
    F->Next 	= 0;
    F->LineList = 0;
    F->Pos  	= CurPos;
    F->LI       = UseLineInfo (CurLineInfo);
    F->Len  	= Len;
    F->Type 	= Type;

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
    IncPC (Len);

    /* And return it */
    return F;
}



void Emit0 (unsigned char OPC)
/* Emit an instruction with a zero sized operand */
{
    Fragment* F = NewFragment (FRAG_LITERAL, 1);
    F->V.Data [0] = OPC;
}



void Emit1 (unsigned char OPC, ExprNode* Value)
/* Emit an instruction with an one byte argument */
{
    Emit0 (OPC);
    EmitByte (Value);
}



void Emit2 (unsigned char OPC, ExprNode* Value)
/* Emit an instruction with a two byte argument */
{
    Emit0 (OPC);
    EmitWord (Value);
}



void Emit3 (unsigned char OPC, ExprNode* Expr)
/* Emit an instruction with a three byte argument */
{
    Emit0 (OPC);
    EmitFarAddr (Expr);
}



void Emit3b (unsigned char OPC, ExprNode* Expr, ExprNode* Bank)
/* Emit an instruction with a three byte argument and separate bank */
{
    Emit0 (OPC);
    EmitWord (Expr);
    EmitByte (Bank);
}



void EmitPCRel (unsigned char OPC, ExprNode* Expr, unsigned Size)
/* Emit an opcode with a PC relative argument of one or two bytes */
{
    Fragment* F;
    Emit0 (OPC);
    F = NewFragment (FRAG_SEXPR, Size);
    F->V.Expr = Expr;
}



void EmitData (const unsigned char* Data, unsigned Size)
/* Emit data into the current segment */
{
    /* Create lots of fragments for the data */
    while (Size) {
	Fragment* F;

	/* Determine the length of the next fragment */
	unsigned Len = Size;
       	if (Len > sizeof (F->V.Data)) {
     	    Len = sizeof (F->V.Data);
       	}

     	/* Create a new fragment */
     	F = NewFragment (FRAG_LITERAL, Len);

     	/* Copy the data */
     	memcpy (F->V.Data, Data, Len);

     	/* Next chunk */
     	Data += Len;
     	Size -= Len;

    }
}



void EmitByte (ExprNode* Expr)
/* Emit one byte */
{
    if (IsConstExpr (Expr)) {
     	/* Constant expression, emit literal byte */
     	long Val = GetExprVal (Expr);
	FreeExpr (Expr);
     	if ((Val & ~0xFF) != 0) {
     	    Error (ERR_RANGE);
     	}
     	Emit0 (Val & 0xFF);
    } else {
     	/* Create a new fragment */
     	Fragment* F = NewFragment (FRAG_EXPR, 1);

     	/* Set the data */
     	F->V.Expr = Expr;
    }
}



void EmitWord (ExprNode* Expr)
/* Emit one word */
{
    if (IsConstExpr (Expr)) {
     	/* Constant expression, emit literal byte */
     	long Val = GetExprVal (Expr);
	FreeExpr (Expr);
       	if ((Val & ~0xFFFF) != 0) {
     	    Error (ERR_RANGE);
     	}
     	Emit0 (Val & 0xFF);
	Emit0 ((Val >> 8) & 0xFF);
    } else {
     	/* Create a new fragment */
     	Fragment* F = NewFragment (FRAG_EXPR, 2);

     	/* Set the data */
     	F->V.Expr = Expr;
    }
}



void EmitFarAddr (ExprNode* Expr)
/* Emit a 24 bit expression */
{
    if (IsConstExpr (Expr)) {
     	/* Constant expression, emit literal byte */
     	long Val = GetExprVal (Expr);
	FreeExpr (Expr);
       	if ((Val & ~0xFFFFFF) != 0) {
     	    Error (ERR_RANGE);
     	}
     	Emit0 (Val & 0xFF);
	Emit0 ((Val >> 8) & 0xFF);
	Emit0 ((Val >> 16) & 0xFF);
    } else {
     	/* Create a new fragment */
     	Fragment* F = NewFragment (FRAG_EXPR, 3);

     	/* Set the data */
     	F->V.Expr = Expr;
    }
}



void EmitDWord (ExprNode* Expr)
/* Emit one dword */
{
    if (IsConstExpr (Expr)) {
     	/* Constant expression, emit literal byte */
     	long Val = GetExprVal (Expr);
	FreeExpr (Expr);
     	Emit0 (Val & 0xFF);
	Emit0 ((Val >> 8) & 0xFF);
       	Emit0 ((Val >> 16) & 0xFF);
	Emit0 ((Val >> 24) & 0xFF);
    } else {
     	/* Create a new fragment */
     	Fragment* F = NewFragment (FRAG_EXPR, 4);

     	/* Set the data */
     	F->V.Expr = Expr;
    }
}



void EmitFill (unsigned long Count)
/* Emit Count fill bytes */
{
    while (Count) {
	/* Calculate the size of the next chunk */
	unsigned Chunk = (Count > 0xFFFF)? 0xFFFF : (unsigned) Count;
	Count -= Chunk;

	/* Emit one chunk */
	NewFragment (FRAG_FILL, Chunk);
    }
}



