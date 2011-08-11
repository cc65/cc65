/*****************************************************************************/
/*                                                                           */
/*                                  span.h                                   */
/*                                                                           */
/*                      A span of data within a segment                      */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2011,      Ullrich von Bassewitz                                      */
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



#include <stdio.h>

/* common */
#include "coll.h"



/*****************************************************************************/
/*	       			   Forwards                                  */
/*****************************************************************************/



struct ObjData;
struct Segment;



/*****************************************************************************/
/*  	       	 		     Data                                    */
/*****************************************************************************/



typedef struct Span Span;
struct Span {
    unsigned		Id;		/* Id of the span */
    struct Segment*     Seg;            /* Segment of this span */
    unsigned long       Offs;           /* Offset of span within segment */
    unsigned long       Size;           /* Size of span */
};



/*****************************************************************************/
/*     	       	      	      	     Code			     	     */
/*****************************************************************************/



Span* NewSpan (struct Segment* Seg, unsigned long Offs, unsigned long Size);
/* Create and return a new span */

Span* ReadSpan (FILE* F, struct ObjData* O);
/* Read a Span from a file and return it */

void ReadSpans (Collection* Spans, FILE* F, struct ObjData* O);
/* Read a list of Spans from a file and return it */

void FreeSpan (Span* S);
/* Free a span structure */

void AddSpan (Collection* Spans, struct Segment* Seg, unsigned long Offs,
              unsigned long Size);
/* Either add a new span to the ones already in the given collection, or - if
 * possible - merge it with adjacent ones that already exist.
 */

unsigned SpanCount (void);
/* Return the total number of spans */

void PrintDbgSpans (FILE* F);
/* Output the spans to a debug info file */



/* End of span.h */
#endif



