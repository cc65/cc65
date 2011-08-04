/*****************************************************************************/
/*                                                                           */
/*                                  span.c                                   */
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



/* common */
#include "xmalloc.h"
                   
/* ld65 */       
#include "span.h"



/*****************************************************************************/
/*     	       	      	      	     Code			     	     */
/*****************************************************************************/



Span* NewSpan (struct Segment* Seg, unsigned long Offs, unsigned long Size)
/* Create and return a new span */                                         
{
    /* Allocate memory */
    Span* S = xmalloc (sizeof (*S));

    /* Initialize the fields */
    S->Seg      = Seg;
    S->Offs     = Offs;
    S->Size     = Size;

    /* Return the result */
   return S;
}



void FreeSpan (Span* S)
/* Free a span structure */
{
    /* Just free the structure */
    xfree (S);
}



void AddSpan (Collection* Spans, struct Segment* Seg, unsigned long Offs,
              unsigned long Size)
/* Either add a new span to the ones already in the given collection, or - if
 * possible - merge it with adjacent ones that already exist.
 */                              
{
    unsigned I;

    /* We don't have many spans in a collection, so we do a linear search here.
     * The collection is kept sorted which eases our work here.
     */
    for (I = 0; I < CollCount (Spans); ++I) {

        /* Get the next span */
    	Span* S = CollAtUnchecked (Spans, I);

        /* Must be same segment, otherwise we cannot merge */
        if (S->Seg != Seg) {
            continue;
        }

        /* Check if we can merge it */
        if (Offs < S->Offs) {

            /* Got the insert position */
            if (Offs + Size == S->Offs) {
                /* Merge the two */
                S->Offs = Offs;
                S->Size += Size;
            } else {
                /* Insert a new entry */
                CollInsert (Spans, NewSpan (Seg, Offs, Size), I);
            }

            /* Done */
            return;

        } else if (S->Offs + S->Size == Offs) {

            /* This is the regular case. Merge the two. */
            S->Size += Size;

            /* Done */
            return;
        }
    }

    /* We must append an entry */
    CollAppend (Spans, NewSpan (Seg, Offs, Size));
}



