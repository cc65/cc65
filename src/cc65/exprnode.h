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



/* common */
#include "coll.h"

/* cc65 */
#include "datatype.h"



/*****************************************************************************/
/*				   Forwards			   	     */
/*****************************************************************************/



struct ExprHeap;
struct SymEntry;



/*****************************************************************************/
/*	      	  		     Data				     */
/*****************************************************************************/



/* Node types */
typedef enum {

    /* Bits encoding the type of the objects stored in List for this
     * particular node.
     */
    NT_LIST_NODE    = 0x0000,	/* Items are expression nodes */
    NT_LIST_SYM	    = 0x0100,	/* Items are symbol table entries */
    NT_LIST_STRING  = 0x0200,	/* List item are character strings */

    NT_NONE,   	       	       	/* None (invalid) op */

    NT_SYM,     	       	/* Symbol */
    NT_CONST,  	       	       	/* A constant of some sort */
    NT_ASM,			/* Inline assembler */

    NT_REG_A,  	       	       	/* A register */
    NT_REG_X,  	       	       	/* X register */
    NT_REG_Y,  	       	       	/* Y register */
    NT_REG_AX, 	       	       	/* AX register */
    NT_REG_EAX,	       	       	/* EAX register */

    NT_ARRAY_SUBSCRIPT,		/* Array subscript */
    NT_STRUCT_ACCESS,		/* Access of a struct field */
    NT_STRUCTPTR_ACCESS,       	/* Access via struct ptr */
    NT_FUNCTION_CALL,		/* Call a function */
    NT_TYPECAST,		/* A cast */
    NT_ADDRESS,			/* Address operator (&) */
    NT_INDIRECT,		/* Indirection operator (*) */

    NT_UNARY_MINUS,
    NT_COMPLEMENT,		/* ~ */
    NT_BOOL_NOT,       	       	/* ! */

    NT_PLUS,   	       	       	/* + */
    NT_MINUS,  	       	       	/* - */
    NT_MUL,    	       	       	/* * */
    NT_DIV,    	       	       	/* / */
    NT_SHL,    	       	       	/* << */
    NT_SHR,    	       	       	/* >> */
    NT_AND,    	       	       	/* & */
    NT_OR,     	       	       	/* | */
    NT_XOR,    	       	       	/* ^ */

    NT_TERNARY,			/* ?: */

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

    NT_BOOL_OR,	       	       	/* || */
    NT_BOOL_AND,       	       	/* && */

    NT_EQ,     	       	       	/* == */
    NT_NE,     	       	       	/* != */
    NT_LT,     	       	       	/* < */
    NT_LE,     	       	       	/* <= */
    NT_GT,     	       	       	/* > */
    NT_GE,     	       	       	/* >= */

    NT_COUNT   	       	       	/* Operation count */
} nodetype_t;



/* Struct describing one node in an expression tree */
typedef struct ExprNode ExprNode;
struct ExprNode {

    /* Management data */
    union {
	struct ExprHeap*       	Owner; 	/* Heap, this node is in */
	struct ExprNode*	Next;	/* Next in free list */
    } MData;

    Collection			List;	/* List of subexpressions */
    nodetype_t	    		NT;    	/* Node type */
    type*  	    		Type;	/* Resulting type */
    int	   			LValue;	/* True if this is an lvalue */

    /* Attributes */
    long       	       	       	IVal;	/* Constant int value if any */
    double	    		FVal;  	/* Constant float value if any */
};



/* Predefined indices for node items in List */
enum {
    IDX_LEFT	= 0,
    IDX_RIGHT	= 1,
    IDX_SYM	= 0
};

/* Some other constants for better readability */
enum {
    RVALUE	= 0,
    LVALUE	= 1
};



/*****************************************************************************/
/*	       	     		     Code	      			     */
/*****************************************************************************/



ExprNode* InitExprNode (ExprNode* E, nodetype_t NT, type* Type,
			int LValue, struct ExprHeap* Owner);
/* Initialize a new expression node */

void* GetItem (ExprNode* N, unsigned Index);
/* Return one of the items from the nodes item list */

void AppendItem (ExprNode* N, void* Item);
/* Append an item to the nodes item list */

void SetItem (ExprNode* N, void* Item, unsigned Index);
/* Set a specific node item. The item list is filled with null pointers as
 * needed.
 */

ExprNode* GetLeftNode (ExprNode* N);
/* Get the left sub-node from the list */

void SetLeftNode (ExprNode* Root, ExprNode* Left);
/* Set the left node in Root */

ExprNode* GetRightNode (ExprNode* N);
/* Get the right sub-node from the list */

void SetRightNode (ExprNode* Root, ExprNode* Right);
/* Set the right node in Root */

struct SymEntry* GetNodeSym (ExprNode* N);
/* Get the symbol entry for a NT_SYM node */

void SetNodeSym (ExprNode* N, struct SymEntry* Sym);
/* Set the symbol entry in a NT_SYM node */



/* End of exprnode.h */

#endif





