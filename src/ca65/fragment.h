/*****************************************************************************/
/*                                                                           */
/*				  fragment.h				     */
/*                                                                           */
/*		  Data fragments for the ca65 crossassembler		     */
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
#include "exprdefs.h"
#include "filepos.h"



/*****************************************************************************/
/*	  			struct Fragment				     */
/*****************************************************************************/



typedef struct Fragment_ Fragment;
struct Fragment_ {
    Fragment*		List;		/* List of all fragments */
    Fragment*  	       	Next;		/* Fragment list in one segment */
    Fragment*		LineList;	/* List of fragments for one src line */
    FilePos    	    	Pos;		/* File position for this fragment */
    unsigned short 	Len;		/* Length for this fragment */
    unsigned char   	Type;		/* Fragment type */
    union {
       	unsigned char	Data [4];       /* Literal values */
       	ExprNode*   	Expr;		/* Expression */
    } V;
};



/* List of all fragments */
extern Fragment* FragList;
extern Fragment* FragLast;



/* End of fragment.h */
#endif



	  
