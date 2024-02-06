/*****************************************************************************/
/*                                                                           */
/*                                 toklist.c                                 */
/*                                                                           */
/*                  Token list for the ca65 macroassembler                   */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2012, Ullrich von Bassewitz                                      */
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



#include <string.h>

/* common */
#include "check.h"
#include "xmalloc.h"

/* ca65 */
#include "error.h"
#include "istack.h"
#include "lineinfo.h"
#include "nexttok.h"
#include "scanner.h"
#include "toklist.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Number of currently pushed token lists */
static unsigned PushCounter = 0;



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



TokNode* NewTokNode (void)
/* Create and return a token node with the current token value */
{

    /* Allocate memory */
    TokNode* N = xmalloc (sizeof (TokNode));

    /* Initialize the token contents */
    N->Next     = 0;
    SB_Init (&N->T.SVal);
    CopyToken (&N->T, &CurTok);

    /* Return the node */
    return N;
}



void FreeTokNode (TokNode* N)
/* Free the given token node */
{
    SB_Done (&N->T.SVal);
    xfree (N);
}



void TokSet (TokNode* N)
/* Set the scanner token from the given token node. */
{
    /* Set the values */
    CopyToken (&CurTok, &N->T);
    SB_Terminate (&CurTok.SVal);
}



enum TC TokCmp (const TokNode* N)
/* Compare the token given as parameter against the current token */
{
    if (N->T.Tok != CurTok.Tok) {
        /* Different token */
        return tcDifferent;
    }

    /* If the token has string attribute, check it */
    if (TokHasSVal (N->T.Tok)) {
        if (SB_Compare (&CurTok.SVal, &N->T.SVal) != 0) {
            return tcSameToken;
        }
    } else if (TokHasIVal (N->T.Tok)) {
        if (N->T.IVal != CurTok.IVal) {
            return tcSameToken;
        }
    }

    /* Tokens are identical */
    return tcIdentical;
}



TokList* NewTokList (void)
/* Create a new, empty token list */
{
    /* Allocate memory for the list structure */
    TokList* T = xmalloc (sizeof (TokList));

    /* Initialize the fields */
    T->Next     = 0;
    T->Root     = 0;
    T->Last     = 0;
    T->RepCount = 0;
    T->RepMax   = 1;
    T->Count    = 0;
    T->Check    = 0;
    T->Data     = 0;
    T->LI       = 0;

    /* Return the new list */
    return T;
}



void FreeTokList (TokList* List)
/* Delete the token list including all token nodes */
{
    /* Free the token list */
    TokNode* T = List->Root;
    while (T) {
        TokNode* Tmp = T;
        T = T->Next;
        FreeTokNode (Tmp);
    }

    /* Free associated line info */
    if (List->LI) {
        EndLine (List->LI);
    }

    /* If we have associated data, free it */
    if (List->Data) {
        xfree (List->Data);
    }

    /* Free the list structure itself */
    xfree (List);
}



enum token_t GetTokListTerm (enum token_t Term)
/* Determine if the following token list is enclosed in curly braces. This is
** the case if the next token is the opening brace. If so, skip it and return
** a closing brace, otherwise return Term.
*/
{
    if (CurTok.Tok == TOK_LCURLY) {
        NextTok ();
        return TOK_RCURLY;
    } else {
        return Term;
    }
}



void AddCurTok (TokList* List)
/* Add the current token to the token list */
{
    /* Create a token node with the current token value */
    TokNode* T = NewTokNode ();

    /* Insert the node into the list */
    if (List->Root == 0) {
        List->Root = T;
    } else {
        List->Last->Next = T;
    }
    List->Last = T;

    /* Count nodes */
    List->Count++;
}



static int ReplayTokList (void* List)
/* Function that gets the next token from a token list and sets it. This
** function may be used together with the PushInput function from the istack
** module.
*/
{
    /* Cast the generic pointer to an actual list */
    TokList* L = List;

    /* If there are no more tokens, decrement the repeat counter. If it goes
    ** zero, delete the list and remove the function from the stack.
    */
    if (L->Last == 0) {
        if (++L->RepCount >= L->RepMax) {
            /* Done with this list */
            FreeTokList (L);
            --PushCounter;
            PopInput ();
            return 0;
        } else {
            /* Replay one more time */
            L->Last = L->Root;
        }
    }

    /* Set the next token from the list */
    TokSet (L->Last);

    /* Set the line info for the new token */
    if (L->LI) {
        EndLine (L->LI);
    }
    L->LI = StartLine (&CurTok.Pos, LI_TYPE_ASM, PushCounter);

    /* If a check function is defined, call it, so it may look at the token
    ** just set and changed it as apropriate.
    */
    if (L->Check) {
        L->Check (L);
    }

    /* Set the pointer to the next token */
    L->Last = L->Last->Next;

    /* We have a token */
    return 1;
}



void PushTokList (TokList* List, const char* Desc)
/* Push a token list to be used as input for InputFromStack. This includes
** several initializations needed in the token list structure, so don't use
** PushInput directly.
*/
{
    /* If the list is empty, just delete it and bail out */
    if (List->Count == 0) {
        FreeTokList (List);
        return;
    }

    /* Reset the last pointer to the first element */
    List->Last = List->Root;

    /* Insert the list specifying our input function */
    ++PushCounter;
    PushInput (ReplayTokList, List, Desc);
}
