/*****************************************************************************/
/*                                                                           */
/*				     ea.c				     */
/*                                                                           */
/*	     Effective address parsing for the ca65 macroassembler	     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2000 Ullrich von Bassewitz                                       */
/*               Wacholderweg 14                                             */
/*               D-70597 Stuttgart                                           */
/* EMail:        uz@musoftware.de                                            */
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



#include "error.h"
#include "expr.h"
#include "instr.h"
#include "scanner.h"
#include "ea.h"



/*****************************************************************************/
/*     	      	    		     Code				     */
/*****************************************************************************/



void GetEA (unsigned long* AddrMode, ExprNode** Expr, ExprNode** Bank)
/* Parse an effective address, return the possible modes in AddrMode, and the
 * expression involved (if any) in Expr.
 */
{
    /* Clear the expressions */
    *Bank = *Expr = 0;


    if (Tok == TOK_SEP) {

	*AddrMode = AM_IMPLICIT;

    } else if (Tok == TOK_HASH) {

	/* #val */
	NextTok ();
	*Expr = Expression ();
	*AddrMode = AM_IMM;

    } else if (Tok == TOK_A) {

	NextTok ();
	*AddrMode = AM_ACCU;

    } else if (Tok == TOK_LBRACK) {

	/* [dir] or [dir],y */
	NextTok ();
	*Expr = Expression ();
	Consume (TOK_RBRACK, ERR_RBRACK_EXPECTED);
	if (Tok == TOK_COMMA) {
	    /* [dir],y */
	    NextTok ();
	    Consume (TOK_Y, ERR_Y_EXPECTED);
	    *AddrMode = AM_DIR_IND_LONG_Y;
	} else {
	    /* [dir] */
	    *AddrMode = AM_DIR_IND_LONG;
	}

    } else if (Tok == TOK_LPAREN) {

    	/* One of the indirect modes */
    	NextTok ();
    	*Expr = Expression ();

    	if (Tok == TOK_COMMA) {

    	    /* (expr,X) or (rel,S),y */
    	    NextTok ();
    	    if (Tok == TOK_X) {
		/* (adr,x) */
    	 	NextTok ();
       	       	*AddrMode = AM_ABS_X_IND | AM_DIR_X_IND;
    	 	ConsumeRParen ();
    	    } else if (Tok == TOK_S) {
		/* (rel,s),y */
    	 	NextTok ();
    	 	*AddrMode = AM_STACK_REL_IND_Y;
    	 	ConsumeRParen ();
    	 	ConsumeComma ();
    	 	Consume (TOK_Y, ERR_Y_EXPECTED);
    	    } else {
    	 	Error (ERR_SYNTAX);
    	    }

       	} else {

	    /* (adr) or (adr),y */
    	    ConsumeRParen ();
    	    if (Tok == TOK_COMMA) {
		/* (adr),y */
    	 	NextTok ();
    	 	Consume (TOK_Y, ERR_Y_EXPECTED);
    	 	*AddrMode = AM_DIR_IND_Y;
    	    } else {
		/* (adr) */
    	 	*AddrMode = AM_ABS_IND | AM_DIR_IND;
    	    }
    	}

    } else {

	/* Remaining stuff:
	 *
	 * adr
	 * bank.adr
	 * adr,x
	 * bank.adr,x
	 * adr,y
	 * adr,s
	 */
       	*Expr = Expression ();

       	if (Tok == TOK_DOT) {

       	    /* Expr was a bank specification: bank.adr or bank.adr,x */
       	    *Bank = *Expr;
       	    NextTok ();
       	    *Expr = Expression ();
       	    if (Tok == TOK_COMMA) {
       	     	/* bank.adr,x */
       	     	NextTok ();
       	     	Consume (TOK_X, ERR_X_EXPECTED);
       	     	*AddrMode = AM_ABS_LONG_X;
       	    } else {
       	     	/* bank.adr */
       	     	*AddrMode = AM_ABS_LONG;
       	    }

	} else {

	    if (Tok == TOK_COMMA) {

	 	NextTok ();
	 	switch (Tok) {

	 	    case TOK_X:
       	       	       	*AddrMode = AM_ABS_X | AM_DIR_X;
	 		NextTok ();
	 		break;

	 	    case TOK_Y:
	 		*AddrMode = AM_ABS_Y | AM_DIR_Y;
	 		NextTok ();
	 		break;

	 	    case TOK_S:
	 		*AddrMode = AM_STACK_REL;
	 		NextTok ();
	 		break;

	 	    default:
	 		Error (ERR_SYNTAX);

	 	}

	    } else {

	 	*AddrMode = AM_ABS | AM_DIR;

	    }
	}
    }
}



