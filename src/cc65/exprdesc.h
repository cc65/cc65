/*****************************************************************************/
/*                                                                           */
/*                                exprdesc.h                                 */
/*                                                                           */
/*                      Expression descriptor structure                      */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2002      Ullrich von Bassewitz                                       */
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



#ifndef EXPRDESC_H
#define EXPRDESC_H


                     
/* cc65 */
#include "datatype.h"



/*****************************************************************************/
/*				     Data                                    */
/*****************************************************************************/



/* Defines for the flags field of the expression descriptor */
#define E_MREG 	       	0x0110U	/* Special: Expression is primary register */
#define E_MGLOBAL      	0x0080U	/* Reference to static variable */
#define E_MLOCAL    	0x0040U	/* Reference to local variable (stack offset) */
#define E_MCONST    	0x0020U	/* Constant value */
#define E_MEXPR	    	0x0010U	/* Result is in primary register */
#define E_MEOFFS       	0x0011U	/* Base is in primary register, const offset */

#define E_MCTYPE       	0x0007U /* Type of a constant */
#define E_TCONST       	0x0000U /* Constant */
#define E_TGLAB	       	0x0001U /* Global label */
#define E_TLIT 	       	0x0002U /* Literal of some kind */
#define E_TLOFFS       	0x0003U /* Constant stack offset */
#define E_TLLAB	       	0x0004U /* Local label */
#define E_TREGISTER    	0x0005U	/* Register variable */

/* Defines for the test field of the expression descriptor */
#define E_CC   	       	0x0001U	/* expr has set cond codes apropos result value */
#define E_FORCETEST    	0x0002U /* if expr has NOT set CC, force a test */

/* Describe the result of an expression */
typedef struct ExprDesc ExprDesc;
struct ExprDesc {
    struct SymEntry*	Sym;	 /* Symbol table entry if known */
    type*	       	Type;    /* Type array of expression */
    long       	       	ConstVal;/* Value if expression constant */
    unsigned short     	Flags;
    unsigned short  	Test;	 /* */
    unsigned long 	Name;	 /* Name or label number */
};



/*****************************************************************************/
/*				     Code                                    */
/*****************************************************************************/



void MakeConstIntExpr (ExprDesc* Expr, long Value);
/* Make Expr a constant integer expression with the given value */

void PrintExprDesc (FILE* F, ExprDesc* Expr);
/* Print an ExprDesc */



/* End of exprdesc.h */
#endif



