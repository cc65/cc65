/*****************************************************************************/
/*                                                                           */
/*                                 cfgexpr.c                                 */
/*                                                                           */
/*          Simple expressions for use with in configuration file            */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2005-2008, Ullrich von Bassewitz                                      */
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



/* common */
#include "strbuf.h"

/* ld65 */
#include "cfgexpr.h"
#include "error.h"
#include "exports.h"
#include "scanner.h"
#include "spool.h"



/*****************************************************************************/
/*     	      	    		     Data				     */
/*****************************************************************************/



/* Type of a CfgExpr */
enum {
    ceEmpty,
    ceInt,
    ceString
};

typedef struct CfgExpr CfgExpr;
struct CfgExpr {
    unsigned    Type;           /* Type of the expression */
    long        IVal;           /* Integer value if it's a string */
    StrBuf      SVal;           /* String value if it's a string */
};

#define CFGEXPR_INITIALIZER { ceEmpty, 0, STATIC_STRBUF_INITIALIZER }



/*****************************************************************************/
/*                                 Forwards                                  */
/*****************************************************************************/



static void Expr (CfgExpr* E);
/* Full expression */



/*****************************************************************************/
/*                              struct CfgExpr                               */
/*****************************************************************************/



static void CE_Done (CfgExpr* E)
/* Cleanup a CfgExpr struct */
{
    /* If the type is a string, we must delete the string buffer */
    if (E->Type == ceString) {
        SB_Done (&E->SVal);
    }
}



static void CE_AssureInt (const CfgExpr* E)
/* Make sure, E contains an integer */
{
    if (E->Type != ceInt) {
        CfgError ("Integer type expected");
    }
}



/*****************************************************************************/
/*     	       	       	       	     Code     				     */
/*****************************************************************************/



static void Factor (CfgExpr* E)
/* Read and return a factor in E */
{
    Export* Sym;


    switch (CfgTok) {

	case CFGTOK_IDENT:
	    /* An identifier - search an export with the given name */
            Sym = FindExport (GetStringId (CfgSVal));
            if (Sym == 0) {
                CfgError ("Unknown symbol in expression: `%s'", CfgSVal);
            }
            /* We can only handle constants */
            if (!IsConstExport (Sym)) {
                CfgError ("Value for symbol `%s' is not constant", CfgSVal);
            }

            /* Use the symbol value */
            E->IVal = GetExportVal (Sym);
            E->Type = ceInt;

            /* Skip the symbol name */
            CfgNextTok ();
            break;

     	case CFGTOK_INTCON:
            /* An integer constant */
            E->IVal = CfgIVal;
            E->Type = ceInt;
	    CfgNextTok ();
       	    break;

	case CFGTOK_STRCON:
	    /* A string constant */
            SB_CopyStr (&E->SVal, CfgSVal);
            E->Type = ceString;
	    CfgNextTok ();
	    break;

        case CFGTOK_PLUS:
            /* Unary plus */
            CfgNextTok ();
            Factor (E);
            CE_AssureInt (E);
            break;

        case CFGTOK_MINUS:
            /* Unary minus */
            CfgNextTok ();
            Factor (E);
            CE_AssureInt (E);
            E->IVal = -E->IVal;
            break;

       	case CFGTOK_LPAR:
            /* Left parenthesis */
       	    CfgNextTok ();
       	    Expr (E);
       	    CfgConsume (CFGTOK_RPAR, "')' expected");
       	    break;

       	default:
       	    CfgError ("Invalid expression: %d", CfgTok);
       	    break;
    }
}



static void Term (CfgExpr* E)
/* Multiplicative operators: * and / */
{
    /* Left operand */
    Factor (E);

    /* Handle multiplicative operators */
    while (CfgTok == CFGTOK_MUL || CfgTok == CFGTOK_DIV) {

        CfgExpr RightSide = CFGEXPR_INITIALIZER;

        /* Remember the token, then skip it */
        cfgtok_t Tok = CfgTok;
        CfgNextTok ();

        /* Left side must be an int */
        CE_AssureInt (E);

        /* Get the right operand and make sure it's an int */
        Factor (&RightSide);
        CE_AssureInt (&RightSide);

        /* Handle the operation */
        switch (Tok) {

            case CFGTOK_MUL:
                E->IVal *= RightSide.IVal;
                break;

            case CFGTOK_DIV:
                if (RightSide.IVal == 0) {
                    CfgError ("Division by zero");
                }
                E->IVal /= RightSide.IVal;
                break;

            default:
                Internal ("Unhandled token in Term: %d", Tok);
        }

        /* Cleanup RightSide (this is not really needed since it may not
         * contain strings at this point, but call it anyway for clarity.
         */
        CE_Done (&RightSide);
    }
}



static void SimpleExpr (CfgExpr* E)
/* Additive operators: + and - */
{
    /* Left operand */
    Term (E);

    /* Handle additive operators */
    while (CfgTok == CFGTOK_PLUS || CfgTok == CFGTOK_MINUS) {

        CfgExpr RightSide = CFGEXPR_INITIALIZER;

        /* Remember the token, then skip it */
        cfgtok_t Tok = CfgTok;
        CfgNextTok ();

        /* Get the right operand */
        Term (&RightSide);

        /* Make sure, left and right side are of the same type */
        if (E->Type != RightSide.Type) {
            CfgError ("Incompatible types in expression");
        }

        /* Handle the operation */
        switch (Tok) {

            case CFGTOK_PLUS:
                /* Plus is defined for strings and ints */
                if (E->Type == ceInt) {
                    E->IVal += RightSide.IVal;
                } else if (E->Type == ceString) {
                    SB_Append (&E->SVal, &RightSide.SVal);
                } else {
                    Internal ("Unhandled type in '+' operator: %u", E->Type);
                }
                break;

            case CFGTOK_MINUS:
                /* Operands must be ints */
                CE_AssureInt (E);
                E->IVal -= RightSide.IVal;
                break;

            default:
                Internal ("Unhandled token in SimpleExpr: %d", Tok);
        }

        /* Cleanup RightSide */
        CE_Done (&RightSide);
    }
}



static void Expr (CfgExpr* E)
/* Full expression */
{
    SimpleExpr (E);
}



long CfgIntExpr (void)
/* Read an expression, make sure it's an int, and return its value */
{
    long Val;

    CfgExpr E = CFGEXPR_INITIALIZER;

    /* Parse the expression */
    Expr (&E);

    /* Make sure it's an integer */
    CE_AssureInt (&E);

    /* Get the value */
    Val = E.IVal;

    /* Cleaup E */
    CE_Done (&E);

    /* Return the value */
    return Val;
}



long CfgCheckedIntExpr (long Min, long Max)
/* Read an expression, make sure it's an int and in range, then return its
 * value.
 */
{
    /* Get the value */
    long Val = CfgIntExpr ();

    /* Check the range */
    if (Val < Min || Val > Max) {
	CfgError ("Range error");
    }

    /* Return the value */
    return Val;
}



