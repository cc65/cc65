/*****************************************************************************/
/*                                                                           */
/*				   asmline.h   	       	       	       	     */
/*                                                                           */
/*		       Internal assembler line structure		     */
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



#ifndef ASMLINE_H
#define ASMLINE_H



#include <stdarg.h>

/* common */
#include "attrib.h"



/*****************************************************************************/
/*	       	 	  	     Data				     */
/*****************************************************************************/



/* Structure that contains one line */
typedef struct Line_ Line;
struct Line_ {
    Line*	 	Next;		/* Next line on double linked list */
    Line* 		Prev;		/* Revious line in list */
    unsigned   	 	Flags;		/* Flags for this line */
    unsigned long	Index;		/* Index of this line */
    unsigned   	 	Size;		/* Size of this code */
    unsigned   	 	Len;   		/* Length of the line */
    char       	 	Line [1];	/* The line itself */
};

/* The line list */
extern Line*	   	FirstLine;     	/* Pointer to first line */
extern Line*   		LastLine;	/* Pointer to last line */



/*****************************************************************************/
/*     	       	      	  	     Code				     */
/*****************************************************************************/



Line* NewCodeLine (const char* Format, va_list ap) attribute((format(printf,1,0)));
/* Create a new code line and return it */

Line* NewCodeLineAfter (Line* LineBefore, const char* Format, va_list ap) attribute((format(printf,2,0)));
/* Create a new line, insert it after L and return it. */

void FreeCodeLine (Line* L);
/* Remove a line from the list and free it */



/* End of asmline.h */
#endif



