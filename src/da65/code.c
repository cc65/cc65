/*****************************************************************************/
/*                                                                           */
/*                                  code.c                                   */
/*                                                                           */
/*                          Binary code management                           */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000-2003 Ullrich von Bassewitz                                       */
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



#include <stdio.h>
#include <string.h>
#include <errno.h>

/* common */
#include "check.h"

/* da65 */
#include "code.h"
#include "error.h"
#include "global.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



unsigned char CodeBuf [0x10000];        /* Code buffer */
unsigned long CodeStart;                /* Start address */
unsigned long CodeEnd;                  /* End address */
unsigned long PC;                       /* Current PC */



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



void LoadCode (void)
/* Load the code from the given file */
{
    long Count, MaxCount, Size;
    FILE* F;


    PRECONDITION (StartAddr < 0x10000);

    /* Open the file */
    F = fopen (InFile, "rb");
    if (F == 0) {
        Error ("Cannot open `%s': %s", InFile, strerror (errno));
    }

    /* Seek to the end to get the size of the file */
    if (fseek (F, 0, SEEK_END) != 0) {
        Error ("Cannot seek on file `%s': %s", InFile, strerror (errno));
    }
    Size = ftell (F);

    /* The input offset must be smaller than the size */
    if (InputOffs >= 0) {
        if (InputOffs >= Size) {
            Error ("Input offset is greater than file size");
        }
    } else {
        /* Use a zero offset */
        InputOffs = 0;
    }

    /* Seek to the input offset and correct size to contain the remainder of
    ** the file.
    */
    if (fseek (F, InputOffs, SEEK_SET) != 0) {
        Error ("Cannot seek on file `%s': %s", InFile, strerror (errno));
    }
    Size -= InputOffs;

    /* Limit the size to the maximum input size if one is given */
    if (InputSize >= 0) {
        if (InputSize > Size) {
            Error ("Input size is greater than what is available");
        }
        Size = InputSize;
    }

    /* If the start address was not given, set it so that the code loads to
    ** 0x10000 - Size. This is a reasonable default assuming that the file
    ** is a ROM that contains the hardware vectors at $FFFA.
    */
    if (StartAddr < 0) {
        if (Size > 0x10000) {
            StartAddr = 0;
        } else {
            StartAddr = 0x10000 - Size;
        }
    }

    /* Calculate the maximum code size */
    MaxCount = 0x10000 - StartAddr;

    /* Check if the size is larger than what we can read */
    if (Size == 0) {
        Error ("Nothing to read from input file `%s'", InFile);
    }
    if (Size > MaxCount) {
        Warning ("File `%s' is too large, ignoring %ld bytes",
                 InFile, Size - MaxCount);
    } else if (MaxCount > Size) {
        MaxCount = (unsigned) Size;
    }

    /* Read from the file and remember the number of bytes read */
    Count = fread (CodeBuf + StartAddr, 1, MaxCount, F);
    if (ferror (F) || Count != MaxCount) {
        Error ("Error reading from `%s': %s", InFile, strerror (errno));
    }

    /* Close the file */
    fclose (F);

    /* Set the buffer variables */
    CodeStart = PC = StartAddr;
    CodeEnd = CodeStart + Count - 1;    /* CodeEnd is inclusive */
}



unsigned char GetCodeByte (unsigned Addr)
/* Get a byte from the given address */
{
    PRECONDITION (Addr <= CodeEnd);
    return CodeBuf [Addr];
}



unsigned GetCodeDByte (unsigned Addr)
/* Get a dbyte from the given address */
{
    unsigned Lo = GetCodeByte (Addr);
    unsigned Hi = GetCodeByte (Addr+1);
    return (Lo <<8) | Hi;
}



unsigned GetCodeWord (unsigned Addr)
/* Get a word from the given address */
{
    unsigned Lo = GetCodeByte (Addr);
    unsigned Hi = GetCodeByte (Addr+1);
    return Lo | (Hi << 8);
}



unsigned long GetCodeDWord (unsigned Addr)
/* Get a dword from the given address */
{
    unsigned long Lo = GetCodeWord (Addr);
    unsigned long Hi = GetCodeWord (Addr+2);
    return Lo | (Hi << 16);
}



unsigned GetRemainingBytes (void)
/* Return the number of remaining code bytes */
{
    if (CodeEnd >= PC) {
        return (CodeEnd - PC + 1);
    } else {
        return 0;
    }
}



int CodeLeft (void)
/* Return true if there are code bytes left */
{
    return (PC <= CodeEnd);
}



void ResetCode (void)
/* Reset the code input to start over for the next pass */
{
    PC = CodeStart;
}
