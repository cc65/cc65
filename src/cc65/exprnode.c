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
    return GetItem (N, IDX_SYM);
}



void SetNodeSym (ExprNode* N, struct SymEntry* Sym)
/* Set the symbol entry in a NT_SYM node */
{
    SetItem (N, Sym, IDX_SYM);
}




