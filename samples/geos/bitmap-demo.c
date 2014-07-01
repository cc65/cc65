/*
** Minimalistic GEOSLib bitmap demo program
**
** 2012-06-10, Oliver Schmidt (ol.sc@web.de)
**
** To create bitmap.c use the sp65 sprite and bitmap utility:
** sp65 -r logo.pcx -c geos-bitmap -w bitmap.c,ident=bitmap
**
*/


#include <conio.h>
#include <geos.h>


#include "bitmap.c"
#if (!(bitmap_COLORS  == 2            && \
       bitmap_WIDTH%8 == 0            && \
       bitmap_WIDTH   <= SC_PIX_WIDTH && \
       bitmap_HEIGHT  <= SC_PIX_HEIGHT))
#error Incompatible Bitmap
#endif


struct iconpic picture = {(char*)bitmap, 0, 0, bitmap_WIDTH/8, bitmap_HEIGHT};


void main(void)
{
    BitmapUp(&picture);
    cgetc();
}
