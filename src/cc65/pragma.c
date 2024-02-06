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
#include "addrsize.h"
#include "chartype.h"
#include "segnames.h"
#include "tgttrans.h"
#include "xmalloc.h"

/* cc65 */
#include "codegen.h"
#include "error.h"
#include "global.h"
#include "litpool.h"
#include "scanner.h"
#include "scanstrbuf.h"
#include "symtab.h"
#include "pragma.h"
#include "wrappedcall.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Tokens for the #pragmas */
typedef enum {
    PRAGMA_ILLEGAL = -1,
    PRAGMA_ALIGN,
    PRAGMA_ALLOW_EAGER_INLINE,
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
    PRAGMA_INLINE_STDFUNCS,
    PRAGMA_LOCAL_STRINGS,
    PRAGMA_MESSAGE,
    PRAGMA_OPTIMIZE,
    PRAGMA_REGISTER_VARS,
    PRAGMA_REGVARADDR,
    PRAGMA_REGVARS,                                     /* obsolete */
    PRAGMA_RODATA_NAME,
    PRAGMA_RODATASEG,                                   /* obsolete */
    PRAGMA_SIGNED_CHARS,
    PRAGMA_SIGNEDCHARS,                                 /* obsolete */
    PRAGMA_STATIC_LOCALS,
    PRAGMA_STATICLOCALS,                                /* obsolete */
    PRAGMA_WARN,
    PRAGMA_WRAPPED_CALL,
    PRAGMA_WRITABLE_STRINGS,
    PRAGMA_ZPSYM,
    PRAGMA_COUNT
} pragma_t;

/* Pragma table */
static const struct Pragma {
    const char* Key;            /* Keyword */
    pragma_t    Tok;            /* Token */
} Pragmas[PRAGMA_COUNT] = {
    { "align",                  PRAGMA_ALIGN              },
    { "allow-eager-inline",     PRAGMA_ALLOW_EAGER_INLINE },
    { "bss-name",               PRAGMA_BSS_NAME           },
    { "bssseg",                 PRAGMA_BSSSEG             },      /* obsolete */
    { "charmap",                PRAGMA_CHARMAP            },
    { "check-stack",            PRAGMA_CHECK_STACK        },
    { "checkstack",             PRAGMA_CHECKSTACK         },      /* obsolete */
    { "code-name",              PRAGMA_CODE_NAME          },
    { "codeseg",                PRAGMA_CODESEG            },      /* obsolete */
    { "codesize",               PRAGMA_CODESIZE           },
    { "data-name",              PRAGMA_DATA_NAME          },
    { "dataseg",                PRAGMA_DATASEG            },      /* obsolete */
    { "inline-stdfuncs",        PRAGMA_INLINE_STDFUNCS    },
    { "local-strings",          PRAGMA_LOCAL_STRINGS      },
    { "message",                PRAGMA_MESSAGE            },
    { "optimize",               PRAGMA_OPTIMIZE           },
    { "register-vars",          PRAGMA_REGISTER_VARS      },
    { "regvaraddr",             PRAGMA_REGVARADDR         },
    { "regvars",                PRAGMA_REGVARS            },      /* obsolete */
    { "rodata-name",            PRAGMA_RODATA_NAME        },
    { "rodataseg",              PRAGMA_RODATASEG          },      /* obsolete */
    { "signed-chars",           PRAGMA_SIGNED_CHARS       },
    { "signedchars",            PRAGMA_SIGNEDCHARS        },      /* obsolete */
    { "static-locals",          PRAGMA_STATIC_LOCALS      },
    { "staticlocals",           PRAGMA_STATICLOCALS       },      /* obsolete */
    { "warn",                   PRAGMA_WARN               },
    { "wrapped-call",           PRAGMA_WRAPPED_CALL       },
    { "writable-strings",       PRAGMA_WRITABLE_STRINGS   },
    { "zpsym",                  PRAGMA_ZPSYM              },
};

/* Result of ParsePushPop */
typedef enum {
    PP_NONE,
    PP_POP,
    PP_PUSH,
    PP_ERROR,
} PushPopResult;

/* Effective scope of the pragma.
** This talks about how far the pragma has effects on whenever it shows up,
** even in the middle of an expression, statement or something.
*/
typedef enum {
    PES_NONE,
    PES_IMM,                    /* No way back */
    PES_EXPR,                   /* Current expression/declarator */
    PES_STMT,                   /* Current statement/declaration */
    PES_SCOPE,                  /* Current scope */
    PES_FUNC,                   /* Current function */
    PES_FILE,                   /* Current file */
    PES_ALL,                    /* All */
} pragma_scope_t;



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



static int IsBoolKeyword (StrBuf* Ident)
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
    Error ("Pragma argument must be one of 'on', 'off', 'true' or 'false'");
    return 0;
}



static void ApplyPragma (int PushPop, IntStack* Stack, long Val)
/* Apply a pragma immediately */
{
    if (PushPop > 0) {
        /* Push the new value */
        PushInt (Stack, Val);
    } else if (PushPop < 0) {
        /* Pop the old value */
        PopInt (Stack);
    } else {
        /* Set the new value */
        IS_Set (Stack, Val);
    }
}



static void ApplySegNamePragma (pragma_t Token, int PushPop, const char* Name, unsigned char AddrSize)
/* Process a segname pragma */
{
    segment_t Seg = SEG_CODE;

    switch (Token) {
        case PRAGMA_CODE_NAME:
        case PRAGMA_CODESEG:
            Seg = SEG_CODE;
            break;

        case PRAGMA_RODATA_NAME:
        case PRAGMA_RODATASEG:
            Seg = SEG_RODATA;
            break;

        case PRAGMA_DATA_NAME:
        case PRAGMA_DATASEG:
            Seg = SEG_DATA;
            break;

        case PRAGMA_BSS_NAME:
        case PRAGMA_BSSSEG:
            Seg = SEG_BSS;
            break;

        default:
            Internal ("Unknown segment name pragma: %02X", Token);
            break;
    }

    /* Set the new name */
    if (PushPop > 0) {
        PushSegName (Seg, Name);
    } else if (PushPop < 0) {
        PopSegName (Seg);
    } else {
        SetSegName (Seg, Name);
    }

    /* Set the optional address size for the segment if valid */
    if (PushPop >= 0 && AddrSize != ADDR_SIZE_INVALID) {
        SetSegAddrSize (Name, AddrSize);
    }

    /* BSS variables are output at the end of the compilation.  Don't
    ** bother to change their segment, now.
    */
    if (Seg != SEG_BSS) {
        g_segname (Seg);
    }
}



/*****************************************************************************/
/*                         Pragma handling functions                         */
/*****************************************************************************/



static void StringPragma (pragma_scope_t Scope, StrBuf* B, void (*Func) (const char*))
/* Handle a pragma that expects a string parameter */
{
    StrBuf S = AUTO_STRBUF_INITIALIZER;

    /* Only PES_IMM is supported */
    CHECK (Scope == PES_IMM);

    /* We expect a string here */
    if (GetString (B, &S)) {
        /* Call the given function with the string argument */
        Func (SB_GetConstBuf (&S));
    }

    /* Call the string buf destructor */
    SB_Done (&S);
}



static void SegNamePragma (pragma_scope_t Scope, pragma_t Token, StrBuf* B)
/* Handle a pragma that expects a segment name parameter */
{
    const char* Name;
    unsigned char AddrSize = ADDR_SIZE_INVALID;
    StrBuf S = AUTO_STRBUF_INITIALIZER;
    StrBuf A = AUTO_STRBUF_INITIALIZER;
    int PushPop = 0;

    /* Unused at the moment */
    (void)Scope;

    /* Check for the "push" or "pop" keywords */
    switch (ParsePushPop (B)) {

        case PP_NONE:
            break;

        case PP_PUSH:
            PushPop = 1;
            break;

        case PP_POP:
            /* Pop the old value and output it */
            ApplySegNamePragma (Token, -1, 0, 0);

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

    /* Get the name string of the segment */
    Name = SB_GetConstBuf (&S);

    /* Check if the name is valid */
    if (ValidSegName (Name)) {

        /* Skip the following comma */
        SB_SkipWhite (B);
        if (SB_Peek (B) == ',') {
            SB_Skip (B);
            SB_SkipWhite (B);

            /* A string argument must follow */
            if (!GetString (B, &A)) {
                goto ExitPoint;
            }

            /* Get the address size for the segment */
            AddrSize = AddrSizeFromStr (SB_GetConstBuf (&A));

            /* Check the address size for the segment */
            if (AddrSize == ADDR_SIZE_INVALID) {
                Warning ("Invalid address size for segment");
            }
        }

        /* Set the new name and optionally address size */
        ApplySegNamePragma (Token, PushPop, Name, AddrSize);

    } else {

        /* Segment name is invalid */
        Error ("Illegal segment name: '%s'", Name);

    }

ExitPoint:

    /* Call the string buf destructor */
    SB_Done (&S);
    SB_Done (&A);
}



static void WrappedCallPragma (pragma_scope_t Scope, StrBuf* B)
/* Handle the wrapped-call pragma */
{
    StrBuf      S = AUTO_STRBUF_INITIALIZER;
    const char *Name;
    long Val;
    SymEntry *Entry;

    /* Only PES_IMM is supported */
    CHECK (Scope == PES_IMM);

    /* Check for the "push" or "pop" keywords */
    switch (ParsePushPop (B)) {

        case PP_NONE:
            Error ("Push or pop required");
            break;

        case PP_PUSH:
            break;

        case PP_POP:
            PopWrappedCall();

            /* Done */
            goto ExitPoint;

        case PP_ERROR:
            /* Bail out */
            goto ExitPoint;

        default:
            Internal ("Invalid result from ParsePushPop");

    }

    /* A symbol argument must follow */
    if (!SB_GetSym (B, &S, NULL)) {
        goto ExitPoint;
    }

    /* Skip the following comma */
    if (!GetComma (B)) {
        /* Error already flagged by GetComma */
        Error ("Value or the word 'bank' required for wrapped-call identifier");
        goto ExitPoint;
    }

    /* Next must be either a numeric value, or "bank" */
    if (HasStr (B, "bank")) {
        Val = WRAPPED_CALL_USE_BANK;
    } else if (!GetNumber (B, &Val)) {
        Error ("Value required for wrapped-call identifier");
        goto ExitPoint;
    }

    if (!(Val == WRAPPED_CALL_USE_BANK) && (Val < 0 || Val > 255)) {
        Error ("Identifier must be between 0-255");
        goto ExitPoint;
    }

    /* Get the string */
    Name = SB_GetConstBuf (&S);
    Entry = FindSym(Name);

    /* Check if the name is valid */
    if (Entry && (Entry->Flags & SC_TYPEMASK) == SC_FUNC) {

        PushWrappedCall(Entry, (unsigned int) Val);
        Entry->Flags |= SC_REF;
        GetFuncDesc (Entry->Type)->Flags |= FD_CALL_WRAPPER;

    } else {

        /* Segment name is invalid */
        Error ("Wrapped-call target does not exist or is not a function");

    }

ExitPoint:
    /* Call the string buf destructor */
    SB_Done (&S);
}



static void CharMapPragma (pragma_scope_t Scope, StrBuf* B)
/* Change the character map */
{
    long Index, C;

    /* Only PES_IMM is supported */
    CHECK (Scope == PES_IMM);

    /* Read the character index */
    if (!GetNumber (B, &Index)) {
        return;
    }
    if (Index < 0 || Index > 255) {
        Error ("Character index out of range");
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
    if (C < 0 || C > 255) {
        Error ("Character code out of range");
        return;
    }

    /* Warn about remapping character code 0x00
    ** (except when remapping it back to itself).
    */
    if (Index + C != 0 && IS_Get (&WarnRemapZero)) {
        if (Index == 0) {
            Warning ("Remapping from 0 is dangerous with string functions");
        }
        else if (C == 0) {
            Warning ("Remapping to 0 can make string functions stop unexpectedly");
        }
    }

    /* Remap the character */
    TgtTranslateSet ((unsigned) Index, (unsigned char) C);
}



static void WarnPragma (pragma_scope_t Scope, StrBuf* B)
/* Enable/disable warnings */
{
    long   Val;
    int    Push;

    /* A warning name must follow */
    IntStack* S = GetWarning (B);

    /* Only PES_IMM is supported */
    CHECK (Scope == PES_IMM);

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



static void FlagPragma (pragma_scope_t Scope, pragma_t Token, StrBuf* B, IntStack* Stack)
/* Handle a pragma that expects a boolean parameter */
{
    StrBuf Ident = AUTO_STRBUF_INITIALIZER;
    long   Val;
    int    PushPop = 0;

    /* Unused at the moment */
    (void)Scope;
    (void)Token;

    /* Try to read an identifier */
    int IsIdent = SB_GetSym (B, &Ident, 0);

    /* Check if we have a first argument named "pop" */
    if (IsIdent && SB_CompareStr (&Ident, "pop") == 0) {
        /* Pop the old value and bail out */
        ApplyPragma (-1, Stack, 0);

        /* No other arguments allowed */
        return;
    }

    /* Check if we have a first argument named "push" */
    if (IsIdent && SB_CompareStr (&Ident, "push") == 0) {
        PushPop = 1;
        if (!GetComma (B)) {
            goto ExitPoint;
        }
        IsIdent = SB_GetSym (B, &Ident, 0);
    }

    /* Boolean argument follows */
    if (IsIdent) {
        Val = IsBoolKeyword (&Ident);
    } else if (!GetNumber (B, &Val)) {
        goto ExitPoint;
    }

    /* Add this pragma and apply it whenever appropriately */
    ApplyPragma (PushPop, Stack, Val);

ExitPoint:
    /* Free the identifier */
    SB_Done (&Ident);
}



static void IntPragma (pragma_scope_t Scope, pragma_t Token, StrBuf* B, IntStack* Stack, long Low, long High)
/* Handle a pragma that expects an int parameter */
{
    long  Val;
    int   Push;

    /* Unused at the moment */
    (void)Scope;
    (void)Token;

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
            ApplyPragma (-1, Stack, 0);
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

    /* Add this pragma and apply it whenever appropriately */
    ApplyPragma (Push, Stack, Val);
}



static void NoteMessagePragma (const char* Message)
/* Wrapper for printf-like Note() function protected from user-provided format
** specifiers.
*/
{
    Note ("%s", Message);
}



static void ParsePragmaString (void)
/* Parse the contents of _Pragma */
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
            Warning ("Unknown pragma '%s'", SB_GetConstBuf (&Ident));
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
            /* TODO: PES_EXPR (PES_DECL) */
            IntPragma (PES_STMT, Pragma, &B, &DataAlignment, 1, 4096);
            break;

        case PRAGMA_ALLOW_EAGER_INLINE:
            FlagPragma (PES_STMT, Pragma, &B, &EagerlyInlineFuncs);
            break;

        case PRAGMA_BSSSEG:
            Warning ("#pragma bssseg is obsolete, please use #pragma bss-name instead");
            /* FALLTHROUGH */
        case PRAGMA_BSS_NAME:
            /* TODO: PES_STMT or even PES_EXPR (PES_DECL) maybe? */
            SegNamePragma (PES_FUNC, PRAGMA_BSS_NAME, &B);
            break;

        case PRAGMA_CHARMAP:
            CharMapPragma (PES_IMM, &B);
            break;

        case PRAGMA_CHECKSTACK:
            Warning ("#pragma checkstack is obsolete, please use #pragma check-stack instead");
            /* FALLTHROUGH */
        case PRAGMA_CHECK_STACK:
            /* TODO: PES_SCOPE maybe? */
            FlagPragma (PES_FUNC, Pragma, &B, &CheckStack);
            break;

        case PRAGMA_CODESEG:
            Warning ("#pragma codeseg is obsolete, please use #pragma code-name instead");
            /* FALLTHROUGH */
        case PRAGMA_CODE_NAME:
            /* PES_FUNC is the only sensible option so far */
            SegNamePragma (PES_FUNC, PRAGMA_CODE_NAME, &B);
            break;

        case PRAGMA_CODESIZE:
            /* PES_EXPR would be optimization nightmare */
            IntPragma (PES_STMT, Pragma, &B, &CodeSizeFactor, 10, 1000);
            break;

        case PRAGMA_DATASEG:
            Warning ("#pragma dataseg is obsolete, please use #pragma data-name instead");
            /* FALLTHROUGH */
        case PRAGMA_DATA_NAME:
            /* TODO: PES_STMT or even PES_EXPR (PES_DECL) maybe? */
            SegNamePragma (PES_FUNC, PRAGMA_DATA_NAME, &B);
            break;

        case PRAGMA_INLINE_STDFUNCS:
            /* TODO: PES_EXPR maybe? */
            FlagPragma (PES_STMT, Pragma, &B, &InlineStdFuncs);
            break;

        case PRAGMA_LOCAL_STRINGS:
            /* TODO: PES_STMT or even PES_EXPR */
            FlagPragma (PES_FUNC, Pragma, &B, &LocalStrings);
            break;

        case PRAGMA_MESSAGE:
            /* PES_IMM is the only sensible option */
            StringPragma (PES_IMM, &B, NoteMessagePragma);
            break;

        case PRAGMA_OPTIMIZE:
            /* TODO: PES_STMT or even PES_EXPR maybe? */
            FlagPragma (PES_STMT, Pragma, &B, &Optimize);
            break;

        case PRAGMA_REGVARADDR:
            /* TODO: PES_STMT or even PES_EXPR maybe? */
            FlagPragma (PES_FUNC, Pragma, &B, &AllowRegVarAddr);
            break;

        case PRAGMA_REGVARS:
            Warning ("#pragma regvars is obsolete, please use #pragma register-vars instead");
            /* FALLTHROUGH */
        case PRAGMA_REGISTER_VARS:
            /* TODO: PES_STMT or even PES_EXPR (PES_DECL) maybe? */
            FlagPragma (PES_FUNC, Pragma, &B, &EnableRegVars);
            break;

        case PRAGMA_RODATASEG:
            Warning ("#pragma rodataseg is obsolete, please use #pragma rodata-name instead");
            /* FALLTHROUGH */
        case PRAGMA_RODATA_NAME:
            /* TODO: PES_STMT or even PES_EXPR maybe? */
            SegNamePragma (PES_FUNC, PRAGMA_RODATA_NAME, &B);
            break;

        case PRAGMA_SIGNEDCHARS:
            Warning ("#pragma signedchars is obsolete, please use #pragma signed-chars instead");
            /* FALLTHROUGH */
        case PRAGMA_SIGNED_CHARS:
            /* TODO: PES_STMT or even PES_EXPR maybe? */
            FlagPragma (PES_FUNC, Pragma, &B, &SignedChars);
            break;

        case PRAGMA_STATICLOCALS:
            Warning ("#pragma staticlocals is obsolete, please use #pragma static-locals instead");
            /* FALLTHROUGH */
        case PRAGMA_STATIC_LOCALS:
            /* TODO: PES_STMT or even PES_EXPR (PES_DECL) maybe? */
            FlagPragma (PES_FUNC, Pragma, &B, &StaticLocals);
            break;

        case PRAGMA_WRAPPED_CALL:
            /* PES_IMM is the only sensible option */
            WrappedCallPragma (PES_IMM, &B);
            break;

        case PRAGMA_WARN:
            /* PES_IMM is the only sensible option */
            WarnPragma (PES_IMM, &B);
            break;

        case PRAGMA_WRITABLE_STRINGS:
            /* TODO: PES_STMT or even PES_EXPR maybe? */
            FlagPragma (PES_FUNC, Pragma, &B, &WritableStrings);
            break;

        case PRAGMA_ZPSYM:
            /* PES_IMM is the only sensible option */
            StringPragma (PES_IMM, &B, MakeZPSym);
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



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



void ConsumePragma (void)
/* Parse a pragma. The pragma comes always in the form of the new C99 _Pragma()
** operator.
*/
{
    /* Skip the _Pragma token */
    NextToken ();

    /* Prevent from translating string literals in _Pragma */
    ++InPragmaParser;

    /* We expect an opening paren */
    if (!ConsumeLParen ()) {
        --InPragmaParser;
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
        /* Parse the pragma */
        ParsePragmaString ();
    }

    --InPragmaParser;

    /* Closing paren needed */
    ConsumeRParen ();
}
