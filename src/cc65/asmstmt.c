/*****************************************************************************/
/*                                                                           */
/*	       			   asmstmt.c                                 */
/*                                                                           */
/*	      Inline assembler statements for the cc65 C compiler            */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2001    	 Ullrich von Bassewitz                                       */
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



#include <string.h>

/* cc65 */	    
#include "codegen.h"
#include "error.h"
#include "litpool.h"
#include "scanner.h"
#include "asmstmt.h"



/*****************************************************************************/
/*	      	  		     Code		     		     */
/*****************************************************************************/



void AsmStatement (void)
/* This function parses ASM statements. The syntax of the ASM directive
 * looks like the one defined for C++ (C has no ASM directive), that is,
 * a string literal in parenthesis.
 */
{
    /* Skip the ASM */
    NextToken ();

    /* Need left parenthesis */
    ConsumeLParen ();

    /* String literal */
    if (CurTok.Tok != TOK_SCONST) {
     	Error ("String literal expected");
    } else {

     	/* The string literal may consist of more than one line of assembler
     	 * code. Separate the single lines and output the code.
     	 */
     	const char* S = GetLiteral (CurTok.IVal);
     	while (*S) {

       	    /* Separate the lines */
     	    const char* E = strchr (S, '\n');
     	    if (E) {
     		/* Found a newline */
     		g_asmcode (S, E-S);
     		S = E+1;
     	    } else {
     		int Len = strlen (S);
     		g_asmcode (S, Len);
     		S += Len;
     	    }
     	}

     	/* Reset the string pointer, effectivly clearing the string from the
     	 * string table. Since we're working with one token lookahead, this
     	 * will fail if the next token is also a string token, but that's a
     	 * syntax error anyway, because we expect a right paren.
     	 */
     	ResetLiteralPoolOffs (CurTok.IVal);
    }

    /* Skip the string token */
    NextToken ();

    /* Closing paren needed */
    ConsumeRParen ();
}



