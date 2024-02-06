/*****************************************************************************/
/*                                                                           */
/*                                  expr.h                                   */
/*                                                                           */
/*                 Expression evaluation for the ld65 linker                 */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2011, Ullrich von Bassewitz                                      */
/*                Roemerstrasse 52                                           */
/*                D-70794 Filderstadt                                        */
/* EMail:         uz@cc65.org                                                */
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

/* ld65 */
#include "objdata.h"
#include "exports.h"
#include "config.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Structure for parsing segment based expression trees */
typedef struct SegExprDesc SegExprDesc;
struct SegExprDesc {
    long            Val;                /* The offset value */
    int             TooComplex;         /* Expression too complex */
    Segment*        Seg;                /* Segment reference if any */
};



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



ExprNode* NewExprNode (ObjData* O, unsigned char Op);
/* Create a new expression node */

void FreeExpr (ExprNode* Root);
/* Free the expression tree, Root is pointing to. */

int IsConstExpr (ExprNode* Root);
/* Return true if the given expression is a constant expression, that is, one
** with no references to external symbols.
*/

Import* GetExprImport (ExprNode* Expr);
/* Get the import data structure for a symbol expression node */

Export* GetExprExport (ExprNode* Expr);
/* Get the exported symbol for a symbol expression node */

Section* GetExprSection (ExprNode* Expr);
/* Get the segment for a section expression node */

long GetExprVal (ExprNode* Expr);
/* Get the value of a constant expression */

void GetSegExprVal (ExprNode* Expr, SegExprDesc* D);
/* Check if the given expression consists of a segment reference and only
** constant values, additions and subtractions. If anything else is found,
** set D->TooComplex to true. The function will initialize D.
*/

ExprNode* LiteralExpr (long Val, ObjData* O);
/* Return an expression tree that encodes the given literal value */

ExprNode* MemoryExpr (MemoryArea* Mem, long Offs, ObjData* O);
/* Return an expression tree that encodes an offset into the memory area */

ExprNode* SegmentExpr (Segment* Seg, long Offs, ObjData* O);
/* Return an expression tree that encodes an offset into a segment */

ExprNode* SectionExpr (Section* Sec, long Offs, ObjData* O);
/* Return an expression tree that encodes an offset into a section */

ExprNode* ReadExpr (FILE* F, ObjData* O);
/* Read an expression from the given file */

int EqualExpr (ExprNode* E1, ExprNode* E2);
/* Check if two expressions are identical. */



/* End of expr.h */

#endif
