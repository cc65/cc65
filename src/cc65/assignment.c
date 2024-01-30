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
#include "stackptr.h"
#include "stdnames.h"
#include "symentry.h"
#include "typecmp.h"
#include "typeconv.h"



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



static void CopyStruct (ExprDesc* LExpr, ExprDesc* RExpr)
/* Copy the struct/union represented by RExpr to the one represented by LExpr */
{
    /* If the size is that of a basic type (char, int, long), we will copy
    ** the struct using the primary register, otherwise we will use memcpy.
    */
    const Type* ltype  = LExpr->Type;
    const Type* stype  = GetStructReplacementType (ltype);
    int         UseReg = (stype != ltype);

    if (UseReg) {
        /* Back up the address of lhs only if it is in the primary */
        PushAddr (LExpr);
    } else {
        /* Push the address of lhs as the destination of memcpy */
        ED_AddrExpr (LExpr);
        LoadExpr (CF_NONE, LExpr);
        g_push (CF_PTR | CF_UNSIGNED, 0);
    }

    /* Get the expression on the right of the '=' */
    hie1 (RExpr);

    /* Check for equality of the structs/unions */
    if (TypeCmp (ltype, RExpr->Type).C < TC_STRICT_COMPATIBLE) {
        TypeCompatibilityDiagnostic (ltype, RExpr->Type, 1,
            "Incompatible types in assignment to '%s' from '%s'");
    } else if (SymHasConstMember (ltype->A.S)) {
        Error ("Assignment to read only variable");
    }

    /* Do we copy the value directly using the primary? */
    if (UseReg) {

        /* Check if the value of the rhs is not in the primary yet */
        if (!ED_IsLocPrimary (RExpr)) {
            /* Just load the value into the primary as the replacement type. */
            LoadExpr (CG_TypeOf (stype) | CF_FORCECHAR, RExpr);
        }

        /* Store it into the location referred in the primary */
        Store (LExpr, stype);

        /* Value is in primary as an rvalue */
        ED_FinalizeRValLoad (LExpr);

    } else {

        /* The rhs cannot happen to be loaded in the primary as it is too big */
        if (!ED_IsLocExpr (RExpr)) {
            ED_AddrExpr (RExpr);
            LoadExpr (CF_NONE, RExpr);
        }

        /* Push the address of the rhs as the source of memcpy */
        g_push (CF_PTR | CF_UNSIGNED, 0);

        /* Load the size of the struct or union into the primary */
        g_getimmed (CF_INT | CF_UNSIGNED | CF_CONST, SizeOf (ltype), 0);

        /* Call the memcpy function */
        g_call (CF_FIXARGC, Func_memcpy, 4);

        /* The result is an rvalue referenced in the primary */
        ED_FinalizeRValLoad (LExpr);

        /* Restore the indirection level of lhs */
        ED_IndExpr (LExpr);
    }

    /* Clear the tested flag set during loading. This is not neccessary
    ** currently (and probably ever) as a struct/union cannot be converted
    ** to a boolean in C, but there is no harm to be future-proof.
    */
    ED_MarkAsUntested (LExpr);
}



void DoIncDecBitField (ExprDesc* Expr, long Val, unsigned KeepResult)
/* Process inc/dec for bit-field */
{
    int         AddrSP;
    unsigned    Flags;                  /* Internal codegen flags */
    unsigned    Mask;
    unsigned    ChunkFlags;
    const Type* ChunkType;

    /* Determine the type to operate on the whole byte chunk containing the bit-field */
    ChunkType = GetBitFieldChunkType (Expr->Type);

    /* Determine code generator flags */
    Flags      = CG_TypeOf (Expr->Type) | CF_FORCECHAR;
    ChunkFlags = CG_TypeOf (ChunkType);
    if ((ChunkFlags & CF_TYPEMASK) == CF_CHAR) {
        ChunkFlags |= CF_FORCECHAR;
    }

    /* Get the address on stack for the store */
    PushAddr (Expr);

    /* We may need the pushed address later */
    AddrSP = StackPtr;

    /* Get bit mask to limit the range of the value */
    Mask = (0x0001U << Expr->Type->A.B.Width) - 1U;

    /* Fetch the lhs into the primary register if needed */
    LoadExpr (CF_NONE, Expr);

    /* Handle for add and sub */
    if (Val > 0) {
        g_inc (Flags | CF_CONST, Val);
    } else if (Val < 0) {
        g_dec (Flags | CF_CONST, -Val);
    }

    /* Apply the mask */
    g_and (Flags | CF_CONST, Mask);

    /* Do integral promotion without sign-extension if needed */
    g_typecast (ChunkFlags | CF_UNSIGNED, Flags);

    /* Shift it into the right position */
    g_asl (ChunkFlags | CF_CONST, Expr->Type->A.B.Offs);

    /* Push the interim result on stack */
    g_push (ChunkFlags & ~CF_FORCECHAR, 0);

    /* If the original lhs was using the primary, it is now accessible only via
    ** the pushed address. Reload that address.
    */
    if (ED_IsLocPrimaryOrExpr (Expr)) {
        g_getlocal (CF_PTR, AddrSP);
    }

    /* Load the whole data chunk containing the bits to be changed */
    LoadExpr (ChunkFlags, Expr);

    if (KeepResult == OA_NEED_OLD) {
        /* Save the original expression value */
        g_save (ChunkFlags | CF_FORCECHAR);
    }

    /* Get the bits that are not to be affected */
    g_and (ChunkFlags | CF_CONST, ~(Mask << Expr->Type->A.B.Offs));

    /* Restore the bits that are not to be affected */
    g_or (ChunkFlags & ~CF_FORCECHAR, 0);

    /* Store the whole data chunk containing the changed bits back */
    Store (Expr, ChunkType);

    if (KeepResult == OA_NEED_OLD) {
        /* Restore the original expression value */
        g_restore (ChunkFlags | CF_FORCECHAR);
    }
}



static void OpAssignBitField (const GenDesc* Gen, ExprDesc* Expr, const char* Op)
/* Parse an "=" (if 'Gen' is 0) or "op=" operation for bit-field lhs */
{
    ExprDesc Expr2;
    CodeMark PushPos;
    int      AddrSP;
    unsigned Mask;
    unsigned Flags;
    unsigned ChunkFlags;
    const Type* ChunkType;

    ED_Init (&Expr2);
    Expr2.Flags |= Expr->Flags & E_MASK_KEEP_SUBEXPR;

    /* Determine the type to operate on the whole byte chunk containing the bit-field */
    ChunkType = GetBitFieldChunkType (Expr->Type);

    /* Determine code generator flags */
    Flags      = CG_TypeOf (Expr->Type) | CF_FORCECHAR;
    ChunkFlags = CG_TypeOf (ChunkType);
    if ((ChunkFlags & CF_TYPEMASK) == CF_CHAR) {
        ChunkFlags |= CF_FORCECHAR;
    }

    /* Get the address on stack for the store */
    PushAddr (Expr);

    /* We may need the pushed address later */
    AddrSP = StackPtr;

    /* Get bit mask to limit the range of the value */
    Mask = (0x0001U << Expr->Type->A.B.Width) - 1U;

    if (Gen != 0) {

        /* Fetch the lhs into the primary register if needed */
        LoadExpr (CF_NONE, Expr);

        /* Backup them on stack */
        GetCodePos (&PushPos);
        g_push (Flags & ~CF_FORCECHAR, 0);

    }

    /* Read the expression on the right side of the '=' or 'op=' */
    MarkedExprWithCheck (hie1, &Expr2);

    /* The rhs must be an integer (or a float, but we don't support that yet */
    if (!IsClassInt (Expr2.Type)) {
        Error ("Invalid right operand for binary operator '%s'", Op);
        /* Continue. Wrong code will be generated, but the compiler won't
        ** break, so this is the best error recovery.
        */
    }

    /* Special treatment if the value is constant.
    ** Beware: Expr2 may contain side effects, so there must not be
    ** code generated for Expr2.
    */
    if (ED_IsConstAbsInt (&Expr2) && ED_CodeRangeIsEmpty (&Expr2)) {

        if (Gen == 0) {

            /* Get the value and apply the mask */
            unsigned Val = (unsigned)(Expr2.IVal & Mask);

            /* Load the whole data chunk containing the bits to be changed */
            LoadExpr (ChunkFlags, Expr);

            /* If the value is equal to the mask now, all bits are one, and we
            ** can skip the mask operation.
            */
            if (Val != Mask) {
                /* Get the bits that are not to be affected */
                g_and (ChunkFlags | CF_CONST, ~(Mask << Expr->Type->A.B.Offs));
            }

            /* Restore the bits that are not to be affected */
            g_or (ChunkFlags | CF_CONST, Val << Expr->Type->A.B.Offs);

            /* Store the whole data chunk containing the changed bits back */
            Store (Expr, ChunkType);

            /* Done */
            goto Done;

        } else {

            /* Since we will operate with a constant, we can remove the push if
            ** the generator has the NOPUSH flag set.
            */
            if (Gen->Flags & GEN_NOPUSH) {
                RemoveCode (&PushPos);
            }

            /* Special handling for add and sub - some sort of a hack, but short code */
            if (Gen->Func == g_add) {
                g_inc (Flags | CF_CONST, Expr2.IVal);
            } else if (Gen->Func == g_sub) {
                g_dec (Flags | CF_CONST, Expr2.IVal);
            } else {
                if (!ED_IsUneval (Expr)) {
                    if (Expr2.IVal == 0) {
                        /* Check for div by zero/mod by zero */
                        if (Gen->Func == g_div) {
                            Warning ("Division by zero");
                        } else if (Gen->Func == g_mod) {
                            Warning ("Modulo operation with zero");
                        }
                    } else if (Gen->Func == g_asl || Gen->Func == g_asr) {
                        const Type* CalType  = IntPromotion (Expr->Type);
                        unsigned    ExprBits = BitSizeOf (CalType);

                        /* If the shift count is greater than or equal to the width of the
                        ** promoted left operand, the behaviour is undefined according to
                        ** the standard.
                        */
                        if (Expr2.IVal < 0) {
                            Warning ("Negative shift count %ld treated as %u for %s",
                                     Expr2.IVal,
                                     (unsigned)Expr2.IVal & (ExprBits - 1),
                                     GetBasicTypeName (CalType));
                        } else if (Expr2.IVal >= (long)ExprBits) {
                            Warning ("Shift count %ld >= width of %s treated as %u",
                                     Expr2.IVal,
                                     GetBasicTypeName (CalType),
                                     (unsigned)Expr2.IVal & (ExprBits - 1));
                        }

                        /* Here we simply "wrap" the shift count around the width */
                        Expr2.IVal &= ExprBits - 1;

                        /* Additional check for bit-fields */
                        if (Expr2.IVal >= (long)Expr->Type->A.B.Width) {
                            Warning ("Shift count %ld >= width of bit-field", Expr2.IVal);
                        }
                    }
                }

                /* Adjust the types of the operands if needed */
                if (Gen->Func == g_div || Gen->Func == g_mod) {
                    unsigned AdjustedFlags = Flags;
                    if (Expr->Type->A.B.Width < INT_BITS || IsSignSigned (Expr->Type)) {
                        AdjustedFlags = (Flags & ~CF_UNSIGNED) | CF_CONST;
                        AdjustedFlags = g_typeadjust (AdjustedFlags, CG_TypeOf (Expr2.Type) | CF_CONST);
                    }
                    Gen->Func (g_typeadjust (Flags, AdjustedFlags) | CF_CONST, Expr2.IVal);
                } else {
                    Gen->Func ((Flags & ~CF_FORCECHAR) | CF_CONST, Expr2.IVal);
                }
            }

        }

    } else {

        /* Do 'op' if provided */
        if (Gen != 0) {

            /* Load rhs into the primary */
            LoadExpr (CF_NONE, &Expr2);

            /* Adjust the types of the operands if needed */
            if (Gen->Func == g_div || Gen->Func == g_mod) {
                unsigned AdjustedFlags = Flags;
                if (Expr->Type->A.B.Width < INT_BITS || IsSignSigned (Expr->Type)) {
                    AdjustedFlags = (Flags & ~CF_UNSIGNED) | CF_CONST;
                    AdjustedFlags = g_typeadjust (AdjustedFlags, CG_TypeOf (Expr2.Type) | CF_CONST);
                }
                Gen->Func (g_typeadjust (Flags, AdjustedFlags), 0);
            } else {
                Gen->Func (g_typeadjust (Flags, CG_TypeOf (Expr2.Type)), 0);
            }

        } else {

            /* Do type conversion if necessary */
            TypeConversion (&Expr2, Expr->Type);

            /* If necessary, load rhs into the primary register */
            LoadExpr (CF_NONE, &Expr2);

        }

    }

    /* Apply the mask */
    g_and (Flags | CF_CONST, Mask);

    /* Do integral promotion without sign-extension if needed */
    g_typecast (ChunkFlags | CF_UNSIGNED, Flags);

    /* Shift it into the right position */
    g_asl (ChunkFlags | CF_CONST, Expr->Type->A.B.Offs);

    /* Push the interim result on stack */
    g_push (ChunkFlags & ~CF_FORCECHAR, 0);

    /* If the original lhs was using the primary, it is now accessible only via
    ** the pushed address. Reload that address.
    */
    if (ED_IsLocPrimaryOrExpr (Expr)) {
        g_getlocal (CF_PTR, AddrSP);
    }

    /* Load the whole data chunk containing the bits to be changed */
    LoadExpr (ChunkFlags, Expr);

    /* Get the bits that are not to be affected */
    g_and (ChunkFlags | CF_CONST, ~(Mask << Expr->Type->A.B.Offs));

    /* Restore the bits that are not to be affected */
    g_or (ChunkFlags & ~CF_FORCECHAR, 0);

    /* Store the whole data chunk containing the changed bits back */
    Store (Expr, ChunkType);

Done:

    /* Value is in primary as an rvalue */
    ED_FinalizeRValLoad (Expr);
}



static void OpAssignArithmetic (const GenDesc* Gen, ExprDesc* Expr, const char* Op)
/* Parse an "=" (if 'Gen' is 0) or "op=" operation for arithmetic lhs */
{
    ExprDesc Expr2;
    CodeMark PushPos;

    unsigned Flags;
    int MustScale;

    ED_Init (&Expr2);
    Expr2.Flags |= Expr->Flags & E_MASK_KEEP_SUBEXPR;

    /* Determine code generator flags */
    Flags = CG_TypeOf (Expr->Type);

    /* Determine the type of the lhs */
    MustScale = Gen != 0 && (Gen->Func == g_add || Gen->Func == g_sub) &&
                IsTypePtr (Expr->Type);

    /* Get the address on stack for the store */
    PushAddr (Expr);

    if (Gen == 0) {

        /* Read the expression on the right side of the '=' */
        MarkedExprWithCheck (hie1, &Expr2);

        /* Do type conversion if necessary. Beware: Do not use char type
        ** here!
        */
        TypeConversion (&Expr2, Expr->Type);

        /* If necessary, load the value into the primary register */
        LoadExpr (CF_NONE, &Expr2);

    } else {

        /* Load the original value if necessary */
        LoadExpr (CF_NONE, Expr);

        /* Push lhs on stack */
        GetCodePos (&PushPos);
        g_push (Flags, 0);

        /* Read the expression on the right side of the '=' or 'op=' */
        MarkedExprWithCheck (hie1, &Expr2);

        /* The rhs must be an integer (or a float, but we don't support that yet */
        if (!IsClassInt (Expr2.Type)) {
            Error ("Invalid right operand for binary operator '%s'", Op);
            /* Continue. Wrong code will be generated, but the compiler won't
            ** break, so this is the best error recovery.
            */
        }

        /* Special treatment if the value is constant.
        ** Beware: Expr2 may contain side effects, so there must not be
        ** code generated for Expr2.
        */
        if (ED_IsConstAbsInt (&Expr2) && ED_CodeRangeIsEmpty (&Expr2)) {

            /* Since we will operate with a constant, we can remove the push if
            ** the generator has the NOPUSH flag set.
            */
            if (Gen->Flags & GEN_NOPUSH) {
                RemoveCode (&PushPos);
            }
            if (MustScale) {
                /* lhs is a pointer, scale rhs */
                Expr2.IVal *= CheckedSizeOf (Expr->Type+1);
            }

            /* If the lhs is character sized, the operation may be later done
            ** with characters.
            */
            if (CheckedSizeOf (Expr->Type) == SIZEOF_CHAR) {
                Flags |= CF_FORCECHAR;
            }

            /* Special handling for add and sub - some sort of a hack, but short code */
            if (Gen->Func == g_add) {
                g_inc (Flags | CF_CONST, Expr2.IVal);
            } else if (Gen->Func == g_sub) {
                g_dec (Flags | CF_CONST, Expr2.IVal);
            } else {
                if (!ED_IsUneval (Expr)) {
                    if (Expr2.IVal == 0 && !ED_IsUneval (Expr)) {
                        /* Check for div by zero/mod by zero */
                        if (Gen->Func == g_div) {
                            Warning ("Division by zero");
                        } else if (Gen->Func == g_mod) {
                            Warning ("Modulo operation with zero");
                        }
                    } else if (Gen->Func == g_asl || Gen->Func == g_asr) {
                        const Type* CalType  = IntPromotion (Expr->Type);
                        unsigned    ExprBits = BitSizeOf (CalType);

                        /* If the shift count is greater than or equal to the width of the
                        ** promoted left operand, the behaviour is undefined according to
                        ** the standard.
                        */
                        if (Expr2.IVal < 0) {
                            Warning ("Negative shift count %ld treated as %u for %s",
                                     Expr2.IVal,
                                     (unsigned)Expr2.IVal & (ExprBits - 1),
                                     GetBasicTypeName (CalType));
                        } else if (Expr2.IVal >= (long)ExprBits) {
                            Warning ("Shift count %ld >= width of %s treated as %u",
                                     Expr2.IVal,
                                     GetBasicTypeName (CalType),
                                     (unsigned)Expr2.IVal & (ExprBits - 1));
                        }

                        /* Here we simply "wrap" the shift count around the width */
                        Expr2.IVal &= ExprBits - 1;

                        /* Additional check for bit width */
                        if (Expr2.IVal >= (long)BitSizeOf (Expr->Type)) {
                            Warning ("Shift count %ld >= width of %s",
                                     Expr2.IVal, GetBasicTypeName (Expr->Type));
                        }
                    }
                }
                Gen->Func (Flags | CF_CONST, Expr2.IVal);
            }

        } else {

            /* If necessary, load the value into the primary register */
            LoadExpr (CF_NONE, &Expr2);

            if (MustScale) {
                /* lhs is a pointer, scale rhs */
                g_scale (CG_TypeOf (Expr2.Type), CheckedSizeOf (Expr->Type+1));
            }

            /* If the lhs is character sized, the operation may be later done
            ** with characters.
            */
            if (CheckedSizeOf (Expr->Type) == SIZEOF_CHAR) {
                Flags |= CF_FORCECHAR;
            }

            /* Adjust the types of the operands if needed */
            Gen->Func (g_typeadjust (Flags, CG_TypeOf (Expr2.Type)), 0);

        }
    }

    /* Generate a store instruction */
    Store (Expr, 0);

    /* Value is in primary as an rvalue */
    ED_FinalizeRValLoad (Expr);
}



void OpAssign (const GenDesc* Gen, ExprDesc* Expr, const char* Op)
/* Parse an "=" (if 'Gen' is 0) or "op=" operation */
{
    const Type* ltype = Expr->Type;

    ExprDesc Expr2;
    ED_Init (&Expr2);
    Expr2.Flags |= Expr->Flags & E_MASK_KEEP_SUBEXPR;

    /* Only "=" accept struct/union */
    if (IsClassStruct (ltype) ? Gen != 0 : !IsScalarType (ltype)) {
        Error ("Invalid left operand for binary operator '%s'", Op);
        /* Continue. Wrong code will be generated, but the compiler won't
        ** break, so this is the best error recovery.
        */
    } else {
        /* Check for assignment to incomplete type */
        if (IsIncompleteESUType (ltype)) {
            Error ("Assignment to incomplete type '%s'", GetFullTypeName (ltype));
        } else if (ED_IsRVal (Expr)) {
            /* Assignment can only be used with lvalues */
            if (IsTypeArray (ltype)) {
                Error ("Array type '%s' is not assignable", GetFullTypeName (ltype));
            } else if (IsTypeFunc (ltype)) {
                Error ("Function type '%s' is not assignable", GetFullTypeName (ltype));
            } else {
                Error ("Assignment to rvalue");
            }
        } else if (IsQualConst (ltype)) {
            /* Check for assignment to const */
            Error ("Assignment to const variable");
        }
    }

    /* Skip the '=' or 'op=' token */
    NextToken ();

    /* cc65 does not have full support for handling structs or unions. Since
    ** assigning structs is one of the more useful operations from this family,
    ** allow it here.
    ** Note: IsClassStruct() is also true for union types.
    */
    if (IsClassStruct (ltype)) {
        /* Copy the struct or union by value */
        CopyStruct (Expr, &Expr2);
    } else if (IsTypeFragBitField (ltype)) {
        /* Special care is needed for bit-fields if they don't fit in full bytes */
        OpAssignBitField (Gen, Expr, Op);
    } else {
        /* Normal straight 'op=' */
        OpAssignArithmetic (Gen, Expr, Op);
    }

    /* Expression has had side effects */
    Expr->Flags |= E_SIDE_EFFECTS;

    /* Propagate viral flags */
    ED_PropagateFrom (Expr, &Expr2);
}



void OpAddSubAssign (const GenDesc* Gen, ExprDesc *Expr, const char* Op)
/* Parse a "+=" or "-=" operation */
{
    ExprDesc Expr2;
    unsigned lflags;
    unsigned rflags;
    int      MustScale;

    /* We currently only handle non-bit-fields in some addressing modes here */
    if (IsTypeBitField (Expr->Type) || ED_IsLocPrimaryOrExpr (Expr)) {
        /* Use generic routine instead */
        OpAssign (Gen, Expr, Op);
        return;
    }

    /* There must be an integer or pointer on the left side */
    if (!IsClassInt (Expr->Type) && !IsTypePtr (Expr->Type)) {
        Error ("Invalid left operand for binary operator '%s'", Op);
        /* Continue. Wrong code will be generated, but the compiler won't
        ** break, so this is the best error recovery.
        */
    } else {
        /* We must have an lvalue */
        if (ED_IsRVal (Expr)) {
            Error ("Invalid lvalue in assignment");
        } else if (IsQualConst (Expr->Type)) {
            /* The left side must not be const qualified */
            Error ("Assignment to const variable");
        }
    }

    /* Skip the operator */
    NextToken ();

    /* Check if we have a pointer expression and must scale rhs */
    MustScale = IsTypePtr (Expr->Type);

    /* Initialize the code generator flags */
    lflags = 0;
    rflags = 0;

    ED_Init (&Expr2);
    Expr2.Flags |= Expr->Flags & E_MASK_KEEP_SUBEXPR;

    /* Evaluate the rhs. We expect an integer here, since float is not
    ** supported
    */
    hie1 (&Expr2);
    if (!IsClassInt (Expr2.Type)) {
        Error ("Invalid right operand for binary operator '%s'", Op);
        /* Continue. Wrong code will be generated, but the compiler won't
        ** break, so this is the best error recovery.
        */
    }

    /* Setup the code generator flags */
    lflags |= CG_TypeOf (Expr->Type) | CG_AddrModeFlags (Expr) | CF_FORCECHAR;
    rflags |= CG_TypeOf (Expr2.Type) | CF_FORCECHAR;

    if (ED_IsConstAbs (&Expr2)) {
        /* The resulting value is a constant */
        rflags |= CF_CONST;
        lflags |= CF_CONST;

        /* Scale it */
        if (MustScale) {
            Expr2.IVal *= CheckedSizeOf (Indirect (Expr->Type));
        }
    } else {
        /* Not constant, load into the primary */
        LoadExpr (CF_NONE, &Expr2);

        /* Convert the type of the rhs to that of the lhs */
        g_typecast (lflags, rflags & ~CF_FORCECHAR);

        if (MustScale) {
            /* lhs is a pointer, scale rhs */
            g_scale (CG_TypeOf (Expr2.Type), CheckedSizeOf (Indirect (Expr->Type)));
        }
    }

    /* Output apropriate code depending on the location */
    switch (ED_GetLoc (Expr)) {

        case E_LOC_ABS:
        case E_LOC_GLOBAL:
        case E_LOC_STATIC:
        case E_LOC_REGISTER:
        case E_LOC_LITERAL:
        case E_LOC_CODE:
            /* Absolute numeric addressed variable, global variable, local
            ** static variable, register variable, pooled literal or code
            ** label location.
            */
            if (Gen->Tok == TOK_PLUS_ASSIGN) {
                g_addeqstatic (lflags, Expr->Name, Expr->IVal, Expr2.IVal);
            } else {
                g_subeqstatic (lflags, Expr->Name, Expr->IVal, Expr2.IVal);
            }
            break;

        case E_LOC_STACK:
            /* Value on the stack */
            if (Gen->Tok == TOK_PLUS_ASSIGN) {
                g_addeqlocal (lflags, Expr->IVal, Expr2.IVal);
            } else {
                g_subeqlocal (lflags, Expr->IVal, Expr2.IVal);
            }
            break;

        default:
            Internal ("Invalid location in Store(): 0x%04X", ED_GetLoc (Expr));
    }

    /* Expression is an rvalue in the primary now */
    ED_FinalizeRValLoad (Expr);

    /* Expression has had side effects */
    Expr->Flags |= E_SIDE_EFFECTS;

    /* Propagate viral flags */
    ED_PropagateFrom (Expr, &Expr2);
}
