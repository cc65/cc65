/*****************************************************************************/
/*                                                                           */
/*				   segname.h				     */
/*                                                                           */
/*			    Segment name management			     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000     Ullrich von Bassewitz                                        */
/*              Wacholderweg 14                                              */
/*              D-70597 Stuttgart                                            */
/* EMail:       uz@musoftware.de                                             */
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



#ifndef SEGNAME_H
#define SEGNAME_H



/*****************************************************************************/
/*	       	    		     Data				     */
/*****************************************************************************/



/* Current segment */
typedef enum segment_t {
    SEG_INV = -1,     	/* Invalid segment */
    SEG_CODE,
    SEG_RODATA,
    SEG_DATA,
    SEG_BSS,
    SEG_COUNT
} segment_t;

/* Actual names for the segments */
extern char* SegmentNames[SEG_COUNT];



/*****************************************************************************/
/*	       	    		     Code				     */
/*****************************************************************************/



void InitSegNames (void);
/* Initialize the segment names */

void NewSegName (segment_t Seg, const char* Name);
/* Set a new name for a segment */

int ValidSegName (const char* Name);
/* Return true if the given segment name is valid, return false otherwise */



/* End of segname.h */

#endif



