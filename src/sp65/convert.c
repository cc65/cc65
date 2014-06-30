/*****************************************************************************/
/*                                                                           */
/*                                 convert.c                                 */
/*                                                                           */
/*    Main target conversion module for the sp65 file and bitmap utility     */
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

/* sp65 */
#include "attr.h"
#include "convert.h"
#include "error.h"
#include "geosbitmap.h"
#include "geosicon.h"
#include "koala.h"
#include "lynxsprite.h"
#include "raw.h"
#include "vic2sprite.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Type of the entry in the converter table */
typedef struct ConverterMapEntry ConverterMapEntry;
struct ConverterMapEntry {
    const char*         Format;
    StrBuf*             (*ConvertFunc) (const Bitmap*, const Collection*);
};

/* Converter table, alphabetically sorted */
static const ConverterMapEntry ConverterMap[] = {
    {   "geos-bitmap",          GenGeosBitmap   },
    {   "geos-icon",            GenGeosIcon     },
    {   "koala",                GenKoala        },
    {   "lynx-sprite",          GenLynxSprite   },
    {   "raw",                  GenRaw          },
    {   "vic2-sprite",          GenVic2Sprite   },
};



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



static int Compare (const void* Key, const void* MapEntry)
/* Compare function for bsearch */
{
    return strcmp (Key, ((const ConverterMapEntry*) MapEntry)->Format);
}



StrBuf* ConvertTo (const Bitmap* B, const Collection* A)
/* Convert the bitmap B into some sort of other binary format. The output is
** stored in a string buffer (which is actually a dynamic char array) and
** returned. The actual output format is taken from the "format" attribute
** in the attribute collection A.
*/
{
    const ConverterMapEntry* E;

    /* Get the format to convert to */
    const char* Format = NeedAttrVal (A, "format", "convert");

    /* Search for the matching converter */
    E = bsearch (Format,
                 ConverterMap,
                 sizeof (ConverterMap) / sizeof (ConverterMap[0]),
                 sizeof (ConverterMap[0]),
                 Compare);
    if (E == 0) {
        Error ("No such target format: `%s'", Format);
    }

    /* Do the conversion */
    return E->ConvertFunc (B, A);
}



void ListConversionTargets (FILE* F)
/* Output a list of conversion targets */
{
    unsigned I;
    for (I = 0; I < sizeof (ConverterMap) / sizeof (ConverterMap[0]); ++I) {
        fprintf (F, "  %s\n", ConverterMap[I].Format);
    }
}
