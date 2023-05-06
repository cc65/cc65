/*****************************************************************************/
/*                                                                           */
/*                                 scanner.c                                 */
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



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <math.h>
#include <inttypes.h>
#include <limits.h>

/* common */
#include "chartype.h"
#include "fp.h"
#include "tgttrans.h"

/* cc65 */
#include "datatype.h"
#include "error.h"
#include "function.h"
#include "global.h"
#include "hexval.h"
#include "ident.h"
#include "input.h"
#include "litpool.h"
#include "preproc.h"
#include "scanner.h"
#include "standard.h"
#include "symtab.h"



/*****************************************************************************/
/*                                   data                                    */
/*****************************************************************************/



Token CurTok;           /* The current token */
Token NextTok;          /* The next token */
int   PPParserRunning;  /* Is tokenizer used by the preprocessor */



/* Token types */
enum {
    TT_C89      = 0x01 << STD_C89,      /* Token valid in C89 */
    TT_C99      = 0x01 << STD_C99,      /* Token valid in C99 */
    TT_CC65     = 0x01 << STD_CC65      /* Token valid in cc65 */
};

/* Token table */
static const struct Keyword {
    char*           Key;        /* Keyword name */
    unsigned char   Tok;        /* The token */
    unsigned char   Std;        /* Token supported in which standards? */
} Keywords [] = {
    { "_Pragma",        TOK_PRAGMA,     TT_C89 | TT_C99 | TT_CC65  },   /* !! */
    { "_Static_assert", TOK_STATIC_ASSERT,                TT_CC65  },   /* C11 */
    { "__AX__",         TOK_AX,         TT_C89 | TT_C99 | TT_CC65  },
    { "__A__",          TOK_A,          TT_C89 | TT_C99 | TT_CC65  },
    { "__EAX__",        TOK_EAX,        TT_C89 | TT_C99 | TT_CC65  },
    { "__X__",          TOK_X,          TT_C89 | TT_C99 | TT_CC65  },
    { "__Y__",          TOK_Y,          TT_C89 | TT_C99 | TT_CC65  },
    { "__asm__",        TOK_ASM,        TT_C89 | TT_C99 | TT_CC65  },
    { "__attribute__",  TOK_ATTRIBUTE,  TT_C89 | TT_C99 | TT_CC65  },
    { "__cdecl__",      TOK_CDECL,      TT_C89 | TT_C99 | TT_CC65  },
    { "__far__",        TOK_FAR,        TT_C89 | TT_C99 | TT_CC65  },
    { "__fastcall__",   TOK_FASTCALL,   TT_C89 | TT_C99 | TT_CC65  },
    { "__inline__",     TOK_INLINE,     TT_C89 | TT_C99 | TT_CC65  },
    { "__near__",       TOK_NEAR,       TT_C89 | TT_C99 | TT_CC65  },
    { "asm",            TOK_ASM,                          TT_CC65  },
    { "auto",           TOK_AUTO,       TT_C89 | TT_C99 | TT_CC65  },
    { "break",          TOK_BREAK,      TT_C89 | TT_C99 | TT_CC65  },
    { "case",           TOK_CASE,       TT_C89 | TT_C99 | TT_CC65  },
    { "cdecl",          TOK_CDECL,                        TT_CC65  },
    { "char",           TOK_CHAR,       TT_C89 | TT_C99 | TT_CC65  },
    { "const",          TOK_CONST,      TT_C89 | TT_C99 | TT_CC65  },
    { "continue",       TOK_CONTINUE,   TT_C89 | TT_C99 | TT_CC65  },
    { "default",        TOK_DEFAULT,    TT_C89 | TT_C99 | TT_CC65  },
    { "do",             TOK_DO,         TT_C89 | TT_C99 | TT_CC65  },
    { "double",         TOK_DOUBLE,     TT_C89 | TT_C99 | TT_CC65  },
    { "else",           TOK_ELSE,       TT_C89 | TT_C99 | TT_CC65  },
    { "enum",           TOK_ENUM,       TT_C89 | TT_C99 | TT_CC65  },
    { "extern",         TOK_EXTERN,     TT_C89 | TT_C99 | TT_CC65  },
    { "far",            TOK_FAR,                          TT_CC65  },
    { "fastcall",       TOK_FASTCALL,                     TT_CC65  },
    { "float",          TOK_FLOAT,      TT_C89 | TT_C99 | TT_CC65  },
    { "for",            TOK_FOR,        TT_C89 | TT_C99 | TT_CC65  },
    { "goto",           TOK_GOTO,       TT_C89 | TT_C99 | TT_CC65  },
    { "if",             TOK_IF,         TT_C89 | TT_C99 | TT_CC65  },
    { "inline",         TOK_INLINE,              TT_C99 | TT_CC65  },
    { "int",            TOK_INT,        TT_C89 | TT_C99 | TT_CC65  },
    { "long",           TOK_LONG,       TT_C89 | TT_C99 | TT_CC65  },
    { "near",           TOK_NEAR,                         TT_CC65  },
    { "register",       TOK_REGISTER,   TT_C89 | TT_C99 | TT_CC65  },
    { "restrict",       TOK_RESTRICT,            TT_C99 | TT_CC65  },
    { "return",         TOK_RETURN,     TT_C89 | TT_C99 | TT_CC65  },
    { "short",          TOK_SHORT,      TT_C89 | TT_C99 | TT_CC65  },
    { "signed",         TOK_SIGNED,     TT_C89 | TT_C99 | TT_CC65  },
    { "sizeof",         TOK_SIZEOF,     TT_C89 | TT_C99 | TT_CC65  },
    { "static",         TOK_STATIC,     TT_C89 | TT_C99 | TT_CC65  },
    { "struct",         TOK_STRUCT,     TT_C89 | TT_C99 | TT_CC65  },
    { "switch",         TOK_SWITCH,     TT_C89 | TT_C99 | TT_CC65  },
    { "typedef",        TOK_TYPEDEF,    TT_C89 | TT_C99 | TT_CC65  },
    { "union",          TOK_UNION,      TT_C89 | TT_C99 | TT_CC65  },
    { "unsigned",       TOK_UNSIGNED,   TT_C89 | TT_C99 | TT_CC65  },
    { "void",           TOK_VOID,       TT_C89 | TT_C99 | TT_CC65  },
    { "volatile",       TOK_VOLATILE,   TT_C89 | TT_C99 | TT_CC65  },
    { "while",          TOK_WHILE,      TT_C89 | TT_C99 | TT_CC65  },
};
#define KEY_COUNT       (sizeof (Keywords) / sizeof (Keywords [0]))



/* Stuff for determining the type of an integer constant */
#define IT_INT          0x01
#define IT_UINT         0x02
#define IT_LONG         0x04
#define IT_ULONG        0x08


/* Internal type for numeric constant scanning.
** Size must be explicit for cross-platform uniformity.
*/
typedef uint32_t scan_t;


/*****************************************************************************/
/*                                   code                                    */
/*****************************************************************************/



static int CmpKey (const void* Key, const void* Elem)
/* Compare function for bsearch */
{
    return strcmp ((const char*) Key, ((const struct Keyword*) Elem)->Key);
}



static token_t FindKey (const char* Key)
/* Find a keyword and return the token. Return IDENT if the token is not a
** keyword.
*/
{
    struct Keyword* K;
    K = bsearch (Key, Keywords, KEY_COUNT, sizeof (Keywords [0]), CmpKey);
    if (K && (K->Std & (0x01 << IS_Get (&Standard))) != 0) {
        return K->Tok;
    } else {
        return TOK_IDENT;
    }
}



static int SkipWhite (void)
/* Skip white space in the input stream, reading and preprocessing new lines
** if necessary. Return 0 if end of file is reached, return 1 otherwise.
*/
{
    while (1) {
        while (CurC == '\0') {
            /* If reading next line fails or is disabled with directives, bail
            ** out.
            */
            if (PPParserRunning || PreprocessNextLine () == 0) {
                return 0;
            }
        }
        if (IsSpace (CurC)) {
            NextChar ();
        } else {
            return 1;
        }
    }
}



int TokIsFuncSpec (const Token* T)
/* Return true if the token is a function specifier */
{
    return (T->Tok == TOK_INLINE)   ||
           (T->Tok == TOK_FASTCALL) || (T->Tok == TOK_CDECL) ||
           (T->Tok == TOK_NEAR)     || (T->Tok == TOK_FAR);
}



void SymName (char* S)
/* Read a symbol from the input stream. The first character must have been
** checked before calling this function. The buffer is expected to be at
** least of size MAX_IDENTLEN+1.
*/
{
    unsigned Len = 0;
    do {
        if (Len < MAX_IDENTLEN) {
            ++Len;
            *S++ = CurC;
        }
        NextChar ();
    } while (IsIdent (CurC) || IsDigit (CurC));
    *S = '\0';
}



int IsWideQuoted (char First, char Second)
/* Return 1 if the two successive characters indicate a wide string literal or
** a wide char constant, otherwise return 0.
*/
{
    return First == 'L' && IsQuote(Second);
}



int IsSym (char* S)
/* If a symbol follows, read it and return 1, otherwise return 0 */
{
    if (IsIdent (CurC) && !IsWideQuoted (CurC, NextC)) {
        SymName (S);
        return 1;
    } else {
        return 0;
    }
}



int IsPPNumber (int Cur, int Next)
/* Return 1 if the two successive characters indicate a pp-number, otherwise
** return 0.
*/
{
    return Cur != '.' ? IsDigit (Cur) : IsDigit (Next);
}



void CopyPPNumber (StrBuf* Target)
/* Copy a pp-number from the input to Target */
{
    int Std;

    if (!IsPPNumber (CurC, NextC)) {
        return;
    }

    /* P-exp is only valid in C99 and later */
    Std = IS_Get (&Standard);
    while (IsIdent (CurC) || IsDigit (CurC) || CurC == '.') {
        SB_AppendChar (Target, CurC);
        if (NextC == '+' || NextC == '-') {
            if (CurC == 'e' || CurC == 'E' ||
                (Std >= STD_C99 && (CurC == 'p' || CurC == 'P'))) {
                SB_AppendChar (Target, NextC);
                NextChar ();
            } else {
                NextChar ();
                break;
            }
        }
        NextChar ();
    }
}



static void UnknownChar (char C)
/* Error message for unknown character */
{
    Error ("Invalid input character with code %02X", C & 0xFF);
    NextChar ();                        /* Skip */
}



static void SetTok (int tok)
/* Set NextTok.Tok and bump line ptr */
{
    NextTok.Tok = tok;
    NextChar ();
}



static int ParseChar (void)
/* Parse a character. Converts escape chars into character codes. */
{
    int C;
    int HadError;
    int Count;

    /* Check for escape chars */
    if (CurC == '\\') {
        NextChar ();
        switch (CurC) {
            case '?':
                C = '\?';
                break;
            case 'a':
                C = '\a';
                break;
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
            case 'v':
                C = '\v';
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
                if (!IsXDigit (NextC)) {
                    Error ("\\x used with no following hex digits");
                    C = ' ';
                } else {
                    HadError = 0;
                    C = 0;
                    while (IsXDigit (NextC)) {
                        if ((C << 4) >= 256) {
                            if (!HadError) {
                                Error ("Hex character constant out of range");
                                HadError = 1;
                            }
                        } else {
                            C = (C << 4) | HexVal (NextC);
                        }
                        NextChar ();
                    }
                }
                break;
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
                /* Octal constant */
                Count = 1;
                C = HexVal (CurC);
                while (IsODigit (NextC) && Count++ < 3) {
                    C = (C << 3) | HexVal (NextC);
                    NextChar ();
                }
                if (C >= 256)
                    Error ("Octal character constant out of range");
                break;
            default:
                C = CurC;
                Error ("Illegal escaped character: 0x%02X", CurC);
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

    if (CurC == 'L') {
        /* Wide character constant */
        NextTok.Tok = TOK_WCCONST;
        NextChar ();
    } else {
        /* Narrow character constant */
        NextTok.Tok = TOK_CCONST;
    }

    /* Skip the quote */
    NextChar ();

    /* Get character */
    C = ParseChar ();

    /* Check for closing quote */
    if (CurC != '\'') {
        Error ("'\'' expected");
    } else {
        /* Skip the quote */
        NextChar ();
    }

    /* Translate into target charset */
    NextTok.IVal = SignExtendChar (TgtTranslateChar (C));

    /* Character constants have type int */
    NextTok.Type = type_int;
}



static void StringConst (void)
/* Parse a quoted string */
{
    /* String buffer */
    StrBuf S = AUTO_STRBUF_INITIALIZER;

    /* Assume next token is a string constant */
    NextTok.Tok  = TOK_SCONST;

    /* Concatenate strings. If at least one of the concenated strings is a wide
    ** character literal, the whole string is a wide char literal, otherwise
    ** it's a normal string literal.
    */
    while (1) {

        /* Check if this is a normal or a wide char string */
        if (CurC == 'L' && NextC == '\"') {
            /* Wide character literal */
            NextTok.Tok = TOK_WCSCONST;
            NextChar ();
            NextChar ();
        } else if (CurC == '\"') {
            /* Skip the quote char */
            NextChar ();
        } else {
            /* No string */
            break;
        }

        /* Read until end of string */
        while (CurC != '\"') {
            if (CurC == '\0') {
                Error ("Unexpected newline");
                break;
            }
            SB_AppendChar (&S, ParseChar ());
        }

        /* Skip closing quote char if there was one */
        NextChar ();

        /* Skip white space, read new input */
        SkipWhite ();

    }

    /* Terminate the string */
    SB_AppendChar (&S, '\0');

    /* Add the whole string to the literal pool */
    NextTok.SVal = AddLiteralStr (&S);

    /* Free the buffer */
    SB_Done (&S);
}



static void NumericConst (void)
/* Parse a numeric constant */
{
    unsigned Base;              /* Temporary number base according to prefix */
    unsigned Index;
    StrBuf   Src = AUTO_STRBUF_INITIALIZER;
    int      IsFloat;
    char     C;
    unsigned DigitVal;
    scan_t   IVal;              /* Scanned value. */
    int      Overflow;

    /* Get the pp-number first, then parse on it */
    CopyPPNumber (&Src);
    SB_Terminate (&Src);
    SB_Reset (&Src);

    /* Check for a leading hex, octal or binary prefix and determine the
    ** possible integer types.
    */
    if (SB_Peek (&Src) == '0') {
        /* Gobble 0 and examine next char */
        SB_Skip (&Src);
        if (toupper (SB_Peek (&Src)) == 'X' &&
            IsXDigit (SB_LookAt (&Src, SB_GetIndex (&Src) + 1))) {
            Base = 16;
            SB_Skip (&Src);     /* gobble "x" */
        } else if (toupper (SB_Peek (&Src)) == 'B'  &&
                   IS_Get (&Standard) >= STD_CC65   &&
                   IsDigit (SB_LookAt (&Src, SB_GetIndex (&Src) + 1))) {
            Base = 2;
            SB_Skip (&Src);     /* gobble 'b' */
        } else {
            Base = 10;          /* Assume 10 for now - see below */
        }
    } else {
        Base = 10;
    }

    /* Because floating point numbers don't have octal prefixes (a number with
    ** a leading zero is decimal), we first have to read the number before
    ** converting it, so we can determine if it's a float or an integer.
    */
    Index = SB_GetIndex (&Src);
    while ((C = SB_Peek (&Src)) != '\0' && (Base <= 10 ? IsDigit (C) : IsXDigit (C))) {
        SB_Skip (&Src);
    }

    /* The following character tells us if we have an integer or floating
    ** point constant. Note: Hexadecimal floating point constants aren't
    ** supported in C89.
    */
    IsFloat = (C == '.' ||
               (Base == 10 && toupper (C) == 'E') ||
               (Base == 16 && toupper (C) == 'P' && IS_Get (&Standard) >= STD_C99));

    /* An octal prefix for an integer type results in an octal base */
    if (!IsFloat && Base == 10 && SB_LookAt (&Src, 0) == '0') {
        Base = 8;
    }

    /* Since we now know the correct base, convert the input into a number */
    SB_SetIndex (&Src, Index);
    IVal = 0;
    Overflow = 0;
    while ((C = SB_Peek (&Src)) != '\0' && (Base <= 10 ? IsDigit (C) : IsXDigit (C))) {
        DigitVal = HexVal (C);
        if (DigitVal >= Base) {
            Error ("Invalid digit \"%c\" beyond radix %u constant", C, Base);
            SB_Clear (&Src);
            break;
        }
        /* Test result of adding digit for overflow. */
        if (((scan_t)(IVal * Base + DigitVal) / Base) != IVal) {
            Overflow = 1;
        }
        IVal = IVal * Base + DigitVal;
        SB_Skip (&Src);
    }
    if (Overflow) {
        Error ("Numerical constant \"%s\" too large for internal %d-bit representation",
               SB_GetConstBuf (&Src), (int)(sizeof(IVal)*CHAR_BIT));
    }

    /* Distinguish between integer and floating point constants */
    if (!IsFloat) {

        unsigned Types;
        unsigned WarnTypes = 0;

        /* Check for a suffix and determine the possible types. It is always
        ** possible to convert the data to unsigned long even if the IT_ULONG
        ** flag were not set, but we are not doing that.
        */
        if (toupper (SB_Peek (&Src)) == 'U') {
            /* Unsigned type */
            SB_Skip (&Src);
            if (toupper (SB_Peek (&Src)) != 'L') {
                Types = IT_UINT | IT_ULONG;
            } else {
                SB_Skip (&Src);
                Types = IT_ULONG;
            }
        } else if (toupper (SB_Peek (&Src)) == 'L') {
            /* Long type */
            SB_Skip (&Src);
            if (toupper (SB_Peek (&Src)) != 'U') {
                Types = IT_LONG | IT_ULONG;
                WarnTypes = IT_ULONG;
            } else {
                SB_Skip (&Src);
                Types = IT_ULONG;
            }
        } else {
            if (SB_Peek (&Src) != '\0') {
                Error ("Invalid suffix \"%s\" on integer constant",
                       SB_GetConstBuf (&Src) + SB_GetIndex (&Src));
            }

            if (Base == 10) {
                /* Decimal constants are of any type but uint */
                Types = IT_INT | IT_LONG | IT_ULONG;
                WarnTypes = IT_LONG | IT_ULONG;
            } else {
                /* Binary, octal and hex constants can be of any type */
                Types = IT_INT | IT_UINT | IT_LONG | IT_ULONG;
            }
        }

        /* Check the range to determine the type */
        if (IVal > 0x7FFF) {
            /* Out of range for int */
            Types &= ~IT_INT;
            /* If the value is in the range 0x8000..0xFFFF, unsigned int is not
            ** allowed, and we don't have a long type specifying suffix, emit a
            ** warning, because the constant is of type long while the user
            ** might expect an unsigned int.
            */
            if (IVal <= 0xFFFF              &&
                (Types & IT_UINT) == 0      &&
                (WarnTypes & IT_LONG) != 0) {
                Warning ("Integer constant implies signed long");
            }
        }
        if (IVal > 0xFFFF) {
            /* Out of range for unsigned int */
            Types &= ~IT_UINT;
        }
        if (IVal > 0x7FFFFFFF) {
            /* Out of range for long int */
            Types &= ~IT_LONG;
            /* If the value is in the range 0x80000000..0xFFFFFFFF, decimal,
            ** and we have no unsigned type specifying suffix, emit a warning,
            ** because the constant is of type unsigned long while the user
            ** might expect a signed integer constant, especially if there is
            ** a preceding unary op or when it is used in constant calculation.
            */
            if (WarnTypes & IT_ULONG) {
                Warning ("Integer constant implies unsigned long");
            }
        }

        /* Now set the type string to the smallest type in types */
        if (Types & IT_INT) {
            NextTok.Type = type_int;
        } else if (Types & IT_UINT) {
            NextTok.Type = type_uint;
        } else if (Types & IT_LONG) {
            NextTok.Type = type_long;
        } else {
            NextTok.Type = type_ulong;
        }

        /* Set the value and the token */
        NextTok.IVal = IVal;
        NextTok.Tok  = TOK_ICONST;

    } else {

        /* Float constant */
        Double FVal = FP_D_FromInt (IVal);      /* Convert to double */

        /* Check for a fractional part and read it */
        if (SB_Peek (&Src) == '.') {

            Double Scale;

            /* Skip the dot */
            SB_Skip (&Src);

            /* Read fractional digits */
            Scale  = FP_D_Make (1.0);
            while (IsXDigit (SB_Peek (&Src)) && (DigitVal = HexVal (SB_Peek (&Src))) < Base) {
                /* Get the value of this digit */
                Double FracVal = FP_D_Div (FP_D_FromInt (DigitVal * Base), Scale);
                /* Add it to the float value */
                FVal = FP_D_Add (FVal, FracVal);
                /* Scale base */
                Scale = FP_D_Mul (Scale, FP_D_FromInt (DigitVal));
                /* Skip the digit */
                SB_Skip (&Src);
            }
        }

        /* Check for an exponent and read it */
        if ((Base == 16 && toupper (SB_Peek (&Src)) == 'P') ||
            (Base == 10 && toupper (SB_Peek (&Src)) == 'E')) {

            unsigned Digits;
            unsigned Exp;

            /* Skip the exponent notifier */
            SB_Skip (&Src);

            /* Read an optional sign */
            if (SB_Peek (&Src) == '-') {
                SB_Skip (&Src);
            } else if (SB_Peek (&Src) == '+') {
                SB_Skip (&Src);
            }

            /* Read exponent digits. Since we support only 32 bit floats
            ** with a maximum exponent of +-/127, we read the exponent
            ** part as integer with up to 3 digits and drop the remainder.
            ** This avoids an overflow of Exp. The exponent is always
            ** decimal, even for hex float consts.
            */
            Digits = 0;
            Exp    = 0;
            while (IsDigit (SB_Peek (&Src))) {
                if (++Digits <= 3) {
                    Exp = Exp * 10 + HexVal (SB_Peek (&Src));
                }
                SB_Skip (&Src);
            }

            /* Check for errors: We must have exponent digits, and not more
            ** than three.
            */
            if (Digits == 0) {
                Error ("Floating constant exponent has no digits");
            } else if (Digits > 3) {
                Warning ("Floating constant exponent is too large");
            }

            /* Scale the exponent and adjust the value accordingly */
            if (Exp) {
                FVal = FP_D_Mul (FVal, FP_D_Make (pow (10, Exp)));
            }
        }

        /* Check for a suffix and determine the type of the constant */
        if (toupper (SB_Peek (&Src)) == 'F') {
            SB_Skip (&Src);
            NextTok.Type = type_float;
        } else {
            if (SB_Peek (&Src) != '\0') {
                Error ("Invalid suffix \"%s\" on floating constant",
                       SB_GetConstBuf (&Src) + SB_GetIndex (&Src));
            }
            NextTok.Type = type_double;
        }

        /* Set the value and the token */
        NextTok.FVal = FVal;
        NextTok.Tok  = TOK_FCONST;

    }

    /* We don't need the string buffer any longer */
    SB_Done (&Src);
}



void NextToken (void)
/* Get next token from input stream */
{
    ident token;

    /* We have to skip white space here before shifting tokens, since the
    ** tokens and the current line info is invalid at startup and will get
    ** initialized by reading the first time from the file. Remember if
    ** we were at end of input and handle that later.
    */
    int GotEOF = (SkipWhite() == 0);

    /* Current token is the lookahead token */
    if (CurTok.LI) {
        ReleaseLineInfo (CurTok.LI);
    }
    CurTok = NextTok;

    /* When reading the first time from the file, the line info in NextTok,
    ** which was copied to CurTok is invalid. Since the information from
    ** the token is used for error messages, we must make it valid.
    */
    if (CurTok.LI == 0) {
        CurTok.LI = UseLineInfo (GetCurLineInfo ());
    }

    /* Remember the starting position of the next token */
    NextTok.LI = UseLineInfo (GetCurLineInfo ());

    /* Now handle end of input. */
    if (GotEOF) {
        /* End of file reached */
        NextTok.Tok = TOK_CEOF;
        return;
    }

    /* Determine the next token from the lookahead */
    if (IsPPNumber (CurC, NextC)) {
        /* A number */
        NumericConst ();
        return;
    }

    /* Check for wide character constants and literals */
    if (CurC == 'L') {
        if (NextC == '\"') {
            StringConst ();
            return;
        } else if (NextC == '\'') {
            CharConst ();
            return;
        }
    }

    /* Check for keywords and identifiers */
    if (IsSym (token)) {

        if (!PPParserRunning) {
            /* Check for a keyword */
            if ((NextTok.Tok = FindKey (token)) != TOK_IDENT) {
                /* Reserved word found */
                return;
            }
        }

        /* No reserved word, check for special symbols */
        if (token[0] == '_' && token[1] == '_') {
            /* Special symbols */
            if (strcmp (token+2, "func__") == 0) {
                /* __func__ is only defined in functions */
                if (CurrentFunc) {
                    NextTok.SVal = AddLiteral (F_GetFuncName (CurrentFunc));
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
            NextChar ();
            if (CurC == '#') {
                SetTok (TOK_DOUBLE_HASH);
            } else {
                NextTok.Tok = TOK_HASH;
            }
            break;

        default:
            UnknownChar (CurC);

    }

}



void SkipTokens (const token_t* TokenList, unsigned TokenCount)
/* Skip tokens until we reach TOK_CEOF or a token in the given token list.
** This routine is used for error recovery.
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



int Consume (token_t Token, const char* ErrorMsg)
/* Eat token if it is the next in the input stream, otherwise print an error
** message. Returns true if the token was found and false otherwise.
*/
{
    if (CurTok.Tok == Token) {
        NextToken ();
        return 1;
    } else {
        Error ("%s", ErrorMsg);
        return 0;
    }
}



int ConsumeColon (void)
/* Check for a colon and skip it. */
{
    return Consume (TOK_COLON, "':' expected");
}



int ConsumeSemi (void)
/* Check for a semicolon and skip it. */
{
    /* Try do be smart about typos... */
    if (CurTok.Tok == TOK_SEMI) {
        NextToken ();
        return 1;
    } else {
        Error ("';' expected");
        if (CurTok.Tok == TOK_COLON || CurTok.Tok == TOK_COMMA) {
            NextToken ();
        }
        return 0;
    }
}



int ConsumeComma (void)
/* Check for a comma and skip it. */
{
    /* Try do be smart about typos... */
    if (CurTok.Tok == TOK_COMMA) {
        NextToken ();
        return 1;
    } else {
        Error ("',' expected");
        if (CurTok.Tok == TOK_SEMI) {
            NextToken ();
        }
        return 0;
    }
}



int ConsumeLParen (void)
/* Check for a left parenthesis and skip it */
{
    return Consume (TOK_LPAREN, "'(' expected");
}



int ConsumeRParen (void)
/* Check for a right parenthesis and skip it */
{
    return Consume (TOK_RPAREN, "')' expected");
}



int ConsumeLBrack (void)
/* Check for a left bracket and skip it */
{
    return Consume (TOK_LBRACK, "'[' expected");
}



int ConsumeRBrack (void)
/* Check for a right bracket and skip it */
{
    return Consume (TOK_RBRACK, "']' expected");
}



int ConsumeLCurly (void)
/* Check for a left curly brace and skip it */
{
    return Consume (TOK_LCURLY, "'{' expected");
}



int ConsumeRCurly (void)
/* Check for a right curly brace and skip it */
{
    return Consume (TOK_RCURLY, "'}' expected");
}
