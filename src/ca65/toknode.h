/*****************************************************************************/
/*                                                                           */
/*			       	   toknode.h				     */
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



#ifndef TOKNODE_H
#define TOKNODE_H



/*****************************************************************************/
/*     	       	    		     Data				     */
/*****************************************************************************/



/* Struct holding a token */
typedef struct TokNode_ TokNode;
struct TokNode_ {
    TokNode*   		Next;		/* For single linked list */
    enum Token		Tok;		/* Token value */
    int	       	       	WS;    		/* Whitespace before token? */
    long       		IVal;		/* Integer token attribute */
    char       		SVal [1];	/* String attribute, dyn. allocated */
};



/* Return codes for TokCmp - higher numeric code means better match */
enum TC {
    tcDifferent,			/* Different tokents */
    tcSameToken,			/* Same token, different attribute */
    tcIdentical				/* Identical (token + attribute) */
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



/* End of toknode.h */

#endif




