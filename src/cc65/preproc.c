/*****************************************************************************/
/*                                                                           */
/*                                  preproc.c                                */
/*                                                                           */
/*                              cc65 preprocessor                            */
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
#include <string.h>
#include <stdlib.h>
#include <errno.h>

/* common */
#include "chartype.h"
#include "check.h"
#include "inline.h"
#include "print.h"
#include "xmalloc.h"

/* cc65 */
#include "codegen.h"
#include "error.h"
#include "expr.h"
#include "global.h"
#include "ident.h"
#include "incpath.h"
#include "input.h"
#include "lineinfo.h"
#include "macrotab.h"
#include "preproc.h"
#include "scanner.h"
#include "standard.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Set when the preprocessor calls expr() recursively */
unsigned char Preprocessing = 0;

/* Management data for #if */
#define MAX_IFS         64
#define IFCOND_NONE     0x00U
#define IFCOND_SKIP     0x01U
#define IFCOND_ELSE     0x02U
#define IFCOND_NEEDTERM 0x04U
static unsigned char IfStack[MAX_IFS];
static int           IfIndex = -1;

/* Buffer for macro expansion */
static StrBuf* MLine;

/* Structure used when expanding macros */
typedef struct MacroExp MacroExp;
struct MacroExp {
    Collection  ActualArgs;     /* Actual arguments */
    StrBuf      Replacement;    /* Replacement with arguments substituted */
    Macro*      M;              /* The macro we're handling */
};



/*****************************************************************************/
/*                                 Forwards                                  */
/*****************************************************************************/



static unsigned Pass1 (StrBuf* Source, StrBuf* Target);
/* Preprocessor pass 1. Remove whitespace. Handle old and new style comments
** and the "defined" operator.
*/

static void MacroReplacement (StrBuf* Source, StrBuf* Target);
/* Perform macro replacement. */



/*****************************************************************************/
/*                   Low level preprocessor token handling                   */
/*****************************************************************************/



/* Types of preprocessor tokens */
typedef enum {
    PP_ILLEGAL  = -1,
    PP_DEFINE,
    PP_ELIF,
    PP_ELSE,
    PP_ENDIF,
    PP_ERROR,
    PP_IF,
    PP_IFDEF,
    PP_IFNDEF,
    PP_INCLUDE,
    PP_LINE,
    PP_PRAGMA,
    PP_UNDEF,
    PP_WARNING,
} pptoken_t;



/* Preprocessor keyword to token mapping table */
static const struct PPToken {
    const char* Key;            /* Keyword */
    pptoken_t   Tok;            /* Token */
} PPTokens[] = {
    {   "define",       PP_DEFINE       },
    {   "elif",         PP_ELIF         },
    {   "else",         PP_ELSE         },
    {   "endif",        PP_ENDIF        },
    {   "error",        PP_ERROR        },
    {   "if",           PP_IF           },
    {   "ifdef",        PP_IFDEF        },
    {   "ifndef",       PP_IFNDEF       },
    {   "include",      PP_INCLUDE      },
    {   "line",         PP_LINE         },
    {   "pragma",       PP_PRAGMA       },
    {   "undef",        PP_UNDEF        },
    {   "warning",      PP_WARNING      },
};

/* Number of preprocessor tokens */
#define PPTOKEN_COUNT   (sizeof(PPTokens) / sizeof(PPTokens[0]))



static int CmpToken (const void* Key, const void* Elem)
/* Compare function for bsearch */
{
    return strcmp ((const char*) Key, ((const struct PPToken*) Elem)->Key);
}



static pptoken_t FindPPToken (const char* Ident)
/* Find a preprocessor token and return it. Return PP_ILLEGAL if the identifier
** is not a valid preprocessor token.
*/
{
    struct PPToken* P;
    P = bsearch (Ident, PPTokens, PPTOKEN_COUNT, sizeof (PPTokens[0]), CmpToken);
    return P? P->Tok : PP_ILLEGAL;
}



/*****************************************************************************/
/*                              struct MacroExp                              */
/*****************************************************************************/



static MacroExp* InitMacroExp (MacroExp* E, Macro* M)
/* Initialize a MacroExp structure */
{
    InitCollection (&E->ActualArgs);
    SB_Init (&E->Replacement);
    E->M = M;
    return E;
}



static void DoneMacroExp (MacroExp* E)
/* Cleanup after use of a MacroExp structure */
{
    unsigned I;

    /* Delete the list with actual arguments */
    for (I = 0; I < CollCount (&E->ActualArgs); ++I) {
        FreeStrBuf (CollAtUnchecked (&E->ActualArgs, I));
    }
    DoneCollection (&E->ActualArgs);
    SB_Done (&E->Replacement);
}



static void ME_AppendActual (MacroExp* E, StrBuf* Arg)
/* Add a copy of Arg to the list of actual macro arguments.
** NOTE: This function will clear Arg!
*/
{
    /* Create a new string buffer */
    StrBuf* A = NewStrBuf ();

    /* Move the contents of Arg to A */
    SB_Move (A, Arg);

    /* Add A to the actual arguments */
    CollAppend (&E->ActualArgs, A);
}



static StrBuf* ME_GetActual (MacroExp* E, unsigned Index)
/* Return an actual macro argument with the given index */
{
    return CollAt (&E->ActualArgs, Index);
}



static int ME_ArgIsVariadic (const MacroExp* E)
/* Return true if the next actual argument we will add is a variadic one */
{
    return (E->M->Variadic &&
            E->M->ArgCount == (int) CollCount (&E->ActualArgs) + 1);
}



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



static void Stringize (StrBuf* Source, StrBuf* Target)
/* Stringize the given string: Add double quotes at start and end and preceed
** each occurance of " and \ by a backslash.
*/
{
    char C;

    /* Add a starting quote */
    SB_AppendChar (Target, '\"');

    /* Replace any characters inside the string may not be part of a string
    ** unescaped.
    */
    while ((C = SB_Get (Source)) != '\0') {
        switch (C) {
            case '\"':
            case '\\':
                SB_AppendChar (Target, '\\');
            /* FALLTHROUGH */
            default:
                SB_AppendChar (Target, C);
                break;
        }
    }

    /* Add the closing quote */
    SB_AppendChar (Target, '\"');
}



static void OldStyleComment (void)
/* Remove an old style C comment from line. */
{
    /* Remember the current line number, so we can output better error
    ** messages if the comment is not terminated in the current file.
    */
    unsigned StartingLine = GetCurrentLine();

    /* Skip the start of comment chars */
    NextChar ();
    NextChar ();

    /* Skip the comment */
    while (CurC != '*' || NextC != '/') {
        if (CurC == '\0') {
            if (NextLine () == 0) {
                PPError ("End-of-file reached in comment starting at line %u",
                         StartingLine);
                return;
            }
        } else {
            if (CurC == '/' && NextC == '*') {
                PPWarning ("`/*' found inside a comment");
            }
            NextChar ();
        }
    }

    /* Skip the end of comment chars */
    NextChar ();
    NextChar ();
}



static void NewStyleComment (void)
/* Remove a new style C comment from line. */
{
    /* Beware: Because line continuation chars are handled when reading
    ** lines, we may only skip until the end of the source line, which
    ** may not be the same as the end of the input line. The end of the
    ** source line is denoted by a lf (\n) character.
    */
    do {
        NextChar ();
    } while (CurC != '\n' && CurC != '\0');
    if (CurC == '\n') {
        NextChar ();
    }
}



static int SkipWhitespace (int SkipLines)
/* Skip white space in the input stream. Do also skip newlines if SkipLines
** is true. Return zero if nothing was skipped, otherwise return a
** value != zero.
*/
{
    int Skipped = 0;
    while (1) {
        if (IsSpace (CurC)) {
            NextChar ();
            Skipped = 1;
        } else if (CurC == '\0' && SkipLines) {
            /* End of line, read next */
            if (NextLine () != 0) {
                Skipped = 1;
            } else {
                /* End of input */
                break;
            }
        } else {
            /* No more white space */
            break;
        }
    }
    return Skipped;
}



static void CopyQuotedString (StrBuf* Target)
/* Copy a single or double quoted string from the input to Target. */
{
    /* Remember the quote character, copy it to the target buffer and skip it */
    char Quote = CurC;
    SB_AppendChar (Target, CurC);
    NextChar ();

    /* Copy the characters inside the string */
    while (CurC != '\0' && CurC != Quote) {
        /* Keep an escaped char */
        if (CurC == '\\') {
            SB_AppendChar (Target, CurC);
            NextChar ();
        }
        /* Copy the character */
        SB_AppendChar (Target, CurC);
        NextChar ();
    }

    /* If we had a terminating quote, copy it */
    if (CurC != '\0') {
        SB_AppendChar (Target, CurC);
        NextChar ();
    }
}



/*****************************************************************************/
/*                                Macro stuff                                */
/*****************************************************************************/



static int MacName (char* Ident)
/* Get a macro symbol name into Ident.  If we have an error, print a
** diagnostic message and clear the line.
*/
{
    if (IsSym (Ident) == 0) {
        PPError ("Identifier expected");
        ClearLine ();
        return 0;
    } else {
        return 1;
    }
}



static void ReadMacroArgs (MacroExp* E)
/* Identify the arguments to a macro call */
{
    unsigned    Parens;         /* Number of open parenthesis */
    StrBuf      Arg = STATIC_STRBUF_INITIALIZER;

    /* Read the actual macro arguments */
    Parens = 0;
    while (1) {
        if (CurC == '(') {

            /* Nested parenthesis */
            SB_AppendChar (&Arg, CurC);
            NextChar ();
            ++Parens;

        } else if (IsQuote (CurC)) {

            /* Quoted string - just copy */
            CopyQuotedString (&Arg);

        } else if (CurC == ',' || CurC == ')') {

            if (Parens) {
                /* Comma or right paren inside nested parenthesis */
                if (CurC == ')') {
                    --Parens;
                }
                SB_AppendChar (&Arg, CurC);
                NextChar ();
            } else if (CurC == ',' && ME_ArgIsVariadic (E)) {
                /* It's a comma, but we're inside a variadic macro argument, so
                ** just copy it and proceed.
                */
                SB_AppendChar (&Arg, CurC);
                NextChar ();
            } else {
                /* End of actual argument. Remove whitespace from the end. */
                while (IsSpace (SB_LookAtLast (&Arg))) {
                    SB_Drop (&Arg, 1);
                }

                /* If this is not the single empty argument for a macro with
                ** an empty argument list, remember it.
                */
                if (CurC != ')' || SB_NotEmpty (&Arg) || E->M->ArgCount > 0) {
                    ME_AppendActual (E, &Arg);
                }

                /* Check for end of macro param list */
                if (CurC == ')') {
                    NextChar ();
                    break;
                }

                /* Start the next param */
                NextChar ();
                SB_Clear (&Arg);
            }
        } else if (SkipWhitespace (1)) {
            /* Squeeze runs of blanks within an arg */
            if (SB_NotEmpty (&Arg)) {
                SB_AppendChar (&Arg, ' ');
            }
        } else if (CurC == '/' && NextC == '*') {
            if (SB_NotEmpty (&Arg)) {
                SB_AppendChar (&Arg, ' ');
            }
            OldStyleComment ();
        } else if (IS_Get (&Standard) >= STD_C99 && CurC == '/' && NextC == '/') {
            if (SB_NotEmpty (&Arg)) {
                SB_AppendChar (&Arg, ' ');
            }
            NewStyleComment ();
        } else if (CurC == '\0') {
            /* End of input inside macro argument list */
            PPError ("Unterminated argument list invoking macro `%s'", E->M->Name);

            ClearLine ();
            break;
        } else {
            /* Just copy the character */
            SB_AppendChar (&Arg, CurC);
            NextChar ();
        }
    }

    /* Deallocate string buf resources */
    SB_Done (&Arg);
}



static void MacroArgSubst (MacroExp* E)
/* Argument substitution according to ISO/IEC 9899:1999 (E), 6.10.3.1ff */
{
    ident       Ident;
    int         ArgIdx;
    StrBuf*     OldSource;
    StrBuf*     Arg;
    int         HaveSpace;


    /* Remember the current input and switch to the macro replacement. */
    int OldIndex = SB_GetIndex (&E->M->Replacement);
    SB_Reset (&E->M->Replacement);
    OldSource = InitLine (&E->M->Replacement);

    /* Argument handling loop */
    while (CurC != '\0') {

        /* If we have an identifier, check if it's a macro */
        if (IsSym (Ident)) {

            /* Check if it's a macro argument */
            if ((ArgIdx = FindMacroArg (E->M, Ident)) >= 0) {

                /* A macro argument. Get the corresponding actual argument. */
                Arg = ME_GetActual (E, ArgIdx);

                /* Copy any following whitespace */
                HaveSpace = SkipWhitespace (0);

                /* If a ## operator follows, we have to insert the actual
                ** argument as is, otherwise it must be macro replaced.
                */
                if (CurC == '#' && NextC == '#') {

                    /* ### Add placemarker if necessary */
                    SB_Append (&E->Replacement, Arg);

                } else {

                    /* Replace the formal argument by a macro replaced copy
                    ** of the actual.
                    */
                    SB_Reset (Arg);
                    MacroReplacement (Arg, &E->Replacement);

                    /* If we skipped whitespace before, re-add it now */
                    if (HaveSpace) {
                        SB_AppendChar (&E->Replacement, ' ');
                    }
                }


            } else {

                /* An identifier, keep it */
                SB_AppendStr (&E->Replacement, Ident);

            }

        } else if (CurC == '#' && NextC == '#') {

            /* ## operator. */
            NextChar ();
            NextChar ();
            SkipWhitespace (0);

            /* Since we need to concatenate the token sequences, remove
            ** any whitespace that was added to target, since it must come
            ** from the input.
            */
            while (IsSpace (SB_LookAtLast (&E->Replacement))) {
                SB_Drop (&E->Replacement, 1);
            }

            /* If the next token is an identifier which is a macro argument,
            ** replace it, otherwise do nothing.
            */
            if (IsSym (Ident)) {

                /* Check if it's a macro argument */
                if ((ArgIdx = FindMacroArg (E->M, Ident)) >= 0) {

                    /* Get the corresponding actual argument and add it. */
                    SB_Append (&E->Replacement, ME_GetActual (E, ArgIdx));

                } else {

                    /* Just an ordinary identifier - add as is */
                    SB_AppendStr (&E->Replacement, Ident);

                }
            }

        } else if (CurC == '#' && E->M->ArgCount >= 0) {

            /* A # operator within a macro expansion of a function like
            ** macro. Read the following identifier and check if it's a
            ** macro parameter.
            */
            NextChar ();
            SkipWhitespace (0);
            if (!IsSym (Ident) || (ArgIdx = FindMacroArg (E->M, Ident)) < 0) {
                PPError ("`#' is not followed by a macro parameter");
            } else {
                /* Make a valid string from Replacement */
                Arg = ME_GetActual (E, ArgIdx);
                SB_Reset (Arg);
                Stringize (Arg, &E->Replacement);
            }

        } else if (IsQuote (CurC)) {
            CopyQuotedString (&E->Replacement);
        } else {
            SB_AppendChar (&E->Replacement, CurC);
            NextChar ();
        }
    }

#if 0
    /* Remove whitespace from the end of the line */
    while (IsSpace (SB_LookAtLast (&E->Replacement))) {
        SB_Drop (&E->Replacement, 1);
    }
#endif

    /* Switch back the input */
    InitLine (OldSource);
    SB_SetIndex (&E->M->Replacement, OldIndex);
}



static void MacroCall (StrBuf* Target, Macro* M)
/* Process a function like macro */
{
    MacroExp    E;

    /* Eat the left paren */
    NextChar ();

    /* Initialize our MacroExp structure */
    InitMacroExp (&E, M);

    /* Read the actual macro arguments */
    ReadMacroArgs (&E);

    /* Compare formal and actual argument count */
    if (CollCount (&E.ActualArgs) != (unsigned) M->ArgCount) {

        StrBuf Arg = STATIC_STRBUF_INITIALIZER;

        /* Argument count mismatch */
        PPError ("Macro argument count mismatch");

        /* Be sure to make enough empty arguments available */
        while (CollCount (&E.ActualArgs) < (unsigned) M->ArgCount) {
            ME_AppendActual (&E, &Arg);
        }
    }

    /* Replace macro arguments handling the # and ## operators */
    MacroArgSubst (&E);

    /* Do macro replacement on the macro that already has the parameters
    ** substituted.
    */
    M->Expanding = 1;
    MacroReplacement (&E.Replacement, Target);
    M->Expanding = 0;

    /* Free memory allocated for the macro expansion structure */
    DoneMacroExp (&E);
}



static void ExpandMacro (StrBuf* Target, Macro* M)
/* Expand a macro into Target */
{
#if 0
    static unsigned V = 0;
    printf ("Expanding %s(%u)\n", M->Name, ++V);
#endif

    /* Check if this is a function like macro */
    if (M->ArgCount >= 0) {

        int Whitespace = SkipWhitespace (1);
        if (CurC != '(') {
            /* Function like macro but no parameter list */
            SB_AppendStr (Target, M->Name);
            if (Whitespace) {
                SB_AppendChar (Target, ' ');
            }
        } else {
            /* Function like macro */
            MacroCall (Target, M);
        }

    } else {

        MacroExp E;
        InitMacroExp (&E, M);

        /* Handle # and ## operators for object like macros */
        MacroArgSubst (&E);

        /* Do macro replacement on the macro that already has the parameters
        ** substituted.
        */
        M->Expanding = 1;
        MacroReplacement (&E.Replacement, Target);
        M->Expanding = 0;

        /* Free memory allocated for the macro expansion structure */
        DoneMacroExp (&E);

    }
#if 0
    printf ("Done with %s(%u)\n", M->Name, V--);
#endif
}



static void DefineMacro (void)
/* Handle a macro definition. */
{
    ident       Ident;
    Macro*      M;
    Macro*      Existing;
    int         C89;

    /* Read the macro name */
    SkipWhitespace (0);
    if (!MacName (Ident)) {
        return;
    }

    /* Remember if we're in C89 mode */
    C89 = (IS_Get (&Standard) == STD_C89);

    /* Get an existing macro definition with this name */
    Existing = FindMacro (Ident);

    /* Create a new macro definition */
    M = NewMacro (Ident);

    /* Check if this is a function like macro */
    if (CurC == '(') {

        /* Skip the left paren */
        NextChar ();

        /* Set the marker that this is a function like macro */
        M->ArgCount = 0;

        /* Read the formal parameter list */
        while (1) {

            /* Skip white space and check for end of parameter list */
            SkipWhitespace (0);
            if (CurC == ')') {
                break;
            }

            /* The next token must be either an identifier, or - if not in
            ** C89 mode - the ellipsis.
            */
            if (!C89 && CurC == '.') {
                /* Ellipsis */
                NextChar ();
                if (CurC != '.' || NextC != '.') {
                    PPError ("`...' expected");
                    ClearLine ();
                    return;
                }
                NextChar ();
                NextChar ();

                /* Remember that the macro is variadic and use __VA_ARGS__ as
                ** the argument name.
                */
                AddMacroArg (M, "__VA_ARGS__");
                M->Variadic = 1;

            } else {
                /* Must be macro argument name */
                if (MacName (Ident) == 0) {
                    return;
                }

                /* __VA_ARGS__ is only allowed in C89 mode */
                if (!C89 && strcmp (Ident, "__VA_ARGS__") == 0) {
                    PPWarning ("`__VA_ARGS__' can only appear in the expansion "
                               "of a C99 variadic macro");
                }

                /* Add the macro argument */
                AddMacroArg (M, Ident);
            }

            /* If we had an ellipsis, or the next char is not a comma, we've
            ** reached the end of the macro argument list.
            */
            SkipWhitespace (0);
            if (M->Variadic || CurC != ',') {
                break;
            }
            NextChar ();
        }

        /* Check for a right paren and eat it if we find one */
        if (CurC != ')') {
            PPError ("`)' expected");
            ClearLine ();
            return;
        }
        NextChar ();
    }

    /* Skip whitespace before the macro replacement */
    SkipWhitespace (0);

    /* Insert the macro into the macro table and allocate the ActualArgs array */
    InsertMacro (M);

    /* Remove whitespace and comments from the line, store the preprocessed
    ** line into the macro replacement buffer.
    */
    Pass1 (Line, &M->Replacement);

    /* Remove whitespace from the end of the line */
    while (IsSpace (SB_LookAtLast (&M->Replacement))) {
        SB_Drop (&M->Replacement, 1);
    }
#if 0
    printf ("%s: <%.*s>\n", M->Name, SB_GetLen (&M->Replacement), SB_GetConstBuf (&M->Replacement));
#endif

    /* If we have an existing macro, check if the redefinition is identical.
    ** Print a diagnostic if not.
    */
    if (Existing && MacroCmp (M, Existing) != 0) {
        PPError ("Macro redefinition is not identical");
    }
}



/*****************************************************************************/
/*                               Preprocessing                               */
/*****************************************************************************/



static unsigned Pass1 (StrBuf* Source, StrBuf* Target)
/* Preprocessor pass 1. Remove whitespace. Handle old and new style comments
** and the "defined" operator.
*/
{
    unsigned    IdentCount;
    ident       Ident;
    int         HaveParen;

    /* Switch to the new input source */
    StrBuf* OldSource = InitLine (Source);

    /* Loop removing ws and comments */
    IdentCount = 0;
    while (CurC != '\0') {
        if (SkipWhitespace (0)) {
            /* Squeeze runs of blanks */
            if (!IsSpace (SB_LookAtLast (Target))) {
                SB_AppendChar (Target, ' ');
            }
        } else if (IsSym (Ident)) {
            if (Preprocessing && strcmp (Ident, "defined") == 0) {
                /* Handle the "defined" operator */
                SkipWhitespace (0);
                HaveParen = 0;
                if (CurC == '(') {
                    HaveParen = 1;
                    NextChar ();
                    SkipWhitespace (0);
                }
                if (IsSym (Ident)) {
                    SB_AppendChar (Target, IsMacro (Ident)? '1' : '0');
                    if (HaveParen) {
                        SkipWhitespace (0);
                        if (CurC != ')') {
                            PPError ("`)' expected");
                        } else {
                            NextChar ();
                        }
                    }
                } else {
                    PPError ("Identifier expected");
                    SB_AppendChar (Target, '0');
                }
            } else {
                ++IdentCount;
                SB_AppendStr (Target, Ident);
            }
        } else if (IsQuote (CurC)) {
            CopyQuotedString (Target);
        } else if (CurC == '/' && NextC == '*') {
            if (!IsSpace (SB_LookAtLast (Target))) {
                SB_AppendChar (Target, ' ');
            }
            OldStyleComment ();
        } else if (IS_Get (&Standard) >= STD_C99 && CurC == '/' && NextC == '/') {
            if (!IsSpace (SB_LookAtLast (Target))) {
                SB_AppendChar (Target, ' ');
            }
            NewStyleComment ();
        } else {
            SB_AppendChar (Target, CurC);
            NextChar ();
        }
    }

    /* Switch back to the old source */
    InitLine (OldSource);

    /* Return the number of identifiers found in the line */
    return IdentCount;
}



static void MacroReplacement (StrBuf* Source, StrBuf* Target)
/* Perform macro replacement. */
{
    ident       Ident;
    Macro*      M;

    /* Remember the current input and switch to Source */
    StrBuf* OldSource = InitLine (Source);

    /* Loop substituting macros */
    while (CurC != '\0') {
        /* If we have an identifier, check if it's a macro */
        if (IsSym (Ident)) {
            /* Check if it's a macro */
            if ((M = FindMacro (Ident)) != 0 && !M->Expanding) {
                /* It's a macro, expand it */
                ExpandMacro (Target, M);
            } else {
                /* An identifier, keep it */
                SB_AppendStr (Target, Ident);
            }
        } else if (IsQuote (CurC)) {
            CopyQuotedString (Target);
        } else if (IsSpace (CurC)) {
            if (!IsSpace (SB_LookAtLast (Target))) {
                SB_AppendChar (Target, CurC);
            }
            NextChar ();
        } else {
            SB_AppendChar (Target, CurC);
            NextChar ();
        }
    }

    /* Switch back the input */
    InitLine (OldSource);
}



static void PreprocessLine (void)
/* Translate one line. */
{
    /* Trim whitespace and remove comments. The function returns the number of
    ** identifiers found. If there were any, we will have to check for macros.
    */
    SB_Clear (MLine);
    if (Pass1 (Line, MLine) > 0) {
        MLine = InitLine (MLine);
        SB_Reset (Line);
        SB_Clear (MLine);
        MacroReplacement (Line, MLine);
    }

    /* Read from the new line */
    SB_Reset (MLine);
    MLine = InitLine (MLine);
}



static int PushIf (int Skip, int Invert, int Cond)
/* Push a new if level onto the if stack */
{
    /* Check for an overflow of the if stack */
    if (IfIndex >= MAX_IFS-1) {
        PPError ("Too many nested #if clauses");
        return 1;
    }

    /* Push the #if condition */
    ++IfIndex;
    if (Skip) {
        IfStack[IfIndex] = IFCOND_SKIP | IFCOND_NEEDTERM;
        return 1;
    } else {
        IfStack[IfIndex] = IFCOND_NONE | IFCOND_NEEDTERM;
        return (Invert ^ Cond);
    }
}



static void DoError (void)
/* Print an error */
{
    SkipWhitespace (0);
    if (CurC == '\0') {
        PPError ("Invalid #error directive");
    } else {
        PPError ("#error: %s", SB_GetConstBuf (Line) + SB_GetIndex (Line));
    }

    /* Clear the rest of line */
    ClearLine ();
}



static int DoIf (int Skip)
/* Process #if directive */
{
    ExprDesc Expr;

    /* We're about to abuse the compiler expression parser to evaluate the
    ** #if expression. Save the current tokens to come back here later.
    ** NOTE: Yes, this is a hack, but it saves a complete separate expression
    ** evaluation for the preprocessor.
    */
    Token SavedCurTok  = CurTok;
    Token SavedNextTok = NextTok;

    /* Make sure the line infos for the tokens won't get removed */
    if (SavedCurTok.LI) {
        UseLineInfo (SavedCurTok.LI);
    }
    if (SavedNextTok.LI) {
        UseLineInfo (SavedNextTok.LI);
    }

    /* Switch into special preprocessing mode */
    Preprocessing = 1;

    /* Expand macros in this line */
    PreprocessLine ();

    /* Add two semicolons as sentinels to the line, so the following
    ** expression evaluation will eat these two tokens but nothing from
    ** the following line.
    */
    SB_AppendStr (Line, ";;");
    SB_Terminate (Line);

    /* Load CurTok and NextTok with tokens from the new input */
    NextToken ();
    NextToken ();

    /* Call the expression parser */
    ConstExpr (hie1, &Expr);

    /* End preprocessing mode */
    Preprocessing = 0;

    /* Reset the old tokens */
    CurTok  = SavedCurTok;
    NextTok = SavedNextTok;

    /* Set the #if condition according to the expression result */
    return PushIf (Skip, 1, Expr.IVal != 0);
}



static int DoIfDef (int skip, int flag)
/* Process #ifdef if flag == 1, or #ifndef if flag == 0. */
{
    ident Ident;

    SkipWhitespace (0);
    if (MacName (Ident) == 0) {
        return 0;
    } else {
        return PushIf (skip, flag, IsMacro(Ident));
    }
}



static void DoInclude (void)
/* Open an include file. */
{
    char        RTerm;
    InputType   IT;
    StrBuf      Filename = STATIC_STRBUF_INITIALIZER;


    /* Preprocess the remainder of the line */
    PreprocessLine ();

    /* Skip blanks */
    SkipWhitespace (0);

    /* Get the next char and check for a valid file name terminator. Setup
    ** the include directory spec (SYS/USR) by looking at the terminator.
    */
    switch (CurC) {

        case '\"':
            RTerm   = '\"';
            IT = IT_USRINC;
            break;

        case '<':
            RTerm   = '>';
            IT = IT_SYSINC;
            break;

        default:
            PPError ("`\"' or `<' expected");
            goto Done;
    }
    NextChar ();

    /* Get a copy of the filename */
    while (CurC != '\0' && CurC != RTerm) {
        SB_AppendChar (&Filename, CurC);
        NextChar ();
    }
    SB_Terminate (&Filename);

    /* Check if we got a terminator */
    if (CurC == RTerm) {
        /* Open the include file */
        OpenIncludeFile (SB_GetConstBuf (&Filename), IT);
    } else if (CurC == '\0') {
        /* No terminator found */
        PPError ("#include expects \"FILENAME\" or <FILENAME>");
    }

Done:
    /* Free the allocated filename data */
    SB_Done (&Filename);

    /* Clear the remaining line so the next input will come from the new
    ** file (if open)
    */
    ClearLine ();
}



static void DoPragma (void)
/* Handle a #pragma line by converting the #pragma preprocessor directive into
** the _Pragma() compiler operator.
*/
{
    /* Skip blanks following the #pragma directive */
    SkipWhitespace (0);

    /* Copy the remainder of the line into MLine removing comments and ws */
    SB_Clear (MLine);
    Pass1 (Line, MLine);

    /* Convert the directive into the operator */
    SB_CopyStr (Line, "_Pragma (");
    SB_Reset (MLine);
    Stringize (MLine, Line);
    SB_AppendChar (Line, ')');

    /* Initialize reading from line */
    SB_Reset (Line);
    InitLine (Line);
}



static void DoUndef (void)
/* Process the #undef directive */
{
    ident Ident;

    SkipWhitespace (0);
    if (MacName (Ident)) {
        UndefineMacro (Ident);
    }
}



static void DoWarning (void)
/* Print a warning */
{
    SkipWhitespace (0);
    if (CurC == '\0') {
        PPError ("Invalid #warning directive");
    } else {
        PPWarning ("#warning: %s", SB_GetConstBuf (Line) + SB_GetIndex (Line));
    }

    /* Clear the rest of line */
    ClearLine ();
}



void Preprocess (void)
/* Preprocess a line */
{
    int         Skip;
    ident       Directive;

    /* Create the output buffer if we don't already have one */
    if (MLine == 0) {
        MLine = NewStrBuf ();
    }

    /* Skip white space at the beginning of the line */
    SkipWhitespace (0);

    /* Check for stuff to skip */
    Skip = 0;
    while (CurC == '\0' || CurC == '#' || Skip) {

        /* Check for preprocessor lines lines */
        if (CurC == '#') {
            NextChar ();
            SkipWhitespace (0);
            if (CurC == '\0') {
                /* Ignore the empty preprocessor directive */
                continue;
            }
            if (!IsSym (Directive)) {
                PPError ("Preprocessor directive expected");
                ClearLine ();
            } else {
                switch (FindPPToken (Directive)) {

                    case PP_DEFINE:
                        if (!Skip) {
                            DefineMacro ();
                        }
                        break;

                    case PP_ELIF:
                        if (IfIndex >= 0) {
                            if ((IfStack[IfIndex] & IFCOND_ELSE) == 0) {

                                /* Handle as #else/#if combination */
                                if ((IfStack[IfIndex] & IFCOND_SKIP) == 0) {
                                    Skip = !Skip;
                                }
                                IfStack[IfIndex] |= IFCOND_ELSE;
                                Skip = DoIf (Skip);

                                /* #elif doesn't need a terminator */
                                IfStack[IfIndex] &= ~IFCOND_NEEDTERM;
                            } else {
                                PPError ("Duplicate #else/#elif");
                            }
                        } else {
                            PPError ("Unexpected #elif");
                        }
                        break;

                    case PP_ELSE:
                        if (IfIndex >= 0) {
                            if ((IfStack[IfIndex] & IFCOND_ELSE) == 0) {
                                if ((IfStack[IfIndex] & IFCOND_SKIP) == 0) {
                                    Skip = !Skip;
                                }
                                IfStack[IfIndex] |= IFCOND_ELSE;
                            } else {
                                PPError ("Duplicate #else");
                            }
                        } else {
                            PPError ("Unexpected `#else'");
                        }
                        break;

                    case PP_ENDIF:
                        if (IfIndex >= 0) {
                            /* Remove any clauses on top of stack that do not
                            ** need a terminating #endif.
                            */
                            while (IfIndex >= 0 && (IfStack[IfIndex] & IFCOND_NEEDTERM) == 0) {
                                --IfIndex;
                            }

                            /* Stack may not be empty here or something is wrong */
                            CHECK (IfIndex >= 0);

                            /* Remove the clause that needs a terminator */
                            Skip = (IfStack[IfIndex--] & IFCOND_SKIP) != 0;
                        } else {
                            PPError ("Unexpected `#endif'");
                        }
                        break;

                    case PP_ERROR:
                        if (!Skip) {
                            DoError ();
                        }
                        break;

                    case PP_IF:
                        Skip = DoIf (Skip);
                        break;

                    case PP_IFDEF:
                        Skip = DoIfDef (Skip, 1);
                        break;

                    case PP_IFNDEF:
                        Skip = DoIfDef (Skip, 0);
                        break;

                    case PP_INCLUDE:
                        if (!Skip) {
                            DoInclude ();
                        }
                        break;

                    case PP_LINE:
                        /* Should do something in C99 at least, but we ignore it */
                        if (!Skip) {
                            ClearLine ();
                        }
                        break;

                    case PP_PRAGMA:
                        if (!Skip) {
                            DoPragma ();
                            goto Done;
                        }
                        break;

                    case PP_UNDEF:
                        if (!Skip) {
                            DoUndef ();
                        }
                        break;

                    case PP_WARNING:
                        /* #warning is a non standard extension */
                        if (IS_Get (&Standard) > STD_C99) {
                            if (!Skip) {
                                DoWarning ();
                            }
                        } else {
                            if (!Skip) {
                                PPError ("Preprocessor directive expected");
                            }
                            ClearLine ();
                        }
                        break;

                    default:
                        if (!Skip) {
                            PPError ("Preprocessor directive expected");
                        }
                        ClearLine ();
                }
            }

        }
        if (NextLine () == 0) {
            if (IfIndex >= 0) {
                PPError ("`#endif' expected");
            }
            return;
        }
        SkipWhitespace (0);
    }

    PreprocessLine ();

Done:
    if (Verbosity > 1 && SB_NotEmpty (Line)) {
        printf ("%s(%u): %.*s\n", GetCurrentFile (), GetCurrentLine (),
                (int) SB_GetLen (Line), SB_GetConstBuf (Line));
    }
}
