/*****************************************************************************/
/*                                                                           */
/*                                  span.c                                   */
/*                                                                           */
/*                      A span of data within a segment                      */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2003-2011, Ullrich von Bassewitz                                      */
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



/* common */
#include "xmalloc.h"

/* ca65 */
#include "objfile.h"
#include "segment.h"
#include "span.h"



/*****************************************************************************/
/*     	      	      	   	     Code				     */
/*****************************************************************************/



Span* NewSpan (struct Segment* Seg)
/* Create a new span. The segment is set to Seg, Start and End are set to the
 * current PC of the segment.
 */
{
    /* Allocate memory */
    Span* S = xmalloc (sizeof (Span));

    /* Initialize the struct */
    S->Seg      = Seg;
    S->Start    = Seg->PC;
    S->End      = Seg->PC;

    /* Return the new struct */
    return S;
}



void AddSpans (Collection* Spans)
/* Add a span for all existing segments to the given collection of spans. The
 * currently active segment will be inserted first with all others following.
 */
{
    unsigned I;

    /* Add the currently active segment */
    CollAppend (Spans, NewSpan (ActiveSeg));

    /* Walk through the segment list and add all other segments */
    for (I = 0; I < CollCount (&SegmentList); ++I) {
        Segment* Seg = CollAtUnchecked (&SegmentList, I);

        /* Be sure to skip the active segment, since it was already added */
        if (Seg != ActiveSeg) {
            CollAppend (Spans, NewSpan (Seg));
        }
    }
}



void CloseSpans (Collection* Spans)
/* Close all open spans by setting PC to the current PC for the segment. */
{
    unsigned I;

    /* Walk over the segment list */
    for (I = 0; I < CollCount (Spans); ++I) {

        /* Get the next segment range */
        Span* S = CollAtUnchecked (Spans, I);

        /* Set the end offset */
        S->End = S->Seg->PC;
    }
}



void WriteSpans (const Collection* Spans)
/* Write a list of spans to the output file */
{
    unsigned I;
    unsigned Count;

    /* Determine how many of the segments contain actual data */
    Count = 0;
    for (I = 0; I < CollCount (Spans); ++I) {

        /* Get next range */
        const Span* S = CollConstAt (Spans, I);

        /* Is this segment range empty? */
        if (S->Start != S->End) {
            ++Count;
        }
    }

    /* Write the number of spans with data */
    ObjWriteVar (Count);

    /* Write the spans */
    for (I = 0; I < CollCount (Spans); ++I) {

        /* Get next range */
        const Span* S = CollConstAt (Spans, I);

        /* Write data for non empty spans. We will write the size instead of
         * the end offset to save some bytes, since most spans are expected
         * to be rather small.
         */
        if (S->Start != S->End) {
            ObjWriteVar (S->Seg->Num);
            ObjWriteVar (S->Start);
            ObjWriteVar (S->End - S->Start);
        }
    }
}




