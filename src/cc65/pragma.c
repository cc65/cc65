/*****************************************************************************/
/*                                                                           */
/*                                 pragma.c                                  */
/*                                                                           */
/*                  Pragma handling for the cc65 C compiler                  */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2011, Ullrich von Bassewitz                                      */
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



#include <stdlib.h>
#include <string.h>

/* common */
#include "chartype.h"
#include "segnames.h"
#include "tgttrans.h"

/* cc65 */
#include "codegen.h"
#include "error.h"
#include "expr.h"
#include "global.h"
#include "litpool.h"
#include "scanner.h"
#include "scanstrbuf.h"
#include "symtab.h"
#include "pragma.h"



/*****************************************************************************/
/*                                   data                                    */
/*****************************************************************************/



/* Tokens for the #pragmas */
typedef enum {
    PRAGMA_ILLEGAL = -1,
    PRAGMA_ALIGN,
    PRAGMA_BSS_NAME,
    PRAGMA_BSSSEG,                                      /* obsolete */
    PRAGMA_CHARMAP,
    PRAGMA_CHECK_STACK,
    PRAGMA_CHECKSTACK,                                  /* obsolete */
    PRAGMA_CODE_NAME,
    PRAGMA_CODESEG,                                     /* obsolete */
    PRAGMA_CODESIZE,
    PRAGMA_DATA_NAME,
    PRAGMA_DATASEG,                                     /* obsolete */
    PRAGMA_LOCAL_STRINGS,
    PRAGMA_OPTIMIZE,
    PRAGMA_REGVARADDR,
    PRAGMA_REGISTER_VARS,
    PRAGMA_REGVARS,                                     /* obsolete */
    PRAGMA_RODATA_NAME,
    PRAGMA_RODATASEG,                                   /* obsolete */
    PRAGMA_SIGNED_CHARS,
    PRAGMA_SIGNEDCHARS,                                 /* obsolete */
    PRAGMA_STATIC_LOCALS,
    PRAGMA_STATICLOCALS,                                /* obsolete */
    PRAGMA_WARN,
    PRAGMA_WRITABLE_STRINGS,
    PRAGMA_ZPSYM,
    PRAGMA_COUNT
} pragma_t;

/* Pragma table */
static const struct Pragma {
    const char* Key;            /* Keyword */
    pragma_t    Tok;            /* Token */
} Pragmas[PRAGMA_COUNT] = {
    { "align",                  PRAGMA_ALIGN            },
    { "bss-name",               PRAGMA_BSS_NAME         },
    { "bssseg",                 PRAGMA_BSSSEG           },      /* obsolete */
    { "charmap",                PRAGMA_CHARMAP          },
    { "check-stack",            PRAGMA_CHECK_STACK      },
    { "checkstack",             PRAGMA_CHECKSTACK       },      /* obsolete */
    { "code-name",              PRAGMA_CODE_NAME        },
    { "codeseg",                PRAGMA_CODESEG          },      /* obsolete */
    { "codesize",               PRAGMA_CODESIZE         },
    { "data-name",              PRAGMA_DATA_NAME        },
    { "dataseg",                PRAGMA_DATASEG          },      /* obsolete */
    { "local-strings",          PRAGMA_LOCAL_STRINGS    },
    { "optimize",               PRAGMA_OPTIMIZE         },
    { "register-vars",          PRAGMA_REGISTER_VARS    },
    { "regvaraddr",             PRAGMA_REGVARADDR       },
    { "regvars",                PRAGMA_REGVARS          },      /* obsolete */
    { "rodata-name",            PRAGMA_RODATA_NAME      },
    { "rodataseg",              PRAGMA_RODATASEG        },      /* obsolete */
    { "signed-chars",           PRAGMA_SIGNED_CHARS     },
    { "signedchars",            PRAGMA_SIGNEDCHARS      },      /* obsolete */
    { "static-locals",          PRAGMA_STATIC_LOCALS    },
    { "staticlocals",           PRAGMA_STATICLOCALS     },      /* obsolete */
    { "warn",                   PRAGMA_WARN             },
    { "writable-strings",       PRAGMA_WRITABLE_STRINGS },
    { "zpsym",                  PRAGMA_ZPSYM            },
};

/* Result of ParsePushPop */
typedef enum {
    PP_NONE,
    PP_POP,
    PP_PUSH,
    PP_ERROR,
} PushPopResult;



/*****************************************************************************/
/*                             Helper functions                              */
/*****************************************************************************/



static void PragmaErrorSkip (void)
/* Called in case of an error, skips tokens until the closing paren or a
** semicolon is reached.
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



static pragma_t FindPragma (const StrBuf* Key)
/* Find a pragma and return the token. Return PRAGMA_ILLEGAL if the keyword is
** not a valid pragma.
*/
{
    struct Pragma* P;
    P = bsearch (SB_GetConstBuf (Key), Pragmas, PRAGMA_COUNT, sizeof (Pragmas[0]), CmpKey);
    return P? P->Tok : PRAGMA_ILLEGAL;
}



static int GetComma (StrBuf* B)
/* Expects and skips a comma in B. Prints an error and returns zero if no
** comma is found. Return a value <> 0 otherwise.
*/
{
    SB_SkipWhite (B);
    if (SB_Get (B) != ',') {
        Error ("Comma expected");
        return 0;
    }
    SB_SkipWhite (B);
    return 1;
}



static int GetString (StrBuf* B, StrBuf* S)
/* Expects and skips a string in B. Prints an error and returns zero if no
** string is found. Returns a value <> 0 otherwise.
*/
{
    if (!SB_GetString (B, S)) {
        Error ("String literal expected");
        return 0;
    }
    return 1;
}



static int GetNumber (StrBuf* B, long* Val)
/* Expects and skips a number in B. Prints an eror and returns zero if no
** number is found. Returns a value <> 0 otherwise.
*/
{
    if (!SB_GetNumber (B, Val)) {
        Error ("Constant integer expected");
        return 0;
    }
    return 1;
}



static IntStack* GetWarning (StrBuf* B)
/* Get a warning name from the string buffer. Returns a pointer to the intstack
** that holds the state of the warning, and NULL in case of errors. The
** function will output error messages in case of problems.
*/
{
    IntStack* S = 0;
    StrBuf W = AUTO_STRBUF_INITIALIZER;

    /* The warning name is a symbol but the '-' char is allowed within */
    if (SB_GetSym (B, &W, "-")) {

        /* Map the warning name to an IntStack that contains its state */
        S = FindWarning (SB_GetConstBuf (&W));

        /* Handle errors */
        if (S == 0) {
            Error ("Pragma expects a warning name as first argument");
        }
    }

    /* Deallocate the string */
    SB_Done (&W);

    /* Done */
    return S;
}



static int HasStr (StrBuf* B, const char* E)
/* Checks if E follows in B. If so, skips it and returns true */
{
    unsigned Len = strlen (E);
    if (SB_GetLen (B) - SB_GetIndex (B) >= Len) {
        if (strncmp (SB_GetConstBuf (B) + SB_GetIndex (B), E, Len) == 0) {
            /* Found */
            SB_SkipMultiple (B, Len);
            return 1;
        }
    }
    return 0;
}



static PushPopResult ParsePushPop (StrBuf* B)
/* Check for and parse the "push" and "pop" keywords. In case of "push", a
** following comma is expected and skipped.
*/
{
    StrBuf Ident      = AUTO_STRBUF_INITIALIZER;
    PushPopResult Res = PP_NONE;

    /* Remember the current string index, so we can go back in case of errors */
    unsigned Index = SB_GetIndex (B);

    /* Try to read an identifier */
    if (SB_GetSym (B, &Ident, 0)) {

        /* Check if we have a first argument named "pop" */
        if (SB_CompareStr (&Ident, "pop") == 0) {

            Res = PP_POP;

        /* Check if we have a first argument named "push" */
        } else if (SB_CompareStr (&Ident, "push") == 0) {

            Res = PP_PUSH;

            /* Skip the following comma */
            if (!GetComma (B)) {
                /* Error already flagged by GetComma */
                Res = PP_ERROR;
            }

        } else {

            /* Unknown keyword, roll back */
            SB_SetIndex (B, Index);
        }
    }

    /* Free the string buffer and return the result */
    SB_Done (&Ident);
    return Res;
}



static void PopInt (IntStack* S)
/* Pops an integer from an IntStack. Prints an error if the stack is empty */
{
    if (IS_GetCount (S) < 2) {
        Error ("Cannot pop, stack is empty");
    } else {
        IS_Drop (S);
    }
}



static void PushInt (IntStack* S, long Val)
/* Pushes an integer onto an IntStack. Prints an error if the stack is full */
{
    if (IS_IsFull (S)) {
        Error ("Cannot push: stack overflow");
    } else {
        IS_Push (S, Val);
    }
}



static int BoolKeyword (StrBuf* Ident)
/* Check if the identifier in Ident is a keyword for a boolean value. Currently
** accepted are true/false/on/off.
*/
{
    if (SB_CompareStr (Ident, "true") == 0) {
        return 1;
    }
    if (SB_CompareStr (Ident, "on") == 0) {
        return 1;
    }
    if (SB_CompareStr (Ident, "false") == 0) {
        return 0;
    }
    if (SB_CompareStr (Ident, "off") == 0) {
        return 0;
    }

    /* Error */
    Error ("Pragma argument must be one of `on', `off', `true' or `false'");
    return 0;
}



/*****************************************************************************/
/*                         Pragma handling functions                         */
/*****************************************************************************/



static void StringPragma (StrBuf* B, void (*Func) (const char*))
/* Handle a pragma that expects a string parameter */
{
    StrBuf S = AUTO_STRBUF_INITIALIZER;

    /* We expect a string here */
    if (GetString (B, &S)) {
        /* Call the given function with the string argument */
        Func (SB_GetConstBuf (&S));
    }

    /* Call the string buf destructor */
    SB_Done (&S);
}



static void SegNamePragma (StrBuf* B, segment_t Seg)
/* Handle a pragma that expects a segment name parameter */
{
    StrBuf      S = AUTO_STRBUF_INITIALIZER;
    const char* Name;

    /* Check for the "push" or "pop" keywords */
    int Push = 0;
    switch (ParsePushPop (B)) {

        case PP_NONE:
            break;

        case PP_PUSH:
            Push = 1;
            break;

        case PP_POP:
            /* Pop the old value and output it */
            PopSegName (Seg);
            g_segname (Seg);

            /* Done */
            goto ExitPoint;

        case PP_ERROR:
            /* Bail out */
            goto ExitPoint;

        default:
            Internal ("Invalid result from ParsePushPop");

    }

    /* A string argument must follow */
    if (!GetString (B, &S)) {
        goto ExitPoint;
    }

    /* Get the string */
    Name = SB_GetConstBuf (&S);

    /* Check if the name is valid */
    if (ValidSegName (Name)) {

        /* Set the new name */
        if (Push) {
            PushSegName (Seg, Name);
        } else {
            SetSegName (Seg, Name);
        }
        g_segname (Seg);

    } else {

        /* Segment name is invalid */
        Error ("Illegal segment name: `%s'", Name);

    }

ExitPoint:
    /* Call the string buf destructor */
    SB_Done (&S);
}



static void CharMapPragma (StrBuf* B)
/* Change the character map */
{
    long Index, C;

    /* Read the character index */
    if (!GetNumber (B, &Index)) {
        return;
    }
    if (Index < 1 || Index > 255) {
        if (Index == 0) {
            /* For groepaz */
            Error ("Remapping 0 is not allowed");
        } else {
            Error ("Character index out of range");
        }
        return;
    }

    /* Comma follows */
    if (!GetComma (B)) {
        return;
    }

    /* Read the character code */
    if (!GetNumber (B, &C)) {
        return;
    }
    if (C < 1 || C > 255) {
        if (C == 0) {
            /* For groepaz */
            Error ("Remapping 0 is not allowed");
        } else {
            Error ("Character code out of range");
        }
        return;
    }

    /* Remap the character */
    TgtTranslateSet ((unsigned) Index, (unsigned char) C);
}



static void WarnPragma (StrBuf* B)
/* Enable/disable warnings */
{
    long   Val;
    int    Push;

    /* A warning name must follow */
    IntStack* S = GetWarning (B);
    if (S == 0) {
        return;
    }

    /* Comma follows */
    if (!GetComma (B)) {
        return;
    }

    /* Check for the "push" or "pop" keywords */
    switch (ParsePushPop (B)) {

        case PP_NONE:
            Push = 0;
            break;

        case PP_PUSH:
            Push = 1;
            break;

        case PP_POP:
            /* Pop the old value and bail out */
            PopInt (S);
            return;

        case PP_ERROR:
            /* Bail out */
            return;

        default:
            Internal ("Invalid result from ParsePushPop");
    }

    /* Boolean argument follows */
    if (HasStr (B, "true") || HasStr (B, "on")) {
        Val = 1;
    } else if (HasStr (B, "false") || HasStr (B, "off")) {
        Val = 0;
    } else if (!SB_GetNumber (B, &Val)) {
        Error ("Invalid pragma argument");
        return;
    }

    /* Set/push the new value */
    if (Push) {
        PushInt (S, Val);
    } else {
        IS_Set (S, Val);
    }
}



static void FlagPragma (StrBuf* B, IntStack* Stack)
/* Handle a pragma that expects a boolean paramater */
{
    StrBuf Ident = AUTO_STRBUF_INITIALIZER;
    long   Val;
    int    Push;


    /* Try to read an identifier */
    int IsIdent = SB_GetSym (B, &Ident, 0);

    /* Check if we have a first argument named "pop" */
    if (IsIdent && SB_CompareStr (&Ident, "pop") == 0) {
        PopInt (Stack);
        /* No other arguments allowed */
        return;
    }

    /* Check if we have a first argument named "push" */
    if (IsIdent && SB_CompareStr (&Ident, "push") == 0) {
        Push = 1;
        if (!GetComma (B)) {
            goto ExitPoint;
        }
        IsIdent = SB_GetSym (B, &Ident, 0);
    } else {
        Push = 0;
    }

    /* Boolean argument follows */
    if (IsIdent) {
        Val = BoolKeyword (&Ident);
    } else if (!GetNumber (B, &Val)) {
        goto ExitPoint;
    }

    /* Set/push the new value */
    if (Push) {
        PushInt (Stack, Val);
    } else {
        IS_Set (Stack, Val);
    }

ExitPoint:
    /* Free the identifier */
    SB_Done (&Ident);
}



static void IntPragma (StrBuf* B, IntStack* Stack, long Low, long High)
/* Handle a pragma that expects an int paramater */
{
    long  Val;
    int   Push;

    /* Check for the "push" or "pop" keywords */
    switch (ParsePushPop (B)) {

        case PP_NONE:
            Push = 0;
            break;

        case PP_PUSH:
            Push = 1;
            break;

        case PP_POP:
            /* Pop the old value and bail out */
            PopInt (Stack);
            return;

        case PP_ERROR:
            /* Bail out */
            return;

        default:
            Internal ("Invalid result from ParsePushPop");

    }

    /* Integer argument follows */
    if (!GetNumber (B, &Val)) {
        return;
    }

    /* Check the argument */
    if (Val < Low || Val > High) {
        Error ("Pragma argument out of bounds (%ld-%ld)", Low, High);
        return;
    }

    /* Set/push the new value */
    if (Push) {
        PushInt (Stack, Val);
    } else {
        IS_Set (Stack, Val);
    }
}



static void ParsePragma (void)
/* Parse the contents of the _Pragma statement */
{
    pragma_t Pragma;
    StrBuf   Ident = AUTO_STRBUF_INITIALIZER;

    /* Create a string buffer from the string literal */
    StrBuf B = AUTO_STRBUF_INITIALIZER;
    SB_Append (&B, GetLiteralStrBuf (CurTok.SVal));

    /* Skip the string token */
    NextToken ();

    /* Get the pragma name from the string */
    SB_SkipWhite (&B);
    if (!SB_GetSym (&B, &Ident, "-")) {
        Error ("Invalid pragma");
        goto ExitPoint;
    }

    /* Search for the name */
    Pragma = FindPragma (&Ident);

    /* Do we know this pragma? */
    if (Pragma == PRAGMA_ILLEGAL) {
        /* According to the ANSI standard, we're not allowed to generate errors
        ** for unknown pragmas, but warn about them if enabled (the default).
        */
        if (IS_Get (&WarnUnknownPragma)) {
            Warning ("Unknown pragma `%s'", SB_GetConstBuf (&Ident));
        }
        goto ExitPoint;
    }

    /* Check for an open paren */
    SB_SkipWhite (&B);
    if (SB_Get (&B) != '(') {
        Error ("'(' expected");
        goto ExitPoint;
    }

    /* Skip white space before the argument */
    SB_SkipWhite (&B);

    /* Switch for the different pragmas */
    switch (Pragma) {

        case PRAGMA_ALIGN:
            IntPragma (&B, &DataAlignment, 1, 4096);
            break;

        case PRAGMA_BSSSEG:
            Warning ("#pragma bssseg is obsolete, please use #pragma bss-name instead");
            /* FALLTHROUGH */
        case PRAGMA_BSS_NAME:
            SegNamePragma (&B, SEG_BSS);
            break;

        case PRAGMA_CHARMAP:
            CharMapPragma (&B);
            break;

        case PRAGMA_CHECKSTACK:
            Warning ("#pragma checkstack is obsolete, please use #pragma check-stack instead");
            /* FALLTHROUGH */
        case PRAGMA_CHECK_STACK:
            FlagPragma (&B, &CheckStack);
            break;

        case PRAGMA_CODESEG:
            Warning ("#pragma codeseg is obsolete, please use #pragma code-name instead");
            /* FALLTHROUGH */
        case PRAGMA_CODE_NAME:
            SegNamePragma (&B, SEG_CODE);
            break;

        case PRAGMA_CODESIZE:
            IntPragma (&B, &CodeSizeFactor, 10, 1000);
            break;

        case PRAGMA_DATASEG:
            Warning ("#pragma dataseg is obsolete, please use #pragma data-name instead");
            /* FALLTHROUGH */
        case PRAGMA_DATA_NAME:
            SegNamePragma (&B, SEG_DATA);
            break;

        case PRAGMA_LOCAL_STRINGS:
            FlagPragma (&B, &LocalStrings);
            break;

        case PRAGMA_OPTIMIZE:
            FlagPragma (&B, &Optimize);
            break;

        case PRAGMA_REGVARADDR:
            FlagPragma (&B, &AllowRegVarAddr);
            break;

        case PRAGMA_REGVARS:
            Warning ("#pragma regvars is obsolete, please use #pragma register-vars instead");
            /* FALLTHROUGH */
        case PRAGMA_REGISTER_VARS:
            FlagPragma (&B, &EnableRegVars);
            break;

        case PRAGMA_RODATASEG:
            Warning ("#pragma rodataseg is obsolete, please use #pragma rodata-name instead");
            /* FALLTHROUGH */
        case PRAGMA_RODATA_NAME:
            SegNamePragma (&B, SEG_RODATA);
            break;

        case PRAGMA_SIGNEDCHARS:
            Warning ("#pragma signedchars is obsolete, please use #pragma signed-chars instead");
            /* FALLTHROUGH */
        case PRAGMA_SIGNED_CHARS:
            FlagPragma (&B, &SignedChars);
            break;

        case PRAGMA_STATICLOCALS:
            Warning ("#pragma staticlocals is obsolete, please use #pragma static-locals instead");
            /* FALLTHROUGH */
        case PRAGMA_STATIC_LOCALS:
            FlagPragma (&B, &StaticLocals);
            break;

        case PRAGMA_WARN:
            WarnPragma (&B);
            break;

        case PRAGMA_WRITABLE_STRINGS:
            FlagPragma (&B, &WritableStrings);
            break;

        case PRAGMA_ZPSYM:
            StringPragma (&B, MakeZPSym);
            break;

        default:
            Internal ("Invalid pragma");
    }

    /* Closing paren expected */
    SB_SkipWhite (&B);
    if (SB_Get (&B) != ')') {
        Error ("')' expected");
        goto ExitPoint;
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

ExitPoint:
    /* Release the string buffers */
    SB_Done (&B);
    SB_Done (&Ident);
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
        ** enclosing paren, or a semicolon.
        */
        PragmaErrorSkip ();

    } else {

        /* Parse the _Pragma statement */
        ParsePragma ();
    }

    /* Closing paren needed */
    ConsumeRParen ();
}
