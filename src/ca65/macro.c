/*****************************************************************************/
/*                                                                           */
/*                                  macro.c                                  */
/*                                                                           */
/*                    Macros for the ca65 macroassembler                     */
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



#include <stdio.h>
#include <string.h>

/* common */
#include "check.h"
#include "hashfunc.h"
#include "hashtab.h"
#include "xmalloc.h"

/* ca65 */
#include "condasm.h"
#include "error.h"
#include "global.h"
#include "instr.h"
#include "istack.h"
#include "lineinfo.h"
#include "nexttok.h"
#include "pseudo.h"
#include "toklist.h"
#include "macro.h"



/*****************************************************************************/
/*                                 Forwards                                  */
/*****************************************************************************/



static unsigned HT_GenHash (const void* Key);
/* Generate the hash over a key. */

static const void* HT_GetKey (const void* Entry);
/* Given a pointer to the user entry data, return a pointer to the key */

static int HT_Compare (const void* Key1, const void* Key2);
/* Compare two keys. The function must return a value less than zero if
** Key1 is smaller than Key2, zero if both are equal, and a value greater
** than zero if Key1 is greater then Key2.
*/



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Struct that describes an identifer (macro param, local list) */
typedef struct IdDesc IdDesc;
struct IdDesc {
    IdDesc*         Next;       /* Linked list */
    StrBuf          Id;         /* Identifier, dynamically allocated */
};



/* Struct that describes a macro definition */
struct Macro {
    HashNode        Node;       /* Hash list node */
    Macro*          List;       /* List of all macros */
    unsigned        LocalCount; /* Count of local symbols */
    IdDesc*         Locals;     /* List of local symbols */
    unsigned        ParamCount; /* Parameter count of macro */
    IdDesc*         Params;     /* Identifiers of macro parameters */
    unsigned        TokCount;   /* Number of tokens for this macro */
    TokNode*        TokRoot;    /* Root of token list */
    TokNode*        TokLast;    /* Pointer to last token in list */
    StrBuf          Name;       /* Macro name, dynamically allocated */
    unsigned        Expansions; /* Number of active macro expansions */
    unsigned char   Style;      /* Macro style */
    unsigned char   Incomplete; /* Macro is currently built */
};

/* Hash table functions */
static const HashFunctions HashFunc = {
    HT_GenHash,
    HT_GetKey,
    HT_Compare
};

/* Macro hash table */
static HashTable MacroTab = STATIC_HASHTABLE_INITIALIZER (117, &HashFunc);

/* Structs that holds data for a macro expansion */
typedef struct MacExp MacExp;
struct MacExp {
    MacExp*     Next;           /* Pointer to next expansion */
    Macro*      M;              /* Which macro do we expand? */
    unsigned    IfSP;           /* .IF stack pointer at start of expansion */
    TokNode*    Exp;            /* Pointer to current token */
    TokNode*    Final;          /* Pointer to final token */
    unsigned    MacExpansions;  /* Number of active macro expansions */
    unsigned    LocalStart;     /* Start of counter for local symbol names */
    unsigned    ParamCount;     /* Number of actual parameters */
    TokNode**   Params;         /* List of actual parameters */
    TokNode*    ParamExp;       /* Node for expanding parameters */
    LineInfo*   LI;             /* Line info for the expansion */
    LineInfo*   ParamLI;        /* Line info for parameter expansion */
};

/* Maximum number of nested macro expansions */
#define MAX_MACEXPANSIONS       256U

/* Number of active macro expansions */
static unsigned MacExpansions = 0;

/* Flag if a macro expansion should get aborted */
static int DoMacAbort = 0;

/* Counter to create local names for symbols */
static unsigned LocalName = 0;

/* Define-style macros disabled if != 0 */
static unsigned DisableDefines = 0;



/*****************************************************************************/
/*                           Hash table functions                            */
/*****************************************************************************/



static unsigned HT_GenHash (const void* Key)
/* Generate the hash over a key. */
{
    return HashBuf (Key);
}



static const void* HT_GetKey (const void* Entry)
/* Given a pointer to the user entry data, return a pointer to the index */
{
    return &((Macro*) Entry)->Name;
}



static int HT_Compare (const void* Key1, const void* Key2)
/* Compare two keys. The function must return a value less than zero if
** Key1 is smaller than Key2, zero if both are equal, and a value greater
** than zero if Key1 is greater then Key2.
*/
{
    return SB_Compare (Key1, Key2);
}



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



static IdDesc* NewIdDesc (const StrBuf* Id)
/* Create a new IdDesc, initialize and return it */
{
    /* Allocate memory */
    IdDesc* ID = xmalloc (sizeof (IdDesc));

    /* Initialize the struct */
    ID->Next = 0;
    SB_Init (&ID->Id);
    SB_Copy (&ID->Id, Id);

    /* Return the new struct */
    return ID;
}



static void FreeIdDesc (IdDesc* ID)
/* Free an IdDesc */
{
    /* Free the name */
    SB_Done (&ID->Id);

    /* Free the structure itself */
    xfree (ID);
}



static void FreeIdDescList (IdDesc* ID)
/* Free a complete list of IdDesc structures */
{
    while (ID) {
        IdDesc* This = ID;
        ID = ID->Next;
        FreeIdDesc (This);
    }
}



static Macro* NewMacro (const StrBuf* Name, unsigned char Style)
/* Generate a new macro entry, initialize and return it */
{
    /* Allocate memory */
    Macro* M = xmalloc (sizeof (Macro));

    /* Initialize the macro struct */
    InitHashNode (&M->Node);
    M->LocalCount = 0;
    M->Locals     = 0;
    M->ParamCount = 0;
    M->Params     = 0;
    M->TokCount   = 0;
    M->TokRoot    = 0;
    M->TokLast    = 0;
    SB_Init (&M->Name);
    SB_Copy (&M->Name, Name);
    M->Expansions = 0;
    M->Style      = Style;
    M->Incomplete = 1;

    /* Insert the macro into the hash table */
    HT_Insert (&MacroTab, &M->Node);

    /* Return the new macro struct */
    return M;
}



static void FreeMacro (Macro* M)
/* Free a macro entry which has already been removed from the macro table. */
{
    TokNode* T;

    /* Free locals */
    FreeIdDescList (M->Locals);

    /* Free identifiers of parameters */
    FreeIdDescList (M->Params);

    /* Free the token list for the macro */
    while ((T = M->TokRoot) != 0) {
        M->TokRoot = T->Next;
        FreeTokNode (T);
    }

    /* Free the macro name */
    SB_Done (&M->Name);

    /* Free the macro structure itself */
    xfree (M);
}



static MacExp* NewMacExp (Macro* M)
/* Create a new expansion structure for the given macro */
{
    unsigned I;

    /* Allocate memory */
    MacExp* E = xmalloc (sizeof (MacExp));

    /* Initialize the data */
    E->M                = M;
    E->IfSP             = GetIfStack ();
    E->Exp              = M->TokRoot;
    E->Final            = 0;
    E->MacExpansions    = ++MacExpansions;      /* One macro expansion more */
    E->LocalStart       = LocalName;
    LocalName          += M->LocalCount;
    E->ParamCount       = 0;
    E->Params           = xmalloc (M->ParamCount * sizeof (TokNode*));
    for (I = 0; I < M->ParamCount; ++I) {
        E->Params[I] = 0;
    }
    E->ParamExp         = 0;
    E->LI               = 0;
    E->ParamLI          = 0;

    /* Mark the macro as expanding */
    ++M->Expansions;

    /* Return the new macro expansion */
    return E;
}



static void FreeMacExp (MacExp* E)
/* Remove and free the current macro expansion */
{
    unsigned I;

    /* One macro expansion less */
    --MacExpansions;

    /* No longer expanding this macro */
    --E->M->Expansions;

    /* Free the parameter lists */
    for (I = 0; I < E->ParamCount; ++I) {
        /* Free one parameter list */
        TokNode* N = E->Params[I];
        while (N) {
            TokNode* P = N->Next;
            FreeTokNode (N);
            N = P;
        }
    }
    xfree (E->Params);

    /* Free the additional line info */
    if (E->ParamLI) {
        EndLine (E->ParamLI);
    }
    if (E->LI) {
        EndLine (E->LI);
    }

    /* Free the final token if we have one */
    if (E->Final) {
        FreeTokNode (E->Final);
    }

    /* Free the structure itself */
    xfree (E);
}



static void MacSkipDef (unsigned Style)
/* Skip a macro definition */
{
    if (Style == MAC_STYLE_CLASSIC) {
        /* Skip tokens until we reach the final .endmacro */
        while (CurTok.Tok != TOK_ENDMACRO && CurTok.Tok != TOK_EOF) {
            NextTok ();
        }
        if (CurTok.Tok != TOK_EOF) {
            SkipUntilSep ();
        } else {
            Error ("`.ENDMACRO' expected");
        }
    } else {
        /* Skip until end of line */
        SkipUntilSep ();
    }
}



void MacDef (unsigned Style)
/* Parse a macro definition */
{
    Macro* M;
    TokNode* N;
    int HaveParams;

    /* We expect a macro name here */
    if (CurTok.Tok != TOK_IDENT) {
        Error ("Identifier expected");
        MacSkipDef (Style);
        return;
    } else if (!UbiquitousIdents && FindInstruction (&CurTok.SVal) >= 0) {
        /* The identifier is a name of a 6502 instruction, which is not
        ** allowed if not explicitly enabled.
        */
        Error ("Cannot use an instruction as macro name");
        MacSkipDef (Style);
        return;
    }

    /* Did we already define that macro? */
    if (HT_Find (&MacroTab, &CurTok.SVal) != 0) {
        /* Macro is already defined */
        Error ("A macro named `%m%p' is already defined", &CurTok.SVal);
        /* Skip tokens until we reach the final .endmacro */
        MacSkipDef (Style);
        return;
    }

    /* Define the macro */
    M = NewMacro (&CurTok.SVal, Style);

    /* Switch to raw token mode and skip the macro name */
    EnterRawTokenMode ();
    NextTok ();

    /* If we have a DEFINE-style macro, we may have parameters in parentheses;
    ** otherwise, we may have parameters without parentheses.
    */
    if (Style == MAC_STYLE_CLASSIC) {
        HaveParams = 1;
    } else {
        if (CurTok.Tok == TOK_LPAREN) {
            HaveParams = 1;
            NextTok ();
        } else {
            HaveParams = 0;
        }
    }

    /* Parse the parameter list */
    if (HaveParams) {

        while (CurTok.Tok == TOK_IDENT) {

            /* Create a struct holding the identifier */
            IdDesc* I = NewIdDesc (&CurTok.SVal);

            /* Insert the struct into the list, checking for duplicate idents */
            if (M->ParamCount == 0) {
                M->Params = I;
            } else {
                IdDesc* List = M->Params;
                while (1) {
                    if (SB_Compare (&List->Id, &CurTok.SVal) == 0) {
                        Error ("Duplicate symbol `%m%p'", &CurTok.SVal);
                    }
                    if (List->Next == 0) {
                        break;
                    } else {
                        List = List->Next;
                    }
                }
                List->Next = I;
            }
            ++M->ParamCount;

            /* Skip the name */
            NextTok ();

            /* Maybe there are more params... */
            if (CurTok.Tok == TOK_COMMA) {
                NextTok ();
            } else {
                break;
            }
        }
    }

    /* For classic macros, we expect a separator token, for define-style macros,
    ** we expect the closing paren.
    */
    if (Style == MAC_STYLE_CLASSIC) {
        ConsumeSep ();
    } else if (HaveParams) {
        ConsumeRParen ();
    }

    /* Preparse the macro body. We will read the tokens until we reach end of
    ** file, or a .endmacro (or end of line for DEFINE-style macros) and store
    ** them into a token list internal to the macro. For classic macros,
    ** the .LOCAL command is detected and removed, at this time.
    */
    while (1) {

        /* Check for end of macro */
        if (Style == MAC_STYLE_CLASSIC) {
            /* In classic macros, only .endmacro is allowed */
            if (CurTok.Tok == TOK_ENDMACRO) {
                /* Done */
                break;
            }
            /* May not have end of file in a macro definition */
            if (CurTok.Tok == TOK_EOF) {
                Error ("`.ENDMACRO' expected");
                goto Done;
            }
        } else {
            /* Accept a newline or end of file for new style macros */
            if (TokIsSep (CurTok.Tok)) {
                break;
            }
        }

        /* Check for a .LOCAL declaration */
        if (CurTok.Tok == TOK_LOCAL && Style == MAC_STYLE_CLASSIC) {

            while (1) {

                IdDesc* I;

                /* Skip .local or comma */
                NextTok ();

                /* Need an identifer */
                if (CurTok.Tok != TOK_IDENT && CurTok.Tok != TOK_LOCAL_IDENT) {
                    Error ("Identifier expected");
                    SkipUntilSep ();
                    break;
                }

                /* Put the identifier into the locals list and skip it */
                I = NewIdDesc (&CurTok.SVal);
                I->Next = M->Locals;
                M->Locals = I;
                ++M->LocalCount;
                NextTok ();

                /* Check for end of list */
                if (CurTok.Tok != TOK_COMMA) {
                    break;
                }

            }

            /* We need end of line after the locals */
            ConsumeSep ();
            continue;
        }

        /* Create a token node for the current token */
        N = NewTokNode ();

        /* If the token is an identifier, check if it is a local parameter */
        if (CurTok.Tok == TOK_IDENT) {
            unsigned Count = 0;
            IdDesc* I = M->Params;
            while (I) {
                if (SB_Compare (&I->Id, &CurTok.SVal) == 0) {
                    /* Local param name, replace it */
                    N->T.Tok  = TOK_MACPARAM;
                    N->T.IVal = Count;
                    break;
                }
                ++Count;
                I = I->Next;
            }
        }

        /* Insert the new token in the list */
        if (M->TokCount == 0) {
            /* First token */
            M->TokRoot = M->TokLast = N;
        } else {
            /* We have already tokens */
            M->TokLast->Next = N;
            M->TokLast = N;
        }
        ++M->TokCount;

        /* Read the next token */
        NextTok ();
    }

    /* Skip the .endmacro for a classic macro */
    if (Style == MAC_STYLE_CLASSIC) {
        NextTok ();
    }

    /* Reset the Incomplete flag now that parsing is done */
    M->Incomplete = 0;

Done:
    /* Switch out of raw token mode */
    LeaveRawTokenMode ();
}



void MacUndef (const StrBuf* Name, unsigned char Style)
/* Undefine the macro with the given name and style. A style mismatch is
** treated as if the macro didn't exist.
*/
{
    /* Search for the macro */
    Macro* M = HT_Find (&MacroTab, Name);

    /* Don't let the user kid with us */
    if (M == 0 || M->Style != Style) {
        Error ("No such macro: %m%p", Name);
        return;
    }
    if (M->Expansions > 0) {
        Error ("Cannot delete a macro that is currently expanded");
        return;
    }

    /* Remove the macro from the macro table */
    HT_Remove (&MacroTab, M);

    /* Free the macro structure */
    FreeMacro (M);
}



static int MacExpand (void* Data)
/* If we're currently expanding a macro, set the the scanner token and
** attribute to the next value and return true. If we are not expanding
** a macro, return false.
*/
{
    /* Cast the Data pointer to the actual data structure */
    MacExp* Mac = (MacExp*) Data;

    /* Check if we should abort this macro */
    if (DoMacAbort) {

        /* Reset the flag */
        DoMacAbort = 0;

        /* Abort any open .IF statements in this macro expansion */
        CleanupIfStack (Mac->IfSP);

        /* Terminate macro expansion */
        goto MacEnd;
    }

    /* We're expanding a macro. Check if we are expanding one of the
    ** macro parameters.
    */
ExpandParam:
    if (Mac->ParamExp) {

        /* Ok, use token from parameter list */
        TokSet (Mac->ParamExp);

        /* Create new line info for this parameter token */
        if (Mac->ParamLI) {
            EndLine (Mac->ParamLI);
        }
        Mac->ParamLI = StartLine (&CurTok.Pos, LI_TYPE_MACPARAM, Mac->MacExpansions);

        /* Set pointer to next token */
        Mac->ParamExp = Mac->ParamExp->Next;

        /* Done */
        return 1;

    } else if (Mac->ParamLI) {

        /* There's still line info open from the parameter expansion - end it */
        EndLine (Mac->ParamLI);
        Mac->ParamLI = 0;

    }

    /* We're not expanding macro parameters. Check if we have tokens left from
    ** the macro itself.
    */
    if (Mac->Exp) {

        /* Use next macro token */
        TokSet (Mac->Exp);

        /* Create new line info for this token */
        if (Mac->LI) {
            EndLine (Mac->LI);
        }
        Mac->LI = StartLine (&CurTok.Pos, LI_TYPE_MACRO, Mac->MacExpansions);

        /* Set pointer to next token */
        Mac->Exp = Mac->Exp->Next;

        /* Is it a request for actual parameter count? */
        if (CurTok.Tok == TOK_PARAMCOUNT) {
            CurTok.Tok  = TOK_INTCON;
            CurTok.IVal = Mac->ParamCount;
            return 1;
        }

        /* Is it the name of a macro parameter? */
        if (CurTok.Tok == TOK_MACPARAM) {

            /* Start to expand the parameter token list */
            Mac->ParamExp = Mac->Params[CurTok.IVal];

            /* Go back and expand the parameter */
            goto ExpandParam;
        }

        /* If it's an identifier, it may in fact be a local symbol */
        if ((CurTok.Tok == TOK_IDENT || CurTok.Tok == TOK_LOCAL_IDENT) &&
            Mac->M->LocalCount) {
            /* Search for the local symbol in the list */
            unsigned Index = 0;
            IdDesc* I = Mac->M->Locals;
            while (I) {
                if (SB_Compare (&CurTok.SVal, &I->Id) == 0) {
                    /* This is in fact a local symbol, change the name. Be sure
                    ** to generate a local label name if the original name was
                    ** a local label, and also generate a name that cannot be
                    ** generated by a user.
                    */
                    if (SB_At (&I->Id, 0) == LocalStart) {
                        /* Must generate a local symbol */
                        SB_Printf (&CurTok.SVal, "%cLOCAL-MACRO_SYMBOL-%04X",
                                   LocalStart, Mac->LocalStart + Index);
                    } else {
                        /* Global symbol */
                        SB_Printf (&CurTok.SVal, "LOCAL-MACRO_SYMBOL-%04X",
                                   Mac->LocalStart + Index);
                    }
                    break;
                }
                /* Next symbol */
                ++Index;
                I = I->Next;
            }

            /* Done */
            return 1;
        }

        /* The token was successfully set */
        return 1;
    }

    /* No more macro tokens. Do we have a final token? */
    if (Mac->Final) {

        /* Set the final token and remove it */
        TokSet (Mac->Final);
        FreeTokNode (Mac->Final);
        Mac->Final = 0;

        /* Problem: When a .define-style macro is expanded within the call
        ** of a classic one, the latter may be terminated and removed while
        ** the expansion of the .define-style macro is still active. Because
        ** line info slots are "stacked", this runs into a CHECK FAILED. For
        ** now, we will fix that by removing the .define-style macro expansion
        ** immediately, once the final token is placed. The better solution
        ** would probably be to not require AllocLineInfoSlot/FreeLineInfoSlot
        ** to be called in FIFO order, but this is a bigger change.
        */
        /* End of macro expansion and pop the input function */
        FreeMacExp (Mac);
        PopInput ();

        /* The token was successfully set */
        return 1;
    }

MacEnd:
    /* End of macro expansion */
    FreeMacExp (Mac);

    /* Pop the input function */
    PopInput ();

    /* No token available */
    return 0;
}



static void StartExpClassic (MacExp* E)
/* Start expanding a classic macro */
{
    token_t Term;

    /* Skip the macro name */
    NextTok ();

    /* Does this invocation have any arguments? */
    if (!TokIsSep (CurTok.Tok)) {

        /* Read the actual parameters */
        while (1) {
            TokNode* Last;

            /* Check for maximum parameter count */
            if (E->ParamCount >= E->M->ParamCount) {
                ErrorSkip ("Too many macro parameters");
                break;
            }

            /* The macro argument optionally may be enclosed in curly braces */
            Term = GetTokListTerm (TOK_COMMA);

            /* Read tokens for one parameter, accept empty params */
            Last = 0;
            while (CurTok.Tok != Term && CurTok.Tok != TOK_SEP) {
                TokNode* T;

                /* Check for end of file */
                if (CurTok.Tok == TOK_EOF) {
                    Error ("Unexpected end of file");
                    FreeMacExp (E);
                    return;
                }

                /* Get the next token in a node */
                T = NewTokNode ();

                /* Insert it into the list */
                if (Last == 0) {
                    E->Params [E->ParamCount] = T;
                } else {
                    Last->Next = T;
                }
                Last = T;

                /* And skip it... */
                NextTok ();
            }

            /* One parameter more */
            ++E->ParamCount;

            /* If the macro argument was enclosed in curly braces, end-of-line
            ** is an error. Skip the closing curly brace.
            */
            if (Term == TOK_RCURLY) {
                if (CurTok.Tok == TOK_SEP) {
                    Error ("End of line encountered within macro argument");
                    break;
                }
                NextTok ();
            }

            /* Check for a comma */
            if (CurTok.Tok == TOK_COMMA) {
                NextTok ();
            } else {
                break;
            }
        }
    }

    /* We must be at end of line now, otherwise something is wrong */
    ExpectSep ();

    /* Insert a new token input function */
    PushInput (MacExpand, E, ".MACRO");
}



static void StartExpDefine (MacExp* E)
/* Start expanding a DEFINE-style macro */
{
    /* A define-style macro must be called with as many actual parameters
    ** as there are formal ones. Get the parameter count.
    */
    unsigned Count = E->M->ParamCount;

    /* Skip the current token */
    NextTok ();

    /* Read the actual parameters */
    while (Count--) {
        TokNode* Last;

        /* The macro argument optionally may be enclosed in curly braces */
        token_t Term = GetTokListTerm (TOK_COMMA);

        /* Check if there is really a parameter */
        if (TokIsSep (CurTok.Tok) || CurTok.Tok == Term) {
            ErrorSkip ("Macro parameter #%u is empty", E->ParamCount+1);
            FreeMacExp (E);
            return;
        }

        /* Read tokens for one parameter */
        Last = 0;
        do {
            TokNode* T;

            /* Get the next token in a node */
            T = NewTokNode ();

            /* Insert it into the list */
            if (Last == 0) {
                E->Params [E->ParamCount] = T;
            } else {
                Last->Next = T;
            }
            Last = T;

            /* And skip it... */
            NextTok ();

        } while (CurTok.Tok != Term && !TokIsSep (CurTok.Tok));

        /* One parameter more */
        ++E->ParamCount;

        /* If the macro argument was enclosed in curly braces, end-of-line
        ** is an error. Skip the closing curly brace.
        */
        if (Term == TOK_RCURLY) {
            if (TokIsSep (CurTok.Tok)) {
                Error ("End of line encountered within macro argument");
                break;
            }
            NextTok ();
        }

        /* Check for a comma */
        if (Count > 0) {
            if (CurTok.Tok == TOK_COMMA) {
                NextTok ();
            } else {
                Error ("`,' expected");
            }
        }
    }

    /* Macro expansion will overwrite the current token. This is a problem
    ** for define-style macros since these are called from the scanner level.
    ** To avoid it, remember the current token and re-insert it, once macro
    ** expansion is done.
    */
    E->Final = NewTokNode ();

    /* Insert a new token input function */
    PushInput (MacExpand, E, ".DEFINE");
}



void MacExpandStart (Macro* M)
/* Start expanding a macro */
{
    MacExp* E;

    /* Check the argument */
    PRECONDITION (M && (M->Style != MAC_STYLE_DEFINE || DisableDefines == 0));

    /* We cannot expand an incomplete macro */
    if (M->Incomplete) {
        Error ("Cannot expand an incomplete macro");
        return;
    }

    /* Don't allow too many nested macro expansions - otherwise it is possible
    ** to force an endless loop and assembler crash.
    */
    if (MacExpansions >= MAX_MACEXPANSIONS) {
        Error ("Too many nested macro expansions");
        return;
    }

    /* Create a structure holding expansion data */
    E = NewMacExp (M);

    /* Call the apropriate subroutine */
    switch (M->Style) {
        case MAC_STYLE_CLASSIC: StartExpClassic (E);    break;
        case MAC_STYLE_DEFINE:  StartExpDefine (E);     break;
        default:                Internal ("Invalid macro style: %d", M->Style);
    }
}



void MacAbort (void)
/* Abort the current macro expansion */
{
    /* Must have an expansion */
    CHECK (MacExpansions > 0);

    /* Set a flag so macro expansion will terminate on the next call */
    DoMacAbort = 1;
}



Macro* FindMacro (const StrBuf* Name)
/* Try to find the macro with the given name and return it. If no macro with
** this name was found, return NULL.
*/
{
    Macro* M = HT_Find (&MacroTab, Name);
    return (M != 0 && M->Style == MAC_STYLE_CLASSIC)? M : 0;
}



Macro* FindDefine (const StrBuf* Name)
/* Try to find the define-style macro with the given name; and, return it.
** If no such macro was found, return NULL.
*/
{
    Macro* M;

    /* Never if disabled */
    if (DisableDefines) {
        return 0;
    }

    /* Check if we have such a macro */
    M = HT_Find (&MacroTab, Name);
    return (M != 0 && M->Style == MAC_STYLE_DEFINE)? M : 0;
}



int InMacExpansion (void)
/* Return true if we're currently expanding a macro */
{
    return (MacExpansions > 0);
}



void DisableDefineStyleMacros (void)
/* Disable define-style macros until EnableDefineStyleMacros() is called */
{
    ++DisableDefines;
}



void EnableDefineStyleMacros (void)
/* Re-enable define-style macros previously disabled with
** DisableDefineStyleMacros().
*/
{
    PRECONDITION (DisableDefines > 0);
    --DisableDefines;
}
