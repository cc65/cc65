/*****************************************************************************/
/*                                                                           */
/*                                 easw16.c                                  */
/*                                                                           */
/*       SWEET16 effective address parsing for the ca65 macroassembler       */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2004-2011, Ullrich von Bassewitz                                      */
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



/* ca65 */
#include "ea.h"
#include "ea65.h"
#include "error.h"
#include "expr.h"
#include "instr.h"
#include "nexttok.h"



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



static long RegNum ()
/* Try to read a register number specified not as a register (Rx) but as a
** numeric value between 0 and 15. Return the register number or -1 on
** failure.
*/
{
    long Val;
    ExprNode* Expr = Expression ();
    if (!IsConstExpr (Expr, &Val) || Val < 0 || Val > 15) {
        /* Invalid register */
        Val = -1L;
    }

    /* Free the expression and return the register number */
    FreeExpr (Expr);
    return Val;
}



void GetSweet16EA (EffAddr* A)
/* Parse an effective address, return the result in A */
{
    long Reg;

    /* Clear the output struct */
    A->AddrModeSet = 0;
    A->Expr = 0;
    A->Reg  = 0;

    /* Parse the effective address */
    if (TokIsSep (CurTok.Tok)) {

        A->AddrModeSet = AMSW16_IMP;

    } else if (CurTok.Tok == TOK_AT) {

        /* @reg or @regnumber */
        A->AddrModeSet = AMSW16_IND;
        NextTok ();
        if (CurTok.Tok == TOK_REG) {
            A->Reg = (unsigned) CurTok.IVal;
            NextTok ();
        } else if ((Reg = RegNum ()) >= 0) {
            /* Register number */
            A->Reg = (unsigned) Reg;
        } else {
            ErrorSkip ("Register or register number expected");
            A->Reg = 0;
        }

    } else if (CurTok.Tok == TOK_REG) {

        A->Reg = (unsigned) CurTok.IVal;
        NextTok ();

        if (CurTok.Tok == TOK_COMMA) {

            /* Rx, constant */
            NextTok ();
            A->Expr = Expression ();

            A->AddrModeSet = AMSW16_IMM;

        } else {

            A->AddrModeSet = AMSW16_REG;

        }

    } else {

        /* OPC ea  or: OPC regnum, constant */
        A->Expr = Expression ();
        A->AddrModeSet = AMSW16_BRA;

        /* If the value is a constant between 0 and 15, it may also be a
        ** register number.
        */
        if (IsConstExpr (A->Expr, &Reg) && Reg >= 0 && Reg <= 15) {
            FreeExpr (A->Expr);
            A->Reg = (unsigned) Reg;

            /* If a comma follows, it is: OPC Rx, constant */
            if (CurTok.Tok == TOK_COMMA) {
                NextTok ();
                A->Expr = Expression ();
                A->AddrModeSet = AMSW16_IMM;
            } else {
                A->Expr = 0;
                A->AddrModeSet |= AMSW16_REG;
            }
        }

    }
}


                                   
