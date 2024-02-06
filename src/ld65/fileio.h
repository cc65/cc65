/*****************************************************************************/
/*                                                                           */
/*                                 fileio.h                                  */
/*                                                                           */
/*                       File I/O for the ld65 linker                        */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2003 Ullrich von Bassewitz                                       */
/*               Roemerstrasse 52                                            */
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



#ifndef FILEIO_H
#define FILEIO_H



#include <stdio.h>

/* common */
#include "filepos.h"



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



void FileSetPos (FILE* F, unsigned long Pos);
/* Seek to the given absolute position, fail on errors */

unsigned long FileGetPos (FILE* F);
/* Return the current file position, fail on errors */

void Write8 (FILE* F, unsigned Val);
/* Write an 8 bit value to the file */

void Write16 (FILE* F, unsigned Val);
/* Write a 16 bit value to the file */

void Write24 (FILE* F, unsigned long Val);
/* Write a 24 bit value to the file */

void Write32 (FILE* F, unsigned long Val);
/* Write a 32 bit value to the file */

void WriteVal (FILE* F, unsigned long Val, unsigned Size);
/* Write a value of the given size to the output file */

void WriteVar (FILE* F, unsigned long V);
/* Write a variable sized value to the file in special encoding */

void WriteStr (FILE* F, const char* S);
/* Write a string to the file */

void WriteData (FILE* F, const void* Data, unsigned Size);
/* Write data to the file */

void WriteMult (FILE* F, unsigned char Val, unsigned long Count);
/* Write one byte several times to the file */

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

unsigned ReadStr (FILE* F);
/* Read a string from the file, place it into the global string pool, and
** return its string id.
*/

FilePos* ReadFilePos (FILE* F, FilePos* Pos);
/* Read a file position from the file */

void* ReadData (FILE* F, void* Data, unsigned Size);
/* Read data from the file */



/* End of fileio.h */

#endif
