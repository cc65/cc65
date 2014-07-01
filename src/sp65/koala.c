/*****************************************************************************/
/*                                                                           */
/*                                  koala.c                                  */
/*                                                                           */
/*        Koala format backend for the sp65 sprite and bitmap utility        */
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

/* sp65 */
#include "error.h"
#include "koala.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Screen size of a koala picture */
#define WIDTH   160U
#define HEIGHT  200U



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



StrBuf* GenKoala (const Bitmap* B, const Collection* A attribute ((unused)))
/* Generate binary output in koala format for the bitmap B. The output is
** stored in a string buffer (which is actually a dynamic char array) and
** returned.
*/
{
    StrBuf* D;
    unsigned char Screen[160][200];
    unsigned char X, Y;

    /* Koala pictures are always 160x200 in size with 16 colors */
    if (!BitmapIsIndexed (B)             ||
        GetBitmapColors (B) > 16         ||
        GetBitmapHeight (B) != HEIGHT    ||
        GetBitmapWidth (B)  != WIDTH) {

        Error ("Bitmaps converted to koala format must be in indexed mode "
               "with 16 colors max and a size of %ux%u", WIDTH, HEIGHT);
    }

    /* Read the image into Screen */
    for (Y = 0; Y < HEIGHT; ++Y) {
        for (X = 0; X < WIDTH; ++X) {
            Screen[X][Y] = (unsigned char) GetPixel (B, X, Y).Index;
        }
    }

    /* Create the output buffer and resize it to the required size. */
    D = NewStrBuf ();
    SB_Realloc (D, 10003);

    /* Add $4400 as load address */
    SB_AppendChar (D, 0x00);
    SB_AppendChar (D, 0x44);
    
    /* TODO: The actual work ;-) */
    (void) Screen;

    /* Return the converted bitmap */
    return D;
}
