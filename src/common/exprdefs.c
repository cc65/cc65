/*****************************************************************************/
/*                                                                           */
/*                                exprdefs.c                                 */
/*                                                                           */
/*                        Expression tree definitions                        */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2012, Ullrich von Bassewitz                                      */
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



#include <stdio.h>

#include "abend.h"
#include "exprdefs.h"



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



static void InternalDumpExpr (const ExprNode* Expr, const ExprNode* (*ResolveSym) (const struct SymEntry*))
/* Dump an expression in RPN to stdout */
{
    if (Expr == 0) {
        return;
    }
    InternalDumpExpr (Expr->Left, ResolveSym);
    InternalDumpExpr (Expr->Right, ResolveSym);

    switch (Expr->Op) {

        case EXPR_LITERAL:
        case EXPR_ULABEL:
            printf (" $%04lX", Expr->V.IVal);
            break;

        case EXPR_SYMBOL:
            printf (" SYM(");
            if (ResolveSym && (Expr = ResolveSym (Expr->V.Sym)) != 0) {
                InternalDumpExpr (Expr, ResolveSym);
            }
            printf (") ");
            break;

        case EXPR_SECTION:
            printf (" SEC");
            break;

        case EXPR_SEGMENT:
            printf (" SEG");
            break;

        case EXPR_MEMAREA:
            printf (" MEM");
            break;

        case EXPR_PLUS:
            printf (" +");
            break;

        case EXPR_MINUS:
            printf (" -");
            break;

        case EXPR_MUL:
            printf (" *");
            break;

        case EXPR_DIV:
            printf (" /");
            break;

        case EXPR_MOD:
            printf (" MOD");
            break;

        case EXPR_OR:
            printf (" OR");
            break;

        case EXPR_XOR:
            printf (" XOR");
            break;

        case EXPR_AND:
            printf (" AND");
            break;

        case EXPR_SHL:
            printf (" SHL");
            break;

        case EXPR_SHR:
            printf (" SHR");
            break;

        case EXPR_EQ:
            printf (" =");
            break;

        case EXPR_NE:
            printf ("<>");
            break;

        case EXPR_LT:
            printf (" <");
            break;

        case EXPR_GT:
            printf (" >");
            break;

        case EXPR_LE:
            printf (" <=");
            break;

        case EXPR_GE:
            printf (" >=");
            break;

        case EXPR_BOOLAND:
            printf (" BOOL_AND");
            break;

        case EXPR_BOOLOR:
            printf (" BOOL_OR");
            break;

        case EXPR_BOOLXOR:
            printf (" BOOL_XOR");
            break;

        case EXPR_MAX:
            printf (" MAX");
            break;

        case EXPR_MIN:
            printf (" MIN");
            break;

        case EXPR_UNARY_MINUS:
            printf (" NEG");
            break;

        case EXPR_NOT:
            printf (" ~");
            break;

        case EXPR_SWAP:
            printf (" SWAP");
            break;

        case EXPR_BOOLNOT:
            printf (" BOOL_NOT");
            break;

        case EXPR_BANK:
            printf (" BANK");
            break;

        case EXPR_BYTE0:
            printf (" BYTE0");
            break;

        case EXPR_BYTE1:
            printf (" BYTE1");
            break;

        case EXPR_BYTE2:
            printf (" BYTE2");
            break;

        case EXPR_BYTE3:
            printf (" BYTE3");
            break;

        case EXPR_WORD0:
            printf (" WORD0");
            break;

        case EXPR_WORD1:
            printf (" WORD1");
            break;

        case EXPR_FARADDR:
            printf (" FARADDR");
            break;

        case EXPR_DWORD:
            printf (" DWORD");
            break;

        default:
            AbEnd ("Unknown Op type: %u", Expr->Op);

    }
}



void DumpExpr (const ExprNode* Expr, const ExprNode* (*ResolveSym) (const struct SymEntry*))
/* Dump an expression tree to stdout */
{
    InternalDumpExpr (Expr, ResolveSym);
    printf ("\n");
}
