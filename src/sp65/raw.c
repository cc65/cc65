/*****************************************************************************/
/*                                                                           */
/*                                   raw.c                                   */
/*                                                                           */
/*        Raw image converter for the sp65 sprite and bitmap utility         */
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
#include "raw.h"



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



StrBuf* GenRaw (const Bitmap* B, const Collection* A attribute ((unused)))
/* Generate binary output in raw format. The output is stored in a string
** buffer (which is actually a dynamic char array) and returned.
*/
{
    StrBuf* D;
    unsigned X, Y;


    /* Output the image properties */
    Print (stdout, 1, "Image is %ux%u with %u colors%s\n",
           GetBitmapWidth (B), GetBitmapHeight (B), GetBitmapColors (B),
           BitmapIsIndexed (B)? " (indexed)" : "");

    /* Check the bitmap properties */
    if (!BitmapIsIndexed (B)) {
        Error ("The raw format needs an input bitmap in indexed format");
    }

    /* Create the output buffer and resize it to the required size. */
    D = NewStrBuf ();
    SB_Realloc (D, GetBitmapWidth (B) * GetBitmapHeight (B));

    /* Convert the image */
    for (Y = 0; Y < GetBitmapHeight (B); ++Y) {
        for (X = 0; X < GetBitmapWidth (B); ++X) {
            /* Place one pixel into the buffer */
            SB_AppendChar (D, (unsigned char) GetPixel (B, X, Y).Index);
        }
    }

    /* Return the converted bitmap */
    return D;
}
