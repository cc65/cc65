/*****************************************************************************/
/*                                                                           */
/*                               geosbitmap.c                                */
/*                                                                           */
/*   GEOS compacted bitmap backend for the sp65 sprite and bitmap utility    */
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
#include "xmalloc.h"

/* sp65 */
#include "attr.h"
#include "error.h"
#include "geosbitmap.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



#define UNIQUE_MAX       91U            /* Maximum number of unique bytes */
#define REPEAT_MAX      127U            /* Maximum number of repeated bytes */



/* Structure used for RLE compression */
struct RLE {
    unsigned char*      Buf;            /* Pointer to pixel buffer */
    unsigned            Remaining;      /* Remaining bytes in buffer */
    unsigned char       LastVal;        /* Last value seen */
    unsigned            Count;          /* Number of occurrences for LastVal */
    StrBuf*             D;              /* Buffer for compressed data */
    StrBuf              UniqueData;     /* Temp buffer for unique data */
};



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



static void StoreByte (struct RLE* RLE)
/* Store a unique byte or a run of repeated bytes. If count is zero, the
** function will flush the internal buffers, so that all data is in RLE->D.
*/
{


    if (RLE->Count == 1 || RLE->Count == 2) {
        /* A run of two identical bytes is treated as two unique bytes, since
        ** this will usually merge with the following run.
        */
        SB_AppendChar (&RLE->UniqueData, RLE->LastVal);
        if (RLE->Count == 2) {
            SB_AppendChar (&RLE->UniqueData, RLE->LastVal);
        }

        /* Clear the repeat counter */
        RLE->Count = 0;

    } else {

        /* Run of repeated bytes. First flush the temp buffer for unique
        ** bytes.
        */
        const char* Buf = SB_GetConstBuf (&RLE->UniqueData);
        unsigned Count  = SB_GetLen (&RLE->UniqueData);
        while (Count) {

            /* Determine the count for this sequence */
            unsigned Chunk = Count;
            if (Chunk > UNIQUE_MAX) {
                Chunk = UNIQUE_MAX;
            }

            /* Output the unique op */
            SB_AppendChar (RLE->D, 0x80 + (unsigned char) Chunk);
            SB_AppendBuf (RLE->D, Buf, Chunk);

            /* Bump the counters */
            Buf += Chunk;
            Count -= Chunk;
        }

        /* Clear the unique byte buffer */
        SB_Clear (&RLE->UniqueData);

        /* Now output the repeat sequence */
        while (RLE->Count) {

            /* Determine the count for this sequence */
            unsigned Chunk = RLE->Count;
            if (Chunk > REPEAT_MAX) {
                Chunk = REPEAT_MAX;
            }
            /* Output the repeat op */
            SB_AppendChar (RLE->D, (unsigned char) Chunk);
            SB_AppendChar (RLE->D, RLE->LastVal);

            /* Bump the counters */
            RLE->Count -= Chunk;
        }
    }
}



StrBuf* GenGeosBitmap (const Bitmap* B, const Collection* A attribute ((unused)))
/* Generate binary output in GEOS compacted bitmap format for the bitmap B.
** The output is stored in a string buffer (which is actually a dynamic char
** array) and returned.
*/
{
    unsigned        LineWidth;
    unsigned char*  Buf;
    unsigned char*  BP;
    StrBuf*         D;
    unsigned        X, Y;
    struct RLE      RLE;


    /* Output the image properties */
    Print (stdout, 1, "Image is %ux%u with %u colors%s\n",
           GetBitmapWidth (B), GetBitmapHeight (B), GetBitmapColors (B),
           BitmapIsIndexed (B)? " (indexed)" : "");

    /* Check the bitmap properties */
    if (!BitmapIsIndexed (B) || GetBitmapColors (B) > 2) {
        Error ("Bitmaps converted to GEOS compacted bitmap must be "
               "indexed with two colors");
    }

    /* Calculate the width of one line in bytes */
    LineWidth = (GetBitmapWidth (B) + 7U) / 8U;

    /* Allocate a buffer for the raw image */
    Buf = xmalloc (LineWidth * GetBitmapHeight (B));

    /* Convert the bitmap into a raw image */
    BP = Buf;
    for (Y = 0; Y < GetBitmapHeight (B); ++Y) {
        for (X = 0; X < GetBitmapWidth (B); ) {
            unsigned char V = 0;
            int Bits = 8;
            if ((unsigned)Bits > GetBitmapWidth (B) - X) {
                Bits = (GetBitmapWidth (B) - X);
            }
            while (--Bits >= 0) {
                V |= (GetPixel (B, X++, Y).Index & 0x01) << Bits;
            }
            *BP++ = V;
        }
    }

    /* Create the output buffer and resize it to something reasonable */
    D = NewStrBuf ();
    SB_Realloc (D, 64);

    /* Compact the image. We're currently using only REPEAT and UNIQUE opcodes.
    ** BIGCOUNT is rather difficult to apply.
    */
    RLE.Buf        = Buf;
    RLE.Remaining  = LineWidth * GetBitmapHeight (B) - 1;
    RLE.LastVal    = *RLE.Buf++;
    RLE.Count      = 1;
    RLE.D          = D;
    RLE.UniqueData = EmptyStrBuf;
    while (RLE.Remaining--) {
        /* */
        if (RLE.LastVal == *RLE.Buf) {
            ++RLE.Buf;
            ++RLE.Count;
        } else {
            StoreByte (&RLE);
            RLE.LastVal = *RLE.Buf++;
            RLE.Count = 1;
        }
    }

    /* If something remains, store it */
    if (RLE.Count) {
        StoreByte (&RLE);
    }

    /* Flush the unique byte buffer */
    StoreByte (&RLE);

    /* Release the unique byte buffer */
    SB_Done (&RLE.UniqueData);

    /* Free the raw image buffer */
    xfree (Buf);

    /* Return the converted bitmap */
    return D;
}
