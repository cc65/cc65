/*****************************************************************************/
/*                                                                           */
/*			       	   toklist.h				     */
/*                                                                           */
/*		    Token list for the ca65 macroassembler		     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000-2004 Ullrich von Bassewitz                                       */
/*               Römerstraße 52                                              */
/*               D-70794 Filderstadt                                         */
/* EMail:        uz@cc65.org                                                 */
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



#ifndef TOKLIST_H
#define TOKLIST_H



#include "scanner.h"



/*****************************************************************************/
/*     	       	    		     Data				     */
/*****************************************************************************/



/* Struct holding a token */
typedef struct TokNode TokNode;
struct TokNode {
    TokNode*	Next;  	      		/* For single linked list */
    enum Token	Tok;	      		/* Token value */
    int	       	WS;    	      		/* Whitespace before token? */
    long       	IVal;	      		/* Integer token attribute */
    char       	SVal [1];     		/* String attribute, dyn. allocated */
};

/* Struct holding a token list */
typedef struct TokList TokList;
struct TokList {
    TokList*	Next;	      		/* Single linked list (for replay) */
    TokNode*	Root;	      		/* First node in list */
    TokNode*	Last;	      		/* Last node in list or replay */
    unsigned   	RepCount;      		/* Repeat counter (used for replay) */
    unsigned	RepMax;			/* Maximum repeat count for replay */
    unsigned	Count;	      		/* Token count */
    void	(*Check)(TokList*);	/* Token check function */
    void*      	Data;			/* Additional data for check */
};



/* Return codes for TokCmp - higher numeric code means better match */
enum TC {
    tcDifferent, 			/* Different tokents */
    tcSameToken, 			/* Same token, different attribute */
    tcIdentical	 			/* Identical (token + attribute) */
};



/*****************************************************************************/
/*     	       	    		     Code			   	     */
/*****************************************************************************/



TokNode* NewTokNode (void);
/* Create and return a token node with the current token value */

void FreeTokNode (TokNode* T);
/* Free the given token node */

void TokSet (TokNode* T);
/* Set the scanner token from the given token node */

enum TC TokCmp (const TokNode* T);
/* Compare the token given as parameter against the current token */

void InitTokList (TokList* T);
/* Initialize a token list structure for later use */

TokList* NewTokList (void);
/* Create a new, empty token list */

void FreeTokList (TokList* T);
/* Delete the token list including all token nodes */

enum Token GetTokListTerm (enum Token Term);
/* Determine if the following token list is enclosed in curly braces. This is
 * the case if the next token is the opening brace. If so, skip it and return
 * a closing brace, otherwise return Term.
 */

void AddCurTok (TokList* T);
/* Add the current token to the token list */

void PushTokList (TokList* List, const char* Desc);
/* Push a token list to be used as input for InputFromStack. This includes
 * several initializations needed in the token list structure, so don't use
 * PushInput directly.
 */



/* End of toklist.h */

#endif




