/*****************************************************************************/
/*                                                                           */
/*                               assignment.c                                */
/*                                                                           */
/*                             Parse assignments                             */
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
#include "error.h"
#include "expr.h"
#include "typecmp.h"
#include "typeconv.h"
#include "assignment.h"



/*****************************************************************************/
/*				     Code                                    */
/*****************************************************************************/



int Assignment (ExprDesc* lval)
/* Parse an assignment */
{
    int k;
    ExprDesc lval2;
    type* ltype = lval->Type;


    /* Check for assignment to const */
    if (IsQualConst (ltype)) {
    	Error ("Assignment to const");
    }

    /* cc65 does not have full support for handling structs by value. Since
     * assigning structs is one of the more useful operations from this
     * family, allow it here.
     */
    if (IsClassStruct (ltype)) {

        /* Get the size of the left hand side. */
        unsigned Size = SizeOf (ltype);

        /* If the size is that of a basic type (char, int, long), we will copy
         * the struct using the primary register, otherwise we use memcpy. In
         * the former case, push the address only if really needed.
         */
        int UseReg = 1;
        type* stype;
        switch (Size) {
            case SIZEOF_CHAR:   stype = type_uchar;             break;
            case SIZEOF_INT:    stype = type_uint;              break;
            case SIZEOF_LONG:   stype = type_ulong;             break;
            default:            stype = ltype; UseReg = 0;      break;
        }
        if (UseReg) {
            PushAddr (lval);
        } else {
    	    exprhs (0, 0, lval);
            g_push (CF_PTR | CF_UNSIGNED, 0);
        }

     	/* Get the expression on the right of the '=' into the primary */
	k = hie1 (&lval2);

        /* Check for equality of the structs */
        if (TypeCmp (ltype, lval2.Type) < TC_STRICT_COMPATIBLE) {
            Error ("Incompatible types");
        }

        /* Check if the right hand side is an lvalue */
	if (k) {
	    /* We have an lvalue. Do we copy using the primary? */
            if (UseReg) {
                /* Just use the replacement type */
                lval2.Type = stype;

                /* Load the value into the primary */
                exprhs (CF_FORCECHAR, k, &lval2);

                /* Store it into the new location */
                Store (lval, stype);

            } else {

                /* We will use memcpy. Push the address of the rhs */
                exprhs (0, 0, &lval2);

                /* Push the address (or whatever is in ax in case of errors) */
                g_push (CF_PTR | CF_UNSIGNED, 0);

                /* Load the size of the struct into the primary */
                g_getimmed (CF_INT | CF_UNSIGNED | CF_CONST, CheckedSizeOf (ltype), 0);

                /* Call the memcpy function */
                g_call (CF_FIXARGC, "memcpy", 4);
            }

        } else {

            /* We have an rvalue. This can only happen if a function returns
             * a struct, since there is no other way to generate an expression
             * that as a struct as an rvalue result. We allow only 1, 2, and 4
             * byte sized structs and do direct assignment.
             */
            if (UseReg) {
                /* Do the store */
                Store (lval, stype);
            } else {
                /* Print a diagnostic */
                Error ("Structs of this size are not supported");
                /* Adjust the stack so we won't run in an internal error later */
                pop (CF_PTR);
            }

        }

    } else {

    	/* Get the address on stack if needed */
    	PushAddr (lval);

	/* Read the expression on the right side of the '=' */
	k = hie1 (&lval2);

	/* Do type conversion if necessary */
	k = TypeConversion (&lval2, k, ltype);

	/* If necessary, load the value into the primary register */
	exprhs (CF_NONE, k, &lval2);

     	/* Generate a store instruction */
     	Store (lval, 0);

    }

    /* Value is still in primary and not an lvalue */
    lval->Flags = E_MEXPR;
    return 0;
}



