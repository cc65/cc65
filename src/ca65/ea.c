/*****************************************************************************/
/*                                                                           */
/*				     ea.c				     */
/*                                                                           */
/*	     Effective address parsing for the ca65 macroassembler	     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2003 Ullrich von Bassewitz                                       */
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



/* ld65 */
#include "error.h"
#include "expr.h"
#include "instr.h"
#include "nexttok.h"
#include "ea.h"



/*****************************************************************************/
/*     	      	    	      	     Code				     */
/*****************************************************************************/



void GetEA (EffAddr* A)
/* Parse an effective address, return the result in A */
{
    unsigned long Restrictions;

    /* Clear the output struct */
    A->AddrModeSet = 0;
    A->Bank = 0;
    A->Expr = 0;

    /* Handle an addressing size override */
    switch (Tok) {
        case TOK_OVERRIDE_ZP:
            Restrictions = AM_DIR | AM_DIR_X | AM_DIR_Y;
            NextTok ();
            break;

        case TOK_OVERRIDE_ABS:
            Restrictions = AM_ABS | AM_ABS_X | AM_ABS_Y;
            NextTok ();
            break;

        case TOK_OVERRIDE_FAR:
            Restrictions = AM_ABS_LONG | AM_ABS_LONG_X;
            NextTok ();
            break;

        default:
            Restrictions = ~0UL;        /* None */
            break;
    }

    /* Parse the effective address */
    if (TokIsSep (Tok)) {

	A->AddrModeSet = AM_IMPLICIT;

    } else if (Tok == TOK_HASH) {

	/* #val */
	NextTok ();
	A->Expr  = Expression ();
	A->AddrModeSet = AM_IMM;

    } else if (Tok == TOK_A) {

	NextTok ();
	A->AddrModeSet = AM_ACCU;

    } else if (Tok == TOK_LBRACK) {

	/* [dir] or [dir],y */
	NextTok ();
	A->Expr = Expression ();
	Consume (TOK_RBRACK, "']' expected");
	if (Tok == TOK_COMMA) {
	    /* [dir],y */
	    NextTok ();
	    Consume (TOK_Y, "`Y' expected");
	    A->AddrModeSet = AM_DIR_IND_LONG_Y;
	} else {
	    /* [dir] */
	    A->AddrModeSet = AM_DIR_IND_LONG;
	}

    } else if (Tok == TOK_LPAREN) {

    	/* One of the indirect modes */
    	NextTok ();
    	A->Expr = Expression ();

    	if (Tok == TOK_COMMA) {

    	    /* (expr,X) or (rel,S),y */
    	    NextTok ();
    	    if (Tok == TOK_X) {
	   	/* (adr,x) */
    	   	NextTok ();
       	       	A->AddrModeSet = AM_ABS_X_IND | AM_DIR_X_IND;
    	       	ConsumeRParen ();
    	    } else if (Tok == TOK_S) {
	   	/* (rel,s),y */
    	 	NextTok ();
    	 	A->AddrModeSet = AM_STACK_REL_IND_Y;
    	 	ConsumeRParen ();
    	 	ConsumeComma ();
    	 	Consume (TOK_Y, "`Y' expected");
    	    } else {
    	 	Error ("Syntax error");
    	    }

       	} else {

	    /* (adr) or (adr),y */
    	    ConsumeRParen ();
    	    if (Tok == TOK_COMMA) {
		/* (adr),y */
    	 	NextTok ();
    	 	Consume (TOK_Y, "`Y' expected");
    	 	A->AddrModeSet = AM_DIR_IND_Y;
    	    } else {
		/* (adr) */
    	 	A->AddrModeSet = AM_ABS_IND | AM_DIR_IND;
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
       	A->Expr = Expression ();

       	if (Tok == TOK_DOT) {

       	    /* Expr was a bank specification: bank.adr or bank.adr,x */
       	    A->Bank = A->Expr;
       	    NextTok ();
       	    A->Expr = Expression ();
       	    if (Tok == TOK_COMMA) {
       	       	/* bank.adr,x */
       	       	NextTok ();
       	       	Consume (TOK_X, "`X' expected");
       	       	A->AddrModeSet = AM_ABS_LONG_X;
       	    } else {
       	     	/* bank.adr */
       	     	A->AddrModeSet = AM_ABS_LONG;
       	    }

	} else {

	    if (Tok == TOK_COMMA) {

	 	NextTok ();
	 	switch (Tok) {

	 	    case TOK_X:
       	       	       	A->AddrModeSet = AM_ABS_LONG_X | AM_ABS_X | AM_DIR_X;
	 		NextTok ();
	 		break;

	 	    case TOK_Y:
	 		A->AddrModeSet = AM_ABS_Y | AM_DIR_Y;
	 		NextTok ();
	 		break;

	 	    case TOK_S:
	 		A->AddrModeSet = AM_STACK_REL;
	 		NextTok ();
	 		break;

	 	    default:
	 		Error ("Syntax error");

	 	}

	    } else {

	 	A->AddrModeSet = AM_ABS_LONG | AM_ABS | AM_DIR;

	    }
	}
    }

    /* Apply addressing mode overrides */
    A->AddrModeSet &= Restrictions;
}



