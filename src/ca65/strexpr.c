/*****************************************************************************/
/*                                                                           */
/*		  		   strexpr.c				     */
/*                                                                           */
/*		String expressions for the ca65 macroassembler		     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000      Ullrich von Bassewitz                                       */
/*               Wacholderweg 14                                             */
/*               D-70597 Stuttgart                                           */
/* EMail:        uz@musoftware.de                                            */
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

#include "error.h"
#include "expr.h"
#include "scanner.h"
#include "strexpr.h"



/*****************************************************************************/
/*     	      	     		     Code		     		     */
/*****************************************************************************/



const char* StringExpression (void)
/* Evaluate a string expression. If there are no errors, the function will
 * place the string into the token attribute buffer SVal and the token will
 * be TOK_STRCON. A pointer to the buffer is returned.
 * If there was an error, a NULL pointer is returned.
 */
{
    char Buf [sizeof (SVal)];

    /* Check for a string constant or a function that returns a string */
    switch (Tok) {

   	case TOK_STRING:
   	    NextTok ();
   	    ConsumeLParen ();
   	    if (Tok == TOK_IDENT) {
   	      	/* Save the identifier, then skip it */
   	      	strcpy (Buf, SVal);
   	      	NextTok ();
   	    } else {
	 	/* Numeric expression */
		long Val = ConstExpression ();
		sprintf (Buf, "%ld", Val);
	    }
   	    if (Tok != TOK_RPAREN) {
   	      	Error (ERR_RPAREN_EXPECTED);
   	    }
   	    /* Overwrite the token, do not skip it! */
   	    strcpy (SVal, Buf);
   	    Tok = TOK_STRCON;
   	    break;

	case TOK_STRCON:
	    /* We already have a string */
	    break;

	default:
	    /* Error - no string constant */
	    return 0;
    }

    /* Return a pointer to the buffer */
    return SVal;
}



