/*****************************************************************************/
/*                                                                           */
/*     	       	       	       	  exprdefs.c				     */
/*                                                                           */
/*			  Expression tree definitions			     */
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



#include <stdio.h>

#include "abend.h"
#include "exprdefs.h"



/*****************************************************************************/
/*     	      	    		     Code	   		       	     */
/*****************************************************************************/



static void InternalDumpExpr (const ExprNode* Expr)
/* Dump an expression in RPN to stdout */
{
    if (Expr == 0) {
	return;
    }
    InternalDumpExpr (Expr->Left);
    InternalDumpExpr (Expr->Right);

    switch (Expr->Op) {

	case EXPR_LITERAL:
	case EXPR_ULABEL:
	    printf (" $%04lX", Expr->V.Val & 0xFFFF);
	    break;

	case EXPR_SYMBOL:
       	    printf (" SYM");
	    break;

	case EXPR_SECTION:
	    printf (" SEC");
	    break;

	case EXPR_SEGMENT:
	    printf (" SEG");
	    break;

       	case EXPR_PLUS:
	    printf (" +");
     	    break;

       	case EXPR_MINUS:
	    printf (" -");
     	    break;

       	case EXPR_MUL:
     	    printf (" *");
     	    break;

       	case EXPR_DIV:
     	    printf (" /");
     	    break;

       	case EXPR_MOD:
     	    printf (" MOD");
     	    break;

	case EXPR_OR:
     	    printf (" OR");
     	    break;

	case EXPR_XOR:
     	    printf (" XOR");
     	    break;

	case EXPR_AND:
     	    printf (" AND");
     	    break;

	case EXPR_SHL:
     	    printf (" SHL");
     	    break;

	case EXPR_SHR:
     	    printf (" SHR");
     	    break;

       	case EXPR_EQ:
     	    printf (" =");
     	    break;

       	case EXPR_NE:
     	    printf ("<>");
     	    break;

       	case EXPR_LT:
     	    printf (" <");
     	    break;

       	case EXPR_GT:
     	    printf (" >");
     	    break;

       	case EXPR_UNARY_MINUS:
	    printf (" NEG");
	    break;

       	case EXPR_NOT:
	    printf (" ~");
	    break;

       	case EXPR_BYTE0:
	    printf (" BYTE0");
	    break;

       	case EXPR_BYTE1:
	    printf (" BYTE1");
	    break;

       	case EXPR_BYTE2:
	    printf (" BYTE2");
	    break;

       	case EXPR_BYTE3:
	    printf (" BYTE3");
	    break;

       	case EXPR_SWAP:
	    printf (" SWAP");
	    break;

	case EXPR_BAND:
	    printf (" BOOL_AND");
	    break;       

	case EXPR_BOR:
	    printf (" BOOL_OR");
	    break;

	case EXPR_BXOR:
	    printf (" BOOL_XOR");
	    break;

	case EXPR_BNOT:
    	    printf (" BOOL_NOT");
	    break;

        default:
       	    AbEnd ("Unknown Op type: %u", Expr->Op);

    }
}



void DumpExpr (const ExprNode* Expr)
/* Dump an expression tree to stdout */
{
    InternalDumpExpr (Expr);
    printf ("\n");
}



