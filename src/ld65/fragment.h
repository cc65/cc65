/*****************************************************************************/
/*                                                                           */
/*				  fragment.h				     */
/*                                                                           */
/*			  Code/data fragment routines			     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2003 Ullrich von Bassewitz                                       */
/*               Römerstrasse 52                                             */
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



#ifndef FRAGMENT_H
#define FRAGMENT_H



/* common */
#include "filepos.h"



/*****************************************************************************/
/*				   Forwards                                  */
/*****************************************************************************/



struct LineInfo;
struct Section;



/*****************************************************************************/
/*     	       	     		     Data  				     */
/*****************************************************************************/



/* Fragment check expression */
typedef struct CheckExpr CheckExpr;
struct CheckExpr {
    struct CheckExpr*   Next;           /* Next check expression */
    struct ExprNode*    Expr;           /* The expression itself */
    unsigned            Action;         /* Action to take if the check fails */
    unsigned            Message;        /* Message number */
};

/* Fragment structure */
typedef struct Fragment Fragment;
struct Fragment {
    Fragment*	   	Next;  		/* Next fragment in list */
    struct ObjData*	Obj;		/* Source of fragment */
    unsigned            Size;  		/* Size of data/expression */
    struct ExprNode*	Expr;		/* Expression if FRAG_EXPR */
    FilePos  		Pos;		/* File position in source */
    struct LineInfo*    LI;             /* Additional line info */
    CheckExpr*          Check;          /* Single linked list of expressions */
    unsigned char    	Type;  		/* Type of fragment */
    unsigned char      	LitBuf [1]; 	/* Dynamically alloc'ed literal buffer */
};



/*****************************************************************************/
/*     	      	     	   	     Code  	      	  	  	     */
/*****************************************************************************/



Fragment* NewFragment (unsigned char Type, unsigned Size, struct Section* S);
/* Create a new fragment and insert it into the section S */



/* End of fragment.h */

#endif




