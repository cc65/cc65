/*****************************************************************************/
/*                                                                           */
/*                                studyexpr.h                                */
/*                                                                           */
/*                         Study an expression tree                          */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2003      Ullrich von Bassewitz                                       */
/*               Römerstraße 52                                              */
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



#ifndef STUDYEXPR_H
#define STUDYEXPR_H



/* common */
#include "exprdefs.h"



/*****************************************************************************/
/*     	      	    	    	     Data				     */
/*****************************************************************************/



/* Structure for parsing expression trees */
typedef struct ExprDesc ExprDesc;
struct ExprDesc {
    long                Val;		/* The offset value */
    long                Left;           /* Left value for StudyBinaryExpr */
    int	       	        TooComplex;     /* Expression is too complex to evaluate */
    long                SymCount;       /* Symbol reference count */
    long                SecCount;       /* Section reference count */
    struct SymEntry*    SymRef;         /* Symbol reference if any */
    unsigned            SecRef;		/* Section reference if any */
};




/*****************************************************************************/
/*     	      	    	       	     Code				     */
/*****************************************************************************/



ExprDesc* InitExprDesc (ExprDesc* ED);
/* Initialize an ExprDesc structure for use with StudyExpr */

int ExprDescIsConst (const ExprDesc* ED);
/* Return true if the expression is constant */

void StudyExpr (ExprNode* Expr, ExprDesc* D, int Sign);
/* Study an expression tree and place the contents into D */



/* End of studyexpr.h */
#endif



