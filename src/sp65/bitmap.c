/*****************************************************************************/
/*                                                                           */
/*                                 bitmap.c                                  */
/*                                                                           */
/*         Bitmap definition for the sp65 sprite and bitmap utility          */
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



/* common */
#include "check.h"
#include "xmalloc.h"

/* sp65 */
#include "bitmap.h"
#include "error.h"



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



Bitmap* NewBitmap (unsigned Width, unsigned Height)
/* Create a new bitmap. The type is set to unknown and the palette to NULL */
{
    Bitmap* B;

    /* Calculate the size of the bitmap in pixels */
    unsigned long Size = (unsigned long) Width * Height;

    /* Some safety checks */
    PRECONDITION (Size > 0 && Size <= BM_MAX_SIZE);

    /* Allocate memory */
    B = xmalloc (sizeof (*B) + (Size - 1) * sizeof (B->Data[0]));

    /* Initialize the data */
    B->Type     = bmUnknown;
    B->Name     = EmptyStrBuf;
    B->Width    = Width;
    B->Height   = Height;
    B->Pal      = 0;

    /* Return the bitmap */
    return B;
}



void FreeBitmap (Bitmap* B)
/* Free a dynamically allocated bitmap */
{
    /* Free the palette and then the bitmap */
    xfree (B->Pal);
    xfree(B);
}



int ValidBitmapSize (unsigned Width, unsigned Height)
/* Return true if this is a valid size for a bitmap */
{
    /* Calculate the size of the bitmap in pixels */
    unsigned long Size = (unsigned long) Width * Height;

    /* Check the size */
    return (Size > 0 && Size <= BM_MAX_SIZE);
}



Bitmap* SliceBitmap (const Bitmap* O, unsigned OrigX, unsigned OrigY,
                     unsigned Width, unsigned Height)
/* Create a slice of the given bitmap. The slice starts at position X/Y of
 * the original and has the given width and height. Location 0/0 is at the
 * upper left corner.
 */
{
    Bitmap*  B;
    unsigned Y;


    /* Check the coordinates and size */
    PRECONDITION (OrigX != 0 && OrigY != 0 &&
                  OrigX + Width <= O->Width &&
                  OrigY + Height <= O->Height);

    /* Create a new bitmap with the given size */
    B = NewBitmap (Width, Height);

    /* Copy fields from the original */
    B->Type = O->Type;
    if (SB_GetLen (&O->Name) > 0) {
        SB_CopyStr (&B->Name, "Slice of ");
        SB_Append (&B->Name, &O->Name);
    }
    B->Pal = DupPalette (O->Pal);

    /* Copy the pixel data */
    for (Y = 0; Y < Height; ++Y) {
        memcpy (B->Data + Y * B->Width,
                O->Data + (OrigY + Y) * O->Width + OrigX,
                Width * sizeof (O->Data[0]));
    }

    /* Return the slice */
    return B;
}



Color GetPixelColor (const Bitmap* B, unsigned X, unsigned Y)
/* Get the color for a given pixel. For indexed bitmaps, the palette entry
 * is returned.
 */
{
    /* Get the pixel at the given coordinates */
    Pixel P = GetPixel (B, X, Y);

    /* If the bitmap has a palette, return the color from the palette. For
     * simplicity, we will only check the palette, not the type.
     */
    if (B->Pal) {
        if (P.Index >= B->Pal->Count) {
            /* Palette index is invalid */
            Error ("Invalid palette index %u at position %u/%u of \"%*s\"",
                   P.Index, X, Y, SB_GetLen (&B->Name),
                   SB_GetConstBuf (&B->Name));
        }
        return B->Pal->Entries[P.Index];
    } else {
        return P.C;
    }
}



Pixel GetPixel (const Bitmap* B, unsigned X, unsigned Y)
/* Return a pixel from the bitmap. The returned value may either be a color
 * or a palette index, depending on the type of the bitmap.
 */
{
    /* Check the coordinates */
    PRECONDITION (X < B->Width && Y < B->Height);

    /* Return the pixel */
    return B->Data[Y * B->Width + X];
}



unsigned GetBitmapColors (const Bitmap* B)
/* Get the number of colors in an image. The function will return the number
 * of palette entries for indexed bitmaps and 2^24 for non indexed bitmaps.
 */                                       
{                                   
    switch (B->Type) {
        case bmMonochrome:      return 2;
        case bmIndexed:         return B->Pal->Count;
        case bmRGB:
        case bmRGBA:            return (1U << 24);
        default:                Internal ("Unknown bitmap type %u", B->Type);
    }
}



