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
#define E_MREG		0x0110	/* Special: Expression is primary register */
#define E_MGLOBAL      	0x0080	/* Reference to static variable */
#define E_MLOCAL    	0x0040	/* Reference to local variable (stack offset) */
#define E_MCONST    	0x0020	/* Constant value */
#define E_MEXPR	    	0x0010	/* Result is in primary register */
#define E_MEOFFS       	0x0011 	/* Base is in primary register, const offset */

#define E_MCTYPE    	0x0007	/* Type of a constant */
#define E_TCONST    	0x0000  /* Constant */
#define E_TGLAB	    	0x0001  /* Global label */
#define E_TLIT	    	0x0002  /* Literal of some kind */
#define E_TLOFFS    	0x0003  /* Constant stack offset */
#define E_TLLAB		0x0004  /* Local label */
#define E_TREGISTER	0x0005 	/* Register variable */

/* Defines for the test field of the expression descriptor */
#define E_CC  		0x0001 	/* expr has set cond codes apropos result value */
#define E_FORCETEST    	0x0002  /* if expr has NOT set CC, force a test */

/* Describe the result of an expression */
typedef struct ExprDesc ExprDesc;
struct ExprDesc {
    struct SymEntry*	Sym;	 /* Symbol table entry if known */
    type*		e_tptr;  /* Type array of expression */
    long		e_const; /* Value if expression constant */
    unsigned		e_flags;
    unsigned		e_test;	 /* */
    unsigned long	e_name;	 /* Name or label number */
};



/*****************************************************************************/
/*			   	     code  				     */
/*****************************************************************************/



void doasm (void);
/* This function parses ASM statements. The syntax of the ASM directive
 * looks like the one defined for C++ (C has no ASM directive), that is,
 * a string literal in parenthesis.
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

void constexpr (ExprDesc* lval);
/* Get a constant value */

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




