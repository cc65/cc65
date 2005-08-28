/*****************************************************************************/
/*                                                                           */
/*				   macpack.h				     */
/*                                                                           */
/*	     Predefined macro packages for the ca65 macroassembler	     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2005, Ullrich von Bassewitz                                      */
/*                Römerstrasse 52                                            */
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



#ifndef MACPACK_H
#define MACPACK_H



/*****************************************************************************/
/*     	       	    	       	     Data				     */
/*****************************************************************************/



/* Constants for the predefined packages */
enum {
    MAC_ATARI,
    MAC_CBM,
    MAC_CPU,
    MAC_GENERIC,
    MAC_LONGBRANCH,

    /* Number of known packages */
    MAC_COUNT
};



/*****************************************************************************/
/*     	       	       	       	     Code    				     */
/*****************************************************************************/



int MacPackFind (const char* Name);
/* Find a macro package by name. The function will either return the id or
 * -1 if the package name was not found.
 */

void MacPackInsert (int Id);
/* Insert the macro package with the given id in the input stream */

void MacPackSetDir (const char* Dir);
/* Set a directory where files for macro packages can be found. Standard is
 * to use the builtin packages. For debugging macro packages, external files
 * can be used.
 */



/* End of macpack.h */

#endif



