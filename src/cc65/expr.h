/*
 * expr.h
 *
 * Ullrich von Bassewitz, 21.06.1998
 */



#ifndef EXPR_H
#define EXPR_H



/* cc65 */
#include "datatype.h"
#include "exprdesc.h"



/*****************************************************************************/
/*		     	   	     code  				     */
/*****************************************************************************/



void PushAddr (ExprDesc* lval);
/* If the expression contains an address that was somehow evaluated,
 * push this address on the stack. This is a helper function for all
 * sorts of implicit or explicit assignment functions where the lvalue
 * must be saved if it's not constant, before evaluating the rhs.
 */

void ConstSubExpr (int (*F) (ExprDesc*), ExprDesc* Expr);
/* Will evaluate an expression via the given function. If the result is not
 * a constant, a diagnostic will be printed, and the value is replaced by
 * a constant one to make sure there are no internal errors that result
 * from this input error.
 */

void CheckBoolExpr (ExprDesc* lval);
/* Check if the given expression is a boolean expression, output a diagnostic
 * if not.
 */

void ExprLoad (unsigned flags, int k, ExprDesc *lval);
/* Put the result of an expression into the primary register */

void Store (ExprDesc* lval, const type* StoreType);
/* Store the primary register into the location denoted by lval. If StoreType
 * is given, use this type when storing instead of lval->Type. If StoreType
 * is NULL, use lval->Type instead.
 */

int hie0 (ExprDesc *lval);
/* Parse comma operator. */

int evalexpr (unsigned flags, int (*f) (ExprDesc*), ExprDesc* lval);
/* Will evaluate an expression via the given function. If the result is a
 * constant, 0 is returned and the value is put in the lval struct. If the
 * result is not constant, ExprLoad is called to bring the value into the
 * primary register and 1 is returned.
 */

void expression1 (ExprDesc* lval);
/* Evaluate an expression on level 1 (no comma operator) and put it into
 * the primary register
 */

void expression (ExprDesc* lval);
/* Evaluate an expression and put it into the primary register */

void ConstExpr (ExprDesc* lval);
/* Get a constant value */

void ConstIntExpr (ExprDesc* Val);
/* Get a constant int value */

void intexpr (ExprDesc* lval);
/* Get an integer expression */

int hie10 (ExprDesc* lval);
/* Handle ++, --, !, unary - etc. */

int hie1 (ExprDesc* lval);
/* Parse first level of expression hierarchy. */

void DefineData (ExprDesc* lval);
/* Output a data definition for the given expression */

void Test (unsigned Label, int Invert);
/* Evaluate a boolean test expression and jump depending on the result of
 * the test and on Invert.
 */

void TestInParens (unsigned Label, int Invert);
/* Evaluate a boolean test expression in parenthesis and jump depending on
 * the result of the test * and on Invert.
 */



/* End of expr.h */

#endif




