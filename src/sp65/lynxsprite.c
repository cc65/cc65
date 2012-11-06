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
    smPacked,
    smPackedTransparent
};


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
        } else if (strcmp (Mode, "transparent") == 0) {
            return smPackedTransparent;
        } else {
            Error ("Invalid value for attribute `mode'");
        }
    } else {
        return smAuto;
    }
}


static void encodeSprite(StrBuf *D, enum Mode M, char ColorBits, char ColorMask, char LineBuffer[512],
    int i, int LastOpaquePixel) {
/*
 * The data starts with a byte count. It tells the number of bytes on this
 * line + 1.
 * Special case is a count of 1. It will change to next quadrant.
 * Other special case is 0. It will end the sprite.
 *
 * Ordinary data packet. These are bits in a stream.
 * 1=literal 0=packed
 * 4 bit count (+1)
 * for literal you put "count" values
 * for packed you repeat the value "count" times
 * Never use packed mode for one pixel
 * If the last bit on a line is 1 you need to add a byte of zeroes
 * A sequence 00000 ends a scan line
 *
 * All data is high nybble first
 */
    char OutBuffer[512]; /* The maximum size is 508 pixels */
    unsigned char OutIndex = 0;
    unsigned char V = 0;
    unsigned W = 0;
    signed j;
    signed k;

    switch (M) {
    case smAuto:
    case smLiteral:
        OutIndex = 0;
        k = 0;
        for (j = 0; j < i; j++) {
            /* Fetch next pixel index into pixel buffer */
            W = (W << ColorBits) | (LineBuffer[j] & ColorMask);
            k += ColorBits;
            if (k > 7) {
                /* The byte is ready */
                k -= 8;
                V = (W >> k) & 0xFF;
                OutBuffer[OutIndex++] = V;
                if (!OutIndex) {
                    Error ("Sprite is too large for the Lynx");
                }
            }
        }
        /* Output last bits */
        if (k != 0) {
            W = (W << (8-k));
            k = 0;
            V = W & 0xFF;
            OutBuffer[OutIndex++] = V;
            if (!OutIndex) {
                Error ("Sprite is too large for the Lynx");
            }
        }
        /* Fix bug in Lynx where the last bit on a line is 1 */
        if (V & 1) {
            OutBuffer[OutIndex++] = 0;
            if (!OutIndex) {
                Error ("Sprite is too large for the Lynx");
            }
        }
        /* Fix bug in Lynx where the count cannot be 1 */
        if (OutIndex == 1) {
            OutBuffer[OutIndex++] = 0;
        }
        /* Write the byte count to the end of the scanline */
        if (OutIndex == 255) {
            Error ("Sprite is too large for the Lynx");
        }
        SB_AppendChar (D, OutIndex+1);
        /* Write scanline data */
        for (j = 0; j < OutIndex; j++) {
            SB_AppendChar (D, OutBuffer[j]);
        }
        break;
    case smPacked:
        /* Bug workaround: If last bit is 1 and it is in bit0 add a zero byte */
        /* Note: These extra pixels will be painted also. There is no workaround for this */
        if (LineBuffer[i - 1] & 0x01) {
            LineBuffer[i++] = 0;
        }
        /* Logical problem workaround: The count can not be 1 so add an extra byte */
        if (i == 1) {
            LineBuffer[i++] = 0;
        }
        /* Write the byte count for this partial scanline */
        SB_AppendChar (D, i);
        for (i = 0; i < LineBuffer[0]; i++) {
            SB_AppendChar (D, LineBuffer[i]);
        }
        break;
    case smPackedTransparent:
        break;
    }
}

StrBuf* GenLynxSprite (const Bitmap* B, const Collection* A)
/* Generate binary output in Lynx sprite format for the bitmap B. The output
 * is stored in a string buffer (which is actually a dynamic char array) and
 * returned.
 *
 * The Lynx will draw 4 quadrants:
 * - Down right
 * - Up right
 * - Up left
 * - Down left
 *
 * The data starts with a byte count. It tells the number of bytes on this
 * line + 1.
 * Special case is a count of 1. It will change to next quadrant.
 * Other special case is 0. It will end the sprite.
 *
 * Ordinary data packet. These are bits in a stream.
 * 1=literal 0=packed
 * 4 bit count (+1)
 * for literal you put "count" values
 * for packed you repeat the value "count" times
 * Never use packed mode for one pixel
 * If the last bit on a line is 1 you need to add a byte of zeroes
 * A sequence 00000 ends a scan line
 *
 * All data is high nybble first
 */
{
    enum Mode M;
    StrBuf* D;
    signed X, Y;
    unsigned OX, OY;
    char ColorBits;
    char ColorMask;

    /* Anchor point of the sprite */
    OX = 0;
    OY = 0;

    /* Output the image properties */
    Print (stdout, 1, "Image is %ux%u with %u colors%s\n",
           GetBitmapWidth (B), GetBitmapHeight (B), GetBitmapColors (B),
           BitmapIsIndexed (B)? " (indexed)" : "");

    /* Get the sprite mode */
    M = GetMode (A);

    /* Now check if bitmap indexes are ok */
    if (GetBitmapColors (B) > 16) {
        Error ("Too many colors for a Lynx sprite");
    }
    ColorBits = 4;
    ColorMask = 0x0f;
    if (GetBitmapColors (B) < 9) {
        ColorBits = 3;
        ColorMask = 0x07;
    }
    if (GetBitmapColors (B) < 5) {
        ColorBits = 2;
        ColorMask = 0x03;
    }
    if (GetBitmapColors (B) < 3) {
        ColorBits = 1;
        ColorMask = 0x01;
    }

    /* Create the output buffer and resize it to the required size. */
    D = NewStrBuf ();
    SB_Realloc (D, 63);

    /* Convert the image for quadrant bottom right */
    for (Y = OY; Y < (signed)GetBitmapHeight (B); ++Y) {
        signed i = 0;
        signed LastOpaquePixel = -1;
        char LineBuffer[512]; /* The maximum size is 508 pixels */

        /* Fill the LineBuffer for easier optimisation */
        for (X = OX; X < (signed)GetBitmapWidth (B); ++X) {

            /* Fetch next bit into byte buffer */
            LineBuffer[i] = GetPixel (B, X, Y).Index & ColorMask;

            if (LineBuffer[i]) {
                LastOpaquePixel = i;
            }
            ++i;
        }

        encodeSprite(D, M, ColorBits, ColorMask, LineBuffer, i, LastOpaquePixel);
    }

    if ((OY == 0) && (OX == 0)) {
        /* Trivial case only one quadrant */

        /* Mark end of sprite */
        SB_AppendChar (D, 0);

        /* Return the converted bitmap */
        return D;
    }

    /* Next quadrant */
    SB_AppendChar (D, 1);

    /* Convert the image for quadrant top right */
    for (Y = OY - 1; Y >= 0; --Y) {
        signed i = 0;
        signed LastOpaquePixel = -1;
        char LineBuffer[512]; /* The maximum size is 508 pixels */

        /* Fill the LineBuffer for easier optimisation */
        for (X = OX; X < (signed)GetBitmapWidth (B); ++X) {

            /* Fetch next bit into byte buffer */
            LineBuffer[i] = GetPixel (B, X, Y).Index & ColorMask;

            if (LineBuffer[i]) {
                LastOpaquePixel = i;
            }
            ++i;
        }

        encodeSprite(D, M, ColorBits, ColorMask, LineBuffer, i, LastOpaquePixel);
    }

    /* Next quadrant */
    SB_AppendChar (D, 1);

    /* Convert the image for quadrant top left */
    for (Y = OY - 1; Y >= 0; --Y) {
        signed i = 0;
        signed LastOpaquePixel = -1;
        char LineBuffer[512]; /* The maximum size is 508 pixels */

        /* Fill the LineBuffer for easier optimisation */
        for (X = OX - 1; X >= 0; --X) {

            /* Fetch next bit into byte buffer */
            LineBuffer[i] = GetPixel (B, X, Y).Index & ColorMask;

            if (LineBuffer[i]) {
                LastOpaquePixel = i;
            }
            ++i;
        }

        encodeSprite(D, M, ColorBits, ColorMask, LineBuffer, i, LastOpaquePixel);
    }

    /* Next quadrant */
    SB_AppendChar (D, 1);

    /* Convert the image for quadrant bottom left */
    for (Y = OY; Y < (signed)GetBitmapHeight (B); ++Y) {
        signed i = 0;
        signed LastOpaquePixel = -1;
        char LineBuffer[512]; /* The maximum size is 508 pixels */

        /* Fill the LineBuffer for easier optimisation */
        for (X = OX - 1; X >= 0; --X) {

            /* Fetch next bit into byte buffer */
            LineBuffer[i] = GetPixel (B, X, Y).Index & ColorMask;

            if (LineBuffer[i]) {
                LastOpaquePixel = i;
            }
            ++i;
        }

        encodeSprite(D, M, ColorBits, ColorMask, LineBuffer, i, LastOpaquePixel);
    }

    /* End sprite */
    SB_AppendChar (D, 0);

    /* Return the converted bitmap */
    return D;
}



