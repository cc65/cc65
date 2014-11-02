/*****************************************************************************/
/*                                                                           */
/*                                    c.c                                    */
/*                                                                           */
/*              C output for the sp65 sprite and bitmap utility              */
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
#include "c.h"
#include "error.h"



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



static int ValidIdentifier (const char* L)
/* Check a C identifier for validity */
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
    if ((V && sscanf (V, "%u%c", &Base, &C) != 1) || (Base != 10 && Base != 16)) {
        Error ("Invalid value for attribute `base'");
    }
    return Base;
}



static const char* GetIdentifier (const Collection* A)
/* Return the variable identifier from the attribute collection A */
{
    /* Check for a ident attribute */
    const char* Ident = GetAttrVal (A, "ident");
    if (Ident && !ValidIdentifier (Ident)) {
        Error ("Invalid value for attribute `ident'");
    }
    return Ident;
}



void WriteCFile (const StrBuf* Data, const Collection* A, const Bitmap* B)
/* Write the contents of Data to a file in C format */
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
             "/*\n"
             "** This file was generated by %s %s from\n"
             "** %.*s (%ux%u, %u colors%s)\n"
             "*/\n"
             "\n",
             ProgName,
             GetVersionAsString (),
             SB_GetLen (S), SB_GetConstBuf (S),
             GetBitmapWidth (B), GetBitmapHeight (B),
             GetBitmapColors (B),
             BitmapIsIndexed (B)? ", indexed" : "");

    /* If an identifier was given, output #defines for width, height, the
    ** number of colors and declare a variable for the data.
    */
    if (Ident) {
        fprintf (F,
                 "#define %s_COLORS       %u\n"
                 "#define %s_WIDTH        %u\n"
                 "#define %s_HEIGHT       %u\n"
                 "const unsigned char %s[] = {\n",
                 Ident, GetBitmapColors (B),
                 Ident, GetBitmapWidth (B),
                 Ident, GetBitmapHeight (B),
                 Ident);
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
        fputs ("    ", F);
        for (I = 0; I < Chunk; ++I) {
            switch (Base) {
                case 10:
                    fprintf (F, "%u,", *D++ & 0xFF);
                    break;
                case 16:
                    fprintf (F, "0x%02X,", *D++ & 0xFF);
                    break;

            }
        }
        fputc ('\n', F);

        /* Bump the counters */
        Size -= Chunk;
    }

    /* Terminate the array if we had an identifier */
    if (Ident) {
        fputs ("};\n", F);
    }

    /* Close the file */
    if (fclose (F) != 0) {
        Error ("Error closing output file `%s': %s", Name, strerror (errno));
    }
}
