/*****************************************************************************/
/*                                                                           */
/*			 	   nexttok.c				     */
/*                                                                           */
/*		Get next token and handle token level functions		     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000-2004 Ullrich von Bassewitz                                       */
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



#include <stdio.h>
#include <string.h>

/* common */
#include "chartype.h"
#include "check.h"
#include "strbuf.h"

/* ca65 */
#include "error.h"
#include "expr.h"
#include "global.h"
#include "scanner.h"
#include "toklist.h"
#include "nexttok.h"



/*****************************************************************************/
/*     	       	    		     Data			   	     */
/*****************************************************************************/



static unsigned RawMode = 0;		/* Raw token mode flag/counter */



/*****************************************************************************/
/*                              Error handling                               */
/*****************************************************************************/



static int LookAtStrCon (void)
/* Make sure the next token is a string constant. If not, print an error
 * messages skip the remainder of the line and return false. Otherwise return
 * true.
 */
{
    if (Tok != TOK_STRCON) {
        Error ("String constant expected");
        SkipUntilSep ();
        return 0;
    } else {
        return 1;
    }
}



/*****************************************************************************/
/*     	       	    		     Code			   	     */
/*****************************************************************************/



static TokList* CollectTokens (unsigned Start, unsigned Count)
/* Read a list of tokens that is optionally enclosed in curly braces and
 * terminated by a right paren. For all tokens starting at the one with index
 * Start, and ending at (Start+Count-1), place them into a token list, and
 * return this token list.
 */
{

    /* Create the token list */
    TokList* List = NewTokList ();

    /* Determine if the list is enclosed in curly braces. */
    enum Token Term = GetTokListTerm (TOK_RPAREN);

    /* Read the token list */
    unsigned Current = 0;
    while (Tok != Term) {

     	/* Check for end of line or end of input */
     	if (TokIsSep (Tok)) {
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
    char    	Buf[MAX_STR_LEN+1];
    char*	B;
    unsigned 	Length;
    unsigned	L;

    /* Skip it */
    NextTok ();

    /* Left paren expected */
    ConsumeLParen ();

    /* Concatenate any number of strings */
    B = Buf;
    B[0] = '\0';
    Length = 0;
    while (1) {

     	/* Next token must be a string */
        if (!LookAtStrCon ()) {
     	    return;
     	}

     	/* Get the length of the string const and check total length */
     	L = strlen (SVal);
     	if (Length + L > MAX_STR_LEN) {
     	    Error ("String is too long");
     	    /* Try to recover */
     	    SkipUntilSep ();
     	    return;
     	}

     	/* Add the new string */
     	memcpy (B, SVal, L);
     	Length += L;
     	B      += L;

     	/* Skip the string token */
     	NextTok ();

     	/* Comma means another argument */
     	if (Tok == TOK_COMMA) {
     	    NextTok ();
     	} else {
     	    /* Done */
     	    break;
     	}
    }

    /* Terminate the string */
    *B = '\0';

    /* We expect a closing parenthesis, but will not skip it but replace it
     * by the string token just created.
     */
    if (Tok != TOK_RPAREN) {
     	Error ("`)' expected");
    } else {
     	Tok = TOK_STRCON;
     	strcpy (SVal, Buf);
    }
}



static void FuncLeft (void)
/* Handle the .LEFT function */
{
    long       	Count;
    TokList* 	List;

    /* Skip it */
    NextTok ();

    /* Left paren expected */
    ConsumeLParen ();

    /* Count argument */
    Count = ConstExpression ();
    if (Count < 0 || Count > 100) {
     	Error ("Range error");
     	Count = 1;
    }
    ConsumeComma ();

    /* Read the token list */
    List = CollectTokens (0, (unsigned) Count);

    /* Since we want to insert the list before the now current token, we have
     * to save the current token in some way and then skip it. To do this, we
     * will add the current token at the end of the token list (so the list
     * will never be empty), push the token list, and then skip the current
     * token. This will replace the current token by the first token from the
     * list (which will be the old current token in case the list was empty).
     */
    AddCurTok (List);

    /* Insert it into the scanner feed */
    PushTokList (List, ".LEFT");

    /* Skip the current token */
    NextTok ();
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
    char       Buf[sizeof(SVal)];
    enum Token Id;
    unsigned   Len;
    unsigned   I;

    /* Skip it */
    NextTok ();

    /* Left paren expected */
    ConsumeLParen ();

    /* The function expects a string argument */
    if (!LookAtStrCon ()) {
        return;
    }

    /* Check that the string contains a valid identifier. While doing so,
     * determine if it is a cheap local, or global one.
     */
    Len = strlen (SVal);
    if (Len == 0) {
        NoIdent ();
        return;
    }
    I = 0;
    if (SVal[0] == LocalStart) {
        if (Len < 2) {
            NoIdent ();
            return;
        }
        I = 1;
        Id = TOK_LOCAL_IDENT;
    } else {
        Id = TOK_IDENT;
    }
    if (!IsIdStart (SVal[I])) {
        NoIdent ();
        return;
    }
    while (I < Len) {
        if (!IsIdChar (SVal[I])) {
            NoIdent ();
            return;
        }
        ++I;
    }
    if (IgnoreCase) {
        UpcaseSVal ();
    }

    /* If anything is ok, save and skip the string. Check that the next token
     * is a right paren, in which case SVal is untouched. Replace the token by
     * a identifier token.
     */
    memcpy (Buf, SVal, Len+1);
    NextTok ();
    if (Tok != TOK_RPAREN) {
     	Error ("`)' expected");
    } else {
        Tok = Id;
        memcpy (SVal, Buf, Len+1);
    }
}



static void FuncMid (void)
/* Handle the .MID function */
{
    long    	Start;
    long       	Count;
    TokList* 	List;

    /* Skip it */
    NextTok ();

    /* Left paren expected */
    ConsumeLParen ();

    /* Start argument */
    Start = ConstExpression ();
    if (Start < 0 || Start > 100) {
     	Error ("Range error");
     	Start = 0;
    }
    ConsumeComma ();

    /* Count argument */
    Count = ConstExpression ();
    if (Count < 0 || Count > 100) {
     	Error ("Range error");
     	Count = 1;
    }
    ConsumeComma ();

    /* Read the token list */
    List = CollectTokens ((unsigned) Start, (unsigned) Count);

    /* Since we want to insert the list before the now current token, we have
     * to save the current token in some way and then skip it. To do this, we
     * will add the current token at the end of the token list (so the list
     * will never be empty), push the token list, and then skip the current
     * token. This will replace the current token by the first token from the
     * list (which will be the old current token in case the list was empty).
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
    long       	Count;
    TokList* 	List;

    /* Skip it */
    NextTok ();

    /* Left paren expected */
    ConsumeLParen ();

    /* Count argument */
    Count = ConstExpression ();
    if (Count < 0 || Count > 100) {
     	Error ("Range error");
     	Count = 1;
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
     * to save the current token in some way and then skip it. To do this, we
     * will add the current token at the end of the token list (so the list
     * will never be empty), push the token list, and then skip the current
     * token. This will replace the current token by the first token from the
     * list (which will be the old current token in case the list was empty).
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
    char        Format[sizeof (SVal)];              /* User given format */
    const char* F = Format;                         /* User format pointer */
    StrBuf      R = AUTO_STRBUF_INITIALIZER;        /* Result string */
    StrBuf      F1 = AUTO_STRBUF_INITIALIZER;       /* One format spec from F */
    StrBuf      R1 = AUTO_STRBUF_INITIALIZER;       /* One result */
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
    strcpy (Format, SVal);
    NextTok ();

    /* Walk over the format string, generating the function result in R */
    while (1) {

        /* Get the next char from the format string and check for EOS */
        if (*F == '\0') {
            break;
        }

        /* Check for a format specifier */
        if (*F != '%') {
            /* No format specifier, just copy */
            SB_AppendChar (&R, *F++);
            continue;
        }
        if (*++F == '%') {
            /* %% */
            SB_AppendChar (&R, '%');
            ++F;
            continue;
        }
        if (*F == '\0') {
            InvalidFormatString ();
            break;
        }

        /* Since a format specifier follows, we do expect anotehr argument for
         * the .sprintf function.
         */
        ConsumeComma ();

        /* We will copy the format spec into F1 checking for the things we
         * support, and later use xsprintf to do the actual formatting. This
         * is easier than adding another printf implementation...
         */
        SB_Clear (&F1);
        SB_AppendChar (&F1, '%');

        /* Check for flags */
        Done = 0;
        while (*F != '\0' && !Done) {
            switch (*F) {
                case '-': /* FALLTHROUGH */
                case '+': /* FALLTHROUGH */
                case ' ': /* FALLTHROUGH */
                case '#': /* FALLTHROUGH */
                case '0': SB_AppendChar (&F1, *F++);    break;
                default:  Done = 1;                     break;
            }
        }

        /* We do only support a numerical width field */
        while (IsDigit (*F)) {
            SB_AppendChar (&F1, *F++);
        }

        /* Precision - only positive numerical fields supported */
        if (*F == '.') {
            SB_AppendChar (&F1, *F++);
            while (IsDigit (*F)) {
                SB_AppendChar (&F1, *F++);
            }
        }

        /* Length modifiers aren't supported, so read the conversion specs */
        switch (*F) {

            case 'd':
            case 'i':
            case 'o':
            case 'u':
            case 'X':
            case 'x':
                /* Our ints are actually longs, so we use the 'l' modifier when
                 * calling xsprintf later. Terminate the format string.
                 */
                SB_AppendChar (&F1, 'l');
                SB_AppendChar (&F1, *F++);
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
                SB_AppendChar (&F1, *F++);
                SB_Terminate (&F1);

                /* The argument must be a string constant */
                if (!LookAtStrCon ()) {
                    /* Make it one */
                    strcpy (SVal, "**undefined**");
                }

                /* Format this argument according to the spec */
                SB_Printf (&R1, SB_GetConstBuf (&F1), SVal);

                /* Skip the string constant */
                NextTok ();

                /* Append the formatted argument to the result */
                SB_Append (&R, &R1);

                break;

            case 'c':
                /* Add the format spec and terminate the format */
                SB_AppendChar (&F1, *F++);
                SB_Terminate (&F1);

                /* The argument must be a constant expression */
                IVal = ConstExpression ();

                /* Check for a valid character range */
                if (IVal <= 0 || IVal > 255) {
                    Error ("Char argument out of range");
                    IVal = ' ';
                }

                /* Format this argument according to the spec. Be sure to pass
                 * an int as the char value.
                 */
                SB_Printf (&R1, SB_GetConstBuf (&F1), (int) IVal);

                /* Append the formatted argument to the result */
                SB_Append (&R, &R1);

                break;

            default:
                Error ("Invalid format string");
                if (*F) {
                    /* Don't skip beyond end of string */
                    ++F;
                }
                break;
        }

    }

    /* The length of the final result may not exceed the size of a string */
    if (SB_GetLen (&R) >= sizeof (SVal)) {
        Error ("Resulting string is too long");
        SB_Cut (&R, sizeof (SVal) - 1);
    }

    /* Terminate the result string */
    SB_Terminate (&R);

    /* We expect a closing parenthesis, but will not skip it but replace it
     * by the string token just created.
     */
    if (Tok != TOK_RPAREN) {
     	Error ("`)' expected");
    } else {
     	Tok = TOK_STRCON;
     	memcpy (SVal, SB_GetConstBuf (&R), SB_GetLen (&R) + 1);
    }


    /* Delete the string buffers */
    DoneStrBuf (&R);
    DoneStrBuf (&F1);
    DoneStrBuf (&R1);
}



static void FuncString (void)
/* Handle the .STRING function */
{
    char Buf[MAX_STR_LEN+1];

    /* Skip it */
    NextTok ();

    /* Left paren expected */
    ConsumeLParen ();

    /* Accept identifiers or numeric expressions */
    if (Tok == TOK_IDENT || Tok == TOK_LOCAL_IDENT) {
     	/* Save the identifier, then skip it */
     	strcpy (Buf, SVal);
     	NextTok ();
    } else {
     	/* Numeric expression */
     	long Val = ConstExpression ();
     	sprintf (Buf, "%ld", Val);
    }

    /* We expect a closing parenthesis, but will not skip it but replace it
     * by the string token just created.
     */
    if (Tok != TOK_RPAREN) {
     	Error ("`)' expected");
    } else {
     	Tok = TOK_STRCON;
     	strcpy (SVal, Buf);
    }
}



void NextTok (void)
/* Get next token and handle token level functions */
{
    /* Get the next raw token */
    NextRawTok ();

    /* In raw mode, pass the token unchanged */
    if (RawMode == 0) {

	/* Execute token handling functions */
	switch (Tok) {

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



void Consume (enum Token Expected, const char* ErrMsg)
/* Consume Expected, print an error if we don't find it */
{
    if (Tok == Expected) {
	NextTok ();
    } else {
	Error (ErrMsg);
    }
}



void ConsumeSep (void)
/* Consume a separator token */
{
    /* We expect a separator token */
    ExpectSep ();

    /* If we are at end of line, skip it */
    if (Tok == TOK_SEP) {
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
    while (!TokIsSep (Tok)) {
     	NextTok ();
    }
}



void ExpectSep (void)
/* Check if we've reached a line separator, and output an error if not. Do
 * not skip the line separator.
 */
{
    if (!TokIsSep (Tok)) {
       	ErrorSkip ("Unexpected trailing garbage characters");
    }
}



void EnterRawTokenMode (void)
/* Enter raw token mode. In raw mode, token handling functions are not
 * executed, but the function tokens are passed untouched to the upper
 * layer. Raw token mode is used when storing macro tokens for later
 * use.
 * Calls to EnterRawTokenMode and LeaveRawTokenMode may be nested.
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



