/*****************************************************************************/
/*                                                                           */
/*                                 bitmap.h                                  */
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



#ifndef BITMAP_H
#define BITMAP_H



/* common */
#include "strbuf.h"

/* sp65 */
#include "palette.h"
#include "pixel.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Safety limit for the size of the bitmap in pixels */
#define BM_MAX_SIZE     4194304UL

/* Bitmap type */
enum BitmapType {
    bmUnknown,
    bmMonochrome,
    bmIndexed,
    bmRGB,
    bmRGBA
};
typedef enum BitmapType BitmapType;

/* Bitmap structure */
typedef struct Bitmap Bitmap;
struct Bitmap {

    /* Type of the bitmap */
    BitmapType  Type;

    /* Name of the bitmap. This is used for error messages and should be
     * something that allows the user to identify which bitmap the message
     * refers to. For bitmaps loaded from a file, using the file name is
     * a good idea.
     */
    StrBuf      Name;

    /* Pointer to some format specific data. May be used by the frontend.
     * The data is free'd as a block when calling FreeBitmap.
     */
    void*       Tag;

    /* Size of the bitmap */
    unsigned    Width;
    unsigned    Height;

    /* Palette for monochrome and indexed bitmap types, otherwise NULL */
    Palette*    Pal;

    /* Pixel data, dynamically allocated */
    Pixel       Data[1];
};



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



Bitmap* NewBitmap (unsigned Width, unsigned Height);
/* Create a new bitmap. The type is set to unknown and the palette to NULL */

void FreeBitmap (Bitmap* B);
/* Free a dynamically allocated bitmap */

int ValidBitmapSize (unsigned Width, unsigned Height);
/* Return true if this is a valid size for a bitmap */

Bitmap* SliceBitmap (const Bitmap* Original,
                     unsigned X, unsigned Y,
                     unsigned Width, unsigned Height);
/* Create a slice of the given bitmap. The slice starts at position X/Y of
 * the original and has the given width and height. Location 0/0 is at the
 * upper left corner.
 */

Color GetPixelColor (const Bitmap* B, unsigned X, unsigned Y);
/* Get the color for a given pixel. For indexed bitmaps, the palette entry
 * is returned.
 */

Pixel GetPixel (const Bitmap* B, unsigned X, unsigned Y);
/* Return a pixel from the bitmap. The returned value may either be a color
 * or a palette index, depending on the type of the bitmap.
 */



/* End of bitmap.h */

#endif



