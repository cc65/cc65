/*****************************************************************************/
/*                                                                           */
/*                                 nexttok.c                                 */
/*                                                                           */
/*              Get next token and handle token level functions              */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000-2011, Ullrich von Bassewitz                                      */
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



#include <stdio.h>
#include <string.h>

/* common */
#include "chartype.h"
#include "check.h"
#include "strbuf.h"

/* ca65 */
#include "condasm.h"
#include "error.h"
#include "expr.h"
#include "global.h"
#include "scanner.h"
#include "toklist.h"
#include "nexttok.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



static unsigned RawMode = 0;            /* Raw token mode flag/counter */



/*****************************************************************************/
/*                              Error handling                               */
/*****************************************************************************/



static int LookAtStrCon (void)
/* Make sure the next token is a string constant. If not, print an error
** messages skip the remainder of the line and return false. Otherwise return
** true.
*/
{
    if (CurTok.Tok != TOK_STRCON) {
        Error ("String constant expected");
        SkipUntilSep ();
        return 0;
    } else {
        return 1;
    }
}



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



static TokList* CollectTokens (unsigned Start, unsigned Count)
/* Read a list of tokens that is optionally enclosed in curly braces and
** terminated by a right paren. For all tokens starting at the one with index
** Start, and ending at (Start+Count-1), place them into a token list, and
** return this token list.
*/
{

    /* Create the token list */
    TokList* List = NewTokList ();

    /* Determine if the list is enclosed in curly braces. */
    token_t Term = GetTokListTerm (TOK_RPAREN);

    /* Read the token list */
    unsigned Current = 0;
    while (CurTok.Tok != Term) {

        /* Check for end of line or end of input */
        if (TokIsSep (CurTok.Tok)) {
            Error ("Unexpected end of line");
            return List;
        }

        /* Collect tokens in the given range */
        if (Current >= Start && Current < Start+Count) {
            /* Add the current token to the list */
            AddCurTok (List);
        }

        /* Get the next token */
        ++Current;
        NextTok ();
    }

    /* Eat the terminator token */
    NextTok ();

    /* If the list was enclosed in curly braces, we do expect now a right paren */
    if (Term == TOK_RCURLY) {
        ConsumeRParen ();
    }

    /* Return the list of collected tokens */
    return List;
}



static void FuncConcat (void)
/* Handle the .CONCAT function */
{
    StrBuf      Buf = STATIC_STRBUF_INITIALIZER;

    /* Skip it */
    NextTok ();

    /* Left paren expected */
    ConsumeLParen ();

    /* Concatenate any number of strings */
    while (1) {

        /* Next token must be a string */
        if (!LookAtStrCon ()) {
            SB_Done (&Buf);
            return;
        }

        /* Append the string */
        SB_Append (&Buf, &CurTok.SVal);

        /* Skip the string token */
        NextTok ();

        /* Comma means another argument */
        if (CurTok.Tok == TOK_COMMA) {
            NextTok ();
        } else {
            /* Done */
            break;
        }
    }

    /* We expect a closing parenthesis, but will not skip it but replace it
    ** by the string token just created.
    */
    if (CurTok.Tok != TOK_RPAREN) {
        Error ("`)' expected");
    } else {
        CurTok.Tok = TOK_STRCON;
        SB_Copy (&CurTok.SVal, &Buf);
        SB_Terminate (&CurTok.SVal);
    }

    /* Free the string buffer */
    SB_Done (&Buf);
}



static void NoIdent (void)
/* Print an error message and skip the remainder of the line */
{
    Error ("Argument of .IDENT is not a valid identifier");
    SkipUntilSep ();
}



static void FuncIdent (void)
/* Handle the .IDENT function */
{
    StrBuf    Buf = STATIC_STRBUF_INITIALIZER;
    token_t   Id;
    unsigned  I;

    /* Skip it */
    NextTok ();

    /* Left paren expected */
    ConsumeLParen ();

    /* The function expects a string argument */
    if (!LookAtStrCon ()) {
        return;
    }

    /* Check that the string contains a valid identifier. While doing so,
    ** determine if it is a cheap local, or global one.
    */
    SB_Reset (&CurTok.SVal);

    /* Check for a cheap local symbol */
    if (SB_Peek (&CurTok.SVal) == LocalStart) {
        SB_Skip (&CurTok.SVal);
        Id = TOK_LOCAL_IDENT;
    } else {
        Id = TOK_IDENT;
    }

    /* Next character must be a valid identifier start */
    if (!IsIdStart (SB_Get (&CurTok.SVal))) {
        NoIdent ();
        return;
    }
    for (I = SB_GetIndex (&CurTok.SVal); I < SB_GetLen (&CurTok.SVal); ++I) {
        if (!IsIdChar (SB_AtUnchecked (&CurTok.SVal, I))) {
            NoIdent ();
            return;
        }
    }
    if (IgnoreCase) {
        UpcaseSVal ();
    }

    /* If anything is ok, save and skip the string. Check that the next token
    ** is a right paren, then replace the token by an identifier token.
    */
    SB_Copy (&Buf, &CurTok.SVal);
    NextTok ();
    if (CurTok.Tok != TOK_RPAREN) {
        Error ("`)' expected");
    } else {
        CurTok.Tok = Id;
        SB_Copy (&CurTok.SVal, &Buf);
        SB_Terminate (&CurTok.SVal);
    }

    /* Free buffer memory */
    SB_Done (&Buf);
}



static void FuncLeft (void)
/* Handle the .LEFT function */
{
    long        Count;
    TokList*    List;

    /* Skip it */
    NextTok ();

    /* Left paren expected */
    ConsumeLParen ();

    /* Count argument. Correct negative counts to zero. */
    Count = ConstExpression ();
    if (Count < 0) {
        Count = 0;
    }
    ConsumeComma ();

    /* Read the token list */
    List = CollectTokens (0, (unsigned) Count);

    /* Since we want to insert the list before the now current token, we have
    ** to save the current token in some way and then skip it. To do this, we
    ** will add the current token at the end of the token list (so the list
    ** will never be empty), push the token list, and then skip the current
    ** token. This will replace the current token by the first token from the
    ** list (which will be the old current token in case the list was empty).
    */
    AddCurTok (List);

    /* Insert it into the scanner feed */
    PushTokList (List, ".LEFT");

    /* Skip the current token */
    NextTok ();
}



static void FuncMid (void)
/* Handle the .MID function */
{
    long        Start;
    long        Count;
    TokList*    List;

    /* Skip it */
    NextTok ();

    /* Left paren expected */
    ConsumeLParen ();

    /* Start argument. Since the start argument can get negative with
    ** expressions like ".tcount(arg)-2", we correct it to zero silently.
    */
    Start = ConstExpression ();
    if (Start < 0 || Start > 100) {
        Start = 0;
    }
    ConsumeComma ();

    /* Count argument. Similar as above, we will accept negative counts and
    ** correct them to zero silently.
    */
    Count = ConstExpression ();
    if (Count < 0) {
        Count = 0;
    }
    ConsumeComma ();

    /* Read the token list */
    List = CollectTokens ((unsigned) Start, (unsigned) Count);

    /* Since we want to insert the list before the now current token, we have
    ** to save the current token in some way and then skip it. To do this, we
    ** will add the current token at the end of the token list (so the list
    ** will never be empty), push the token list, and then skip the current
    ** token. This will replace the current token by the first token from the
    ** list (which will be the old current token in case the list was empty).
    */
    AddCurTok (List);

    /* Insert it into the scanner feed */
    PushTokList (List, ".MID");

    /* Skip the current token */
    NextTok ();
}



static void FuncRight (void)
/* Handle the .RIGHT function */
{
    long        Count;
    TokList*    List;

    /* Skip it */
    NextTok ();

    /* Left paren expected */
    ConsumeLParen ();

    /* Count argument. Correct negative counts to zero. */
    Count = ConstExpression ();
    if (Count < 0) {
        Count = 0;
    }
    ConsumeComma ();

    /* Read the complete token list */
    List = CollectTokens (0, 9999);

    /* Delete tokens from the list until Count tokens are remaining */
    while (List->Count > (unsigned) Count) {
        /* Get the first node */
        TokNode* T = List->Root;

        /* Remove it from the list */
        List->Root = List->Root->Next;

        /* Free the node */
        FreeTokNode (T);

        /* Corrent the token counter */
        List->Count--;
    }

    /* Since we want to insert the list before the now current token, we have
    ** to save the current token in some way and then skip it. To do this, we
    ** will add the current token at the end of the token list (so the list
    ** will never be empty), push the token list, and then skip the current
    ** token. This will replace the current token by the first token from the
    ** list (which will be the old current token in case the list was empty).
    */
    AddCurTok (List);

    /* Insert it into the scanner feed */
    PushTokList (List, ".RIGHT");

    /* Skip the current token */
    NextTok ();
}



static void InvalidFormatString (void)
/* Print an error message and skip the remainder of the line */
{
    Error ("Invalid format string");
    SkipUntilSep ();
}



static void FuncSPrintF (void)
/* Handle the .SPRINTF function */
{
    StrBuf      Format = STATIC_STRBUF_INITIALIZER; /* User supplied format */
    StrBuf      R = STATIC_STRBUF_INITIALIZER;      /* Result string */
    StrBuf      F1 = STATIC_STRBUF_INITIALIZER;     /* One format spec from F */
    StrBuf      R1 = STATIC_STRBUF_INITIALIZER;     /* One result */
    char        C;
    int         Done;
    long        IVal;                               /* Integer value */



    /* Skip the .SPRINTF token */
    NextTok ();

    /* Left paren expected */
    ConsumeLParen ();

    /* First argument is a format string. Remember and skip it */
    if (!LookAtStrCon ()) {
        return;
    }
    SB_Copy (&Format, &CurTok.SVal);
    NextTok ();

    /* Walk over the format string, generating the function result in R */
    while (1) {

        /* Get the next char from the format string and check for EOS */
        if (SB_Peek (&Format) == '\0') {
            break;
        }

        /* Check for a format specifier */
        if (SB_Peek (&Format) != '%') {
            /* No format specifier, just copy */
            SB_AppendChar (&R, SB_Get (&Format));
            continue;
        }
        SB_Skip (&Format);
        if (SB_Peek (&Format) == '%') {
            /* %% */
            SB_AppendChar (&R, '%');
            SB_Skip (&Format);
            continue;
        }
        if (SB_Peek (&Format) == '\0') {
            InvalidFormatString ();
            break;
        }

        /* Since a format specifier follows, we do expect another argument for
        ** the .sprintf function.
        */
        ConsumeComma ();

        /* We will copy the format spec into F1 checking for the things we
        ** support, and later use xsprintf to do the actual formatting. This
        ** is easier than adding another printf implementation...
        */
        SB_Clear (&F1);
        SB_AppendChar (&F1, '%');

        /* Check for flags */
        Done = 0;
        while ((C = SB_Peek (&Format)) != '\0' && !Done) {
            switch (C) {
                case '-': /* FALLTHROUGH */
                case '+': /* FALLTHROUGH */
                case ' ': /* FALLTHROUGH */
                case '#': /* FALLTHROUGH */
                case '0': SB_AppendChar (&F1, SB_Get (&Format));  break;
                default:  Done = 1;                               break;
            }
        }

        /* We do only support a numerical width field */
        while (IsDigit (SB_Peek (&Format))) {
            SB_AppendChar (&F1, SB_Get (&Format));
        }

        /* Precision - only positive numerical fields supported */
        if (SB_Peek (&Format) == '.') {
            SB_AppendChar (&F1, SB_Get (&Format));
            while (IsDigit (SB_Peek (&Format))) {
                SB_AppendChar (&F1, SB_Get (&Format));
            }
        }

        /* Length modifiers aren't supported, so read the conversion specs */
        switch (SB_Peek (&Format)) {

            case 'd':
            case 'i':
            case 'o':
            case 'u':
            case 'X':
            case 'x':
                /* Our ints are actually longs, so we use the 'l' modifier when
                ** calling xsprintf later. Terminate the format string.
                */
                SB_AppendChar (&F1, 'l');
                SB_AppendChar (&F1, SB_Get (&Format));
                SB_Terminate (&F1);

                /* The argument must be a constant expression */
                IVal = ConstExpression ();

                /* Format this argument according to the spec */
                SB_Printf (&R1, SB_GetConstBuf (&F1), IVal);

                /* Append the formatted argument to the result */
                SB_Append (&R, &R1);

                break;

            case 's':
                /* Add the format spec and terminate the format */
                SB_AppendChar (&F1, SB_Get (&Format));
                SB_Terminate (&F1);

                /* The argument must be a string constant */
                if (!LookAtStrCon ()) {
                    /* Make it one */
                    SB_CopyStr (&CurTok.SVal, "**undefined**");
                }

                /* Format this argument according to the spec */
                SB_Printf (&R1, SB_GetConstBuf (&F1), SB_GetConstBuf (&CurTok.SVal));

                /* Skip the string constant */
                NextTok ();

                /* Append the formatted argument to the result */
                SB_Append (&R, &R1);

                break;

            case 'c':
                /* Add the format spec and terminate the format */
                SB_AppendChar (&F1, SB_Get (&Format));
                SB_Terminate (&F1);

                /* The argument must be a constant expression */
                IVal = ConstExpression ();

                /* Check for a valid character range */
                if (IVal <= 0 || IVal > 255) {
                    Error ("Char argument out of range");
                    IVal = ' ';
                }

                /* Format this argument according to the spec. Be sure to pass
                ** an int as the char value.
                */
                SB_Printf (&R1, SB_GetConstBuf (&F1), (int) IVal);

                /* Append the formatted argument to the result */
                SB_Append (&R, &R1);

                break;

            default:
                Error ("Invalid format string");
                SB_Skip (&Format);
                break;
        }

    }

    /* Terminate the result string */
    SB_Terminate (&R);

    /* We expect a closing parenthesis, but will not skip it but replace it
    ** by the string token just created.
    */
    if (CurTok.Tok != TOK_RPAREN) {
        Error ("`)' expected");
    } else {
        CurTok.Tok = TOK_STRCON;
        SB_Copy (&CurTok.SVal, &R);
        SB_Terminate (&CurTok.SVal);
    }


    /* Delete the string buffers */
    SB_Done (&Format);
    SB_Done (&R);
    SB_Done (&F1);
    SB_Done (&R1);
}



static void FuncString (void)
/* Handle the .STRING function */
{
    StrBuf Buf = STATIC_STRBUF_INITIALIZER;

    /* Skip it */
    NextTok ();

    /* Left paren expected */
    ConsumeLParen ();

    /* Accept identifiers or numeric expressions */
    if (CurTok.Tok == TOK_LOCAL_IDENT) {
        /* Save the identifier, then skip it */
        SB_Copy (&Buf, &CurTok.SVal);
        NextTok ();
    } else if (CurTok.Tok == TOK_NAMESPACE || CurTok.Tok == TOK_IDENT) {

        /* Parse a fully qualified symbol name. We cannot use
        ** ParseScopedSymName here since the name may be invalid.
        */
        int NameSpace;
        do {
            NameSpace = (CurTok.Tok == TOK_NAMESPACE);
            if (NameSpace) {
                SB_AppendStr (&Buf, "::");
            } else {
                SB_Append (&Buf, &CurTok.SVal);
            }
            NextTok ();
        } while ((NameSpace != 0 && CurTok.Tok == TOK_IDENT) ||
                 (NameSpace == 0 && CurTok.Tok == TOK_NAMESPACE));

    } else {
        /* Numeric expression */
        long Val = ConstExpression ();
        SB_Printf (&Buf, "%ld", Val);
    }

    /* We expect a closing parenthesis, but will not skip it but replace it
    ** by the string token just created.
    */
    if (CurTok.Tok != TOK_RPAREN) {
        Error ("`)' expected");
    } else {
        CurTok.Tok = TOK_STRCON;
        SB_Copy (&CurTok.SVal, &Buf);
        SB_Terminate (&CurTok.SVal);
    }

    /* Free string memory */
    SB_Done (&Buf);
}



void NextTok (void)
/* Get next token and handle token level functions */
{
    /* Get the next raw token */
    NextRawTok ();

    /* In raw mode, or when output is suppressed via conditional assembly,
    ** pass the token unchanged.
    */
    if (RawMode == 0 && IfCond) {

        /* Execute token handling functions */
        switch (CurTok.Tok) {

            case TOK_CONCAT:
                FuncConcat ();
                break;

            case TOK_LEFT:
                FuncLeft ();
                break;

            case TOK_MAKEIDENT:
                FuncIdent ();
                break;

            case TOK_MID:
                FuncMid ();
                break;

            case TOK_RIGHT:
                FuncRight ();
                break;

            case TOK_SPRINTF:
                FuncSPrintF ();
                break;

            case TOK_STRING:
                FuncString ();
                break;

            default:
                /* Quiet down gcc */
                break;

        }
    }
}



void Consume (token_t Expected, const char* ErrMsg)
/* Consume Expected, print an error if we don't find it */
{
    if (CurTok.Tok == Expected) {
        NextTok ();
    } else {
        Error ("%s", ErrMsg);
    }
}



void ConsumeSep (void)
/* Consume a separator token */
{
    /* We expect a separator token */
    ExpectSep ();

    /* If we are at end of line, skip it */
    if (CurTok.Tok == TOK_SEP) {
        NextTok ();
    }
}



void ConsumeLParen (void)
/* Consume a left paren */
{
    Consume (TOK_LPAREN, "`(' expected");
}



void ConsumeRParen (void)
/* Consume a right paren */
{
    Consume (TOK_RPAREN, "`)' expected");
}



void ConsumeComma (void)
/* Consume a comma */
{
    Consume (TOK_COMMA, "`,' expected");
}



void SkipUntilSep (void)
/* Skip tokens until we reach a line separator or end of file */
{
    while (!TokIsSep (CurTok.Tok)) {
        NextTok ();
    }
}



void ExpectSep (void)
/* Check if we've reached a line separator, and output an error if not. Do
** not skip the line separator.
*/
{
    if (!TokIsSep (CurTok.Tok)) {
        ErrorSkip ("Unexpected trailing garbage characters");
    }
}



void EnterRawTokenMode (void)
/* Enter raw token mode. In raw mode, token handling functions are not
** executed, but the function tokens are passed untouched to the upper
** layer. Raw token mode is used when storing macro tokens for later
** use.
** Calls to EnterRawTokenMode and LeaveRawTokenMode may be nested.
*/
{
    ++RawMode;
}



void LeaveRawTokenMode (void)
/* Leave raw token mode. */
{
    PRECONDITION (RawMode > 0);
    --RawMode;
}
