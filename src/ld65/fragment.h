/*****************************************************************************/
/*                                                                           */
/*				  fragment.h				     */
/*                                                                           */
/*			  Code/data fragment routines			     */
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



#ifndef FRAGMENT_H
#define FRAGMENT_H



/* common */
#include "filepos.h"



/*****************************************************************************/
/*     	       	     		     Data  				     */
/*****************************************************************************/



/* Fragment structure */
typedef struct Fragment Fragment;
struct Fragment {
    Fragment*	   	Next;  		/* Next fragment in list */
    struct ObjData*	Obj;		/* Source of fragment */
    unsigned long      	Size;  		/* Size of data/expression */
    struct ExprNode*	Expr;		/* Expression if FRAG_EXPR */
    FilePos  		Pos;		/* File position in source */
    unsigned char    	Type;  		/* Type of fragment */
    unsigned char      	LitBuf [1]; 	/* Dynamically alloc'ed literal buffer */
};



/*****************************************************************************/
/*     	      	     	   	     Code  	      	  	  	     */
/*****************************************************************************/



Fragment* NewFragment (unsigned char Type, unsigned long Size, struct Section* S);
/* Create a new fragment and insert it into the section S */



/* End of fragment.h */

#endif




