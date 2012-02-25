/*****************************************************************************/
/*                                                                           */
/*                                   pcx.c                                   */
/*                                                                           */
/*                              Read PCX files                               */
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

/* sp65 */
#include "error.h"
#include "fileio.h"
#include "pcx.h"



/*****************************************************************************/
/*                                  Macros                                   */
/*****************************************************************************/



/* Size of the PCX header and other constants */
#define PCX_HEADER_SIZE         128
#define PCX_MAGIC_ID            0x0A

/* Type of a PCX header */
typedef unsigned char           PCXHeader[PCX_HEADER_SIZE];

/* The following macros are used to access the PCX header, which is a 128 byte
 * block of raw data.
 */
#define WORD(H, O)              ((H)[O] | ((H)[O+1] << 8))
#define PCX_ID(H)               ((H)[0])
#define PCX_FILE_VERSION(H)     ((H)[1])
#define PCX_COMPRESSION(H)      ((H)[2])
#define PCX_BPP(H)              ((H)[3])
#define PCX_XMIN(H)             WORD(H, 4)
#define PCX_YMIN(H)             WORD(H, 6)
#define PCX_XMAX(H)             WORD(H, 8)
#define PCX_YMAX(H)             WORD(H, 10)
#define PCX_PLANES(H)           ((H)[65])
#define PCX_BYTES_PER_LINE(H)   WORD(H, 66)
#define PCX_PAL_INFO(H)         WORD(H, 68)
#define PCX_WIDTH(H)            WORD(H, 70)
#define PCX_HEIGHT(H)           WORD(H, 72)



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



Bitmap* ReadPCXFile (const char* Name)
/* Read a bitmap from a PCX file */
{
    PCXHeader H;
    unsigned Version, Compression;
    unsigned BPP, Planes, BytesPerLine;
    unsigned Width, Height;
    Bitmap* B;

    /* Open the file */
    FILE* F = fopen (Name, "rb");
    if (F == 0) {
        Error ("Cannot open PCX file `%s': %s", Name, strerror (errno));
    }

    /* Read the PCX header */
    ReadData (F, H, sizeof (H));

    /* Check the magic id byte */
    if (PCX_ID (H) != PCX_MAGIC_ID) {
        Error ("`%s' is not a PCX file", Name);
    }

    /* Read more data */
    Version      = PCX_FILE_VERSION (H);
    Compression  = PCX_COMPRESSION (H);
    BPP          = PCX_BPP (H);
    Planes       = PCX_PLANES (H);
    BytesPerLine = PCX_BYTES_PER_LINE (H);
    Width        = PCX_XMAX (H) - PCX_XMIN (H) + 1;
    Height       = PCX_YMAX (H) - PCX_YMIN (H) + 1;

    /* Check the data */
    if (Compression != 0 && Compression != 1) {
        Error ("Unsupported compression (%d) in PCX file `%s'",
               Compression, Name);
    }
    if (BPP != 1 && BPP != 4 && BPP != 8) {
        Error ("Unsupported bit depth (%d) in PCX file `%s'",
               BPP, Name);
    }
    if (!ValidBitmapSize (Width, Height)) {
        Error ("PCX file `%s' has an unsupported size (w=%u, h=%d)",
               Name, Width, Height);
    }

    /* Close the file */
    fclose (F);

    /* Return the bitmap */
    return B;
}



