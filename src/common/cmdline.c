/*****************************************************************************/
/*                                                                           */
/*				   cmdline.c				     */
/*                                                                           */
/*		   Helper functions for command line parsing		     */
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



#include <stdio.h>
#include <stdlib.h>

#include "cmdline.h"



/*****************************************************************************/
/*     	       	       	       	     Data    				     */
/*****************************************************************************/



static char** ArgVec     = 0;
static unsigned ArgCount = 0;



/*****************************************************************************/
/*     	       	       	       	     Code    				     */
/*****************************************************************************/



void InitCmdLine (unsigned aArgCount, char* aArgVec[])
/* Initialize command line parsing. aArgVec is the argument array terminated by
 * a NULL pointer (as usual), ArgCount is the number of valid arguments in the
 * array. Both arguments are remembered in static storage.
 */
{
    ArgCount = aArgCount;
    ArgVec   = aArgVec;
}



void UnknownOption (const char* Opt)
/* Print an error about an unknown option. */
{
    fprintf (stderr, "Unknown option: %s\n", Opt);
    exit (EXIT_FAILURE);
}



void NeedArg (const char* Opt)
/* Print an error about a missing option argument and exit. */
{
    fprintf (stderr, "Option requires an argument: %s\n", Opt);
    exit (EXIT_FAILURE);
}



void InvSym (const char* Def)
/* Print an error about an invalid symbol definition and die */
{
    fprintf (stderr, "Invalid symbol definition: `%s'\n", Def);
    exit (EXIT_FAILURE);
}



const char* GetArg (int* ArgNum, unsigned Len)
/* Get an argument for a short option. The argument may be appended to the
 * option itself or may be separate. Len is the length of the option string.
 */
{
    const char* Arg = ArgVec[*ArgNum];
    if (Arg[Len] != '\0') {
	/* Argument appended */
	return Arg + Len;
    } else {
	/* Separate argument */
	Arg = ArgVec[*ArgNum + 1];
	if (Arg == 0) {
	    /* End of arguments */
	    NeedArg (ArgVec[*ArgNum]);
	}
	++(*ArgNum);
	return Arg;
    }
}



void LongOption (int* ArgNum, const LongOpt* OptTab, unsigned OptCount)
/* Handle a long command line option */
{
    /* Get the option and the argument (which may be zero) */
    const char* Opt = ArgVec[*ArgNum];

    /* Search the table for a match */
    while (OptCount) {
	if (strcmp (Opt, OptTab->Option) == 0) {
	    /* Found, call the function */
	    if (OptTab->ArgCount > 0) {
		OptTab->Func (Opt, ArgVec[++(*ArgNum)]);
	    } else {
		OptTab->Func (Opt, 0);
	    }
	    /* Done */
	    return;
	}

	/* Next table entry */
	--OptCount;
	++OptTab;
    }

    /* Invalid option */
    UnknownOption (Opt);
}



