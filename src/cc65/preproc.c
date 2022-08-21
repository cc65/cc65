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
#include "global.h"
#include "ident.h"
#include "incpath.h"
#include "input.h"
#include "lineinfo.h"
#include "macrotab.h"
#include "ppexpr.h"
#include "preproc.h"
#include "scanner.h"
#include "standard.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Macro scanner mode flags */
#define MSM_NONE                0x00U   /* Default */
#define MSM_MULTILINE           0x01U   /* Read from multiple lines */
#define MSM_IN_DIRECTIVE        0x02U   /* In PP directives scan */
#define MSM_IN_ARG_LIST         0x04U   /* In macro argument scan */
#define MSM_IN_ARG_EXPANSION    0x08U   /* In expansion on arguments */
#define MSM_OP_DEFINED          0x10U   /* Handle the defined operator */
#define MSM_OP_HAS_INCLUDE      0x20U   /* Handle the __has_include operator */
#define MSM_OP_HAS_C_ATTRIBUTE  0x40U   /* Handle the __has_c_attribute operator */
#define MSM_TOK_HEADER          0x80U   /* Support header tokens */

/* Management data for #if */
#define IFCOND_NONE     0x00U
#define IFCOND_SKIP     0x01U
#define IFCOND_ELSE     0x02U
#define IFCOND_NEEDTERM 0x04U

/* Current PP if stack */
static PPIfStack* PPStack;

/* Intermediate input buffers */
static StrBuf* PLine;   /* Buffer for macro expansion */
static StrBuf* MLine;   /* Buffer for macro expansion in #pragma */
static StrBuf* OLine;   /* Buffer for #pragma output */

/* Newlines to be added to preprocessed text */
static int PendingNewLines;
static int FileChanged;

/* Structure used when expanding macros */
typedef struct MacroExp MacroExp;
struct MacroExp {
    Collection  ActualArgs;     /* Actual arguments */
    StrBuf      Replacement;    /* Replacement with arguments substituted */
};



/*****************************************************************************/
/*                                 Forwards                                  */
/*****************************************************************************/



static void TranslationPhase3 (StrBuf* Source, StrBuf* Target);
/* Mimic Translation Phase 3. Handle old and new style comments. Collapse
** non-newline whitespace sequences.
*/

static void PreprocessDirective (StrBuf* Source, StrBuf* Target, unsigned ModeFlags);
/* Preprocess a single line. Handle specified tokens and operators, remove
** whitespace and comments, then do macro replacement.
*/

static int ParseDirectives (unsigned ModeFlags);
/* Handle directives. Return 1 if any whitespace or newlines are parsed. */

static void MacroReplacement (StrBuf* Source, StrBuf* Target, unsigned ModeFlags);
/* Scan for and perform macro replacement */



/*****************************************************************************/
/*                   Low level preprocessor token handling                   */
/*****************************************************************************/



/* Types of preprocessor directives */
typedef enum {
    PPD_ILLEGAL = -1,
    PPD_DEFINE,
    PPD_ELIF,
    PPD_ELSE,
    PPD_ENDIF,
    PPD_ERROR,
    PPD_IF,
    PPD_IFDEF,
    PPD_IFNDEF,
    PPD_INCLUDE,
    PPD_LINE,
    PPD_PRAGMA,
    PPD_UNDEF,
    PPD_WARNING,
} ppdirective_t;



/* Preprocessor directive tokens mapping table */
static const struct PPDType {
    const char*     Tok;        /* Token */
    ppdirective_t   Type;       /* Type */
} PPDTypes[] = {
    {   "define",       PPD_DEFINE      },
    {   "elif",         PPD_ELIF        },
    {   "else",         PPD_ELSE        },
    {   "endif",        PPD_ENDIF       },
    {   "error",        PPD_ERROR       },
    {   "if",           PPD_IF          },
    {   "ifdef",        PPD_IFDEF       },
    {   "ifndef",       PPD_IFNDEF      },
    {   "include",      PPD_INCLUDE     },
    {   "line",         PPD_LINE        },
    {   "pragma",       PPD_PRAGMA      },
    {   "undef",        PPD_UNDEF       },
    {   "warning",      PPD_WARNING     },
};

/* Number of preprocessor directive types */
#define PPDTOKEN_COUNT  (sizeof(PPDTypes) / sizeof(PPDTypes[0]))



static int CmpToken (const void* Key, const void* Elem)
/* Compare function for bsearch */
{
    return strcmp ((const char*) Key, ((const struct PPDType*) Elem)->Tok);
}



static ppdirective_t FindPPDirectiveType (const char* Ident)
/* Find a preprocessor directive type and return it. Return PPD_ILLEGAL if the
** identifier is not a valid preprocessor directive token.
*/
{
    struct PPDType* P;
    P = bsearch (Ident, PPDTypes, PPDTOKEN_COUNT, sizeof (PPDTypes[0]), CmpToken);
    return P? P->Type : PPD_ILLEGAL;
}



/*****************************************************************************/
/*                              struct MacroExp                              */
/*****************************************************************************/



static MacroExp* InitMacroExp (MacroExp* E)
/* Initialize a MacroExp structure */
{
    InitCollection (&E->ActualArgs);
    SB_Init (&E->Replacement);
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



static int ME_ArgIsVariadic (const MacroExp* E, const Macro* M)
/* Return true if the next actual argument we will add is a variadic one */
{
    return (M->Variadic &&
            M->ArgCount == (int) CollCount (&E->ActualArgs) + 1);
}



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



static int MacName (char* Ident)
/* Get a macro symbol name into Ident.  If we have an error, print a
** diagnostic message and clear the line.
*/
{
    if (IsSym (Ident) == 0) {
        if (CurC != '\0') {
            PPError ("Macro name must be an identifier");
        } else {
            PPError ("Missing macro name");
        }
        ClearLine ();
        return 0;
    } else {
        return 1;
    }
}



static void AddPreLine (StrBuf* Str)
/* Add newlines to the string buffer */
{
    if (!PreprocessOnly) {
        PendingNewLines = 0;
        return;
    }

    if (FileChanged || PendingNewLines > 6) {
        /* Output #line directives as source info */
        StrBuf Comment = AUTO_STRBUF_INITIALIZER;
        if (SB_NotEmpty (Str) && SB_LookAtLast (Str) != '\n') {
            SB_AppendChar (Str, '\n');
        }
        SB_Printf (&Comment, "#line %u \"%s\"\n", GetCurrentLine (), GetCurrentFile ());
        SB_Append (Str, &Comment);
    } else {
        /* Output new lines */
        while (PendingNewLines > 0) {
            SB_AppendChar (Str, '\n');
            --PendingNewLines;
        }
    }
    FileChanged = 0;
    PendingNewLines = 0;
}



static void AppendIndent (StrBuf* Str, int Count)
/* Add Count of spaces ' ' to the string buffer */
{
    while (Count > 0) {
        SB_AppendChar (Str, ' ');
        --Count;
    }
}



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
    unsigned StartingLine = GetCurrentLine ();

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
                PPWarning ("'/*' found inside a comment");
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
/* Skip white space and comments in the input stream. If skipLines is true,
** also skip newlines and add that count to global PendingNewLines. Return 1
** if the last skipped character was a white space other than a newline '\n',
** otherwise return -1 if there were any newline characters skipped, otherwise
** return 0 if nothing was skipped.
*/
{
    int Skipped = 0;
    int NewLine = 0;
    while (1) {
        if (IsSpace (CurC)) {
            NextChar ();
            Skipped = 1;
        } else if (CurC == '/' && NextC == '*') {
            OldStyleComment ();
            Skipped = 1;
        } else if (IS_Get (&Standard) >= STD_C99 && CurC == '/' && NextC == '/') {
            NewStyleComment ();
            Skipped = 1;
        } else if (CurC == '\0' && SkipLines) {
            /* End of line, read next */
            if (NextLine () != 0) {
                ++PendingNewLines;
                NewLine = 1;
                Skipped = 0;
            } else {
                /* End of input */
                break;
            }
        } else {
            /* No more white space */
            break;
        }
    }
    return Skipped != 0 ? Skipped : -(NewLine != 0);
}



static void CopyHeaderNameToken (StrBuf* Target)
/* Copy a header name from the input to Target. */
{
    /* Remember the quote character, copy it to the target buffer and skip it */
    char Quote = CurC == '"' ? '"' : '>';
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
    } else {
        PPWarning ("Missing terminating %c character", Quote);
    }
}



static int CheckExtraTokens (const char* Name)
/* Check for extra tokens at the end of the directive. Return 1 if there are
** extra tokens, otherwise 0.
*/
{
    SkipWhitespace (0);
    if (SB_GetIndex (Line) != SB_GetLen (Line)) {
        PPWarning ("Extra tokens at end of #%s directive", Name);
        return 1;
    }
    return 0;
}



/*****************************************************************************/
/*                                Macro stuff                                */
/*****************************************************************************/



static void ReadMacroArgs (MacroExp* E, const Macro* M, int MultiLine)
/* Identify the arguments to a macro call as-is */
{
    int         MissingParen = 0;
    unsigned    Parens;         /* Number of open parenthesis */
    StrBuf      Arg = AUTO_STRBUF_INITIALIZER;

    /* Eat the left paren */
    NextChar ();

    /* Read the actual macro arguments */
    Parens = 0;
    while (1) {
        /* Squeeze runs of blanks within an arg */
        int OldPendingNewLines = PendingNewLines;
        int Skipped = SkipWhitespace (MultiLine);
        if (MultiLine && CurC == '#') {
            int Newlines = 0;

            while (CurC == '#') {
                Newlines += PendingNewLines - OldPendingNewLines;
                PendingNewLines = OldPendingNewLines;
                OldPendingNewLines = 0;
                Skipped = ParseDirectives (MSM_IN_ARG_LIST) || Skipped;
                Skipped = SkipWhitespace (MultiLine) || Skipped;
            }
            PendingNewLines += Newlines;
        }
        if (Skipped && SB_NotEmpty (&Arg)) {
            SB_AppendChar (&Arg, ' ');
        }
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
            } else if (CurC == ',' && ME_ArgIsVariadic (E, M)) {
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
                if (CurC != ')' || SB_NotEmpty (&Arg) || M->ArgCount > 0) {
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
        } else if (CurC == '\0') {
            /* End of input inside macro argument list */
            PPError ("Unterminated argument list invoking macro '%s'", M->Name);
            MissingParen = 1;
            ClearLine ();
            break;
        } else {
            /* Just copy the character */
            SB_AppendChar (&Arg, CurC);
            NextChar ();
        }
    }

    /* Compare formal and actual argument count */
    if (CollCount (&E->ActualArgs) != (unsigned) M->ArgCount) {

        if (!MissingParen) {
            /* Argument count mismatch */
            PPError ("Macro argument count mismatch");
        }

        /* Be sure to make enough empty arguments available */
        SB_Clear (&Arg);
        while (CollCount (&E->ActualArgs) < (unsigned) M->ArgCount) {
            ME_AppendActual (E, &Arg);
        }
    }

    /* Deallocate string buf resources */
    SB_Done (&Arg);
}



static void SubstMacroArgs (MacroExp* E, Macro* M)
/* Argument substitution according to ISO/IEC 9899:1999 (E), 6.10.3.1ff */
{
    ident       Ident;
    int         ArgIdx;
    StrBuf*     OldSource;
    StrBuf*     Arg;
    int         HaveSpace;


    /* Remember the current input and switch to the macro replacement. */
    int OldIndex = SB_GetIndex (&M->Replacement);
    SB_Reset (&M->Replacement);
    OldSource = InitLine (&M->Replacement);

    /* Argument handling loop */
    while (CurC != '\0') {

        /* If we have an identifier, check if it's a macro */
        if (IsSym (Ident)) {

            /* Check if it's a macro argument */
            if ((ArgIdx = FindMacroArg (M, Ident)) >= 0) {

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
                    MacroReplacement (Arg, &E->Replacement, 0);

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
                if ((ArgIdx = FindMacroArg (M, Ident)) >= 0) {

                    /* Get the corresponding actual argument and add it. */
                    SB_Append (&E->Replacement, ME_GetActual (E, ArgIdx));

                } else {

                    /* Just an ordinary identifier - add as is */
                    SB_AppendStr (&E->Replacement, Ident);

                }
            }

        } else if (CurC == '#' && M->ArgCount >= 0) {

            /* A # operator within a macro expansion of a function like
            ** macro. Read the following identifier and check if it's a
            ** macro parameter.
            */
            NextChar ();
            SkipWhitespace (0);
            if (!IsSym (Ident) || (ArgIdx = FindMacroArg (M, Ident)) < 0) {
                PPError ("'#' is not followed by a macro parameter");
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
    SB_SetIndex (&M->Replacement, OldIndex);
}



static void ExpandMacro (StrBuf* Target, Macro* M, int MultiLine)
/* Expand a macro into Target */
{
    MacroExp E;

#if 0
    static unsigned V = 0;
    printf ("Expanding %s(%u)\n", M->Name, ++V);
#endif

    /* Initialize our MacroExp structure */
    InitMacroExp (&E);

    /* Check if this is a function like macro */
    if (M->ArgCount >= 0) {

        /* Read the actual macro arguments (with the enclosing parentheses) */
        ReadMacroArgs (&E, M, MultiLine);

    }

    /* Replace macro arguments handling the # and ## operators */
    SubstMacroArgs (&E, M);

    /* Forbide repeated expansion of the same macro in use */
    M->Expanding = 1;
    MacroReplacement (&E.Replacement, Target, 0);
    M->Expanding = 0;

#if 0
    printf ("Done with %s(%u)\n", E.M->Name, V--);
#endif

    /* Free memory allocated for the macro expansion structure */
    DoneMacroExp (&E);
}



static void MacroReplacement (StrBuf* Source, StrBuf* Target, unsigned ModeFlags)
/* Scan for and perform macro replacement */
{
    ident       Ident;

    /* Remember the current input and switch to Source */
    StrBuf* OldSource = InitLine (Source);

    /* Loop substituting macros */
    while (CurC != '\0') {
        /* If we have an identifier, check if it's a macro */
        if (IsSym (Ident)) {
            if ((ModeFlags & MSM_OP_DEFINED) != 0 && strcmp (Ident, "defined") == 0) {
                /* Handle the "defined" operator */
                int HaveParen = 0;

                SkipWhitespace (0);
                if (CurC == '(') {
                    HaveParen = 1;
                    NextChar ();
                    SkipWhitespace (0);
                }
                if (IsSym (Ident)) {
                    /* Eat the identifier */
                    SB_AppendChar (Target, IsMacro (Ident) ? '1' : '0');
                    if (HaveParen) {
                        SkipWhitespace (0);
                        if (CurC != ')') {
                            PPError ("')' expected");
                            ClearLine ();
                        } else {
                            NextChar ();
                        }
                    }
                } else {
                    PPError ("Macro name must be an identifier");
                    ClearLine ();
                    SB_AppendChar (Target, '0');
                }
            } else {
                /* Check if it's an expandable macro */
                Macro* M = FindMacro (Ident);
                if (M != 0 && !M->Expanding) {
                    /* Check if this is a function-like macro */
                    if (M->ArgCount >= 0) {
                        int MultiLine  = (ModeFlags & MSM_MULTILINE) != 0;
                        int Whitespace = SkipWhitespace (MultiLine);
                        if (CurC != '(') {
                            /* Function-like macro without an argument list is not replaced */
                            SB_AppendStr (Target, M->Name);
                            if (Whitespace > 0) {
                                SB_AppendChar (Target, ' ');
                            }

                            /* Directives can only be found in an argument list
                            ** that spans multiple lines.
                            */
                            if (MultiLine) {
                                if (CurC == '#') {
                                    /* If we were going to support #pragma in
                                    ** macro argument list, it would be output
                                    ** to OLine.
                                    */
                                    if (OLine == 0) {
                                        OLine = Target;
                                        ParseDirectives (ModeFlags);
                                        OLine = 0;
                                    } else {
                                        ParseDirectives (ModeFlags);
                                    }
                                }
                                /* Add the source info to preprocessor output if needed */
                                AddPreLine (Target);
                            }
                        } else {
                            /* Function-like macro */
                            if (OLine == 0) {
                                OLine = Target;
                                ExpandMacro (Target, M, MultiLine);
                                OLine = 0;
                            } else {
                                ExpandMacro (Target, M, MultiLine);
                            }
                        }
                    } else {
                        /* Object-like macro */
                        ExpandMacro (Target, M, 0);
                    }
                } else {
                    /* An identifier, keep it */
                    SB_AppendStr (Target, Ident);
                }
            }
        } else {
            if ((ModeFlags & MSM_TOK_HEADER) != 0 && (CurC == '<' || CurC == '\"')) {
                CopyHeaderNameToken (Target);
            } else if (IsQuote (CurC)) {
                CopyQuotedString (Target);
            } else {
                int Whitespace = SkipWhitespace (0);
                if (Whitespace) {
                    SB_AppendChar (Target, ' ');
                } else {
                    SB_AppendChar (Target, CurC);
                    NextChar ();
                }
            }
        }
    }

    /* Drop whitespace at the end */
    if (IsBlank (SB_LookAtLast (Target))) {
        SB_Drop (Target, 1);
    }

    /* Switch back the input */
    InitLine (OldSource);
}



/*****************************************************************************/
/*                                Directives                                 */
/*****************************************************************************/



static void DoDefine (void)
/* Process #define directive */
{
    ident       Ident;
    Macro*      M;
    Macro*      Existing;
    int         C89;
    unsigned    Len;

    /* Read the macro name */
    SkipWhitespace (0);
    if (!MacName (Ident)) {
        return;
    }

    /* Remember if we're in C89 mode */
    C89 = (IS_Get (&Standard) == STD_C89);

    /* Check for forbidden macro names */
    if (strcmp (Ident, "defined") == 0) {
        PPError ("'%s' cannot be used as a macro name", Ident);
        return;
    }

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
                    PPError ("'...' expected");
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

                /* __VA_ARGS__ is only allowed in post-C89 mode */
                if (!C89 && strcmp (Ident, "__VA_ARGS__") == 0) {
                    PPWarning ("'__VA_ARGS__' can only appear in the expansion "
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
            PPError ("')' expected");
            ClearLine ();
            return;
        }
        NextChar ();
    }

    /* Skip whitespace before the macro replacement */
    SkipWhitespace (0);

    /* Remove whitespace and comments from the line, store the preprocessed
    ** line into the macro replacement buffer.
    */
    TranslationPhase3 (Line, &M->Replacement);

    /* Remove whitespace from the end of the line */
    while (IsSpace (SB_LookAtLast (&M->Replacement))) {
        SB_Drop (&M->Replacement, 1);
    }
#if 0
    printf ("%s: <%.*s>\n", M->Name, SB_GetLen (&M->Replacement), SB_GetConstBuf (&M->Replacement));
#endif

    /* Check for ## at start or end */
    Len = SB_GetLen (&M->Replacement);
    if (Len >= 2) {
        if (SB_LookAt (&M->Replacement, 0) == '#' &&
            SB_LookAt (&M->Replacement, 1) == '#') {
            /* Diagnose and bail out */
            PPError ("'##' cannot appear at start of macro expansion");
            FreeMacro (M);
            return;
        } else if (SB_LookAt (&M->Replacement, Len - 1) == '#' &&
                   SB_LookAt (&M->Replacement, Len - 2) == '#') {
            /* Diagnose and bail out */
            PPError ("'##' cannot appear at end of macro expansion");
            FreeMacro (M);
            return;
        }
    }

    /* Get an existing macro definition with this name */
    Existing = FindMacro (M->Name);

    /* If we have an existing macro, check if the redefinition is identical.
    ** Print a diagnostic if not.
    */
    if (Existing != 0) {
        if (MacroCmp (M, Existing) != 0) {
            PPError ("Macro redefinition is not identical");
        }
        /* Undefine the existing macro anyways */
        UndefineMacro (Existing->Name);
    }

    /* Insert the new macro into the macro table */
    InsertMacro (M);
}



static int PushIf (int Skip, int Invert, int Cond)
/* Push a new if level onto the if stack */
{
    /* Check for an overflow of the if stack */
    if (PPStack->Index >= MAX_PP_IFS-1) {
        PPError ("Too many nested #if clauses");
        return 1;
    }

    /* Push the #if condition */
    ++PPStack->Index;
    if (Skip) {
        PPStack->Stack[PPStack->Index] = IFCOND_SKIP | IFCOND_NEEDTERM;
        return 1;
    } else {
        PPStack->Stack[PPStack->Index] = IFCOND_NONE | IFCOND_NEEDTERM;
        return (Invert ^ Cond);
    }
}



static void DoError (void)
/* Print an error */
{
    SkipWhitespace (0);
    if (CurC == '\0') {
        PPError ("#error");
    } else {
        StrBuf MsgLine = AUTO_STRBUF_INITIALIZER;
        TranslationPhase3 (Line, &MsgLine);
        PPError ("#error: %s", SB_GetConstBuf (&MsgLine) + SB_GetIndex (&MsgLine));
        SB_Done (&MsgLine);
    }

    /* Clear the rest of line */
    ClearLine ();
}



static int DoIf (int Skip)
/* Process #if directive */
{
    PPExpr Expr = AUTO_PPEXPR_INITIALIZER;

    if (!Skip) {

        /* We're about to use a dedicated expression parser to evaluate the #if
        ** expression. Save the current tokens to come back here later.
        */
        Token SavedCurTok = CurTok;
        Token SavedNextTok = NextTok;

        /* Make sure the line infos for the tokens won't get removed */
        if (SavedCurTok.LI) {
            UseLineInfo (SavedCurTok.LI);
        }
        if (SavedNextTok.LI) {
            UseLineInfo (SavedNextTok.LI);
        }

        /* Macro-replace a single line with support for the "defined" operator */
        SB_Clear (MLine);
        PreprocessDirective (Line, MLine, MSM_OP_DEFINED);

        /* Read from the processed line */
        SB_Reset (MLine);
        MLine = InitLine (MLine);

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
        ParsePPExprInLine (&Expr);

        /* Restore input source */
        MLine = InitLine (MLine);

        /* Reset the old tokens */
        CurTok  = SavedCurTok;
        NextTok = SavedNextTok;
    }

    /* Set the #if condition according to the expression result */
    return PushIf (Skip, 1, Expr.IVal != 0);
}



static int DoIfDef (int skip, int flag)
/* Process #ifdef if flag == 1, or #ifndef if flag == 0. */
{
    int Value = 0;

    if (!skip) {
        ident Ident;

        SkipWhitespace (0);
        if (MacName (Ident)) {
            Value = IsMacro (Ident);
            /* Check for extra tokens */
            CheckExtraTokens (flag ? "ifdef" : "ifndef");
        }
    }

    return PushIf (skip, flag, Value);
}



static void DoInclude (void)
/* Open an include file. */
{
    char        RTerm;
    InputType   IT;
    StrBuf      Filename = AUTO_STRBUF_INITIALIZER;

    /* Macro-replace a single line with special support for <filename> */
    SB_Clear (MLine);
    PreprocessDirective (Line, MLine, MSM_TOK_HEADER);

    /* Read from the processed line */
    SB_Reset (MLine);
    MLine = InitLine (MLine);

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
            PPError ("'\"' or '<' expected");
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
        /* Skip the terminator */
        NextChar ();
        /* Check for extra tokens following the filename */
        CheckExtraTokens ("include");
        /* Open the include file */
        OpenIncludeFile (SB_GetConstBuf (&Filename), IT);
    } else if (CurC == '\0') {
        /* No terminator found */
        PPError ("#include expects \"FILENAME\" or <FILENAME>");
    }

Done:
    /* Free the allocated filename data */
    SB_Done (&Filename);

    /* Restore input source */
    MLine = InitLine (MLine);

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
    StrBuf* PragmaLine = OLine;

    PRECONDITION (PragmaLine != 0);

    /* Add the source info to preprocessor output if needed */
    AddPreLine (PragmaLine);

    /* Macro-replace a single line */
    SB_Clear (MLine);
    PreprocessDirective (Line, MLine, MSM_NONE);

    /* Convert #pragma to _Pragma () */
    SB_AppendStr (PragmaLine, "_Pragma (");
    SB_Reset (MLine);
    Stringize (MLine, PragmaLine);
    SB_AppendChar (PragmaLine, ')');

    /* End this line */
    SB_SetIndex (PragmaLine, SB_GetLen (PragmaLine));
}



static void DoUndef (void)
/* Process the #undef directive */
{
    ident Ident;

    SkipWhitespace (0);
    if (MacName (Ident)) {
        UndefineMacro (Ident);
    }
    /* Check for extra tokens */
    CheckExtraTokens ("undef");
}



static void DoWarning (void)
/* Print a warning */
{
    SkipWhitespace (0);
    if (CurC == '\0') {
        PPWarning ("#warning");
    } else {
        StrBuf MsgLine = AUTO_STRBUF_INITIALIZER;
        TranslationPhase3 (Line, &MsgLine);
        PPWarning ("#warning: %s", SB_GetConstBuf (&MsgLine) + SB_GetIndex (&MsgLine));
        SB_Done (&MsgLine);
    }

    /* Clear the rest of line */
    ClearLine ();
}



static int ParseDirectives (unsigned ModeFlags)
/* Handle directives. Return 1 if any whitespace or newlines are parsed. */
{
    int         PPSkip = 0;
    ident       Directive;

    /* Skip white space at the beginning of the first line */
    int Whitespace = SkipWhitespace (0);

    /* Check for stuff to skip */
    while (CurC == '\0' || CurC == '#' || PPSkip) {

        /* Check for preprocessor lines lines */
        if (CurC == '#') {
            NextChar ();
            SkipWhitespace (0);
            if (CurC == '\0') {
                /* Ignore the empty preprocessor directive */
                continue;
            }
            if (!IsSym (Directive)) {
                if (!PPSkip) {
                    PPError ("Preprocessor directive expected");
                }
                ClearLine ();
            } else {
                switch (FindPPDirectiveType (Directive)) {

                    case PPD_DEFINE:
                        if (!PPSkip) {
                            DoDefine ();
                        }
                        break;

                    case PPD_ELIF:
                        if (PPStack->Index >= 0) {
                            if ((PPStack->Stack[PPStack->Index] & IFCOND_ELSE) == 0) {
                                /* Handle as #else/#if combination */
                                if ((PPStack->Stack[PPStack->Index] & IFCOND_SKIP) == 0) {
                                    PPSkip = !PPSkip;
                                }
                                PPStack->Stack[PPStack->Index] |= IFCOND_ELSE;
                                PPSkip = DoIf (PPSkip);

                                /* #elif doesn't need a terminator */
                                PPStack->Stack[PPStack->Index] &= ~IFCOND_NEEDTERM;
                            } else {
                                PPError ("Duplicate #else/#elif");
                            }
                        } else {
                            PPError ("Unexpected #elif");
                        }
                        break;

                    case PPD_ELSE:
                        if (PPStack->Index >= 0) {
                            if ((PPStack->Stack[PPStack->Index] & IFCOND_ELSE) == 0) {
                                if ((PPStack->Stack[PPStack->Index] & IFCOND_SKIP) == 0) {
                                    PPSkip = !PPSkip;
                                }
                                PPStack->Stack[PPStack->Index] |= IFCOND_ELSE;

                                /* Check for extra tokens */
                                CheckExtraTokens ("else");
                            } else {
                                PPError ("Duplicate #else");
                            }
                        } else {
                            PPError ("Unexpected '#else'");
                        }
                        break;

                    case PPD_ENDIF:
                        if (PPStack->Index >= 0) {
                            /* Remove any clauses on top of stack that do not
                            ** need a terminating #endif.
                            */
                            while (PPStack->Index >= 0 &&
                                   (PPStack->Stack[PPStack->Index] & IFCOND_NEEDTERM) == 0) {
                                --PPStack->Index;
                            }

                            /* Stack may not be empty here or something is wrong */
                            CHECK (PPStack->Index >= 0);

                            /* Remove the clause that needs a terminator */
                            PPSkip = (PPStack->Stack[PPStack->Index--] & IFCOND_SKIP) != 0;

                            /* Check for extra tokens */
                            CheckExtraTokens ("endif");
                        } else {
                            PPError ("Unexpected '#endif'");
                        }
                        break;

                    case PPD_ERROR:
                        if (!PPSkip) {
                            DoError ();
                        }
                        break;

                    case PPD_IF:
                        PPSkip = DoIf (PPSkip);
                        break;

                    case PPD_IFDEF:
                        PPSkip = DoIfDef (PPSkip, 1);
                        break;

                    case PPD_IFNDEF:
                        PPSkip = DoIfDef (PPSkip, 0);
                        break;

                    case PPD_INCLUDE:
                        if (!PPSkip) {
                            DoInclude ();
                        }
                        break;

                    case PPD_LINE:
                        /* Should do something in C99 at least, but we ignore it */
                        if (!PPSkip) {
                            ClearLine ();
                        }
                        break;

                    case PPD_PRAGMA:
                        if (!PPSkip) {
                            if ((ModeFlags & MSM_IN_ARG_LIST) == 0) {
                                DoPragma ();
                            } else {
                                PPError ("Embedded #pragma directive within macro arguments is unsupported");
                            }
                        }
                        break;

                    case PPD_UNDEF:
                        if (!PPSkip) {
                            DoUndef ();
                        }
                        break;

                    case PPD_WARNING:
                        /* #warning is a non standard extension */
                        if (IS_Get (&Standard) > STD_C99) {
                            if (!PPSkip) {
                                DoWarning ();
                            }
                        } else {
                            if (!PPSkip) {
                                PPError ("Preprocessor directive expected");
                            }
                            ClearLine ();
                        }
                        break;

                    default:
                        if (!PPSkip) {
                            PPError ("Preprocessor directive expected");
                        }
                        ClearLine ();
                }
            }

        }
        if (NextLine () == 0) {
            break;
        }
        ++PendingNewLines;
        Whitespace = SkipWhitespace (0) || Whitespace;
    }

    return Whitespace != 0;
}



/*****************************************************************************/
/*                               Preprocessing                               */
/*****************************************************************************/



static void TranslationPhase3 (StrBuf* Source, StrBuf* Target)
/* Mimic Translation Phase 3. Handle old and new style comments. Collapse
** non-newline whitespace sequences.
*/
{
    /* Switch to the new input source */
    StrBuf* OldSource = InitLine (Source);

    /* Loop removing ws and comments */
    while (CurC != '\0') {
        int HasWhiteSpace = 0;
        while (1) {
            /* Squeeze runs of blanks */
            if (IsSpace (CurC)) {
                NextChar ();
                HasWhiteSpace = 1;
            } else if (CurC == '/' && NextC == '*') {
                OldStyleComment ();
                HasWhiteSpace = 1;
            } else if (IS_Get (&Standard) >= STD_C99 && CurC == '/' && NextC == '/') {
                NewStyleComment ();
                HasWhiteSpace = 1;
            } else {
                /* No more white space */
                break;
            }
        }
        if (HasWhiteSpace) {
            SB_AppendChar (Target, ' ');
        } else if (IsQuote (CurC)) {
            CopyQuotedString (Target);
        } else {
            SB_AppendChar (Target, CurC);
            NextChar ();
        }
    }

    /* Terminate the new input line */
    SB_Terminate (Target);

    /* Switch back to the old source */
    InitLine (OldSource);
}



static void PreprocessDirective (StrBuf* Source, StrBuf* Target, unsigned ModeFlags)
/* Preprocess a single line. Handle specified tokens and operators, remove
** whitespace and comments, then do macro replacement.
*/
{
    int         OldIndex = SB_GetIndex (Source);
    MacroExp    E;

    SkipWhitespace (0);
    InitMacroExp (&E);
    MacroReplacement (Source, Target, ModeFlags | MSM_IN_DIRECTIVE);
    DoneMacroExp (&E);

    /* Restore the source input index */
    SB_SetIndex (Source, OldIndex);
}



void Preprocess (void)
/* Preprocess lines count of which is affected by directives */
{
    SB_Clear (PLine);

    /* Add the source info to preprocessor output if needed */
    AddPreLine (PLine);

    /* Parse any directives */
    OLine = PLine;
    ParseDirectives (MSM_MULTILINE);
    OLine = 0;

    /* Add the source info to preprocessor output if needed */
    AddPreLine (PLine);

    /* Add leading whitespace to prettify preprocessor output */
    AppendIndent (PLine, SB_GetIndex (Line));

    /* Expand macros if any */
    MacroReplacement (Line, PLine, MSM_MULTILINE);

    /* Add the source info to preprocessor output if needed */
    AddPreLine (PLine);

    /* Read from the new line */
    SB_Reset (PLine);
    PLine = InitLine (PLine);

    if (Verbosity > 1 && SB_NotEmpty (Line)) {
        printf ("%s:%u: %.*s\n", GetCurrentFile (), GetCurrentLine (),
                (int) SB_GetLen (Line), SB_GetConstBuf (Line));
    }

    /* Free all undefined macros */
    FreeUndefinedMacros ();
}



void InitPreprocess (void)
/* Init preprocessor */
{
    /* Create the output buffers */
    MLine = NewStrBuf ();
    PLine = NewStrBuf ();
}



void DonePreprocess (void)
/* Done with preprocessor */
{
    /* Done with the output buffers */
    SB_Done (MLine);
    SB_Done (PLine);
}



void SetPPIfStack (PPIfStack* Stack)
/* Specify which PP #if stack to use */
{
    PPStack = Stack;
}



void PreprocessBegin (void)
/* Initialize preprocessor with current file */
{
    /* Reset #if depth */
    PPStack->Index = -1;

    /* Remember to update source file location in preprocess-only mode */
    FileChanged = 1;
}



void PreprocessEnd (void)
/* Preprocessor done with current file */
{
    /* Check for missing #endif */
    while (PPStack->Index >= 0) {
        if ((PPStack->Stack[PPStack->Index] & IFCOND_NEEDTERM) != 0) {
            PPError ("#endif expected");
        }
        --PPStack->Index;
    }

    /* Remember to update source file location in preprocess-only mode */
    FileChanged = 1;
}
