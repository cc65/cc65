/*****************************************************************************/
/*                                                                           */
/*				   fileio.h				     */
/*                                                                           */
/*		File I/O for the od65 object file dump utility		     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2000 Ullrich von Bassewitz                                       */
/*               Wacholderweg 14                                             */
/*               D-70597 Stuttgart                                           */
/* EMail:        uz@musoftware.de                                            */
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



#ifndef FILEIO_H
#define FILEIO_H



#include <stdio.h>

/* common */
#include "filepos.h"
#include "objdefs.h"



/*****************************************************************************/
/*     	      	    		     Code				     */
/*****************************************************************************/



void FileSeek (FILE* F, unsigned long Pos);
/* Seek to the given absolute position, fail on errors */

unsigned Read8 (FILE* F);
/* Read an 8 bit value from the file */

unsigned Read16 (FILE* F);
/* Read a 16 bit value from the file */

unsigned long Read24 (FILE* F);
/* Read a 24 bit value from the file */

unsigned long Read32 (FILE* F);
/* Read a 32 bit value from the file */

long Read32Signed (FILE* F);
/* Read a 32 bit value from the file. Sign extend the value. */

unsigned long ReadVar (FILE* F);
/* Read a variable size value from the file */

char* ReadStr (FILE* F);
/* Read a string from the file into a malloced area */

FilePos* ReadFilePos (FILE* F, FilePos* Pos);
/* Read a file position from the file */

void* ReadData (FILE* F, void* Data, unsigned Size);
/* Read data from the file */

void ReadObjHeader (FILE* F, ObjHeader* Header);
/* Read an object file header from the file */



/* End of fileio.h */

#endif



