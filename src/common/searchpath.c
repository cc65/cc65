/*****************************************************************************/
/*                                                                           */
/*                               searchpath.h                                */
/*                                                                           */
/*                         Handling of search paths                          */
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



#include <stdlib.h>
#include <string.h>
#if defined(_MSC_VER)
/* Microsoft compiler */
#  include <io.h>
#else
/* Anyone else */
#  include <unistd.h>
#endif

/* common */
#include "coll.h"
#include "searchpath.h"
#include "strbuf.h"
#include "xmalloc.h"



/*****************************************************************************/
/*	      	     	      	     Code    		     		     */
/*****************************************************************************/



static char* CleanupPath (const char* Path)
/* Prepare and return a clean copy of Path */
{
    unsigned Len;
    char*    NewPath;

    /* Get the length of the path */
    Len = strlen (Path);

    /* Check for a trailing path separator and remove it */
    if (Len > 0 && (Path[Len-1] == '\\' || Path[Len-1] == '/')) {
    	--Len;
    }

    /* Allocate memory for the new string */
    NewPath = (char*) xmalloc (Len + 1);

    /* Copy the path and terminate it, then return the copy */
    memcpy (NewPath, Path, Len);
    NewPath [Len] = '\0';
    return NewPath;
}



static void Add (SearchPath* P, const char* New)
/* Cleanup a new search path and add it to the list */
{
    /* Add a clean copy of the path to the collection */
    CollAppend (P, CleanupPath (New));
}



SearchPath* NewSearchPath (void)
/* Create a new, empty search path list */
{
    return NewCollection ();
}



void AddSearchPath (SearchPath* P, const char* NewPath)
/* Add a new search path to the end of an existing list */
{
    /* Allow a NULL path */
    if (NewPath) {
        Add (P, NewPath);
    }
}



void AddSearchPathFromEnv (SearchPath* P, const char* EnvVar)
/* Add a search path from an environment variable to the end of an existing
 * list.
 */
{
    AddSearchPath (P, getenv (EnvVar));
}



void AddSubSearchPathFromEnv (SearchPath* P, const char* EnvVar, const char* SubDir)
/* Add a search path from an environment variable, adding a subdirectory to
 * the environment variable value.
 */
{
    StrBuf Dir = AUTO_STRBUF_INITIALIZER;

    const char* EnvVal = getenv (EnvVar);
    if (EnvVal == 0) {
    	/* Not found */
    	return;
    }

    /* Copy the environment variable to the buffer */
    SB_CopyStr (&Dir, EnvVal);

    /* Add a path separator if necessary */
    if (SB_NotEmpty (&Dir)) {
	if (SB_LookAtLast (&Dir) != '\\' && SB_LookAtLast (&Dir) != '/') {
	    SB_AppendChar (&Dir, '/');
	}
    }

    /* Add the subdirectory and terminate the string */
    SB_AppendStr (&Dir, SubDir);
    SB_Terminate (&Dir);

    /* Add the search path */
    AddSearchPath (P, SB_GetConstBuf (&Dir));

    /* Free the temp buffer */
    SB_Done (&Dir);
}



void PushSearchPath (SearchPath* P, const char* NewPath)
/* Add a new search path to the head of an existing search path list */
{
    /* Insert a clean copy of the path at position 0 */
    CollInsert (P, CleanupPath (NewPath), 0);
}



void PopSearchPath (SearchPath* P)
/* Remove a search path from the head of an existing search path list */
{
    /* Remove the path at position 0 */
    xfree (CollAt (P, 0));
    CollDelete (P, 0);
}



void ForgetSearchPath (SearchPath* P)
/* Forget all search paths in the given list */
{
    unsigned I;
    for (I = 0; I < CollCount (P); ++I) {
        xfree (CollAt (P, I));
    }
    CollDeleteAll (P);
}



char* SearchFile (const SearchPath* P, const char* File)
/* Search for a file in a list of directories. Return a pointer to a malloced
 * area that contains the complete path, if found, return 0 otherwise.
 */
{
    char* Name = 0;
    StrBuf PathName = AUTO_STRBUF_INITIALIZER;

    /* Start the search */
    unsigned I;
    for (I = 0; I < CollCount (P); ++I) {

        /* Copy the next path element into the buffer */
        SB_CopyStr (&PathName, CollConstAt (P, I));

	/* Add a path separator and the filename */
       	if (SB_NotEmpty (&PathName)) {
     	    SB_AppendChar (&PathName, '/');
	}
	SB_AppendStr (&PathName, File);
    	SB_Terminate (&PathName);

	/* Check if this file exists */
       	if (access (SB_GetBuf (&PathName), 0) == 0) {
	    /* The file exists, we're done */
	    Name = xstrdup (SB_GetBuf (&PathName));
            break;
	}
    }

    /* Cleanup and return the result of the search */
    SB_Done (&PathName);
    return Name;
}



