/*****************************************************************************/
/*                                                                           */
/*                                  span.h                                   */
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



#ifndef SPAN_H
#define SPAN_H



/* common */
#include "coll.h"
#include "inline.h"



/*****************************************************************************/
/*	   			     Data   				     */
/*****************************************************************************/



/* Span definition */
typedef struct Span Span;
struct Span{
    struct Segment* Seg;       	       	/* Pointer to segment */
    unsigned long   Start;              /* Start of range */
    unsigned long   End;                /* End of range */
};



/*****************************************************************************/
/*     	      	      	   	     Code   				     */
/*****************************************************************************/



Span* NewSpan (struct Segment* Seg);
/* Create a new span. The segment is set to Seg, Start and End are set to the
 * current PC of the segment.
 */

#if defined(HAVE_INLINE)
INLINE unsigned long GetSpanSize (const Span* R)
/* Return the span size in bytes */
{
    return (R->End - R->Start);
}
#else
#  define GetSpanSize(R)   ((R)->End - (R)->Start)
#endif

void AddSpans (Collection* Spans);
/* Add a span for all existing segments to the given collection of spans. The
 * currently active segment will be inserted first with all others following.
 */

void CloseSpans (Collection* Spans);
/* Close all open spans by setting PC to the current PC for the segment. */

void WriteSpans (const Collection* Spans);
/* Write a list of spans to the output file */



/* End of span.h */

#endif



