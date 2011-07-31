/*****************************************************************************/
/*                                                                           */
/*                                segrange.c                                 */
/*                                                                           */
/*                              A segment range                              */
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
#include "segrange.h"



/*****************************************************************************/
/*     	      	      	   	     Code				     */
/*****************************************************************************/



SegRange* NewSegRange (struct Segment* Seg)
/* Create a new segment range. The segment is set to Seg, Start and End are
 * set to the current PC of the segment.
 */
{
    /* Allocate memory */
    SegRange* R = xmalloc (sizeof (SegRange));

    /* Initialize the struct */
    R->Seg      = Seg;
    R->Start    = Seg->PC;
    R->End      = Seg->PC;

    /* Return the new struct */
    return R;
}



void AddSegRanges (Collection* Ranges)
/* Add a segment range for all existing segments to the given collection of
 * ranges. The currently active segment will be inserted first with all others
 * following.
 */
{
    Segment* Seg;

    /* Add the currently active segment */
    CollAppend (Ranges, NewSegRange (ActiveSeg));

    /* Walk through the segment list and add all other segments */
    Seg = SegmentList;
    while (Seg) {
        /* Be sure to skip the active segment, since it was already added */
        if (Seg != ActiveSeg) {
            CollAppend (Ranges, NewSegRange (Seg));
        }
        Seg = Seg->List;
    }
}



void CloseSegRanges (Collection* Ranges)
/* Close all open segment ranges by setting PC to the current PC for the
 * segment.
 */
{
    unsigned I;

    /* Walk over the segment list */
    for (I = 0; I < CollCount (Ranges); ++I) {

        /* Get the next segment range */
        SegRange* R = CollAtUnchecked (Ranges, I);

        /* Set the end offset */
        R->End = R->Seg->PC;
    }
}



void WriteSegRanges (const Collection* Ranges)
/* Write a list of segment ranges to the output file */
{
    unsigned I;
    unsigned Count;

    /* Determine how many of the segments contain actual data */
    Count = 0;
    for (I = 0; I < CollCount (Ranges); ++I) {

        /* Get next range */
        const SegRange* R = CollConstAt (Ranges, I);

        /* Is this segment range empty? */
        if (R->Start != R->End) {
            ++Count;
        }
    }

    /* Write the number of ranges with data */
    ObjWriteVar (Count);

    /* Write the ranges */
    for (I = 0; I < CollCount (Ranges); ++I) {

        /* Get next range */
        const SegRange* R = CollConstAt (Ranges, I);

        /* Write data for non empty ranges */
        if (R->Start != R->End) {
            ObjWriteVar (R->Seg->Num);
            ObjWriteVar (R->Start);
            ObjWriteVar (R->End);
        }
    }
}




