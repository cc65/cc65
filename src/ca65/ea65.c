/*****************************************************************************/
/*                                                                           */
/*                                  ea65.c                                   */
/*                                                                           */
/*        65XX effective address parsing for the ca65 macroassembler         */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2011, Ullrich von Bassewitz                                      */
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
#include "global.h"



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



void GetEA (EffAddr* A)
/* Parse an effective address, return the result in A */
{
    unsigned long Restrictions;
    token_t IndirectEnter;
    token_t IndirectLeave;
    const char* IndirectExpect;

    /* Choose syntax for indirection */
    if (BracketAsIndirect) {
        IndirectEnter = TOK_LBRACK;
        IndirectLeave = TOK_RBRACK;
        IndirectExpect = "']' expected";
    } else {
        IndirectEnter = TOK_LPAREN;
        IndirectLeave = TOK_RPAREN;
        IndirectExpect = "')' expected";
    }

    /* Clear the output struct */
    A->AddrModeSet = 0;
    A->Expr = 0;
    A->Flags = 0;

    /* Handle an addressing size override */
    switch (CurTok.Tok) {
        case TOK_OVERRIDE_ZP:
            Restrictions = AM65_DIR | AM65_DIR_X | AM65_DIR_Y;
            A->Flags |= EFFADDR_OVERRIDE_ZP;
            NextTok ();
            break;

        case TOK_OVERRIDE_ABS:
            Restrictions = AM65_ABS | AM65_ABS_X | AM65_ABS_Y;
            NextTok ();
            break;

        case TOK_OVERRIDE_FAR:
            Restrictions = AM65_ABS_LONG | AM65_ABS_LONG_X;
            NextTok ();
            break;

        default:
            Restrictions = ~0UL;        /* None */
            break;
    }

    /* Parse the effective address */
    if (TokIsSep (CurTok.Tok)) {

        A->AddrModeSet = AM65_IMPLICIT;

    } else if (CurTok.Tok == TOK_HASH) {

        /* #val */
        NextTok ();
        A->Expr  = Expression ();
        A->AddrModeSet = AM65_ALL_IMM;

    } else if (CurTok.Tok == TOK_A) {

        NextTok ();
        A->AddrModeSet = AM65_ACCU;

    } else if (CurTok.Tok == IndirectEnter) {

        /* One of the indirect modes */
        NextTok ();
        A->Expr = Expression ();

        if (CurTok.Tok == TOK_COMMA) {

            /* (expr,X) or (rel,S),y */
            NextTok ();
            if (CurTok.Tok == TOK_X) {
                /* (adr,x) */
                NextTok ();
                A->AddrModeSet = AM65_ABS_X_IND | AM65_DIR_X_IND;
                Consume (IndirectLeave, IndirectExpect);
            } else if (CurTok.Tok == TOK_S) {
                /* (rel,s),y */
                NextTok ();
                A->AddrModeSet = AM65_STACK_REL_IND_Y;
                Consume (IndirectLeave, IndirectExpect);
                ConsumeComma ();
                Consume (TOK_Y, "'Y' expected");
            } else {
                Error ("Syntax error");
            }

        } else {

            /* (adr), (adr),y or (adr),z */
            Consume (IndirectLeave, IndirectExpect);
            if (CurTok.Tok == TOK_COMMA) {
                /* (adr),y */
                NextTok ();
                switch (CurTok.Tok) {
                case TOK_Z:
                    /* only set by scanner.c if in 4510-mode */
                    NextTok ();
                    A->AddrModeSet = AM65_DIR_IND;
                    break;
                default:
                    Consume (TOK_Y, "'Y' expected");
                    A->AddrModeSet = AM65_DIR_IND_Y;
                    break;
                }
            } else {
                /* (adr) */
                A->AddrModeSet = (CPU == CPU_4510) ? AM65_ABS_IND
                                                   : AM65_ABS_IND | AM65_ABS_IND_LONG | AM65_DIR_IND;
            }
        }

    } else if (CurTok.Tok == TOK_LBRACK) {

        /* Never executed if BracketAsIndirect feature is enabled. */
        /* [dir] or [dir],y */
        NextTok ();
        A->Expr = Expression ();
        Consume (TOK_RBRACK, "']' expected");
        if (CurTok.Tok == TOK_COMMA) {
            /* [dir],y */
            NextTok ();
            Consume (TOK_Y, "'Y' expected");
            A->AddrModeSet = AM65_DIR_IND_LONG_Y;
        } else {
            /* [dir] */
            A->AddrModeSet = AM65_DIR_IND_LONG | AM65_ABS_IND_LONG;
        }

    } else {

        /* Remaining stuff:
        **
        ** adr
        ** adr,x
        ** adr,y
        ** adr,s
        */
        A->Expr = Expression ();

        if (CurTok.Tok == TOK_COMMA) {

            NextTok ();
            switch (CurTok.Tok) {

                case TOK_X:
                    A->AddrModeSet = AM65_ABS_LONG_X | AM65_ABS_X | AM65_DIR_X;
                    NextTok ();
                    break;

                case TOK_Y:
                    A->AddrModeSet = AM65_ABS_Y | AM65_DIR_Y;
                    NextTok ();
                    break;

                case TOK_S:
                    A->AddrModeSet = AM65_STACK_REL;
                    NextTok ();
                    break;

                default:
                    Error ("Syntax error");

            }

        } else {

            A->AddrModeSet = AM65_ABS_LONG | AM65_ABS | AM65_DIR;

        }
    }

    /* Apply addressing mode overrides */
    A->AddrModeSet &= Restrictions;
}
