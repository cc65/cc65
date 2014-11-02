/*****************************************************************************/
/*                                                                           */
/*                                 segment.c                                 */
/*                                                                           */
/*                   Segments for the ca65 macroassembler                    */
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



#include <string.h>
#include <errno.h>

/* common */
#include "addrsize.h"
#include "alignment.h"
#include "coll.h"
#include "mmodel.h"
#include "segdefs.h"
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
#include "span.h"
#include "spool.h"
#include "studyexpr.h"
#include "symtab.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* If OrgPerSeg is false, all segments share the RelocMode flag and a PC
** used when in absolute mode. OrgPerSeg may be set by .feature org_per_seg
*/
static int              RelocMode = 1;
static unsigned long    AbsPC     = 0;          /* PC if in absolute mode */

/* Definitions for predefined segments */
SegDef NullSegDef     = STATIC_SEGDEF_INITIALIZER (SEGNAME_NULL,     ADDR_SIZE_ABS);
SegDef ZeropageSegDef = STATIC_SEGDEF_INITIALIZER (SEGNAME_ZEROPAGE, ADDR_SIZE_ZP);
SegDef DataSegDef     = STATIC_SEGDEF_INITIALIZER (SEGNAME_DATA,     ADDR_SIZE_ABS);
SegDef BssSegDef      = STATIC_SEGDEF_INITIALIZER (SEGNAME_BSS,      ADDR_SIZE_ABS);
SegDef RODataSegDef   = STATIC_SEGDEF_INITIALIZER (SEGNAME_RODATA,   ADDR_SIZE_ABS);
SegDef CodeSegDef     = STATIC_SEGDEF_INITIALIZER (SEGNAME_CODE,     ADDR_SIZE_ABS);

/* Collection containing all segments */
Collection SegmentList = STATIC_COLLECTION_INITIALIZER;

/* Currently active segment */
Segment* ActiveSeg;



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



static Segment* NewSegFromDef (SegDef* Def)
/* Create a new segment from a segment definition. Used only internally, no
** checks.
*/
{
    /* Create a new segment */
    Segment* S = xmalloc (sizeof (*S));

    /* Initialize it */
    S->Root      = 0;
    S->Last      = 0;
    S->FragCount = 0;
    S->Num       = CollCount (&SegmentList);
    S->Flags     = SEG_FLAG_NONE;
    S->Align     = 1;
    S->RelocMode = 1;
    S->PC        = 0;
    S->AbsPC     = 0;
    S->Def       = Def;

    /* Insert it into the segment list */
    CollAppend (&SegmentList, S);

    /* And return it... */
    return S;
}



static Segment* NewSegment (const char* Name, unsigned char AddrSize)
/* Create a new segment, insert it into the global list and return it */
{
    /* Check for too many segments */
    if (CollCount (&SegmentList) >= 256) {
        Fatal ("Too many segments");
    }

    /* Check the segment name for invalid names */
    if (!ValidSegName (Name)) {
        Error ("Illegal segment name: `%s'", Name);
    }

    /* Create a new segment and return it */
    return NewSegFromDef (NewSegDef (Name, AddrSize));
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
    if (OrgPerSeg) {
        /* Relocatable mode is switched per segment */
        if (!ActiveSeg->RelocMode) {
            ActiveSeg->AbsPC += F->Len;
        }
    } else {
        /* Relocatable mode is switched globally */
        if (!RelocMode) {
            AbsPC += F->Len;
        }
    }

    /* Return the fragment */
    return F;
}



void UseSeg (const SegDef* D)
/* Use the segment with the given name */
{
    unsigned I;
    for (I = 0; I < CollCount (&SegmentList); ++I) {
        Segment* Seg = CollAtUnchecked (&SegmentList, I);
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
    }

    /* Segment is not in list, create a new one */
    if (D->AddrSize == ADDR_SIZE_DEFAULT) {
        ActiveSeg = NewSegment (D->Name, ADDR_SIZE_ABS);
    } else {
        ActiveSeg = NewSegment (D->Name, D->AddrSize);
    }
}



unsigned long GetPC (void)
/* Get the program counter of the current segment */
{
    if (OrgPerSeg) {
        /* Relocatable mode is switched per segment */
        return ActiveSeg->RelocMode? ActiveSeg->PC : ActiveSeg->AbsPC;
    } else {
        /* Relocatable mode is switched globally */
        return RelocMode? ActiveSeg->PC : AbsPC;
    }
}



void EnterAbsoluteMode (unsigned long PC)
/* Enter absolute (non relocatable mode). Depending on the OrgPerSeg flag,
** this will either switch the mode globally or for the current segment.
*/
{
    if (OrgPerSeg) {
        /* Relocatable mode is switched per segment */
        ActiveSeg->RelocMode = 0;
        ActiveSeg->AbsPC = PC;
    } else {
        /* Relocatable mode is switched globally */
        RelocMode = 0;
        AbsPC = PC;
    }
}



int GetRelocMode (void)
/* Return true if we're currently in relocatable mode */
{
    if (OrgPerSeg) {
        /* Relocatable mode is switched per segment */
        return ActiveSeg->RelocMode;
    } else {
        /* Relocatable mode is switched globally */
        return RelocMode;
    }
}



void EnterRelocMode (void)
/* Enter relocatable mode. Depending on the OrgPerSeg flag, this will either
** switch the mode globally or for the current segment.
*/
{
    if (OrgPerSeg) {
        /* Relocatable mode is switched per segment */
        ActiveSeg->RelocMode = 1;
    } else {
        /* Relocatable mode is switched globally */
        RelocMode = 1;
    }
}



void SegAlign (unsigned long Alignment, int FillVal)
/* Align the PC segment to Alignment. If FillVal is -1, emit fill fragments
** (the actual fill value will be determined by the linker), otherwise use
** the given value.
*/
{
    unsigned char Data [4];
    unsigned long CombinedAlignment;
    unsigned long Count;

    /* The segment must have the combined alignment of all separate alignments
    ** in the source. Calculate this alignment and check it for sanity.
    */
    CombinedAlignment = LeastCommonMultiple (ActiveSeg->Align, Alignment);
    if (CombinedAlignment > MAX_ALIGNMENT) {
        Error ("Combined alignment for active segment is %lu which exceeds %lu",
               CombinedAlignment, MAX_ALIGNMENT);

        /* Avoid creating large fills for an object file that is thrown away
        ** later.
        */
        Count = 1;

    } else {
        ActiveSeg->Align = CombinedAlignment;

        /* Output a warning for larger alignments if not suppressed */
        if (CombinedAlignment > LARGE_ALIGNMENT && !LargeAlignment) {
            Warning (0, "Combined alignment is suspiciously large (%lu)",
                     CombinedAlignment);
        }

        /* Calculate the number of fill bytes */
        Count = AlignCount (ActiveSeg->PC, Alignment);

    }


    /* Emit the data or a fill fragment */
    if (FillVal != -1) {
        /* User defined fill value */
        memset (Data, FillVal, sizeof (Data));
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
}



unsigned char GetSegAddrSize (unsigned SegNum)
/* Return the address size of the segment with the given number */
{
    /* Is there such a segment? */
    if (SegNum >= CollCount (&SegmentList)) {
        FAIL ("Invalid segment number");
    }

    /* Return the address size */
    return ((Segment*) CollAtUnchecked (&SegmentList, SegNum))->Def->AddrSize;
}



void SegDone (void)
/* Check the segments for range and other errors. Do cleanup. */
{
    static const unsigned long U_Hi[4] = {
        0x000000FFUL, 0x0000FFFFUL, 0x00FFFFFFUL, 0xFFFFFFFFUL
    };
    static const long S_Hi[4] = {
        0x0000007FL, 0x00007FFFL, 0x007FFFFFL, 0x7FFFFFFFL
    };

    unsigned I;
    for (I = 0; I < CollCount (&SegmentList); ++I) {
        Segment* S = CollAtUnchecked (&SegmentList, I);
        Fragment* F = S->Root;
        while (F) {
            if (F->Type == FRAG_EXPR || F->Type == FRAG_SEXPR) {

                /* We have an expression, study it */
                ExprDesc ED;
                ED_Init (&ED);
                StudyExpr (F->V.Expr, &ED);

                /* Check if the expression is constant */
                if (ED_IsConst (&ED)) {

                    unsigned J;

                    /* The expression is constant. Check for range errors. */
                    CHECK (F->Len <= 4);
                    if (F->Type == FRAG_SEXPR) {
                        long Hi = S_Hi[F->Len-1];
                        long Lo = ~Hi;
                        if (ED.Val > Hi || ED.Val < Lo) {
                            LIError (&F->LI,
                                     "Range error (%ld not in [%ld..%ld])",
                                     ED.Val, Lo, Hi);
                        }
                    } else {
                        if (((unsigned long)ED.Val) > U_Hi[F->Len-1]) {
                            LIError (&F->LI,
                                     "Range error (%lu not in [0..%lu])",
                                     (unsigned long)ED.Val, U_Hi[F->Len-1]);
                        }
                    }

                    /* We don't need the expression tree any longer */
                    FreeExpr (F->V.Expr);

                    /* Convert the fragment into a literal fragment */
                    for (J = 0; J < F->Len; ++J) {
                        F->V.Data[J] = ED.Val & 0xFF;
                        ED.Val >>= 8;
                    }
                    F->Type = FRAG_LITERAL;

                } else if (RelaxChecks == 0) {

                    /* We cannot evaluate the expression now, leave the job for
                    ** the linker. However, we can check if the address size
                    ** matches the fragment size. Mismatches are errors in 
                    ** most situations.
                    */
                    if ((F->Len == 1 && ED.AddrSize > ADDR_SIZE_ZP)  ||
                        (F->Len == 2 && ED.AddrSize > ADDR_SIZE_ABS) ||
                        (F->Len == 3 && ED.AddrSize > ADDR_SIZE_FAR)) {
                        LIError (&F->LI, "Range error");
                    }
                }

                /* Release memory allocated for the expression decriptor */
                ED_Done (&ED);
            }
            F = F->Next;
        }
    }
}



void SegDump (void)
/* Dump the contents of all segments */
{
    unsigned I;
    unsigned X = 0;

    printf ("\n");
    for (I = 0; I < CollCount (&SegmentList); ++I) {
        Segment* S = CollAtUnchecked (&SegmentList, I);
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
    }
    printf ("\n");
}



void SegInit (void)
/* Initialize segments */
{
    /* Create the predefined segments. Code segment is active */
    ActiveSeg = NewSegFromDef (&CodeSegDef);
    NewSegFromDef (&RODataSegDef);
    NewSegFromDef (&BssSegDef);
    NewSegFromDef (&DataSegDef);
    NewSegFromDef (&ZeropageSegDef);
    NewSegFromDef (&NullSegDef);
}



void SetSegmentSizes (void)
/* Set the default segment sizes according to the memory model */
{
    /* Initialize segment sizes. The segment definitions do already contain
    ** the correct values for the default case (near), so we must only change
    ** things that should be different.
    */
    switch (MemoryModel) {

        case MMODEL_NEAR:
            break;

        case MMODEL_FAR:
            CodeSegDef.AddrSize = ADDR_SIZE_FAR;
            break;

        case MMODEL_HUGE:
            CodeSegDef.AddrSize   = ADDR_SIZE_FAR;
            DataSegDef.AddrSize   = ADDR_SIZE_FAR;
            BssSegDef.AddrSize    = ADDR_SIZE_FAR;
            RODataSegDef.AddrSize = ADDR_SIZE_FAR;
            break;

        default:
            Internal ("Invalid memory model: %d", MemoryModel);
    }
}



static void WriteOneSeg (Segment* Seg)
/* Write one segment to the object file */
{
    Fragment* Frag;
    unsigned long DataSize;
    unsigned long EndPos;

    /* Remember the file position, then write a dummy for the size of the
    ** following data
    */
    unsigned long SizePos = ObjGetFilePos ();
    ObjWrite32 (0);

    /* Write the segment data */
    ObjWriteVar (GetStringId (Seg->Def->Name)); /* Name of the segment */
    ObjWriteVar (Seg->Flags);                   /* Segment flags */
    ObjWriteVar (Seg->PC);                      /* Size */
    ObjWriteVar (Seg->Align);                   /* Segment alignment */
    ObjWrite8 (Seg->Def->AddrSize);             /* Address size of the segment */
    ObjWriteVar (Seg->FragCount);               /* Number of fragments */

    /* Now walk through the fragment list for this segment and write the
    ** fragments.
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
                    case 2:   ObjWrite8 (FRAG_EXPR16);  break;
                    case 3:   ObjWrite8 (FRAG_EXPR24);  break;
                    case 4:   ObjWrite8 (FRAG_EXPR32);  break;
                    default:  Internal ("Invalid fragment size: %u", Frag->Len);
                }
                WriteExpr (Frag->V.Expr);
                break;

            case FRAG_SEXPR:
                switch (Frag->Len) {
                    case 1:   ObjWrite8 (FRAG_SEXPR8);  break;
                    case 2:   ObjWrite8 (FRAG_SEXPR16); break;
                    case 3:   ObjWrite8 (FRAG_SEXPR24); break;
                    case 4:   ObjWrite8 (FRAG_SEXPR32); break;
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

        /* Write the line infos for this fragment */
        WriteLineInfo (&Frag->LI);

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
    unsigned I;

    /* Tell the object file module that we're about to start the seg list */
    ObjStartSegments ();

    /* First thing is segment count */
    ObjWriteVar (CollCount (&SegmentList));

    /* Now walk through all segments and write them to the object file */
    for (I = 0; I < CollCount (&SegmentList); ++I) {
        /* Write one segment */
        WriteOneSeg (CollAtUnchecked (&SegmentList, I));
    }

    /* Done writing segments */
    ObjEndSegments ();
}
