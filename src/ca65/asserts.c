/*****************************************************************************/
/*                                                                           */
/*                                 asserts.c                                 */
/*                                                                           */
/*               Linker assertions for the ca65 crossassembler               */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2003      Ullrich von Bassewitz                                       */
/*               Römerstrasse 52                                             */
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
#include "coll.h"
#include "xmalloc.h"

/* ca65 */
#include "asserts.h"
#include "expr.h"
#include "objfile.h"
#include "scanner.h"



/*****************************************************************************/
/*     	     	    		     Data     				     */
/*****************************************************************************/



/* An assertion entry */
typedef struct Assertion Assertion;
struct Assertion {
    ExprNode*   Expr;           /* Expression to evaluate */
    unsigned    Action;         /* Action to take */
    unsigned    Msg;            /* Message to print (if any) */
    FilePos     Pos;            /* File position of assertion */
};

/* Collection with all assertions for a module */
static Collection Assertions = STATIC_COLLECTION_INITIALIZER;



/*****************************************************************************/
/*     	     	       		     Code     				     */
/*****************************************************************************/



static Assertion* NewAssertion (ExprNode* Expr, unsigned Action, unsigned Msg)
/* Create a new Assertion struct and return it */
{
    /* Allocate memory */
    Assertion* A = xmalloc (sizeof (Assertion));

    /* Initialize the fields */
    A->Expr     = Expr;
    A->Action   = Action;
    A->Msg      = Msg;
    A->Pos      = CurPos;

    /* Return the new struct */
    return A;
}



void AddAssertion (ExprNode* Expr, unsigned Action, unsigned Msg)
/* Add an assertion to the assertion table */
{
    /* Add an assertion object to the table */
    CollAppend (&Assertions, NewAssertion (Expr, Action, Msg));
}



void WriteAssertions (void)
/* Write the assertion table to the object file */
{
    unsigned I;

    /* Get the number of strings in the string pool */
    unsigned Count = CollCount (&Assertions);

    /* Tell the object file module that we're about to start the assertions */
    ObjStartAssertions ();

    /* Write the string count to the list */
    ObjWriteVar (Count);

    /* Write the assertions */
    for (I = 0; I < Count; ++I) {

        /* Get the next assertion */
        Assertion* A = CollAtUnchecked (&Assertions, I);

        /* Finalize the expression */
        A->Expr = FinalizeExpr (A->Expr);

        /* Write it to the file */
        WriteExpr (A->Expr);
        ObjWriteVar (A->Action);
        ObjWriteVar (A->Msg);
        ObjWritePos (&A->Pos);
    }

    /* Done writing the assertions */
    ObjEndAssertions ();
}



                 
