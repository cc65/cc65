
/*
 * $ cl65 -o mandelfloat.prg mandelfloat.c
 *
 * use the CBM kernal fp routines like this:
 *
 * $ cl65 -o mandelfloat.prg mandelfloat.c c64-fp488kernal.o
 *
 */


#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define MAX_ITER        40

#define XRES        20
#define YRES        20

#define XMIN        -2.25f
#define XMAX        2.25f
#define YMIN        -2.25f
#define YMAX        2.25f

// FIXME: Invalid operands for binary operator '-'
// #define XRANGE      (XMAX - XMIN)
// #define YRANGE      (YMAX - YMIN)
#define XRANGE      4.5f
#define YRANGE      4.5f

#define XSTEP       (XRANGE / XRES)
#define YSTEP       (YRANGE / YRES)

char *colortab = ".+*#%$=-";

void color(int iterations)  {
    if (iterations >= MAX_ITER) {
        putchar(' ');
    } else {
        putchar(colortab[iterations & 7]);
    }
}

float x, y; /* Coordinates of the current point in the complex plane. */
float u, v; /* Coordinates of the iterated point. */
unsigned char i, j; /* Pixel counters */
unsigned char k;    /* Iteration counter */

float u, v;
float u2, v2;

int main(void)
{
    for (j = 0; j < YRES; j++) {
        y = YMIN + j * YSTEP;
        for(i = 0; i < XRES; i++) {

            u = 0.0f;
            v = 0.0f;
            u2 = 0.0f;
            v2 = 0.0f;
            x = i * XSTEP + XMIN;
            /* iterate the point */
            for (k = 1; (k < MAX_ITER) && ((u2 + v2) < 4.0f); ++k) {
                v = 2.0f * u * v + y;
                u = u2 - v2 + x;
                u2 = u * u;
                v2 = v * v;
            };
            color(k);
        }
        putchar('\n');
    }
}
