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
    TOK_NORETURN,
    TOK_FASTCALL,
    TOK_CDECL,

    /* Address sizes */
    TOK_FAR,
    TOK_NEAR,

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

    /* Selection statements */
    TOK_IF,
    TOK_ELSE,
    TOK_SWITCH,

    /* Iteration statements */
    TOK_WHILE,
    TOK_DO,
    TOK_FOR,

    /* Jump statements */
    TOK_GOTO,
    TOK_CONTINUE,
    TOK_BREAK,
    TOK_RETURN,

    /* Labels */
    TOK_CASE,
    TOK_DEFAULT,

    /* Misc. */
    TOK_ATTRIBUTE,
    TOK_PRAGMA,
    TOK_STATIC_ASSERT,
    TOK_ASM,
    TOK_SIZEOF,

    /* Punctuators */
    TOK_FIRST_PUNC,
    TOK_LBRACK          = TOK_FIRST_PUNC,
    TOK_RBRACK,
    TOK_LPAREN,
    TOK_RPAREN,
    TOK_LCURLY,
    TOK_RCURLY,
    TOK_DOT,
    TOK_PTR_REF,
    TOK_INC,
    TOK_DEC,
    TOK_ADDR,
    TOK_AND = TOK_ADDR,         /* Alias */
    TOK_STAR,
    TOK_MUL = TOK_STAR,         /* Alias */
    TOK_PLUS,
    TOK_MINUS,
    TOK_COMP,
    TOK_BOOL_NOT,
    TOK_DIV,
    TOK_MOD,
    TOK_SHL,
    TOK_SHR,
    TOK_LT,
    TOK_GT,
    TOK_LE,
    TOK_GE,
    TOK_EQ,
    TOK_NE,
    TOK_XOR,
    TOK_OR,
    TOK_BOOL_AND,
    TOK_BOOL_OR,
    TOK_QUEST,
    TOK_COLON,
    TOK_SEMI,
    TOK_ELLIPSIS,
    TOK_ASSIGN,
    TOK_MUL_ASSIGN,
    TOK_DIV_ASSIGN,
    TOK_MOD_ASSIGN,
    TOK_PLUS_ASSIGN,
    TOK_MINUS_ASSIGN,
    TOK_SHL_ASSIGN,
    TOK_SHR_ASSIGN,
    TOK_AND_ASSIGN,
    TOK_XOR_ASSIGN,
    TOK_OR_ASSIGN,
    TOK_COMMA,
    TOK_HASH,
    TOK_HASH_HASH,
    TOK_DOUBLE_HASH     = TOK_HASH_HASH,    /* Alias */
    TOK_LAST_PUNC       = TOK_DOUBLE_HASH,

    /* Primary expressions */
    TOK_ICONST,
    TOK_CCONST,
    TOK_WCCONST,
    TOK_FCONST,
    TOK_SCONST,
    TOK_WCSCONST,
    TOK_IDENT,
    TOK_A,
    TOK_X,
    TOK_Y,
    TOK_AX,
    TOK_EAX
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
    const Type*     Type;       /* Type if integer or float constant */
};

extern Token    CurTok;             /* The current token */
extern Token    NextTok;            /* The next token */
extern int      PPParserRunning;    /* Is tokenizer used by the preprocessor */
extern int      NoCharMap;          /* Disable literal translation */
extern unsigned InPragmaParser;     /* Depth of pragma parser calling */



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



#if defined(HAVE_INLINE)
INLINE int TokIsPunc (const Token* T)
/* Return true if the token is a punctuator */
{
    return (T->Tok >= TOK_FIRST_PUNC && T->Tok <= TOK_LAST_PUNC);
}
#else
#  define TokIsPunc(T)  \
        ((T)->Tok >= TOK_FIRST_PUNC && (T)->Tok <= TOK_LAST_PUNC)
#endif

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

int IsWideQuoted (char First, char Second);
/* Return 1 if the two successive characters indicate a wide string literal or
** a wide char constant, otherwise return 0.
*/

int IsSym (char* S);
/* If a symbol follows, read it and return 1, otherwise return 0 */

int IsPPNumber (int Cur, int Next);
/* Return 1 if the two successive characters indicate a pp-number, otherwise
** return 0.
*/

void CopyPPNumber (StrBuf* Target);
/* Copy a pp-number from the input to Target */

void NextToken (void);
/* Get next non-pragma token from input stream consuming any pragmas
** encountered. Adjacent string literal tokens will be concatenated.
*/

void SkipTokens (const token_t* TokenList, unsigned TokenCount);
/* Skip tokens until we reach TOK_CEOF or a token in the given token list.
** This routine is used for error recovery.
*/

int SmartErrorSkip (int TillEnd);
/* Try some smart error recovery.
**
** - If TillEnd == 0:
**   Skip tokens until a comma or closing curly brace that is not enclosed in
**   an open parenthesis/bracket/curly brace, or until a semicolon, EOF or
**   unpaired right parenthesis/bracket/curly brace is reached. The closing
**   curly brace is consumed in the former case.
**
** - If TillEnd != 0:
**   Skip tokens until a right curly brace or semicolon is reached and consumed
**   while there are no open parentheses/brackets/curly braces, or until an EOF
**   is reached anytime. Any open parenthesis/bracket/curly brace is considered
**   to be closed by consuming a right parenthesis/bracket/curly brace even if
**   they didn't match.
**
** - Return -1:
**   If this exits at a semicolon or unpaired right parenthesis/bracket/curly
**   brace while there are still open parentheses/brackets/curly braces.
**
** - Return 0:
**   If this exits as soon as it reaches an EOF;
**   Or if this exits right after consuming a semicolon or right curly brace
**   while there are no open parentheses/brackets/curly braces.
**
** - Return 1:
**   If this exits at a non-EOF without consuming it.
*/

int SimpleErrorSkip (void);
/* Skip tokens until an EOF or unpaired right parenthesis/bracket/curly brace
** is reached. Return 0 If this exits at an EOF. Otherwise return -1.
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
