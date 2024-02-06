/*****************************************************************************/
/*                                                                           */
/*                                 fileio.h                                  */
/*                                                                           */
/*                      File I/O for the ar65 archiver                       */
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



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



void Write8 (FILE* F, unsigned char Val);
/* Write an 8 bit value to the file */

void Write16 (FILE* F, unsigned Val);
/* Write a 16 bit value to the file */

void Write32 (FILE* F, unsigned long Val);
/* Write a 32 bit value to the file */

void WriteVar (FILE* F, unsigned long V);
/* Write a variable sized value to the file in special encoding */

void WriteStr (FILE* F, const char* S);
/* Write a string to the file */

void WriteData (FILE* F, const void* Data, unsigned Size);
/* Write data to the file */

unsigned Read8 (FILE* F);
/* Read an 8 bit value from the file */

unsigned Read16 (FILE* F);
/* Read a 16 bit value from the file */

unsigned long Read32 (FILE* F);
/* Read a 32 bit value from the file */

unsigned long ReadVar (FILE* F);
/* Read a variable size value from the file */

char* ReadStr (FILE* F);
/* Read a string from the file (the memory will be malloc'ed) */

void* ReadData (FILE* F, void* Data, unsigned Size);
/* Read data from the file */



/* End of fileio.h */

#endif
