/*****************************************************************************/
/*                                                                           */
/*				   repeat.c				     */
/*                                                                           */
/*		     Handle the .REPEAT pseudo instruction		     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000     Ullrich von Bassewitz                                        */
/*              Wacholderweg 14                                              */
/*              D-70597 Stuttgart                                            */
/* EMail:       uz@musoftware.de                                             */
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
/*     	       	    	  	     Code			   	     */
/*****************************************************************************/



static TokList* CollectRepeatTokens (void)
/* Collect all tokens inside the .REPEAT body in a token list and return
 * this list. In case of errors, NULL is returned.
 */
{
    /* Create the token list */
    TokList* List = NewTokList ();

    /* Read the token list */
    unsigned Repeats = 0;
    while (Repeats != 0 || Tok != TOK_ENDREP) {

     	/* Check for end of input */
       	if (Tok == TOK_EOF) {
     	    Error (ERR_UNEXPECTED_EOF);
	    FreeTokList (List);
     	    return 0;
     	}

	/* If we find a token that is equal to the repeat counter name,
	 * replace it by a REPCOUNTER token. This way we have to do strcmps
	 * only once for each identifier, and not for each expansion.
	 * Note: This will fail for nested repeats using the same repeat
	 * counter name, but
	 */



       	/* Collect all tokens in the list */
	AddCurTok (List);

     	/* Check for and count nested .REPEATs */
     	if (Tok == TOK_REPEAT) {
     	    ++Repeats;
     	} else if (Tok == TOK_ENDREP) {
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
 * for and replace identifiers that are the repeat counter.
 */
{
    if (Tok == TOK_IDENT && L->Data != 0 && strcmp (SVal, L->Data) == 0) {
 	/* Must replace by the repeat counter */
 	Tok  = TOK_INTCON;
 	IVal = L->RepCount;
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
	Error (ERR_RANGE);
	RepCount = 0;
    }

    /* Optional there is a comma and a counter variable */
    Name = 0;
    if (Tok == TOK_COMMA) {

       	/* Skip the comma */
       	NextTok ();

       	/* Check for an identifier */
       	if (Tok != TOK_IDENT) {
       	    ErrorSkip (ERR_IDENT_EXPECTED);
       	} else {
       	    /* Remember the name and skip it */
       	    Name = xstrdup (SVal);
       	    NextTok ();
       	}
    }

    /* Separator */
    ConsumeSep ();

    /* Read the token list */
    List = CollectRepeatTokens ();

    /* If we had an error, bail out */
    if (List == 0) {
	xfree (Name);
       	return;
    }

    /* Update the token list for replay */
    List->RepMax = (unsigned) RepCount;
    List->Data   = Name;
    List->Check  = RepeatTokenCheck;

    /* If the list is empty, or repeat count zero, there is nothing
     * to repeat.
     */
    if (List->Count == 0 || RepCount == 0) {
     	FreeTokList (List);
	return;
    }

    /* Read input from the repeat descriptor */
    PushTokList (List, ".REPEAT");
}



