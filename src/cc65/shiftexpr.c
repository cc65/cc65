/*****************************************************************************/
/*                                                                           */
/*                                shiftexpr.c                                */
/*                                                                           */
/*                       Parse the << and >> operators                       */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2004      Ullrich von Bassewitz                                       */
/*               Römerstraße 52                                              */
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
/*		     		     Data				     */
/*****************************************************************************/



/*****************************************************************************/
/*	   	  		     Code				     */
/*****************************************************************************/



void ShiftExpr (struct ExprDesc* Expr)
/* Parse the << and >> operators. */
{
    ExprDesc Expr2;
    CodeMark Mark1;
    CodeMark Mark2;
    token_t Tok;       	      	 	/* The operator token */
    unsigned ExprBits;                  /* Bits of the lhs operand */
    unsigned ltype, rtype, flags;
    int rconst;	       	       	       	/* Operand is a constant */


    /* Evaluate the lhs */
    ExprWithCheck (hie8, Expr);

    while (CurTok.Tok == TOK_SHL || CurTok.Tok == TOK_SHR) {

	/* All operators that call this function expect an int on the lhs */
	if (!IsClassInt (Expr->Type)) {
	    Error ("Integer expression expected");
            ED_MakeConstAbsInt (Expr, 1);
	}

	/* Remember the operator token, then skip it */
       	Tok = CurTok.Tok;
	NextToken ();

        /* Calculate the number of bits the lhs operand has */
        ExprBits = SizeOf (Expr->Type) * 8;

	/* Get the lhs on stack */
       	Mark1 = GetCodePos ();
	ltype = TypeOf (Expr->Type);
       	if (ED_IsConstAbs (Expr)) {
	    /* Constant value */
	    Mark2 = GetCodePos ();
       	    g_push (ltype | CF_CONST, Expr->IVal);
	} else {
	    /* Value not constant */
	    LoadExpr (CF_NONE, Expr);
	    Mark2 = GetCodePos ();
	    g_push (ltype, 0);
	}

	/* Get the right hand side */
        ExprWithCheck (hie8, &Expr2);

	/* Check the type of the rhs */
	if (!IsClassInt (Expr2.Type)) {
	    Error ("Integer expression expected");
            ED_MakeConstAbsInt (&Expr2, 1);
	}

        /* Check for a constant right side expression */
        rconst = ED_IsConstAbs (&Expr2);
        if (!rconst) {

            /* Not constant, load into the primary */
            LoadExpr (CF_NONE, &Expr2);

        } else {

            /* The rhs is a constant numeric value */

            /* If the shift count is greater or equal than the bit count of
             * the operand, the behaviour is undefined according to the
             * standard.
             */
            if (Expr2.IVal < 0 || Expr2.IVal >= (long) ExprBits) {

                Warning ("Shift count too large for operand type");
                Expr2.IVal &= ExprBits - 1;

            }

            /* If the shift count is zero, nothing happens */
            if (Expr2.IVal == 0) {

                /* Result is already in Expr, remove the generated code */
                RemoveCode (Mark1);
                pop (ltype);

                /* Done */
                goto Next;
            }

            /* If the left hand side is a constant, the result is constant */
            if (ED_IsConstAbs (Expr)) {

                /* Evaluate the result */
                switch (Tok) {
                    case TOK_SHL: Expr->IVal <<= Expr2.IVal; break;
                    case TOK_SHR: Expr->IVal >>= Expr2.IVal; break;
                    default: /* Shutup gcc */                break;
                }

                /* Both operands are constant, remove the generated code */
                RemoveCode (Mark1);
                pop (ltype);

                /* Done */
                goto Next;
            }

            /* If we're shifting an integer or unsigned to the left, the
             * lhs has a const address, and the shift count is larger than 8,
             * we can load just the high byte as a char with the correct
             * signedness, and reduce the shift count by 8. If the remaining
             * shift count is zero, we're done.
             */
            if (Tok == TOK_SHL &&
                IsTypeInt (Expr->Type) &&
                ED_IsLVal (Expr) &&
                (ED_IsLocConst (Expr) || ED_IsLocStack (Expr)) &&
                Expr2.IVal >= 8) {

                type* OldType;

                /* Increase the address by one and decrease the shift count */
                ++Expr->IVal;
                Expr2.IVal -= 8;

                /* Replace the type of the expression temporarily by the
                 * corresponding char type.
                 */
                OldType = Expr->Type;
                if (IsSignUnsigned (Expr->Type)) {
                    Expr->Type = type_uchar;
                } else {
                    Expr->Type = type_schar;
                }

                /* Remove the generated load code */
                RemoveCode (Mark1);
                pop (ltype);

                /* Generate again code for the load */
                LoadExpr (CF_NONE, Expr);

                /* Reset the type */
                Expr->Type = OldType;

                /* If the shift count is now zero, we're done */
                if (Expr2.IVal == 0) {
                    /* Be sure to mark the value as in the primary */
                    goto Loaded;
                }

                /* Otherwise generate code to push the value */
                Mark2 = GetCodePos ();
                g_push (ltype, 0);
            }

        }

        /* If the right hand side is a constant, remove the push of the
         * primary register.
         */
        rtype = TypeOf (Expr2.Type);
        flags = 0;
        if (rconst) {
            flags |= CF_CONST;
            rtype |= CF_CONST;
            RemoveCode (Mark2);
            pop (ltype);
            ltype |= CF_REG;      	/* Value is in register */
        }

        /* Determine the type of the operation result. */
        flags |= g_typeadjust (ltype, rtype);

        /* Generate code */
        switch (Tok) {
            case TOK_SHL: g_asl (flags, Expr2.IVal); break;
            case TOK_SHR: g_asr (flags, Expr2.IVal); break;
            default:                                 break;
        }

Loaded:
        /* We have a rvalue in the primary now */
        ED_MakeRValExpr (Expr);

Next:
        /* Get the type of the result */
       	Expr->Type = IntPromotion (Expr->Type);
    }
}



