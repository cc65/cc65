/*****************************************************************************/
/*                                                                           */
/*	  			   easw16.c                                  */
/*                                                                           */
/*       SWEET16 effective address parsing for the ca65 macroassembler       */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2004      Ullrich von Bassewitz                                       */
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



/* ca65 */
#include "ea.h"
#include "ea65.h"
#include "error.h"
#include "expr.h"
#include "instr.h"
#include "nexttok.h"



/*****************************************************************************/
/*     	       	    	      	     Code				     */
/*****************************************************************************/



void GetSweet16EA (EffAddr* A)
/* Parse an effective address, return the result in A */
{
    /* Clear the output struct */
    A->AddrModeSet = 0;
    A->Expr = 0;
    A->Reg  = 0;

    /* Parse the effective address */
    if (TokIsSep (Tok)) {

       	A->AddrModeSet = AMSW16_IMP;

    } else if (Tok == TOK_AT) {

	/* @reg */
	A->AddrModeSet = AMSW16_IND;
	NextTok ();
        if (Tok != TOK_REG) {
            ErrorSkip ("Register expected");
            A->Reg = 0;
        } else {
            A->Reg = (unsigned) IVal;
            NextTok ();
        }

    } else if (Tok == TOK_REG) {

        A->Reg = (unsigned) IVal;
        NextTok ();

        if (Tok == TOK_COMMA) {

            /* Rx, Constant */
            NextTok ();
            A->Expr = Expression ();

            A->AddrModeSet = AMSW16_IMM;

        } else {

            A->AddrModeSet = AMSW16_REG;

        }

    } else {

        /* OPC  ea */
        A->Expr = Expression ();
        A->AddrModeSet = AMSW16_BRA;

    }
}



