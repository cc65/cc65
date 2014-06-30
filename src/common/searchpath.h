/*****************************************************************************/
/*                                                                           */
/*                               searchpath.h                                */
/*                                                                           */
/*                         Handling of search paths                          */
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



/* Exports facilities to search files in a list of directories. */



#ifndef SEARCHPATH_H
#define SEARCHPATH_H



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Convert argument to C string */
#define _STRINGIZE(arg) #arg
#define  STRINGIZE(arg) _STRINGIZE(arg)

/* A search path is a pointer to the list */
typedef struct Collection SearchPaths;



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



SearchPaths* NewSearchPath (void);
/* Create a new, empty search path list */

void AddSearchPath (SearchPaths* P, const char* NewPath);
/* Add a new search path to the end of an existing list */

void AddSearchPathFromEnv (SearchPaths* P, const char* EnvVar);
/* Add a search path from an environment variable to the end of an existing
** list.
*/

void AddSubSearchPathFromEnv (SearchPaths* P, const char* EnvVar, const char* SubDir);
/* Add a search path from an environment variable, adding a subdirectory to
** the environment variable value.
*/

void AddSubSearchPathFromWinBin (SearchPaths* P, const char* SubDir);
/* Windows only:
** Add a search path from the running binary, adding a subdirectory to
** the parent directory of the directory containing the binary.
*/

int PushSearchPath (SearchPaths* P, const char* NewPath);
/* Add a new search path to the head of an existing search path list, provided
** that it's not already there. If the path is already at the first position,
** return zero, otherwise return a non zero value.
*/

void PopSearchPath (SearchPaths* P);
/* Remove a search path from the head of an existing search path list */

char* SearchFile (const SearchPaths* P, const char* File);
/* Search for a file in a list of directories. Return a pointer to a malloced
** area that contains the complete path, if found, return 0 otherwise.
*/



/* End of searchpath.h */

#endif
