/*****************************************************************************/
/*                                                                           */
/*                                typeconv.c                                 */
/*                                                                           */
/*                          Handle type conversions                          */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2002-2008 Ullrich von Bassewitz                                       */
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



/* common */
#include "shift.h"

/* cc65 */
#include "codegen.h"
#include "datatype.h"
#include "declare.h"
#include "error.h"
#include "expr.h"
#include "loadexpr.h"
#include "scanner.h"
#include "typecmp.h"
#include "typeconv.h"



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



static void DoConversion (ExprDesc* Expr, const Type* NewType)
/* Emit code to convert the given expression to a new type. */
{
    Type*    OldType;
    unsigned OldSize;
    unsigned NewSize;


    /* Remember the old type */
    OldType = Expr->Type;

    /* If we're converting to void, we're done. Note: This does also cover a
    ** conversion void -> void.
    */
    if (IsTypeVoid (NewType)) {
        ED_MakeRVal (Expr);     /* Never an lvalue */
        goto ExitPoint;
    }

    /* Don't allow casts from void to something else. */
    if (IsTypeVoid (OldType)) {
        Error ("Cannot convert from `void' to something else");
        goto ExitPoint;
    }

    /* Get the sizes of the types. Since we've excluded void types, checking
    ** for known sizes makes sense here.
    */
    OldSize = CheckedSizeOf (OldType);
    NewSize = CheckedSizeOf (NewType);

    /* lvalue? */
    if (ED_IsLVal (Expr)) {

        /* We have an lvalue. If the new size is smaller than the old one,
        ** we don't need to do anything. The compiler will generate code
        ** to load only the portion of the value that is actually needed.
        ** This works only on a little endian architecture, but that's
        ** what we support.
        ** If both sizes are equal, do also leave the value alone.
        ** If the new size is larger, we must convert the value.
        */
        if (NewSize > OldSize) {
            /* Load the value into the primary */
            LoadExpr (CF_NONE, Expr);

            /* Emit typecast code */
            g_typecast (TypeOf (NewType), TypeOf (OldType) | CF_FORCECHAR);

            /* Value is now in primary and an rvalue */
            ED_MakeRValExpr (Expr);
        }

    } else if (ED_IsLocAbs (Expr)) {

        /* A cast of a constant numeric value to another type. Be sure
        ** to handle sign extension correctly.
        */

        /* Get the current and new size of the value */
        unsigned OldBits = OldSize * 8;
        unsigned NewBits = NewSize * 8;

        /* Check if the new datatype will have a smaller range. If it
        ** has a larger range, things are OK, since the value is
        ** internally already represented by a long.
        */
        if (NewBits <= OldBits) {

            /* Cut the value to the new size */
            Expr->IVal &= (0xFFFFFFFFUL >> (32 - NewBits));

            /* If the new type is signed, sign extend the value */
            if (IsSignSigned (NewType)) {
                if (Expr->IVal & (0x01UL << (NewBits-1))) {
                    /* Beware: Use the safe shift routine here. */
                    Expr->IVal |= shl_l (~0UL, NewBits);
                }
            }
        }

    } else {

        /* The value is not a constant. If the sizes of the types are
        ** not equal, add conversion code. Be sure to convert chars
        ** correctly.
        */
        if (OldSize != NewSize) {

            /* Load the value into the primary */
            LoadExpr (CF_NONE, Expr);

            /* Emit typecast code. */
            g_typecast (TypeOf (NewType), TypeOf (OldType) | CF_FORCECHAR);

            /* Value is now a rvalue in the primary */
            ED_MakeRValExpr (Expr);
        }
    }

ExitPoint:
    /* The expression has always the new type */
    ReplaceType (Expr, NewType);
}



void TypeConversion (ExprDesc* Expr, Type* NewType)
/* Do an automatic conversion of the given expression to the new type. Output
** warnings or errors where this automatic conversion is suspicious or
** impossible.
*/
{
#if 0
    /* Debugging */
    printf ("Expr:\n=======================================\n");
    PrintExprDesc (stdout, Expr);
    printf ("Type:\n=======================================\n");
    PrintType (stdout, NewType);
    printf ("\n");
    PrintRawType (stdout, NewType);
#endif

    /* First, do some type checking */
    if (IsTypeVoid (NewType) || IsTypeVoid (Expr->Type)) {
        /* If one of the sides are of type void, output a more apropriate
        ** error message.
        */
        Error ("Illegal type");
    }

    /* If Expr is a function, convert it to pointer to function */
    if (IsTypeFunc(Expr->Type)) {
        Expr->Type = PointerTo (Expr->Type);
    }

    /* If both types are equal, no conversion is needed */
    if (TypeCmp (Expr->Type, NewType) >= TC_EQUAL) {
        /* We're already done */
        return;
    }

    /* Check for conversion problems */
    if (IsClassInt (NewType)) {

        /* Handle conversions to int type */
        if (IsClassPtr (Expr->Type)) {
            /* Pointer -> int conversion. Convert array to pointer */
            if (IsTypeArray (Expr->Type)) {
                Expr->Type = ArrayToPtr (Expr->Type);
            }
            Warning ("Converting pointer to integer without a cast");
        } else if (!IsClassInt (Expr->Type) && !IsClassFloat (Expr->Type)) {
            Error ("Incompatible types");
        }

    } else if (IsClassFloat (NewType)) {

        if (!IsClassFloat (Expr->Type) && !IsClassInt (Expr->Type)) {
            Error ("Incompatible types");
        }

    } else if (IsClassPtr (NewType)) {

        /* Handle conversions to pointer type */
        if (IsClassPtr (Expr->Type)) {

            /* Convert array to pointer */
            if (IsTypeArray (Expr->Type)) {
                Expr->Type = ArrayToPtr (Expr->Type);
            }

            /* Pointer to pointer assignment is valid, if:
            **   - both point to the same types, or
            **   - the rhs pointer is a void pointer, or
            **   - the lhs pointer is a void pointer.
            */
            if (!IsTypeVoid (Indirect (NewType)) && !IsTypeVoid (Indirect (Expr->Type))) {
                /* Compare the types */
                switch (TypeCmp (NewType, Expr->Type)) {

                    case TC_INCOMPATIBLE:
                        Error ("Incompatible pointer types");
                        break;

                    case TC_QUAL_DIFF:
                        Error ("Pointer types differ in type qualifiers");
                        break;

                    default:
                        /* Ok */
                        break;
                }
            }

        } else if (IsClassInt (Expr->Type)) {
            /* Int to pointer assignment is valid only for constant zero */
            if (!ED_IsConstAbsInt (Expr) || Expr->IVal != 0) {
                Warning ("Converting integer to pointer without a cast");
            }
        } else {
            Error ("Incompatible types");
        }

    } else {

        /* Invalid automatic conversion */
        Error ("Incompatible types");

    }

    /* Do the actual conversion */
    DoConversion (Expr, NewType);
}



void TypeCast (ExprDesc* Expr)
/* Handle an explicit cast. */
{
    Type    NewType[MAXTYPELEN];

    /* Skip the left paren */
    NextToken ();

    /* Read the type */
    ParseType (NewType);

    /* Closing paren */
    ConsumeRParen ();

    /* Read the expression we have to cast */
    hie10 (Expr);

    /* Convert functions and arrays to "pointer to" object */
    Expr->Type = PtrConversion (Expr->Type);

    /* Convert the value. */
    DoConversion (Expr, NewType);
}
