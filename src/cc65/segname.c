/*****************************************************************************/
/*                                                                           */
/*				   segname.c				     */
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



#include <string.h>
#include <ctype.h>					      

/* common */
#include "xmalloc.h"

/* cc65 */
#include "check.h"
#include "segname.h"



/*****************************************************************************/
/*	       	    		     Data		      		     */
/*****************************************************************************/



/* Actual names for the segments */
char* SegmentNames[SEG_COUNT];



/*****************************************************************************/
/*	       	    		     Code				     */
/*****************************************************************************/



void InitSegNames (void)
/* Initialize the segment names */
{
    SegmentNames [SEG_BSS]	= xstrdup ("BSS");
    SegmentNames [SEG_CODE] 	= xstrdup ("CODE");
    SegmentNames [SEG_DATA]	= xstrdup ("DATA");
    SegmentNames [SEG_RODATA]	= xstrdup ("RODATA");
}



void NewSegName (segment_t Seg, const char* Name)
/* Set a new name for a segment */
{
    /* Check the parameter */
    CHECK (Seg != SEG_INV);

    /* Free the old name and set a new one */
    xfree (SegmentNames [Seg]);
    SegmentNames [Seg] = xstrdup (Name);
}



int ValidSegName (const char* Name)
/* Return true if the given segment name is valid, return false otherwise */
{
    /* Must start with '_' or a letter */
    if ((*Name != '_' && !isalpha(*Name)) || strlen(Name) > 80) {
     	return 0;
    }

    /* Can have letters, digits or the underline */
    while (*++Name) {
     	if (*Name != '_' && !isalnum(*Name)) {
     	    return 0;
     	}
    }

    /* Name is ok */
    return 1;
}



