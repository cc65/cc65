/*****************************************************************************/
/*                                                                           */
/*                                geosicon.c                                 */
/*                                                                           */
/*      GEOS icon format backend for the sp65 sprite and bitmap utility      */
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
#include "attrib.h"
#include "print.h"

/* sp65 */
#include "attr.h"
#include "error.h"
#include "geosicon.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Screen size of geos icon */
#define WIDTH           24U
#define HEIGHT          21U



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



StrBuf* GenGeosIcon (const Bitmap* B, const Collection* A attribute ((unused)))
/* Generate binary output in GEOS icon format for the bitmap B. The output
** is stored in a string buffer (which is actually a dynamic char array) and
** returned.
*/
{
    StrBuf* D;
    unsigned X, Y;


    /* Output the image properties */
    Print (stdout, 1, "Image is %ux%u with %u colors%s\n",
           GetBitmapWidth (B), GetBitmapHeight (B), GetBitmapColors (B),
           BitmapIsIndexed (B)? " (indexed)" : "");

    /* Check the bitmap properties */
    if (!BitmapIsIndexed (B)            ||
        GetBitmapHeight (B) != HEIGHT   ||
        GetBitmapWidth (B) != WIDTH     ||
        GetBitmapColors (B) > 2) {
        Error ("Invalid bitmap properties for conversion to GEOS icon");
    }

    /* Create the output buffer and resize it to the required size. */
    D = NewStrBuf ();
    SB_Realloc (D, 63);

    /* Convert the image */
    for (Y = 0; Y < HEIGHT; ++Y) {
        unsigned char V = 0;
        for (X = 0; X < WIDTH; ++X) {

            /* Fetch next bit into byte buffer */
            V = (V << 1) | (GetPixel (B, X, Y).Index & 0x01);

            /* Store full bytes into the output buffer */
            if ((X & 0x07) == 0x07) {
                SB_AppendChar (D, V);
                V = 0;
            }
        }
    }

    /* Return the converted bitmap */
    return D;
}
