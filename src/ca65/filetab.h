/*****************************************************************************/
/*                                                                           */
/*				   filetab.h				     */
/*                                                                           */
/*			   Input file table for ca65			     */
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



#ifndef FILETAB_H
#define	FILETAB_H



/*****************************************************************************/
/*     	       	    		     Code			   	     */
/*****************************************************************************/



const char* GetFileName (unsigned Name);
/* Get the name of a file where the name index is known */

unsigned GetFileIndex (const char* Name);
/* Return the file index for the given file name. */

unsigned AddFile (const char* Name, unsigned long Size, unsigned long MTime);
/* Add a new file to the list of input files. Return the index of the file in
 * the table.
 */

void WriteFiles (void);
/* Write the list of input files to the object file */




/* End of filetab.h */

#endif





