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
static struct Keyword {
    char*    	    Key;    	/* Keyword name */
    unsigned char   Tok;    	/* The token */
    unsigned char   Type;      	/* Token type */
} Keywords [] = {
    { "__AX__",	       	TOK_AX,		TT_C	},
    { "__EAX__",       	TOK_EAX,   	TT_C	},
    { "__asm__",       	TOK_ASM,   	TT_C	},
    { "__attribute__",	TOK_ATTRIBUTE,	TT_C	},
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
	return TOK_IDENT;
    }
}



static int skipwhite (void)
/* Skip white space in the input stream, reading and preprocessing new lines
 * if necessary. Return 0 if end of file is reached, return 1 otherwise.
 */
{
    while (1) {
       	while (*lptr == 0) {
	    if (NextLine () == 0) {
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
    nxttok  = TOK_CCONST;
    nxtval  = SignExtendChar (ctrans (c)); 	/* Translate into target charset */
    nxttype = type_int;	       			/* Character constants have type int */
}



static void StringConst (void)
/* Parse a quoted string */
{
    nxtval = GetLiteralOffs ();
    nxttok = TOK_SCONST;

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



void NextToken (void)
/* Get next token from input stream */
{
    char c;
    ident token;

    /* Current token is the lookahead token */
    CurTok = NextTok;

    /* Remember the starting position of the next token */
    NextTok.Pos = GetCurrentLine();

    /* Skip spaces and read the next line if needed */
    if (skipwhite () == 0) {
	/* End of file reached */
	nxttok = TOK_CEOF;
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
     	nxttok = TOK_ICONST;
     	return;
    }

    if (issym (token)) {

     	/* Check for a keyword */
     	if ((nxttok = FindKey (token)) != TOK_IDENT) {
     	    /* Reserved word found */
     	    return;
     	}
     	/* No reserved word, check for special symbols */
     	if (token [0] == '_') {
     	    /* Special symbols */
     	    if (strcmp (token, "__FILE__") == 0) {
	       	nxtval = AddLiteral (GetCurrentFile());
	       	nxttok = TOK_SCONST;
	       	return;
	    } else if (strcmp (token, "__LINE__") == 0) {
	       	nxttok  = TOK_ICONST;
    	       	nxtval  = GetCurrentLine();
    	       	nxttype = type_int;
    	       	return;
    	    } else if (strcmp (token, "__fixargs__") == 0) {
    	       	nxttok  = TOK_ICONST;
    	       	nxtval  = GetParamSize (CurrentFunc);
    	       	nxttype = type_uint;
    	       	return;
    	    } else if (strcmp (token, "__func__") == 0) {
	       	/* __func__ is only defined in functions */
	       	if (CurrentFunc) {
	       	    nxtval = AddLiteral (GetFuncName (CurrentFunc));
	       	    nxttok = TOK_SCONST;
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
    switch (c) {

    	case '!':
    	    if (*++lptr == '=') {
    		SetTok (TOK_NE);
    	    } else {
    		nxttok = TOK_BOOL_NOT;
    	    }
    	    break;

    	case '\"':
       	    StringConst ();
    	    break;

    	case '%':
    	    if (*++lptr == '=') {
    		SetTok (TOK_MOD_ASSIGN);
    	    } else {
    		nxttok = TOK_MOD;
    	    }
    	    break;

    	case '&':
    	    switch (*++lptr) {
    		case '&':
    		    SetTok (TOK_BOOL_AND);
    		    break;
    		case '=':
    		    SetTok (TOK_AND_ASSIGN);
    	      	    break;
    		default:
    		    nxttok = TOK_AND;
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
    	    if (*++lptr == '=') {
    		SetTok (TOK_MUL_ASSIGN);
    	    } else {
    		nxttok = TOK_STAR;
    	    }
    	    break;

    	case '+':
    	    switch (*++lptr) {
    	    	case '+':
    		    SetTok (TOK_INC);
    		    break;
    	     	case '=':
    		    SetTok (TOK_PLUS_ASSIGN);
    		    break;
    		default:
    		    nxttok = TOK_PLUS;
    	    }
    	    break;

    	case ',':
    	    SetTok (TOK_COMMA);
    	    break;

    	case '-':
    	    switch (*++lptr) {
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
    		    nxttok = TOK_MINUS;
    	    }
    	    break;

    	case '.':
    	    if (*++lptr == '.') {
    		if (*++lptr == '.') {
    		    SetTok (TOK_ELLIPSIS);
    		} else {
    		    unknown (*lptr);
    		}
    	    } else {
    		nxttok = TOK_DOT;
    	    }
    	    break;

    	case '/':
    	    if (*++lptr == '=') {
    		SetTok (TOK_DIV_ASSIGN);
    	    } else {
    	     	nxttok = TOK_DIV;
    	    }
    	    break;

    	case ':':
    	    SetTok (TOK_COLON);
    	    break;

    	case ';':
    	    SetTok (TOK_SEMI);
    	    break;

    	case '<':
    	    switch (*++lptr) {
    		case '=':
    	      	    SetTok (TOK_LE);
    	    	    break;
    		case '<':
    		    if (*++lptr == '=') {
    		    	SetTok (TOK_SHL_ASSIGN);
    		    } else {
    		    	nxttok = TOK_SHL;
    	    	    }
    		    break;
    		default:
    		    nxttok = TOK_LT;
    	    }
    	    break;

    	case '=':
    	    if (*++lptr == '=') {
    		SetTok (TOK_EQ);
    	    } else {
    		nxttok = TOK_ASSIGN;
    	    }
    	    break;

    	case '>':
    	    switch (*++lptr) {
    		case '=':
    		    SetTok (TOK_GE);
    		    break;
    		case '>':
    		    if (*++lptr == '=') {
    		    	SetTok (TOK_SHR_ASSIGN);
    		    } else {
    	     	    	nxttok = TOK_SHR;
    		    }
    		    break;
    		default:
    		    nxttok = TOK_GT;
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
    	    if (*++lptr == '=') {
    		SetTok (TOK_XOR_ASSIGN);
    	    } else {
    		nxttok = TOK_XOR;
    	    }
    	    break;

    	case '{':
    	    SetTok (TOK_LCURLY);
    	    break;

        case '|':
    	    switch (*++lptr) {
    		case '|':
    		    SetTok (TOK_BOOL_OR);
    		    break;
    		case '=':
    		    SetTok (TOK_OR_ASSIGN);
    		    break;
    		default:
    		    nxttok = TOK_OR;
    	    }
    	    break;

    	case '}':
    	    SetTok (TOK_RCURLY);
    	    break;

    	case '~':
    	    SetTok (TOK_COMP);
    	    break;

        case '#':
	    while (*++lptr == ' ') ;	/* Skip it and following whitespace */
	    if (!issym (token) || strcmp (token, "pragma") != 0) {
	      	/* OOPS - should not happen */
	      	Error (ERR_CPP_DIRECTIVE_EXPECTED);
	    }
	    nxttok = TOK_PRAGMA;
	    break;

    	default:
       	    unknown (c);

    }

}



void Consume (token_t Token, unsigned ErrNum)
/* Eat token if it is the next in the input stream, otherwise print an error
 * message.
 */
{
    if (curtok == Token) {
	NextToken ();
    } else {
       	Error (ErrNum);
    }
}



void ConsumeColon (void)
/* Check for a colon and skip it. */
{
    Consume (TOK_COLON, ERR_COLON_EXPECTED);
}



void ConsumeSemi (void)
/* Check for a semicolon and skip it. */
{
    /* Try do be smart about typos... */
    if (curtok == TOK_SEMI) {
	NextToken ();
    } else {
	Error (ERR_SEMICOLON_EXPECTED);
	if (curtok == TOK_COLON || curtok == TOK_COMMA) {
	    NextToken ();
	}
    }
}



void ConsumeLParen (void)
/* Check for a left parenthesis and skip it */
{
    Consume (TOK_LPAREN, ERR_LPAREN_EXPECTED);
}



void ConsumeRParen (void)
/* Check for a right parenthesis and skip it */
{
    Consume (TOK_RPAREN, ERR_RPAREN_EXPECTED);
}



void ConsumeLBrack (void)
/* Check for a left bracket and skip it */
{
    Consume (TOK_LBRACK, ERR_LBRACK_EXPECTED);
}



void ConsumeRBrack (void)
/* Check for a right bracket and skip it */
{
    Consume (TOK_RBRACK, ERR_RBRACK_EXPECTED);
}



void ConsumeLCurly (void)
/* Check for a left curly brace and skip it */
{
    Consume (TOK_LCURLY, ERR_LCURLY_EXPECTED);
}



void ConsumeRCurly (void)
/* Check for a right curly brace and skip it */
{
    Consume (TOK_RCURLY, ERR_RCURLY_EXPECTED);
}



