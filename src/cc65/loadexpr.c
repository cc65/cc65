/*****************************************************************************/
/*                                                                           */
/*                                loadexpr.c                                 */
/*                                                                           */
/*               Load an expression into the primary register                */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2004-2009, Ullrich von Bassewitz                                      */
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



/* cc65 */
#include "codegen.h"
#include "error.h"
#include "exprdesc.h"
#include "global.h"
#include "loadexpr.h"



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



static void LoadAddress (unsigned Flags, ExprDesc* Expr)
/* Load the primary register with some address value. */
{
    switch (ED_GetLoc (Expr)) {

        case E_LOC_ABS:
            /* Numberic address */
            g_getimmed (Flags | CF_IMM | CF_CONST, Expr->IVal, 0);
            break;

        case E_LOC_GLOBAL:
            /* Global symbol, load address */
            g_getimmed ((Flags | CF_EXTERNAL) & ~CF_CONST, Expr->Name, Expr->IVal);
            break;

        case E_LOC_STATIC:
        case E_LOC_LITERAL:
            /* Static symbol or literal, load address */
            g_getimmed ((Flags | CF_STATIC) & ~CF_CONST, Expr->Name, Expr->IVal);
            break;

        case E_LOC_REGISTER:
            /* Register variable. Taking the address is usually not
            ** allowed.
            */
            if (IS_Get (&AllowRegVarAddr) == 0) {
                Error ("Cannot take the address of a register variable");
            }
            g_getimmed ((Flags | CF_REGVAR) & ~CF_CONST, Expr->Name, Expr->IVal);
            break;

        case E_LOC_STACK:
            g_leasp (Expr->IVal);
            break;

        case E_LOC_EXPR:
            if (Expr->IVal != 0) {
                /* We have an expression in the primary plus a constant
                ** offset. Adjust the value in the primary accordingly.
                */
                g_inc (Flags | CF_CONST, Expr->IVal);
            }
            break;

        default:
            Internal ("Unknown address type: %04X", Expr->Flags);
    }
}



void LoadExpr (unsigned Flags, struct ExprDesc* Expr)
/* Load an expression into the primary register if it is not already there.
** Note: This function can't modify the content in Expr since there are many
** instances of the "GetCodePos + LoadExpr (maybe indirectly) + RemoveCode"
** code pattern here and there which assumes that Expr should be unchanged,
** unfortunately.
*/
{
    if (!ED_IsAddrExpr (Expr)) {

        /* Lvalue. If this is a bit field its type is unsigned. But if the
        ** field is completely contained in the lower byte, we will throw away
        ** the high byte anyway and may therefore load just the low byte.
        */
        if (ED_IsBitField (Expr)) {
            Flags |= (Expr->BitOffs + Expr->BitWidth <= CHAR_BITS) ? CF_CHAR : CF_INT;
            Flags |= CF_UNSIGNED;
        } else {
            Flags |= TypeOf (Expr->Type);
        }
        if (ED_NeedsTest (Expr)) {
            Flags |= CF_TEST;
        }

        /* Load the content of Expr */
        switch (ED_GetLoc (Expr)) {

            case E_LOC_NONE:
                /* Immediate number constant */
                g_getimmed (Flags | CF_IMM | TypeOf (Expr->Type) | CF_CONST, Expr->IVal, 0);
                break;

            case E_LOC_ABS:
                /* Absolute numeric addressed variable */
                g_getstatic (Flags | CF_ABSOLUTE, Expr->IVal, 0);
                break;

            case E_LOC_GLOBAL:
                /* Global variable */
                g_getstatic (Flags | CF_EXTERNAL, Expr->Name, Expr->IVal);
                break;

            case E_LOC_STATIC:
            case E_LOC_LITERAL:
                /* Static variable or literal in the literal pool */
                g_getstatic (Flags | CF_STATIC, Expr->Name, Expr->IVal);
                break;

            case E_LOC_REGISTER:
                /* Register variable */
                g_getstatic (Flags | CF_REGVAR, Expr->Name, Expr->IVal);
                break;

            case E_LOC_STACK:
                /* Value on the stack */
                g_getlocal (Flags, Expr->IVal);
                break;

            case E_LOC_PRIMARY:
                /* The primary register */
                if (Expr->IVal != 0) {
                    /* We have an expression in the primary plus a constant
                    ** offset. Adjust the value in the primary accordingly.
                    */
                    g_inc (Flags | CF_CONST, Expr->IVal);

                    /* We might want to clear the offset, but we can't */
                }
                if (Flags & CF_TEST) {
                    g_test (Flags);
                }
                break;

            case E_LOC_EXPR:
                /* Reference to address in primary with offset in Expr */
                g_getind (Flags, Expr->IVal);

                /* Since the content in primary is now overwritten with the
                ** dereference value, we might want to change the expression
                ** loc to E_LOC_PRIMARY as well. That way we could be able to
                ** call this function as many times as we want. Unfortunately,
                ** we can't.
                */
                break;

            default:
                Internal ("Invalid location in LoadExpr: 0x%04X", ED_GetLoc (Expr));
        }

        /* Handle bit fields. The actual type may have been casted or
        ** converted, so be sure to always use unsigned ints for the
        ** operations.
        */
        if (ED_IsBitField (Expr)) {
            unsigned F = CF_INT | CF_UNSIGNED | CF_CONST | (Flags & CF_TEST);
            /* Shift right by the bit offset */
            g_asr (F, Expr->BitOffs);
            /* And by the width if the field doesn't end on an int boundary */
            if (Expr->BitOffs + Expr->BitWidth != CHAR_BITS &&
                Expr->BitOffs + Expr->BitWidth != INT_BITS) {
                g_and (F, (0x0001U << Expr->BitWidth) - 1U);
            }
        }

        /* Expression was tested */
        ED_TestDone (Expr);

    } else {
        /* An address */
        Flags |= CF_INT | CF_UNSIGNED;
        /* Constant of some sort, load it into the primary */
        LoadAddress (Flags, Expr);

        /* Are we testing this value? */
        if (ED_NeedsTest (Expr)) {
            /* Yes, force a test */
            g_test (Flags);
            ED_TestDone (Expr);
        }
    }

}
