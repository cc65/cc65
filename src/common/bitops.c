/*****************************************************************************/
/*                                                                           */
/*                                 bitops.c                                  */
/*                                                                           */
/*                           Single bit operations                           */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998     Ullrich von Bassewitz                                        */
/*              Wacholderweg 14                                              */
/*              D-70597 Stuttgart                                            */
/* EMail:       uz@musoftware.de                                             */
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



#include "bitops.h"



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



unsigned BitFind (unsigned long Val)
/* Find the first bit that is set in Val. Val must *not* be zero */
{
    unsigned long Mask;
    unsigned Bit;

    /* Search for the bits */
    Mask = 1;
    Bit  = 0;
    while (1) {
        if (Val & Mask) {
            return Bit;
        }
        Mask <<= 1;
        ++Bit;
    }
}



void BitSet (void* Data, unsigned Bit)
/* Set a bit in a char array */
{
    /* Make a char pointer */
    unsigned char* D = Data;

    /* Set the bit */
    D [Bit / 8] |= 0x01 << (Bit % 8);
}



void BitReset (void* Data, unsigned Bit)
/* Reset a bit in a char array */
{
    /* Make a char pointer */
    unsigned char* D = Data;

    /* Set the bit */
    D [Bit / 8] &= ~(0x01 << (Bit % 8));
}



int BitIsSet (void* Data, unsigned Bit)
/* Check if a bit is set in a char array */
{
    /* Make a char pointer */
    unsigned char* D = Data;

    /* Check the bit state */
    return (D [Bit / 8] & (0x01 << (Bit % 8))) != 0;
}



int BitIsReset (void* Data, unsigned Bit)
/* Check if a bit is reset in a char array */
{
    /* Make a char pointer */
    unsigned char* D = Data;

    /* Check the bit state */
    return (D [Bit / 8] & (0x01 << (Bit % 8))) == 0;
}



void BitMerge (void* Target, const void* Source, unsigned Size)
/* Merge the bits of two char arrays (that is, do an or for the full array) */
{
    /* Make char arrays */
    unsigned char*       T = Target;
    const unsigned char* S = Source;

    /* Merge the arrays */
    while (Size--) {
        *T++ |= *S++;
    }
}
