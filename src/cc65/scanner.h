/*
 * scanner.h
 *
 * Ullrich von Bassewitz, 07.06.1998
 */



#ifndef SCANNER_H
#define SCANNER_H



#include "datatype.h"
#include "ident.h"



/*****************************************************************************/
/*			       Token definitions			     */
/*****************************************************************************/



typedef enum token_t {
    TOK_CEOF,

    TOK_AUTO,
    TOK_EXTERN,
    TOK_REGISTER,
    TOK_STATIC,
    TOK_TYPEDEF,
    TOK_ENUM,
    TOK_CONST,
    TOK_VOLATILE,

    /* Tokens denoting types */
    TOK_FIRSTTYPE,
    TOK_CHAR		= TOK_FIRSTTYPE,
    TOK_INT,
    TOK_DOUBLE,
    TOK_FLOAT,
    TOK_LONG,
    TOK_UNSIGNED,
    TOK_SIGNED,
    TOK_SHORT,
    TOK_STRUCT,
    TOK_UNION,
    TOK_VOID,
    TOK_LASTTYPE	= TOK_VOID,

    /* Control statements */
    TOK_DO,
    TOK_FOR,
    TOK_GOTO,
    TOK_IF,
    TOK_RETURN,
    TOK_SWITCH,
    TOK_WHILE,

    TOK_ASM,
    TOK_CASE,
    TOK_DEFAULT,
    TOK_BREAK,
    TOK_CONTINUE,
    TOK_ELSE,
    TOK_ELLIPSIS,
    TOK_SIZEOF,

    TOK_IDENT,
    TOK_SEMI,

    /* Primary operators */
    TOK_LBRACK,
    TOK_LPAREN,
    TOK_DOT,
    TOK_PTR_REF,

    TOK_LCURLY,
    TOK_RBRACK,
    TOK_COMP,
    TOK_INC,
    TOK_PLUS_ASSIGN,
    TOK_PLUS,
    TOK_COMMA,
    TOK_DEC,
    TOK_MINUS_ASSIGN,
    TOK_RCURLY,
    TOK_MINUS,
    TOK_MUL_ASSIGN,
    TOK_STAR,
    TOK_DIV_ASSIGN,
    TOK_DIV,
    TOK_BOOL_AND,
    TOK_AND_ASSIGN,
    TOK_AND,
    TOK_NE,
    TOK_BOOL_NOT,
    TOK_BOOL_OR,
    TOK_OR_ASSIGN,
    TOK_OR,
    TOK_EQ,
    TOK_ASSIGN,
    TOK_SHL_ASSIGN,
    TOK_SHL,

    /* Inequalities */
    TOK_LE,
    TOK_LT,
    TOK_GE,
    TOK_GT,

    TOK_SHR_ASSIGN,
    TOK_SHR,
    TOK_XOR_ASSIGN,
    TOK_XOR,
    TOK_MOD_ASSIGN,
    TOK_MOD,
    TOK_QUEST,
    TOK_COLON,
    TOK_RPAREN,
    TOK_SCONST,
    TOK_ICONST,
    TOK_CCONST,
    TOK_FCONST,

    TOK_ATTRIBUTE,
    TOK_FASTCALL,
    TOK_AX,
    TOK_EAX,

    TOK_PRAGMA
} token_t;



/*****************************************************************************/
/*				     data				     */
/*****************************************************************************/



/* Token stuff */
typedef struct Token_ Token;
struct Token_ {
    token_t	Tok;		/* The token itself */
    long	IVal;		/* The integer attribute */
    ident	Ident;		/* Identifier if IDENT */
    unsigned	Pos;		/* Source line where the token comes from */
    type*	IType;		/* Type if integer constant */
};

extern Token CurTok;		/* The current token */
extern Token NextTok;		/* The next token */

/* Defines to make the old code work */
#define curtok 	CurTok.Tok
#define curval 	CurTok.IVal
#define curpos 	CurTok.Pos
#define curtype	CurTok.IType

#define nxttok 	NextTok.Tok
#define nxtval 	NextTok.IVal
#define nxtpos 	NextTok.Pos
#define nxttype	NextTok.IType



/*****************************************************************************/
/*	   			     code				     */
/*****************************************************************************/



void symname (char* s);
/* Get symbol from input stream */

int issym (char* s);
/* Get symbol from input stream or return 0 if not a symbol. */

void NextToken (void);		      
/* Get next token from input stream */

void Consume (token_t Token, unsigned ErrNum);
/* Eat token if it is the next in the input stream, otherwise print an error
 * message.
 */

void ConsumeColon (void);
/* Check for a colon and skip it. */

void ConsumeSemi (void);
/* Check for a semicolon and skip it. */

void ConsumeLParen (void);
/* Check for a left parenthesis and skip it */

void ConsumeRParen (void);
/* Check for a right parenthesis and skip it */

void ConsumeLBrack (void);
/* Check for a left bracket and skip it */

void ConsumeRBrack (void);
/* Check for a right bracket and skip it */

void ConsumeLCurly (void);
/* Check for a left curly brace and skip it */

void ConsumeRCurly (void);
/* Check for a right curly brace and skip it */



/* End of scanner.h */
#endif






