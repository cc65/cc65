/*****************************************************************************/
/*                                                                           */
/*                                 incpath.c                                 */
/*                                                                           */
/*            Include path handling for the ca65 macro assembler             */
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



/* ca65 */
#include "incpath.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



SearchPaths*    IncSearchPath;          /* Standard include path */
SearchPaths*    BinSearchPath;          /* Binary include path */



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



void InitIncludePaths (void)
/* Initialize the include path search list */
{
    /* Create the search path lists */
    IncSearchPath = NewSearchPath ();
    BinSearchPath = NewSearchPath ();
}



void FinishIncludePaths (void)
/* Finish creating the include path search list. */
{
    /* Add specific paths from the environment */
    AddSearchPathFromEnv (IncSearchPath, "CA65_INC");

    /* Add paths relative to a main directory defined in an env. var. */
    AddSubSearchPathFromEnv (IncSearchPath, "CC65_HOME", "asminc");

    /* Add some compiled-in search paths if defined at compile time. */
#if defined(CA65_INC) && !defined(_WIN32) && !defined(_AMIGA)
    AddSearchPath (IncSearchPath, CA65_INC);
#endif

    /* Add paths relative to the parent directory of the Windows binary. */
    AddSubSearchPathFromBin (IncSearchPath, "asminc");
}
