/*****************************************************************************/
/*                                                                           */
/*                                 scanner.h                                 */
/*                                                                           */
/*                      Source file line info structure                      */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2010, Ullrich von Bassewitz                                      */
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



#ifndef SCANNER_H
#define SCANNER_H



/* common */
#include "fp.h"

/* cc65 */
#include "datatype.h"
#include "ident.h"
#include "lineinfo.h"



/*****************************************************************************/
/*                             Token definitions                             */
/*****************************************************************************/



typedef enum token_t {
    TOK_INVALID,
    TOK_CEOF,

    /* Storage specifiers */
    TOK_FIRST_STORAGE_CLASS,
    TOK_AUTO            = TOK_FIRST_STORAGE_CLASS,
    TOK_EXTERN,
    TOK_REGISTER,
    TOK_STATIC,
    TOK_TYPEDEF,
    TOK_LAST_STORAGE_CLASS = TOK_TYPEDEF,

    /* Tokens denoting type qualifiers */
    TOK_FIRST_TYPEQUAL,
    TOK_CONST           = TOK_FIRST_TYPEQUAL,
    TOK_VOLATILE,
    TOK_RESTRICT,
    TOK_LAST_TYPEQUAL   = TOK_RESTRICT,

    /* Function specifiers */
    TOK_INLINE,
    TOK_FASTCALL,
    TOK_CDECL,

    /* Tokens denoting types */
    TOK_FIRST_TYPE,
    TOK_ENUM            = TOK_FIRST_TYPE,
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
    TOK_LAST_TYPE       = TOK_VOID,

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
    TOK_MUL = TOK_STAR,         /* Alias */
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
    TOK_WCSCONST,

    TOK_ATTRIBUTE,
    TOK_FAR,
    TOK_NEAR,
    TOK_A,
    TOK_X,
    TOK_Y,
    TOK_AX,
    TOK_EAX,

    TOK_PRAGMA
} token_t;



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Forward for struct Literal */
struct Literal;

/* Token stuff */
typedef struct Token Token;
struct Token {
    token_t         Tok;        /* The token itself */
    long            IVal;       /* The integer attribute */
    Double          FVal;       /* The float attribute */
    struct Literal* SVal;       /* String literal is any */
    ident           Ident;      /* Identifier if IDENT */
    LineInfo*       LI;         /* Source line where the token comes from */
    Type*           Type;       /* Type if integer or float constant */
};

extern Token CurTok;            /* The current token */
extern Token NextTok;           /* The next token */



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



#if defined(HAVE_INLINE)
INLINE int TokIsStorageClass (const Token* T)
/* Return true if the token is a storage class specifier */
{
    return (T->Tok >= TOK_FIRST_STORAGE_CLASS && T->Tok <= TOK_LAST_STORAGE_CLASS);
}
#else
#  define TokIsStorageClass(T)  \
        ((T)->Tok >= TOK_FIRST_STORAGE_CLASS && (T)->Tok <= TOK_LAST_STORAGE_CLASS)
#endif

#if defined(HAVE_INLINE)
INLINE int TokIsType (const Token* T)
/* Return true if the token is a type */
{
    return (T->Tok >= TOK_FIRST_TYPE && T->Tok <= TOK_LAST_TYPE);
}
#else
#  define TokIsType(T)  ((T)->Tok >= TOK_FIRST_TYPE && (T)->Tok <= TOK_LAST_TYPE)
#endif

#if defined(HAVE_INLINE)
INLINE int TokIsTypeQual (const Token* T)
/* Return true if the token is a type qualifier */
{
    return (T->Tok >= TOK_FIRST_TYPEQUAL && T->Tok <= TOK_LAST_TYPEQUAL);
}
#else
#  define TokIsTypeQual(T)  ((T)->Tok >= TOK_FIRST_TYPEQUAL && (T)->Tok <= TOK_LAST_TYPEQUAL)
#endif

int TokIsFuncSpec (const Token* T);
/* Return true if the token is a function specifier */

void SymName (char* S);
/* Read a symbol from the input stream. The first character must have been
** checked before calling this function. The buffer is expected to be at
** least of size MAX_IDENTLEN+1.
*/

int IsSym (char* S);
/* If a symbol follows, read it and return 1, otherwise return 0 */

void NextToken (void);
/* Get next token from input stream */

void SkipTokens (const token_t* TokenList, unsigned TokenCount);
/* Skip tokens until we reach TOK_CEOF or a token in the given token list.
** This routine is used for error recovery.
*/

int Consume (token_t Token, const char* ErrorMsg);
/* Eat token if it is the next in the input stream, otherwise print an error
** message. Returns true if the token was found and false otherwise.
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
