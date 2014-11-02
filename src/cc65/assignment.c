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



void Assignment (ExprDesc* Expr)
/* Parse an assignment */
{
    ExprDesc Expr2;
    Type* ltype = Expr->Type;


    /* We must have an lvalue for an assignment */
    if (ED_IsRVal (Expr)) {
        Error ("Invalid lvalue in assignment");
    }

    /* Check for assignment to const */
    if (IsQualConst (ltype)) {
        Error ("Assignment to const");
    }

    /* Skip the '=' token */
    NextToken ();

    /* cc65 does not have full support for handling structs by value. Since
    ** assigning structs is one of the more useful operations from this
    ** family, allow it here.
    */
    if (IsClassStruct (ltype)) {

        /* Get the size of the left hand side. */
        unsigned Size = SizeOf (ltype);

        /* If the size is that of a basic type (char, int, long), we will copy
        ** the struct using the primary register, otherwise we use memcpy. In
        ** the former case, push the address only if really needed.
        */
        int UseReg = 1;
        Type* stype;
        switch (Size) {
            case SIZEOF_CHAR:   stype = type_uchar;             break;
            case SIZEOF_INT:    stype = type_uint;              break;
            case SIZEOF_LONG:   stype = type_ulong;             break;
            default:            stype = ltype; UseReg = 0;      break;
        }
        if (UseReg) {
            PushAddr (Expr);
        } else {
            ED_MakeRVal (Expr);
            LoadExpr (CF_NONE, Expr);
            g_push (CF_PTR | CF_UNSIGNED, 0);
        }

        /* Get the expression on the right of the '=' into the primary */
        hie1 (&Expr2);

        /* Check for equality of the structs */
        if (TypeCmp (ltype, Expr2.Type) < TC_STRICT_COMPATIBLE) {
            Error ("Incompatible types");
        }

        /* Check if the right hand side is an lvalue */
        if (ED_IsLVal (&Expr2)) {
            /* We have an lvalue. Do we copy using the primary? */
            if (UseReg) {
                /* Just use the replacement type */
                Expr2.Type = stype;

                /* Load the value into the primary */
                LoadExpr (CF_FORCECHAR, &Expr2);

                /* Store it into the new location */
                Store (Expr, stype);

            } else {

                /* We will use memcpy. Push the address of the rhs */
                ED_MakeRVal (&Expr2);
                LoadExpr (CF_NONE, &Expr2);

                /* Push the address (or whatever is in ax in case of errors) */
                g_push (CF_PTR | CF_UNSIGNED, 0);

                /* Load the size of the struct into the primary */
                g_getimmed (CF_INT | CF_UNSIGNED | CF_CONST, CheckedSizeOf (ltype), 0);

                /* Call the memcpy function */
                g_call (CF_FIXARGC, Func_memcpy, 4);
            }

        } else {

            /* We have an rvalue. This can only happen if a function returns
            ** a struct, since there is no other way to generate an expression
            ** that has a struct as an rvalue result. We allow only 1, 2, and 4
            ** byte sized structs, and do direct assignment.
            */
            if (UseReg) {
                /* Do the store */
                Store (Expr, stype);
            } else {
                /* Print a diagnostic */
                Error ("Structs of this size are not supported");
                /* Adjust the stack so we won't run in an internal error later */
                pop (CF_PTR);
            }

        }

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
    ED_MakeRValExpr (Expr);
}
