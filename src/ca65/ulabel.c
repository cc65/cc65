/*****************************************************************************/
/*                                                                           */
/*				   ulabel.c				     */
/*                                                                           */
/*		  Unnamed labels for the ca65 macroassembler		     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000-2003 Ullrich von Bassewitz                                       */
/*               Römerstraße 52                                              */
/*               D-70794 Filderstadt                                         */
/* EMail:        uz@cc65.org                                                 */
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
#include "filepos.h"
#include "xmalloc.h"

/* ca65 */
#include "error.h"
#include "expr.h"
#include "scanner.h"
#include "ulabel.h"



/*****************************************************************************/
/*  		    	      	     Data				     */
/*****************************************************************************/



/* Struct that describes an unnamed label */
typedef struct ULabel ULabel;
struct ULabel {
    ULabel*    	Prev;         		/* Pointer to previous node in list */
    ULabel*    	Next;  	       		/* Pointer to next node in list */
    FilePos	Pos;			/* Position of the label in the source */
    ExprNode*	Val;          	       	/* The label value - may be NULL */
};

/* List management */
static ULabel*  ULabRoot       	= 0;	/* Root of the list */
static ULabel*  ULabLast	= 0;    /* Last ULabel */
static ULabel*  ULabLastDef	= 0;    /* Last defined ULabel */
static unsigned	ULabCount	= 0; 	/* Number of labels */
static unsigned ULabDefCount	= 0;	/* Number of defined labels */
static ULabel** ULabList	= 0;	/* Array with pointers to all labels */



/*****************************************************************************/
/*     	      	     	  	     Code		     	     	     */
/*****************************************************************************/



static ULabel* NewULabel (ExprNode* Val)
/* Create a new ULabel and insert it into the list. The function will move
 * ULabelLast, but not ULabelLastDef. The created label structure is returned.
 */
{
    /* Allocate memory for the ULabel structure */
    ULabel* L = xmalloc (sizeof (ULabel));

    /* Initialize the fields */
    L->Pos = CurPos;
    L->Val = Val;

    /* Insert the label into the list */
    L->Next = 0;
    if (ULabRoot == 0) {
	/* First label */
	L->Prev = 0;
	ULabRoot = L;
    } else {
    	ULabLast->Next = L;
    	L->Prev = ULabLast;
    }
    ULabLast = L;

    /* One label more */
    ++ULabCount;

    /* Return the created label */
    return L;
}



ExprNode* ULabRef (int Which)
/* Get an unnamed label. If Which is negative, it is a backreference (a
 * reference to an already defined label), and the function will return a
 * segment relative expression. If Which is positive, it is a forward ref,
 * and the function will return a expression node for an unnamed label that
 * must be resolved later.
 */
{
    ULabel* L;

    /* Which can never be 0 */
    PRECONDITION (Which != 0);

    /* Which is never really big (usually -3..+3), so a linear search is
     * the best we can do here.
     */
    L = ULabLastDef;
    if (Which < 0) {
      	/* Backward reference */
      	while (Which < -1 && L != 0) {
      	    L = L->Prev;
      	    ++Which;
      	}
      	if (L == 0) {
      	    /* Label does not exist */
      	    Error ("Undefined label");
      	    /* We must return something valid */
      	    return GenCurrentPC();
      	} else {
      	    /* Return a copy of the label value */
      	    return CloneExpr (L->Val);
      	}
    } else {
      	/* Forward reference. Create labels as needed */
	unsigned LabelNum = ULabDefCount + Which - 1;
	while (LabelNum < ULabCount) {
       	    NewULabel (0);
	}

	/* Return an unnamed label expression */
       	return GenULabelExpr (LabelNum);
    }
}



void ULabDef (void)
/* Define an unnamed label at the current PC */
{
    /* Create a new label if needed, or use an existing one */
    if (ULabLastDef == 0 || ULabLastDef->Next == 0) {
      	/* The last label is also the last defined label, we need a new one */
      	ULabLastDef = NewULabel (GenCurrentPC ());
    } else {
      	/* We do already have the label, but it's undefined until now */
      	ULabLastDef = ULabLastDef->Next;
      	ULabLastDef->Val = GenCurrentPC ();
      	ULabLastDef->Pos = CurPos;
    }
    ++ULabDefCount;
}



int ULabCanResolve (void)
/* Return true if we can resolve arbitrary ULabels. */
{
    /* We can resolve labels if we have built the necessary access array */
    return (ULabList != 0);
}



ExprNode* ULabResolve (unsigned Index)
/* Return a valid expression for the unnamed label with the given index. This
 * is used to resolve unnamed labels when assembly is done, so it is an error
 * if a label is still undefined in this phase.
 */
{
    ULabel* L;

    /* Must be in resolve phase and the index must be valid */
    CHECK (ULabList != 0 && Index < ULabCount);

    /* Get the label */
    L = ULabList [Index];

    /* If the label is open (not defined), return some valid value */
    if (L->Val == 0) {
	return GenLiteralExpr (0);
    } else {
	return CloneExpr (L->Val);
    }
}



void ULabCheck (void)
/* Run through all unnamed labels and check for anomalies and errors */
{
    ULabel* L;

    /* Check if there are undefined labels */
    if (ULabLastDef) {
	L = ULabLastDef->Next;
	while (L) {
	    PError (&L->Pos, "Undefined label");
	    L = L->Next;
	}
    }

    /* Create an array that holds pointers to all labels. This allows us to
     * access the labels quickly by index in the resolver phase at the end of
     * the assembly.
     */
    if (ULabCount) {
	unsigned I = 0;
	ULabList = xmalloc (ULabCount * sizeof (ULabel*));
	L = ULabRoot;
	while (L) {
	    ULabList[I] = L;
	    ++I;
	    L = L->Next;
	}
	CHECK (I == ULabCount);
    }
}



