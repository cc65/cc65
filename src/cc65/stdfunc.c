/*****************************************************************************/
/*                                                                           */
/*				   stdfunc.c				     */
/*                                                                           */
/*	   Handle inlining of known functions for the cc65 compiler	     */
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
#include <string.h>

/* common */
#include "check.h"

/* cc65 */
#include "codegen.h"
#include "error.h"
#include "global.h"
#include "scanner.h"
#include "stdfunc.h"



/*****************************************************************************/
/*			       Function forwards	    		     */
/*****************************************************************************/



static void StdFunc_strlen (struct expent*);



/*****************************************************************************/
/*  		    		     Data   	    	    		     */
/*****************************************************************************/



/* Table with all known functions and their handlers. Must be sorted
 * alphabetically!
 */
static struct StdFuncDesc {
    const char*	 	Name;
    void 	 	(*Handler) (struct expent*);
} StdFuncs [] = {
    {  	"strlen",	StdFunc_strlen	  	},

};
#define FUNC_COUNT	(sizeof (StdFuncs) / sizeof (StdFuncs [0]))


/*****************************************************************************/
/*			       Helper functions	     	    		     */
/*****************************************************************************/



static int CmpFunc (const void* Key, const void* Elem)
/* Compare function for bsearch */
{
    return strcmp ((const char*) Key, ((const struct StdFuncDesc*) Elem)->Name);
}



static struct StdFuncDesc* FindFunc (const char* Name)
/* Find a function with the given name. Return a pointer to the descriptor if
 * found, return NULL otherwise.
 */
{
    return bsearch (Name, StdFuncs, FUNC_COUNT, sizeof (StdFuncs [0]), CmpFunc);
}



/*****************************************************************************/
/*		 	    Handle known functions  			     */
/*****************************************************************************/



static void StdFunc_strlen (struct expent* lval)
/* Handle the strlen function */
{
    struct expent pval;
    static type ArgType[] = { T_PTR, T_SCHAR, T_END };


    /* Fetch the parameter */
    int k = hie1 (&pval);

    /* Check if the parameter is a const address */
    unsigned flags = 0;
    unsigned pflags = pval.e_flags & ~E_MCTYPE;
    if (pflags == E_MCONST) {
    	/* Constant numeric address */
    	flags |= CF_CONST | CF_ABSOLUTE;
    } else if (k == 0 && ((pflags & E_MGLOBAL) != 0 || pval.e_flags == E_MEOFFS)) {
    	/* Global array with or without offset */
    	flags |= CF_CONST;
    	if (pval.e_flags & E_TGLAB) {
    	    /* External linkage */
    	    flags |= CF_EXTERNAL;
    	} else {
    	    flags |= CF_STATIC;
    	}
    } else {
    	/* Not const, load parameter into primary */
     	exprhs (CF_NONE, k, &pval);
    }

    /* Setup the argument type string */
    ArgType[1] = GetDefaultChar () | T_QUAL_CONST;

    /* Convert the parameter type to the type needed, check for mismatches */
    assignadjust (ArgType, &pval);

    /* Generate the strlen code */
    g_strlen (flags, pval.e_name, pval.e_const);

    /* We expect the closing brace */
    ConsumeRParen ();
}



/*****************************************************************************/
/*  		    	 	     Code	       	 		     */
/*****************************************************************************/



int IsStdFunc (const char* Name)
/* Determine if the given function is a known standard function that may be
 * called in a special way.
 */
{
    /* Look into the table for known names */
    return FindFunc (Name) != 0;
}



void HandleStdFunc (struct expent* lval)
/* Generate code for a known standard function. */
{
    /* Get a pointer to the table entry */
    struct StdFuncDesc* F = FindFunc ((const char*) lval->e_name);
    CHECK (F != 0);

    /* Call the handler function */
    F->Handler (lval);
}



