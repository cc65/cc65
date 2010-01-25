/*****************************************************************************/
/*                                                                           */
/*				   incpath.c				     */
/*                                                                           */
/*	      Include path handling for the ca65 macro assembler	     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000-2010, Ullrich von Bassewitz                                      */
/*                Roemerstrasse 52                                           */
/*                D-70794 Filderstadt                                        */
/* EMail:         uz@cc65.org                                                */
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



/* common */
#include "searchpath.h"

/* ca65 */
#include "incpath.h"



/*****************************************************************************/
/*     	     	    		     Code   				     */
/*****************************************************************************/



void AddIncludePath (const char* NewPath, unsigned Where)
/* Add a new include path to the existing one */
{
    AddSearchPath (NewPath, Where);
}



char* FindInclude (const char* Name, unsigned Where)
/* Find an include file. Return a pointer to a malloced area that contains
 * the complete path, if found, return 0 otherwise.
 */
{
    /* Search in the include directories */
    return SearchFile (Name, Where);
}



void ForgetAllIncludePaths (void)
/* Remove all include search paths. */
{
    ForgetAllSearchPaths (INC_STD | INC_BIN);
}



void InitIncludePaths (void)
/* Initialize the include path search list */
{
    /* Add some standard paths to the include search path */
    AddSearchPath ("", INC_STD);  		/* Current directory */
    AddSearchPath ("", INC_BIN);

    /* Add some compiled in search paths if defined at compile time */
#ifdef CA65_INC
    AddSearchPath (CA65_INC, INC_STD);
#endif

    /* Add specific paths from the environment */
    AddSearchPathFromEnv ("CA65_INC", INC_STD);

    /* Add paths relative to a main directory defined in an env var */
    AddSubSearchPathFromEnv ("CC65_HOME", "asminc", INC_STD);
}



