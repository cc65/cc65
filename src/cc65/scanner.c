/*****************************************************************************/
/*                                                                           */
/*				   scanner.c                                 */
/*                                                                           */
/*			Source file line info structure                      */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2001 Ullrich von Bassewitz                                       */
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



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

/* common */
#include "chartype.h"
#include "tgttrans.h"

/* cc65 */
#include "datatype.h"
#include "error.h"
#include "function.h"
#include "global.h"
#include "ident.h"
#include "input.h"
#include "litpool.h"
#include "preproc.h"
#include "symtab.h"
#include "util.h"
#include "scanner.h"



/*****************************************************************************/
/*		  		     data				     */
/*****************************************************************************/



Token CurTok;		/* The current token */
Token NextTok;		/* The next token */



/* Token types */
#define TT_C 	0		/* ANSI C token */
#define TT_EXT	1 		/* cc65 extension */

/* Token table */
static const struct Keyword {
    char*    	    Key;    	/* Keyword name */
    unsigned char   Tok;    	/* The token */
    unsigned char   Type;      	/* Token type */
} Keywords [] = {
    { "__A__",	       	TOK_A,	       	TT_C   	},
    { "__AX__",	       	TOK_AX,		TT_C	},
    { "__EAX__",       	TOK_EAX,   	TT_C	},
    { "__X__", 	       	TOK_X,		TT_C	},
    { "__Y__", 	       	TOK_Y,		TT_C	},
    { "__asm__",       	TOK_ASM,   	TT_C	},
    { "__attribute__",	TOK_ATTRIBUTE,	TT_C	},
    { "__far__",	TOK_FAR,	TT_C	},
    { "__fastcall__",  	TOK_FASTCALL,   TT_C	},
    { "asm",   	       	TOK_ASM,   	TT_EXT 	},
    { "auto",  	       	TOK_AUTO,  	TT_C  	},
    { "break", 	       	TOK_BREAK, 	TT_C  	},
    { "case",  	       	TOK_CASE,  	TT_C  	},
    { "char",  	       	TOK_CHAR,  	TT_C  	},
    { "const", 	       	TOK_CONST, 	TT_C  	},
    { "continue",      	TOK_CONTINUE,   TT_C  	},
    { "default",       	TOK_DEFAULT,    TT_C  	},
    { "do",    	       	TOK_DO,    	TT_C  	},
    { "double",        	TOK_DOUBLE,	TT_C   	},
    { "else",  	       	TOK_ELSE,  	TT_C  	},
    { "enum",  	       	TOK_ENUM,  	TT_C  	},
    { "extern",        	TOK_EXTERN,	TT_C   	},
    { "far",		TOK_FAR,	TT_EXT	},
    { "fastcall",      	TOK_FASTCALL,	TT_EXT 	},
    { "float", 	       	TOK_FLOAT, 	TT_C  	},
    { "for",   	       	TOK_FOR,   	TT_C  	},
    { "goto",  	       	TOK_GOTO,  	TT_C  	},
    { "if",    	       	TOK_IF,    	TT_C  	},
    { "int",   	       	TOK_INT,   	TT_C  	},
    { "long",  	       	TOK_LONG,  	TT_C  	},
    { "register",      	TOK_REGISTER,   TT_C  	},
    { "return",        	TOK_RETURN,	TT_C  	},
    { "short", 	       	TOK_SHORT, 	TT_C  	},
    { "signed",	       	TOK_SIGNED,	TT_C  	},
    { "sizeof",        	TOK_SIZEOF,	TT_C  	},
    { "static",        	TOK_STATIC,	TT_C  	},
    { "struct",        	TOK_STRUCT,	TT_C  	},
    { "switch",        	TOK_SWITCH,	TT_C	},
    { "typedef",       	TOK_TYPEDEF,    TT_C	},
    { "union", 	       	TOK_UNION, 	TT_C	},
    { "unsigned",      	TOK_UNSIGNED,   TT_C	},
    { "void",  	       	TOK_VOID,  	TT_C	},
    { "volatile",      	TOK_VOLATILE,   TT_C	},
    { "while", 	       	TOK_WHILE, 	TT_C	},
};
#define KEY_COUNT	(sizeof (Keywords) / sizeof (Keywords [0]))



/* Stuff for determining the type of an integer constant */
#define IT_INT	 	0x01
#define IT_UINT	 	0x02
#define IT_LONG	 	0x04
#define IT_ULONG 	0x08



/*****************************************************************************/
/*		 		     code 				     */
/*****************************************************************************/



static int CmpKey (const void* Key, const void* Elem)
/* Compare function for bsearch */
{
    return strcmp ((const char*) Key, ((const struct Keyword*) Elem)->Key);
}



static int FindKey (const char* Key)
/* Find a keyword and return the token. Return IDENT if the token is not a
 * keyword.
 */
{
    struct Keyword* K;
    K = bsearch (Key, Keywords, KEY_COUNT, sizeof (Keywords [0]), CmpKey);
    if (K && (K->Type != TT_EXT || ANSI == 0)) {
	return K->Tok;
    } else {
	return TOK_IDENT;
    }
}



static int SkipWhite (void)
/* Skip white space in the input stream, reading and preprocessing new lines
 * if necessary. Return 0 if end of file is reached, return 1 otherwise.
 */
{
    while (1) {
       	while (CurC == 0) {
	    if (NextLine () == 0) {
	     	return 0;
     	    }
	    Preprocess ();
     	}
	if (IsSpace (CurC)) {
    	    NextChar ();
	} else {
    	    return 1;
	}
    }
}



void SymName (char* s)
/* Get symbol from input stream */
{
    unsigned k = 0;
    do {
       	if (k != MAX_IDENTLEN) {
       	    ++k;
       	    *s++ = CurC;
     	}
       	NextChar ();
    } while (IsIdent (CurC) || IsDigit (CurC));
    *s = '\0';
}



int IsSym (char *s)
/* Get symbol from input stream or return 0 if not a symbol. */
{
    if (IsIdent (CurC)) {
     	SymName (s);
     	return 1;
    } else {
     	return 0;
    }
}



static void UnknownChar (char C)
/* Error message for unknown character */
{
    Error ("Invalid input character with code %02X", C & 0xFF);
    NextChar (); 			/* Skip */
}



static unsigned hexval (int c)
/* Convert a hex digit into a value */
{
    if (!IsXDigit (c)) {
	Error ("Invalid hexadecimal digit: `%c'", c);
    }
    if (IsDigit (c)) {
	return c - '0';
    } else {
       	return toupper (c) - 'A' + 10;
    }
}



static void SetTok (int tok)
/* Set NextTok.Tok and bump line ptr */
{
    NextTok.Tok = tok;
    NextChar ();
}



static int SignExtendChar (int C)
/* Do correct sign extension of a character */
{
    if (SignedChars && (C & 0x80) != 0) {
       	return C | ~0xFF;
    } else {
       	return C & 0xFF;
    }
}



static int ParseChar (void)
/* Parse a character. Converts \n into EOL, etc. */
{
    int i;
    unsigned val;
    int C;

    /* Check for escape chars */
    if (CurC == '\\') {
	NextChar ();
	switch (CurC) {
	    case 'b':
	       	C = '\b';
	   	break;
     	    case 'f':
	   	C = '\f';
		break;
	    case 'r':
		C = '\r';
		break;
	    case 'n':
		C = '\n';
		break;
	    case 't':
		C = '\t';
		break;
	    case '\"':
		C = '\"';
		break;
	    case '\'':
		C = '\'';
		break;
	    case '\\':
		C = '\\';
		break;
	    case 'x':
	    case 'X':
		/* Hex character constant */
		NextChar ();
		val = hexval (CurC) << 4;
		NextChar ();
       	       	C = val | hexval (CurC); 	/* Do not translate */
		break;
	    case '0':
	    case '1':
		/* Octal constant */
		i = 0;
     		C = CurC - '0';
       	       	while (NextC >= '0' && NextC <= '7' && i++ < 4) {
     	 	    NextChar ();
     	       	    C = (C << 3) | (CurC - '0');
     		}
     		break;
     	    default:
     		Error ("Illegal character constant");
		C = ' ';
		break;
     	}
    } else {
     	C = CurC;
    }

    /* Skip the character read */
    NextChar ();

    /* Do correct sign extension */
    return SignExtendChar (C);
}



static void CharConst (void)
/* Parse a character constant. */
{
    int C;

    /* Skip the quote */
    NextChar ();

    /* Get character */
    C = ParseChar ();

    /* Check for closing quote */
    if (CurC != '\'') {
       	Error ("`\'' expected");
    } else {
	/* Skip the quote */
	NextChar ();
    }

    /* Setup values and attributes */
    NextTok.Tok  = TOK_CCONST;

    /* Translate into target charset */
    NextTok.IVal = SignExtendChar (TgtTranslateChar (C));

    /* Character constants have type int */
    NextTok.Type = type_int;
}



static void StringConst (void)
/* Parse a quoted string */
{
    NextTok.IVal = GetLiteralPoolOffs ();
    NextTok.Tok  = TOK_SCONST;

    /* Be sure to concatenate strings */
    while (CurC == '\"') {

	/* Skip the quote char */
	NextChar ();

	while (CurC != '\"') {
	    if (CurC == '\0') {
	     	Error ("Unexpected newline");
	     	break;
	    }
	    AddLiteralChar (ParseChar ());
	}

	/* Skip closing quote char if there was one */
	NextChar ();

	/* Skip white space, read new input */
	SkipWhite ();

    }

    /* Terminate the string */
    AddLiteralChar ('\0');
}



void NextToken (void)
/* Get next token from input stream */
{
    ident token;

    /* We have to skip white space here before shifting tokens, since the
     * tokens and the current line info is invalid at startup and will get
     * initialized by reading the first time from the file. Remember if
     * we were at end of input and handle that later.
     */
    int GotEOF = (SkipWhite() == 0);

    /* Current token is the lookahead token */
    if (CurTok.LI) {
	ReleaseLineInfo (CurTok.LI);
    }
    CurTok = NextTok;

    /* Remember the starting position of the next token */
    NextTok.LI = UseLineInfo (GetCurLineInfo ());

    /* Now handle end of input. */
    if (GotEOF) {
	/* End of file reached */
	NextTok.Tok = TOK_CEOF;
	return;
    }

    /* Determine the next token from the lookahead */
    if (IsDigit (CurC)) {

     	/* A number */
   	int HaveSuffix;		/* True if we have a type suffix */
     	unsigned types;		/* Possible types */
     	unsigned base;
     	unsigned long k;	/* Value */

     	k     = 0;
     	base  = 10;
     	types = IT_INT | IT_LONG | IT_ULONG;

       	if (CurC == '0') {
     	    /* Octal or hex constants may also be of type unsigned int */
     	    types = IT_INT | IT_UINT | IT_LONG | IT_ULONG;
     	    /* gobble 0 and examin next char */
	    NextChar ();
     	    if (toupper (CurC) == 'X') {
     	     	base = 16;
     	    	NextTok.Type = type_uint;
       	       	NextChar ();	/* gobble "x" */
     	    } else {
     	     	base = 8;
     	    }
     	}
     	while (1) {
     	    if (IsDigit (CurC)) {
     	     	k = k * base + (CurC - '0');
     	    } else if (base == 16 && IsXDigit (CurC)) {
     	     	k = (k << 4) + hexval (CurC);
     	    } else {
     	     	break; 	      	/* not digit */
     	    }
       	    NextChar ();	/* gobble char */
     	}

     	/* Check for a suffix */
	HaveSuffix = 1;
     	if (CurC == 'u' || CurC == 'U') {
     	    /* Unsigned type */
	    NextChar ();
     	    if (toupper (CurC) != 'L') {
     	    	types = IT_UINT | IT_ULONG;
     	    } else {
     	    	NextChar ();
     	    	types = IT_ULONG;
     	    }
     	} else if (CurC == 'l' || CurC == 'L') {
     	    /* Long type */
       	    NextChar ();
     	    if (toupper (CurC) != 'U') {
     	    	types = IT_LONG | IT_ULONG;
     	    } else {
     	    	NextChar ();
     	    	types = IT_ULONG;
     	    }
     	} else {
	    HaveSuffix = 0;
	}

     	/* Check the range to determine the type */
       	if (k > 0x7FFF) {
     	    /* Out of range for int */
     	    types &= ~IT_INT;
	    /* If the value is in the range 0x8000..0xFFFF, unsigned int is not
	     * allowed, and we don't have a type specifying suffix, emit a
	     * warning.
	     */
       	    if (k <= 0xFFFF && (types & IT_UINT) == 0 && !HaveSuffix) {
		Warning ("Constant is long");
	    }
     	}
     	if (k > 0xFFFF) {
     	    /* Out of range for unsigned int */
     	    types &= ~IT_UINT;
     	}
     	if (k > 0x7FFFFFFF) {
     	    /* Out of range for long int */
     	    types &= ~IT_LONG;
     	}

     	/* Now set the type string to the smallest type in types */
     	if (types & IT_INT) {
     	    NextTok.Type = type_int;
     	} else if (types & IT_UINT) {
     	    NextTok.Type = type_uint;
     	} else if (types & IT_LONG) {
     	    NextTok.Type = type_long;
     	} else {
     	    NextTok.Type = type_ulong;
     	}

     	/* Set the value and the token */
     	NextTok.IVal = k;
     	NextTok.Tok  = TOK_ICONST;
     	return;
    }

    if (IsSym (token)) {

     	/* Check for a keyword */
     	if ((NextTok.Tok = FindKey (token)) != TOK_IDENT) {
     	    /* Reserved word found */
     	    return;
     	}
     	/* No reserved word, check for special symbols */
     	if (token [0] == '_') {
     	    /* Special symbols */
     	    if (strcmp (token, "__FILE__") == 0) {
	       	NextTok.IVal = AddLiteral (GetCurrentFile());
	       	NextTok.Tok  = TOK_SCONST;
	       	return;
	    } else if (strcmp (token, "__LINE__") == 0) {
	       	NextTok.Tok  = TOK_ICONST;
    	       	NextTok.IVal = GetCurrentLine();
    	       	NextTok.Type = type_int;
    	       	return;
    	    } else if (strcmp (token, "__func__") == 0) {
	       	/* __func__ is only defined in functions */
	       	if (CurrentFunc) {
	       	    NextTok.IVal = AddLiteral (F_GetFuncName (CurrentFunc));
	       	    NextTok.Tok  = TOK_SCONST;
	       	    return;
	       	}
	    }
    	}

       	/* No reserved word but identifier */
	strcpy (NextTok.Ident, token);
     	NextTok.Tok = TOK_IDENT;
    	return;
    }

    /* Monstrous switch statement ahead... */
    switch (CurC) {

    	case '!':
	    NextChar ();
    	    if (CurC == '=') {
    		SetTok (TOK_NE);
    	    } else {
    		NextTok.Tok = TOK_BOOL_NOT;
    	    }
    	    break;

    	case '\"':
       	    StringConst ();
    	    break;

    	case '%':
	    NextChar ();
    	    if (CurC == '=') {
    		SetTok (TOK_MOD_ASSIGN);
    	    } else {
    		NextTok.Tok = TOK_MOD;
    	    }
    	    break;

    	case '&':
	    NextChar ();
    	    switch (CurC) {
    		case '&':
    		    SetTok (TOK_BOOL_AND);
    		    break;
    		case '=':
    		    SetTok (TOK_AND_ASSIGN);
    	      	    break;
    		default:
    		    NextTok.Tok = TOK_AND;
    	    }
    	    break;

    	case '\'':
    	    CharConst ();
    	    break;

    	case '(':
    	    SetTok (TOK_LPAREN);
    	    break;

    	case ')':
    	    SetTok (TOK_RPAREN);
    	    break;

    	case '*':
	    NextChar ();
    	    if (CurC == '=') {
    		SetTok (TOK_MUL_ASSIGN);
    	    } else {
    		NextTok.Tok = TOK_STAR;
    	    }
    	    break;

    	case '+':
	    NextChar ();
    	    switch (CurC) {
    	    	case '+':
    		    SetTok (TOK_INC);
    		    break;
    	     	case '=':
    		    SetTok (TOK_PLUS_ASSIGN);
    		    break;
    		default:
    		    NextTok.Tok = TOK_PLUS;
    	    }
    	    break;

    	case ',':
    	    SetTok (TOK_COMMA);
    	    break;

    	case '-':
	    NextChar ();
    	    switch (CurC) {
    	      	case '-':
    		    SetTok (TOK_DEC);
    		    break;
    		case '=':
    	    	    SetTok (TOK_MINUS_ASSIGN);
    		    break;
    		case '>':
    	    	    SetTok (TOK_PTR_REF);
    		    break;
    		default:
    		    NextTok.Tok = TOK_MINUS;
    	    }
    	    break;

    	case '.':
	    NextChar ();
       	    if (CurC == '.') {
		NextChar ();
    		if (CurC == '.') {
    		    SetTok (TOK_ELLIPSIS);
    		} else {
    		    UnknownChar (CurC);
    		}
    	    } else {
    		NextTok.Tok = TOK_DOT;
    	    }
    	    break;

    	case '/':
	    NextChar ();
    	    if (CurC == '=') {
    		SetTok (TOK_DIV_ASSIGN);
    	    } else {
    	     	NextTok.Tok = TOK_DIV;
    	    }
    	    break;

    	case ':':
    	    SetTok (TOK_COLON);
    	    break;

    	case ';':
    	    SetTok (TOK_SEMI);
    	    break;

    	case '<':
	    NextChar ();
    	    switch (CurC) {
    		case '=':
    	      	    SetTok (TOK_LE);
    	    	    break;
    		case '<':
		    NextChar ();
    		    if (CurC == '=') {
    		    	SetTok (TOK_SHL_ASSIGN);
    		    } else {
    		    	NextTok.Tok = TOK_SHL;
    	    	    }
    		    break;
    		default:
    		    NextTok.Tok = TOK_LT;
    	    }
    	    break;

    	case '=':
	    NextChar ();
       	    if (CurC == '=') {
    		SetTok (TOK_EQ);
    	    } else {
    		NextTok.Tok = TOK_ASSIGN;
    	    }
    	    break;

    	case '>':
	    NextChar ();
    	    switch (CurC) {
    		case '=':
    		    SetTok (TOK_GE);
    		    break;
    		case '>':
		    NextChar ();
    		    if (CurC == '=') {
    		    	SetTok (TOK_SHR_ASSIGN);
    		    } else {
    	     	    	NextTok.Tok = TOK_SHR;
    		    }
    		    break;
    		default:
    		    NextTok.Tok = TOK_GT;
    	    }
    	    break;

    	case '?':
    	    SetTok (TOK_QUEST);
    	    break;

    	case '[':
    	    SetTok (TOK_LBRACK);
    	    break;

    	case ']':
    	    SetTok (TOK_RBRACK);
    	    break;

    	case '^':
	    NextChar ();
    	    if (CurC == '=') {
    		SetTok (TOK_XOR_ASSIGN);
    	    } else {
    		NextTok.Tok = TOK_XOR;
    	    }
    	    break;

    	case '{':
    	    SetTok (TOK_LCURLY);
    	    break;

        case '|':
	    NextChar ();
    	    switch (CurC) {
    		case '|':
    		    SetTok (TOK_BOOL_OR);
    		    break;
    		case '=':
    		    SetTok (TOK_OR_ASSIGN);
    		    break;
    		default:
    		    NextTok.Tok = TOK_OR;
    	    }
    	    break;

    	case '}':
    	    SetTok (TOK_RCURLY);
    	    break;

    	case '~':
    	    SetTok (TOK_COMP);
    	    break;

        case '#':
	    /* Skip it and following whitespace */
	    do {
	    	NextChar ();
	    } while (CurC == ' ');
	    if (!IsSym (token) || strcmp (token, "pragma") != 0) {
	      	/* OOPS - should not happen */
	      	Error ("Preprocessor directive expected");
	    }
	    NextTok.Tok = TOK_PRAGMA;
	    break;

    	default:
       	    UnknownChar (CurC);

    }

}



void SkipTokens (const token_t* TokenList, unsigned TokenCount)
/* Skip tokens until we reach TOK_CEOF or a token in the given token list.
 * This routine is used for error recovery.
 */
{
    while (CurTok.Tok != TOK_CEOF) {

    	/* Check if the current token is in the token list */
	unsigned I;
    	for (I = 0; I < TokenCount; ++I) {
    	    if (CurTok.Tok == TokenList[I]) {
    	    	/* Found a token in the list */
    	    	return;
    	    }
    	}

    	/* Not in the list: Skip it */
    	NextToken ();

    }
}



void Consume (token_t Token, const char* ErrorMsg)
/* Eat token if it is the next in the input stream, otherwise print an error
 * message.
 */
{
    if (CurTok.Tok == Token) {
	NextToken ();
    } else {
       	Error (ErrorMsg);
    }
}



void ConsumeColon (void)
/* Check for a colon and skip it. */
{
    Consume (TOK_COLON, "`:' expected");
}



void ConsumeSemi (void)
/* Check for a semicolon and skip it. */
{
    /* Try do be smart about typos... */
    if (CurTok.Tok == TOK_SEMI) {
	NextToken ();
    } else {
	Error ("`;' expected");
	if (CurTok.Tok == TOK_COLON || CurTok.Tok == TOK_COMMA) {
	    NextToken ();
	}
    }
}



void ConsumeComma (void)
/* Check for a comma and skip it. */
{
    /* Try do be smart about typos... */
    if (CurTok.Tok == TOK_COMMA) {
	NextToken ();
    } else {
      	Error ("`,' expected");
	if (CurTok.Tok == TOK_SEMI) {
	    NextToken ();
	}
    }
}



void ConsumeLParen (void)
/* Check for a left parenthesis and skip it */
{
    Consume (TOK_LPAREN, "`(' expected");
}



void ConsumeRParen (void)
/* Check for a right parenthesis and skip it */
{
    Consume (TOK_RPAREN, "`)' expected");
}



void ConsumeLBrack (void)
/* Check for a left bracket and skip it */
{
    Consume (TOK_LBRACK, "`[' expected");
}



void ConsumeRBrack (void)
/* Check for a right bracket and skip it */
{
    Consume (TOK_RBRACK, "`]' expected");
}



void ConsumeLCurly (void)
/* Check for a left curly brace and skip it */
{
    Consume (TOK_LCURLY, "`{' expected");
}



void ConsumeRCurly (void)
/* Check for a right curly brace and skip it */
{
    Consume (TOK_RCURLY, "`}' expected");
}



