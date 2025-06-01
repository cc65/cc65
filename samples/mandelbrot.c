/*****************************************************************************\
** mandelbrot sample program for cc65.                                       **
**                                                                           **
** (w) 2002 by groepaz/hitmen, TGI support by Stefan Haubenthal              **
\*****************************************************************************/



#include <stdlib.h>
#include <time.h>
#include <conio.h>
#include <tgi.h>
#include <cc65.h>



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

/* Workaround missing clock stuff */
#ifdef __APPLE2__
#  define clock()       0
#  undef  CLK_TCK
#  define CLK_TCK       1
#endif

/* Use dynamically loaded driver by default */
#ifndef DYN_DRV
#  define DYN_DRV       1
#endif

/* Use static local variables for speed */
#pragma static-locals (1)



void mandelbrot (signed short x1, signed short y1, signed short x2,
                 signed short y2)
{
    register unsigned char count;
    register signed short r, r1, i;
    register signed short xs, ys, xx, yy;
    register signed short x, y;
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
                switch (maxcol) {
                  case 2:
                    tgi_setcolor (1);
                    break;
                  case 0:               /* 256 colors */
                    tgi_setcolor (count);
                    break;
                  default:
                    tgi_setcolor (count % maxcol);
                }
            }
            /* Set pixel */
            tgi_setpixel (x, y);
        }
    }
}

int main (void)
{
    clock_t t;
    unsigned long sec;
    unsigned sec10;
    unsigned char err;

    clrscr ();

#if DYN_DRV
    /* Load the graphics driver */
    cprintf ("initializing... mompls\r\n");
    tgi_load_driver (tgi_stddrv);
#else
    /* Install the graphics driver */
    tgi_install (tgi_static_stddrv);
#endif
    err = tgi_geterror ();
    if (err  != TGI_ERR_OK) {
        cprintf ("Error #%d initializing graphics.\r\n%s\r\n",
                 err, tgi_geterrormsg (err));
        if (doesclrscrafterexit ()) {
            cgetc ();
        }
        exit (EXIT_FAILURE);
    };
    cprintf ("ok.\n\r");

    /* Initialize graphics */
    tgi_init ();
    tgi_clear ();

    t = clock ();

    /* Calc mandelbrot set */
    mandelbrot (tofp (-2), tofp (-2), tofp (2), tofp (2));

    t = clock () - t;

    /* Fetch the character from the keyboard buffer and discard it */
    cgetc ();

    /* Shut down gfx mode and return to textmode */
    tgi_done ();

    /* Calculate stats */
    sec = (t * 10) / CLK_TCK;
    sec10 = sec % 10;
    sec /= 10;

    /* Output stats */
    cprintf ("time  : %lu.%us\n\r", sec, sec10);

    if (doesclrscrafterexit ()) {
        /* Wait for a key, then end */
        cputs ("Press any key when done...\n\r");
        cgetc ();
    }

    /* Done */
    return EXIT_SUCCESS;
}
