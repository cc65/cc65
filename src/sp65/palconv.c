/*****************************************************************************/
/*                                                                           */
/*                                 palconv.c                                 */
/*                                                                           */
/*      Color palette conversions for the sp65 sprite and bitmap utility     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2022,      Karri Kaksonen                                             */
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
#include <stdlib.h>

/* common */
#include "check.h"
#include "xmalloc.h"

/* sp65 */
#include "attr.h"
#include "error.h"
#include "palette.h"
#include "lynxpalette.h"


/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/


/* Type of the entry in the palette table */
typedef struct PaletteMapEntry PaletteMapEntry;
struct PaletteMapEntry {
    const char*         Format;
    StrBuf*             (*PaletteFunc) (const Bitmap*, const Collection*);
};

/* Converter table */
/* CAUTION: table must be alphabetically sorted for bsearch */
static const PaletteMapEntry PaletteMap[] = {
    {   "lynx-palette",         GenLynxPalette  },
};



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/


static int Compare (const void* Key, const void* MapEntry)
/* Compare function for bsearch */
{
    return strcmp (Key, ((const PaletteMapEntry*) MapEntry)->Format);
}



StrBuf* PaletteTo (const Bitmap* B, const Collection* A)
/* Convert the palette of bitmap B into some sort of other binary format.
** The output is stored in a string buffer (which is actually a dynamic
** char array) and returned. The actual output format is taken from the
** "format" attribute in the attribute collection A.
*/
{
    const PaletteMapEntry* E;

    /* Get the format to convert to */
    const char* Format = NeedAttrVal (A, "target", "palette");

    /* Search for the matching converter */
    E = bsearch (Format,
                 PaletteMap,
                 sizeof (PaletteMap) / sizeof (PaletteMap[0]),
                 sizeof (PaletteMap[0]),
                 Compare);
    if (E == 0) {
        Error ("No such target format: '%s'", Format);
    }

    /* Do the conversion */
    return E->PaletteFunc (B, A);
}

