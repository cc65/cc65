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
#include "attr.h"
#include "error.h"
#include "fileio.h"
#include "pcx.h"



/*****************************************************************************/
/*                                  Macros                                   */
/*****************************************************************************/



/* Some PCX constants */
#define PCX_MAGIC_ID            0x0A
#define PCX_MAX_PLANES          4

/* A raw PCX header is just a block of bytes */
typedef unsigned char           RawPCXHeader[128];

/* Structured PCX header */
typedef struct PCXHeader PCXHeader;
struct PCXHeader {
    unsigned        Id;
    unsigned        FileVersion;
    unsigned        Compressed;
    unsigned        BPP;
    unsigned        XMin;
    unsigned        YMin;
    unsigned        XMax;
    unsigned        YMax;
    unsigned        XDPI;
    unsigned        YDPI;
    unsigned        Planes;
    unsigned        BytesPerPlane;
    unsigned        PalInfo;
    unsigned        ScreenWidth;
    unsigned        ScreenHeight;

    /* Calculated data */
    unsigned        Width;
    unsigned        Height;
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



static void FreePCXHeader (PCXHeader* H)
/* Free a PCX header structure */
{
    xfree (H);
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
    /* We support:
    **   - one plane with either 1 or 8 bits per pixel
    **   - three planes with 8 bits per pixel
    **   - four planes with 8 bits per pixel (does this exist?)
    */
    if (!((P->BPP == 1 && P->Planes == 1) ||
          (P->BPP == 8 && (P->Planes == 1 || P->Planes == 3 || P->Planes == 4)))) {
        /* We could support others, but currently we don't */
        Error ("Unsupported PCX format: %u planes, %u bpp in PCX file `%s'",
               P->Planes, P->BPP, Name);
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
    printf ("File name:       %s\n", Name);
    printf ("PCX Version:     ");
    switch (P->FileVersion) {
        case 0: puts ("2.5");                             break;
        case 2: puts ("2.8 with palette");                break;
        case 3: puts ("2.8 without palette");             break;
        case 4: puts ("PCX for Windows without palette"); break;
        case 5: puts ("3.0");                             break;
    }
    printf ("Image type:      %s\n", P->PalInfo? "color" : "grayscale");
    printf ("Compression:     %s\n", P->Compressed? "RLE" : "None");
    printf ("Structure:       %u planes of %u bits\n", P->Planes, P->BPP);
    printf ("Bounding box:    [%u/%u - %u/%u]\n", P->XMin, P->YMin, P->XMax, P->YMax);
    printf ("Resolution:      %u/%u DPI\n", P->XDPI, P->YDPI);
    printf ("Screen size:     %u/%u\n", P->ScreenWidth, P->ScreenHeight);
    printf ("Bytes per plane: %u\n", P->BytesPerPlane);
}



static void ReadPlane (FILE* F, PCXHeader* P, unsigned char* L)
/* Read one (possibly compressed) plane from the file */
{
    if (P->Compressed) {

        /* Uncompress RLE data */
        unsigned Remaining = P->Width;
        while (Remaining) {

            unsigned char C;

            /* Read the next byte */
            unsigned char B = Read8 (F);

            /* Check for a run length */
            if ((B & 0xC0) == 0xC0) {
                C = (B & 0x3F);         /* Count */
                B = Read8 (F);          /* Value */
            } else {
                C = 1;
            }

            /* Write the data to the buffer */
            if (C > Remaining) {
                C = Remaining;
            }
            memset (L, B, C);

            /* Bump counters */
            L += C;
            Remaining -= C;

        }
    } else {

        /* Just read one line */
        ReadData (F, L, P->Width);

    }
}



Bitmap* ReadPCXFile (const Collection* A)
/* Read a bitmap from a PCX file */
{
    PCXHeader* P;
    Bitmap* B;
    unsigned char* L;
    Pixel* Px;
    unsigned MaxIdx = 0;
    unsigned X, Y;


    /* Get the file name */
    const char* Name = NeedAttrVal (A, "name", "read pcx file");

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

    /* Create the bitmap */
    B = NewBitmap (P->Width, P->Height);

    /* Copy the name */
    SB_CopyStr (&B->Name, Name);

    /* Allocate memory for the scan line */
    L = xmalloc (P->Width);

    /* Read the pixel data */
    Px = B->Data;
    if (P->Planes == 1) {

        /* This is either monochrome or indexed */
        if (P->BPP == 1) {
            /* Monochrome */
            for (Y = 0, Px = B->Data; Y < P->Height; ++Y) {

                unsigned I;
                unsigned char Mask;

                /* Read the plane */
                ReadPlane (F, P, L);

                /* Create pixels */
                for (X = 0, I = 0, Mask = 0x01; X < P->Width; ++Px) {
                    Px->Index = (L[I] & Mask) != 0;
                    if (Mask == 0x80) {
                        Mask = 0x01;
                        ++I;
                    } else {
                        Mask <<= 1;
                    }
                }

            }
        } else {
            /* One plane with 8bpp is indexed */
            for (Y = 0, Px = B->Data; Y < P->Height; ++Y) {

                /* Read the plane */
                ReadPlane (F, P, L);

                /* Create pixels */
                for (X = 0; X < P->Width; ++X, ++Px) {
                    if (L[X] > MaxIdx) {
                        MaxIdx = L[X];
                    }
                    Px->Index = L[X];
                }
            }
        }

        /* One plane means we have a palette which is either part of the header
        ** or follows.
        */
        if (P->PalInfo == 0) {

            /* Create the monochrome palette */
            B->Pal = NewMonochromePalette ();

        } else {

            unsigned      Count;
            unsigned      I;
            unsigned char Palette[256][3];
            unsigned long EndPos;

            /* Determine the current file position */
            unsigned long CurPos = FileGetPos (F);

            /* Seek to the end of the file */
            (void) fseek (F, 0, SEEK_END);

            /* Get this position */
            EndPos = FileGetPos (F);

            /* There's a palette if the old location is 769 bytes from the end */
            if (EndPos - CurPos == sizeof (Palette) + 1) {

                /* Seek back */
                FileSetPos (F, CurPos);

                /* Check for palette marker */
                if (Read8 (F) != 0x0C) {
                    Error ("Invalid palette marker in PCX file `%s'", Name);
                }

            } else if (EndPos == CurPos) {

                /* The palette is in the header */
                FileSetPos (F, 16);

                /* Check the maximum index for safety */
                if (MaxIdx > 15) {
                    Error ("PCX file `%s' contains more than 16 indexed colors "
                           "but no extra palette", Name);
                }

            } else {
                Error ("Error in PCX file `%s': %lu bytes at end of pixel data",
                       Name, EndPos - CurPos);
            }

            /* Read the palette. We will just read what we need. */
            Count = MaxIdx + 1;
            ReadData (F, Palette, Count * sizeof (Palette[0]));

            /* Create the palette from the data */
            B->Pal = NewPalette (Count);
            for (I = 0; I < Count; ++I) {
                B->Pal->Entries[I].R = Palette[I][0];
                B->Pal->Entries[I].G = Palette[I][1];
                B->Pal->Entries[I].B = Palette[I][2];
                B->Pal->Entries[I].A = 0;
            }

        }

    } else {

        /* 3 or 4 planes are RGB or RGBA (don't know if this exists) */
        for (Y = 0, Px = B->Data; Y < P->Height; ++Y) {

            /* Read the R plane and move the data */
            ReadPlane (F, P, L);
            for (X = 0; X < P->Width; ++X, ++Px) {
                Px->C.R = L[X];
            }

            /* Read the G plane and move the data */
            ReadPlane (F, P, L);
            for (X = 0; X < P->Width; ++X, ++Px) {
                Px->C.G = L[X];
            }

            /* Read the B plane and move the data */
            ReadPlane (F, P, L);
            for (X = 0; X < P->Width; ++X, ++Px) {
                Px->C.B = L[X];
            }

            /* Either read the A plane or clear it */
            if (P->Planes == 4) {
                ReadPlane (F, P, L);
                for (X = 0; X < P->Width; ++X, ++Px) {
                    Px->C.A = L[X];
                }
            } else {
                for (X = 0; X < P->Width; ++X, ++Px) {
                    Px->C.A = 0;
                }
            }
        }
    }

    /* Close the file */
    fclose (F);

    /* Free memory for the scan line */
    xfree (L);

    /* Free the PCX header */
    FreePCXHeader (P);

    /* Return the bitmap */
    return B;
}
