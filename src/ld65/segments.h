/*****************************************************************************/
/*                                                                           */
/*				  segments.h				     */
/*                                                                           */
/*		     Segment handling for the ld65 linker		     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2000 Ullrich von Bassewitz                                       */
/*               Wacholderweg 14                                             */
/*               D-70597 Stuttgart                                           */
/* EMail:        uz@musoftware.de                                            */
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



#ifndef SEGMENTS_H
#define SEGMENTS_H



#include <stdio.h>

#include "../common/exprdefs.h"

#include "objdata.h"



/*****************************************************************************/
/*     	      	     		     Data  				     */
/*****************************************************************************/



/* Forward for the section structure (a section is a part of a segment) */
typedef struct Section_ Section;

/* Segment structure */
typedef struct Segment_ Segment;
struct Segment_ {
    Segment*	    	Next;		/* Hash list */
    Segment*	    	List;		/* List of all segments */
    Section*		SecRoot;	/* Section list */
    Section*		SecLast;	/* Pointer to last section */
    unsigned long   	PC;    		/* PC were this segment is located */
    unsigned long   	Size;		/* Size of data so far */
    ObjData*		AlignObj;	/* Module that requested the alignment */
    unsigned char   	Align;		/* Alignment needed */
    unsigned char	FillVal;	/* Value to use for fill bytes */
    unsigned char	Type;		/* Type of segment */
    char	    	Dumped;		/* Did we dump this segment? */
    char       	       	Name [1];  	/* Name, dynamically allocated */
};



/* Section structure (a section is a part of a segment) */
struct Section_ {
    Section*		Next;		/* List of sections in a segment */
    Segment*	    	Seg;		/* Segment that contains the section */
    struct Fragment_*	FragRoot;	/* Fragment list */
    struct Fragment_*	FragLast;	/* Pointer to last fragment */
    unsigned long   	Offs;		/* Offset into the segment */
    unsigned long   	Size;		/* Size of the section */
    unsigned char   	Align;		/* Alignment */
    unsigned char	Fill;		/* Fill bytes for alignment */
    unsigned char	Type;		/* Type of segment */
};



/* Prototype for a function that is used to write expressions to the target
 * file (used in SegWrite). It returns one of the following values:
 */
#define SEG_EXPR_OK		0	/* Ok */
#define SEG_EXPR_RANGE_ERROR	1	/* Range error */
#define SEG_EXPR_TOO_COMPLEX	2	/* Expression too complex */

typedef unsigned (*SegWriteFunc) (ExprNode* E, 	      /* The expression to write */
				  int Signed,  	      /* Signed expression? */
				  unsigned Size,      /* Size (=range) */
				  unsigned long Offs, /* File offset */
				  void* Data);	      /* Callers data */



/*****************************************************************************/
/*     	      	     	   	     Code  	      	  	  	     */
/*****************************************************************************/



Section* ReadSection (FILE* F, ObjData* O);
/* Read a section from a file */

Segment* SegFind (const char* Name);
/* Return the given segment or NULL if not found. */

int IsBSSType (Segment* S);
/* Check if the given segment is a BSS style segment, that is, it does not
 * contain non-zero data.
 */

void SegDump (void);
/* Dump the segments and it's contents */

unsigned SegWriteConstExpr (FILE* F, ExprNode* E, int Signed, unsigned Size);
/* Write a supposedly constant expression to the target file. Do a range
 * check and return one of the SEG_EXPR_xxx codes.
 */

void SegWrite (FILE* Tgt, Segment* S, SegWriteFunc F, void* Data);
/* Write the data from the given segment to a file. For expressions, F is
 * called (see description of SegWriteFunc above).
 */

void PrintSegmentMap (FILE* F);
/* Print a segment map to the given file */

void CheckSegments (void);
/* Walk through the segment list and check if there are segments that were
 * not written to the output file. Output an error if this is the case.
 */



/* End of segments.h */

#endif



