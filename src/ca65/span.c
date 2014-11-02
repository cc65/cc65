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
#include "hashfunc.h"
#include "hashtab.h"
#include "xmalloc.h"

/* ca65 */
#include "global.h"
#include "objfile.h"
#include "segment.h"
#include "span.h"
#include "spool.h"



/*****************************************************************************/
/*                                 Forwards                                  */
/*****************************************************************************/



static unsigned HT_GenHash (const void* Key);
/* Generate the hash over a key. */

static const void* HT_GetKey (const void* Entry);
/* Given a pointer to the user entry data, return a pointer to the key */

static int HT_Compare (const void* Key1, const void* Key2);
/* Compare two keys. The function must return a value less than zero if
** Key1 is smaller than Key2, zero if both are equal, and a value greater
** than zero if Key1 is greater then Key2.
*/



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Hash table functions */
static const HashFunctions HashFunc = {
    HT_GenHash,
    HT_GetKey,
    HT_Compare
};

/* Span hash table */
static HashTable SpanTab = STATIC_HASHTABLE_INITIALIZER (1051, &HashFunc);



/*****************************************************************************/
/*                           Hash table functions                            */
/*****************************************************************************/



static unsigned HT_GenHash (const void* Key)
/* Generate the hash over a key. */
{
    /* Key is a Span pointer */
    const Span* S = Key;

    /* Hash over a combination of segment number, start and end */
    return HashInt ((S->Seg->Num << 28) ^ (S->Start << 14) ^ S->End);
}



static const void* HT_GetKey (const void* Entry)
/* Given a pointer to the user entry data, return a pointer to the key */
{
    return Entry;
}



static int HT_Compare (const void* Key1, const void* Key2)
/* Compare two keys. The function must return a value less than zero if
** Key1 is smaller than Key2, zero if both are equal, and a value greater
** than zero if Key1 is greater then Key2.
*/
{
    /* Convert both parameters to Span pointers */
    const Span* S1 = Key1;
    const Span* S2 = Key2;

    /* Compare segment number, then start and end */
    int Res = (int)S2->Seg->Num - (int)S1->Seg->Num;
    if (Res == 0) {
        Res = (int)S2->Start - (int)S1->Start;
        if (Res == 0) {
            Res = (int)S2->End - (int)S1->End;
        }
    }

    /* Done */
    return Res;
}



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



static Span* NewSpan (Segment* Seg, unsigned long Start, unsigned long End)
/* Create a new span. The segment is set to Seg, Start and End are set to the
** current PC of the segment.
*/
{
    /* Allocate memory */
    Span* S = xmalloc (sizeof (Span));

    /* Initialize the struct */
    InitHashNode (&S->Node);
    S->Id       = ~0U;
    S->Seg      = Seg;
    S->Start    = Start;
    S->End      = End;
    S->Type     = EMPTY_STRING_ID;

    /* Return the new struct */
    return S;
}



static void FreeSpan (Span* S)
/* Free a span */
{
    xfree (S);
}



static Span* MergeSpan (Span* S)
/* Check if we have a span with the same data as S already. If so, free S and
** return the already existing one. If not, remember S and return it.
*/
{
    /* Check if we have such a span already. If so use the existing
    ** one and free the one from the collection. If not, add the one to
    ** the hash table and return it.
    */
    Span* E = HT_Find (&SpanTab, S);
    if (E) {
        /* If S has a type and E not, move the type */
        if (S->Type != EMPTY_STRING_ID) {
            CHECK (E->Type == EMPTY_STRING_ID);
            E->Type = S->Type;
        }

        /* Free S and return E */
        FreeSpan (S);
        return E;
    } else {
        /* Assign the id, insert S, then return it */
        S->Id = HT_GetCount (&SpanTab);
        HT_Insert (&SpanTab, S);
        return S;
    }
}



void SetSpanType (Span* S, const StrBuf* Type)
/* Set the generic type of the span to Type */
{                                 
    /* Ignore the call if we won't generate debug infos */
    if (DbgSyms) {
        S->Type = GetStrBufId (Type);
    }
}



Span* OpenSpan (void)
/* Open a span for the active segment and return it. */
{
    return NewSpan (ActiveSeg, ActiveSeg->PC, ActiveSeg->PC);
}



Span* CloseSpan (Span* S)
/* Close the given span. Be sure to replace the passed span by the one
** returned, since the span will get deleted if it is empty or may be
** replaced if a duplicate exists.
*/
{
    /* Set the end offset */
    if (S->Start == S->Seg->PC) {
        /* Span is empty */
        FreeSpan (S);
        return 0;
    } else {
        /* Span is not empty */
        S->End = S->Seg->PC;

        /* Check if we have such a span already. If so use the existing
        ** one and free the one from the collection. If not, add the one to
        ** the hash table and return it.
        */
        return MergeSpan (S);
    }
}



void OpenSpanList (Collection* Spans)
/* Open a list of spans for all existing segments to the given collection of
** spans. The currently active segment will be inserted first with all others
** following.
*/
{
    unsigned I;

    /* Grow the Spans collection as necessary */
    CollGrow (Spans, CollCount (&SegmentList));

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



void CloseSpanList (Collection* Spans)
/* Close a list of spans. This will add new segments to the list, mark the end
** of existing ones, and remove empty spans from the list.
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

            /* Merge duplicate spans, then insert it at the new position */
            CollReplace (Spans, MergeSpan (S), J++);
        }
    }

    /* New Count is now in J */
    Spans->Count = J;
}



void WriteSpanList (const Collection* Spans)
/* Write a list of spans to the output file */
{
    unsigned I;

    /* We only write spans if debug info is enabled */
    if (DbgSyms == 0) {
        /* Number of spans is zero */
        ObjWriteVar (0);
    } else {
        /* Write the number of spans */
        ObjWriteVar (CollCount (Spans));

        /* Write the spans */
        for (I = 0; I < CollCount (Spans); ++I) {
            /* Write the id of the next span */
            ObjWriteVar (((const Span*)CollConstAt (Spans, I))->Id);
        }
    }
}



static int CollectSpans (void* Entry, void* Data)
/* Collect all spans in a collection sorted by id */
{
    /* Cast the pointers to real objects */
    Span* S       = Entry;
    Collection* C = Data;

    /* Place the entry into the collection */
    CollReplaceExpand (C, S, S->Id);

    /* Keep the span */
    return 0;
}

                

void WriteSpans (void)
/* Write all spans to the object file */
{
    /* Tell the object file module that we're about to start the spans */
    ObjStartSpans ();

    /* We will write scopes only if debug symbols are requested */
    if (DbgSyms) {

        unsigned I;

        /* We must first collect all items in a collection sorted by id */
        Collection SpanList = STATIC_COLLECTION_INITIALIZER;
        CollGrow (&SpanList, HT_GetCount (&SpanTab));

        /* Walk over the hash table and fill the span list */
        HT_Walk (&SpanTab, CollectSpans, &SpanList);

        /* Write the span count to the file */
        ObjWriteVar (CollCount (&SpanList));

        /* Write all spans */
        for (I = 0; I < CollCount (&SpanList); ++I) {

            /* Get the span and check it */
            const Span* S = CollAtUnchecked (&SpanList, I);
            CHECK (S->End > S->Start);

            /* Write data for the span We will write the size instead of the
            ** end offset to save some bytes, since most spans are expected
            ** to be rather small.
            */
            ObjWriteVar (S->Seg->Num);
            ObjWriteVar (S->Start);
            ObjWriteVar (S->End - S->Start);
            ObjWriteVar (S->Type);
        }

        /* Free the collection with the spans */
        DoneCollection (&SpanList);

    } else {

        /* No debug info requested */
        ObjWriteVar (0);

    }

    /* Done writing the spans */
    ObjEndSpans ();
}
