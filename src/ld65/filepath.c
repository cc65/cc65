/*****************************************************************************/
/*                                                                           */
/*				   filepath.c				     */
/*                                                                           */
/*                    File search path handling for ld65                     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2003-2013, Ullrich von Bassewitz                                      */
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



/* ld65 */
#include "filepath.h"



/*****************************************************************************/
/*	       	     		     Data		     		     */
/*****************************************************************************/



SearchPath*      LibSearchPath;         /* Library path */
SearchPath*      ObjSearchPath;         /* Object file path */
SearchPath*      CfgSearchPath;         /* Config file path */

SearchPath*      LibDefaultPath;        /* Default Library path */
SearchPath*      ObjDefaultPath;        /* Default Object file path */
SearchPath*      CfgDefaultPath;        /* Default Config file path */



/*****************************************************************************/
/*	       	     	     	     Code      		     		     */
/*****************************************************************************/



void InitSearchPaths (void)
/* Initialize the path search list */
{
    /* Create the search path lists */
    LibSearchPath = NewSearchPath ();
    ObjSearchPath = NewSearchPath ();
    CfgSearchPath = NewSearchPath ();

    LibDefaultPath = NewSearchPath ();
    ObjDefaultPath = NewSearchPath ();
    CfgDefaultPath = NewSearchPath ();

    /* Always search all stuff in the current directory */
    AddSearchPath (LibSearchPath, "");
    AddSearchPath (ObjSearchPath, "");
    AddSearchPath (CfgSearchPath, "");

    /* Add specific paths from the environment. */
    AddSearchPathFromEnv (LibDefaultPath, "LD65_LIB");
    AddSearchPathFromEnv (ObjDefaultPath, "LD65_OBJ");
    AddSearchPathFromEnv (CfgDefaultPath, "LD65_CFG");

    /* Add paths relative to a main directory defined in an env. var. */
    AddSubSearchPathFromEnv (LibDefaultPath, "CC65_HOME", "lib");
    AddSubSearchPathFromEnv (ObjDefaultPath, "CC65_HOME", "obj");
    AddSubSearchPathFromEnv (CfgDefaultPath, "CC65_HOME", "cfg");

    /* Add some compiled-in search paths if defined at compile time. */
#if defined(LD65_LIB)
    AddSearchPath (LibDefaultPath, STRINGIZE (LD65_LIB));
#endif
#if defined(LD65_OBJ)
    AddSearchPath (ObjDefaultPath, STRINGIZE (LD65_OBJ));
#endif
#if defined(LD65_CFG)
    AddSearchPath (CfgDefaultPath, STRINGIZE (LD65_CFG));
#endif
}



