/*****************************************************************************/
/*                                                                           */
/*				   pragma.c				     */
/*                                                                           */
/*		    Pragma handling for the cc65 C compiler		     */
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



#include <stdlib.h>
#include <ctype.h>

#include "global.h"
#include "error.h"
#include "io.h"
#include "litpool.h"
#include "symtab.h"
#include "preproc.h"
#include "scanner.h"
#include "codegen.h"
#include "expr.h"
#include "pragma.h"



/*****************************************************************************/
/*		     		     data				     */
/*****************************************************************************/



/* Tokens for the #pragmas */
enum {
    PR_BSSSEG,
    PR_CODESEG,
    PR_DATASEG,
    PR_REGVARADDR,
    PR_RODATASEG,
    PR_SIGNEDCHARS,
    PR_STATICLOCALS,
    PR_ZPSYM,
    PR_ILLEGAL
};



/*****************************************************************************/
/*    	      	     	   	     code  				     */
/*****************************************************************************/



static void StringPragma (void (*Func) (const char*))
/* Handle a pragma that expects a string parameter */
{
    if (curtok != SCONST) {
	Error (ERR_STRLIT_EXPECTED);
    } else {
     	/* Get the string */
     	const char* Name = GetLiteral (curval);

       	/* Call the given function with the string argument */
	Func (Name);

     	/* Reset the string pointer, removing the string from the pool */
     	ResetLiteralOffs (curval);
    }

    /* Skip the string (or error) token */
    gettok ();
}



static void FlagPragma (unsigned char* Flag)
/* Handle a pragma that expects a boolean paramater */
{
    /* Read a constant expression */
    struct expent val;
    constexpr (&val);

    /* Store the value into the flag parameter */
    *Flag = val.e_const;
}



void DoPragma (void)
/* Handle pragmas */
{
    static const struct tok_elt Pragmas [] = {
      	{ 	"bssseg",       PR_BSSSEG	},
       	{       "codeseg",  	PR_CODESEG	},
      	{       "dataseg",  	PR_DATASEG	},
       	{       "regvaraddr",	PR_REGVARADDR	},
      	{       "rodataseg",	PR_RODATASEG	},
	{	"signedchars",	PR_SIGNEDCHARS	},
	{	"staticlocals",	PR_STATICLOCALS	},
      	{       "zpsym",       	PR_ZPSYM  	},
      	{       0,     	   	PR_ILLEGAL	},
    };

    int Pragma;

    /* Skip the token itself */
    gettok ();

    /* Identifier must follow */
    if (curtok != IDENT) {
	Error (ERR_IDENT_EXPECTED);
	return;
    }

    /* Do we know this pragma? */
    Pragma = searchtok (CurTok.Ident, Pragmas);
    if (Pragma == PR_ILLEGAL) {
	/* According to the ANSI standard, we're not allowed to generate errors
	 * for unknown pragmas, however, we're allowed to warn - and we will
	 * do so. Otherwise one typo may give you hours of bug hunting...
	 */
    	Warning (WARN_UNKNOWN_PRAGMA);
     	return;
    }

    /* Skip the identifier and check for an open paren */
    gettok ();
    ConsumeLParen ();

    /* Switch for the different pragmas */
    switch (Pragma) {

	case PR_BSSSEG:
	    StringPragma (g_bssname);
	    break;

	case PR_CODESEG:
	    StringPragma (g_codename);
	    break;

	case PR_DATASEG:
	    StringPragma (g_dataname);
	    break;

	case PR_REGVARADDR:
	    FlagPragma (&AllowRegVarAddr);
	    break;

	case PR_RODATASEG:
	    StringPragma (g_rodataname);
	    break;

	case PR_SIGNEDCHARS:
	    FlagPragma (&SignedChars);
	    break;

	case PR_STATICLOCALS:
	    FlagPragma (&LocalsAreStatic);
	    break;

	case PR_ZPSYM:
	    StringPragma (MakeZPSym);
	    break;

	default:
       	    Internal ("Invalid pragma");
    }

    /* Closing paren needed */
    ConsumeRParen ();
}



