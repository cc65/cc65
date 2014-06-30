/*****************************************************************************/
/*                                                                           */
/*                                 fileio.c                                  */
/*                                                                           */
/*                       File I/O for the ld65 linker                        */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2008 Ullrich von Bassewitz                                       */
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



#include <string.h>
#include <errno.h>

/* common */
#include "xmalloc.h"

/* ld65 */
#include "error.h"
#include "fileio.h"
#include "spool.h"



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



void FileSetPos (FILE* F, unsigned long Pos)
/* Seek to the given absolute position, fail on errors */
{
    if (fseek (F, Pos, SEEK_SET) != 0) {
        Error ("Cannot seek: %s", strerror (errno));
    }
}



unsigned long FileGetPos (FILE* F)
/* Return the current file position, fail on errors */
{
    long Pos = ftell (F);
    if (Pos < 0) {
        Error ("Error in ftell: %s", strerror (errno));
    }
    return Pos;
}



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



void WriteVar (FILE* F, unsigned long V)
/* Write a variable sized value to the file in special encoding */
{
    /* We will write the value to the file in 7 bit chunks. If the 8th bit
    ** is clear, we're done, if it is set, another chunk follows. This will
    ** allow us to encode smaller values with less bytes, at the expense of
    ** needing 5 bytes if a 32 bit value is written to file.
    */
    do {
        unsigned char C = (V & 0x7F);
        V >>= 7;
        if (V) {
            C |= 0x80;
        }
        Write8 (F, C);
    } while (V != 0);
}



void WriteStr (FILE* F, const char* S)
/* Write a string to the file */
{
    unsigned Len = strlen (S);
    WriteVar (F, Len);
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
        long Pos = ftell (F);
        Error ("Read error at position %ld (file corrupt?)", Pos);
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



unsigned long ReadVar (FILE* F)
/* Read a variable size value from the file */
{
    /* The value was written to the file in 7 bit chunks LSB first. If there
    ** are more bytes, bit 8 is set, otherwise it is clear.
    */
    unsigned char C;
    unsigned long V = 0;
    unsigned Shift = 0;
    do {
        /* Read one byte */
        C = Read8 (F);
        /* Encode it into the target value */
        V |= ((unsigned long)(C & 0x7F)) << Shift;
        /* Next value */
        Shift += 7;
    } while (C & 0x80);

    /* Return the value read */
    return V;
}



unsigned ReadStr (FILE* F)
/* Read a string from the file, place it into the global string pool, and
** return its string id.
*/
{
    unsigned    Id;
    StrBuf      Buf = STATIC_STRBUF_INITIALIZER;

    /* Read the length */
    unsigned Len = ReadVar (F);

    /* Expand the string buffer memory */
    SB_Realloc (&Buf, Len);

    /* Read the string */
    ReadData (F, SB_GetBuf (&Buf), Len);
    Buf.Len = Len;

    /* Insert it into the string pool and remember the id */
    Id = GetStrBufId (&Buf);

    /* Free the memory buffer */
    SB_Done (&Buf);

    /* Return the string id */
    return Id;
}



FilePos* ReadFilePos (FILE* F, FilePos* Pos)
/* Read a file position from the file */
{
    /* Read the data fields */
    Pos->Line = ReadVar (F);
    Pos->Col  = ReadVar (F);
    Pos->Name = ReadVar (F);
    return Pos;
}



void* ReadData (FILE* F, void* Data, unsigned Size)
/* Read data from the file */
{
    /* Explicitly allow reading zero bytes */
    if (Size > 0) {
        if (fread (Data, 1, Size, F) != Size) {
            long Pos = ftell (F);
            Error ("Read error at position %ld (file corrupt?)", Pos);
        }
    }
    return Data;
}
