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
#include "scanstrbuf.h"
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
#define MSM_OP_DEFINED          0x10U   /* Handle the "defined" operator */
#define MSM_OP_HAS_INCLUDE      0x20U   /* Handle the "__has_include" operator */
#define MSM_OP_HAS_C_ATTRIBUTE  0x40U   /* Handle the "__has_c_attribute" operator */
#define MSM_TOK_HEADER          0x80U   /* Support header tokens */

/* Macro expansion state flags */
#define MES_NONE                0x00U   /* Nothing */
#define MES_FIRST_TOKEN         0x01U   /* Mark for detecting pp-token count in the sequence */
#define MES_MULTIPLE_TOKEN      0x02U   /* Multiple pp-tokens are detected in the sequence */
#define MES_BEGIN_WITH_IDENT    0x04U   /* The first pp-token of the sequence is an identifier */
#define MES_HAS_REPLACEMENT     0x10U   /* Macro argument has cached replacement result */
#define MES_NO_VA_COMMA         0x20U   /* Variadic macro called w/o the ',' in front of variable argument */
#define MES_ERROR               0x80U   /* Error has occurred in macro expansion */

/* Management data for #if */
#define IFCOND_NONE     0x00U
#define IFCOND_SKIP     0x01U
#define IFCOND_ELSE     0x02U
#define IFCOND_NEEDTERM 0x04U

/* Current PP if stack */
static PPIfStack* PPStack;

/* Struct for rescan */
typedef struct RescanInputStack RescanInputStack;
struct RescanInputStack {
    Collection  Lines;
    Collection  LastTokLens;
    StrBuf*     PrevTok;
};

/* Input backup for rescan */
static RescanInputStack* CurRescanStack;

/* Intermediate input buffers */
static StrBuf* PLine;   /* Buffer for macro expansion */
static StrBuf* MLine;   /* Buffer for macro expansion in #pragma */
static StrBuf* OLine;   /* Buffer for #pragma output */

/* Newlines to be added to preprocessed text */
static unsigned PendingNewLines;
static unsigned ContinuedLines;
static int FileChanged;

/* Structure used when expanding macros */
typedef struct MacroExp MacroExp;
struct MacroExp {
    Collection  Args;               /* Actual arguments (for function-like) */
    Collection  HideSets;           /* Macros hidden from expansion */
    StrBuf      Tokens;             /* Originally read sequence */
    unsigned    IdentCount;         /* Count of identifiers in the pp-token sequence */
    unsigned    Flags;              /* Macro argument flags */
    MacroExp*   Replaced;           /* Macro-replaced version of this pp-token sequence */
    unsigned    FirstTokLen;        /* Length of the first pp-token */
    unsigned    LastTokLen;         /* Length of the last pp-token */
};

typedef struct HideRange HideRange;
struct HideRange
{
    HideRange*  Next;
    unsigned    Start;
    unsigned    End;
};

typedef struct HiddenMacro HiddenMacro;
struct HiddenMacro {
    const Macro*    M;
    HideRange*      HS;
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

static unsigned ReplaceMacros (StrBuf* Source, StrBuf* Target, MacroExp* E, unsigned ModeFlags);
/* Scan for and perform macro replacement. Return the count of identifiers and
** right parentheses in the replacement result.
*/

static MacroExp* InitMacroExp (MacroExp* E);
/* Initialize a MacroExp structure */

static void DoneMacroExp (MacroExp* E);
/* Cleanup after use of a MacroExp structure */

static int CheckPastePPTok (StrBuf* Target, unsigned TokLen, char Next);
/* Return 1 if the last pp-tokens from Source could be concatenated with any
** characters from Appended to form a new valid one.
*/

static void LazyCheckNextPPTok (const StrBuf* Prev, unsigned LastTokLen);
/* Memorize the previous pp-token(s) to later check for potential pp-token
** concatenation.
*/



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
/*                             MacroExp helpers                              */
/*****************************************************************************/



static HideRange* NewHideRange (unsigned Start, unsigned Len)
/* Create a hide range */
{
    HideRange* HS = xmalloc (sizeof (HideRange));

    HS->Next  = 0;
    HS->Start = Start;
    HS->End   = Start + Len;

    return HS;
}



static void FreeHideRange (HideRange* HS)
/* Free a hide range */
{
    xfree (HS);
}



static HiddenMacro* NewHiddenMacro (const Macro* M)
/* Create a new struct for the hidden macro */
{
    HiddenMacro* MHS = xmalloc (sizeof (HiddenMacro));

    MHS->M  = M;
    MHS->HS = 0;

    return MHS;
}



static void FreeHiddenMacro (HiddenMacro* MHS)
/* Free the struct and all ranges of the hidden macro */
{
    HideRange* This;
    HideRange* Next;

    if (MHS == 0) {
        return;
    }

    for (This = MHS->HS; This != 0; This = Next) {
        Next = This->Next;
        FreeHideRange (This);
    }

    xfree (MHS);
}



/*****************************************************************************/
/*                              struct MacroExp                              */
/*****************************************************************************/



static HiddenMacro* ME_FindHiddenMacro (const MacroExp* E, const Macro* M)
/* Find the macro hide set */
{
    unsigned I;

    for (I = 0; I < CollCount (&E->HideSets); ++I) {
        HiddenMacro* MHS = CollAtUnchecked (&E->HideSets, I);
        if (MHS->M == M) {
            return MHS;
        }
    }

    return 0;
}



static void ME_HideMacro (unsigned Idx, unsigned Count, MacroExp* E, const Macro* M)
/* Hide the macro from the Idx'th identifier */
{
    if (Count > 0) {
        /* Find the macro hideset */
        HiddenMacro*    MHS = ME_FindHiddenMacro (E, M);
        HideRange**     This;

        /* New hidden section */
        HideRange*      NewHS = NewHideRange (Idx, Count);

        /* New macro to hide */
        if (MHS == 0) {
            MHS = NewHiddenMacro (M);
            CollAppend (&E->HideSets, MHS);
        }
        This = &MHS->HS;

        if (*This == 0) {
            *This = NewHS;
        } else {
            /* Insert */
            while (1) {
                if (*This == 0 || NewHS->Start <= (*This)->Start) {
                    /* Insert before */
                    NewHS->Next = *This;
                    *This = NewHS;
                    break;
                } else if (NewHS->Start <= (*This)->End) {
                    /* Insert after */
                    NewHS->Next = (*This)->Next;
                    (*This)->Next = NewHS;
                    break;
                }
                /* Advance */
                This = &(*This)->Next;
            }

            /* Merge */
            while (*This != 0) {
                HideRange* Next = (*This)->Next;

                if (Next != 0 && (*This)->End >= Next->Start) {
                    /* Expand this to the next */
                    if ((*This)->End < Next->End) {
                        (*This)->End = Next->End;
                    }

                    /* Remove next */
                    (*This)->Next = Next->Next;
                    FreeHideRange (Next);

                    /* Advance */
                    This = &(*This)->Next;
                } else {
                    /* No more */
                    break;
                }
            }
        }
    }
}



static int ME_CanExpand (unsigned Idx, const MacroExp* E, const Macro* M)
/* Return 1 if the macro can be expanded with the Idx'th identifier */
{
    if (E != 0) {
        /* Find the macro hideset */
        HiddenMacro* MHS = ME_FindHiddenMacro (E, M);
        if (MHS != 0) {
            /* Check if the macro is hidden from this identifier */
            HideRange* HS = MHS->HS;
            while (HS != 0) {
                /* If the macro name overlaps with the range where the macro is hidden,
                ** the macro cannot be expanded.
                */
                if (Idx >= HS->Start && Idx < HS->End) {
                    return 0;
                }
                HS = HS->Next;
            }
        }
    }

    return 1;
}



static void ME_OffsetHideSets (unsigned Idx, unsigned Offs, MacroExp* E)
/* Adjust all macro hide set ranges for the macro expansion when the identifier
** at Idx is replaced with a count of Offs + 1 (if Offs > 0) of identifiers.
*/
{
    if (Offs != 0) {
        unsigned I;

        for (I = 0; I < CollCount (&E->HideSets); ++I) {
            HiddenMacro* MHS = CollAtUnchecked (&E->HideSets, I);
            HideRange* This;

            for (This = MHS->HS; This != 0; This = This->Next) {
                if (Idx < This->Start) {
                    This->Start += Offs;
                    This->End   += Offs;
                } else if (Idx < This->End) {
                    This->End   += Offs;
                }
            }
        }
    }
}



static void ME_RemoveToken (unsigned Idx, unsigned Count, MacroExp* E)
/* Remove the Idx'th identifier token from tracking and offset all hidden
** ranges accordingly.
*/
{
    unsigned I;

    for (I = 0; I < CollCount (&E->HideSets); ++I) {
        HiddenMacro* MHS = CollAtUnchecked (&E->HideSets, I);
        HideRange* This;
        HideRange** Prev;

        for (Prev = &MHS->HS, This = *Prev; This != 0; This = *Prev) {
            if (Idx < This->Start) {
                if (This->Start - Idx >= Count) {
                    This->Start -= Count;
                    This->End   -= Count;
                } else {
                    if (This->End - Idx > Count) {
                        This->Start = Idx;
                        This->End  -= Count;
                    } else {
                        /* Remove */
                        (*Prev) = This->Next;
                        FreeHideRange (This);
                        continue;
                    }
                }
            } else if (Idx < This->End) {
                if (This->End - Idx > Count) {
                    This->End -= Count;
                } else {
                    This->End = Idx;
                }

                if (This->End == This->Start) {
                    /* Remove */
                    (*Prev) = This->Next;
                    FreeHideRange (This);
                    continue;
                }
            }

            Prev = &This->Next;
        }
    }
}



static void ME_HandleSemiNestedMacro (unsigned NameIdx, unsigned LastIdx, MacroExp* E)
/* Unhide the macro name from all hidesets if it was expanded with an unhidden
** right parenthesis. This is unspecified but allowed behavior according to
** ISO/IEC 9899:2018, 6.10.3.4ff.
*/
{
    unsigned I;

    for (I = 0; I < CollCount (&E->HideSets); ++I) {
        HiddenMacro* MHS = CollAtUnchecked (&E->HideSets, I);
        HideRange* This;
        HideRange** Prev;

        for (Prev = &MHS->HS, This = *Prev; This != 0; This = *Prev) {
            if (NameIdx < This->End) {
                if (NameIdx >= This->Start && LastIdx >= This->End) {
                    This->End = NameIdx;
                    if (This->End == This->Start) {
                        /* Remove */
                        (*Prev) = This->Next;
                        FreeHideRange (This);
                        continue;
                    }
                }
                Prev = &This->Next;
            } else {
                break;
            }
        }
    }
}



static void ME_AddArgHideSets (unsigned Idx, const MacroExp* A, MacroExp* Parent)
/* Propagate the macro hide sets of the substituted argument starting as the
** Idx'th identifier of the result.
*/
{
    unsigned I;

    /* Move the hide set generated with in the argument as it will be freed later */
    for (I = 0; I < CollCount (&A->HideSets); ++I) {
        HiddenMacro* MHS = CollAtUnchecked (&A->HideSets, I);
        HideRange* HS;

        for (HS = MHS->HS; HS != 0; HS = HS->Next) {
            ME_HideMacro (Idx + HS->Start, HS->End - HS->Start, Parent, MHS->M);
        }
    }
}



static void ME_DoneHideSets (MacroExp* E)
/* Free all of hidden macros for the macro expansion */
{
    unsigned I;

    for (I = 0; I < CollCount (&E->HideSets); ++I) {
        HiddenMacro* MHS = CollAtUnchecked (&E->HideSets, I);
        FreeHiddenMacro (MHS);
    }
    DoneCollection (&E->HideSets);
}



static void ME_SetTokLens (MacroExp* E, unsigned TokLen)
/* Set token lengths and flags for macro expansion struct */
{
    E->LastTokLen = TokLen;
    if ((E->Flags & MES_FIRST_TOKEN) != 0) {
        E->Flags &= ~MES_FIRST_TOKEN;
        E->FirstTokLen = E->LastTokLen;
    } else {
        E->Flags |= MES_MULTIPLE_TOKEN;
    }
}



static MacroExp* ME_MakeReplaced (MacroExp* A)
/* Make a replaced version of the argument */
{
    /* Replace the parameter with actual argument tokens */
    if ((A->Flags & MES_HAS_REPLACEMENT) == 0) {
        A->Replaced = xmalloc (sizeof (MacroExp));

        InitMacroExp (A->Replaced);
        SB_Reset (&A->Tokens);

        /* Propagate the hide sets */
        ME_AddArgHideSets (0, A, A->Replaced);

        /* Do macro expansion on the argument */
        A->Replaced->IdentCount = ReplaceMacros (&A->Tokens,
                                                 &A->Replaced->Tokens,
                                                 A->Replaced,
                                                 MSM_IN_ARG_EXPANSION);

        A->Flags |= MES_HAS_REPLACEMENT;
    }

    return A->Replaced != 0 ? A->Replaced : A;
}



static MacroExp* ME_GetOriginalArg (const MacroExp* E, unsigned Index)
/* Return an actual macro argument with the given index */
{
    return CollAt (&E->Args, Index);
}



static MacroExp* ME_GetReplacedArg (const MacroExp* E, unsigned Index)
/* Return a replaced macro argument with the given index */
{
    return ME_MakeReplaced (CollAt (&E->Args, Index));
}



static MacroExp* ME_AppendArg (MacroExp* E, MacroExp* Arg)
/* Add a copy of Arg to the list of actual macro arguments.
** NOTE: This function will clear the token sequence of Arg!
*/
{
    MacroExp* A = xmalloc (sizeof (MacroExp));

    /* Initialize our MacroExp structure */
    InitMacroExp (A);

    /* Copy info about the original strings */
    A->IdentCount  = Arg->IdentCount;
    A->Flags       = Arg->Flags;
    A->FirstTokLen = Arg->FirstTokLen;
    A->LastTokLen  = Arg->LastTokLen;

    /* Move the contents of Arg to A */
    SB_Move (&A->Tokens, &Arg->Tokens);

    /* Add A to the list of actual arguments */
    CollAppend (&E->Args, A);

    return A;
}



static void ME_ClearArgs (MacroExp* E)
/* Clear all read arguments for macro expansion */
{
    unsigned I;

    /* Delete the list with actual arguments */
    for (I = 0; I < CollCount (&E->Args); ++I) {
        MacroExp* A = CollAtUnchecked (&E->Args, I);

        /* Destroy the macro expansion structure and then free memory allocated
        ** for it.
        */
        DoneMacroExp (A);
        xfree (A);
    }

    DoneCollection (&E->Args);
    InitCollection (&E->Args);
}



static int ME_IsNextArgVariadic (const MacroExp* E, const Macro* M)
/* Return true if the next actual argument we will add is a variadic one */
{
    return (M->Variadic &&
            M->ParamCount == (int) CollCount (&E->Args) + 1);
}



static MacroExp* InitMacroExp (MacroExp* E)
/* Initialize a MacroExp structure */
{
    InitCollection (&E->Args);
    InitCollection (&E->HideSets);
    SB_Init (&E->Tokens);
    E->IdentCount   = 0;
    E->Flags        = MES_FIRST_TOKEN;
    E->Replaced     = 0;
    E->FirstTokLen  = 0;
    E->LastTokLen   = 0;
    return E;
}



static void DoneMacroExp (MacroExp* E)
/* Cleanup after use of a MacroExp structure */
{
    ME_ClearArgs (E);
    ME_DoneHideSets (E);
    SB_Done (&E->Tokens);
    if (E->Replaced != 0) {
        DoneMacroExp (E->Replaced);
    }
}



/*****************************************************************************/
/*                            Rescan input stack                             */
/*****************************************************************************/



static void PushRescanLine (RescanInputStack* RIS, StrBuf* L, unsigned LastTokLen)
/* Push an input line to the rescan input stack */
{
    CollAppend (&RIS->Lines, L);
    /* Abuse the pointer to store an unsigned */
    CollAppend (&RIS->LastTokLens, (void*)(uintptr_t)LastTokLen);
}



static void PopRescanLine (void)
/* Pop and free a rescan input line if it reaches the end */
{
    if (CurC == '\0' && CollCount (&CurRescanStack->Lines) > 1) {
        FreeStrBuf (CollPop (&CurRescanStack->Lines));
        InitLine (CollLast (&CurRescanStack->Lines));
        CollPop (&CurRescanStack->LastTokLens);
    }
}



static void InitRescanInputStack (RescanInputStack* RIS)
/* Init a RescanInputStack struct */
{
    InitCollection (&RIS->Lines);
    InitCollection (&RIS->LastTokLens);
    RIS->PrevTok = 0;
}



static void DoneRescanInputStack (RescanInputStack* RIS)
/* Free a RescanInputStack struct. RIS must be non-NULL. */
{
    /* Free pushed input lines */
    while (CollCount (&RIS->Lines) > 1) {
        FreeStrBuf (CollPop (&RIS->Lines));
    }
    /* Switch back to the old input stack */
    InitLine (CollPop (&RIS->Lines));

    /* Free any remaining pp-tokens used for concatenation check */
    FreeStrBuf (RIS->PrevTok);

    /* Done */
    DoneCollection (&RIS->Lines);
    DoneCollection (&RIS->LastTokLens);
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



static void CheckForBadIdent (const char* Ident, int Std, const Macro* M)
/* Check for and warning on problematic identifiers */
{
    if (Std >= STD_C99              &&
        (M == 0 || !M->Variadic)    &&
        strcmp (Ident, "__VA_ARGS__") == 0) {
        /* __VA_ARGS__ cannot be used as a macro parameter name in post-C89
        ** mode.
        */
        PPWarning ("__VA_ARGS__ can only appear in the expansion of a C99 variadic macro");
    }
}



static void AddPreLine (StrBuf* Str)
/* Add newlines to the string buffer */
{
    /* No need to prettify the non-exist output */
    if (!PreprocessOnly) {
        PendingNewLines = 0;
        ContinuedLines = 0;
        return;
    }

    /* We'll adjust the line number later if necessary */
    PendingNewLines += ContinuedLines;

    if (FileChanged || PendingNewLines > 6) {
        /* Output #line directives as source info */
        StrBuf Comment = AUTO_STRBUF_INITIALIZER;
        if (SB_NotEmpty (Str) && SB_LookAtLast (Str) != '\n') {
            SB_AppendChar (Str, '\n');
        }
        SB_Printf (&Comment, "#line %u \"%s\"\n",
                   GetCurrentLineNum () - ContinuedLines, GetCurrentFileName ());
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
    ContinuedLines = 0;
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
/* Remove an old style C comment from line */
{
    /* Remember the current line number, so we can output better error
    ** messages if the comment is not terminated in the current file.
    */
    unsigned StartingLine = GetCurrentLineNum ();

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
            ++PendingNewLines;
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
/* Remove a new style C comment from line */
{
    /* Diagnose if this is unsupported */
    if (IS_Get (&Standard) < STD_C99 && !AllowNewComments) {
        PPError ("C++ style comments are not allowed in C89");
        PPNote ("(this will be reported only once per input file)");
        AllowNewComments = 1;
    }

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

    /* Rescanning */
    while (1) {
        if (IsSpace (CurC)) {
            NextChar ();
            Skipped = 1;
        } else if (CurC == '/' && NextC == '*') {
            OldStyleComment ();
            Skipped = 1;
        } else if (CurC == '/' && NextC == '/') {
            NewStyleComment ();
            Skipped = 1;
        } else if (CurC == '\0') {
            /* End of line */
            if (CurRescanStack != 0 &&
                CollCount (&CurRescanStack->Lines) > 1 &&
                Line == CollLast (&CurRescanStack->Lines)) {

                unsigned LastTokLen = (unsigned)(uintptr_t)CollLast (&CurRescanStack->LastTokLens);

                /* Check for potentially merged tokens */
                if (Skipped == 0 && LastTokLen != 0) {
                    /* Get the following input */
                    StrBuf* Next = CollAtUnchecked (&CurRescanStack->Lines,
                                                    CollCount (&CurRescanStack->Lines) - 2);
                    char    C    = SB_Peek (Next);

                    /* We cannot check right now if the next pp-token may be a
                    ** macro.
                    */
                    if (IsIdent (C)) {
                        /* Memorize the previous pp-token and check it later */
                        LazyCheckNextPPTok (Line, LastTokLen);
                    } else if (C != '\0' && !IsSpace (C)) {
                        /* If the two adjacent pp-tokens could be put together
                        ** to form a new one, we have to separate them with an
                        ** additional space.
                        */
                        Skipped = CheckPastePPTok (Line, LastTokLen, SB_Peek (Next));
                    }

                }

                /* switch back to previous input */
                PopRescanLine ();

            } else if (SkipLines) {
                /* Read next line */
                if (NextLine () != 0) {
                    ++PendingNewLines;
                    NewLine = 1;
                    Skipped = 0;
                } else {
                    /* End of input */
                    break;
                }
            } else {
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



static int IsQuotedString (void)
/* Retrun 1 if the incoming characters indicate a string literal or character
** constant, otherwise return 0.
*/
{
    return IsQuote (CurC) || IsWideQuoted (CurC, NextC);
}



static void CopyQuotedString (StrBuf* Target)
/* Copy a single or double quoted string from the input to Target. */
{
    /* Remember the quote character, copy it to the target buffer and skip it */
    char Quote;

    if (CurC == 'L') {
        SB_AppendChar (Target, CurC);
        NextChar ();
    }

    Quote = CurC;
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



static int GetPunc (char* S)
/* Parse a punctuator token. Return 1 and store the parsed token string into S
** on success, otherwise just return 0.
*/
{
    char C;
    switch (CurC) {

        case '[':
        case ']':
        case '(':
        case ')':
        case '{':
        case '}':
        case '~':
        case '?':
        case ':':
        case ';':
        case ',':
            /* C */
            *S++ = CurC;
            NextChar ();
            break;

        case '=':
        case '#':
            /* C or CC */
            C = *S++ = CurC;
            NextChar ();
            if (CurC == C) {
                *S++ = C;
                NextChar ();
            }
            break;

        case '*':
        case '/':
        case '%':
        case '^':
        case '!':
            /* C or C= */
            *S++ = CurC;
            NextChar ();
            if (CurC == '=') {
                *S++ = CurC;
                NextChar ();
            }
            break;

        case '+':
        case '&':
        case '|':
            /* C, CC or C= */
            C = *S++ = CurC;
            NextChar ();
            if (CurC == C || CurC == '=') {
                *S++ = CurC;
                NextChar ();
            }
            break;

        case '<':
        case '>':
            /* C, CC, C= or CC= */
            C = *S++ = CurC;
            NextChar ();
            if (CurC == C) {
                *S++ = CurC;
                if (NextC == '=') {
                    *S++ = NextC;
                    NextChar ();
                }
                NextChar ();
            } else if (CurC == '=') {
                *S++ = CurC;
                NextChar ();
            }
            break;

        case '-':
            /* C, CC, C= or C> */
            *S++ = CurC;
            NextChar ();
            switch (CurC) {
                case '-':
                case '=':
                case '>':
                    *S++ = CurC;
                    NextChar ();
                    break;
                default:
                    break;
            }
            break;

        case '.':
            /* C or CCC */
            *S++ = CurC;
            NextChar ();
            if (CurC == '.' && NextC == '.') {
                *S++ = CurC;
                *S++ = NextC;
                NextChar ();
                NextChar ();
            }
            break;

        default:
            return 0;

    }

    *S = '\0';
    return 1;
}



static int CheckPastePPTok (StrBuf* Source, unsigned TokLen, char Next)
/* Return 1 if the last pp-tokens from Source could be concatenated with any
** characters from Appended to form a new valid one.
*/
{
    char        C;
    unsigned    NewTokLen;
    StrBuf*     OldSource;
    StrBuf      Src = AUTO_STRBUF_INITIALIZER;
    StrBuf      Buf = AUTO_STRBUF_INITIALIZER;

    if (TokLen == 0 || IsBlank (SB_LookAtLast (Source))) {
        return 0;
    }

    PRECONDITION (SB_GetLen (Source) >= TokLen);

    /* Special casing "..", "/ /" and "/ *" that are not pp-tokens but still
    ** need be separated.
    */
    C = SB_LookAt (Source, SB_GetLen (Source) - TokLen);
    if ((C == '.' && Next == '.') || (C == '/' && (Next == '/' || Next == '*'))) {
        return 1;
    }

    SB_CopyBuf (&Src, SB_GetConstBuf (Source) + SB_GetLen (Source) - TokLen, TokLen);
    SB_AppendChar (&Src, Next);

    SB_Reset (&Src);
    OldSource = InitLine (&Src);

    if (IsPPNumber (CurC, NextC)) {
        /* PP-number */
        CopyPPNumber (&Buf);
    } else if (IsQuotedString ()) {
        /* Quoted string */
        CopyQuotedString (&Buf);
    } else {
        ident Ident;
        if (GetPunc (Ident)) {
            /* Punctuator */
            SB_CopyStr (&Buf, Ident);
        } else if (IsSym (Ident)) {
            /* Identifier */
            SB_CopyStr (&Buf, Ident);
        }
    }

    NewTokLen = SB_GetLen (&Buf);

    SB_Done (&Buf);
    SB_Done (&Src);

    /* Restore old source */
    InitLine (OldSource);

    /* Return if concatenation succeeded */
    return NewTokLen != TokLen;
}



static int TryPastePPTok (StrBuf* Target,
                          StrBuf* Appended,
                          unsigned FirstTokLen,
                          unsigned SecondTokLen)
/* Paste the whole appened pp-token sequence onto the end of the target
** pp-token sequence. Diagnose if it fails to form a valid pp-token with the
** two pp-tokens pasted together. Return 1 if succeeds.
*/
{
    unsigned    TokLen;
    StrBuf*     OldSource;
    StrBuf      Src = AUTO_STRBUF_INITIALIZER;
    StrBuf      Buf = AUTO_STRBUF_INITIALIZER;

    if (FirstTokLen == 0 || SecondTokLen == 0) {
        SB_Append (Target, Appended);
        return 1;
    }

    /* Since we need to concatenate the token sequences, remove the
    ** last whitespace that was added to target, since it must come
    ** from the input.
    */
    if (IsBlank (SB_LookAtLast (Target))) {
        SB_Drop (Target, 1);
    }

    PRECONDITION (SB_GetLen (Target) >= FirstTokLen &&
                  SB_GetLen (Appended) >= SecondTokLen);

    /* Special casing "..", "/ /" and "/ *" */
    if (FirstTokLen == 1) {
        char C = SB_LookAt (Target, SB_GetLen (Target) - FirstTokLen);
        char N = SB_LookAt (Appended, 0);
        /* Avoid forming a comment introducer or an ellipsis. Note that an
        ** ellipsis pp-token cannot be formed with macros anyway.
        */
        if ((C == '.' && N == '.') || (C == '/' && (N == '/' || N == '*'))) {
            SB_AppendChar (Target, ' ');
            SB_Append (Target, Appended);
            PPWarning ("Pasting formed \"%c%c\", an invalid preprocessing token", C, N);

            return 0;
        }
    }

    SB_CopyBuf (&Src, SB_GetConstBuf (Target) + SB_GetLen (Target) - FirstTokLen, FirstTokLen);
    if (SecondTokLen == SB_GetLen (Appended) || IsSpace (SB_LookAt (Appended, SecondTokLen))) {
        SB_AppendBuf (&Src, SB_GetConstBuf (Appended), SecondTokLen);
    } else {
        SB_AppendBuf (&Src, SB_GetConstBuf (Appended), SecondTokLen + 1);
    }

    SB_Reset (&Src);
    OldSource = InitLine (&Src);

    if (IsPPNumber (CurC, NextC)) {
        /* PP-number */
        CopyPPNumber (&Buf);
    } else if (IsQuotedString ()) {
        /* Quoted string */
        CopyQuotedString (&Buf);
    } else {
        ident Ident;
        if (GetPunc (Ident)) {
            /* Punctuator */
            SB_CopyStr (&Buf, Ident);
        } else if (IsSym (Ident)) {
            /* Identifier */
            SB_CopyStr (&Buf, Ident);
        } else {
            /* Unknown */
         }
    }

    TokLen = SB_GetLen (&Buf);
    if (TokLen < FirstTokLen + SecondTokLen) {
        /* The pasting doesn't form a valid pp-token */
        while (SB_GetLen (&Buf) < FirstTokLen + SecondTokLen) {
            SB_AppendChar (&Buf, CurC);
            NextChar ();
        }
        SB_Terminate (&Buf);
        PPWarning ("Pasting formed \"%s\", an invalid preprocessing token",
                   SB_GetConstBuf (&Buf));

        /* Add a space between the tokens to avoid problems in rescanning */
        if (TokLen > FirstTokLen) {
            SB_AppendChar (Target, ' ');
        }
        /* Append all remaining tokens */
        SB_Append (Target, Appended);

        /* No concatenation */
        TokLen = FirstTokLen;
    } else {
        /* Add a space after the merged token if necessary */
        SB_AppendBuf (Target, SB_GetConstBuf (Appended), SecondTokLen);
        if (TokLen > FirstTokLen + SecondTokLen) {
            SB_AppendChar (Target, ' ');
        }
        /* Append all remaining tokens */
        SB_AppendBuf (Target,
                      SB_GetConstBuf (Appended) + SecondTokLen,
                      SB_GetLen (Appended) - SecondTokLen);
    }

    SB_Done (&Buf);
    SB_Done (&Src);

    /* Restore old source */
    InitLine (OldSource);

    /* Return if concatenation succeeded */
    return TokLen != FirstTokLen;
}



static void SeparatePPTok (StrBuf* Target, char Next)
/* Add a space to target if the previous pp-token could be concatenated with
** the following character.
*/
{
    if (CurRescanStack->PrevTok != 0) {
        unsigned Len = SB_GetLen (CurRescanStack->PrevTok) - SB_GetIndex (CurRescanStack->PrevTok);

        /* Check for pp-token pasting */
        if (CheckPastePPTok (CurRescanStack->PrevTok, Len, Next)) {
            SB_AppendChar (Target, ' ');
        }
        FreeStrBuf (CurRescanStack->PrevTok);
        CurRescanStack->PrevTok = 0;
    }
}



static void LazyCheckNextPPTok (const StrBuf* Prev, unsigned LastTokLen)
/* Memorize the previous pp-token(s) to later check for potential pp-token
** concatenation.
*/
{
    char        C;
    int         CheckEllipsis = 0;
    unsigned    NewIndex = SB_GetLen (Prev) - LastTokLen;

    PRECONDITION (SB_GetLen (Prev) >= LastTokLen);

    /* Check for some special cases */
    C = SB_AtUnchecked (Prev, NewIndex);

    /* We may exclude certain punctuators for speedups. As newer C standards
    ** could add more punctuators such as "[[", "]]", "::" and so on, this
    ** check might need changes accordingly.
    */
    if (C == '[' || C == ']' || C == '(' || C == ')' ||
        C == '{' || C == '}' || C == '~' || C == '?' ||
        C == ':' || C == ';' || C == ',') {
        /* These punctuators cannot be concatenated */
        return;
    }

    /* Special check for .. */
    if (NewIndex > 0    &&
        C == '.'        &&
        SB_AtUnchecked (Prev, NewIndex - 1) == '.') {
        /* Save the preceding '.' as well */
        CheckEllipsis = 1;
    }

    if (CurRescanStack->PrevTok != 0) {
        unsigned OldIndex = SB_GetIndex (CurRescanStack->PrevTok);
        unsigned OldLen = SB_GetLen (CurRescanStack->PrevTok) - OldIndex;
        unsigned NewLen = SB_GetLen (Prev) - NewIndex;
        if (OldLen == NewLen &&
            strncmp (SB_GetConstBuf (CurRescanStack->PrevTok) + OldIndex - CheckEllipsis,
                     SB_GetConstBuf (Prev) + NewIndex - CheckEllipsis,
                     OldLen + CheckEllipsis) == 0) {
            /* Same pp-token, keep using the old one */
        } else {
            /* Logic error */
            SB_Terminate (CurRescanStack->PrevTok);
            Internal ("Unchecked pp-token concatenation: \"%s\"",
                      SB_GetConstBuf (CurRescanStack->PrevTok) + SB_GetIndex (CurRescanStack->PrevTok));
        }
    } else {
        /* Memorize the current line */
        CurRescanStack->PrevTok = NewStrBuf ();
        SB_CopyBuf (CurRescanStack->PrevTok,
                    SB_GetConstBuf (Prev) + NewIndex - CheckEllipsis,
                    LastTokLen + CheckEllipsis);
        SB_Reset (CurRescanStack->PrevTok);
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



static unsigned ReadMacroArgs (unsigned NameIdx, MacroExp* E, const Macro* M, int MultiLine)
/* Identify the arguments to a macro call as-is. Return the total count of
** identifiers and right parentheses in the read argument list.
*/
{
    unsigned    Idx         = 0;
    unsigned    CountInArg  = 0;
    unsigned    Parens      = 0;                        /* Number of open parenthesis */
    ident       Ident;
    MacroExp    Arg;

    InitMacroExp (&Arg);

    /* Eat the left paren */
    NextChar ();

    /* Read the actual macro arguments */
    while (1) {
        /* Squeeze runs of blanks within an arg */
        unsigned OldPendingNewLines = PendingNewLines;
        int Skipped = SkipWhitespace (MultiLine);

        /* Directives can only be found in an argument list that spans
        ** multiple lines.
        */
        if (MultiLine && OldPendingNewLines < PendingNewLines && CurC == '#') {
            unsigned Newlines = 0;

            while (OldPendingNewLines < PendingNewLines && CurC == '#') {
                Newlines += PendingNewLines - OldPendingNewLines;
                PendingNewLines = OldPendingNewLines;
                OldPendingNewLines = 0;
                Skipped = ParseDirectives (MSM_IN_ARG_LIST) || Skipped;
            }
            PendingNewLines += Newlines;
        }

        /* Append a space as a separator */
        if (Skipped && SB_NotEmpty (&Arg.Tokens)) {
            SB_AppendChar (&Arg.Tokens, ' ');
            Skipped = 1;
        } else {
            Skipped = 0;
        }

        /* Finish reading the current argument if we are not inside nested
        ** parentheses or a variadic macro argument.
        */
        if (Parens == 0 &&
            ((CurC == ',' && !ME_IsNextArgVariadic (E, M)) || CurC == ')')) {

            /* End of actual argument. Remove whitespace from the end. */
            while (IsBlank (SB_LookAtLast (&Arg.Tokens))) {
                SB_Drop (&Arg.Tokens, 1);
            }

            /* If this is not the single empty argument for a macro with an
            ** empty argument list, remember it.
            */
            if (CurC != ')'                 ||
                CollCount (&E->Args) > 0    ||
                SB_NotEmpty (&Arg.Tokens)   ||
                M->ParamCount > 0) {
                MacroExp* A = ME_AppendArg (E, &Arg);
                unsigned I;

                /* Copy the hide sets from the argument list */
                for (I = 0; I < CollCount (&E->HideSets); ++I) {
                    HiddenMacro* MHS = CollAtUnchecked (&E->HideSets, I);
                    HideRange* HS;

                    for (HS = MHS->HS; HS != 0; HS = HS->Next) {
                        /* Get the correct hide range */
                        unsigned Start = NameIdx + 1 + Idx;
                        unsigned Len;
                        if (HS->Start < Start) {
                            if (HS->End > Start) {
                                Len = HS->End - Start;
                            } else {
                                /* Out of the range */
                                continue;
                            }
                            Start = 0;
                        } else {
                            Len   = HS->End - HS->Start;
                            Start = HS->Start - Start;
                        }
                        if (Start + Len > Idx + CountInArg) {
                            if (Idx + CountInArg > Start) {
                                Len = Idx + CountInArg - Start;
                            } else {
                                /* Out of the range */
                                break;
                            }
                        }
                        ME_HideMacro (Start, Len, A, MHS->M);
                    }
                }

                /* More argument info */
                A->IdentCount = CountInArg;
            }

            Idx += CountInArg;

            /* Check for end of macro param list */
            if (CurC == ')') {
                /* Count right parens */
                ++Idx;
                NextChar ();
                break;
            }

            /* Start the next param */
            NextChar ();
            DoneMacroExp (&Arg);
            InitMacroExp (&Arg);
            CountInArg = 0;

            continue;

        } else if (CurC == '\0') {

            /* End of input inside macro argument list */
            PPError ("Unterminated argument list invoking macro '%s'", M->Name);
            Parens = -1;
            ClearLine ();
            E->Flags |= MES_ERROR;
            Idx = 0;
            break;

        } else {
            unsigned LastLen = SB_GetLen (&Arg.Tokens);

            if (IsSym (Ident)) {
                /* Just copy the identifier */
                SB_AppendStr (&Arg.Tokens, Ident);

                /* Count identifiers */
                ++CountInArg;

                /* Used for concatentation check */
                if ((Arg.Flags & MES_FIRST_TOKEN) != 0) {
                    Arg.Flags |= MES_BEGIN_WITH_IDENT;
                }
            } else if (IsPPNumber (CurC, NextC)) {
                /* Copy a pp-number */
                CopyPPNumber (&Arg.Tokens);
            } else if (IsQuotedString ()) {
                /* Quoted string - just copy */
                CopyQuotedString (&Arg.Tokens);
            } else if (GetPunc (Ident)) {
                /* Check nested parentheses */
                if (Ident[0] == '(') {
                    /* Opening nested parenthesis */
                    ++Parens;
                } else if (Ident[0] == ')') {
                    /* Closing nested parenthesis */
                    --Parens;

                    /* Count right parens */
                    ++CountInArg;
                }
                /* Just copy the punctuator */
                SB_AppendStr (&Arg.Tokens, Ident);
            } else {
                /* Just copy the character */
                SB_AppendChar (&Arg.Tokens, CurC);
                NextChar ();

                /* But don't count it */
                ++LastLen;
            }

            /* Used for concatentation check */
            ME_SetTokLens (&Arg, SB_GetLen (&Arg.Tokens) - LastLen - Skipped);
        }
    }

    /* Compare formal and actual argument count */
    if (CollCount (&E->Args) < (unsigned) M->ParamCount) {
        /* Check further only when the parentheses are paired */
        if (Parens == 0) {
            /* Specially casing variable argument */
            if (M->Variadic         &&
                M->ParamCount > 0   &&
                CollCount (&E->Args) + 1 == (unsigned) M->ParamCount) {
                /* The variable argument is left out entirely */
                E->Flags |= MES_NO_VA_COMMA;
                if (IS_Get (&Standard) < STD_CC65) {
                    PPWarning ("ISO C does not permit leaving out the comma before the variable argument");
                }
            } else {
                /* Too few argument */
                PPError ("Macro \"%s\" passed only %u arguments, but requires %u",
                         M->Name, CollCount (&E->Args), (unsigned) M->ParamCount);
            }
        }

        /* Be sure to make enough empty arguments available */
        DoneMacroExp (&Arg);
        InitMacroExp (&Arg);
        Arg.Flags |= MES_HAS_REPLACEMENT;
        while (CollCount (&E->Args) < (unsigned) M->ParamCount) {
            ME_AppendArg (E, &Arg);
        }
    } else if (Parens == 0 && CollCount (&E->Args) > (unsigned) M->ParamCount) {
        /* Too many arguments */
        PPError ("Macro \"%s\" passed %u arguments, but takes just %u",
                 M->Name, CollCount (&E->Args), (unsigned) M->ParamCount);
    }

    /* Deallocate argument resources */
    DoneMacroExp (&Arg);

    /* Return the total count of identifiers and right parentheses in the
    ** argument list.
    */
    return Idx;
}



static unsigned SubstMacroArgs (unsigned NameIdx, StrBuf* Target, MacroExp* E, Macro* M, unsigned* IdentCount)
/* Argument substitution according to ISO/IEC 9899:1999 (E), 6.10.3.1ff.
** Return the length of the last pp-token in the result and output the count
** of identifiers and right parentheses in the result to *IdentCount.
*/
{
    unsigned    Idx         = NameIdx;
    ident       Ident;
    unsigned    TokLen      = 0;
    int         ParamIdx;
    StrBuf*     OldSource;
    int         HaveSpace   = 0;
    StrBuf      Buf         = AUTO_STRBUF_INITIALIZER;  /* Temporary buffer */

    /* Remember the current input stack and disable it for now */
    Collection* OldInputStack = UseInputStack (0);

    /* Remember the current input and switch to the macro replacement */
    unsigned    OldIndex = SB_GetIndex (&M->Replacement);

    SB_Reset (&M->Replacement);
    OldSource = InitLine (&M->Replacement);

    /* If the macro expansion replaces an function-like macro with an argument
    ** list containing a right parenthesis outside the hidesets of previously
    ** replaced macros, stop those hidesets from this replacement. This is not
    ** required by the standard but just to match up with other major C
    ** compilers.
    */
    ME_HandleSemiNestedMacro (NameIdx, NameIdx + *IdentCount, E);

    /* Substitution loop */
    while (CurC != '\0') {
        int NeedPaste = 0;

        /* If we have an identifier, check if it's a macro */
        if (IsSym (Ident)) {

            /* Remember and skip any following whitespace */
            HaveSpace = SkipWhitespace (0);

            /* Check if it's a macro parameter */
            if ((ParamIdx = FindMacroParam (M, Ident)) >= 0) {

                /* If a ## operator follows, we have to insert the actual
                ** argument as-is, otherwise it must be macro-replaced.
                */
                if (CurC == '#' && NextC == '#') {

                    /* Get the corresponding actual argument */
                    const MacroExp* A = ME_GetOriginalArg (E, ParamIdx);

                    /* Separate with a white space if necessary */
                    if (CheckPastePPTok (Target, TokLen, SB_Peek (&A->Tokens))) {
                        SB_AppendChar (Target, ' ');
                    }

                    /* For now we need no placemarkers */
                    SB_Append (Target, &A->Tokens);

                    /* Adjust tracking */
                    ME_OffsetHideSets (Idx, A->IdentCount, E);
                    Idx += A->IdentCount;

                    /* This will be used for concatenation */
                    TokLen = A->LastTokLen;

                } else {

                    /* Get the corresponding macro-replaced argument */
                    const MacroExp* A = ME_GetReplacedArg (E, ParamIdx);

                    /* Separate with a white space if necessary */
                    if (CheckPastePPTok (Target, TokLen, SB_Peek (&A->Tokens))) {
                        SB_AppendChar (Target, ' ');
                    }

                    /* Append the replaced string */
                    SB_Append (Target, &A->Tokens);

                    /* Insert the range of identifiers to parent preceding this argument */
                    ME_OffsetHideSets (Idx, A->IdentCount, E);

                    /* Add hide range */
                    ME_AddArgHideSets (Idx, A, E);

                    /* Adjust tracking */
                    Idx += A->IdentCount;

                    /* May be used for later pp-token merge check */
                    TokLen = A->LastTokLen;
                }

            } else {

                /* An identifier, keep it */
                SB_AppendStr (Target, Ident);

                /* Adjust tracking */
                ME_OffsetHideSets (Idx, 1, E);
                ++Idx;

                /* May be used for later concatenation */
                TokLen = strlen (Ident);
            }

            /* Special casing for 'L' prefixing '#' */
            if (TokLen == 1 && SB_LookAtLast (Target) == 'L' && CurC == '#') {
                HaveSpace = 1;
            }

            /* Squeeze and add the skipped whitespace back for consistency */
            if (HaveSpace && !IsBlank (SB_LookAtLast (Target))) {
                SB_AppendChar (Target, ' ');
            }

            /* Done with this substituted argument */
            continue;

        } else if (CurC == '#' && NextC == '#') {

            /* ## operator */
            NextChar ();
            NextChar ();
            SkipWhitespace (0);

            /* If the next token is an identifier which is a macro argument,
            ** replace it, otherwise just add it.
            */
            if (IsSym (Ident)) {
                unsigned NewCount = 1;

                /* Check if it's a macro parameter */
                if ((ParamIdx = FindMacroParam (M, Ident)) >= 0) {

                    /* Get the corresponding actual argument */
                    MacroExp* A = ME_GetOriginalArg (E, ParamIdx);

                    /* Insert the range of identifiers to parent preceding this argument */
                    ME_OffsetHideSets (Idx, A->IdentCount, E);

                    /* Add hide range */
                    ME_AddArgHideSets (Idx, A, E);

                    /* Adjust tracking */
                    NewCount = A->IdentCount;

                    /* If the preceding pp-token is not a placemarker and is
                    ** concatenated to with an identifier, the count of tracked
                    ** identifiers is then one less.
                    */
                    if (TryPastePPTok (Target, &A->Tokens, TokLen, A->FirstTokLen)) {
                        if (TokLen > 0 && (A->Flags & MES_BEGIN_WITH_IDENT) != 0) {
                            --NewCount;
                            ME_RemoveToken (Idx, 1, E);
                        }
                        if ((A->Flags & MES_MULTIPLE_TOKEN) == 0) {
                            TokLen += A->FirstTokLen;
                        } else {
                            TokLen = A->LastTokLen;
                        }
                    } else {
                        TokLen = A->LastTokLen;
                    }

                } else {

                    unsigned Len;

                    /* Just an ordinary identifier - add as-is */
                    SB_CopyStr (&Buf, Ident);

                    /* If the preceding pp-token is not a placemarker and is
                    ** concatenated to with an identifier, the count of tracked
                    ** identifiers is then one less.
                    */
                    Len = SB_GetLen (&Buf);
                    if (TryPastePPTok (Target, &Buf, TokLen, Len)) {
                        if (TokLen > 0) {
                            --NewCount;
                        }
                        TokLen += Len;
                    } else {
                        TokLen = Len;
                    }

                    /* Adjust tracking */
                    ME_OffsetHideSets (Idx, NewCount, E);

                }

                /* Adjust tracking */
                Idx += NewCount;

                /* Keep the whitespace for consistency */
                HaveSpace = SkipWhitespace (0);
                if (HaveSpace && !IsBlank (SB_LookAtLast (Target))) {
                    SB_AppendChar (Target, ' ');
                }

                /* Done with this concatenated identifier */
                continue;
            }

            if (CurC != '\0') {
                /* Non-identifiers may still be pasted together */
                NeedPaste = 1;
            }

        }

        /* Use the temporary buffer */
        SB_Clear (&Buf);
        if (IsPPNumber (CurC, NextC)) {
            CopyPPNumber (&Buf);
        } else if (IsQuotedString ()) {
            CopyQuotedString (&Buf);
        } else {
            if (CurC == '#' && M->ParamCount >= 0) {
                /* A # operator within a macro expansion of a function-like
                ** macro. Read the following identifier and check if it's a
                ** macro parameter.
                */
                NextChar ();
                SkipWhitespace (0);
                if (!IsSym (Ident) || (ParamIdx = FindMacroParam (M, Ident)) < 0) {
                    /* Should not happen, but still */
                    Internal ("'#' is not followed by a macro parameter");
                } else {
                    /* Make a valid string from Replacement */
                    MacroExp* A = ME_GetOriginalArg (E, ParamIdx);
                    SB_Reset (&A->Tokens);
                    Stringize (&A->Tokens, &Buf);
                }
            } else if (GetPunc (Ident)) {
                /* Count right parens. This is OK since they cannot be pasted
                ** to form different punctuators with others.
                */
                if (Ident[0] == ')') {
                    /* Adjust tracking */
                    ME_OffsetHideSets (Idx, 1, E);
                    ++Idx;
                }
                SB_AppendStr (&Buf, Ident);
            } else if (CurC != '\0') {
                SB_AppendChar (&Buf, CurC);
                NextChar ();
            }
        }

        /* Squeeze any whitespace for consistency. Especially, comments must
        ** be consumed before fed to the punctuator parser, or their leading
        ** '/' characters would be parsed wrongly as division operators.
        */
        HaveSpace = SkipWhitespace (0);

        if (NeedPaste) {
            unsigned Len = SB_GetLen (&Buf);

            /* Concatenate pp-tokens */
            if (TryPastePPTok (Target, &Buf, TokLen, Len)) {
                TokLen += Len;
            } else {
                TokLen = Len;
            }
        } else {
            /* Just append the token */
            SB_Append (Target, &Buf);
            TokLen = SB_GetLen (&Buf);
        }

        if (HaveSpace && !IsBlank (SB_LookAtLast (Target))) {
            SB_AppendChar (Target, ' ');
        }

    }

    /* Done with the temporary buffer */
    SB_Done (&Buf);

    /* Remove the macro name itself together with the arguments (if any) */
    ME_RemoveToken (Idx, 1 + *IdentCount, E);

    /* Hide this macro for the whole result of this expansion */
    ME_HideMacro (NameIdx, Idx - NameIdx, E, M);

    /* Switch back the input */
    UseInputStack (OldInputStack);
    InitLine (OldSource);
    SB_SetIndex (&M->Replacement, OldIndex);

    /* Set the count of identifiers and right parentheses in the result */
    *IdentCount = Idx - NameIdx;

    /* Return the length of the last pp-token */
    return TokLen;
}



static unsigned ExpandMacro (unsigned Idx, StrBuf* Target, MacroExp* E, Macro* M, int MultiLine)
/* Expand a macro into Target. Return the length of the last pp-token in the
** result of the expansion.
*/
{
    unsigned Count = 0;     /* Count of identifiers and right parentheses */
    unsigned Len   = 0;     /* Length of the last pp-token in the result */

    /* Disable previous pp-token spacing checking */
    StrBuf* PrevTok = CurRescanStack->PrevTok;
    CurRescanStack->PrevTok = 0;

#if DEV_CC65_DEBUG
    static unsigned V = 0;
    printf ("Expanding (%u) %s\n", ++V, M->Name);
#endif

    /* Check if this is a function like macro */
    if (M->ParamCount >= 0) {
        /* Read the actual macro arguments (with the enclosing parentheses) */
        Count = ReadMacroArgs (Idx, E, M, MultiLine);
    }

    if ((E->Flags & MES_ERROR) == 0) {
        /* Replace macro parameters with arguments handling the # and ## operators */
        Len = SubstMacroArgs (Idx, Target, E, M, &Count);
    } else {
        SB_CopyStr (Target, M->Name);
    }

    if (CollCount (&E->Args) > 0) {
        /* Clear all arguments */
        ME_ClearArgs (E);
    }

#if DEV_CC65_DEBUG
    printf ("Expanded (%u) %s to %d ident(s) at %u: %s\n",
            V--, M->Name, Count, Idx, SB_GetConstBuf (Target));
#endif

    /* Reenable previous pp-token concatenation checking */
    FreeStrBuf (CurRescanStack->PrevTok);
    CurRescanStack->PrevTok = PrevTok;

    /* Return the length of the last pp-token in the expansion result */
    return Len;
}



static unsigned ReplaceMacros (StrBuf* Source, StrBuf* Target, MacroExp* E, unsigned ModeFlags)
/* Scan for and perform macro replacement. Return the count of identifiers and
** right parentheses in the replacement result.
*/
{
    unsigned    Count       = 0;
    StrBuf*     TmpTarget   = NewStrBuf ();

    /* Remember the current input and switch to Source */
    StrBuf*             OldSource      = InitLine (Source);
    RescanInputStack    RescanStack;
    RescanInputStack*   OldRescanStack = CurRescanStack;

    InitRescanInputStack (&RescanStack);
    PushRescanLine (&RescanStack, Line, 0);
    CurRescanStack = &RescanStack;

    /* Loop substituting macros */
    while (CurC != '\0') {
        int Skipped = 0;
        ident Ident;

        /* If we have an identifier, check if it's a macro */
        if (IsSym (Ident)) {
            /* Check for bad identifier names */
            if ((ModeFlags & (MSM_MULTILINE | MSM_IN_DIRECTIVE | MSM_IN_ARG_LIST)) != 0 &&
                (CollCount (&CurRescanStack->Lines) == 1 || CurC == '\0')) {
                CheckForBadIdent (Ident, IS_Get (&Standard), 0);
            }

            if ((ModeFlags & MSM_OP_DEFINED) != 0 && strcmp (Ident, "defined") == 0) {
                /* Handle the "defined" operator */
                int HaveParen = 0;

                /* Eat the "defined" operator */
                ME_RemoveToken (Count, 1, E);

                SkipWhitespace (0);
                if (CurC == '(') {
                    HaveParen = 1;
                    NextChar ();
                    SkipWhitespace (0);
                }

                /* Add a space to separate the result if necessary */
                SeparatePPTok (Target, '0');

                if (IsSym (Ident)) {
                    /* Eat the identifier */
                    ME_RemoveToken (Count, 1, E);
                    SB_AppendChar (Target, IsMacro (Ident) ? '1' : '0');
                    if (HaveParen) {
                        SkipWhitespace (0);
                        if (CurC != ')') {
                            PPError ("')' expected");
                            ClearLine ();
                        } else {
                            /* Eat the right parenthesis */
                            ME_RemoveToken (Count, 1, E);
                            NextChar ();
                        }
                    }
                } else {
                    PPError ("Macro name must be an identifier");
                    ClearLine ();
                    SB_AppendChar (Target, '0');
                }
            } else {
                Macro* M = FindMacro (Ident);

                /* Check if it's an expandable macro */
                if (M != 0 && ME_CanExpand (Count, E, M)) {
                    int      MultiLine = (ModeFlags & MSM_MULTILINE) != 0;
                    unsigned LastTokLen;

                    /* Check if this is a function-like macro */
                    if (M->ParamCount >= 0) {
                        int HaveSpace = SkipWhitespace (MultiLine) > 0;

                        /* A function-like macro name without an immediately
                        ** following argument list is not subject to expansion.
                        */
                        if (CurC != '(') {
                            /* No expansion */
                            ++Count;

                            /* Add a space to separate the macro name if necessary */
                            SeparatePPTok (Target, M->Name[0]);
                            SB_AppendStr (Target, M->Name);

                            /* Keep tracking pp-token lengths */
                            if ((ModeFlags & MSM_IN_ARG_EXPANSION) != 0) {
                                /* Used for concatentation check */
                                if ((E->Flags & MES_FIRST_TOKEN) != 0) {
                                    E->Flags |= MES_BEGIN_WITH_IDENT;
                                }
                                ME_SetTokLens (E, strlen (M->Name));
                            }

                            /* Since we have already got on hold of the next
                            ** line, we have to reuse it as the next line
                            ** instead of reading a new line from the source.
                            */
                            if (PendingNewLines > 0 && MultiLine) {
                                unsigned I = SB_GetIndex (Line);

                                /* There is no way a function-like macro call
                                ** detection could span multiple lines within
                                ** the range of another just expanded macro.
                                */
                                CHECK (CollCount (&CurRescanStack->Lines) == 1);

                                /* Revert one newline */
                                --PendingNewLines;

                                /* Align indention */
                                while (I > 0) {
                                    --I;
                                    if (SB_GetBuf (Line)[I] == '\n') {
                                        ++I;
                                        break;
                                    }
                                    SB_GetBuf (Line)[I] = ' ';
                                }

                                /* Set start index */
                                SB_SetIndex (Line, I);

                                /* Add newlines */
                                AddPreLine (Target);

                                /* Reuse this line as the next line */
                                ReuseInputLine ();

                                /* Quit this loop */
                                break;
                            }

                            /* Append back the whitespace */
                            if (HaveSpace) {
                                SB_AppendChar (Target, ' ');
                            }

                            /* Loop */
                            goto Loop;
                        }
                    }

                    /* Either an object-like or function-like macro */
                    MultiLine = MultiLine && M->ParamCount >= 0;

                    /* If we were going to support #pragma in macro argument
                    ** list, it would be output to OLine.
                    */
                    if (MultiLine && OLine == 0) {
                        OLine = TmpTarget;
                        LastTokLen = ExpandMacro (Count, TmpTarget, E, M, MultiLine);
                        OLine = 0;
                    } else {
                        LastTokLen = ExpandMacro (Count, TmpTarget, E, M, MultiLine);
                    }

                    /* Check for errors in expansion */
                    if ((E->Flags & MES_ERROR) != 0) {
                        break;
                    }

                    /* Pop the current line if it is at the end */
                    PopRescanLine ();

                    if (SB_GetLen (TmpTarget) > 0) {
                        /* Start rescanning from the temporary result */
                        SB_Reset (TmpTarget);
                        InitLine (TmpTarget);
                        PushRescanLine (CurRescanStack, TmpTarget, LastTokLen);

                        /* Add a space before a '#' at the beginning of the line */
                        if (CurC  == '#' &&
                            NextC != '#' &&
                            (SB_IsEmpty (Target) || SB_LookAtLast (Target) == '\n')) {
                            SB_AppendChar (Target, ' ');
                        }

                        /* Switch the buffers */
                        TmpTarget = NewStrBuf ();
                    } else if (PendingNewLines > 0 && MultiLine) {
                        /* Cancel remaining check for pp-tokens separation
                        ** if there is since ther have been newlines that
                        ** can always separate them.
                        */
                        if (CurRescanStack->PrevTok != 0) {
                            FreeStrBuf (CurRescanStack->PrevTok);
                            CurRescanStack->PrevTok = 0;
                        }

                        /* Squeeze whitespace */
                        SkipWhitespace (0);

                        /* Add indention to preprocessor output if needed */
                        if (CurC != '\0' && CollCount (&CurRescanStack->Lines) == 1) {
                            /* Add newlines */
                            AddPreLine (Target);

                            /* Align indention */
                            AppendIndent (Target, SB_GetIndex (Line));
                        }
                    }

                    /* Since we are rescanning, we needn't add the
                    ** count of just replaced identifiers right now.
                    */
                    continue;
                }

                /* An unexpandable identifier. Keep it. */
                ++Count;

                /* Add a space to separate the macro name if necessary */
                SeparatePPTok (Target, Ident[0]);
                SB_AppendStr (Target, Ident);

                /* Keep tracking pp-token lengths */
                if ((ModeFlags & MSM_IN_ARG_EXPANSION) != 0) {
                    /* Used for concatentation check */
                    if ((E->Flags & MES_FIRST_TOKEN) != 0) {
                        E->Flags |= MES_BEGIN_WITH_IDENT;
                    }
                    ME_SetTokLens (E, strlen (Ident));
                }
            }
        } else {
            unsigned LastLen;

            /* Add a space to separate the macro name if necessary */
            SeparatePPTok (Target, CurC);

            LastLen = SB_GetLen (Target);

            if ((ModeFlags & MSM_TOK_HEADER) != 0 && (CurC == '<' || CurC == '\"')) {
                CopyHeaderNameToken (Target);
            } else if (IsPPNumber (CurC, NextC)) {
                CopyPPNumber (Target);
            } else if (IsQuotedString ()) {
                CopyQuotedString (Target);
            } else {
                /* We want to squeeze whitespace until the end of the current
                ** input line, so we have to deal with such cases specially.
                */
                if (CollCount (&CurRescanStack->Lines) > 1) {
                    RescanInputStack* RIS = CurRescanStack;

                    /* Temporarily disable input popping */
                    CurRescanStack = 0;
                    Skipped = SkipWhitespace (0);
                    CurRescanStack = RIS;

                    if (CurC == '\0') {
                        /* Now we are at the end of the input line */
                        goto Loop;
                    }
                } else {
                    Skipped = SkipWhitespace (0);
                }

                /* Punctuators must be checked after whitespace since comments
                ** introducers may be misinterpreted as division operators.
                */
                if (!Skipped) {
                    if (GetPunc (Ident)) {
                        if (Ident[0] == ')') {
                            /* Count right parens */
                            ++Count;
                        }
                        SB_AppendStr (Target, Ident);

                        /* If an identifier follows immediately, it could be a macro
                        ** expanded later that occasionally need a space to separate.
                        */
                        if (IsIdent (CurC)) {
                            /* Memorize the previous pp-token and check it later */
                            LazyCheckNextPPTok (Target, strlen (Ident));
                        }
                    } else {
                        SB_AppendChar (Target, CurC);
                        NextChar ();

                        /* Don't count this character */
                        ++LastLen;
                    }
                }
            }

            /* Keep tracking pp-token lengths */
            if ((ModeFlags & MSM_IN_ARG_EXPANSION) != 0) {
                ME_SetTokLens (E, SB_GetLen (Target) - LastLen);
            }
        }

Loop:
        /* Switch back to the previous input stream if we have finished
        ** rescanning the current one.
        */
        if (CurC == '\0' && CollCount (&CurRescanStack->Lines) > 1) {
            /* Check for rescan sequence end and pp-token pasting */
            Skipped = SkipWhitespace (0) || Skipped;

            /* Add indention to preprocessor output if needed */
            if (CurC != '\0'                        &&
                PendingNewLines > 0                 &&
                (ModeFlags & MSM_MULTILINE) != 0    &&
                CollCount (&CurRescanStack->Lines) == 1) {
                /* Add newlines */
                AddPreLine (Target);

                /* Align indention */
                AppendIndent (Target, SB_GetIndex (Line));
                Skipped = 0;
            }
        }

        /* Append a space if there hasn't been one */
        if (Skipped && !IsSpace (SB_LookAtLast (Target))) {
            SB_AppendChar (Target, ' ');
        }
    }

    /* Append the remaining result */
    SB_Append (Target, TmpTarget);

    /* Done with the temporary buffer */
    SB_Done (TmpTarget);

    /* Drop whitespace at the end */
    if (IsBlank (SB_LookAtLast (Target))) {
        SB_Drop (Target, 1);
    }

    /* Sanity check */
    if ((E->Flags & MES_ERROR) == 0) {
        CHECK (CollCount (&CurRescanStack->Lines) == 1);
    }

    /* Done with the current input stack */
    DoneRescanInputStack (CurRescanStack);
    CurRescanStack = OldRescanStack;

    /* Switch back the input */
    InitLine (OldSource);

    /* Return the count of identifiers and right parentheses */
    return Count;
}



/*****************************************************************************/
/*                                Directives                                 */
/*****************************************************************************/



static int ParseMacroReplacement (StrBuf* Source, Macro* M)
/* Check correctness of macro definition while squeezing old and new style
** comments and other non-newline whitespace sequences. Return 1 on success
** or 0 on failure.
*/
{
    /* Switch to the new input source */
    StrBuf*     OldSource = InitLine (Source);
    int         HasWhiteSpace = 0;
    unsigned    Len;
    ident       Ident;
    int         Std = IS_Get (&Standard);

    /* Skip whitespace before the macro replacement */
    SkipWhitespace (0);

    /* Check for ## at start */
    if (CurC == '#' && NextC == '#') {
        /* Diagnose and bail out */
        PPError ("'##' cannot appear at start of macro expansion");
        goto Error_Handler;
    }

    /* Loop removing ws and comments */
    while (CurC != '\0') {
        if (HasWhiteSpace) {
            SB_AppendChar (&M->Replacement, ' ');
        } else if (IsQuotedString ()) {
            CopyQuotedString (&M->Replacement);
        } else if (IsSym (Ident)) {
            CheckForBadIdent (Ident, Std, M);
            SB_AppendStr (&M->Replacement, Ident);
        } else {
            if (M->ParamCount >= 0 && GetPunc (Ident)) {
                Len = strlen (Ident);
                /* Check for # */
                if (Len == 1 && Ident[0] == '#') {
                    HasWhiteSpace = SkipWhitespace (0);

                    /* Check next pp-token */
                    if (!IsSym (Ident) || FindMacroParam (M, Ident) < 0) {
                        PPError ("'#' is not followed by a macro parameter");
                        goto Error_Handler;
                    }

                    /* Make the replacement */
                    SB_AppendChar (&M->Replacement, '#');
                    if (HasWhiteSpace) {
                        SB_AppendChar (&M->Replacement, ' ');
                    }
                    SB_AppendStr (&M->Replacement, Ident);
                } else {
                    SB_AppendBuf (&M->Replacement, Ident, Len);
                }
            } else {
                SB_AppendChar (&M->Replacement, CurC);
                NextChar ();
            }
        }

        HasWhiteSpace = SkipWhitespace (0);
    }

    /* Check for ## at end */
    Len = SB_GetLen (&M->Replacement);
    if (Len >= 2) {
        if (SB_LookAt (&M->Replacement, Len - 1) == '#' &&
            SB_LookAt (&M->Replacement, Len - 2) == '#') {
            /* Diagnose and bail out */
            PPError ("'##' cannot appear at end of macro expansion");
            goto Error_Handler;
        }
    }

    /* Terminate the new input line */
    SB_Terminate (&M->Replacement);

    /* Switch back to the old source */
    InitLine (OldSource);

    /* Success */
    return 1;

Error_Handler:

    /* Switch back to the old source */
    InitLine (OldSource);

    /* Failure */
    return 0;
}



static void DoDefine (void)
/* Process #define directive */
{
    ident       Ident;
    Macro*      M = 0;
    Macro*      Existing;
    int         Std;

    /* Read the macro name */
    SkipWhitespace (0);
    if (!MacName (Ident)) {
        goto Error_Handler;
    }

    /* Remember the language standard we are in */
    Std = IS_Get (&Standard);

    /* Check for forbidden macro names */
    if (strcmp (Ident, "defined") == 0) {
        PPError ("'%s' cannot be used as a macro name", Ident);
        goto Error_Handler;
    }

    /* Check for and warn on special identifiers */
    CheckForBadIdent (Ident, Std, 0);

    /* Create a new macro definition */
    M = NewMacro (Ident);

    /* Check if this is a function-like macro */
    if (CurC == '(') {

        /* Skip the left paren */
        NextChar ();

        /* Set the marker that this is a function-like macro */
        M->ParamCount = 0;

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
            if (Std >= STD_C99 && CurC == '.') {
                /* Ellipsis */
                NextChar ();
                if (CurC != '.' || NextC != '.') {
                    PPError ("'...' expected");
                    goto Error_Handler;
                }
                NextChar ();
                NextChar ();

                /* Remember that the macro is variadic and use __VA_ARGS__ as
                ** the parameter name.
                */
                AddMacroParam (M, "__VA_ARGS__");
                M->Variadic = 1;

            } else {
                /* Must be macro parameter name */
                if (MacName (Ident) == 0) {
                    goto Error_Handler;
                }

                /* Check for and warn on special identifiers */
                CheckForBadIdent (Ident, Std, 0);

                /* Add the macro parameter */
                AddMacroParam (M, Ident);
            }

            /* If we had an ellipsis, or the next char is not a comma, we've
            ** reached the end of the macro parameter list.
            */
            SkipWhitespace (0);
            if (M->Variadic || CurC != ',') {
                break;
            }
            NextChar ();
        }

        /* Check for a right paren and eat it if we find one */
        if (CurC != ')') {
            PPError ("')' expected for macro definition");
            goto Error_Handler;
        }
        NextChar ();
    }

    /* Remove whitespace and comments from the line, store the preprocessed
    ** line into the macro replacement buffer.
    */
    if (ParseMacroReplacement (Line, M) == 0) {
        goto Error_Handler;
    }

#if 0
    printf ("%s: <%.*s>\n", M->Name, SB_GetLen (&M->Replacement), SB_GetConstBuf (&M->Replacement));
#endif

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

    /* Success */
    return;

Error_Handler:

    /* Cleanup */
    ClearLine ();

    if (M != 0) {
        FreeMacro (M);
    }
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
            CheckForBadIdent (Ident, IS_Get (&Standard), 0);
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
            PPError ("#include expects \"FILENAME\" or <FILENAME>");
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
    } else {
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



static unsigned GetLineDirectiveNum (void)
/* Get a decimal digit-sequence from the input. Return 0 on errors. */
{
    unsigned long Num = 0;
    StrBuf Buf = AUTO_STRBUF_INITIALIZER;

    /* The only non-decimal-numeric character allowed in the digit-sequence is
    ** the digit separator '\'' as of C23, but we haven't supported it yet.
    */
    SkipWhitespace (0);
    while (IsDigit (CurC))
    {
        SB_AppendChar (&Buf, CurC);
        NextChar ();
    }

    /* Ensure the buffer is terminated with a '\0' */
    SB_Terminate (&Buf);
    if (SkipWhitespace (0) != 0 || CurC == '\0') {
        const char* Str = SB_GetConstBuf (&Buf);
        if (Str[0] == '\0') {
            PPWarning ("#line directive interprets number as decimal, not octal");
        } else {
            Num = strtoul (Str, 0, 10);
            if (Num > 2147483647) {
                PPError ("#line directive requires an integer argument not greater than 2147483647");
                ClearLine ();
                Num = 0;
            } else if (Num == 0) {
                PPError ("#line directive requires a positive integer argument");
                ClearLine ();
            }
        }
    } else {
        PPError ("#line directive requires a simple decimal digit sequence");
        ClearLine ();
    }

    /* Done with the buffer */
    SB_Done (&Buf);

    return (unsigned)Num;
}



static void DoLine (void)
/* Process #line directive */
{
    unsigned LineNum;

    /* Macro-replace a single line with support for the "defined" operator */
    SB_Clear (MLine);
    PreprocessDirective (Line, MLine, MSM_NONE);

    /* Read from the processed line */
    SB_Reset (MLine);
    MLine = InitLine (MLine);

    /* Parse and check the specified line number */
    LineNum = GetLineDirectiveNum ();
    if (LineNum != 0) {
        /* Parse and check the optional filename argument */
        if (SB_GetIndex (Line) < SB_GetLen (Line)) {
            StrBuf Filename = AUTO_STRBUF_INITIALIZER;
            if (SB_GetString (Line, &Filename)) {
                SB_Terminate (&Filename);
                SetCurrentFileName (SB_GetConstBuf (&Filename));
            } else {
                PPError ("Invalid filename for #line directive");
                LineNum = 0;
            }
            SB_Done (&Filename);
        }

        /* #line actually sets the line number of the next line */
        if (LineNum > 0) {
            SetCurrentLineNum (LineNum - 1);
            /* Check for extra tokens at the end */
            CheckExtraTokens ("line");
        }
    }

    /* Restore input source */
    MLine = InitLine (MLine);
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
        CheckForBadIdent (Ident, IS_Get (&Standard), 0);
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
                        if (!PPSkip) {
                            DoLine ();
                        }
                        break;

                    case PPD_PRAGMA:
                        if (!PPSkip) {
                            if ((ModeFlags & MSM_IN_ARG_LIST) == 0) {
                                DoPragma ();
                                return Whitespace;
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

    return Whitespace;
}



void HandleSpecialMacro (Macro* M, const char* Name)
/* Handle special "magic" macros that may change */
{
    if (strcmp (Name, "__COUNTER__") == 0) {
        /* Replace __COUNTER__ with the current counter number */
        if (IS_Get (&Standard) < STD_CC65) {
            PPWarning ("__COUNTER__ is a cc65 extension");
        }
        SB_Printf (&M->Replacement, "%u", GetCurrentCounter ());
    } else if (strcmp (Name, "__LINE__") == 0) {
        /* Replace __LINE__ with the current line number */
        SB_Printf (&M->Replacement, "%u", GetCurrentLineNum ());
    } else if (strcmp (Name, "__FILE__") == 0) {
        /* Replace __FILE__ with the current filename */
        StrBuf B = AUTO_STRBUF_INITIALIZER;
        SB_InitFromString (&B, GetCurrentFileName ());
        SB_Clear (&M->Replacement);
        Stringize (&B, &M->Replacement);
        SB_Done (&B);
    }
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
            } else if (CurC == '/' && NextC == '/') {
                NewStyleComment ();
                HasWhiteSpace = 1;
            } else {
                /* No more white space */
                break;
            }
        }
        if (HasWhiteSpace) {
            SB_AppendChar (Target, ' ');
        } else if (IsQuotedString ()) {
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
    MacroExp    E;

    SkipWhitespace (0);
    InitMacroExp (&E);
    ReplaceMacros (Source, Target, &E, ModeFlags | MSM_IN_DIRECTIVE);
    DoneMacroExp (&E);
}



void Preprocess (void)
/* Preprocess lines count of which is affected by directives */
{
    MacroExp E;

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
    if (CurC != '\0') {
        AppendIndent (PLine, SB_GetIndex (Line));
    }

    /* Expand macros if any */
    InitMacroExp (&E);
    ReplaceMacros (Line, PLine, &E, MSM_MULTILINE);
    DoneMacroExp (&E);

    /* Add the source info to preprocessor output if needed */
    AddPreLine (PLine);

    /* Read from the new line */
    SB_Reset (PLine);
    PLine = InitLine (PLine);

    if (Verbosity > 1 && SB_NotEmpty (Line)) {
        printf ("%s:%u: %.*s\n", GetCurrentFileName (), GetCurrentLineNum (),
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



void ContinueLine (void)
/* Continue the current line ended with a '\\' */
{
    ++ContinuedLines;
}



void PreprocessBegin (void)
/* Initialize preprocessor with current file */
{
    /* Reset #if depth */
    PPStack->Index = -1;

    /* Remember to update source file location in preprocess-only mode */
    FileChanged = 1;

    /* Enable diagnostics on new style comments in C89 mode */
    AllowNewComments = 0;
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
