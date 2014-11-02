/*****************************************************************************/
/*                                                                           */
/*                                   asm.c                                   */
/*                                                                           */
/*          Assembler output for the sp65 sprite and bitmap utility          */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2012,      Ullrich von Bassewitz                                      */
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



#include <errno.h>
#include <stdio.h>
#include <string.h>

/* common */
#include "chartype.h"
#include "check.h"
#include "cmdline.h"
#include "version.h"

/* sp65 */
#include "attr.h"
#include "asm.h"
#include "error.h"



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



static int ValidIdentifier (const char* L)
/* Check an assembler label for validity */
{
    /* Must begin with underscore or alphabetic character */
    if (*L != '_' && !IsAlpha (*L)) {
        return 0;
    }
    ++L;

    /* Remainder must be as above plus digits */
    while (*L) {
        if (*L != '_' && !IsAlNum (*L)) {
            return 0;
        }
        ++L;
    }

    /* Ok */
    return 1;
}



static unsigned GetBytesPerLine (const Collection* A)
/* Return the number of bytes per line from the attribute collection A */
{
    char        C;
    unsigned    BytesPerLine = 16;

    /* Check for a bytesperline attribute */
    const char* V = GetAttrVal (A, "bytesperline");
    if ((V && sscanf (V, "%u%c", &BytesPerLine, &C) != 1) ||
        (BytesPerLine < 1 || BytesPerLine > 64)) {
        Error ("Invalid value for attribute `bytesperline'");
    }
    return BytesPerLine;
}



static unsigned GetBase (const Collection* A)
/* Return the number base from the attribute collection A */
{
    char        C;
    unsigned    Base = 16;

    /* Check for a base attribute */
    const char* V = GetAttrVal (A, "base");
    if ((V && sscanf (V, "%u%c", &Base, &C) != 1) ||
        (Base != 2 && Base != 10 && Base != 16)) {
        Error ("Invalid value for attribute `base'");
    }
    return Base;
}



static const char* GetIdentifier (const Collection* A)
/* Return the label identifier from the attribute collection A */
{
    /* Check for a ident attribute */
    const char* Ident = GetAttrVal (A, "ident");
    if (Ident && !ValidIdentifier (Ident)) {
        Error ("Invalid value for attribute `ident'");
    }
    return Ident;
}



void WriteAsmFile (const StrBuf* Data, const Collection* A, const Bitmap* B)
/* Write the contents of Data to the given file in assembler (ca65) format */
{
    FILE*       F;
    const char* D;
    unsigned    Size;


    /* Get the name of the image */
    const StrBuf* S = GetBitmapName (B);

    /* Get the file name */
    const char* Name = NeedAttrVal (A, "name", "write");

    /* Check the number of bytes per line */
    unsigned BytesPerLine = GetBytesPerLine (A);

    /* Get the number base */
    unsigned Base = GetBase (A);

    /* Get the identifier */
    const char* Ident = GetIdentifier (A);

    /* Open the output file */
    F = fopen (Name, "w");
    if (F == 0) {
        Error ("Cannot open output file `%s': %s", Name, strerror (errno));
    }

    /* Write a readable header */
    fprintf (F,
             ";\n"
             "; This file was generated by %s %s from\n"
             "; %.*s (%ux%u, %u colors%s)\n"
             ";\n"
             "\n",
             ProgName,
             GetVersionAsString (),
             SB_GetLen (S), SB_GetConstBuf (S),
             GetBitmapWidth (B), GetBitmapHeight (B),
             GetBitmapColors (B),
             BitmapIsIndexed (B)? ", indexed" : "");

    /* If we have an assembler label, output that */
    if (Ident) {
        fprintf (F,
                 ".proc   %s\n"
                 "        COLORS = %u\n"
                 "        WIDTH  = %u\n"
                 "        HEIGHT = %u\n",
                 Ident,
                 GetBitmapColors (B),
                 GetBitmapWidth (B),
                 GetBitmapHeight (B));
    }

    /* Write the data */
    D    = SB_GetConstBuf (Data);
    Size = SB_GetLen (Data);
    while (Size) {

        unsigned I;

        /* Output one line */
        unsigned Chunk = Size;
        if (Chunk > BytesPerLine) {
            Chunk = BytesPerLine;
        }
        fputs ("        .byte   ", F);
        for (I = 0; I < Chunk; ++I) {
            unsigned char V = *D++;
            if (I > 0) {
                fputc (',', F);
            }
            switch (Base) {
                case 2:
                    fprintf (F, "%%%u%u%u%u%u%u%u%u",
                             (V >> 7) & 0x01, (V >> 6) & 0x01,
                             (V >> 5) & 0x01, (V >> 4) & 0x01,
                             (V >> 3) & 0x01, (V >> 2) & 0x01,
                             (V >> 1) & 0x01, (V >> 0) & 0x01);
                    break;

                case 10:
                    fprintf (F, "%u", V);
                    break;

                case 16:
                    fprintf (F, "$%02X", V);
                    break;

            }
        }
        fputc ('\n', F);

        /* Bump the counters */
        Size -= Chunk;
    }

    /* Terminate the .proc if we had an identifier */
    if (Ident) {
        fputs (".endproc\n", F);
    }

    /* Add an empty line at the end */
    fputc ('\n', F);

    /* Close the file */
    if (fclose (F) != 0) {
        Error ("Error closing output file `%s': %s", Name, strerror (errno));
    }
}
