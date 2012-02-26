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

/* common */
#include "print.h"
#include "xmalloc.h"

/* sp65 */
#include "error.h"
#include "fileio.h"
#include "pcx.h"



/*****************************************************************************/
/*                                  Macros                                   */
/*****************************************************************************/



/* Some PCX constants */
#define PCX_MAGIC_ID            0x0A

/* A raw PCX header is just a block of bytes */
typedef unsigned char           RawPCXHeader[128];

/* Structured PCX header */
typedef struct PCXHeader PCXHeader;
struct PCXHeader {
    unsigned    Id;
    unsigned    FileVersion;
    unsigned    Compressed;
    unsigned    BPP;
    unsigned    XMin;
    unsigned    YMin;
    unsigned    XMax;
    unsigned    YMax;
    unsigned    XDPI;
    unsigned    YDPI;
    unsigned    Planes;
    unsigned    BytesPerPlane;
    unsigned    PalInfo;
    unsigned    ScreenWidth;
    unsigned    ScreenHeight;

    /* Calculated data */
    unsigned    Width;
    unsigned    Height;
};

/* Read a little endian word from a byte array at offset O */
#define WORD(H, O)              ((H)[O] | ((H)[O+1] << 8))



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



static PCXHeader* NewPCXHeader (void)
/* Allocate a new PCX header and return it */
{
    /* No initialization here */
    return xmalloc (sizeof (PCXHeader));
}



static PCXHeader* ReadPCXHeader (FILE* F, const char* Name)
/* Read a structured PCX header from the given file and return it */
{
    RawPCXHeader H;

    /* Allocate a new PCXHeader structure */
    PCXHeader* P = NewPCXHeader ();

    /* Read the raw header */
    ReadData (F, H, sizeof (H));

    /* Convert the data into structured form */
    P->Id               = H[0];
    P->FileVersion      = H[1];
    P->Compressed       = H[2];
    P->BPP              = H[3];
    P->XMin             = WORD (H, 4);
    P->YMin             = WORD (H, 6);
    P->XMax             = WORD (H, 8);
    P->YMax             = WORD (H, 10);
    P->XDPI             = WORD (H, 12);
    P->YDPI             = WORD (H, 14);
    P->Planes           = H[65];
    P->BytesPerPlane    = WORD (H, 66);
    P->PalInfo          = WORD (H, 68);
    P->ScreenWidth      = WORD (H, 70);
    P->ScreenHeight     = WORD (H, 72);
    P->Width            = P->XMax - P->XMin + 1;
    P->Height           = P->YMax - P->YMin + 1;

    /* Check the header data */
    if (P->Id != PCX_MAGIC_ID || P->FileVersion == 1 || P->FileVersion > 5) {
        Error ("`%s' is not a PCX file", Name);
    }
    if (P->Compressed > 1) {
        Error ("Unsupported compression (%d) in PCX file `%s'",
               P->Compressed, Name);
    }
    if (P->BPP != 1 && P->BPP != 4 && P->BPP != 8) {
        Error ("Unsupported bit depth (%u) in PCX file `%s'",
               P->BPP, Name);
    }
    if (P->PalInfo != 1 && P->PalInfo != 2) {
        Error ("Unsupported palette info (%u) in PCX file `%s'",
               P->PalInfo, Name);
    }
    if (!ValidBitmapSize (P->Width, P->Height)) {
        Error ("PCX file `%s' has an unsupported size (w=%u, h=%d)",
               Name, P->Width, P->Height);
    }

    /* Return the structured header */
    return P;
}



static void DumpPCXHeader (const PCXHeader* P, const char* Name)
/* Dump the header of the PCX file in readable form to stdout */
{
    printf ("File name:      %s\n", Name);
    printf ("PCX Version:    ");
    switch (P->FileVersion) {
        case 0: puts ("2.5");                             break;
        case 2: puts ("2.8 with palette");                break;
        case 3: puts ("2.8 without palette");             break;
        case 4: puts ("PCX for Windows without palette"); break;
        case 5: puts ("3.0");                             break;
    }
    printf ("Image type:     %s\n", P->PalInfo? "color" : "grayscale");
    printf ("Compression:    %s\n", P->Compressed? "RLE" : "None");
    printf ("Structure:      %u planes of %u bits\n", P->Planes, P->BPP);
    printf ("Bounding box:   [%u/%u - %u/%u]\n", P->XMin, P->YMin, P->XMax, P->YMax);
    printf ("Resolution:     %u/%u DPI\n", P->XDPI, P->YDPI);
    printf ("Screen size:    %u/%u\n", P->ScreenWidth, P->ScreenHeight);
}



Bitmap* ReadPCXFile (const char* Name)
/* Read a bitmap from a PCX file */
{
    PCXHeader* P;
    Bitmap* B;

    /* Open the file */
    FILE* F = fopen (Name, "rb");
    if (F == 0) {
        Error ("Cannot open PCX file `%s': %s", Name, strerror (errno));
    }

    /* Read the PCX header */
    P = ReadPCXHeader (F, Name);

    /* Dump the header if requested */
    if (Verbosity > 0) {
        DumpPCXHeader (P, Name);
    }

    /* Close the file */
    fclose (F);

    /* Return the bitmap */
    return B;
}



