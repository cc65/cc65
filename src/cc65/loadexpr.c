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

#include <stdlib.h>

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
    printf("LoadAddress flags:%04x\n", Flags);
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
            /* Static symbol, load address */
            g_getimmed ((Flags | CF_STATIC) & ~CF_CONST, Expr->Name, Expr->IVal);
            break;

        case E_LOC_LITERAL:
            /* Literal, load address */
            g_getimmed ((Flags | CF_LITERAL) & ~CF_CONST, Expr->Name, Expr->IVal);
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

        case E_LOC_CODE:
            /* Code label, load address */
            g_getimmed ((Flags | CF_CODE) & ~CF_CONST, Expr->Name, Expr->IVal);
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
** If Flags contains any CF_TYPEMASK bits, it then overrides the codegen type
** info that would be otherwise taken from the expression type.
** Note: This function can't modify the content in Expr since there are many
** instances of the "GetCodePos + LoadExpr (maybe indirectly) + RemoveCode"
** code pattern here and there which assumes that Expr should be unchanged,
** unfortunately.
*/
{
    /*printf("LoadExpr flags:%4x\n", Flags);*/
    if (!ED_IsAddrExpr (Expr)) {

        /* Lvalue. If this is a bit field its type is unsigned. But if the
        ** field is completely contained in the lower byte, we will throw away
        ** the high byte anyway and may therefore load just the low byte.
        */
        int AdjustBitField = 0;
        unsigned BitFieldFullWidthFlags = 0;
        if ((Flags & CF_TYPEMASK) == 0) {
            if (IsTypeFragBitField (Expr->Type)) {
                /* We need to adjust the bits in this case.  */
                AdjustBitField = 1;

                /* Flags we need operate on the whole bit-field, without CF_FORCECHAR.  */
                BitFieldFullWidthFlags = Flags | TypeOf (Expr->Type);

                /* Flags we need operate on the whole chunk containing the bit-field.  */
                Flags |= TypeOf (GetBitFieldChunkType (Expr->Type));

                /* If we're adjusting, then only load a char (not an int) and do only char ops;
                ** We will clear the high byte in the adjustment.  CF_FORCECHAR does nothing if
                ** the type is not CF_CHAR;
                ** If adjusting, then we're sign extending manually, so do everything unsigned
                ** to make shifts faster.
                */
                Flags |= CF_UNSIGNED | CF_FORCECHAR;
                BitFieldFullWidthFlags |= CF_UNSIGNED;
            } else {
                /* If Expr is an incomplete ESY type, bail out */
                if (IsIncompleteESUType (Expr->Type)) {
                    return;
                }
                Flags |= TypeOf (Expr->Type);
            }
        }

        if (ED_YetToTest (Expr)) {
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
        /*printf("LoadExpr ED_GetLoc:%04x\n", ED_GetLoc (Expr));*/
        switch (ED_GetLoc (Expr)) {

            case E_LOC_NONE:
                /* FIXME: float */
                /*printf("LoadExpr E_LOC_NONE (%s)\n", (TypeOf (Expr->Type) == CF_FLOAT) ? "float" : "integer");*/
                /* Immediate number constant */
                if (TypeOf (Expr->Type) == CF_FLOAT) {
                    g_getimmed (Flags | CF_IMM | TypeOf (Expr->Type) | CF_CONST, FP_D_As32bitRaw(Expr->V.FVal), 0);
                } else {
                    g_getimmed (Flags | CF_IMM | TypeOf (Expr->Type) | CF_CONST, Expr->IVal, 0);
                }
                break;

            case E_LOC_ABS:
                printf("LoadExpr E_LOC_ABS (%s)\n", (TypeOf (Expr->Type) == CF_FLOAT) ? "float" : "integer");
                if (TypeOf (Expr->Type) == CF_FLOAT) { printf("%s:%d FIXME: E_LOC_ABS\n", __FILE__, __LINE__); exit(-1); }
                /* Absolute numeric addressed variable */
                g_getstatic (Flags | CF_ABSOLUTE, Expr->IVal, 0);
                break;

            case E_LOC_GLOBAL:
                /* printf("LoadExpr E_LOC_GLOBAL (%s)\n", (TypeOf (Expr->Type) == CF_FLOAT) ? "float" : "integer"); */
                /* if (TypeOf (Expr->Type) == CF_FLOAT) { printf("%s:%d FIXME: E_LOC_GLOBAL\n", __FILE__, __LINE__); exit(-1); } */
                /* Global variable, offset in IVal */
                g_getstatic (Flags | CF_EXTERNAL, Expr->Name, Expr->IVal);
                break;

            case E_LOC_STATIC:
                /* printf("LoadExpr E_LOC_STATIC (%s)\n", (TypeOf (Expr->Type) == CF_FLOAT) ? "float" : "integer"); */
                /* if (TypeOf (Expr->Type) == CF_FLOAT) { printf("%s:%d FIXME: E_LOC_STATIC\n", __FILE__, __LINE__); exit(-1); } */
                /* Static variable, offset in IVal */
                g_getstatic (Flags | CF_STATIC, Expr->Name, Expr->IVal);
                break;

            case E_LOC_LITERAL:
                /* printf("LoadExpr E_LOC_LITERAL (%s)\n", (TypeOf (Expr->Type) == CF_FLOAT) ? "float" : "integer"); */
                /* if (TypeOf (Expr->Type) == CF_FLOAT) { printf("%s:%d FIXME: E_LOC_LITERAL\n", __FILE__, __LINE__); exit(-1); } */
                /* Literal in the literal pool, offset in IVal */
                g_getstatic (Flags | CF_LITERAL, Expr->Name, Expr->IVal);
                break;

            case E_LOC_REGISTER:
                /* printf("LoadExpr E_LOC_REGISTER (%s)\n", (TypeOf (Expr->Type) == CF_FLOAT) ? "float" : "integer"); */
                /* if (TypeOf (Expr->Type) == CF_FLOAT) { printf("%s:%d FIXME: E_LOC_REGISTER\n", __FILE__, __LINE__); exit(-1); } */
                /* Register variable, offset in IVal */
                g_getstatic (Flags | CF_REGVAR, Expr->Name, Expr->IVal);
                break;

            case E_LOC_CODE:
                /* printf("LoadExpr E_LOC_CODE (%s)\n", (TypeOf (Expr->Type) == CF_FLOAT) ? "float" : "integer"); */
                /* if (TypeOf (Expr->Type) == CF_FLOAT) { printf("%s:%d FIXME: E_LOC_CODE\n", __FILE__, __LINE__); exit(-1); } */
                /* Code label location, offset in IVal */
                g_getstatic (Flags | CF_CODE, Expr->Name, Expr->IVal);
                break;

            case E_LOC_STACK:
                /* printf("LoadExpr E_LOC_STACK (%s)\n", (TypeOf (Expr->Type) == CF_FLOAT) ? "float" : "integer"); */
                /* if (TypeOf (Expr->Type) == CF_FLOAT) { printf("%s:%d FIXME: E_LOC_STACK\n", __FILE__, __LINE__); exit(-1); } */
                /* Fetch value on the stack (with offset in IVal) */
                g_getlocal (Flags, Expr->IVal);
                break;

            case E_LOC_PRIMARY:
                /*printf("LoadExpr E_LOC_PRIMARY (%s)\n", (TypeOf (Expr->Type) == CF_FLOAT) ? "float" : "integer");*/
                /*if (TypeOf (Expr->Type) == CF_FLOAT) { printf("%s:%d FIXME: E_LOC_PRIMARY\n", __FILE__, __LINE__); exit(-1); }*/
                /* The primary register */
                if (TypeOf (Expr->Type) == CF_FLOAT) {
                    /* FIXME: float */
                    Flags |= CF_FLOAT;
                    if (Expr->V.FVal.V != 0.0f) {
                         g_inc (Flags | CF_CONST, FP_D_As32bitRaw(Expr->V.FVal));
                    }
                } else {
                    if (Expr->IVal != 0) {
                        /* We have an expression in the primary plus a constant
                        ** offset. Adjust the value in the primary accordingly.
                        */
                        g_inc (Flags | CF_CONST, Expr->IVal);

                        /* We might want to clear the offset, but we can't */
                    }
                }
                if (Flags & CF_TEST) {
                    g_test (Flags);
                }
                break;

            case E_LOC_EXPR:
                /*printf("LoadExpr E_LOC_EXPR (%s)\n", (TypeOf (Expr->Type) == CF_FLOAT) ? "float" : "integer");*/
                /*if (TypeOf (Expr->Type) == CF_FLOAT) { printf("%s:%d FIXME: E_LOC_EXPR\n", __FILE__, __LINE__); exit(-1); }*/
                /* Reference to address in primary with offset in IVal */
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
            /* We always need to do something with the low byte, so there is no opportunity
            ** for optimization by skipping it.
            */
            CHECK (Expr->Type->A.B.Offs < CHAR_BITS);

            if (ED_YetToTest (Expr)) {
                g_testbitfield (Flags, Expr->Type->A.B.Offs, Expr->Type->A.B.Width);
            } else {
                g_extractbitfield (Flags, BitFieldFullWidthFlags, IsSignSigned (Expr->Type),
                                   Expr->Type->A.B.Offs, Expr->Type->A.B.Width);
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
        if (ED_YetToTest (Expr)) {
            /* Yes, force a test */
            g_test (Flags);
            ED_TestDone (Expr);
        }
    }

}
