/*****************************************************************************/
/*                                                                           */
/*				  exprnode.h				     */
/*                                                                           */
/*	       Expression node structure for the cc65 C compiler	     */
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



#ifndef EXPRNODE_H
#define EXPRNODE_H



#include "datatype.h"



/*****************************************************************************/
/*				   Forwards				     */
/*****************************************************************************/



struct SymEntry;



/*****************************************************************************/
/*	      	  		     Data				     */
/*****************************************************************************/



/* Node types */
typedef enum {
    NT_NONE,   	       	       	/* None (invalid) op */

    NT_SYM,     	       	/* Symbol */
    NT_CONST,  	       	       	/* A constant of some sort */

    NT_ICAST,  	       	       	/* Implicit type cast */
    NT_ECAST,  	       	       	/* Explicit type cast */

    NT_REG_A,  	       	       	/* A register */
    NT_REG_X,  	       	       	/* X register */
    NT_REG_Y,  	       	       	/* Y register */
    NT_REG_AX, 	       	       	/* AX register */
    NT_REG_EAX,	       	       	/* EAX register */

    NT_CALLFUNC,       	       	/* Function call */
    NT_PUSH,   	       	       	/* Push the value onto the stack */
    NT_POP,    	       	       	/* Pop the value from the stack */

    NT_NOT,    	       	       	/* ~ */
    NT_PLUS,   	       	       	/* + */
    NT_MINUS,  	       	       	/* - */
    NT_MUL,    	       	       	/* * */
    NT_DIV,    	       	       	/* / */
    NT_SHL,    	       	       	/* << */
    NT_SHR,    	       	       	/* >> */
    NT_AND,    	       	       	/* & */
    NT_OR,     	       	       	/* | */
    NT_XOR,    	       	       	/* ^ */

    NT_ASSIGN, 	       	       	/* = */
    NT_PLUS_ASSIGN,    	       	/* += */
    NT_MINUS_ASSIGN,   	       	/* -= */
    NT_MUL_ASSIGN,     	       	/* *= */
    NT_DIV_ASSIGN,     	       	/* /= */
    NT_SHL_ASSIGN,     	       	/* <<= */
    NT_SHR_ASSIGN,     	       	/* >>= */
    NT_AND_ASSIGN,     	       	/* &= */
    NT_OR_ASSIGN,      	       	/* |= */
    NT_XOR_ASSIGN,     	       	/* ^= */

    NT_PRE_DEC,	       	       	/* -- */
    NT_POST_DEC,       	       	/* -- */
    NT_PRE_INC,	       	       	/* ++ */
    NT_POST_INC,       	       	/* ++ */

    NT_BOOL_NOT,       	       	/* ! */
    NT_BOOL_OR,	       	       	/* || */
    NT_BOOL_AND,       	       	/* && */

    NT_EQ,     	       	       	/* == */
    NT_NE,     	       	       	/* != */
    NT_LT,     	       	       	/* < */
    NT_LE,     	       	       	/* <= */
    NT_GT,     	       	       	/* > */
    NT_GE,     	       	       	/* >= */

    NT_DEREF,  	       	       	/* * and others */

    NT_COUNT   	       	       	/* Operation count */
} nodetype_t;



/* Struct describing one node in an expression tree */
typedef struct ExprNode ExprNode;
struct ExprNode {

    ExprNode*	    		Left; 	/* Left and right leaves */
    ExprNode*	    		Right;

    nodetype_t	    		NT;    	/* Node type */
    type* 	    		Type;	/* Resulting type */
    int				LValue;	/* True if this is an lvalue */

    union {
	/* Branch data */
	ExprNode*		Test;	/* Third expr for ternary op */

	/* Leave data */
       	long	    		I;	/* Constant int value if any */
       	double	    		F;	/* Constant float value if any */
       	struct SymEntry*	Sym;	/* Symbol table entry if any */
    } V;

};



/*****************************************************************************/
/*	      	  		     Code	     			     */
/*****************************************************************************/



void InitExprNode (ExprNode* E);
/* Initialize a new expression node */



/* End of exprnode.h */

#endif





