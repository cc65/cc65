/*****************************************************************************/
/*                                                                           */
/*                               lynxsprite.c                                */
/*                                                                           */
/*    Lynx sprite format backend for the sp65 sprite and bitmap utility      */
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
#include "lynxsprite.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Sprite mode */
enum Mode {
    smAuto,
    smLiteral,
    smPacked
};

/* Screen size of a koala picture */
#define WIDTH_HR        24U
#define WIDTH_MC        12U
#define HEIGHT          21U



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



static enum Mode GetMode (const Collection* A)
/* Return the sprite mode from the attribute collection A */
{
    /* Check for a mode attribute */
    const char* Mode = GetAttrVal (A, "mode");
    if (Mode) {
        if (strcmp (Mode, "literal") == 0) {
            return smLiteral;
        } else if (strcmp (Mode, "packed") == 0) {
            return smPacked;
        } else {
            Error ("Invalid value for attribute `mode'");
        }
    } else {
        return smAuto;
    }
}



StrBuf* GenLynxSprite (const Bitmap* B, const Collection* A)
/* Generate binary output in Lynx sprite format for the bitmap B. The output
 * is stored in a string buffer (which is actually a dynamic char array) and
 * returned.
 */
{
    enum Mode M;
    StrBuf* D;
    unsigned X, Y;


    /* Output the image properties */
    Print (stdout, 1, "Image is %ux%u with %u colors%s\n",
           GetBitmapWidth (B), GetBitmapHeight (B), GetBitmapColors (B),
           BitmapIsIndexed (B)? " (indexed)" : "");

    /* Get the sprite mode */
    M = GetMode (A);

    /* Check the height of the bitmap */
    if (GetBitmapHeight (B) != HEIGHT) {
        Error ("Invalid bitmap height (%u) for conversion to vic2 sprite",
               GetBitmapHeight (B));
    }

    /* If the mode wasn't given, fallback to packed */
    if (M == smAuto) {
        M = smPacked;
    }

    /* Now check if bitmap indexes are ok */
    if (GetBitmapColors (B) > 16) {
        Error ("Too many colors for a Lynx sprite");
    }

    /* Create the output buffer and resize it to the required size. */
    D = NewStrBuf ();
    SB_Realloc (D, 63);

    /* Convert the image */
    for (Y = 0; Y < HEIGHT; ++Y) {
        unsigned char V = 0;
        if (M == smLiteral) {
            for (X = 0; X < WIDTH_HR; ++X) {

                /* Fetch next bit into byte buffer */
                V = (V << 1) | (GetPixel (B, X, Y).Index & 0x01);

                /* Store full bytes into the output buffer */
                if ((X & 0x07) == 0x07) {
                    SB_AppendChar (D, V);
                    V = 0;
                }
            }
        } else {
            for (X = 0; X < WIDTH_MC; ++X) {

                /* Fetch next bit into byte buffer */
                V = (V << 2) | (GetPixel (B, X, Y).Index & 0x03);

                /* Store full bytes into the output buffer */
                if ((X & 0x03) == 0x03) {
                    SB_AppendChar (D, V);
                    V = 0;
                }
            }
        }
    }

    /* Return the converted bitmap */
    return D;
}



