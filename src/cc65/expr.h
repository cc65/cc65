/*
 * expr.h
 *
 * Ullrich von Bassewitz, 21.06.1998
 */



#ifndef EXPR_H
#define EXPR_H



#include "datatype.h"



/*****************************************************************************/
/*				     data  		   		     */
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
/*			   	     code  				     */
/*****************************************************************************/



void ConstSubExpr (int (*F) (ExprDesc*), ExprDesc* Expr);
/* Will evaluate an expression via the given function. If the result is not
 * a constant, a diagnostic will be printed, and the value is replaced by
 * a constant one to make sure there are no internal errors that result
 * from this input error.
 */

unsigned assignadjust (type* lhst, ExprDesc* rhs);
/* Adjust the type of the right hand expression so that it can be assigned to
 * the type on the left hand side. This function is used for assignment and
 * for converting parameters in a function call. It returns the code generator
 * flags for the operation.
 */

void exprhs (unsigned flags, int k, ExprDesc *lval);
/* Put the result of an expression into the primary register */

void expression1 (ExprDesc* lval);
/* Evaluate an expression on level 1 (no comma operator) and put it into
 * the primary register
 */

void expression (ExprDesc* lval);
/* Evaluate an expression and put it into the primary register */

int evalexpr (unsigned flags, int (*f) (ExprDesc*), ExprDesc* lval);
/* Will evaluate an expression via the given function. If the result is a
 * constant, 0 is returned and the value is put in the lval struct. If the
 * result is not constant, exprhs is called to bring the value into the
 * primary register and 1 is returned.
 */

void ConstExpr (ExprDesc* lval);
/* Get a constant value */

void ConstIntExpr (ExprDesc* Val);
/* Get a constant int value */

void intexpr (ExprDesc* lval);
/* Get an integer expression */

void boolexpr (ExprDesc* lval);
/* Get a boolean expression */

void test (unsigned label, int cond);
/* Generate code to perform test and jump if false. */

int hie1 (ExprDesc* lval);
/* Parse first level of expression hierarchy. */

int hie0 (ExprDesc* lval);
/* Parse comma operator (highest level of expression hierarchy) */

void DefineData (ExprDesc* lval);
/* Output a data definition for the given expression */



/* End of expr.h */

#endif




