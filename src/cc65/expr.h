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
/*		     	     	     code  				     */
/*****************************************************************************/



void ExprWithCheck (void (*Func) (ExprDesc*), ExprDesc *Expr);
/* Call an expression function with checks. */

void PushAddr (const ExprDesc* Expr);
/* If the expression contains an address that was somehow evaluated,
 * push this address on the stack. This is a helper function for all
 * sorts of implicit or explicit assignment functions where the lvalue
 * must be saved if it's not constant, before evaluating the rhs.
 */

void Store (ExprDesc* Expr, const type* StoreType);
/* Store the primary register into the location denoted by lval. If StoreType
 * is given, use this type when storing instead of lval->Type. If StoreType
 * is NULL, use lval->Type instead.
 */

void hie0 (ExprDesc* Expr);
/* Parse comma operator. */

int evalexpr (unsigned flags, void (*Func) (ExprDesc*), ExprDesc* Expr);
/* Will evaluate an expression via the given function. If the result is a
 * constant, 0 is returned and the value is put in the lval struct. If the
 * result is not constant, LoadExpr is called to bring the value into the
 * primary register and 1 is returned.
 */

void Expression0 (ExprDesc* Expr);
/* Evaluate an expression via hie0 and put the result into the primary register */

void ConstExpr (void (*Func) (ExprDesc*), ExprDesc* Expr);
/* Will evaluate an expression via the given function. If the result is not
 * a constant of some sort, a diagnostic will be printed, and the value is
 * replaced by a constant one to make sure there are no internal errors that
 * result from this input error.
 */

void BoolExpr (void (*Func) (ExprDesc*), ExprDesc* Expr);
/* Will evaluate an expression via the given function. If the result is not
 * something that may be evaluated in a boolean context, a diagnostic will be
 * printed, and the value is replaced by a constant one to make sure there
 * are no internal errors that result from this input error.
 */

void ConstAbsIntExpr (void (*Func) (ExprDesc*), ExprDesc* Expr);
/* Will evaluate an expression via the given function. If the result is not
 * a constant numeric integer value, a diagnostic will be printed, and the
 * value is replaced by a constant one to make sure there are no internal
 * errors that result from this input error.
 */

void hie10 (ExprDesc* lval);
/* Handle ++, --, !, unary - etc. */

void hie1 (ExprDesc* lval);
/* Parse first level of expression hierarchy. */

void DefineData (ExprDesc* lval);
/* Output a data definition for the given expression */



/* End of expr.h */

#endif




