/*****************************************************************************/
/*                                                                           */
/*				  exprheap.c				     */
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



/* common */
#include "check.h"
#include "xmalloc.h"

/* cc65 */
#include "exprheap.h"



/*****************************************************************************/
/*	      	  		     Data				     */
/*****************************************************************************/



/* A block of expression nodes */
typedef struct ExprNodeBlock ExprNodeBlock;
struct ExprNodeBlock {
    ExprNodeBlock*	Next;		/* Pointer to next block */
    unsigned	  	Count;		/* Number of nodes in the block */
    unsigned	  	Used;		/* Number of nodes used */
    ExprNode   	       	Nodes[1];	/* Nodes, dynamically allocated */
};

/* An expression heap */
struct ExprHeap {
    ExprHeap*		Last;		/* Upper level expression tree */
    ExprNodeBlock*     	BlockRoot; 	/* Root of node blocks */
    ExprNodeBlock*    	BlockLast;  	/* Last node block */
    ExprNode*	      	FreeList;	/* List of free nodes */
};

/* The current expression heap */
static ExprHeap*	CurHeap = 0;



/*****************************************************************************/
/*			     struct ExprHeapBlock			     */
/*****************************************************************************/



static ExprNodeBlock* NewExprNodeBlock (unsigned Count)
/* Create a new ExprNodeBlock, initialize and return it */
{
    /* Calculate the size of the memory block requested */
    unsigned Size = sizeof (ExprNodeBlock) + (Count-1) * sizeof (ExprNode);

    /* Allocate memory */
    ExprNodeBlock* B = xmalloc (Size);

    /* Initialize the fields */
    B->Next  = 0;
    B->Count = Count;
    B->Used  = 0;

    /* Return the new block */
    return B;
}



/*****************************************************************************/
/*				struct ExprHeap				     */
/*****************************************************************************/



static ExprHeap* NewExprHeap (void)
/* Create and return a new expression tree */
{
    /* Allocate memory */
    ExprHeap* H = xmalloc (sizeof (ExprHeap));

    /* Allocate the first node block */
    H->BlockRoot = NewExprNodeBlock (64);

    /* Initialize the remaining fields */
    H->Last      = 0;
    H->BlockLast = H->BlockRoot;
    H->FreeList  = 0;

    /* Return the new heap */
    return H;
}



/*****************************************************************************/
/*	      	  		     Code				     */
/*****************************************************************************/



void PushExprHeap (void)
/* Create a new expression heap and push it onto the expression heap stack, so
 * it is the current expression heap.
 */
{
    /* Create a new heap */
    ExprHeap* H = NewExprHeap ();

    /* Push it onto the stack */
    H->Last = CurHeap;
    CurHeap = H;
}



ExprHeap* PopExprHeap (void)
/* Pop the current expression heap from the heap stack and return it */
{
    ExprHeap* H;

    /* Cannot pop a non existant heap */
    PRECONDITION (CurHeap != 0);

    /* Pop the heap */
    H = CurHeap;
    CurHeap = H->Last;

    /* Return the old heap */
    return H;
}



ExprNode* AllocExprNode (nodetype_t NT, type* Type, int LValue)
/* Get a new node from the current expression heap */
{							       
    ExprNode* N;

    /* Must have a heap */
    PRECONDITION (CurHeap != 0);

    /* Get a node from the freelist if possible */
    if (CurHeap->FreeList) {
	/* There are nodes in the free list */
	N = CurHeap->FreeList;
 	CurHeap->FreeList = N->MData.Next;
    } else {
	/* Free list is empty, allocate a new node */
	ExprNodeBlock* B = CurHeap->BlockLast;
	if (B->Used >= B->Count) {
	    /* No nodes left, allocate a new node block */
	    B = NewExprNodeBlock (64);
	    CurHeap->BlockLast->Next = B;
	    CurHeap->BlockLast = B;
	}
	N = B->Nodes + B->Count++;
    }

    /* Initialize and return the allocated node */
    return InitExprNode (N, NT, Type, LValue, CurHeap);
}



void FreeExprNode (ExprNode* N)
/* Free an expression node from the current expression heap */
{
    /* There must be a heap, and the node must be from this heap */
    PRECONDITION (CurHeap != 0 && N->MData.Owner == CurHeap);

    /* Insert the node in the freelist invalidating the owner pointer */
    N->MData.Next = CurHeap->FreeList;
    CurHeap->FreeList = N;
}




