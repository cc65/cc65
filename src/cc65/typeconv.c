/*****************************************************************************/
/*                                                                           */
/*                                typeconv.c                                 */
/*                                                                           */
/*                          Handle type conversions                          */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2002-2003 Ullrich von Bassewitz                                       */
/*               Römerstrasse 52                                             */
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
#include "codegen.h"
#include "datatype.h"
#include "declare.h"
#include "error.h"
#include "expr.h"
#include "scanner.h"
#include "typecmp.h"
#include "typeconv.h"



/*****************************************************************************/
/*				     Code                                    */
/*****************************************************************************/



static void DoPtrConversions (ExprDesc* Expr)
/* If the expression is a function, convert it to pointer to function.
 * If the expression is an array, convert it to pointer to first element.
 */
{
    if (IsTypeFunc (Expr->Type)) {
    	Expr->Type = PointerTo (Expr->Type);
    } else if (IsTypeArray (Expr->Type)) {
        Expr->Type = ArrayToPtr (Expr->Type);
    }
}



static int DoConversion (ExprDesc* Expr, int k, type* NewType)
/* Emit code to convert the given expression to a new type. */
{
    type*    OldType;
    unsigned OldSize;
    unsigned NewSize;


    /* Remember the old type */
    OldType = Expr->Type;

    /* If we're converting to void, we're done. Note: This does also cover a
     * conversion void -> void.
     */
    if (IsTypeVoid (NewType)) {
        k = 0;          /* Never an lvalue */
        goto ExitPoint;
    }

    /* Don't allow casts from void to something else. */
    if (IsTypeVoid (OldType)) {
        Error ("Cannot convert from `void' to something else");
        goto ExitPoint;
    }

    /* Get the sizes of the types. Since we've excluded void types, checking
     * for known sizes makes sense here.
     */
    OldSize = CheckedSizeOf (OldType);
    NewSize = CheckedSizeOf (NewType);

    /* lvalue? */
    if (k != 0) {

        /* We have an lvalue. If the new size is smaller than the new one,
         * we don't need to do anything. The compiler will generate code
         * to load only the portion of the value that is actually needed.
         * This works only on a little endian architecture, but that's
         * what we support.
         * If both sizes are equal, do also leave the value alone.
         * If the new size is larger, we must convert the value.
         */
        if (NewSize > OldSize) {
            /* Load the value into the primary */
            exprhs (CF_NONE, k, Expr);

            /* Emit typecast code */
            g_typecast (TypeOf (NewType), TypeOf (OldType));

            /* Value is now in primary */
            Expr->Flags = E_MEXPR;
            k = 0;
        }

    } else {

        /* We have an rvalue. Check for a constant. */
        if (Expr->Flags == E_MCONST) {

            /* A cast of a constant to an integer. Be sure to handle sign
             * extension correctly.
             */

            /* Get the current and new size of the value */
            unsigned OldBits = OldSize * 8;
            unsigned NewBits = NewSize * 8;

            /* Check if the new datatype will have a smaller range. If it
             * has a larger range, things are ok, since the value is
             * internally already represented by a long.
             */
            if (NewBits <= OldBits) {

                /* Cut the value to the new size */
                Expr->ConstVal &= (0xFFFFFFFFUL >> (32 - NewBits));

                /* If the new type is signed, sign extend the value */
                if (!IsSignUnsigned (NewType)) {
                    if (Expr->ConstVal & (0x01UL << (NewBits-1))) {
                        Expr->ConstVal |= ((~0L) << NewBits);
                    }
                }
            }

        } else {

            /* The value is not a constant. If the sizes of the types are
             * not equal, add conversion code. Be sure to convert chars
             * correctly.
             */
            if (OldSize != NewSize) {

                /* Load the value into the primary */
                exprhs (CF_NONE, k, Expr);

                /* Emit typecast code. */
                g_typecast (TypeOf (NewType) | CF_FORCECHAR, TypeOf (OldType));

                /* Value is now in primary */
                Expr->Flags = E_MEXPR;
                k = 0;
            }
        }
    }

ExitPoint:
    /* The expression has always the new type */
    ReplaceType (Expr, NewType);

    /* Done */
    return k;
}



int TypeConversion (ExprDesc* Expr, int k, type* NewType)
/* Do an automatic conversion of the given expression to the new type. Output
 * warnings or errors where this automatic conversion is suspicious or
 * impossible.
 */
{
    /* Get the type of the right hand side. Treat function types as
     * pointer-to-function
     */
    DoPtrConversions (Expr);

    /* First, do some type checking */
    if (IsTypeVoid (NewType) || IsTypeVoid (Expr->Type)) {
    	/* If one of the sides are of type void, output a more apropriate
    	 * error message.
    	 */
       	Error ("Illegal type");
	return k;
    }

    /* Handle conversions to int type */
    if (IsClassInt (NewType)) {
       	if (IsClassPtr (Expr->Type)) {
     	    /* Pointer -> int conversion */
     	    Warning ("Converting pointer to integer without a cast");
       	} else if (!IsClassInt (Expr->Type)) {
     	    Error ("Incompatible types");
       	}

        /* Do a conversion regardless of errors and return the result. */
        return DoConversion (Expr, k, NewType);
    }

    /* Handle conversions to pointer type */
    if (IsClassPtr (NewType)) {
     	if (IsClassPtr (Expr->Type)) {
     	    /* Pointer to pointer assignment is valid, if:
     	     *   - both point to the same types, or
     	     *   - the rhs pointer is a void pointer, or
	     *   - the lhs pointer is a void pointer.
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
     	    if (Expr->Flags != E_MCONST || Expr->ConstVal != 0) {
     	       	Warning ("Converting integer to pointer without a cast");
     	    }
	} else if (IsTypeFuncPtr (NewType) && IsTypeFunc(Expr->Type)) {
	    /* Assignment of function to function pointer is allowed, provided
	     * that both functions have the same parameter list.
	     */
	    if (TypeCmp (Indirect (NewType), Expr->Type) < TC_EQUAL) {
	 	Error ("Incompatible types");
	    }
     	} else {
	    Error ("Incompatible types");
	}

       	/* Do the conversion even in case of errors */
	return DoConversion (Expr, k, NewType);

    }

    /* Invalid automatic conversion */
    Error ("Incompatible types");
    return DoConversion (Expr, k, NewType);
}



int TypeCast (ExprDesc* Expr)
/* Handle an explicit cast. The function returns true if the resulting
 * expression is an lvalue and false if not.
 */
{
    int	    k;
    type    NewType[MAXTYPELEN];

    /* Skip the left paren */
    NextToken ();

    /* Read the type */
    ParseType (NewType);

    /* Closing paren */
    ConsumeRParen ();

    /* Read the expression we have to cast */
    k = hie10 (Expr);

    /* Convert functions and arrays to "pointer to" object */
    DoPtrConversions (Expr);

    /* Convert the value and return the result. */
    return DoConversion (Expr, k, NewType);
}



