/*****************************************************************************/
/*                                                                           */
/*				   scanner.h                                 */
/*                                                                           */
/*			Source file line info structure                      */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2004 Ullrich von Bassewitz                                       */
/*               Römerstrasse 52                                             */
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



#ifndef SCANNER_H
#define SCANNER_H



/* cc65 */
#include "datatype.h"
#include "ident.h"
#include "lineinfo.h"



/*****************************************************************************/
/*  			       Token definitions			     */
/*****************************************************************************/



typedef enum token_t {
    TOK_INVALID,
    TOK_CEOF,

    TOK_AUTO,
    TOK_EXTERN,
    TOK_REGISTER,
    TOK_RESTRICT,
    TOK_STATIC,
    TOK_TYPEDEF,
    TOK_CONST,
    TOK_VOLATILE,

    /* Tokens denoting types */
    TOK_FIRSTTYPE,
    TOK_ENUM		= TOK_FIRSTTYPE,
    TOK_CHAR,
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
    TOK_MUL = TOK_STAR,		/* Alias */
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

    /* Inequalities */
    TOK_LE,
    TOK_LT,
    TOK_GE,
    TOK_GT,

    TOK_SHL_ASSIGN,
    TOK_SHL,
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
    TOK_FAR,
    TOK_NEAR,
    TOK_FASTCALL,
    TOK_A,
    TOK_X,
    TOK_Y,
    TOK_AX,
    TOK_EAX,

    TOK_PRAGMA
} token_t;



/*****************************************************************************/
/*				     data				     */
/*****************************************************************************/



/* Token stuff */
typedef struct Token Token;
struct Token {
    token_t 	Tok;		/* The token itself */
    long    	IVal;		/* The integer attribute */
    double  	FVal;		/* The float attribute */
    ident   	Ident;		/* Identifier if IDENT */
    LineInfo*   LI;		/* Source line where the token comes from */
    type*	Type;		/* Type if integer or float constant */
};

extern Token CurTok;		/* The current token */
extern Token NextTok;		/* The next token */



/*****************************************************************************/
/*	   			     code     				     */
/*****************************************************************************/



void SymName (char* s);
/* Get symbol from input stream */

int IsSym (char* s);
/* Get symbol from input stream or return 0 if not a symbol. */

void NextToken (void);
/* Get next token from input stream */

void SkipTokens (const token_t* TokenList, unsigned TokenCount);
/* Skip tokens until we reach TOK_CEOF or a token in the given token list.
 * This routine is used for error recovery.
 */

int Consume (token_t Token, const char* ErrorMsg);
/* Eat token if it is the next in the input stream, otherwise print an error
 * message. Returns true if the token was found and false otherwise.
 */

int ConsumeColon (void);
/* Check for a colon and skip it. */

int ConsumeSemi (void);
/* Check for a semicolon and skip it. */

int ConsumeComma (void);
/* Check for a comma and skip it. */

int ConsumeLParen (void);
/* Check for a left parenthesis and skip it */

int ConsumeRParen (void);
/* Check for a right parenthesis and skip it */

int ConsumeLBrack (void);
/* Check for a left bracket and skip it */

int ConsumeRBrack (void);
/* Check for a right bracket and skip it */

int ConsumeLCurly (void);
/* Check for a left curly brace and skip it */

int ConsumeRCurly (void);
/* Check for a right curly brace and skip it */



/* End of scanner.h */
#endif






