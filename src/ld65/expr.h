/*****************************************************************************/
/*                                                                           */
/*				    expr.h				     */
/*                                                                           */
/*		   Expression evaluation for the ld65 linker		     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2000 Ullrich von Bassewitz                                       */
/*               Wacholderweg 14                                             */
/*               D-70597 Stuttgart                                           */
/* EMail:        uz@musoftware.de                                            */
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



#include "../common/exprdefs.h"

#include "objdata.h"
#include "exports.h"
#include "config.h"



/*****************************************************************************/
/*     	      	     		     Code		     		     */
/*****************************************************************************/



void FreeExpr (ExprNode* Root);
/* Free the expression tree, Root is pointing to. */

int IsConstExpr (ExprNode* Root);
/* Return true if the given expression is a constant expression, that is, one
 * with no references to external symbols.
 */

Import* GetExprImport (ExprNode* Expr);
/* Get the import data structure for a symbol expression node */

Export* GetExprExport (ExprNode* Expr);
/* Get the exported symbol for a symbol expression node */

Section* GetExprSection (ExprNode* Expr);
/* Get the segment for a segment expression node */

long GetExprVal (ExprNode* Expr);
/* Get the value of a constant expression */

ExprNode* LiteralExpr (long Val, ObjData* O);
/* Return an expression tree that encodes the given literal value */

ExprNode* MemExpr (Memory* Mem, long Offs, ObjData* O);
/* Return an expression tree that encodes an offset into the memory area */

void DumpExpr (const ExprNode* Expr);
/* Dump an expression tree to stdout */

ExprNode* ReadExpr (FILE* F, ObjData* O);
/* Read an expression from the given file */

int EqualExpr (ExprNode* E1, ExprNode* E2);
/* Check if two expressions are identical. */



/* End of expr.h */

#endif



