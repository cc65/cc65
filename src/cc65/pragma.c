/*****************************************************************************/
/*                                                                           */
/*				   pragma.c				     */
/*                                                                           */
/*		    Pragma handling for the cc65 C compiler		     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2000 Ullrich von Bassewitz                                       */
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



#include <stdlib.h>
#include <string.h>

/* cc65 */
#include "codegen.h"
#include "error.h"
#include "expr.h"
#include "global.h"
#include "litpool.h"
#include "scanner.h"
#include "segname.h"
#include "symtab.h"
#include "pragma.h"



/*****************************************************************************/
/*		     		     data				     */
/*****************************************************************************/



/* Tokens for the #pragmas */
typedef enum {
    PR_BSSSEG,
    PR_CODESEG,
    PR_DATASEG,
    PR_REGVARADDR,
    PR_RODATASEG,
    PR_SIGNEDCHARS,
    PR_STATICLOCALS,
    PR_ZPSYM,
    PR_ILLEGAL
} pragma_t;

/* Pragma table */
static const struct Pragma {
    const char*	Key;		/* Keyword */
    pragma_t	Tok;		/* Token */
} Pragmas[] = {
    { 	"bssseg",       PR_BSSSEG	},
    {   "codeseg",    	PR_CODESEG	},
    {   "dataseg",    	PR_DATASEG	},
    {   "regvaraddr", 	PR_REGVARADDR	},
    {   "rodataseg",  	PR_RODATASEG	},
    {	"signedchars",	PR_SIGNEDCHARS	},
    {	"staticlocals",	PR_STATICLOCALS	},
    {   "zpsym",       	PR_ZPSYM  	},
};

/* Number of pragmas */
#define PRAGMA_COUNT	(sizeof(Pragmas) / sizeof(Pragmas[0]))



/*****************************************************************************/
/*    	      	     	   	     Code  				     */
/*****************************************************************************/



static int CmpKey (const void* Key, const void* Elem)
/* Compare function for bsearch */
{
    return strcmp ((const char*) Key, ((const struct Pragma*) Elem)->Key);
}



static pragma_t FindPragma (const char* Key)
/* Find a pragma and return the token. Return PR_ILLEGAL if the keyword is
 * not a valid pragma.
 */
{
    struct Pragma* P;
    P = bsearch (Key, Pragmas, PRAGMA_COUNT, sizeof (Pragmas[0]), CmpKey);
    return P? P->Tok : PR_ILLEGAL;
}



static void StringPragma (void (*Func) (const char*))
/* Handle a pragma that expects a string parameter */
{
    if (curtok != TOK_SCONST) {
	Error ("String literal expected");
    } else {
     	/* Get the string */
     	const char* Name = GetLiteral (curval);

       	/* Call the given function with the string argument */
	Func (Name);

     	/* Reset the string pointer, removing the string from the pool */
     	ResetLiteralOffs (curval);
    }

    /* Skip the string (or error) token */
    NextToken ();
}



static void SegNamePragma (void (*Func) (const char*))
/* Handle a pragma that expects a segment name parameter */
{
    if (curtok != TOK_SCONST) {
	Error ("String literal expected");
    } else {
     	/* Get the segment name */
     	const char* Name = GetLiteral (curval);

	/* Check if the name is valid */
	if (ValidSegName (Name)) {

       	    /* Call the given function to set the name */
	    Func (Name);

	} else {

	    /* Segment name is invalid */
	    Error ("Illegal segment name: `%s'", Name);

	}

     	/* Reset the string pointer, removing the string from the pool */
     	ResetLiteralOffs (curval);
    }

    /* Skip the string (or error) token */
    NextToken ();
}



static void FlagPragma (unsigned char* Flag)
/* Handle a pragma that expects a boolean paramater */
{
    /* Read a constant expression */
    struct expent val;
    constexpr (&val);

    /* Store the value into the flag parameter */
    *Flag = (val.e_const != 0);
}



void DoPragma (void)
/* Handle pragmas */
{
    pragma_t Pragma;

    /* Skip the token itself */
    NextToken ();

    /* Identifier must follow */
    if (curtok != TOK_IDENT) {
	Error ("Identifier expected");
	return;
    }

    /* Do we know this pragma? */
    Pragma = FindPragma (CurTok.Ident);
    if (Pragma == PR_ILLEGAL) {
	/* According to the ANSI standard, we're not allowed to generate errors
	 * for unknown pragmas, however, we're allowed to warn - and we will
	 * do so. Otherwise one typo may give you hours of bug hunting...
	 */
    	Warning ("Unknown #pragma `%s'", CurTok.Ident);
     	return;
    }

    /* Skip the identifier and check for an open paren */
    NextToken ();
    ConsumeLParen ();

    /* Switch for the different pragmas */
    switch (Pragma) {

	case PR_BSSSEG:
	    SegNamePragma (g_bssname);
	    break;

	case PR_CODESEG:
	    SegNamePragma (g_codename);
	    break;

	case PR_DATASEG:
	    SegNamePragma (g_dataname);
	    break;

	case PR_REGVARADDR:
	    FlagPragma (&AllowRegVarAddr);
	    break;

	case PR_RODATASEG:
	    SegNamePragma (g_rodataname);
	    break;

	case PR_SIGNEDCHARS:
	    FlagPragma (&SignedChars);
	    break;

	case PR_STATICLOCALS:
	    FlagPragma (&StaticLocals);
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



