/*****************************************************************************/
/*                                                                           */
/*				   incpath.h				     */
/*                                                                           */
/*	      Include path handling for the ca65 macro assembler	     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000-2009, Ullrich von Bassewitz                                      */
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



#ifndef INCPATH_H
#define INCPATH_H



/*****************************************************************************/
/*     	     	    		     Code   				     */
/*****************************************************************************/



void AddIncludePath (const char* NewPath);
/* Add a new include path to the existing one */

char* FindInclude (const char* Name);
/* Find an include file. Return a pointer to a malloced area that contains
 * the complete path, if found, return 0 otherwise.
 */

void InitIncludePaths (void);
/* Initialize the include path search list */



/* End of incpath.h */

#endif



