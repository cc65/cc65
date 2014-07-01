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



#include <stdlib.h>

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
    smShaped
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
        } else if (strcmp (Mode, "shaped") == 0) {
            return smShaped;
        } else {
            Error ("Invalid value for attribute `mode'");
        }
    }

    return smAuto;
}


static unsigned GetActionPointX (const Collection* A)
/* Return the sprite mode from the attribute collection A */
{
    /* Check for a action point x attribute */
    const char* ActionPointX = GetAttrVal (A, "ax");
    if (ActionPointX) {
        return atoi(ActionPointX);
    } else {
        return 0;
    }
}


static unsigned GetActionPointY (const Collection* A)
/* Return the sprite mode from the attribute collection A */
{
    /* Check for a action point y attribute */
    const char* ActionPointY = GetAttrVal (A, "ay");
    if (ActionPointY) {
        return atoi(ActionPointY);
    } else {
        return 0;
    }
}

static unsigned GetEdgeIndex (const Collection* A)
/* Return the sprite mode from the attribute collection A */
{
    /* Get index for edge color in shaped mode */
    const char* EdgeIndex = GetAttrVal (A, "edge");
    if (EdgeIndex) {
        return atoi(EdgeIndex);
    } else {
        return 0;
    }
}

static char OutBuffer[512]; /* The maximum size is 508 pixels */
static unsigned char OutIndex;

static void AssembleByte(unsigned bits, char val)
{
    static char bit_counter = 8, byte = 0;

    /* initialize */
    if (!bits) {
        OutIndex = 0;
        bit_counter = 8;
        byte = 0;
        return;
    }
    /* handle end of line */
    if (bits == 8) {
        if (bit_counter != 8) {
            byte <<= bit_counter;
            OutBuffer[OutIndex++] = byte;
            if (!OutIndex) {
                Error ("Sprite is too large for the Lynx");
            }
            if (byte & 0x1) {
                OutBuffer[OutIndex++] = byte;
                if (!OutIndex) {
                    Error ("Sprite is too large for the Lynx");
                }
            }
        }
        return;
    }
    /* handle end of line for literal */
    if (bits == 7) {
        if (bit_counter != 8) {
            byte <<= bit_counter;
            OutBuffer[OutIndex++] = byte;
            if (!OutIndex) {
                Error ("Sprite is too large for the Lynx");
            }
        }
        return;
    }
    val <<= 8 - bits;

    do {
        byte <<= 1;

        if (val & 0x80)
            ++byte;

        if (!(--bit_counter)) {
            OutBuffer[OutIndex++] = byte;
            if (!OutIndex) {
                Error ("Sprite is too large for the Lynx");
            }
            byte = 0;
            bit_counter = 8;
        }

        val <<= 1;

    } while (--bits);
}

static unsigned char ChoosePackagingMode(signed len, signed index, char ColorBits, char LineBuffer[512])
{
    --len;
    if (!len) {
        return 0;
    }
    if (LineBuffer[index] != LineBuffer[index + 1]) {
        return 0;
    }
    if (ColorBits > 2) {
        return 1;
    }
    if (LineBuffer[index] != LineBuffer[index + 2]) {
        return 0;
    }
    if (ColorBits > 1) {
        return 1;
    }
    if (LineBuffer[index] != LineBuffer[index + 3]) {
        return 0;
    }
    return 1;
}

static void WriteOutBuffer(StrBuf *D)
{
    signed i;

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
    for (i = 0; i < OutIndex; i++) {
        SB_AppendChar (D, OutBuffer[i]);
    }
}

static void encodeSprite(StrBuf *D, enum Mode M, char ColorBits, char ColorMask, char LineBuffer[512],
    int len, int LastOpaquePixel) {
/*
** The data starts with a byte count. It tells the number of bytes on this
** line + 1.
** Special case is a count of 1. It will change to next quadrant.
** Other special case is 0. It will end the sprite.
**
** Ordinary data packet. These are bits in a stream.
** 1=literal 0=packed
** 4 bit count (+1)
** for literal you put "count" values
** for packed you repeat the value "count" times
** Never use packed mode for one pixel
** If the last bit on a line is 1 you need to add a byte of zeroes
** A sequence 00000 ends a scan line
**
** All data is high nybble first
*/
    unsigned char V = 0;
    signed i;
    signed count;
    unsigned char differ[16];
    unsigned char *d_ptr;

    AssembleByte(0, 0);
    switch (M) {
    case smAuto:
    case smLiteral:
        for (i = 0; i < len; i++) {
            /* Fetch next pixel index into pixel buffer */
            AssembleByte(ColorBits, LineBuffer[i] & ColorMask);
        }
        AssembleByte(7, 0);
        /* Write the buffer to file */
        WriteOutBuffer(D);
        break;
    case smPacked:
        i = 0;
        while (len) {
            if (ChoosePackagingMode(len, i, ColorBits, LineBuffer)) {
                /* Make runlength packet */
                V = LineBuffer[i];
                ++i;
                --len;
                count = 0;
                do {
                    ++count;
                    ++i;
                    --len;
                } while (V == LineBuffer[i] && len && count != 15);

                AssembleByte(5, count);
                AssembleByte(ColorBits, V);

            } else {
                /* Make packed literal packet */
                d_ptr = differ;
                V = LineBuffer[i++];
                *d_ptr++ = V;
                --len;
                count = 0;
                while (ChoosePackagingMode(len, i, ColorBits, LineBuffer) == 0 && len && count != 15) {
                    V = LineBuffer[i++];
                    *d_ptr++ = V;
                    ++count;
                    --len;
                }

                AssembleByte(5, count | 0x10);
                d_ptr = differ;
                do {
                    AssembleByte(ColorBits, *d_ptr++);
                } while (--count >= 0);

            }
        }
        AssembleByte(8, 0);
        /* Write the buffer to file */
        WriteOutBuffer(D);
        break;

    case smShaped:
        if (LastOpaquePixel > -1) {
            if (LastOpaquePixel < len - 1) {
                len = LastOpaquePixel + 1;
            }
            i = 0;
            while (len) {
                if (ChoosePackagingMode(len, i, ColorBits, LineBuffer)) {
                    /* Make runlength packet */
                    V = LineBuffer[i];
                    ++i;
                    --len;
                    count = 0;
                    do {
                        ++count;
                        ++i;
                        --len;
                    } while (V == LineBuffer[i] && len && count != 15);

                    AssembleByte(5, count);
                    AssembleByte(ColorBits, V);

                } else {
                    /* Make packed literal packet */
                    d_ptr = differ;
                    V = LineBuffer[i++];
                    *d_ptr++ = V;
                    --len;
                    count = 0;
                    while (ChoosePackagingMode(len, i, ColorBits, LineBuffer) == 0 && len && count != 15) {
                        V = LineBuffer[i++];
                        *d_ptr++ = V;
                        ++count;
                        --len;
                    }

                    AssembleByte(5, count | 0x10);
                    d_ptr = differ;
                    do {
                        AssembleByte(ColorBits, *d_ptr++);
                    } while (--count >= 0);

                }
            }
            AssembleByte(5, 0);
            AssembleByte(8, 0);
            /* Write the buffer to file */
            WriteOutBuffer(D);
        }
        break;
    }
}

StrBuf* GenLynxSprite (const Bitmap* B, const Collection* A)
/* Generate binary output in Lynx sprite format for the bitmap B. The output
** is stored in a string buffer (which is actually a dynamic char array) and
** returned.
**
** The Lynx will draw 4 quadrants:
** - Down right
** - Up right
** - Up left
** - Down left
**
** The sprite will end with a byte 0.
*/
{
    enum Mode M;
    StrBuf* D;
    signed X, Y;
    unsigned OX, OY;
    char ColorBits;
    char ColorMask;
    char EdgeIndex;

    /* Get EdgeIndex */
    EdgeIndex = GetEdgeIndex (A);

    /* Action point of the sprite */
    OX = GetActionPointX (A);
    OY = GetActionPointY (A);
    if (OX >= GetBitmapWidth (B)) {
        Error ("Action point X cannot be larger than bitmap width");
    }
    if (OY >= GetBitmapHeight (B)) {
        Error ("Action point Y cannot be larger than bitmap height");
    }

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

            if (LineBuffer[i] != EdgeIndex) {
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

            if (LineBuffer[i] != EdgeIndex) {
                LastOpaquePixel = i;
            }
            ++i;
        }

        encodeSprite(D, M, ColorBits, ColorMask, LineBuffer, i, LastOpaquePixel);
    }

    if (OX == 0) {
        /* Special case only two quadrants */

        /* Mark end of sprite */
        SB_AppendChar (D, 0);

        /* Return the converted bitmap */
        return D;
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

            if (LineBuffer[i] != EdgeIndex) {
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

            if (LineBuffer[i] != EdgeIndex) {
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
