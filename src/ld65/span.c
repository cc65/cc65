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
#include "gentype.h"
#include "xmalloc.h"

/* ld65 */
#include "fileio.h"
#include "objdata.h"
#include "segments.h"
#include "span.h"
#include "tpool.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Definition of a span */
struct Span {
    unsigned            Id;             /* Id of the span */
    unsigned            Sec;            /* Section id of this span */
    unsigned long       Offs;           /* Offset of span within segment */
    unsigned long       Size;           /* Size of span */
    unsigned            Type;           /* Generic type of the data */
};



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



static Span* NewSpan (unsigned Id)
/* Create and return a new span */
{
    /* Allocate memory */
    Span* S = xmalloc (sizeof (Span));

    /* Initialize the fields as necessary */
    S->Id       = Id;

    /* Return the result */
    return S;
}



Span* ReadSpan (FILE* F, ObjData* O, unsigned Id)
/* Read a Span from a file and return it */
{
    unsigned Type;

    /* Create a new Span and initialize it */
    Span* S = NewSpan (Id);
    S->Sec  = ReadVar (F);
    S->Offs = ReadVar (F);
    S->Size = ReadVar (F);

    /* Read the type. An id of zero means an empty string (no need to check) */
    Type    = ReadVar (F);
    if (Type == 0) {
        S->Type = INVALID_TYPE_ID;
    } else {
        S->Type = GetTypeId (GetObjString (O, Type));
    }

    /* Return the new span */
    return S;
}



unsigned* ReadSpanList (FILE* F)
/* Read a list of span ids from a file. The list is returned as an array of
** unsigneds, the first being the number of spans (never zero) followed by
** the span ids. If the number of spans is zero, NULL is returned.
*/
{
    unsigned* Spans;

    /* First is number of Spans */
    unsigned Count = ReadVar (F);
    if (Count == 0) {
        return 0;
    }

    /* Allocate memory for the list and set the count */
    Spans  = xmalloc ((Count + 1) * sizeof (*Spans));
    *Spans = Count;

    /* Read the spans and add them */
    while (Count) {
        Spans[Count] = ReadVar (F);
        --Count;
    }

    /* Return the list */
    return Spans;
}



unsigned* DupSpanList (const unsigned* S)
/* Duplicate a span list */
{
    unsigned Size;

    /* The list may be empty */
    if (S == 0) {
        return 0;
    }

    /* Allocate memory, copy and return the new list */
    Size = (*S + 1) * sizeof (*S);
    return memcpy (xmalloc (Size), S, Size);
}



void FreeSpan (Span* S)
/* Free a span structure */
{
    /* Just free the structure */
    xfree (S);
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



void PrintDbgSpanList (FILE* F, const ObjData* O, const unsigned* List)
/* Output a string ",span=x[+y...]" for the given list. If the list is empty
** or NULL, output nothing. This is a helper function for other modules to
** print a list of spans read by ReadSpanList to the debug info file.
*/
{
    if (List && *List) {
        unsigned I;
        const char* Format = ",span=%u";
        for (I = 0; I < *List; ++I) {
            fprintf (F, Format, O->SpanBaseId + List[I+1]);
            Format = "+%u";
        }
    }
}



void PrintDbgSpans (FILE* F)
/* Output the spans to a debug info file */
{
    unsigned I, J;
    StrBuf SpanType = STATIC_STRBUF_INITIALIZER;


    /* Walk over all object files */
    for (I = 0; I < CollCount (&ObjDataList); ++I) {

        /* Get this object file */
        ObjData* O = CollAtUnchecked (&ObjDataList, I);

        /* Walk over all spans in this object file */
        for (J = 0; J < CollCount (&O->Spans); ++J) {

            /* Get this span */
            const Span* S = CollAtUnchecked (&O->Spans, J);

            /* Get the section for this span */
            const Section* Sec = GetObjSection (O, S->Sec);

            /* Output the data */
            fprintf (F, "span\tid=%u,seg=%u,start=%lu,size=%lu",
                     O->SpanBaseId + S->Id,
                     Sec->Seg->Id,
                     Sec->Offs + S->Offs,
                     S->Size);

            /* If we have a type, add it */
            if (S->Type != INVALID_TYPE_ID) {
                fprintf (F, ",type=%u", S->Type);
            }

            /* Terminate the output line */
            fputc ('\n', F);
        }
    }

    /* Free the string buffer */
    SB_Done (&SpanType);
}
