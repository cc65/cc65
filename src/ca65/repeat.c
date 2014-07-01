/*****************************************************************************/
/*                                                                           */
/*                                 repeat.c                                  */
/*                                                                           */
/*                   Handle the .REPEAT pseudo instruction                   */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000-2011, Ullrich von Bassewitz                                      */
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
#include "xmalloc.h"

/* ca65 */
#include "error.h"
#include "expr.h"
#include "nexttok.h"
#include "toklist.h"
#include "repeat.h"



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



static TokList* CollectRepeatTokens (void)
/* Collect all tokens inside the .REPEAT body in a token list and return
** this list. In case of errors, NULL is returned.
*/
{
    /* Create the token list */
    TokList* List = NewTokList ();

    /* Read the token list */
    unsigned Repeats = 0;
    while (Repeats != 0 || CurTok.Tok != TOK_ENDREP) {

        /* Check for end of input */
        if (CurTok.Tok == TOK_EOF) {
            Error ("Unexpected end of file");
            FreeTokList (List);
            return 0;
        }

        /* Collect all tokens in the list */
        AddCurTok (List);

        /* Check for and count nested .REPEATs */
        if (CurTok.Tok == TOK_REPEAT) {
            ++Repeats;
        } else if (CurTok.Tok == TOK_ENDREP) {
            --Repeats;
        }

        /* Get the next token */
        NextTok ();
    }

    /* Eat the closing .ENDREP */
    NextTok ();

    /* Return the list of collected tokens */
    return List;
}



static void RepeatTokenCheck (TokList* L)
/* Called each time a token from a repeat token list is set. Is used to check
** for and replace identifiers that are the repeat counter.
*/
{
    if (CurTok.Tok == TOK_IDENT &&
        L->Data != 0            &&
        SB_CompareStr (&CurTok.SVal, L->Data) == 0) {
        /* Must replace by the repeat counter */
        CurTok.Tok  = TOK_INTCON;
        CurTok.IVal = L->RepCount;
    }
}



void ParseRepeat (void)
/* Parse and handle the .REPEAT statement */
{
    char* Name;
    TokList* List;

    /* Repeat count follows */
    long RepCount = ConstExpression ();
    if (RepCount < 0) {
        Error ("Range error");
        RepCount = 0;
    }

    /* Optional there is a comma and a counter variable */
    Name = 0;
    if (CurTok.Tok == TOK_COMMA) {

        /* Skip the comma */
        NextTok ();

        /* Check for an identifier */
        if (CurTok.Tok != TOK_IDENT) {
            ErrorSkip ("Identifier expected");
        } else {
            /* Remember the name and skip it */
            SB_Terminate (&CurTok.SVal);
            Name = xstrdup (SB_GetConstBuf (&CurTok.SVal));
            NextTok ();
        }
    }

    /* Switch to raw token mode, then skip the separator */
    EnterRawTokenMode ();
    ConsumeSep ();

    /* Read the token list */
    List = CollectRepeatTokens ();

    /* If we had an error, bail out */
    if (List == 0) {
        xfree (Name);
        goto Done;
    }

    /* Update the token list for replay */
    List->RepMax = (unsigned) RepCount;
    List->Data   = Name;
    List->Check  = RepeatTokenCheck;

    /* If the list is empty, or repeat count zero, there is nothing
    ** to repeat.
    */
    if (List->Count == 0 || RepCount == 0) {
        FreeTokList (List);
        goto Done;
    }

    /* Read input from the repeat descriptor */
    PushTokList (List, ".REPEAT");

Done:
    /* Switch out of raw token mode */
    LeaveRawTokenMode ();
}
