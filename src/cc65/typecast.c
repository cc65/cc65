/*****************************************************************************/
/*                                                                           */
/*                                typecast.c                                 */
/*                                                                           */
/*                             Handle type casts                             */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2002      Ullrich von Bassewitz                                       */
/*               Wacholderweg 14                                             */
/*               D-70597 Stuttgart                                           */
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
#include "typecast.h"



/*****************************************************************************/
/*				     Code                                    */
/*****************************************************************************/



int TypeCast (ExprDesc* lval)
/* Handle an explicit cast. The function returns true if the resulting
 * expression is an lvalue and false if not.
 */
{
    int      k;
    type*    OldType;
    type     NewType[MAXTYPELEN];
    unsigned OldSize;
    unsigned NewSize;

    /* Skip the left paren */
    NextToken ();

    /* Read the type */
    ParseType (NewType);

    /* Closing paren */
    ConsumeRParen ();

    /* Read the expression we have to cast */
    k = hie10 (lval);

    /* If the expression is a function, treat it as pointer-to-function */
    if (IsTypeFunc (lval->Type)) {
    	lval->Type = PointerTo (lval->Type);
    }

    /* Remember the old type and use the new one */
    OldType = lval->Type;
    lval->Type = TypeDup (NewType);

    /* If we're casting to void, we're done. Note: This does also cover a cast
     * void -> void.
     */
    if (IsTypeVoid (NewType)) {
        return 0;       /* Never an lvalue */
    }

    /* Don't allow casts from void to something else. The new type is already
     * set which should avoid more errors, but code will not get generated
     * because of the error.
     */
    if (IsTypeVoid (OldType)) {
        Error ("Cannot cast from `void' to something else");
        return k;
    }

    /* Get the sizes of the types. Since we've excluded void types, checking
     * for known sizes makes sense here.
     */
    OldSize = CheckedSizeOf (OldType);
    NewSize = CheckedSizeOf (NewType);

    /* Is this a cast of something into an integer? */
    if (IsClassInt (NewType)) {

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
                exprhs (CF_NONE, k, lval);

                /* Emit typecast code */
                g_typecast (TypeOf (OldType), TypeOf (NewType));

                /* Value is now in primary */
                lval->Flags = E_MEXPR;
                k = 0;
            }

        } else {

            /* We have an rvalue. Check for a constant. */
            if (lval->Flags == E_MCONST) {

                /* A cast of a constant to an integer. Be sure to handle sign
                 * extension correctly.
                 */

                /* Get the current and new size of the value */
                unsigned OldBits = OldSize * 8;
                unsigned NewBits = NewSize * 8;

                /* Check if the new datatype will have a smaller range */
                if (NewBits <= OldBits) {

                    /* Cut the value to the new size */
                    lval->ConstVal &= (0xFFFFFFFFUL >> (32 - NewBits));

                    /* If the new type is signed, sign extend the value */
                    if (!IsSignUnsigned (NewType)) {
                        lval->ConstVal |= ((~0L) << NewBits);
                    }

                } else {

                    /* Sign extend the value if needed */
                    if (!IsSignUnsigned (OldType) && !IsSignUnsigned (NewType)) {
                        if (lval->ConstVal & (0x01UL << (OldBits-1))) {
                            lval->ConstVal |= ((~0L) << OldBits);
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
                    exprhs (CF_NONE, k, lval);

                    /* Emit typecast code. ### CHARS */
                    g_typecast (TypeOf (OldType), TypeOf (NewType));

                    /* Value is now in primary */
                    lval->Flags = E_MEXPR;
                    k = 0;
                }
            }
        }

    } else {

        /* All other stuff is handled equally */
        if (NewSize != OldSize) {
            /* Load the value into the primary */
            exprhs (CF_NONE, k, lval);

            /* Emit typecast code */
            g_typecast (TypeOf (OldType), TypeOf (NewType));

            /* Value is now in primary */
            lval->Flags = E_MEXPR;
            k = 0;
        }
    }

    /* Done */
    return k;
}






