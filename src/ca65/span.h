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
#include "gentype.h"
#include "hashtab.h"
#include "inline.h"
#include "strbuf.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Forwards */
struct Segment;

/* Span definition */
typedef struct Span Span;
struct Span{
    HashNode        Node;               /* Node for hash table */
    unsigned        Id;                 /* Id of span */
    struct Segment* Seg;                /* Pointer to segment */
    unsigned        Start;              /* Start of range */
    unsigned        End;                /* End of range */
    unsigned        Type;               /* Type of data in span */
};



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



#if defined(HAVE_INLINE)
INLINE unsigned long GetSpanSize (const Span* R)
/* Return the span size in bytes */
{
    return (R->End - R->Start);
}
#else
#  define GetSpanSize(R)   ((R)->End - (R)->Start)
#endif

void SetSpanType (Span* S, const StrBuf* Type);
/* Set the generic type of the span to Type */

Span* OpenSpan (void);
/* Open a span for the active segment and return it. */

Span* CloseSpan (Span* S);
/* Close the given span. Be sure to replace the passed span by the one
** returned, since the span will get deleted if it is empty or may be
** replaced if a duplicate exists.
*/

void OpenSpanList (Collection* Spans);
/* Open a list of spans for all existing segments to the given collection of
** spans. The currently active segment will be inserted first with all others
** following.
*/

void CloseSpanList (Collection* Spans);
/* Close all open spans by setting PC to the current PC for the segment. */

void WriteSpanList (const Collection* Spans);
/* Write a list of spans to the output file */

void WriteSpans (void);
/* Write all spans to the object file */



/* End of span.h */

#endif
