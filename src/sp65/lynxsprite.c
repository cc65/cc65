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
            Error ("Invalid value for attribute 'mode'");
        }
    }

    return smAuto;
}


static unsigned GetActionPointX (const Bitmap* B, const Collection* A)
/* Return the sprite mode from the attribute collection A */
{
    /* Check for a action point x attribute */
    const char* ActionPointX = GetAttrVal (A, "ax");
    if (ActionPointX) {
        if (strcmp (ActionPointX, "mid") == 0) {
            return GetBitmapWidth (B) / 2;
        }
        if (strcmp (ActionPointX, "max") == 0) {
            return GetBitmapWidth (B) - 1;
        }
        return atoi(ActionPointX);
    } else {
        return 0;
    }
}


static unsigned GetActionPointY (const Bitmap* B, const Collection* A)
/* Return the sprite mode from the attribute collection A */
{
    /* Check for a action point y attribute */
    const char* ActionPointY = GetAttrVal (A, "ay");
    if (ActionPointY) {
        if (strcmp (ActionPointY, "mid") == 0) {
            return GetBitmapHeight (B) / 2;
        }
        if (strcmp (ActionPointY, "max") == 0) {
            return GetBitmapHeight (B) - 1;
        }
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

static unsigned GetQuadrant (const Collection* A)
/* Return the sprite mode from the attribute collection A */
{
    /* Get index for edge color in shaped mode */
    const char* Quadrant = GetAttrVal (A, "quadrant");
    if (Quadrant) {
        return atoi(Quadrant);
    } else {
        return 0;
    }
}

static void OptimizePenpal (const Bitmap* B, char *PenPal)
/* Create an optimal Penpal */
{
    char usage[16];
    unsigned I, J, Val;

    memset(usage, 0, sizeof(usage));
    for (J = 0; J < GetBitmapHeight (B); J++) {
        for (I = 0; I < GetBitmapWidth (B); I++) {
            Val = GetPixel (B, I, J).Index;
            if (Val < 16) {
                usage[Val] = 1;
            }
        }
    }
    J = 0;
    for (I = 0; I < 16; I++) {
        if (usage[I]) {
            switch (I) {
            case 0:
                PenPal[J] = '0';
                break;
            case 1:
                PenPal[J] = '1';
                break;
            case 2:
                PenPal[J] = '2';
                break;
            case 3:
                PenPal[J] = '3';
                break;
            case 4:
                PenPal[J] = '4';
                break;
            case 5:
                PenPal[J] = '5';
                break;
            case 6:
                PenPal[J] = '6';
                break;
            case 7:
                PenPal[J] = '7';
                break;
            case 8:
                PenPal[J] = '8';
                break;
            case 9:
                PenPal[J] = '9';
                break;
            case 10:
                PenPal[J] = 'a';
                break;
            case 11:
                PenPal[J] = 'b';
                break;
            case 12:
                PenPal[J] = 'c';
                break;
            case 13:
                PenPal[J] = 'd';
                break;
            case 14:
                PenPal[J] = 'e';
                break;
            case 15:
                PenPal[J] = 'f';
                break;
            }
            J++;
        }
    }
    while (J < 16) {
        PenPal[J] = 0;
        J++;
    }
    /* printf("Penpal %s\n", PenPal); */
}

static unsigned GetPenpal (const Bitmap* B, const Collection* A, char *PenPal)
/* Return the penpal from the attribute collection A */
{
    const char* Pen = GetAttrVal (A, "pen");
    if (Pen) {
        if (strcmp (Pen, "opt") == 0) {
            /* So we need to optimize the penpal and colour depth */
            OptimizePenpal (B, PenPal);
        } else {
            strncpy(PenPal, Pen, 17);
        }
        return 1;
    }
    return 0;
}

static unsigned GetBPP (const Collection* A)
/* Return the sprite depth from the attribute collection A */
{
    /* Get index for edge color in shaped mode */
    const char* BPP = GetAttrVal (A, "bpp");
    if (BPP) {
        return atoi(BPP);
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
    if (bits == 7) {
        if (bit_counter != 8) {
            byte <<= bit_counter;
            OutBuffer[OutIndex++] = byte;
            if (!OutIndex) {
                Error ("Sprite is too large for the Lynx");
            }
        } else {
            /* Add pad byte */
            byte = 0;
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

static unsigned char AnalyseNextChunks(signed *newlen, signed len, char data[32], char ColorBits)
{
    char longest = 1;
    unsigned char prev = 255;
    char count = 0;
    char index = 0;
    char lindex = 0;
    int i;
    int literal_cost;
    int packed_cost;

    for (i = 0; i < len; i++) {
        index = index + 1;
        if (data[i] == prev) {
            count = count + 1;
            if (count >= longest) {
                longest = count;
                lindex = index - count;
            }
        } else {
            prev = data[i];
            count = 1;
        }
    }
    if (longest == 1) {
        if (len > 16) {
            *newlen = 16;
        } else {
            *newlen = len;
        }
        return 'L';
    }
    if ((lindex > 0) && (lindex + longest > 15)) {
        /* We cannot pack the stride in this packet */
        *newlen = lindex;
        return 'A';
    }
    /* Cost till end of area */
    literal_cost = 5 + lindex * ColorBits + longest * ColorBits;
    packed_cost = 5 + lindex * ColorBits + 5 + ColorBits;
    if (packed_cost < literal_cost) {
        if (lindex == 0) {
            /* Use packed data */
            if (longest > 16) {
                *newlen = 16;
            } else {
                *newlen = longest;
            }
            return 'P';
        }
        /* We had a good find, but it was not at the start of the line */
        *newlen = lindex;
        return 'A';
    }
    /* There is no point in packing - use literal */
    if (len > 16) {
        *newlen = 16;
    } else {
        *newlen = len;
    }
    return 'L';
}

static unsigned char GetNextChunk(signed *newlen, signed len, char data[32], char ColorBits)
{
    char oper = 'A';

    while (oper == 'A') {
        oper = AnalyseNextChunks(newlen, len, data, ColorBits);
        len = *newlen;
    }
    return oper; /* The packet type is now P or L and the length is in newlen */
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
 * If the last bit on a line is in use you need to add a byte of zeroes
 * A sequence 00000 ends a scan line
 *
 * All data is high nybble first
 */
    unsigned char V = 0;
    signed i;
    signed count;

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
    case smShaped:
        if (M == smShaped) {
            if (LastOpaquePixel > -1) {
                if (LastOpaquePixel < len - 1) {
                    len = LastOpaquePixel + 1;
                        }
            } else {
                len = 0;
            }
   }
        i = 0;
        while (len) {
            signed analyselen;
            analyselen = len;
            if (analyselen > 32) {
                analyselen = 32;
            }
            if (GetNextChunk(&count, analyselen, LineBuffer + i, ColorBits) == 'P') {
                /* Make runlength packet */
                V = LineBuffer[i];
                i += count;
                len -= count;
                AssembleByte(5, count-1);
                AssembleByte(ColorBits, V & ColorMask);

            } else {
                /* Make packed literal packet */
                AssembleByte(5, (count-1) | 0x10);
                do {
                    AssembleByte(ColorBits, LineBuffer[i]);
                    i++;
                    len--;
                } while (--count > 0);
            }
        }
        /* Force EOL for shaped? AssembleByte(5, 0); */
        AssembleByte(7, 0);
        /* Write the buffer to file */
        WriteOutBuffer(D);
        break;
    }
}

StrBuf* GenLynxSprite (const Bitmap* B, const Collection* A)
/* Generate binary output in Lynx sprite format for the bitmap B. The output
** is stored in a string buffer (which is actually a dynamic char array) and
** returned.
**
** The Lynx will draw 4 quadrants:
** 0 - Down right
** 1 - Up right
** 2 - Up left
** 3 - Down left
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
    char Quadrant;
    char quad;
    char BPP;
    /* The default mapping is 1:1 plus extra colours become 0 */
    char Map[17] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,0};
    signed PenColors;
    char PenPal[18];
    signed Val;

    /* Get EdgeIndex */
    EdgeIndex = GetEdgeIndex (A);

    /* Get Quadrant for starting the draw process */
    Quadrant = GetQuadrant (A) & 3;

    /* Action point of the sprite */
    OX = GetActionPointX (B, A);
    OY = GetActionPointY (B, A);
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

    /* Now check how to do the mapping */
    if (GetPenpal (B, A, &PenPal[0])) {
            signed I;

            /* Modify the map by content of PenPal */
            PenColors = strlen(PenPal);
            for (I = 0; I < PenColors; I++) {
                switch (PenPal[I]) {
            case '0':
                    Map[0] = I;
                        break;
            case '1':
                    Map[1] = I;
                        break;
            case '2':
                    Map[2] = I;
                        break;
            case '3':
                    Map[3] = I;
                        break;
            case '4':
                    Map[4] = I;
                        break;
            case '5':
                    Map[5] = I;
                        break;
            case '6':
                    Map[6] = I;
                        break;
            case '7':
                    Map[7] = I;
                        break;
            case '8':
                    Map[8] = I;
                        break;
            case '9':
                    Map[9] = I;
                        break;
            case 'a':
            case 'A':
                    Map[10] = I;
                        break;
            case 'b':
            case 'B':
                    Map[11] = I;
                        break;
            case 'c':
            case 'C':
                    Map[12] = I;
                        break;
            case 'd':
            case 'D':
                    Map[13] = I;
                        break;
            case 'e':
            case 'E':
                    Map[14] = I;
                        break;
            case 'f':
            case 'F':
                    Map[15] = I;
                        break;
                /* The X is reserved as transparency. This allows for shaped sprites */
            case 'x':
            case 'X':
                    Map[16] = I;
                        break;
                }
            }
    } else {
            PenColors = GetBitmapColors (B);
    }
    ColorBits = 4;
    if (PenColors < 9) {
        ColorBits = 3;
    }
    if (PenColors < 5) {
        ColorBits = 2;
    }
    if (PenColors < 3) {
        ColorBits = 1;
    }

    BPP = GetBPP (A);
    if (BPP > 0) {
        ColorBits = BPP;
    }
    switch (ColorBits) {
    case 1:
        ColorMask = 0x01;
        break;
    case 2:
        ColorMask = 0x03;
        break;
    case 3:
        ColorMask = 0x07;
        break;
    default:
    case 4:
        ColorMask = 0x0f;
        break;
    }
    /* B->BPP = ColorBits; */
    /* Create the output buffer and resize it to the required size. */
    D = NewStrBuf ();
    SB_Realloc (D, 63);

    for (quad = 0; quad < 4; quad++) {
        switch ((Quadrant + quad) & 3) {
        case 0:
            /* Convert the image for quadrant bottom right */
            for (Y = OY; Y < (signed)GetBitmapHeight (B); ++Y) {
                signed i = 0;
                signed LastOpaquePixel = -1;
                char LineBuffer[512]; /* The maximum size is 508 pixels */

                /* Fill the LineBuffer for easier optimisation */
                for (X = OX; X < (signed)GetBitmapWidth (B); ++X) {
                    /* Fetch next bit into byte buffer */
                    Val = GetPixel (B, X, Y).Index;
                    if (Val > 16) Val = 16;
                    LineBuffer[i] = Map[Val] & ColorMask;

                    if (Val != EdgeIndex) {
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
            if ((quad == 1) && (OY == 0)) {
                /* Special case only two quadrants */

                /* Mark end of sprite */
                SB_AppendChar (D, 0);

                /* Return the converted bitmap */
                return D;
            }
            break;
        case 1:
            /* Convert the image for quadrant top right */
            for (Y = OY - 1; Y >= 0; --Y) {
                signed i = 0;
                signed LastOpaquePixel = -1;
                char LineBuffer[512]; /* The maximum size is 508 pixels */

                /* Fill the LineBuffer for easier optimisation */
                for (X = OX; X < (signed)GetBitmapWidth (B); ++X) {
                    /* Fetch next bit into byte buffer */
                    Val = GetPixel (B, X, Y).Index;
                    if (Val > 16) Val = 16;

                    LineBuffer[i] = Map[Val] & ColorMask;

                    if (Val != EdgeIndex) {
                        LastOpaquePixel = i;
                    }
                    ++i;
                }

                encodeSprite(D, M, ColorBits, ColorMask, LineBuffer, i, LastOpaquePixel);
            }

            if ((OY == GetBitmapHeight (B) - 1) && (OX == 0)) {
                /* Trivial case only one quadrant */

                /* Mark end of sprite */
                SB_AppendChar (D, 0);

                /* Return the converted bitmap */
                return D;
            }
            if ((quad == 1) && (OX == 0)) {
                /* Special case only two quadrants */

                /* Mark end of sprite */
                SB_AppendChar (D, 0);

                /* Return the converted bitmap */
                return D;
            }
            break;
        case 2:
            /* Convert the image for quadrant top left */
            for (Y = OY - 1; Y >= 0; --Y) {
                signed i = 0;
                signed LastOpaquePixel = -1;
                char LineBuffer[512]; /* The maximum size is 508 pixels */

                /* Fill the LineBuffer for easier optimisation */
                for (X = OX - 1; X >= 0; --X) {
                    /* Fetch next bit into byte buffer */
                    Val = GetPixel (B, X, Y).Index;
                    if (Val > 16) Val = 16;

                    LineBuffer[i] = Map[Val] & ColorMask;

                    if (Val != EdgeIndex) {
                        LastOpaquePixel = i;
                    }
                    ++i;
                }

                encodeSprite(D, M, ColorBits, ColorMask, LineBuffer, i, LastOpaquePixel);
            }
            if ((OY == GetBitmapHeight (B) - 1) && (OX == GetBitmapWidth (B) - 1)) {
                /* Trivial case only one quadrant */

                /* Mark end of sprite */
                SB_AppendChar (D, 0);

                /* Return the converted bitmap */
                return D;
            }
            if ((quad == 1) && (OY == GetBitmapHeight (B) - 1)) {
                /* Special case only two quadrants */

                /* Mark end of sprite */
                SB_AppendChar (D, 0);

                /* Return the converted bitmap */
                return D;
            }
            break;
        case 3:
            /* Convert the image for quadrant bottom left */
            for (Y = OY; Y < (signed)GetBitmapHeight (B); ++Y) {
                signed i = 0;
                signed LastOpaquePixel = -1;
                char LineBuffer[512]; /* The maximum size is 508 pixels */

                /* Fill the LineBuffer for easier optimisation */
                for (X = OX - 1; X >= 0; --X) {
                    /* Fetch next bit into byte buffer */
                    Val = GetPixel (B, X, Y).Index;
                    if (Val > 16) Val = 16;

                    LineBuffer[i] = Map[Val] & ColorMask;

                    if (Val != EdgeIndex) {
                        LastOpaquePixel = i;
                    }
                    ++i;
                }

                encodeSprite(D, M, ColorBits, ColorMask, LineBuffer, i, LastOpaquePixel);
            }
            if ((OY == 0) && (OX == GetBitmapWidth (B) - 1)) {
                /* Trivial case only one quadrant */

                /* Mark end of sprite */
                SB_AppendChar (D, 0);

                /* Return the converted bitmap */
                return D;
            }
            if ((quad == 1) && (OX == GetBitmapWidth (B) - 1)) {
                /* Special case only two quadrants */

                /* Mark end of sprite */
                SB_AppendChar (D, 0);

                /* Return the converted bitmap */
                return D;
            }
            break;
        }
        if (quad < 3) {
            /* Next quadrant */
            SB_AppendChar (D, 1);
        } else {
            /* End sprite */
            SB_AppendChar (D, 0);
        }
    }

    /* Return the converted bitmap */
    return D;
}
