/*****************************************************************************/
/*                                                                           */
/*				  exprnode.c				     */
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



/* common */
#include "check.h"

/* cc65 */
#include "error.h"
#include "exprnode.h"



/*****************************************************************************/
/*	      	  		     Code				     */
/*****************************************************************************/



ExprNode* InitExprNode (ExprNode* E, nodetype_t NT, type* Type,
			int LValue, struct ExprHeap* Owner)
/* Initialize a new expression node */
{
    /* Intialize basic data */
    E->MData.Owner = Owner;
    E->NT 	   = NT;
    E->Type	   = Type;
    E->LValue	   = LValue;
    E->IVal	   = 0;
    E->FVal	   = 0.0;

    /* Initialize the expression list in the node */
    InitCollection (&E->List);

    /* Return the node just initialized */
    return E;
}



void* GetItem (ExprNode* N, unsigned Index)
/* Return one of the items from the nodes item list */
{
    return CollAt (&N->List, Index);
}



void AppendItem (ExprNode* N, void* Item)
/* Append an item to the nodes item list */
{
    CollAppend (&N->List, Item);
}



void SetItem (ExprNode* N, void* Item, unsigned Index)
/* Set a specific node item. The item list is filled with null pointers as
 * needed.
 */
{
    if (Index >= CollCount (&N->List)) {
	/* Fill up with NULL pointers */
       	while (Index >= CollCount (&N->List) < Index) {
	    CollAppend (&N->List, 0);
	}
	/* Append the new item */
	CollAppend (&N->List, Item);
    } else {
	/* There is an item with this index, replace it */
	CollReplace (&N->List, Item, Index);
    }
}



ExprNode* GetNode (ExprNode* N, unsigned Index)
/* Get one of the sub-nodes from the list */
{
    return GetNode (N, Index);
}



ExprNode* GetLeftNode (ExprNode* N)
/* Get the left sub-node from the list */
{
    return GetNode (N, IDX_LEFT);
}



void SetLeftNode (ExprNode* Root, ExprNode* Left)
/* Set the left node in Root */
{
    SetItem (Root, Left, IDX_LEFT);
}



ExprNode* GetRightNode (ExprNode* N)
/* Get the right sub-node from the list */
{
    return GetNode (N, IDX_RIGHT);
}



void SetRightNode (ExprNode* Root, ExprNode* Right)
/* Set the right node in Root */
{
    SetItem (Root, Right, IDX_RIGHT);
}



struct SymEntry* GetNodeSym (ExprNode* N)
/* Get the symbol entry for a NT_SYM node */
{
    return (struct SymEntry*) GetItem (N, IDX_SYM);
}



void SetNodeSym (ExprNode* N, struct SymEntry* Sym)
/* Set the symbol entry in a NT_SYM node */
{
    SetItem (N, Sym, IDX_SYM);
}



int IsLeafNode (const ExprNode* E)
/* Return true if this is a leaf node */
{
    return (E->NT & NT_MASK_LEAF) == NT_LEAF;
}



int IsBranchNode (const ExprNode* E)
/* Return true if this is a branch node */
{
    return (E->NT & NT_MASK_LEAF) == NT_BRANCH;
}



void DumpExpr (FILE* F, const ExprNode* E)
/* Dump an expression in UPN notation to the given file */
{
    if (IsLeafNode (E)) {

	/* Operand */
	switch (E->NT) {

	    case NT_SYM:
		/* Symbol */
		fprintf (F, "SYM ");
		break;

	    case NT_CONST:
		/* A constant of some sort */
		if (IsClassInt (E->Type)) {
		    fprintf (F, "%0*lX ", SizeOf (E->Type), E->IVal);
		} else if (IsClassFloat (E->Type)) {
		    fprintf (F, "%f ", E->FVal);
		} else {
		    Internal ("Unknown type for NT_CONST");
		}
		break;

	    case NT_ASM:
		/* Inline assembler */
		fprintf (F, "ASM ");
		break;

	    case NT_REG_A:
		/* A register */
		fprintf (F, "REG_A ");
		break;

	    case NT_REG_X:
		/* X register */
		fprintf (F, "REG_X ");
		break;

	    case NT_REG_Y:
		/* Y register */
		fprintf (F, "REG_Y ");
		break;

	    case NT_REG_AX:
		/* AX register */
		fprintf (F, "REG_AX ");
		break;

	    case NT_REG_EAX:
		/* EAX register */
		fprintf (F, "REG_EAX ");
		break;

	    default:
		Internal ("Unknown node type: %04X", E->NT);
		break;

	}

    } else {

	unsigned I, Count;

	/* Dump the operands */
	switch (E->NT & NT_MASK_LIST) {

	    case NT_LIST_EXPR:
		Count = CollCount (&E->List);
		for (I = 0; I < Count; ++I) {
		    DumpExpr (F, (const ExprNode*) CollConstAt (&E->List, I));
		}
		break;

	    default:
		Internal ("Operator with LIST != NT_LIST_EXPR");

	}

	/* Dump the operator */
	switch (E->NT) {

	    case NT_ARRAY_SUBSCRIPT:
		/* Array subscript */
		fprintf (F, "[] ");
		break;

	    case NT_STRUCT_ACCESS:
		/* Access of a struct field */
		fprintf (F, ". ");
		break;

	    case NT_STRUCTPTR_ACCESS:
		/* Access via struct ptr */
		fprintf (F, "-> ");
		break;

	    case NT_FUNCTION_CALL:
		/* Call a function */
		fprintf (F, "CALL ");
		break;

	    case NT_TYPECAST:
		/* A cast */
		fprintf (F, "CAST ");
		break;

	    case NT_ADDRESS:
		/* Address operator (&) */
		fprintf (F, "ADDR ");
		break;

	    case NT_INDIRECT:
		/* Indirection operator (*) */
		fprintf (F, "FETCH ");
		break;

	    case NT_UNARY_MINUS:
		/* - */
		fprintf (F, "NEG ");
		break;

	    case NT_COMPLEMENT:
		/* ~ */
		fprintf (F, "~ ");
		break;

	    case NT_BOOL_NOT:
		/* ! */
		fprintf (F, "! ");
		break;

	    case NT_PLUS:
		/* + */
		fprintf (F, "+ ");
		break;

	    case NT_MINUS:
		/* - */
		fprintf (F, "- ");
		break;

	    case NT_MUL:
		/* * */
		fprintf (F, "* ");
		break;

	    case NT_DIV:
		/* / */
		fprintf (F, "/ ");
		break;

	    case NT_SHL:
		/* << */
		fprintf (F, "<< ");
		break;

	    case NT_SHR:
		/* >> */
		fprintf (F, ">> ");
		break;

	    case NT_AND:
		/* & */
		fprintf (F, "& ");
		break;

	    case NT_OR:
		/* | */
		fprintf (F, "| ");
		break;

	    case NT_XOR:
		/* ^ */
		fprintf (F, "^ ");
		break;

	    case NT_TERNARY:
		/* ?: */
		fprintf (F, "?: ");
		break;

	    case NT_ASSIGN:
		/* = */
		fprintf (F, "= ");
		break;

	    case NT_PLUS_ASSIGN:
		/* += */
		fprintf (F, "+= ");
		break;

	    case NT_MINUS_ASSIGN:
		/* -= */
		fprintf (F, "-= ");
		break;

	    case NT_MUL_ASSIGN:
		/* *= */
		fprintf (F, "*= ");
		break;

	    case NT_DIV_ASSIGN:
		/* /= */
		fprintf (F, "/= ");
		break;

	    case NT_SHL_ASSIGN:
		/* <<= */
		fprintf (F, "<<= ");
		break;

	    case NT_SHR_ASSIGN:
		/* >>= */
		fprintf (F, ">>= ");
		break;

	    case NT_AND_ASSIGN:
		/* &= */
		fprintf (F, "&= ");
		break;

	    case NT_OR_ASSIGN:
		/* |= */
		fprintf (F, "|= ");
		break;

	    case NT_XOR_ASSIGN:
		/* ^= */
		fprintf (F, "^= ");
		break;

	    case NT_PRE_DEC:
		/* -- */
		fprintf (F, "<-- ");
		break;

	    case NT_POST_DEC:
		/* -- */
		fprintf (F, "--> ");
		break;

	    case NT_PRE_INC:
		/* ++ */
		fprintf (F, "<++ ");
		break;

	    case NT_POST_INC:
		/* ++ */
		fprintf (F, "++> ");
		break;

	    case NT_BOOL_OR:
		/* || */
		fprintf (F, "|| ");
		break;

	    case NT_BOOL_AND:
		/* && */
		fprintf (F, "&& ");
		break;

	    case NT_EQ:
		/* == */
		fprintf (F, "== ");
		break;

	    case NT_NE:
		/* != */
		fprintf (F, "!= ");
		break;

	    case NT_LT:
		/* < */
		fprintf (F, "< ");
		break;

	    case NT_LE:
		/* <= */
		fprintf (F, "<= ");
		break;

	    case NT_GT:
		/* > */
		fprintf (F, "> ");
		break;

	    case NT_GE:
		/* >= */
		fprintf (F, ">= ");
		break;

	    default:
		Internal ("Unknown node type: %04X", E->NT);
		break;

	}
    }
}



