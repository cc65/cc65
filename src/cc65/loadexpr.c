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



#include <limits.h>

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
        unsigned EndBit = 0;  /* End bit for bit-fields, or zero if non-bit-field. */
        int AdjustBitField = 0;
        if (ED_IsBitField (Expr)) {
            EndBit = Expr->BitOffs + Expr->BitWidth;
            AdjustBitField = Expr->BitOffs != 0 || (EndBit != CHAR_BITS && EndBit != INT_BITS);

            Flags |= (EndBit <= CHAR_BITS) ? CF_CHAR : CF_INT;
            Flags |= CF_UNSIGNED;
        } else if ((Flags & CF_TYPEMASK) == 0) {
            Flags |= TypeOf (Expr->Type);
        }

        if (ED_NeedsTest (Expr)) {
            /* If we're only testing, we do not need to promote char to int.
            ** CF_FORCECHAR does nothing if the type is not CF_CHAR.
            */
            Flags |= CF_FORCECHAR;

            /* Setting CF_TEST will cause the load to perform optimizations and not actually load
            ** all bits of the bit-field, instead just computing the condition codes.  Therefore,
            ** if adjustment is required, we do not set CF_TEST here, but handle it below.
            */
            if (!AdjustBitField) {
                Flags |= CF_TEST;
            }
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

        /* Handle bit fields if necessary. The actual type may have been casted or converted,
        ** so be sure to always use unsigned ints for the operations.
        */
        if (AdjustBitField) {
            /* If the field was loaded as a char, force the shift/mask ops to be char ops.
            ** If it is a char, the load has already put 0 in the upper byte, so that can remain.
            */
            unsigned F = Flags | CF_FORCECHAR | CF_CONST;

            /* We always need to do something with the low byte, so there is no opportunity
            ** for optimization by skipping it.
            */
            CHECK (Expr->BitOffs < CHAR_BITS);

            if (ED_NeedsTest (Expr)) {
                /* If we need to do a test, then we avoid shifting (ASR only shifts one bit
                ** at a time, so is slow) and just AND with the appropriate mask, then test
                ** the result of that.
                */

                /* Avoid overly large shift on host platform. */
                if (EndBit == sizeof (unsigned long) * CHAR_BIT) {
                    g_and (F, (~0UL << Expr->BitOffs));
                } else {
                    g_and (F, ((1UL << EndBit) - 1) & (~0UL << Expr->BitOffs));
                }

                /* TODO: When long bit-fields are supported, an optimization to test only 3 bytes
                ** when EndBit <= 24 is possible.
                */
                g_test (F);
                ED_TestDone (Expr);
            } else {
                /* Shift right by the bit offset; no code is emitted if BitOffs is zero */
                g_asr (F, Expr->BitOffs);

                /* Since we have now shifted down, we can do char ops as long as the width fits in
                ** a char.
                */
                if (Expr->BitWidth <= CHAR_BITS) {
                    F |= CF_CHAR;
                }

                /* And by the width if the field doesn't end on a char or int boundary.
                ** If it does end on a boundary, then zeros have already been shifted in.
                ** g_and emits no code if the mask is all ones.
                */
                if (EndBit != CHAR_BITS && EndBit != INT_BITS) {
                    g_and (F, (0x0001U << Expr->BitWidth) - 1U);
                }
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
