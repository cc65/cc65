/*****************************************************************************/
/*                                                                           */
/*                                exprdesc.h                                 */
/*                                                                           */
/*                      Expression descriptor structure                      */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2002-2004 Ullrich von Bassewitz                                       */
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



#ifndef EXPRDESC_H
#define EXPRDESC_H



#include <string.h>

/* common */
#include "inline.h"

/* cc65 */
#include "datatype.h"



/*****************************************************************************/
/*  				     Data                                    */
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

#define E_RVAL          0x0000U /* Expression node is a value */
#define E_LVAL          0x1000U /* Expression node is a reference */

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
/*		   		     Code                                    */
/*****************************************************************************/



#if defined(HAVE_INLINE)
INLINE ExprDesc* InitExprDesc (ExprDesc* Expr)
/* Initialize an ExprDesc */
{
    return memset (Expr, 0, sizeof (*Expr));
}
#else
#  define InitExprDesc(E)       memset ((E), 0, sizeof (*(E)))
#endif

#if defined(HAVE_INLINE)
INLINE int ED_IsLVal (const ExprDesc* Expr)
/* Return true if the expression is a reference */
{
    return (Expr->Flags & E_LVAL) != 0;
}
#else
#  define ED_IsLVal(Expr)       (((Expr)->Flags & E_LVAL) != 0)
#endif

#if defined(HAVE_INLINE)
INLINE int ED_IsRVal (const ExprDesc* Expr)
/* Return true if the expression is a rvalue */
{
    return (Expr->Flags & E_LVAL) == 0;
}
#else
#  define ED_IsRVal(Expr)       (((Expr)->Flags & E_LVAL) == 0)
#endif

#if defined(HAVE_INLINE)
INLINE int ED_SetValType (ExprDesc* Expr, int Ref)
/* Set the reference flag for an expression and return it (the flag) */
{
    Expr->Flags = Ref? (Expr->Flags | E_LVAL) : (Expr->Flags & ~E_LVAL);
    return Ref;
}
#else
/* Beware: Just one occurance of R below, since it may have side effects! */
#  define ED_SetValType(E, R)                                                   \
        (((E)->Flags = (R)? ((E)->Flags | E_LVAL) : ((E)->Flags & ~E_LVAL)),    \
        ED_IsLVal (E))
#endif

#if defined(HAVE_INLINE)
INLINE int ED_MakeLVal (ExprDesc* Expr)
/* Make the expression a lvalue and return true */
{
    return ED_SetValType (Expr, 1);
}
#else
#  define ED_MakeLVal(Expr)       ED_SetValType (Expr, 1)
#endif

#if defined(HAVE_INLINE)
INLINE int ED_MakeRVal (ExprDesc* Expr)
/* Make the expression a rvalue and return false */
{
    return ED_SetValType (Expr, 0);
}
#else
#  define ED_MakeRVal(Expr)       ED_SetValType (Expr, 0)
#endif

ExprDesc* ED_MakeConstInt (ExprDesc* Expr, long Value);
/* Make Expr a constant integer expression with the given value */

void PrintExprDesc (FILE* F, ExprDesc* Expr);
/* Print an ExprDesc */

type* ReplaceType (ExprDesc* Expr, const type* NewType);
/* Replace the type of Expr by a copy of Newtype and return the old type string */



/* End of exprdesc.h */
#endif



