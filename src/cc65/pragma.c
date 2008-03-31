/*****************************************************************************/
/*                                                                           */
/*				   pragma.c				     */
/*                                                                           */
/*		    Pragma handling for the cc65 C compiler		     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2008 Ullrich von Bassewitz                                       */
/*               Roemerstrasse 52                                            */
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



#include <stdlib.h>
#include <string.h>

/* common */
#include "segnames.h"
#include "tgttrans.h"

/* cc65 */
#include "codegen.h"
#include "error.h"
#include "expr.h"
#include "global.h"
#include "litpool.h"
#include "scanner.h"
#include "scanstrbuf.h"
#include "symtab.h"
#include "pragma.h"



/*****************************************************************************/
/*		      		     data				     */
/*****************************************************************************/



/* Tokens for the #pragmas */
typedef enum {
    PR_ILLEGAL = -1,
    PR_BSSSEG,
    PR_CHARMAP,
    PR_CHECKSTACK,
    PR_CODESEG,
    PR_CODESIZE,
    PR_DATASEG,
    PR_OPTIMIZE,
    PR_REGVARADDR,
    PR_REGVARS,
    PR_RODATASEG,
    PR_SIGNEDCHARS,
    PR_STATICLOCALS,
    PR_WARN,
    PR_ZPSYM,
    PR_COUNT
} pragma_t;

/* Pragma table */
static const struct Pragma {
    const char*	Key;		/* Keyword */
    pragma_t   	Tok;		/* Token */
} Pragmas[PR_COUNT] = {
    { 	"bssseg",       PR_BSSSEG	},
    {   "charmap",      PR_CHARMAP      },
    {	"checkstack",	PR_CHECKSTACK	},
    {   "codeseg",    	PR_CODESEG	},
    {   "codesize",     PR_CODESIZE     },
    {   "dataseg",    	PR_DATASEG	},
    {   "optimize",     PR_OPTIMIZE     },
    {   "regvaraddr", 	PR_REGVARADDR	},
    {   "regvars",      PR_REGVARS      },
    {   "rodataseg",  	PR_RODATASEG	},
    {	"signedchars",	PR_SIGNEDCHARS	},
    {	"staticlocals",	PR_STATICLOCALS	},
    {   "warn",         PR_WARN         },
    {   "zpsym",       	PR_ZPSYM  	},
};



/*****************************************************************************/
/*    	      	     	   	     Code  				     */
/*****************************************************************************/



static void PragmaErrorSkip (void)
/* Called in case of an error, skips tokens until the closing paren or a
 * semicolon is reached.
 */
{
    static const token_t TokenList[] = { TOK_RPAREN, TOK_SEMI };
    SkipTokens (TokenList, sizeof(TokenList) / sizeof(TokenList[0]));
}



static int CmpKey (const void* Key, const void* Elem)
/* Compare function for bsearch */
{
    return strcmp ((const char*) Key, ((const struct Pragma*) Elem)->Key);
}



static pragma_t FindPragma (const char* Key)
/* Find a pragma and return the token. Return PR_ILLEGAL if the keyword is
 * not a valid pragma.
 */
{
    struct Pragma* P;
    P = bsearch (Key, Pragmas, PR_COUNT, sizeof (Pragmas[0]), CmpKey);
    return P? P->Tok : PR_ILLEGAL;
}



static void StringPragma (StrBuf* B, void (*Func) (const char*))
/* Handle a pragma that expects a string parameter */
{
    StrBuf S;

    /* We expect a string here */
    if (SB_GetString (B, &S)) {
       	/* Call the given function with the string argument */
	Func (SB_GetConstBuf (&S));
    } else {
	Error ("String literal expected");
    }

    /* Call the string buf destructor */
    SB_Done (&S);
}



static void SegNamePragma (StrBuf* B, segment_t Seg)
/* Handle a pragma that expects a segment name parameter */
{
    ident       Ident;
    StrBuf      S;
    const char* Name;

    /* Try to read an identifier */
    int Push = 0;
    if (SB_GetSym (B, Ident)) {

        /* Check if we have a first argument named "pop" */
        if (strcmp (Ident, "pop") == 0) {

            /* Pop the old value */
            PopSegName (Seg);

            /* Set the segment name */
            g_segname (Seg);

            /* Done */
            return;

        /* Check if we have a first argument named "push" */
        } else if (strcmp (Ident, "push") == 0) {

            Push = 1;
            SB_SkipWhite (B);
            if (SB_Get (B) != ',') {
                Error ("Comma expected");
                return;
            }
            SB_SkipWhite (B);

        } else {
            Error ("Invalid pragma arguments");
            return;
        }
    }

    /* A string argument must follow */
    if (!SB_GetString (B, &S)) {
	Error ("String literal expected");
        return;
    }

    /* Get the string */
    Name = SB_GetConstBuf (&S);

    /* Check if the name is valid */
    if (!ValidSegName (Name)) {
        /* Segment name is invalid */
        Error ("Illegal segment name: `%s'", Name);
        return;
    }

    /* Set the new name */
    if (Push) {
        PushSegName (Seg, Name);
    } else {
        SetSegName (Seg, Name);
    }
    g_segname (Seg);

    /* Call the string buf destructor */
    SB_Done (&S);
}



static void CharMapPragma (StrBuf* B)
/* Change the character map */
{
    long Index, C;

    /* Read the character index */
    if (!SB_GetNumber (B, &Index)) {
        return;
    }
    if (Index < 1 || Index > 255) {
     	Error ("Character index out of range");
     	return;
    }

    /* Comma follows */
    SB_SkipWhite (B);
    if (SB_Get (B) != ',') {
        Error ("Comma expected");
        return;
    }
    SB_SkipWhite (B);

    /* Read the character code */
    if (!SB_GetNumber (B, &C)) {
        return;
    }
    if (C < 1 || C > 255) {
    	Error ("Character code out of range");
    	return;
    }

    /* Remap the character */
    TgtTranslateSet ((unsigned) Index, (unsigned char) C);
}



static void FlagPragma (StrBuf* B, IntStack* Stack)
/* Handle a pragma that expects a boolean paramater */
{
    ident Ident;
    long  Val;
    int   Push;

    /* Try to read an identifier */
    int IsIdent = SB_GetSym (B, Ident);

    /* Check if we have a first argument named "pop" */
    if (IsIdent && strcmp (Ident, "pop") == 0) {
        if (IS_GetCount (Stack) < 2) {
            Error ("Cannot pop, stack is empty");
        } else {
            IS_Drop (Stack);
        }
        /* No other arguments allowed */
        return;
    }

    /* Check if we have a first argument named "push" */
    if (IsIdent && strcmp (Ident, "push") == 0) {
        Push = 1;
        SB_SkipWhite (B);
        if (SB_Get (B) != ',') {
            Error ("Comma expected");
            return;
        }
        SB_SkipWhite (B);
        IsIdent = SB_GetSym (B, Ident);
    } else {
        Push = 0;
    }

    /* Boolean argument follows */
    if (IsIdent) {
        if (strcmp (Ident, "true") == 0 || strcmp (Ident, "on") == 0) {
            Val = 1;
        } else if (strcmp (Ident, "false") == 0 || strcmp (Ident, "off") == 0) {
            Val = 0;
        } else {
            Error ("Pragma argument must be one of `on', `off', `true' or `false'");
        }
    } else if (!SB_GetNumber (B, &Val)) {
        Error ("Invalid pragma argument");
        return;
    }

    /* Set/push the new value */
    if (Push) {
        if (IS_IsFull (Stack)) {
            Error ("Cannot push: stack overflow");
        } else {
            IS_Push (Stack, Val);
        }
    } else {
        IS_Set (Stack, Val);
    }
}



static void IntPragma (StrBuf* B, IntStack* Stack, long Low, long High)
/* Handle a pragma that expects an int paramater */
{
    ident Ident;
    long  Val;
    int   Push;

    /* Try to read an identifier */
    int IsIdent = SB_GetSym (B, Ident);

    /* Check if we have a first argument named "pop" */
    if (IsIdent && strcmp (Ident, "pop") == 0) {
        if (IS_GetCount (Stack) < 2) {
            Error ("Cannot pop, stack is empty");
        } else {
            IS_Drop (Stack);
        }
        /* No other arguments allowed */
        return;
    }

    /* Check if we have a first argument named "push" */
    if (IsIdent && strcmp (Ident, "push") == 0) {
        Push = 1;
        SB_SkipWhite (B);
        if (SB_Get (B) != ',') {
            Error ("Comma expected");
            return;
        }
        SB_SkipWhite (B);
        IsIdent = 0;
    } else {
        Push = 0;
    }

    /* Integer argument follows */
    if (IsIdent || !SB_GetNumber (B, &Val)) {
        Error ("Pragma argument must be numeric");
        return;
    }

    /* Check the argument */
    if (Val < Low || Val > High) {
        Error ("Pragma argument out of bounds (%ld-%ld)", Low, High);
        return;
    }

    /* Set/push the new value */
    if (Push) {
        if (IS_IsFull (Stack)) {
            Error ("Cannot push: stack overflow");
        } else {
            IS_Push (Stack, Val);
        }
    } else {
        IS_Set (Stack, Val);
    }
}



static void ParsePragma (void)
/* Parse the contents of the _Pragma statement */
{
    pragma_t Pragma;
    ident    Ident;

    /* Create a string buffer from the string literal */
    StrBuf B = AUTO_STRBUF_INITIALIZER;
    GetLiteralStrBuf (&B, CurTok.IVal);

    /* Reset the string pointer, effectivly clearing the string from the
     * string table. Since we're working with one token lookahead, this
     * will fail if the next token is also a string token, but that's a
     * syntax error anyway, because we expect a right paren.
     */
    ResetLiteralPoolOffs (CurTok.IVal);

    /* Skip the string token */
    NextToken ();

    /* Get the pragma name from the string */
    SB_SkipWhite (&B);
    if (!SB_GetSym (&B, Ident)) {
        Error ("Invalid pragma");
        return;
    }

    /* Search for the name */
    Pragma = FindPragma (Ident);

    /* Do we know this pragma? */
    if (Pragma == PR_ILLEGAL) {
       	/* According to the ANSI standard, we're not allowed to generate errors
       	 * for unknown pragmas, however, we're allowed to warn - and we will
       	 * do so. Otherwise one typo may give you hours of bug hunting...
       	 */
       	Warning ("Unknown pragma `%s'", Ident);
       	return;
    }

    /* Check for an open paren */
    SB_SkipWhite (&B);
    if (SB_Get (&B) != '(') {
        Error ("'(' expected");
        return;
    }

    /* Skip white space before the argument */
    SB_SkipWhite (&B);

    /* Switch for the different pragmas */
    switch (Pragma) {

     	case PR_BSSSEG:
     	    SegNamePragma (&B, SEG_BSS);
     	    break;

     	case PR_CHARMAP:
     	    CharMapPragma (&B);
     	    break;

     	case PR_CHECKSTACK:
     	    FlagPragma (&B, &CheckStack);
     	    break;

     	case PR_CODESEG:
     	    SegNamePragma (&B, SEG_CODE);
     	    break;

     	case PR_CODESIZE:
     	    IntPragma (&B, &CodeSizeFactor, 10, 1000);
     	    break;

     	case PR_DATASEG:
     	    SegNamePragma (&B, SEG_DATA);
     	    break;

        case PR_OPTIMIZE:
            FlagPragma (&B, &Optimize);
            break;

     	case PR_REGVARADDR:
       	    FlagPragma (&B, &AllowRegVarAddr);
     	    break;

     	case PR_REGVARS:
       	    FlagPragma (&B, &EnableRegVars);
     	    break;

     	case PR_RODATASEG:
     	    SegNamePragma (&B, SEG_RODATA);
     	    break;

     	case PR_SIGNEDCHARS:
       	    FlagPragma (&B, &SignedChars);
     	    break;

     	case PR_STATICLOCALS:
     	    FlagPragma (&B, &StaticLocals);
     	    break;

        case PR_WARN:
            FlagPragma (&B, &WarnDisable);
            break;

     	case PR_ZPSYM:
     	    StringPragma (&B, MakeZPSym);
     	    break;

     	default:
       	    Internal ("Invalid pragma");
    }

    /* Closing paren expected */
    SB_SkipWhite (&B);
    if (SB_Get (&B) != ')') {
        Error ("')' expected");
        return;
    }
    SB_SkipWhite (&B);

    /* Allow an optional semicolon to be compatible with the old syntax */
    if (SB_Peek (&B) == ';') {
        SB_Skip (&B);
        SB_SkipWhite (&B);
    }

    /* Make sure nothing follows */
    if (SB_Peek (&B) != '\0') {
        Error ("Unexpected input following pragma directive");
    }

    /* Release the StrBuf */
    SB_Done (&B);
}



void DoPragma (void)
/* Handle pragmas. These come always in form of the new C99 _Pragma() operator. */
{
    /* Skip the token itself */
    NextToken ();

    /* We expect an opening paren */
    if (!ConsumeLParen ()) {
	return;
    }

    /* String literal */
    if (CurTok.Tok != TOK_SCONST) {

    	/* Print a diagnostic */
     	Error ("String literal expected");

    	/* Try some smart error recovery: Skip tokens until we reach the
    	 * enclosing paren, or a semicolon.
    	 */
       	PragmaErrorSkip ();

    } else {

    	/* Parse the _Pragma statement */
    	ParsePragma ();
    }

    /* Closing paren needed */
    ConsumeRParen ();
}



