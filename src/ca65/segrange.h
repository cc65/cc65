/*****************************************************************************/
/*                                                                           */
/*                                segrange.h                                 */
/*                                                                           */
/*                              A segment range                              */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2003      Ullrich von Bassewitz                                       */
/*               Römerstraße 52                                              */
/*               D-70794 Filderstadt                                         */
/* EMail:        uz@cc65.org                                                 */
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



#ifndef SEGRANGE_H
#define SEGRANGE_H



/* common */
#include "coll.h"
#include "inline.h"



/*****************************************************************************/
/*	   			     Data   				     */
/*****************************************************************************/



/* Segment range definition */
typedef struct SegRange SegRange;
struct SegRange{
    struct Segment* Seg;       	       	/* Pointer to segment */
    unsigned long   Start;              /* Start of range */
    unsigned long   End;                /* End of range */
};



/*****************************************************************************/
/*     	      	      	   	     Code   				     */
/*****************************************************************************/



SegRange* NewSegRange (struct Segment* Seg);
/* Create a new segment range. The segment is set to Seg, Start and End are
 * set to the current PC of the segment.
 */
                                            
#if defined(HAVE_INLINE)
INLINE unsigned long GetSegRangeSize (const SegRange* R)
/* Return the segment range size in bytes */
{
    return (R->End - R->Start);
}
#else  
#  define GetSegRangeSize (R)   ((R)->End - (R)->Start)
#endif

void AddSegRanges (Collection* Ranges);
/* Add a segment range for all existing segments to the given collection of
 * ranges. The currently active segment will be inserted first with all others
 * following.
 */

void CloseSegRanges (Collection* Ranges);
/* Close all open segment ranges by setting PC to the current PC for the
 * segment.
 */



/* End of segrange.h */

#endif



