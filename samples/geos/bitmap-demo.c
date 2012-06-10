/*
 * Minimalistic GEOSLib bitmap demo program
 *
 * 2012-06-10, Oliver Schmidt (ol.sc@web.de)
 *
 * To create bitmap.c use the sp65 sprite and bitmap utility:
 * sp65 -r <input>.pcx -c geos-bitmap -w bitmap.c,ident=bitmap
 *
 */


#include <assert.h>
#include <conio.h>
#include <geos.h>


#include "bitmap.c"

//assert(bitmap_COLORS  == 2);
//assert(bitmap_WIDTH%8 == 0);
//assert(bitmap_WIDTH   <= SC_PIX_WIDTH);
//assert(bitmap_HEIGHT  <= SC_PIX_HEIGHT);


struct iconpic picture = {(char*)bitmap, 0, 0, bitmap_WIDTH/8, bitmap_HEIGHT};


void main(void)
{
    BitmapUp(&picture);
    cgetc();
}
