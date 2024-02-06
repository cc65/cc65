/*****************************************************************************/
/*                                                                           */
/*                                 fileio.c                                  */
/*                                                                           */
/*              File I/O for the sp65 sprite and bitmap utility              */
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



#include <string.h>
#include <errno.h>

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
