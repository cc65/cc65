/*****************************************************************************/
/*                                                                           */
/*			       	   toknode.c				     */
/*                                                                           */
/*		  Token list node for the ca65 macroassembler		     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998     Ullrich von Bassewitz                                        */
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

#include "mem.h"
#include "scanner.h"
#include "toknode.h"



/*****************************************************************************/
/*     	       	    		     Code	       			     */
/*****************************************************************************/



TokNode* NewTokNode (void)
/* Create and return a token node with the current token value */
{
    TokNode* T;

    /* Allocate memory */
    unsigned Len = TokHasSVal (Tok)? strlen (SVal) : 0;
    T = Xmalloc (sizeof (TokNode) + Len);

    /* Initialize the token contents */
    T->Next   	= 0;
    T->Tok	= Tok;
    T->WS	= WS;
    T->IVal	= IVal;
    memcpy (T->SVal, SVal, Len);
    T->SVal [Len] = '\0';

    /* Return the node */
    return T;
}



void FreeTokNode (TokNode* T)
/* Free the given token node */
{
    Xfree (T);
}



void TokSet (TokNode* T)
/* Set the scanner token from the given token node */
{
    /* Set the values */
    Tok  = T->Tok;
    WS   = T->WS;
    IVal = T->IVal;
    strcpy (SVal, T->SVal);
}



enum TC TokCmp (const TokNode* T)
/* Compare the token given as parameter against the current token */
{
    if (T->Tok != Tok) {
	/* Different token */
	return tcDifferent;
    }
     
    /* If the token has string attribute, check it */
    if (TokHasSVal (T->Tok)) {
	if (strcmp  (T->SVal, SVal) != 0) {
     	    return tcSameToken;
	}
    } else if (TokHasIVal (T->Tok)) {
	if (T->IVal != IVal) {
     	    return tcSameToken;
	}
    }
	 
    /* Tokens are identical */
    return tcIdentical;
}



