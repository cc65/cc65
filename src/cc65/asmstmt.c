/*****************************************************************************/
/*                                                                           */
/*                                 asmstmt.c                                 */
/*                                                                           */
/*            Inline assembler statements for the cc65 C compiler            */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2001-2008 Ullrich von Bassewitz                                       */
/*               Roemerstrasse 52                                            */
/*               D-70794 Filderstadt                                         */
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



#include <string.h>

/* common */
#include "xsprintf.h"

/* cc65 */
#include "asmlabel.h"
#include "codegen.h"
#include "datatype.h"
#include "error.h"
#include "expr.h"
#include "function.h"
#include "litpool.h"
#include "scanner.h"
#include "stackptr.h"
#include "symtab.h"
#include "asmstmt.h"



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



static void AsmRangeError (unsigned Arg)
/* Print a diagnostic about a range error in the argument with the given number */
{
    Error ("Range error in argument %u", Arg);
}



static void AsmErrorSkip (void)
/* Called in case of an error, skips tokens until the closing paren or a
** semicolon is reached.
*/
{
    static const token_t TokenList[] = { TOK_RPAREN, TOK_SEMI };
    SkipTokens (TokenList, sizeof(TokenList) / sizeof(TokenList[0]));
}



static SymEntry* AsmGetSym (unsigned Arg, unsigned Type)
/* Find the symbol with the name currently in NextTok. The symbol must be of
** the given type. On errors, NULL is returned.
*/
{
    SymEntry* Sym;

    /* We expect an argument separated by a comma */
    ConsumeComma ();

    /* Argument must be an identifier */
    if (CurTok.Tok != TOK_IDENT) {
        Error ("Identifier expected for argument %u", Arg);
        AsmErrorSkip ();
        return 0;
    }

    /* Get a pointer to the symbol table entry */
    Sym = FindSym (CurTok.Ident);

    /* Did we find a symbol with this name? */
    if (Sym == 0) {
        Error ("Undefined symbol `%s' for argument %u", CurTok.Ident, Arg);
        AsmErrorSkip ();
        return 0;
    }

    /* We found the symbol - skip the name token */
    NextToken ();

    /* Check if we have a global symbol */
    if ((Sym->Flags & Type) != Type) {
        Error ("Type of argument %u differs from format specifier", Arg);
        AsmErrorSkip ();
        return 0;
    }

    /* Mark the symbol as referenced */
    Sym->Flags |= SC_REF;

    /* Return it */
    return Sym;
}



static void ParseByteArg (StrBuf* T, unsigned Arg)
/* Parse the %b format specifier */
{
    ExprDesc Expr;
    char     Buf [16];

    /* We expect an argument separated by a comma */
    ConsumeComma ();

    /* Evaluate the expression */
    ConstAbsIntExpr (hie1, &Expr);

    /* Check the range but allow negative values if the type is signed */
    if (IsSignUnsigned (Expr.Type)) {
        if (Expr.IVal < 0 || Expr.IVal > 0xFF) {
            AsmRangeError (Arg);
            Expr.IVal = 0;
        }
    } else {
        if (Expr.IVal < -128 || Expr.IVal > 127) {
            AsmRangeError (Arg);
            Expr.IVal = 0;
        }
    }

    /* Convert into a hex number */
    xsprintf (Buf, sizeof (Buf), "$%02lX", Expr.IVal & 0xFF);

    /* Add the number to the target buffer */
    SB_AppendStr (T, Buf);
}



static void ParseWordArg (StrBuf* T, unsigned Arg)
/* Parse the %w format specifier */
{
    ExprDesc Expr;
    char     Buf [16];

    /* We expect an argument separated by a comma */
    ConsumeComma ();

    /* Evaluate the expression */
    ConstAbsIntExpr (hie1, &Expr);

    /* Check the range but allow negative values if the type is signed */
    if (IsSignUnsigned (Expr.Type)) {
        if (Expr.IVal < 0 || Expr.IVal > 0xFFFF) {
            AsmRangeError (Arg);
            Expr.IVal = 0;
        }
    } else {
        if (Expr.IVal < -32768 || Expr.IVal > 32767) {
            AsmRangeError (Arg);
            Expr.IVal = 0;
        }
    }

    /* Convert into a hex number */
    xsprintf (Buf, sizeof (Buf), "$%04lX", Expr.IVal & 0xFFFF);

    /* Add the number to the target buffer */
    SB_AppendStr (T, Buf);
}



static void ParseLongArg (StrBuf* T, unsigned Arg attribute ((unused)))
/* Parse the %l format specifier */
{
    ExprDesc Expr;
    char     Buf [16];

    /* We expect an argument separated by a comma */
    ConsumeComma ();

    /* Evaluate the expression */
    ConstAbsIntExpr (hie1, &Expr);

    /* Convert into a hex number */
    xsprintf (Buf, sizeof (Buf), "$%08lX", Expr.IVal & 0xFFFFFFFF);

    /* Add the number to the target buffer */
    SB_AppendStr (T, Buf);
}



static void ParseGVarArg (StrBuf* T, unsigned Arg)
/* Parse the %v format specifier */
{
    /* Parse the symbol name parameter and check the type */
    SymEntry* Sym = AsmGetSym (Arg, SC_STATIC);
    if (Sym == 0) {
        /* Some sort of error */
        return;
    }

    /* Check for external linkage */
    if (Sym->Flags & (SC_EXTERN | SC_STORAGE | SC_FUNC)) {
        /* External linkage or a function */
        /* ### FIXME: Asm name should be generated by codegen */
        SB_AppendChar (T, '_');
        SB_AppendStr (T, Sym->Name);
    } else if (Sym->Flags & SC_REGISTER) {
        char Buf[32];
        xsprintf (Buf, sizeof (Buf), "regbank+%d", Sym->V.R.RegOffs);
        SB_AppendStr (T, Buf);
    } else {
        /* Static variable */
        char Buf [16];
        xsprintf (Buf, sizeof (Buf), "L%04X", Sym->V.Label);
        SB_AppendStr (T, Buf);
    }
}



static void ParseLVarArg (StrBuf* T, unsigned Arg)
/* Parse the %o format specifier */
{
    unsigned Offs;
    char Buf [16];

    /* Parse the symbol name parameter and check the type */
    SymEntry* Sym = AsmGetSym (Arg, SC_AUTO);
    if (Sym == 0) {
        /* Some sort of error */
        return;
    }

    /* The symbol may be a parameter to a variadic function. In this case, we
    ** don't have a fixed stack offset, so check it and bail out with an error
    ** if this is the case.
    */
    if ((Sym->Flags & SC_PARAM) == SC_PARAM && F_IsVariadic (CurrentFunc)) {
        Error ("Argument %u has no fixed stack offset", Arg);
        AsmErrorSkip ();
        return;
    }

    /* Calculate the current offset from SP */
    Offs = Sym->V.Offs - StackPtr;

    /* Output the offset */
    xsprintf (Buf, sizeof (Buf), (Offs > 0xFF)? "$%04X" : "$%02X", Offs);
    SB_AppendStr (T, Buf);
}



static void ParseLabelArg (StrBuf* T, unsigned Arg attribute ((unused)))
/* Parse the %g format specifier */
{
    /* We expect an identifier separated by a comma */
    ConsumeComma ();
    if (CurTok.Tok != TOK_IDENT) {

        Error ("Label name expected");

    } else {

        /* Add a new label symbol if we don't have one until now */
        SymEntry* Entry = AddLabelSym (CurTok.Ident, SC_REF);

        /* Append the label name to the buffer */
        SB_AppendStr (T, LocalLabelName (Entry->V.Label));

        /* Eat the label name */
        NextToken ();

    }
}



static void ParseStrArg (StrBuf* T, unsigned Arg attribute ((unused)))
/* Parse the %s format specifier */
{
    ExprDesc Expr;
    char Buf [64];

    /* We expect an argument separated by a comma */
    ConsumeComma ();

    /* Check what comes */
    switch (CurTok.Tok) {

        case TOK_IDENT:
            /* Identifier */
            SB_AppendStr (T, CurTok.Ident);
            NextToken ();
            break;

        case TOK_SCONST:
            /* String constant */
            SB_Append (T, GetLiteralStrBuf (CurTok.SVal));
            NextToken ();
            break;

        default:
            ConstAbsIntExpr (hie1, &Expr);
            xsprintf (Buf, sizeof (Buf), "%ld", Expr.IVal);
            SB_AppendStr (T, Buf);
            break;
    }
}



static void ParseAsm (void)
/* Parse the contents of the ASM statement */
{
    unsigned Arg;
    char     C;

    /* Create a target string buffer */
    StrBuf T = AUTO_STRBUF_INITIALIZER;

    /* Create a string buffer from the string literal */
    StrBuf S = AUTO_STRBUF_INITIALIZER;
    SB_Append (&S, GetLiteralStrBuf (CurTok.SVal));

    /* Skip the string token */
    NextToken ();

    /* Parse the statement. It may contain several lines and one or more
    ** of the following place holders:
    **   %b     - Numerical 8 bit value
    **   %w     - Numerical 16 bit value
    **   %l     - Numerical 32 bit value
    **   %v     - Assembler name of a (global) variable
    **   %o     - Stack offset of a (local) variable
    **   %g     - Assembler name of a C label
    **   %s     - Any argument converted to a string (almost)
    **   %%     - The % sign
    */
    Arg = 0;
    while ((C = SB_Get (&S)) != '\0') {

        /* If it is a newline, the current line is ready to go */
        if (C == '\n') {

            /* Pass it to the backend and start over */
            g_asmcode (&T);
            SB_Clear (&T);

        } else if (C == '%') {

            /* Format specifier */
            ++Arg;
            C = SB_Get (&S);
            switch (C) {
                case '%':   SB_AppendChar (&T, '%');    break;
                case 'b':   ParseByteArg (&T, Arg);     break;
                case 'g':   ParseLabelArg (&T, Arg);    break;
                case 'l':   ParseLongArg (&T, Arg);     break;
                case 'o':   ParseLVarArg (&T, Arg);     break;
                case 's':   ParseStrArg (&T, Arg);      break;
                case 'v':   ParseGVarArg (&T, Arg);     break;
                case 'w':   ParseWordArg (&T, Arg);     break;
                default:
                    Error ("Error in __asm__ format specifier %u", Arg);
                    AsmErrorSkip ();
                    goto Done;
            }

        } else {

            /* A normal character, just copy it */
            SB_AppendChar (&T, C);

        }
    }

    /* If the target buffer is not empty, we have a last line in there */
    if (!SB_IsEmpty (&T)) {
        g_asmcode (&T);
    }

Done:
    /* Call the string buf destructors */
    SB_Done (&S);
    SB_Done (&T);
}



void AsmStatement (void)
/* This function parses ASM statements. The syntax of the ASM directive
** looks like the one defined for C++ (C has no ASM directive), that is,
** a string literal in parenthesis.
*/
{
    /* Skip the ASM */
    NextToken ();

    /* Need left parenthesis */
    if (!ConsumeLParen ()) {
        return;
    }

    /* String literal */
    if (CurTok.Tok != TOK_SCONST) {

        /* Print a diagnostic */
        Error ("String literal expected");

        /* Try some smart error recovery: Skip tokens until we reach the
        ** enclosing paren, or a semicolon.
        */
        AsmErrorSkip ();

    } else {

        /* Parse the ASM statement */
        ParseAsm ();
    }

    /* Closing paren needed */
    ConsumeRParen ();
}
