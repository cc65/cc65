/*
 * scanner.c
 *
 * Ullrich von Bassewitz, 07.06.1998
 */



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

#include "ctrans.h"
#include "datatype.h"
#include "error.h"
#include "function.h"
#include "global.h"
#include "ident.h"
#include "io.h"
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
static struct Keyword {
    char*    	    Key;    	/* Keyword name */
    unsigned char   Tok;    	/* The token */
    unsigned char   Type;      	/* Token type */
} Keywords [] = {
    { "__AX__",	       	AX,    	       	TT_C	},
    { "__EAX__",       	EAX,   	       	TT_C	},
    { "__asm__",       	ASM,   	       	TT_C	},
    { "__fastcall__",  	FASTCALL,      	TT_C	},
    { "asm",   	       	ASM,   		TT_EXT 	},
    { "auto",  	       	AUTO,  	       	TT_C  	},
    { "break", 	       	BREAK, 	       	TT_C  	},
    { "case",  	       	CASE,  	       	TT_C  	},
    { "char",  	       	CHAR,  	       	TT_C  	},
    { "const", 	       	CONST, 	       	TT_C  	},
    { "continue",      	CONTINUE,      	TT_C  	},
    { "default",       	DEFAULT,       	TT_C  	},
    { "do",    	       	DO,    	       	TT_C  	},
    { "double",        	DOUBLE,	       	TT_C   	},
    { "else",  	       	ELSE,  	       	TT_C  	},
    { "enum",  	       	ENUM,  	       	TT_C  	},
    { "extern",        	EXTERN,	       	TT_C   	},
    { "fastcall",      	FASTCALL,	TT_EXT 	},
    { "float", 	       	FLOAT, 	       	TT_C  	},
    { "for",   	       	FOR,   	       	TT_C  	},
    { "goto",  	       	GOTO,  	       	TT_C  	},
    { "if",    	       	IF,    	       	TT_C  	},
    { "int",   	       	INT,   	       	TT_C  	},
    { "long",  	       	LONG,  	       	TT_C  	},
    { "register",      	REGISTER,      	TT_C  	},
    { "return",        	RETURN,	       	TT_C  	},
    { "short", 	       	SHORT, 	       	TT_C  	},
    { "signed",	       	SIGNED,	       	TT_C  	},
    { "sizeof",        	SIZEOF,	       	TT_C  	},
    { "static",        	STATIC,	       	TT_C  	},
    { "struct",        	STRUCT,	       	TT_C  	},
    { "switch",        	SWITCH,	       	TT_C	},
    { "typedef",       	TYPEDEF,       	TT_C	},
    { "union", 	       	UNION, 	       	TT_C	},
    { "unsigned",      	UNSIGNED,      	TT_C	},
    { "void",  	       	VOID,  	       	TT_C	},
    { "volatile",      	VOLATILE,      	TT_C	},
    { "while", 	       	WHILE, 	       	TT_C	},
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



static int FindKey (char* Key)
/* Find a keyword and return the token. Return IDENT if the token is not a
 * keyword.
 */
{
    struct Keyword* K;
    K = bsearch (Key, Keywords, KEY_COUNT, sizeof (Keywords [0]), CmpKey);
    if (K && (K->Type != TT_EXT || ANSI == 0)) {
	return K->Tok;
    } else {
	return IDENT;
    }
}



static int skipwhite (void)
/* Skip white space in the input stream, reading and preprocessing new lines
 * if necessary. Return 0 if end of file is reached, return 1 otherwise.
 */
{
    while (1) {
       	while (*lptr == 0) {
	    if (readline () == 0) {
	     	return 0;
     	    }
	    preprocess ();
     	}
	if (*lptr == ' ' || *lptr == '\r') {
    	    ++lptr;
	} else {
    	    return 1;
	}
    }
}



void symname (char *s)
/* Get symbol from input stream */
{
    unsigned k = 0;
    do {
     	if (k != MAX_IDENTLEN) {
     	    ++k;
     	    *s++ = *lptr;
     	}
     	++lptr;
    } while (IsIdent (*lptr) || isdigit (*lptr));
    *s = '\0';
}



int issym (char *s)
/* Get symbol from input stream or return 0 if not a symbol. */
{
    if (IsIdent (*lptr)) {
     	symname (s);
     	return 1;
    } else {
     	return 0;
    }
}



static void unknown (unsigned char c)
/* Error message for unknown character */
{
    Error (ERR_INVALID_CHAR, c);
    gch ();			/* Skip */
}



static unsigned hexval (int c)
/* Convert a hex digit into a value */
{
    if (!isxdigit (c)) {
	Error (ERR_ILLEGAL_HEX_DIGIT);
    }
    if (isdigit (c)) {
	return c - '0';
    } else {
       	return toupper (c) - 'A' + 10;
    }
}



static void SetTok (int tok)
/* set nxttok and bump line ptr */
{
    nxttok = tok;
    ++lptr;
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



static int parsechar (int c)
/* Parse a character. Converts \n into EOL, etc. */
{
    int i;
    int val;

    /* Check for escape chars */
    if (c == '\\') {
	switch (c = gch ()) {
	    case 'b':
	   	c = '\b';
	   	break;
	    case 'f':
	   	c = '\f';
		break;
	    case 'r':
		c = '\r';
		break;
	    case 'n':
		c = '\n';
		break;
	    case 't':
		c = '\t';
		break;
	    case '\"':
		c = '\"';
		break;
	    case '\'':
		c = '\'';
		break;
	    case '\\':
		c = '\\';
		break;
	    case 'x':
	    case 'X':
		/* Hex character constant */
		val = hexval (gch ()) << 4;
       	       	c = val | hexval (gch ()); 	/* Do not translate */
		break;
	    case '0':
	    case '1':
		/* Octal constant */
		i = 0;
		val = c - '0';
		while ((c = *lptr) >= '0' && c <= '7' && i++ < 4) {
		    val = (val << 3) | (c - '0');
		    gch ();
		}
		c = val;	     	/* Do not translate */
		break;
	    default:
		Error (ERR_ILLEGAL_CHARCONST);
	}
    }

    /* Do correct sign extension */
    return SignExtendChar (c);
}



static void CharConst (void)
/* Parse a character constant. */
{
    int c;

    /* Skip the quote */
    ++lptr;

    /* Get character */
    c = parsechar (cgch ());

    /* Check for closing quote */
    if (cgch () != '\'') {
       	Error (ERR_QUOTE_EXPECTED);
    }

    /* Setup values and attributes */
    nxttok  = CCONST;
    nxtval  = SignExtendChar (ctrans (c)); 	/* Translate into target charset */
    nxttype = type_int;	       			/* Character constants have type int */
}



static void StringConst (void)
/* Parse a quoted string */
{
    nxtval = GetLiteralOffs ();
    nxttok = SCONST;

    /* Be sure to concatenate strings */
    while (*lptr == '\"') {

	/* Skip the quote char */
	++lptr;

	while (*lptr != '\"') {
	    if (*lptr == 0) {
	     	Error (ERR_UNEXPECTED_NEWLINE);
	     	break;
	    }
	    AddLiteralChar (parsechar (gch()));
	}

	/* Skip closing quote char if there was one */
	cgch ();

	/* Skip white space, read new input */
	skipwhite ();

    }

    /* Terminate the string */
    AddLiteralChar ('\0');
}



void gettok (void)
/* Get next token from input stream */
{
    char c;
    ident token;

    /* Current token is the lookahead token */
    CurTok = NextTok;

    /* Remember the starting position of the next token */
    NextTok.Pos = ln;

    /* Skip spaces and read the next line if needed */
    if (skipwhite () == 0) {
	/* End of file reached */
	nxttok = CEOF;
	return;
    }

    /* Determine the next token from the lookahead */
    c = *lptr;
    if (isdigit (c)) {

     	/* A number */
   	int HaveSuffix;		/* True if we have a type suffix */
     	unsigned types;		/* Possible types */
     	unsigned base;
     	unsigned long k;	/* Value */

     	k     = 0;
     	base  = 10;
     	types = IT_INT | IT_LONG | IT_ULONG;

       	if (c == '0') {
     	    /* Octal or hex constants may also be of type unsigned int */
     	    types = IT_INT | IT_UINT | IT_LONG | IT_ULONG;
     	    /* gobble 0 and examin next char */
     	    if (toupper (*++lptr) == 'X') {
     	     	base = 16;
     	    	nxttype = type_uint;
     	     	++lptr;	     		/* gobble "x" */
     	    } else {
     	     	base = 8;
     	    }
     	}
     	while (1) {
     	    c = *lptr;
     	    if (isdigit (c)) {
     	     	k = k * base + (c - '0');
     	    } else if (base == 16 && isxdigit (c)) {
     	     	k = (k << 4) + hexval (c);
     	    } else {
     	     	break; 	      	/* not digit */
     	    }
       	    ++lptr;   		/* gobble char */
     	}

     	/* Check for a suffix */
	HaveSuffix = 1;
     	c = toupper (*lptr);
     	if (c == 'U') {
     	    /* Unsigned type */
     	    ++lptr;
     	    if (toupper (*lptr) != 'L') {
     	    	types = IT_UINT | IT_ULONG;
     	    } else {
     	    	++lptr;
     	    	types = IT_ULONG;
     	    }
     	} else if (c == 'L') {
     	    /* Long type */
     	    ++lptr;
     	    if (toupper (*lptr) != 'U') {
     	    	types = IT_LONG | IT_ULONG;
     	    } else {
     	    	++lptr;
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
		Warning (WARN_CONSTANT_IS_LONG);
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
     	    nxttype = type_int;
     	} else if (types & IT_UINT) {
     	    nxttype = type_uint;
     	} else if (types & IT_LONG) {
     	    nxttype = type_long;
     	} else {
     	    nxttype = type_ulong;
     	}

     	/* Set the value and the token */
     	nxtval = k;
     	nxttok = ICONST;
     	return;
    }

    if (issym (token)) {

     	/* Check for a keyword */
     	if ((nxttok = FindKey (token)) != IDENT) {
     	    /* Reserved word found */
     	    return;
     	}
     	/* No reserved word, check for special symbols */
     	if (token [0] == '_') {
     	    /* Special symbols */
     	    if (strcmp (token, "__FILE__") == 0) {
	       	nxtval = AddLiteral (fin);
	       	nxttok = SCONST;
	       	return;
	    } else if (strcmp (token, "__LINE__") == 0) {
	       	nxttok  = ICONST;
    	       	nxtval  = ln;
    	       	nxttype = type_int;
    	       	return;
    	    } else if (strcmp (token, "__fixargs__") == 0) {
    	       	nxttok  = ICONST;
    	       	nxtval  = GetParamSize (CurrentFunc);
    	       	nxttype = type_uint;
    	       	return;
    	    } else if (strcmp (token, "__func__") == 0) {
	       	/* __func__ is only defined in functions */
	       	if (CurrentFunc) {
	       	    nxtval = AddLiteral (GetFuncName (CurrentFunc));
	       	    nxttok = SCONST;
	       	    return;
	       	}
	    }
    	}

       	/* No reserved word but identifier */
	strcpy (NextTok.Ident, token);
     	NextTok.Tok = IDENT;
    	return;
    }

    /* Monstrous switch statement ahead... */
    switch (c) {

    	case '!':
    	    if (*++lptr == '=') {
    		SetTok (NE);
    	    } else {
    		nxttok = BANG;
    	    }
    	    break;

    	case '\"':
       	    StringConst ();
    	    break;

    	case '%':
    	    if (*++lptr == '=') {
    		SetTok (MOASGN);
    	    } else {
    		nxttok = MOD;
    	    }
    	    break;

    	case '&':
    	    switch (*++lptr) {
    		case '&':
    		    SetTok (DAMP);
    		    break;
    		case '=':
    		    SetTok (AASGN);
    	      	    break;
    		default:
    		    nxttok = AMP;
    	    }
    	    break;

    	case '\'':
    	    CharConst ();
    	    break;

    	case '(':
    	    SetTok (LPAREN);
    	    break;

    	case ')':
    	    SetTok (RPAREN);
    	    break;

    	case '*':
    	    if (*++lptr == '=') {
    		SetTok (MASGN);
    	    } else {
    		nxttok = STAR;
    	    }
    	    break;

    	case '+':
    	    switch (*++lptr) {
    	    	case '+':
    		    SetTok (INC);
    		    break;
    	     	case '=':
    		    SetTok (PASGN);
    		    break;
    		default:
    		    nxttok = PLUS;
    	    }
    	    break;

    	case ',':
    	    SetTok (COMMA);
    	    break;

    	case '-':
    	    switch (*++lptr) {
    	      	case '-':
    		    SetTok (DEC);
    		    break;
    		case '=':
    	    	    SetTok (SASGN);
    		    break;
    		case '>':
    	    	    SetTok (PREF);
    		    break;
    		default:
    		    nxttok = MINUS;
    	    }
    	    break;

    	case '.':
    	    if (*++lptr == '.') {
    		if (*++lptr == '.') {
    		    SetTok (ELLIPSIS);
    		} else {
    		    unknown (*lptr);
    		}
    	    } else {
    		nxttok = DOT;
    	    }
    	    break;

    	case '/':
    	    if (*++lptr == '=') {
    		SetTok (DASGN);
    	    } else {
    		nxttok = DIV;
    	    }
    	    break;

    	case ':':
    	    SetTok (COLON);
    	    break;

    	case ';':
    	    SetTok (SEMI);
    	    break;

    	case '<':
    	    switch (*++lptr) {
    		case '=':
    	      	    SetTok (LE);
    	    	    break;
    		case '<':
    		    if (*++lptr == '=') {
    		    	SetTok (SLASGN);
    		    } else {
    		    	nxttok = ASL;
    	    	    }
    		    break;
    		default:
    		    nxttok = LT;
    	    }
    	    break;

    	case '=':
    	    if (*++lptr == '=') {
    		SetTok (EQ);
    	    } else {
    		nxttok = ASGN;
    	    }
    	    break;

    	case '>':
    	    switch (*++lptr) {
    		case '=':
    		    SetTok (GE);
    		    break;
    		case '>':
    		    if (*++lptr == '=') {
    		    	SetTok (SRASGN);
    		    } else {
    	     	    	nxttok = ASR;
    		    }
    		    break;
    		default:
    		    nxttok = GT;
    	    }
    	    break;

    	case '?':
    	    SetTok (QUEST);
    	    break;

    	case '[':
    	    SetTok (LBRACK);
    	    break;

    	case ']':
    	    SetTok (RBRACK);
    	    break;

    	case '^':
    	    if (*++lptr == '=') {
    		SetTok (XOASGN);
    	    } else {
    		nxttok = XOR;
    	    }
    	    break;

    	case '{':
    	    SetTok (LCURLY);
    	    break;

        case '|':
    	    switch (*++lptr) {
    		case '|':
    		    SetTok (DBAR);
    		    break;
    		case '=':
    		    SetTok (OASGN);
    		    break;
    		default:
    		    nxttok = BAR;
    	    }
    	    break;

    	case '}':
    	    SetTok (RCURLY);
    	    break;

    	case '~':
    	    SetTok (COMP);
    	    break;

        case '#':
	    while (*++lptr == ' ') ;	/* Skip it and following whitespace */
	    if (!issym (token) || strcmp (token, "pragma") != 0) {
	      	/* OOPS - should not happen */
	      	Error (ERR_CPP_DIRECTIVE_EXPECTED);
	    }
	    nxttok = PRAGMA;
	    break;

    	default:
       	    unknown (c);

    }

}



void Consume (unsigned Token, unsigned char ErrNum)
/* Eat token if it is the next in the input stream, otherwise print an error
 * message.
 */
{
    if (curtok == Token) {
	gettok ();
    } else {
       	Error (ErrNum);
    }
}



void ConsumeColon (void)
/* Check for a colon and skip it. */
{
    Consume (COLON, ERR_COLON_EXPECTED);
}



void ConsumeSemi (void)
/* Check for a semicolon and skip it. */
{
    /* Try do be smart about typos... */
    if (curtok == SEMI) {
	gettok ();
    } else {
	Error (ERR_SEMICOLON_EXPECTED);
	if (curtok == COLON || curtok == COMMA) {
	    gettok ();
	}
    }
}



void ConsumeLParen (void)
/* Check for a left parenthesis and skip it */
{
    Consume (LPAREN, ERR_LPAREN_EXPECTED);
}



void ConsumeRParen (void)
/* Check for a right parenthesis and skip it */
{
    Consume (RPAREN, ERR_RPAREN_EXPECTED);
}



void ConsumeLBrack (void)
/* Check for a left bracket and skip it */
{
    Consume (LBRACK, ERR_LBRACK_EXPECTED);
}



void ConsumeRBrack (void)
/* Check for a right bracket and skip it */
{
    Consume (RBRACK, ERR_RBRACK_EXPECTED);
}



void ConsumeLCurly (void)
/* Check for a left curly brace and skip it */
{
    Consume (LCURLY, ERR_LCURLY_EXPECTED);
}



void ConsumeRCurly (void)
/* Check for a right curly brace and skip it */
{
    Consume (RCURLY, ERR_RCURLY_EXPECTED);
}



