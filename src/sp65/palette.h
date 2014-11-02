/*****************************************************************************/
/*                                                                           */
/*                                 palette.h                                 */
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



#ifndef PALETTE_H
#define PALETTE_H



#include <stdio.h>

/* sp65 */
#include "color.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



typedef struct Palette Palette;
struct Palette {
    unsigned            Count;          /* Number of entries */
    Color               Entries[1];     /* Palette entries - dynamic */
};



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



Palette* NewPalette (unsigned Entries);
/* Create a new palette with the given number of entries */

Palette* NewMonochromePalette (void);
/* Create and return a palette with two entries (black and white) */

Palette* DupPalette (const Palette* P);
/* Create a copy of a palette */

void FreePalette (Palette* P);
/* Free a dynamically allocated palette */

void DumpPalette (FILE* F, const Palette* P);
/* Dump the palette in readable form to the given file */



/* End of palette.h */

#endif
