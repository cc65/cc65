/*****************************************************************************/
/*                                                                           */
/*                                 asserts.c                                 */
/*                                                                           */
/*                      Assertions for the ld65 linker                       */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2003-2011, Ullrich von Bassewitz                                      */
/*                Roemerstrasse 52                                           */
/*                D-70794 Filderstadt                                        */
/* EMail:         uz@cc65.org                                                */
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
#include "assertion.h"
#include "coll.h"
#include "xmalloc.h"

/* ld65 */
#include "asserts.h"
#include "error.h"
#include "expr.h"
#include "fileio.h"
#include "lineinfo.h"
#include "objdata.h"
#include "spool.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Assertion struct decl */
struct Assertion {
    Collection          LineInfos;      /* File position of assertion */
    ExprNode*           Expr;           /* Expression to evaluate */
    AssertAction        Action;         /* What to do */
    unsigned            Msg;            /* Message to print */
    ObjData*            Obj;            /* Object file containing the assertion */
};

/* List with all assertions */
static Collection Assertions = STATIC_COLLECTION_INITIALIZER;



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



Assertion* ReadAssertion (FILE* F, struct ObjData* O)
/* Read an assertion from the given file */
{
    /* Allocate memory */
    Assertion* A = xmalloc (sizeof (Assertion));

    /* Read the fields from the file */
    A->LineInfos = EmptyCollection;
    A->Expr      = ReadExpr (F, O);
    A->Action    = (AssertAction) ReadVar (F);
    A->Msg       = MakeGlobalStringId (O, ReadVar (F));
    ReadLineInfoList (F, O, &A->LineInfos);

    /* Set remaining fields */
    A->Obj = O;

    /* Add the assertion to the global list */
    CollAppend (&Assertions, A);

    /* Return the new struct */
    return A;
}



void CheckAssertions (void)
/* Check all assertions */
{
    unsigned I;

    /* Walk over all assertions */
    for (I = 0; I < CollCount (&Assertions); ++I) {

        const LineInfo* LI;
        const char* Module;
        unsigned Line;

        /* Get the assertion */
        Assertion* A = CollAtUnchecked (&Assertions, I);

        /* Ignore assertions that shouldn't be handled at link time */
        if (!AssertAtLinkTime (A->Action)) {
            continue;
        }

        /* Retrieve the relevant line info for this assertion */
        LI = CollConstAt (&A->LineInfos, 0);

        /* Get file name and line number from the source */
        Module = GetSourceName (LI);
        Line   = GetSourceLine (LI);

        /* If the expression is not constant, we're not able to handle it */
        if (!IsConstExpr (A->Expr)) {
            Warning ("Cannot evaluate assertion in module `%s', line %u",
                     Module, Line);
        } else if (GetExprVal (A->Expr) == 0) {

            /* Assertion failed */
            const char* Message = GetString (A->Msg);

            switch (A->Action) {

                case ASSERT_ACT_WARN:
                case ASSERT_ACT_LDWARN:
                    Warning ("%s(%u): %s", Module, Line, Message);
                    break;

                case ASSERT_ACT_ERROR:
                case ASSERT_ACT_LDERROR:
                    Error ("%s(%u): %s", Module, Line, Message);
                    break;

                default:
                    Internal ("Invalid assertion action (%u) in module `%s', "
                              "line %u (file corrupt?)",
                              A->Action, Module, Line);
                    break;
            }
        }
    }
}
