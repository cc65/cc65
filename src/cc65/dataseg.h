/*****************************************************************************/
/*                                                                           */
/*				   dataseg.h				     */
/*                                                                           */
/*			    Data segment structure			     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2001     Ullrich von Bassewitz                                        */
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



#ifndef DATASEG_H
#define DATASEG_H



#include <stdio.h>

/* common */
#include "attrib.h"
#include "coll.h"



/*****************************************************************************/
/*  	       	 	  	     Data				     */
/*****************************************************************************/



typedef struct DataSeg DataSeg;
struct DataSeg {
    DataSeg*		Next;	       	/* Pointer to next DataSeg */
    char*		Name;		/* Segment name */
    Collection 	       	Lines;	       	/* List of code lines */
};

/* Pointer to current data segment */
extern DataSeg* DS;



/*****************************************************************************/
/*     	       	       	  	     Code				     */
/*****************************************************************************/



DataSeg* NewDataSeg (const char* Name);
/* Create a new data segment, initialize and return it */

void FreeDataSeg (DataSeg* S);
/* Free a data segment including all line entries */

void PushDataSeg (DataSeg* S);
/* Push the given data segment onto the stack */

DataSeg* PopDataSeg (void);
/* Remove the current data segment from the stack and return it */

void AppendDataSeg (DataSeg* Target, const DataSeg* Source);
/* Append the data from Source to Target. */

void AddDataSegLine (DataSeg* S, const char* Format, ...) attribute ((format(printf,2,3)));
/* Add a line to the given data segment */

void OutputDataSeg (FILE* F, const DataSeg* S);
/* Output the data segment data to a file */



/* End of dataseg.h */
#endif



