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
/*			       token definitions			     */
/*****************************************************************************/



#define CEOF 		0

#define AUTO	     	10
#define EXTERN	     	11
#define REGISTER     	12
#define STATIC	     	13
#define TYPEDEF	     	14
#define ENUM		15
#define CONST		16
#define VOLATILE	17

#define FIRSTTYPE      	19
#define CHAR	     	19
#define INT	     	20
#define DOUBLE	     	21
#define FLOAT	     	22
#define LONG	     	23
#define UNSIGNED     	24
#define SIGNED	     	25
#define SHORT	     	26
#define STRUCT	     	27
#define UNION	     	28
#define VOID	     	29
#define LASTTYPE     	29

#define DO		30
#define FOR		31
#define GOTO		32
#define IF		33
#define RETURN		34
#define SWITCH		35
#define WHILE		36

#define ASM		40
#define CASE		41
#define DEFAULT		42
#define BREAK		43
#define CONTINUE	44
#define ELSE		45
#define ELLIPSIS	46
#define SIZEOF 		47

#define IDENT 		50
#define SEMI 		51

/* primary operators */
#define LBRACK	     	52
#define LPAREN	     	53
#define DOT	     	54
#define PREF	     	55

#define LCURLY	     	56
#define RBRACK 	     	57
#define COMP 	     	58
#define INC 	     	59
#define PASGN 	     	60
#define PLUS 	     	61
#define COMMA 	     	62
#define DEC 	     	63
#define SASGN 	     	64
#define RCURLY 	     	65
#define MINUS 	     	66
#define MASGN 	     	67
#define STAR 	     	68
#define DASGN 	     	69
#define DIV 	     	70
#define DAMP 	     	71
#define AASGN 	     	72
#define AMP 	     	73
#define NE 	     	74
#define BANG 	     	75
#define DBAR 	     	76
#define OASGN 	     	77
#define BAR 	     	78
#define EQ 	     	79
#define ASGN 	     	80
#define SLASGN 	     	81
#define ASL 	     	82

/* inequalities */
#define LE	     	83
#define LT	     	84
#define GE	     	85
#define GT	     	86

#define SRASGN 	     	87
#define ASR 	     	88
#define XOASGN 	     	89
#define XOR 	     	90
#define MOASGN 	     	91
#define MOD 	     	92
#define QUEST 	     	93
#define COLON 	     	94
#define RPAREN 	     	95
#define SCONST 	     	96
#define ICONST 	     	97
#define CCONST 	     	98
#define FCONST 	     	99

#define	FASTCALL	100
#define AX 	       	101
#define EAX		102

#define PRAGMA		110



/*****************************************************************************/
/*				     data				     */
/*****************************************************************************/



/* Token stuff */
typedef struct Token_ Token;
struct Token_ {
    unsigned	Tok;		/* The token itself */
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

void gettok (void);
/* Get next token from input stream */

void Consume (unsigned Token, unsigned char ErrNum);
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






