/*****************************************************************************/
/*                                                                           */
/*                               staticassert.h                              */
/*                                                                           */
/*          _Static_assert handling for the cc65 C compiler                  */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* Copyright 2020 The cc65 Authors                                           */
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
#include "error.h"
#include "expr.h"
#include "litpool.h"
#include "scanner.h"
#include "staticassert.h"



/*****************************************************************************/
/*                      _Static_assert handling functions                    */
/*****************************************************************************/



void ParseStaticAssert ()
{
    /*
    ** static_assert-declaration ::=
    **     _Static_assert ( constant-expression , string-literal ) ;
    */
    ExprDesc Expr;
    int failed;

    /* Skip the _Static_assert token itself */
    CHECK (CurTok.Tok == TOK_STATIC_ASSERT);
    NextToken ();

    /* We expect an opening paren */
    if (!ConsumeLParen ()) {
        return;
    }

    /* Parse assertion condition */
    ConstAbsIntExpr (hie1, &Expr);
    failed = !Expr.IVal;

    /* We expect a comma */
    if (!ConsumeComma ()) {
        return;
    }

    /* String literal */
    if (CurTok.Tok != TOK_SCONST) {
        Error ("String literal expected for static_assert message");
        return;
    }

    /* Issue an error including the message if the static_assert failed. */
    if (failed) {
        Error ("static_assert failed '%s'", GetLiteralStr (CurTok.SVal));
    }

    /* Consume the string constant, now that we don't need it anymore.
    ** This should never fail since we checked the token type above.
    */
    if (!Consume (TOK_SCONST, "String literal expected")) {
        return;
    }

    /* Closing paren and semi-colon needed */
    ConsumeRParen ();
    ConsumeSemi ();
}
