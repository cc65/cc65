/*****************************************************************************/
/*                                                                           */
/*				   pragma.c				     */
/*                                                                           */
/*		    Pragma handling for the cc65 C compiler		     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2002 Ullrich von Bassewitz                                       */
/*               Wacholderweg 14                                             */
/*               D-70597 Stuttgart                                           */
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
#include "tgttrans.h"

/* cc65 */
#include "codegen.h"
#include "error.h"
#include "expr.h"
#include "global.h"
#include "litpool.h"
#include "scanner.h"
#include "scanstrbuf.h"
#include "segments.h"
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
    PR_DATASEG,
    PR_REGVARADDR,
    PR_RODATASEG,
    PR_SIGNEDCHARS,
    PR_STATICLOCALS,
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
    {   "dataseg",    	PR_DATASEG	},
    {   "regvaraddr", 	PR_REGVARADDR	},
    {   "rodataseg",  	PR_RODATASEG	},
    {	"signedchars",	PR_SIGNEDCHARS	},
    {	"staticlocals",	PR_STATICLOCALS	},
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
    DoneStrBuf (&S);
}



static void SegNamePragma (StrBuf* B, segment_t Seg)
/* Handle a pragma that expects a segment name parameter */
{
    StrBuf S;

    if (SB_GetString (B, &S)) {

        /* Get the string */
        const char* Name = SB_GetConstBuf (&S);

	/* Check if the name is valid */
	if (ValidSegName (Name)) {

       	    /* Set the new name */
	    g_segname (Seg, Name);

	} else {

	    /* Segment name is invalid */
	    Error ("Illegal segment name: `%s'", Name);

	}

    } else {
	Error ("String literal expected");
    }

    /* Call the string buf destructor */
    DoneStrBuf (&S);
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



static void FlagPragma (StrBuf* B, unsigned char* Flag)
/* Handle a pragma that expects a boolean paramater */
{
    ident Ident;
    long  Val;

    if (SB_GetSym (B, Ident)) {
        if (strcmp (Ident, "true") == 0 || strcmp (Ident, "on") == 0) {
            *Flag = 1;
        } else if (strcmp (Ident, "false") == 0 || strcmp (Ident, "off") == 0) {
            *Flag = 0;
        } else {
            Error ("Pragma argument must be one of `on', `off', `true' or `false'");
        }
    } else if (SB_GetNumber (B, &Val)) {
	*Flag = (Val != 0);
    } else {
        Error ("Invalid pragma argument");
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

     	case PR_DATASEG:
     	    SegNamePragma (&B, SEG_DATA);
     	    break;

     	case PR_REGVARADDR:
     	    FlagPragma (&B, &AllowRegVarAddr);
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
    DoneStrBuf (&B);
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



