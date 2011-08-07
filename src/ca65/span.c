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
/*                                   Data                                    */
/*****************************************************************************/



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



static Span* NewSpan (Segment* Seg, unsigned long Start, unsigned long End)
/* Create a new span. The segment is set to Seg, Start and End are set to the
 * current PC of the segment.
 */
{
    /* Allocate memory */
    Span* S = xmalloc (sizeof (Span));

    /* Initialize the struct */
    S->Seg      = Seg;
    S->Start    = Start;
    S->End      = End;

    /* Return the new struct */
    return S;
}



static void FreeSpan (Span* S)
/* Free a span */
{
    xfree (S);
}



void OpenSpans (Collection* Spans)
/* Open a list of spans for all existing segments to the given collection of
 * spans. The currently active segment will be inserted first with all others
 * following.
 */
{
    unsigned I;

    /* Add the currently active segment */
    CollAppend (Spans, NewSpan (ActiveSeg, ActiveSeg->PC, ActiveSeg->PC));

    /* Walk through the segment list and add all other segments */
    for (I = 0; I < CollCount (&SegmentList); ++I) {
        Segment* Seg = CollAtUnchecked (&SegmentList, I);

        /* Be sure to skip the active segment, since it was already added */
        if (Seg != ActiveSeg) {
            CollAppend (Spans, NewSpan (Seg, Seg->PC, Seg->PC));
        }
    }
}



void CloseSpans (Collection* Spans)
/* Close a list of spans. This will add new segments to the list, mark the end
 * of existing ones, and remove empty spans from the list.
 */
{
    unsigned I, J;

    /* Have new segments been added while the span list was open? */
    for (I = CollCount (Spans); I < CollCount (&SegmentList); ++I) {

        /* Add new spans if not empty */
        Segment* S = CollAtUnchecked (&SegmentList, I);
        if (S->PC == 0) {
            /* Segment is empty */
            continue;
        }
        CollAppend (Spans, NewSpan (S, 0, S->PC));
    }

    /* Walk over the spans, close open, remove empty ones */
    for (I = 0, J = 0; I < CollCount (Spans); ++I) {

        /* Get the next span */
        Span* S = CollAtUnchecked (Spans, I);

        /* Set the end offset */
        if (S->Start == S->Seg->PC) {
            /* Span is empty */
            FreeSpan (S);
        } else {
            /* Span is not empty */
            S->End = S->Seg->PC;
            CollReplace (Spans, S, J++);
        }
    }

    /* New Count is now in J */
    Spans->Count = J;
}



void WriteSpans (const Collection* Spans)
/* Write a list of spans to the output file */
{
    unsigned I;

    /* Write the number of spans */
    ObjWriteVar (CollCount (Spans));

    /* Write the spans */
    for (I = 0; I < CollCount (Spans); ++I) {

        /* Get next range */
        const Span* S = CollConstAt (Spans, I);

        /* Write data for th span We will write the size instead of the end
         * offset to save some bytes, since most spans are expected to be
         * rather small.
         */
        ObjWriteVar (S->Seg->Num);
        ObjWriteVar (S->Start);
        ObjWriteVar (S->End - S->Start);
    }
}




