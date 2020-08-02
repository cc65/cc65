/*****************************************************************************/
/*                                                                           */
/*                               assignment.c                                */
/*                                                                           */
/*                             Parse assignments                             */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2002-2009, Ullrich von Bassewitz                                      */
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
#include "asmcode.h"
#include "assignment.h"
#include "codegen.h"
#include "datatype.h"
#include "error.h"
#include "expr.h"
#include "loadexpr.h"
#include "scanner.h"
#include "stdnames.h"
#include "typecmp.h"
#include "typeconv.h"



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



static int CopyStruct (ExprDesc* LExpr, ExprDesc* RExpr)
/* Copy the struct/union represented by RExpr to the one represented by LExpr */
{
    /* If the size is that of a basic type (char, int, long), we will copy
    ** the struct using the primary register, otherwise we use memcpy. In
    ** the former case, push the address only if really needed.
    */
    const Type* ltype  = LExpr->Type;
    const Type* stype  = GetStructReplacementType (ltype);
    int         UseReg = (stype != ltype);

    if (UseReg) {
        PushAddr (LExpr);
    } else {
        ED_MarkExprAsRVal (LExpr);
        LoadExpr (CF_NONE, LExpr);
        g_push (CF_PTR | CF_UNSIGNED, 0);
    }

    /* Get the expression on the right of the '=' into the primary */
    hie1 (RExpr);

    /* Check for equality of the structs */
    if (TypeCmp (ltype, RExpr->Type) < TC_STRICT_COMPATIBLE) {
        TypeCompatibilityDiagnostic (ltype, RExpr->Type, 1,
            "Incompatible types in assignment to '%s' from '%s'");
    }

    /* Do we copy using the primary? */
    if (UseReg) {

        /* Check if the right hand side is an lvalue */
        if (ED_IsLVal (RExpr)) {
            /* Just load the value into the primary as the replacement type. */
            LoadExpr (TypeOf (stype) | CF_FORCECHAR, RExpr);
        }

        /* Store it into the new location */
        Store (LExpr, stype);

    } else {

        /* Check if the right hand side is an lvalue */
        if (ED_IsLVal (RExpr)) {
            /* We will use memcpy. Push the address of the rhs */
            ED_MarkExprAsRVal (RExpr);
            LoadExpr (CF_NONE, RExpr);
        }

        /* Push the address (or whatever is in ax in case of errors) */
        g_push (CF_PTR | CF_UNSIGNED, 0);

        /* Load the size of the struct or union into the primary */
        g_getimmed (CF_INT | CF_UNSIGNED | CF_CONST, CheckedSizeOf (ltype), 0);

        /* Call the memcpy function */
        g_call (CF_FIXARGC, Func_memcpy, 4);
    }

    return 0;
}



void Assignment (ExprDesc* Expr)
/* Parse an assignment */
{
    ExprDesc Expr2;
    Type* ltype = Expr->Type;


    /* We must have an lvalue for an assignment */
    if (ED_IsRVal (Expr)) {
        if (IsTypeArray (Expr->Type)) {
            Error ("Array type '%s' is not assignable", GetFullTypeName (Expr->Type));
        } else if (IsTypeFunc (Expr->Type)) {
            Error ("Function type '%s' is not assignable", GetFullTypeName (Expr->Type));
        } else {
            Error ("Assignment to rvalue");
        }
    }

    /* Check for assignment to const */
    if (IsQualConst (ltype)) {
        Error ("Assignment to const");
    }

    /* Skip the '=' token */
    NextToken ();

    /* cc65 does not have full support for handling structs or unions. Since
    ** assigning structs is one of the more useful operations from this family,
    ** allow it here.
    ** Note: IsClassStruct() is also true for union types. 
    */
    if (IsClassStruct (ltype)) {
        /* Copy the struct or union by value */
        CopyStruct (Expr, &Expr2);

    } else if (ED_IsBitField (Expr)) {

        CodeMark AndPos;
        CodeMark PushPos;

        unsigned Mask;
        unsigned Flags;

        /* If the bit-field fits within one byte, do the following operations
        ** with bytes.
        */
        if (Expr->BitOffs / CHAR_BITS == (Expr->BitOffs + Expr->BitWidth - 1) / CHAR_BITS) {
            Expr->Type = type_uchar;
        }

        /* Determine code generator flags */
        Flags = TypeOf (Expr->Type);

        /* Assignment to a bit field. Get the address on stack for the store. */
        PushAddr (Expr);

        /* Load the value from the location */
        Expr->Flags &= ~E_BITFIELD;
        LoadExpr (CF_NONE, Expr);

        /* Mask unwanted bits */
        Mask = (0x0001U << Expr->BitWidth) - 1U;
        GetCodePos (&AndPos);
        g_and (Flags | CF_CONST, ~(Mask << Expr->BitOffs));

        /* Push it on stack */
        GetCodePos (&PushPos);
        g_push (Flags, 0);

        /* Read the expression on the right side of the '=' */
        MarkedExprWithCheck (hie1, &Expr2);

        /* Do type conversion if necessary. Beware: Do not use char type
        ** here!
        */
        TypeConversion (&Expr2, ltype);

        /* Special treatment if the value is constant. */
        /* Beware: Expr2 may contain side effects, so there must not be
        ** code generated for Expr2.
        */
        if (ED_IsConstAbsInt (&Expr2) && ED_CodeRangeIsEmpty (&Expr2)) {

            /* Get the value and apply the mask */
            unsigned Val = (unsigned) (Expr2.IVal & Mask);

            /* Since we will do the OR with a constant, we can remove the push */
            RemoveCode (&PushPos);

            /* If the value is equal to the mask now, all bits are one, and we
            ** can remove the mask operation from above.
            */
            if (Val == Mask) {
                RemoveCode (&AndPos);
            }

            /* Generate the or operation */
            g_or (Flags | CF_CONST, Val << Expr->BitOffs);

        } else {

            /* If necessary, load the value into the primary register */
            LoadExpr (CF_NONE, &Expr2);

            /* Apply the mask */
            g_and (Flags | CF_CONST, Mask);

            /* Shift it into the right position */
            g_asl (Flags | CF_CONST, Expr->BitOffs);

            /* Or both values */
            g_or (Flags, 0);
        }

        /* Generate a store instruction */
        Store (Expr, 0);

        /* Restore the expression type */
        Expr->Type = ltype;

    } else {

        /* Get the address on stack if needed */
        PushAddr (Expr);

        /* Read the expression on the right side of the '=' */
        hie1 (&Expr2);

        /* Do type conversion if necessary */
        TypeConversion (&Expr2, ltype);

        /* If necessary, load the value into the primary register */
        LoadExpr (CF_NONE, &Expr2);

        /* Generate a store instruction */
        Store (Expr, 0);

    }

    /* Value is still in primary and not an lvalue */
    ED_FinalizeRValLoad (Expr);
}
