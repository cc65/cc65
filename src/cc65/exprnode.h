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
/*	      	  	      	     Data				     */
/*****************************************************************************/



/* Node types */
typedef enum {

    /* Bits encoding the type of the objects stored in List for this
     * particular node.
     */
    NT_LIST_NONE	= 0x0000,	/* No items */
    NT_LIST_EXPR       	= 0x0100,  	/* Items are expression nodes */
    NT_LIST_SYM		= 0x0200, 	/* Items are symbol table entries */
    NT_LIST_STRING	= 0x0300, 	/* List item are character strings */
    NT_MASK_LIST      	= 0x0300,

    /* Two bits telling if this is a leaf or a branch */
    NT_LEAF    	       	= 0x0000, 	/* Leaf */
    NT_BRANCH         	= 0x8000,      	/* Branch */
    NT_MASK_LEAF	= 0x8000,

    /* Special node type */
    NT_NONE	      	= 0x0000, 	/* None (invalid) op */

    /* Leaves */
    NT_SYM     	       	= 0x0001 | NT_LEAF   | NT_LIST_SYM,	/* Symbol */
    NT_CONST   	       	= 0x0002 | NT_LEAF   | NT_LIST_NONE,  	/* A constant of some sort */
    NT_ASM     	       	= 0x0003 | NT_LEAF   | NT_LIST_STRING,	/* Inline assembler */

    NT_REG_A   	       	= 0x0005 | NT_LEAF   | NT_LIST_NONE,	/* A register */
    NT_REG_X	      	= 0x0006 | NT_LEAF   | NT_LIST_NONE,	/* X register */
    NT_REG_Y	      	= 0x0007 | NT_LEAF   | NT_LIST_NONE,	/* Y register */
    NT_REG_AX	      	= 0x0008 | NT_LEAF   | NT_LIST_NONE,	/* AX register */
    NT_REG_EAX	      	= 0x0009 | NT_LEAF   | NT_LIST_NONE,	/* EAX register */

    /* Branches */
    NT_ARRAY_SUBSCRIPT 	= 0x0010 | NT_BRANCH | NT_LIST_EXPR,   	/* Array subscript */
    NT_STRUCT_ACCESS   	= 0x0011 | NT_BRANCH | NT_LIST_EXPR,	/* Access of a struct field */
    NT_STRUCTPTR_ACCESS	= 0x0012 | NT_BRANCH | NT_LIST_EXPR,	/* Access via struct ptr */
    NT_FUNCTION_CALL  	= 0x0013 | NT_BRANCH | NT_LIST_EXPR,	/* Call a function */
    NT_TYPECAST	      	= 0x0014 | NT_BRANCH | NT_LIST_EXPR,	/* A cast */
    NT_ADDRESS	      	= 0x0015 | NT_BRANCH | NT_LIST_EXPR,	/* Address operator (&) */
    NT_INDIRECT	      	= 0x0016 | NT_BRANCH | NT_LIST_EXPR,	/* Indirection operator (*) */

    NT_UNARY_MINUS	= 0x0018 | NT_BRANCH | NT_LIST_EXPR,
    NT_COMPLEMENT	= 0x0019 | NT_BRANCH | NT_LIST_EXPR,	/* ~ */
    NT_BOOL_NOT		= 0x001A | NT_BRANCH | NT_LIST_EXPR,	/* ! */

    NT_PLUS		= 0x001B | NT_BRANCH | NT_LIST_EXPR,	/* + */
    NT_MINUS		= 0x001C | NT_BRANCH | NT_LIST_EXPR,	/* - */
    NT_MUL     	       	= 0x001D | NT_BRANCH | NT_LIST_EXPR,	/* * */
    NT_DIV		= 0x001E | NT_BRANCH | NT_LIST_EXPR,	/* / */
    NT_SHL		= 0x001F | NT_BRANCH | NT_LIST_EXPR,	/* << */
    NT_SHR		= 0x0020 | NT_BRANCH | NT_LIST_EXPR,	/* >> */
    NT_AND		= 0x0021 | NT_BRANCH | NT_LIST_EXPR,	/* & */
    NT_OR		= 0x0022 | NT_BRANCH | NT_LIST_EXPR,	/* | */
    NT_XOR		= 0x0023 | NT_BRANCH | NT_LIST_EXPR,	/* ^ */

    NT_TERNARY	      	= 0x0024 | NT_BRANCH | NT_LIST_EXPR,	/* ?: */

    NT_ASSIGN		= 0x0025 | NT_BRANCH | NT_LIST_EXPR,	/* = */
    NT_PLUS_ASSIGN     	= 0x0026 | NT_BRANCH | NT_LIST_EXPR,	/* += */
    NT_MINUS_ASSIGN	= 0x0027 | NT_BRANCH | NT_LIST_EXPR,	/* -= */
    NT_MUL_ASSIGN	= 0x0028 | NT_BRANCH | NT_LIST_EXPR,	/* *= */
    NT_DIV_ASSIGN	= 0x0029 | NT_BRANCH | NT_LIST_EXPR,	/* /= */
    NT_SHL_ASSIGN	= 0x002A | NT_BRANCH | NT_LIST_EXPR,	/* <<= */
    NT_SHR_ASSIGN	= 0x002B | NT_BRANCH | NT_LIST_EXPR,	/* >>= */
    NT_AND_ASSIGN	= 0x002C | NT_BRANCH | NT_LIST_EXPR,	/* &= */
    NT_OR_ASSIGN	= 0x002D | NT_BRANCH | NT_LIST_EXPR,	/* |= */
    NT_XOR_ASSIGN	= 0x002E | NT_BRANCH | NT_LIST_EXPR,	/* ^= */

    NT_PRE_DEC 	       	= 0x002F | NT_BRANCH | NT_LIST_EXPR,	/* -- */
    NT_POST_DEC		= 0x0030 | NT_BRANCH | NT_LIST_EXPR,	/* -- */
    NT_PRE_INC		= 0x0031 | NT_BRANCH | NT_LIST_EXPR,	/* ++ */
    NT_POST_INC		= 0x0032 | NT_BRANCH | NT_LIST_EXPR,	/* ++ */

    NT_BOOL_OR		= 0x0033 | NT_BRANCH | NT_LIST_EXPR,	/* || */
    NT_BOOL_AND		= 0x0034 | NT_BRANCH | NT_LIST_EXPR,	/* && */

    NT_EQ		= 0x0035 | NT_BRANCH | NT_LIST_EXPR,	/* == */
    NT_NE		= 0x0036 | NT_BRANCH | NT_LIST_EXPR,	/* != */
    NT_LT		= 0x0037 | NT_BRANCH | NT_LIST_EXPR,	/* < */
    NT_LE		= 0x0038 | NT_BRANCH | NT_LIST_EXPR,	/* <= */
    NT_GT		= 0x0039 | NT_BRANCH | NT_LIST_EXPR,	/* > */
    NT_GE      	       	= 0x003A | NT_BRANCH | NT_LIST_EXPR,   	/* >= */

    NT_MASK_TYPE	= 0x00FF

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

int IsLeafNode (const ExprNode* E);
/* Return true if this is a leaf node */

int IsBranchNode (const ExprNode* E);
/* Return true if this is a branch node */



/* End of exprnode.h */

#endif



