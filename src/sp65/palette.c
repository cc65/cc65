/*****************************************************************************/
/*                                                                           */
/*                                 palette.c                                 */
/*                                                                           */
/*      Color palette definition for the sp65 sprite and bitmap utility      */
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



#include <string.h>

/* common */
#include "check.h"
#include "xmalloc.h"

/* sp65 */
#include "palette.h"



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



Palette* NewPalette (unsigned Entries)
/* Create a new palette with the given number of entries */
{
    Palette* P;

    /* Some safety checks */
    PRECONDITION (Entries > 0 && Entries <= 256);

    /* Allocate memory */
    P = xmalloc (sizeof (*P) + (Entries - 1) * sizeof (P->Entries[0]));

    /* Initialize the struct, then return it */
    P->Count = Entries;
    return P;
}



Palette* NewMonochromePalette (void)
/* Create and return a palette with two entries (black and white) */
{
    /* Create a new palette */
    Palette* P = NewPalette (2);

    /* Set the two colors */
    P->Entries[0] = RGBA (0x00, 0x00, 0x00, 0x00);
    P->Entries[1] = RGBA (0xFF, 0xFF, 0xFF, 0x00);

    /* Return the new palette */
    return P;
}



Palette* DupPalette (const Palette* P)
/* Create a copy of a palette */
{
    Palette* N;


    /* Allow to pass a NULL palette */
    if (P == 0) {
        return 0;
    }

    /* Create a new palette */
    N = NewPalette (P->Count);

    /* Copy the palette data */
    memcpy (N->Entries, P->Entries, P->Count * sizeof (P->Entries[0]));

    /* Return the copy */
    return N;
}



void FreePalette (Palette* P)
/* Free a dynamically allocated palette */
{
    xfree (P);
}



void DumpPalette (FILE* F, const Palette* P)
/* Dump the palette in readable form to the given file */
{
    unsigned I;

    fputs ("Entry     R      G      B      A     Combined\n", F);
    fputs ("----------------------------------------------\n", F);
    for (I = 0; I < P->Count; ++I) {

        /* Get the color entry */
        const Color* C = P->Entries + I;

        /* Output it */
        fprintf (F,
                 " %3u    %3u    %3u    %3u    %3u    #%08lX\n",
                 I,
                 C->R, C->G, C->B, C->A,
                 (((unsigned long) C->A) << 24) |
                 (((unsigned long) C->B) << 16) |
                 (((unsigned long) C->G) <<  8) |
                 (((unsigned long) C->R) <<  0));
    }
}
