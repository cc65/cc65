/*****************************************************************************/
/*                                                                           */
/*		   		   dataseg.h				     */
/*                                                                           */
/*		   	    Data segment structure			     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2001      Ullrich von Bassewitz                                       */
/*               Wacholderweg 14                                             */
/*               D-70597 Stuttgart                                           */
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



#ifndef DATASEG_H
#define DATASEG_H



#include <stdarg.h>
#include <stdio.h>

/* common */
#include "attrib.h"
#include "coll.h"

/* cc65 */
#include "symentry.h"



/*****************************************************************************/
/*  	       	 	  	     Data		    		     */
/*****************************************************************************/



typedef struct DataSeg DataSeg;
struct DataSeg {
    char*		SegName; 	/* Segment name */
    SymEntry*		Func;		/* Owner function */
    Collection 	       	Lines;	       	/* List of code lines */
};



/*****************************************************************************/
/*     	       	       	  	     Code				     */
/*****************************************************************************/



DataSeg* NewDataSeg (const char* SegName, SymEntry* Func);
/* Create a new data segment, initialize and return it */

void AppendDataSeg (DataSeg* Target, const DataSeg* Source);
/* Append the data from Source to Target. */

void AddDataEntry (DataSeg* S, const char* Format, va_list ap) attribute ((format(printf,2,0)));
/* Add a line to the given data segment */

void OutputDataSeg (const DataSeg* S, FILE* F);
/* Output the data segment data to a file */



/* End of dataseg.h */
#endif



