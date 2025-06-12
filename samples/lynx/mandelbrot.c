/*****************************************************************************\
** mandelbrot sample program for Atari Lynx                                  **
**                                                                           **
** (w) 2002 by groepaz/hitmen, TGI support by Stefan Haubenthal              **
\*****************************************************************************/



#include <stdlib.h>
#include <tgi.h>



/* Graphics definitions */
#define SCREEN_X        (tgi_getxres())
#define SCREEN_Y        (tgi_getyres())
#define MAXCOL          (tgi_getcolorcount())

#define maxiterations   32
#define fpshift         (10)
#define tofp(_x)        ((_x)<<fpshift)
#define fromfp(_x)      ((_x)>>fpshift)
#define fpabs(_x)       (abs(_x))

#define mulfp(_a,_b)    ((((signed long)_a)*(_b))>>fpshift)
#define divfp(_a,_b)    ((((signed long)_a)<<fpshift)/(_b))

/* Use static local variables for speed */
#pragma static-locals (1)



static void mandelbrot (signed short x1, signed short y1, signed short x2,
                        signed short y2)
{
    /*  */
    register signed short r, r1, i;
    register signed short xs, ys, xx, yy;
    register signed short x, y;
    register unsigned char count;
    register unsigned char maxcol = MAXCOL;

    /* Calc stepwidth */
    xs = ((x2 - x1) / (SCREEN_X));
    ys = ((y2 - y1) / (SCREEN_Y));

    yy = y1;
    for (y = 0; y < (SCREEN_Y); y++) {
        yy += ys;
        xx = x1;
        for (x = 0; x < (SCREEN_X); x++) {
            xx += xs;
            /* Do iterations */
            r = 0;
            i = 0;
            for (count = 0; (count < maxiterations) &&
                 (fpabs (r) < tofp (2)) && (fpabs (i) < tofp (2));
                 ++count) {
                r1 = (mulfp (r, r) - mulfp (i, i)) + xx;
                /* i = (mulfp(mulfp(r,i),tofp(2)))+yy; */
                i = (((signed long) r * i) >> (fpshift - 1)) + yy;
                r = r1;
            }
            if (count == maxiterations) {
                tgi_setcolor (0);
            } else {
                tgi_setcolor (count % maxcol);
            }
            /* Set pixel */
            tgi_setpixel (x, y);
        }
    }
}

void main (void)
{
    /* Install the graphics driver */
    tgi_install (tgi_static_stddrv);

    /* Initialize graphics */
    tgi_init ();
    tgi_clear ();

    /* Calc mandelbrot set */
    mandelbrot (tofp (-2), tofp (-2), tofp (2), tofp (2));
}
