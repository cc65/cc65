/*****************************************************************************/
/*                                                                           */
/*                                testexpr.c                                 */
/*                                                                           */
/*                        Test an expression and jump                        */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2004      Ullrich von Bassewitz                                       */
/*               Roemerstrasse 52                                            */
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



/* cc65 */
#include "codegen.h"
#include "error.h"
#include "expr.h"
#include "loadexpr.h"
#include "scanner.h"
#include "seqpoint.h"
#include "testexpr.h"



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



unsigned Test (unsigned Label, int Invert)
/* Evaluate a boolean test expression and jump depending on the result of
** the test and on Invert. The function returns one of the TESTEXPR_xx codes
** defined above. If the jump is always true, a warning is output.
*/
{
    ExprDesc Expr;
    unsigned Result;

    ED_Init (&Expr);

    /* Read a boolean expression */
    BoolExpr (hie0, &Expr);

    /* Check for a constant numeric expression */
    if (ED_IsConstAbs (&Expr)) {

        /* Append deferred inc/dec at sequence point */
        DoDeferred (SQP_KEEP_NONE, &Expr);

        /* Result is constant, so we know the outcome */
        Result = (Expr.IVal != 0) ? TESTEXPR_TRUE : TESTEXPR_FALSE;

        /* Constant rvalue */
        if (!Invert && Expr.IVal == 0) {
            g_jump (Label);
            Warning ("Unreachable code");
        } else if (Invert && Expr.IVal != 0) {
            g_jump (Label);
        }

    } else if (ED_IsAddrExpr (&Expr)) {

        /* Append deferred inc/dec at sequence point */
        DoDeferred (SQP_KEEP_NONE, &Expr);

        /* Object addresses are non-NULL */
        Result = TESTEXPR_TRUE;

        /* Condition is always true */
        if (Invert) {
            g_jump (Label);
        }

    } else {

        /* Result is unknown */
        Result = TESTEXPR_UNKNOWN;

        /* Set the test flag */
        ED_RequireTest (&Expr);

        /* Load the value into the primary register */
        LoadExpr (CF_FORCECHAR, &Expr);

        /* Append deferred inc/dec at sequence point */
        DoDeferred (SQP_KEEP_TEST, &Expr);

        /* Generate the jump */
        if (Invert) {
            g_truejump (CF_NONE, Label);
        } else {
            g_falsejump (CF_NONE, Label);
        }
    }

    /* Return the result */
    return Result;
}



unsigned TestInParens (unsigned Label, int Invert)
/* Evaluate a boolean test expression in parenthesis and jump depending on
** the result of the test * and on Invert. The function returns one of the
** TESTEXPR_xx codes defined above. If the jump is always true, a warning is
** output.
*/
{
    unsigned Result;

    /* Eat the parenthesis */
    ConsumeLParen ();

    /* Do the test */
    Result = Test (Label, Invert);

    /* Check for the closing brace */
    ConsumeRParen ();

    /* Return the result of the expression */
    return Result;
}
