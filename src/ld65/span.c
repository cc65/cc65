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
#include "attrib.h"
#include "xmalloc.h"

/* ld65 */
#include "fileio.h"
#include "objdata.h"
#include "segments.h"
#include "span.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Definition of a span */
struct Span {
    unsigned		Id;		/* Id of the span */
    unsigned            Sec;            /* Section id of this span */
    unsigned long       Offs;           /* Offset of span within segment */
    unsigned long       Size;           /* Size of span */
};



/*****************************************************************************/
/*     	       	       	      	     Code			     	     */
/*****************************************************************************/



static Span* NewSpan (unsigned Id, unsigned Sec, unsigned long Offs, unsigned long Size)
/* Create and return a new span */
{
    /* Allocate memory */
    Span* S = xmalloc (sizeof (*S));

    /* Initialize the fields */
    S->Id       = Id;
    S->Sec      = Sec;
    S->Offs     = Offs;
    S->Size     = Size;

    /* Return the result */
    return S;
}



Span* ReadSpan (FILE* F, ObjData* O attribute ((unused)), unsigned Id)
/* Read a Span from a file and return it */
{
    /* Create a new Span and return it */
    unsigned SecId     = ReadVar (F);
    unsigned long Offs = ReadVar (F);
    unsigned Size      = ReadVar (F);
    return NewSpan (Id, SecId, Offs, Size);
}



void ReadSpanList (Collection* Spans, FILE* F, ObjData* O)
/* Read a list of span ids from a file and return the spans for the ids */
{
    /* First is number of Spans */
    unsigned Count = ReadVar (F);

    /* Preallocate enough entries in the collection */
    CollGrow (Spans, Count);

    /* Read the spans and add them */
    while (Count--) {
        CollAppend (Spans, CollAt (&O->Spans, ReadVar (F)));
    }
}



void FreeSpan (Span* S)
/* Free a span structure */
{
    /* Just free the structure */
    xfree (S);
}



unsigned SpanId (const struct ObjData* O, const Span* S)
/* Return the global id of a span */
{
    return O->SpanBaseId + S->Id;
}



unsigned SpanCount (void)
/* Return the total number of spans */
{
    /* Walk over all object files */
    unsigned I;
    unsigned Count = 0;
    for (I = 0; I < CollCount (&ObjDataList); ++I) {

        /* Get this object file */
        const ObjData* O = CollAtUnchecked (&ObjDataList, I);

        /* Count spans */
        Count += CollCount (&O->Spans);
    }

    return Count;
}



void PrintDbgSpans (FILE* F)
/* Output the spans to a debug info file */
{
    unsigned I, J;

    /* Walk over all object files */
    for (I = 0; I < CollCount (&ObjDataList); ++I) {

        /* Get this object file */
        ObjData* O = CollAtUnchecked (&ObjDataList, I);

        /* Walk over all spans in this object file */
        for (J = 0; J < CollCount (&O->Spans); ++J) {

            /* Get this span */
            Span* S = CollAtUnchecked (&O->Spans, J);

            /* Get the section for this span */
            const Section* Sec = GetObjSection (O, S->Sec);

            /* Output the data */
            fprintf (F, "span\tid=%u,seg=%u,start=%lu,size=%lu\n",
                     O->SpanBaseId + S->Id,
                     Sec->Seg->Id,
                     Sec->Offs + S->Offs,
                     S->Size);
        }
    }
}



