/*****************************************************************************/
/*                                                                           */
/*				   fileio.c				     */
/*                                                                           */
/*			 File I/O for the ld65 linker			     */
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



#include <string.h>

#include "error.h"
#include "mem.h"
#include "fileio.h"



/*****************************************************************************/
/*     	      	    		     Code				     */
/*****************************************************************************/



void Write8 (FILE* F, unsigned Val)
/* Write an 8 bit value to the file */
{
    if (putc (Val, F) == EOF) {
    	Error ("Write error (disk full?)");
    }
}



void Write16 (FILE* F, unsigned Val)
/* Write a 16 bit value to the file */
{
    Write8 (F, (unsigned char) Val);
    Write8 (F, (unsigned char) (Val >> 8));
}



void Write24 (FILE* F, unsigned long Val)
/* Write a 24 bit value to the file */
{
    Write8 (F, (unsigned char) Val);
    Write8 (F, (unsigned char) (Val >> 8));
    Write8 (F, (unsigned char) (Val >> 16));
}



void Write32 (FILE* F, unsigned long Val)
/* Write a 32 bit value to the file */
{
    Write8 (F, (unsigned char) Val);
    Write8 (F, (unsigned char) (Val >> 8));
    Write8 (F, (unsigned char) (Val >> 16));
    Write8 (F, (unsigned char) (Val >> 24));
}



void WriteVal (FILE* F, unsigned long Val, unsigned Size)
/* Write a value of the given size to the output file */
{
    switch (Size) {

	case 1:
	    Write8 (F, Val);
	    break;

	case 2:
	    Write16 (F, Val);
	    break;

	case 3:
	    Write24 (F, Val);
	    break;

	case 4:
	    Write32 (F, Val);
	    break;

	default:
       	    Internal ("WriteVal: Invalid size: %u", Size);

    }
}



void WriteStr (FILE* F, const char* S)
/* Write a string to the file */
{
    unsigned Len = strlen (S);
    if (Len > 255) {
       	Internal ("String too long");
    }
    Write8 (F, (unsigned char) Len);
    WriteData (F, S, Len);
}



void WriteData (FILE* F, const void* Data, unsigned Size)
/* Write data to the file */
{
    if (fwrite (Data, 1, Size, F) != Size) {
 	Error ("Write error (disk full?)");
    }
}



void WriteMult (FILE* F, unsigned char Val, unsigned long Count)
/* Write one byte several times to the file */
{
    while (Count--) {
	Write8 (F, Val);
    }
}



unsigned Read8 (FILE* F)
/* Read an 8 bit value from the file */
{
    int C = getc (F);
    if (C == EOF) {
 	Error ("Read error (file corrupt?)");
    }
    return C;
}



unsigned Read16 (FILE* F)
/* Read a 16 bit value from the file */
{
    unsigned Lo = Read8 (F);
    unsigned Hi = Read8 (F);
    return (Hi << 8) | Lo;
}



unsigned long Read24 (FILE* F)
/* Read a 24 bit value from the file */
{
    unsigned long Lo = Read16 (F);
    unsigned long Hi = Read8 (F);
    return (Hi << 16) | Lo;
}



unsigned long Read32 (FILE* F)
/* Read a 32 bit value from the file */
{
    unsigned long Lo = Read16 (F);
    unsigned long Hi = Read16 (F);
    return (Hi << 16) | Lo;
}



long Read32Signed (FILE* F)
/* Read a 32 bit value from the file. Sign extend the value. */
{
    /* Read a 32 bit value */
    unsigned long V = Read32 (F);

    /* Sign extend the value */
    if (V & 0x80000000UL) {
	/* Signed value */
	V |= ~0xFFFFFFFFUL;
    }

    /* Return it as a long */
    return (long) V;
}



char* ReadStr (FILE* F, char* Str)
/* Read a string from the file. Str must hold 256 chars at max */
{
    /* Read the length byte */
    unsigned Len = Read8 (F);

    /* Read the string itself */
    ReadData (F, Str, Len);

    /* Terminate the string and return it */
    Str [Len] = '\0';
    return Str;
}



char* ReadMallocedStr (FILE* F)
/* Read a string from the file into a malloced area */
{
    /* Read the length byte */
    unsigned Len = Read8 (F);

    /* Allocate memory */
    char* Str = Xmalloc (Len + 1);

    /* Read the string itself */
    ReadData (F, Str, Len);

    /* Terminate the string and return it */
    Str [Len] = '\0';
    return Str;
}



FilePos* ReadFilePos (FILE* F, FilePos* Pos)
/* Read a file position from the file */
{
    /* The line number is encoded as 24 bit value to save some space */
    Pos->Line =	Read24 (F);
    Pos->Col  = Read8 (F);
    Pos->Name = Read8 (F);
    return Pos;
}



void* ReadData (FILE* F, void* Data, unsigned Size)
/* Read data from the file */
{
    if (fread (Data, 1, Size, F) != Size) {
	Error ("Read error (file corrupt?)");
    }
    return Data;
}



