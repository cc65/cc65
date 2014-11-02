/*****************************************************************************/
/*                                                                           */
/*                                 cfgexpr.c                                 */
/*                                                                           */
/*          Simple expressions for use with in configuration file            */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2005-2011, Ullrich von Bassewitz                                      */
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
#include "addrsize.h"
#include "strbuf.h"

/* ld65 */
#include "cfgexpr.h"
#include "error.h"
#include "exports.h"
#include "expr.h"
#include "lineinfo.h"
#include "scanner.h"
#include "spool.h"



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



static ExprNode* Factor (void)
/* Read and return a factor */
{
    ExprNode* N = 0;            /* Initialize to avoid compiler warnings */
    Export*   E;
    unsigned  Name;


    switch (CfgTok) {

        case CFGTOK_IDENT:
            /* Get the name as an id */
            Name = GetStrBufId (&CfgSVal);

            /* Check if we know the symbol already */
            E = FindExport (Name);
            if (E != 0 && IsConstExport (E)) {
                N = LiteralExpr (GetExportVal (E), 0);
            } else {
                N = NewExprNode (0, EXPR_SYMBOL);
                N->V.Imp = InsertImport (GenImport (Name, ADDR_SIZE_ABS));
                CollAppend (&N->V.Imp->RefLines, GenLineInfo (&CfgErrorPos));
            }

            /* Skip the symbol name */
            CfgNextTok ();
            break;

        case CFGTOK_INTCON:
            /* An integer constant */
            N = LiteralExpr (CfgIVal, 0);
            CfgNextTok ();
            break;

        case CFGTOK_PLUS:
            /* Unary plus */
            CfgNextTok ();
            N = Factor ();
            break;

        case CFGTOK_MINUS:
            /* Unary minus */
            CfgNextTok ();
            N = NewExprNode (0, EXPR_UNARY_MINUS);
            N->Left = Factor ();
            break;

        case CFGTOK_LPAR:
            /* Left parenthesis */
            CfgNextTok ();
            N = CfgExpr ();
            CfgConsume (CFGTOK_RPAR, "')' expected");
            break;

        default:
            CfgError (&CfgErrorPos, "Invalid expression: %d", CfgTok);
            break;
    }

    /* Return the new expression node */
    return N;
}



static ExprNode* Term (void)
/* Multiplicative operators: * and / */
{
    /* Read left hand side */
    ExprNode* Root = Factor ();

    /* Handle multiplicative operators */
    while (CfgTok == CFGTOK_MUL || CfgTok == CFGTOK_DIV) {

        ExprNode* Left;
        ExprNode* Right;
        unsigned char Op;

        /* Remember the token, then skip it */
        cfgtok_t Tok = CfgTok;
        CfgNextTok ();

        /* Move root to left side, then read right side */
        Left = Root;
        Right = Factor ();

        /* Handle the operation */
        switch (Tok) {
            case CFGTOK_MUL:    Op = EXPR_MUL;  break;
            case CFGTOK_DIV:    Op = EXPR_DIV;  break;
            default:            Internal ("Unhandled token in Term: %d", Tok);
        }
        Root = NewExprNode (0, Op);
        Root->Left = Left;
        Root->Right = Right;
    }

    /* Return the expression tree we've created */
    return Root;
}



static ExprNode* SimpleExpr (void)
/* Additive operators: + and - */
{
    /* Read left hand side */
    ExprNode* Root = Term ();

    /* Handle additive operators */
    while (CfgTok == CFGTOK_PLUS || CfgTok == CFGTOK_MINUS) {

        ExprNode* Left;
        ExprNode* Right;
        unsigned char Op;

        /* Remember the token, then skip it */
        cfgtok_t Tok = CfgTok;
        CfgNextTok ();

        /* Move root to left side, then read right side */
        Left = Root;
        Right = Term ();

        /* Handle the operation */
        switch (Tok) {
            case CFGTOK_PLUS:   Op = EXPR_PLUS;         break;
            case CFGTOK_MINUS:  Op = EXPR_MINUS;        break;
            default:            Internal ("Unhandled token in SimpleExpr: %d", Tok);
        }
        Root = NewExprNode (0, Op);
        Root->Left = Left;
        Root->Right = Right;
    }

    /* Return the expression tree we've created */
    return Root;
}



ExprNode* CfgExpr (void)
/* Full expression */
{
    return SimpleExpr ();
}



long CfgConstExpr (void)
/* Read an integer expression, make sure its constant and return its value */
{
    long Val;

    /* Parse the expression */
    ExprNode* Expr = CfgExpr ();

    /* Check that it's const */
    if (!IsConstExpr (Expr)) {
        CfgError (&CfgErrorPos, "Constant expression expected");
    }

    /* Get the value */
    Val = GetExprVal (Expr);

    /* Cleanup E */
    FreeExpr (Expr);

    /* Return the value */
    return Val;
}



long CfgCheckedConstExpr (long Min, long Max)
/* Read an expression, make sure it's an int and in range, then return its
** value.
*/
{
    /* Get the value */
    long Val = CfgConstExpr ();

    /* Check the range */
    if (Val < Min || Val > Max) {
        CfgError (&CfgErrorPos, "Range error");
    }

    /* Return the value */
    return Val;
}
