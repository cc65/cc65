/*****************************************************************************/
/*                                                                           */
/*				   incpath.c				     */
/*                                                                           */
/*			Include path handling for cc65			     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000-2013, Ullrich von Bassewitz                                      */
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



/* cc65 */
#include "incpath.h"



/*****************************************************************************/
/*     	       	     		     Data		     		     */
/*****************************************************************************/



SearchPath*     SysIncSearchPath;       /* System include path */
SearchPath*     UsrIncSearchPath;       /* User include path */



/*****************************************************************************/
/*    	      	     	      	     Code		     		     */
/*****************************************************************************/



void ForgetAllIncludePaths (void)
/* Remove all include search paths. */
{
    ForgetSearchPath (SysIncSearchPath);
    ForgetSearchPath (UsrIncSearchPath);
}



void InitIncludePaths (void)
/* Initialize the include path search list */
{
    /* Create the search path lists */
    SysIncSearchPath = NewSearchPath ();
    UsrIncSearchPath = NewSearchPath ();
}



void FinishIncludePaths (void)
/* Finish creating the include path search lists. */
{
    /* Add specific paths from the environment */
    AddSearchPathFromEnv (SysIncSearchPath, "CC65_INC");
    AddSearchPathFromEnv (UsrIncSearchPath, "CC65_INC");

    /* Add paths relative to a main directory defined in an env. var. */
    AddSubSearchPathFromEnv (SysIncSearchPath, "CC65_HOME", "include");

    /* Add some compiled-in search paths if defined at compile time. */
#ifdef CC65_INC
    AddSearchPath (SysIncSearchPath, STRINGIZE (CC65_INC));
#endif
}



