/*****************************************************************************/
/*                                                                           */
/*                               searchpath.h                                */
/*                                                                           */
/*                    Search path path handling for ld65                     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000-2003 Ullrich von Bassewitz                                       */
/*               Römerstrasse 52                                             */
/*               D-70794 Filderstadt                                         */
/* EMail:        uz@cc65.org                                                 */
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



/* Exports facilities to search files in a list of directories. 8 of these
 * lists are managed, and each list can contain an arbitrary number of
 * directories. The "Where" argument is actually a bitset, specifying which
 * of the search lists should be used when adding paths or searching files.
 */



#ifndef SEARCHPATH_H
#define SEARCHPATH_H



/*****************************************************************************/
/*	      	     	      	     Data		     		     */
/*****************************************************************************/



/* Maximum number of search paths */
#define MAX_SEARCH_PATHS        8



/*****************************************************************************/
/*	       	     	     	     Code		     		     */
/*****************************************************************************/



void AddSearchPath (const char* NewPath, unsigned Where);
/* Add a new search path to the existing one */

void AddSearchPathFromEnv (const char* EnvVar, unsigned Where);
/* Add a search from an environment variable */

char* SearchFile (const char* Name, unsigned Where);
/* Search for a file in a list of directories. Return a pointer to a malloced
 * area that contains the complete path, if found, return 0 otherwise.
 */



/* End of searchpath.h */
#endif



