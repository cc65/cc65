/*****************************************************************************/
/*                                                                           */
/*                                shiftexpr.c                                */
/*                                                                           */
/*                       Parse the << and >> operators                       */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2004-2006 Ullrich von Bassewitz                                       */
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



/* cc65 */
#include "asmcode.h"
#include "codegen.h"
#include "datatype.h"
#include "error.h"
#include "expr.h"
#include "exprdesc.h"
#include "loadexpr.h"
#include "scanner.h"
#include "shiftexpr.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



void ShiftExpr (struct ExprDesc* Expr)
/* Parse the << and >> operators. */
{
    CodeMark Mark1;
    CodeMark Mark2;
    token_t Tok;                        /* The operator token */
    const Type* ResultType;             /* Type of the result */
    unsigned ExprBits;                  /* Bits of the lhs operand */
    unsigned GenFlags;                  /* Generator flags */
    unsigned ltype;
    int lconst;                         /* Operand is a constant */
    int rconst;                         /* Operand is a constant */


    /* Evaluate the lhs */
    ExprWithCheck (hie8, Expr);

    while (CurTok.Tok == TOK_SHL || CurTok.Tok == TOK_SHR) {

        ExprDesc Expr2;
        ED_Init (&Expr2);
        Expr2.Flags |= Expr->Flags & E_MASK_KEEP_SUBEXPR;

        /* All operators that call this function expect an int on the lhs */
        if (!IsClassInt (Expr->Type)) {
            Error ("Integer expression expected");
            ED_MakeConstAbsInt (Expr, 1);
        }

        /* Remember the operator token, then skip it */
        Tok = CurTok.Tok;
        NextToken ();

        /* Get the type of the result */
        ResultType = IntPromotion (Expr->Type);

        /* Prepare the code generator flags */
        GenFlags = CG_TypeOf (ResultType);

        /* Calculate the number of bits the lhs operand has */
        ExprBits = SizeOf (ResultType) * 8;

        /* Get the lhs on stack */
        GetCodePos (&Mark1);
        ltype = CG_TypeOf (Expr->Type);
        lconst = ED_IsConstAbs (Expr);
        if (lconst) {
            /* Constant value */
            GetCodePos (&Mark2);
            g_push (ltype | CF_CONST, Expr->IVal);
        } else {
            /* Value not constant */
            LoadExpr (CF_NONE, Expr);
            GetCodePos (&Mark2);
            g_push (ltype, 0);
        }

        /* Get the right hand side */
        MarkedExprWithCheck (hie8, &Expr2);

        /* Check the type of the rhs */
        if (!IsClassInt (Expr2.Type)) {
            Error ("Integer expression expected");
            ED_MakeConstAbsInt (&Expr2, 1);
        }

        /* Check for a constant right side expression */
        rconst = ED_IsConstAbs (&Expr2) && ED_CodeRangeIsEmpty (&Expr2);
        if (!rconst) {

            /* Not constant, load into the primary */
            LoadExpr (CF_NONE, &Expr2);

        } else {

            /* The rhs is a constant numeric value. */
            GenFlags |= CF_CONST;

            /* Remove the code that pushes the rhs onto the stack. */
            RemoveCode (&Mark2);

            /* If the shift count is greater than or equal to the width of the
            ** promoted left operand, the behaviour is undefined according to
            ** the standard.
            */
            if (!ED_IsUneval (Expr)) {
                if (Expr2.IVal < 0) {
                    Warning ("Negative shift count %ld treated as %u for %s",
                             Expr2.IVal,
                             (unsigned)Expr2.IVal & (ExprBits - 1),
                             GetBasicTypeName (ResultType));
                } else if (Expr2.IVal >= (long) ExprBits) {
                    Warning ("Shift count %ld >= width of %s treated as %u",
                             Expr2.IVal,
                             GetBasicTypeName (ResultType),
                             (unsigned)Expr2.IVal & (ExprBits - 1));
                }
            }

            /* Here we simply "wrap" the shift count around the width */
            Expr2.IVal &= ExprBits - 1;

            /* Additional check for bit-fields */
            if (IsTypeBitField (Expr->Type) &&
                Tok == TOK_SHR              &&
                Expr2.IVal >= (long) Expr->Type->A.B.Width) {
                if (!ED_IsUneval (Expr)) {
                    Warning ("Right-shift count %ld >= width of bit-field", Expr2.IVal);
                }
            }

            /* If the left hand side is a constant, the result is constant */
            if (lconst) {
                /* Set the result type */
                Expr->Type = ResultType;

                /* Evaluate the result */
                switch (Tok) {
                    case TOK_SHL: Expr->IVal <<= Expr2.IVal; break;
                    case TOK_SHR: Expr->IVal >>= Expr2.IVal; break;
                    default: /* Shutup gcc */                break;
                }

                /* Limit the calculated value to the range of its type */
                LimitExprValue (Expr, 1);

                /* Result is already got, remove the generated code */
                RemoveCode (&Mark1);

                /* Done */
                continue;
            }

            /* If the shift count is zero, nothing happens */
            if (Expr2.IVal == 0) {
                /* Result is already got, remove the pushing code */
                RemoveCode (&Mark2);

                /* Be sure to mark the value as in the primary */
                goto MakeRVal;
            }

            /* If we're shifting an integer or unsigned to the right, the lhs
            ** has a quasi-const address, and the shift count is larger than 8,
            ** we can load just the high byte as a char with the correct
            ** signedness, and reduce the shift count by 8. If the remaining
            ** shift count is zero, we're done.
            */
            if (Tok == TOK_SHR &&
                IsClassInt (Expr->Type) &&
                SizeOf (Expr->Type) == SIZEOF_INT &&
                ED_IsLVal (Expr) &&
                ED_IsLocQuasiConst (Expr) &&
                Expr2.IVal >= 8) {

                /* Increase the address by one and decrease the shift count */
                ++Expr->IVal;
                Expr2.IVal -= 8;

                /* Replace the type of the expression temporarily by the
                ** corresponding char type.
                */
                if (IsSignUnsigned (Expr->Type)) {
                    Expr->Type = type_uchar;
                } else {
                    Expr->Type = type_schar;
                }

                /* Remove the generated load code */
                RemoveCode (&Mark1);

                /* Generate again code for the load, this time with the new type */
                LoadExpr (CF_NONE, Expr);

                /* If the shift count is now zero, we're done */
                if (Expr2.IVal == 0) {
                    /* Be sure to mark the value as in the primary */
                    goto MakeRVal;
                }
            }

        }

        /* Generate code */
        switch (Tok) {
            case TOK_SHL: g_asl (GenFlags, Expr2.IVal); break;
            case TOK_SHR: g_asr (GenFlags, Expr2.IVal); break;
            default:                                    break;
        }

MakeRVal:
        /* We have an rvalue in the primary now */
        ED_FinalizeRValLoad (Expr);

        /* Set the type of the result */
        Expr->Type = ResultType;

        /* Propagate from subexpressions */
        Expr->Flags |= Expr2.Flags & E_MASK_VIRAL;
    }
}
