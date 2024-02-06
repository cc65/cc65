/*****************************************************************************/
/*                                                                           */
/*                                 library.h                                 */
/*                                                                           */
/*         Library data structures and helpers for the ar65 archiver         */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2012, Ullrich von Bassewitz                                      */
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



#ifndef LIBRARY_H
#define LIBRARY_H



#include <stdio.h>



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Name of the library file */
extern const char* LibName;



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



void LibOpen (const char* Name, int MustExist, int NeedTemp);
/* Open an existing library and a temporary copy. If MustExist is true, the
** old library is expected to exist. If NeedTemp is true, a temporary library
** is created.
*/

unsigned long LibCopyTo (FILE* F, unsigned long Bytes);
/* Copy data from F to the temp library file, return the start position in
** the temporary library file.
*/

void LibCopyFrom (unsigned long Pos, unsigned long Bytes, FILE* F);
/* Copy data from the library file into another file */

void LibClose (void);
/* Write remaining data, close both files and copy the temp file to the old
** filename
*/



/* End of library.h */

#endif
