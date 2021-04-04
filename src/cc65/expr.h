/*
** expr.h
**
** Ullrich von Bassewitz, 21.06.1998
*/



#ifndef EXPR_H
#define EXPR_H



/* cc65 */
#include "datatype.h"
#include "exprdesc.h"



/*****************************************************************************/
/*                                   data                                    */
/*****************************************************************************/



#define SQP_KEEP_NONE   0x00
#define SQP_KEEP_TEST   0x01U
#define SQP_KEEP_EAX    0x02U
#define SQP_KEEP_EXPR   0x03U   /* SQP_KEEP_TEST | SQP_KEEP_EAX */



/*****************************************************************************/
/*                                   code                                    */
/*****************************************************************************/



void ExprWithCheck (void (*Func) (ExprDesc*), ExprDesc* Expr);
/* Call an expression function with checks. */

void MarkedExprWithCheck (void (*Func) (ExprDesc*), ExprDesc* Expr);
/* Call an expression function with checks and record start and end of the
** generated code.
*/

void PushAddr (const ExprDesc* Expr);
/* If the expression contains an address that was somehow evaluated,
** push this address on the stack. This is a helper function for all
** sorts of implicit or explicit assignment functions where the lvalue
** must be saved if it's not constant, before evaluating the rhs.
*/

void InitDeferredOps (void);
/* Init the collection for storing deferred ops */

void DoneDeferredOps (void);
/* Deinit the collection for storing deferred ops */

int GetDeferredOpCount (void);
/* Return how many deferred operations are still waiting in the queque */

void CheckDeferredOpAllDone (void);
/* Check if all deferred operations are done at sequence points.
** Die off if check fails.
*/

void DoDeferred (unsigned Flags, ExprDesc* Expr);
/* Do deferred operations such as post-inc/dec at sequence points */

void Store (ExprDesc* Expr, const Type* StoreType);
/* Store the primary register into the location denoted by lval. If StoreType
** is given, use this type when storing instead of lval->Type. If StoreType
** is NULL, use lval->Type instead.
*/

void Expression0 (ExprDesc* Expr);
/* Evaluate an expression via hie0 and put the result into the primary register.
** The expression is completely evaluated and all side effects complete.
*/

void BoolExpr (void (*Func) (ExprDesc*), ExprDesc* Expr);
/* Will evaluate an expression via the given function. If the result is not
** something that may be evaluated in a boolean context, a diagnostic will be
** printed, and the value is replaced by a constant one to make sure there
** are no internal errors that result from this input error.
*/

ExprDesc NoCodeConstExpr (void (*Func) (ExprDesc*));
/* Get an expression evaluated via the given function. If the result is not a
** constant expression without runtime code generated, a diagnostic will be
** printed, and the value is replaced by a constant one to make sure there are
** no internal errors that result from this input error.
*/

ExprDesc NoCodeConstAbsIntExpr (void (*Func) (ExprDesc*));
/* Get an expression evaluated via the given function. If the result is not a
** constant numeric integer value without runtime code generated, a diagnostic
** will be printed, and the value is replaced by a constant one to make sure
** there are no internal errors that result from this input error.
*/

void hie10 (ExprDesc* lval);
/* Handle ++, --, !, unary - etc. */

void hie8 (ExprDesc* Expr);
/* Process + and - binary operators. */

void hie1 (ExprDesc* lval);
/* Parse first level of expression hierarchy. */

void hie0 (ExprDesc* Expr);
/* Parse comma operator. */



/* End of expr.h */

#endif
