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
#include "funcdesc.h"
#include "loadexpr.h"
#include "typecmp.h"
#include "typeconv.h"



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



static void DoConversion (ExprDesc* Expr, const Type* NewType, int Explicit)
/* Emit code to convert the given expression to a new type. */
{
    const Type* OldType;
    unsigned    OldBits;
    unsigned    NewBits;


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
    if (IsTypeBitField (OldType)) {
        OldBits = OldType->A.B.Width;
    } else {
        OldBits = CheckedSizeOf (OldType) * CHAR_BITS;
    }

    /* If the new type is a bit-field, we use its underlying type instead */
    if (IsTypeBitField (NewType)) {
        NewType = GetUnderlyingType (NewType);
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
            long OldVal = Expr->IVal;

            /* Cut the value to the new size */
            Expr->IVal &= (0xFFFFFFFFUL >> (32 - NewBits));

            /* If the new type is signed, sign extend the value */
            if (IsSignSigned (NewType)) {
                if (Expr->IVal & (0x01UL << (NewBits-1))) {
                    /* Beware: Use the safe shift routine here. */
                    Expr->IVal |= shl_l (~0UL, NewBits);
                }
            }

            if ((OldVal != Expr->IVal) && IS_Get (&WarnConstOverflow) && !Explicit) {
                Warning ("Implicit conversion of constant overflows %d-bit destination", NewBits);
            }
        }

        /* Do the integer constant <-> absolute address conversion if necessary */
        if (IsClassPtr (NewType)) {
            Expr->Flags &= ~E_MASK_LOC;
            Expr->Flags |= E_LOC_ABS | E_ADDRESS_OF;
        } else if (IsClassInt (NewType)) {
            Expr->Flags &= ~(E_MASK_LOC | E_ADDRESS_OF);
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
    typecmp_t Result    = TYPECMP_INITIALIZER;
    int HasError        = 0;
    const char* Msg     = 0;
    const Type* OldType = Expr->Type;


    /* If one of the sides is of type void, it is an error */
    if (IsTypeVoid (NewType) || IsTypeVoid (OldType)) {
        HasError = 1;
    }

    /* If both types are the same, no conversion is needed */
    Result = TypeCmp (NewType, OldType);
    if (Result.C < TC_IDENTICAL && (IsTypeArray (OldType) || IsTypeFunc (OldType))) {
        /* If Expr is an array or a function, convert it to a pointer */
        Expr->Type = PtrConversion (Expr->Type);
        /* Recompare */
        Result = TypeCmp (NewType, Expr->Type);
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

            /* Implicit pointer-to-pointer conversion is valid, if:
            **   - both point to the same types, or
            **   - the rhs pointer is a void pointer, or
            **   - the lhs pointer is a void pointer.
            ** Note: We additionally allow converting function pointers to and from
            **       void pointers, just with warnings.
            */
            if (Result.C == TC_PTR_SIGN_DIFF) {
                /* Specific warning for pointer signedness difference */
                if (IS_Get (&WarnPointerSign)) {
                    TypeCompatibilityDiagnostic (NewType, Expr->Type,
                        0, "Pointer conversion to '%s' from '%s' changes pointer signedness");
                }
            } else if ((Result.C <= TC_PTR_INCOMPATIBLE ||
                 (Result.F & TCF_INCOMPATIBLE_QUAL) != 0)) {
                /* Incompatible pointee types or qualifiers */
                if (IS_Get (&WarnPointerTypes)) {
                    TypeCompatibilityDiagnostic (NewType, Expr->Type,
                        0, "Incompatible pointer conversion to '%s' from '%s'");
                }
            }

            if ((Result.F & TCF_PTR_QUAL_DIFF) != 0) {
                /* Discarding qualifiers is a bad thing and we always warn */
                TypeCompatibilityDiagnostic (NewType, Expr->Type,
                    0, "Pointer conversion to '%s' from '%s' discards qualifiers");
            }

        } else if (IsClassInt (Expr->Type)) {
            /* Int to pointer conversion is valid only for constant zero */
            if (!ED_IsConstAbsInt (Expr) || Expr->IVal != 0) {
                Warning ("Converting integer to pointer without a cast");
            }
        } else {
            HasError = 1;
        }

    } else if (Result.C < TC_IDENTICAL) {
         /* Invalid automatic conversion */
         HasError = 1;
    }

    /* Set default diagnostic message */
    if (Msg == 0) {
        Msg = "Converting to '%s' from '%s'";
    }

    if (HasError) {
        TypeCompatibilityDiagnostic (NewType, OldType, 1, Msg);
    } else {
        /* Both types must be complete */
        if (!IsIncompleteESUType (NewType) && !IsIncompleteESUType (Expr->Type)) {
            /* Do the actual conversion */
            DoConversion (Expr, NewType, 0);
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

            if (TypeCmp (NewType, Expr->Type).C >= TC_PTR_INCOMPATIBLE) {
                /* If the new type has the same underlying presentation, just
                ** use it to replace the old one.
                */
                ReplaceType (Expr, NewType);
            } else if (IsCastType (Expr->Type)) {
                /* Convert the value. The result has always the new type */
                DoConversion (Expr, NewType, 1);
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

    /* If the new type is void, the cast expression can have no effects */
    if (IsTypeVoid (NewType)) {
        Expr->Flags |= E_EVAL_MAYBE_UNUSED;
    }

    /* The result is always an rvalue */
    ED_MarkExprAsRVal (Expr);
}



static void ComposeFuncParamList (const FuncDesc* F1, const FuncDesc* F2)
/* Compose two function symbol tables regarding function parameters into F1 */
{
    /* Get the symbol tables */
    const SymTable* Tab1 = F1->SymTab;
    const SymTable* Tab2 = F2->SymTab;

    /* Compose the parameter lists */
    SymEntry* Sym1 = Tab1->SymHead;
    SymEntry* Sym2 = Tab2->SymHead;

    /* Sanity check */
    CHECK ((F1->Flags & FD_EMPTY) == 0 && (F2->Flags & FD_EMPTY) == 0);

    /* Compose the fields */
    while (Sym1 && (Sym1->Flags & SC_PARAM) && Sym2 && (Sym2->Flags & SC_PARAM)) {

        /* Get the symbol types */
        const Type* Type1 = Sym1->Type;
        const Type* Type2 = Sym2->Type;

        /* If either of both functions is old style, apply the default
        ** promotions to the parameter type.
        */
        if (F1->Flags & FD_OLDSTYLE) {
            if (IsClassInt (Type1)) {
                Type1 = IntPromotion (Type1);
            }
        }
        if (F2->Flags & FD_OLDSTYLE) {
            if (IsClassInt (Type2)) {
                Type2 = IntPromotion (Type2);
            }
        }

        /* When we compose two function parameter lists with any FD_OLDSTYLE
        ** flags set, we are either refining the declaration of the function
        ** with its definition seen, or determining the result type of a
        ** ternary operation. In either case, we can just replace the types
        ** with the promoted ones since the original types of the parameters
        ** only matters inside the function definition.
        */
        if (Type1 != Sym1->Type) {
            Sym1->Type = TypeDup (Type1);
        }

        /* Compose this field */
        TypeComposition (Sym1->Type, Type2);

        /* Get the pointers to the next fields */
        Sym1 = Sym1->NextSym;
        Sym2 = Sym2->NextSym;
    }
}



void TypeComposition (Type* lhs, const Type* rhs)
/* Recursively compose two types into lhs. The two types must have compatible
** type or this fails with a critical check.
*/
{
    FuncDesc*   F1;
    FuncDesc*   F2;
    long LeftCount, RightCount;

    /* Compose two types */
    while (lhs->C != T_END) {

        /* Check if the end of the type string is reached */
        if (rhs->C == T_END) {
            break;
        }

        /* Check for sanity */
        CHECK (GetUnqualTypeCode (lhs) == GetUnqualTypeCode (rhs));

        /* Check for special type elements */
        if (IsTypeFunc (lhs)) {
            /* Compose the function descriptors */
            F1 = GetFuncDesc (lhs);
            F2 = GetFuncDesc (rhs);

            /* If F1 has an empty parameter list (which does also mean, it is
            ** not a function definition, because the flag is reset in this
            ** case), its declaration is replaced by the other declaration. If
            ** neither of the parameter lists is empty, we have to compose them
            ** as well as other attributes.
            */
            if ((F1->Flags & FD_EMPTY) == FD_EMPTY) {
                if ((F2->Flags & FD_EMPTY) == 0) {
                    /* Copy the parameters and flags */
                    TypeCopy (lhs, rhs);
                    F1->Flags = F2->Flags;
                }
            } else if ((F2->Flags & FD_EMPTY) == 0) {
                /* Compose the parameter lists */
                ComposeFuncParamList (F1, F2);
                /* Prefer non-old-style */
                if ((F2->Flags & FD_OLDSTYLE) == 0) {
                    F1->Flags &= ~FD_OLDSTYLE;
                }
            }
        } else if (IsTypeArray (lhs)) {
            /* Check member count */
            LeftCount  = GetElementCount (lhs);
            RightCount = GetElementCount (rhs);

            /* Set composite type if it is requested */
            if (LeftCount != UNSPECIFIED) {
                SetElementCount (lhs, LeftCount);
            } else if (RightCount != UNSPECIFIED) {
                SetElementCount (lhs, RightCount);
            }
        } else {
            /* Combine the qualifiers */
            if (IsClassPtr (lhs)) {
                ++lhs;
                ++rhs;
                lhs->C |= GetQualifier (rhs);
            }
        }

        /* Next type string element */
        ++lhs;
        ++rhs;
    }

    return;
}



FuncDesc* RefineFuncDesc (Type* OldType, const Type* NewType)
/* Refine the existing function descriptor with a new one */
{
    FuncDesc* Old = GetFuncDesc (OldType);
    FuncDesc* New = GetFuncDesc (NewType);

    CHECK (Old != 0 && New != 0);

    if ((New->Flags & FD_EMPTY) == 0) {
        if ((Old->Flags & FD_EMPTY) == 0) {
            TypeComposition (OldType, NewType);
        } else {
            TypeCopy (OldType, NewType);
            Old->Flags &= ~FD_EMPTY;
        }
    }

    return Old;
}
