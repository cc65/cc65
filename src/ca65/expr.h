/*****************************************************************************/
/*                                                                           */
/*				    expr.h				     */
/*                                                                           */
/*	       Expression evaluation for the ca65 macroassembler	     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2003 Ullrich von Bassewitz                                       */
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



#ifndef EXPR_H
#define EXPR_H



/* common */
#include "exprdefs.h"



/*****************************************************************************/
/*     	      	     		     Code		     		     */
/*****************************************************************************/



ExprNode* Expression (void);
/* Evaluate an expression, build the expression tree on the heap and return
 * a pointer to the root of the tree.
 */

long ConstExpression (void);
/* Parse an expression. Check if the expression is const, and print an error
 * message if not. Return the value of the expression, or a dummy, if it is
 * not constant.
 */

void FreeExpr (ExprNode* Root);
/* Free the expression tree, Root is pointing to. */

ExprNode* GenLiteralExpr (long Val);
/* Return an expression tree that encodes the given literal value */

ExprNode* GenSymExpr (struct SymEntry* Sym);
/* Return an expression node that encodes the given symbol */

ExprNode* GenCurrentPC (void);
/* Return the current program counter as expression */

ExprNode* GenSwapExpr (ExprNode* Expr);
/* Return an extended expression with lo and hi bytes swapped */

ExprNode* GenBranchExpr (unsigned Offs);
/* Return an expression that encodes the difference between current PC plus
 * offset and the target expression (that is, Expression() - (*+Offs) ).
 */

ExprNode* GenULabelExpr (unsigned Num);
/* Return an expression for an unnamed label with the given index */

ExprNode* GenByteExpr (ExprNode* Expr);
/* Force the given expression into a byte and return the result */

ExprNode* GenWordExpr (ExprNode* Expr);
/* Force the given expression into a word and return the result. */

ExprNode* GenNE (ExprNode* Expr, long Val);
/* Generate an expression that compares Expr and Val for inequality */

int IsConstExpr (ExprNode* Expr, long* Val);
/* Return true if the given expression is a constant expression, that is, one
 * with no references to external symbols. If Val is not NULL and the
 * expression is constant, the constant value is stored here.
 */

int IsByteExpr (ExprNode* Root);
/* Return true if this is a byte expression */

int IsByteRange (long Val);
/* Return true if this is a byte value */

int IsWordRange (long Val);
/* Return true if this is a word value */

ExprNode* CloneExpr (ExprNode* Expr);
/* Clone the given expression tree. The function will simply clone symbol
 * nodes, it will not resolve them.
 */

void WriteExpr (ExprNode* Expr);
/* Write the given expression to the object file */



/* End of expr.h */

#endif



