/*****************************************************************************/
/*                                                                           */
/*				   fileio.c				     */
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



#include <string.h>
#include <errno.h>

/* common */
#include "xmalloc.h"

/* od65 */
#include "error.h"
#include "fileio.h"



/*****************************************************************************/
/*     	      	    		     Code				     */
/*****************************************************************************/



void FileSeek (FILE* F, unsigned long Pos)
/* Seek to the given absolute position, fail on errors */
{
    if (fseek (F, Pos, SEEK_SET) != 0) {
 	Error ("Cannot seek: %s", strerror (errno));
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
    char* Str = xmalloc (Len + 1);

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



void ReadObjHeader (FILE* F, ObjHeader* H)
/* Read an object file header from the file */
{
    /* Read all fields */
    H->Magic	    = Read32 (F);
    H->Version	    = Read16 (F);
    H->Flags	    = Read16 (F);
    H->OptionOffs   = Read32 (F);
    H->OptionSize   = Read32 (F);
    H->FileOffs	    = Read32 (F);
    H->FileSize	    = Read32 (F);
    H->SegOffs	    = Read32 (F);
    H->SegSize	    = Read32 (F);
    H->ImportOffs   = Read32 (F);
    H->ImportSize   = Read32 (F);
    H->ExportOffs   = Read32 (F);
    H->ExportSize   = Read32 (F);
    H->DbgSymOffs   = Read32 (F);
    H->DbgSymSize   = Read32 (F);
}



