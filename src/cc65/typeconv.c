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



void TypeCompatibilityDiagnostic (const Type* NewType, const Type* OldType, int IsError, const char* Msg)
/* Print error or warning message about type conversion with proper type names */
{
    StrBuf NewTypeName = STATIC_STRBUF_INITIALIZER;
    StrBuf OldTypeName = STATIC_STRBUF_INITIALIZER;
    GetFullTypeNameBuf (&NewTypeName, NewType);
    GetFullTypeNameBuf (&OldTypeName, OldType);
    if (IsError) {
        Error (Msg, SB_GetConstBuf (&NewTypeName), SB_GetConstBuf (&OldTypeName));
    } else {
        Warning (Msg, SB_GetConstBuf (&NewTypeName), SB_GetConstBuf (&OldTypeName));
    }
    SB_Done (&OldTypeName);
    SB_Done (&NewTypeName);
}



static void DoConversion (ExprDesc* Expr, const Type* NewType)
/* Emit code to convert the given expression to a new type. */
{
    Type*    OldType;
    unsigned OldBits;
    unsigned NewBits;


    /* Remember the old type */
    OldType = Expr->Type;

    /* If we're converting to void, we're done. Note: This does also cover a
    ** conversion void -> void.
    */
    if (IsTypeVoid (NewType)) {
        ED_MarkExprAsRVal (Expr);     /* Never an lvalue */
        goto ExitPoint;
    }

    /* Don't allow casts from void to something else. */
    if (IsTypeVoid (OldType)) {
        Error ("Cannot convert from 'void' to something else");
        goto ExitPoint;
    }

    /* Get the sizes of the types. Since we've excluded void types, checking
    ** for known sizes makes sense here.
    */
    if (ED_IsBitField (Expr)) {
        OldBits = Expr->BitWidth;
    } else {
        OldBits = CheckedSizeOf (OldType) * CHAR_BITS;
    }
    NewBits = CheckedSizeOf (NewType) * CHAR_BITS;

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
        if (NewBits > OldBits) {
            /* Load the value into the primary */
            LoadExpr (CF_NONE, Expr);

            /* Emit typecast code */
            g_typecast (TypeOf (NewType), TypeOf (OldType));

            /* Value is now in primary and an rvalue */
            ED_FinalizeRValLoad (Expr);
        }

    } else if (ED_IsConstAbs (Expr)) {

        /* A cast of a constant numeric value to another type. Be sure
        ** to handle sign extension correctly.
        */

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

        /* Do the integer constant <-> absolute address conversion if necessary */
        if (IsClassPtr (NewType)) {
            Expr->Flags &= ~E_LOC_NONE;
            Expr->Flags |= E_LOC_ABS | E_ADDRESS_OF;
        } else if (IsClassInt (NewType)) {
            Expr->Flags &= ~(E_LOC_ABS | E_ADDRESS_OF);
            Expr->Flags |= E_LOC_NONE;
        }

    } else {

        /* The value is not a constant. If the sizes of the types are
        ** not equal, add conversion code. Be sure to convert chars
        ** correctly.
        */
        if (OldBits != NewBits) {

            /* Load the value into the primary */
            LoadExpr (CF_NONE, Expr);

            /* Emit typecast code. */
            g_typecast (TypeOf (NewType), TypeOf (OldType));

            /* Value is now an rvalue in the primary */
            ED_FinalizeRValLoad (Expr);
        }
    }

ExitPoint:
    /* The expression has always the new type */
    ReplaceType (Expr, NewType);

    /* Bit-fields are converted to integers */
    ED_DisBitField (Expr);
}



void TypeConversion (ExprDesc* Expr, const Type* NewType)
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
    int HasWarning  = 0;
    int HasError    = 0;
    const char* Msg = 0;
    const Type* OldType = Expr->Type;


    /* If one of the sides is of type void, it is an error */
    if (IsTypeVoid (NewType) || IsTypeVoid (OldType)) {
        HasError = 1;
    }

    /* If both types are strictly compatible, no conversion is needed */
    if (TypeCmp (NewType, OldType) >= TC_STRICT_COMPATIBLE) {
        /* We're already done */
        return;
    }

    /* If Expr is an array or a function, convert it to a pointer */
    Expr->Type = PtrConversion (Expr->Type);

    /* If we have changed the type, check again for strictly compatibility */
    if (Expr->Type != OldType &&
        TypeCmp (NewType, Expr->Type) >= TC_STRICT_COMPATIBLE) {
        /* We're already done */
        return;
    }

    /* Check for conversion problems */
    if (IsClassInt (NewType)) {

        /* Handle conversions to int type */
        if (IsClassPtr (Expr->Type)) {
            Warning ("Converting pointer to integer without a cast");
        } else if (!IsClassInt (Expr->Type) && !IsClassFloat (Expr->Type)) {
            HasError = 1;
        }
    } else if (IsClassFloat (NewType)) {
        if (!IsClassFloat (Expr->Type) && !IsClassInt (Expr->Type)) {
            HasError = 1;
        }
    } else if (IsClassPtr (NewType)) {

        /* Handle conversions to pointer type */
        if (IsClassPtr (Expr->Type)) {

            /* Pointer to pointer assignment is valid, if:
            **   - both point to the same types, or
            **   - the rhs pointer is a void pointer, or
            **   - the lhs pointer is a void pointer.
            */
            if (!IsTypeVoid (IndirectConst (NewType)) && !IsTypeVoid (Indirect (Expr->Type))) {
                /* Compare the types */
                switch (TypeCmp (NewType, Expr->Type)) {

                case TC_INCOMPATIBLE:
                    HasWarning = 1;
                    Msg = "Incompatible pointer assignment to '%s' from '%s'";
                    /* Use the pointer type in the diagnostic */
                    OldType = Expr->Type;
                    break;

                case TC_QUAL_DIFF:
                    HasWarning = 1;
                    Msg = "Pointer assignment to '%s' from '%s' discards qualifiers";
                    /* Use the pointer type in the diagnostic */
                    OldType = Expr->Type;
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
            HasError = 1;
        }

    } else {
         /* Invalid automatic conversion */
         HasError = 1;
    }

    if (Msg == 0) {
        Msg = "Converting to '%s' from '%s'";
    }

    if (HasError) {
        TypeCompatibilityDiagnostic (NewType, OldType, 1, Msg);
    } else {
        if (HasWarning) {
            TypeCompatibilityDiagnostic (NewType, OldType, 0, Msg);
        }

        /* Both types must be complete */
        if (!IsIncompleteESUType (NewType) && !IsIncompleteESUType (Expr->Type)) {
            /* Do the actual conversion */
            DoConversion (Expr, NewType);
        } else {
            /* We should have already generated error elsewhere so that we
            ** could just silently fail here to avoid excess errors, but to
            ** be safe, we must ensure that we do have errors.
            */
            if (IsIncompleteESUType (NewType)) {
                Error ("Conversion to incomplete type '%s'", GetFullTypeName (NewType));
            } else {
                Error ("Conversion from incomplete type '%s'", GetFullTypeName (Expr->Type));
            }
        }
    }
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

    /* Only allow casts to arithmetic, pointer or void types */
    if (IsCastType (NewType)) {
        if (!IsIncompleteESUType (NewType)) {
            /* Convert functions and arrays to "pointer to" object */
            Expr->Type = PtrConversion (Expr->Type);

            if (TypeCmp (NewType, Expr->Type) >= TC_QUAL_DIFF) {
                /* If the new type only differs in qualifiers, just use it to
                ** replace the old one.
                */
                ReplaceType (Expr, NewType);
            } else if (IsCastType (Expr->Type)) {
                /* Convert the value. The rsult has always the new type */
                DoConversion (Expr, NewType);
            } else {
                TypeCompatibilityDiagnostic (NewType, Expr->Type, 1,
                    "Cast to incompatible type '%s' from '%s'");
            }
        } else {
            Error ("Cast to incomplete type '%s'",
                   GetFullTypeName (NewType));
        }
    } else {
        Error ("Arithmetic or pointer type expected but %s is used",
               GetBasicTypeName (NewType));
    }

    /* The result is always an rvalue */
    ED_MarkExprAsRVal (Expr);
}
