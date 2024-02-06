/*****************************************************************************/
/*                                                                           */
/*                                 objfile.h                                 */
/*                                                                           */
/*                Object file handling for the ar65 archiver                 */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2011, Ullrich von Bassewitz                                      */
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



#ifndef OBJFILE_H
#define OBJFILE_H



#include <stdio.h>



/*****************************************************************************/
/*                                 Forwards                                  */
/*****************************************************************************/



struct ObjData;



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



void ObjReadData (FILE* F, struct ObjData* O);
/* Read object file data from the given file. The function expects the Name
** and Start fields to be valid. Header and basic data are read.
*/

void ObjAdd (const char* Name);
/* Add an object file to the library */

void ObjExtract (const char* Name);
/* Extract a module from the library */



/* End of objfile.h */

#endif
