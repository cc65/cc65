/*****************************************************************************/
/*                                                                           */
/*				  exprheap.h				     */
/*                                                                           */
/*			 Expression node heap manager			     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000	 Ullrich von Bassewitz                                       */
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



#ifndef EXPRHEAP_H
#define EXPRHEAP_H



#include "exprnode.h"



/*****************************************************************************/
/*	      	  		     Data				     */
/*****************************************************************************/



/* An expression heap */
typedef struct ExprHeap ExprHeap;



/*****************************************************************************/
/*	      	  		     Code				     */
/*****************************************************************************/



void PushExprHeap (void);
/* Create a new expression heap and push it onto the expression heap stack, so
 * it is the current expression heap.
 */

ExprHeap* PopExprHeap (void);
/* Pop the current expression heap from the heap stack and return it */

ExprNode* AllocExprNode (nodetype_t NT, type* Type, int LValue);
/* Get a new node from the current expression heap */

void FreeExprNode (ExprNode* N);
/* Free an expression node from the current expression heap */

void FreeExprTree (ExprNode* N);
/* Free a complete expression tree starting with the current node */



/* End of exprheap.h */

#endif



