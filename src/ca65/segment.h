/*****************************************************************************/
/*                                                                           */
/*				   segment.h				     */
/*                                                                           */
/*                   Segments for the ca65 macroassembler                    */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2007 Ullrich von Bassewitz                                       */
/*               Roemerstrasse 52                                            */
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



#ifndef SEGMENT_H
#define SEGMENT_H



/* common */
#include "fragdefs.h"
#include "inline.h"
#include "segdefs.h"

/* ca65 */
#include "fragment.h"



/*****************************************************************************/
/*	   			     Data				     */
/*****************************************************************************/



/* Segment definition */
typedef struct Segment Segment;
struct Segment {
    Segment*   	    List;      	       	/* List of all segments */
    Fragment*  	    Root;      	  	/* Root of fragment list */
    Fragment*  	    Last;      	  	/* Pointer to last fragment */
    unsigned long   FragCount;          /* Number of fragments */
    unsigned        Num;       		/* Segment number */
    unsigned        Align;     		/* Segment alignment */
    int             RelocMode;          /* Relocatable mode if OrgPerSeg */
    unsigned long   PC;                 /* PC if in relocatable mode */
    unsigned long   AbsPC;              /* PC if in local absolute mode */
                                        /* (OrgPerSeg is true) */
    SegDef*         Def;                /* Segment definition (name and type) */
};

/* Definitions for predefined segments */
extern SegDef NullSegDef;
extern SegDef ZeropageSegDef;
extern SegDef DataSegDef;
extern SegDef BssSegDef;
extern SegDef RODataSegDef;
extern SegDef CodeSegDef;

/* List of all segments */
extern Segment* SegmentList;

/* Currently active segment */
extern Segment* ActiveSeg;



/*****************************************************************************/
/*     	      	      	   	     Code				     */
/*****************************************************************************/



Fragment* GenFragment (unsigned char Type, unsigned short Len);
/* Generate a new fragment, add it to the current segment and return it. */

void UseSeg (const SegDef* D);
/* Use the given segment */

#if defined(HAVE_INLINE)
INLINE const SegDef* GetCurrentSegDef (void)
/* Get a pointer to the segment defininition of the current segment */
{
    return ActiveSeg->Def;
}
#else
#  define GetCurrentSegDef()    (ActiveSeg->Def)
#endif

#if defined(HAVE_INLINE)
INLINE unsigned GetCurrentSegNum (void)
/* Get the number of the current segment */
{
    return ActiveSeg->Num;
}
#else
#  define GetCurrentSegNum()    (ActiveSeg->Num)
#endif

#if defined(HAVE_INLINE)
INLINE unsigned char GetCurrentSegAddrSize (void)
/* Get the address size of the current segment */
{
    return ActiveSeg->Def->AddrSize;
}
#else
#  define GetCurrentSegAddrSize()    (ActiveSeg->Def->AddrSize)
#endif

void SegAlign (unsigned Power, int Val);
/* Align the PC segment to 2^Power. If Val is -1, emit fill fragments (the
 * actual fill value will be determined by the linker), otherwise use the
 * given value.
 */

unsigned char GetSegAddrSize (unsigned SegNum);
/* Return the address size of the segment with the given number */

unsigned long GetPC (void);
/* Get the program counter of the current segment */

int GetRelocMode (void);
/* Return true if we're currently in relocatable mode */

void EnterAbsoluteMode (unsigned long AbsPC);
/* Enter absolute (non relocatable mode). Depending on the OrgPerSeg flag,
 * this will either switch the mode globally or for the current segment.
 */

void EnterRelocMode (void);
/* Enter relocatable mode. Depending on the OrgPerSeg flag, this will either
 * switch the mode globally or for the current segment.
 */

void SegCheck (void);
/* Check the segments for range and other errors */

void SegDump (void);
/* Dump the contents of all segments */

void InitSegments (void);
/* Initialize segments */

void WriteSegments (void);
/* Write the segment data to the object file */



/* End of segment.h */

#endif



