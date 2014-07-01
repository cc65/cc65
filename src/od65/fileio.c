/*****************************************************************************/
/*                                                                           */
/*                                 fileio.c                                  */
/*                                                                           */
/*              File I/O for the od65 object file dump utility               */
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



#include <string.h>
#include <errno.h>

/* common */
#include "xmalloc.h"

/* od65 */
#include "error.h"
#include "fileio.h"



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



char* ReadStr (FILE* F)
/* Read a string from the file into a malloced area */
{
    /* Read the length */
    unsigned Len = ReadVar (F);

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
    /* Read the data fields */
    Pos->Line = ReadVar (F);
    Pos->Col  = ReadVar (F);
    Pos->Name = ReadVar (F);
    return Pos;
}



void* ReadData (FILE* F, void* Data, unsigned Size)
/* Read data from the file */
{
    /* Accept zero sized reads */
    if (Size > 0) {
        if (fread (Data, 1, Size, F) != Size) {
            Error ("Read error (file corrupt?)");
        }
    }
    return Data;
}



void ReadObjHeader (FILE* F, ObjHeader* H)
/* Read an object file header from the file */
{
    /* Read all fields */
    H->Magic        = Read32 (F);
    H->Version      = Read16 (F);
    H->Flags        = Read16 (F);
    H->OptionOffs   = Read32 (F);
    H->OptionSize   = Read32 (F);
    H->FileOffs     = Read32 (F);
    H->FileSize     = Read32 (F);
    H->SegOffs      = Read32 (F);
    H->SegSize      = Read32 (F);
    H->ImportOffs   = Read32 (F);
    H->ImportSize   = Read32 (F);
    H->ExportOffs   = Read32 (F);
    H->ExportSize   = Read32 (F);
    H->DbgSymOffs   = Read32 (F);
    H->DbgSymSize   = Read32 (F);
    H->LineInfoOffs = Read32 (F);
    H->LineInfoSize = Read32 (F);
    H->StrPoolOffs  = Read32 (F);
    H->StrPoolSize  = Read32 (F);
    H->AssertOffs   = Read32 (F);
    H->AssertSize   = Read32 (F);
    H->ScopeOffs    = Read32 (F);
    H->ScopeSize    = Read32 (F);
    H->SpanOffs     = Read32 (F);
    H->SpanSize     = Read32 (F);
}



void ReadStrPool (FILE* F, Collection* C)
/* Read a string pool from the current position into C. */
{
    /* The number of strings is the first item */
    unsigned long Count = ReadVar (F);

    /* Read all the strings into C */
    while (Count--) {
        CollAppend (C, ReadStr (F));
    }
}
