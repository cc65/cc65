/*
  !!DESCRIPTION!! mandelbrot test program
  !!ORIGIN!!      testsuite
  !!LICENCE!!     Public Domain
  !!AUTHOR!!      Groepaz/Hitmen
*/

#include <stdio.h>
#include <stdlib.h>

static unsigned short SCREEN_X;
static unsigned char  SCREEN_Y;

#define MAXCOL     8

#define maxiterations 16

#define fpshift (12)

#define tofp(_x)        ((_x)<<fpshift)
#define fromfp(_x)      ((_x)>>fpshift)
#define fpabs(_x)       (abs(_x))

#define mulfp(_a,_b)    ((((signed long)_a)*(_b))>>fpshift)
#define divfp(_a,_b)    ((((signed long)_a)<<fpshift)/(_b))

unsigned char dither[MAXCOL]={" .*o+0%#"};

void mandelbrot(signed short x1,signed short y1,signed short x2,signed short y2)
{
register signed short  r,r1,i;
register unsigned char count;
register signed short xs,ys,xx,yy;
register signed short x;
register unsigned char y;

        /* calc stepwidth */
        xs=((x2-x1)/(SCREEN_X));
        ys=((y2-y1)/(SCREEN_Y));

        yy=y1;
        for(y = 0; y < (SCREEN_Y); ++y)
        {
                yy+=ys; xx=x1;
                for(x = 0; x < (SCREEN_X); ++x)
                {
                    xx+=xs;
                    /* do iterations */
                    r=0;i=0;
                    for(count=0;(count<maxiterations)&&
                                (fpabs(r)<tofp(2))&&
                                (fpabs(i)<tofp(2))
                                ;++count)
                    {
                            r1 = (mulfp(r,r)-mulfp(i,i))+xx;
                            /* i = (mulfp(mulfp(r,i),tofp(2)))+yy; */
                            i = (((signed long)r*i)>>(fpshift-1))+yy;
                            r=r1;
                    }
                    if(count==maxiterations)
                    {
                            printf(" ");
                    }
                    else
                    {
                            printf("%c",dither[(count%MAXCOL)]);
                    }
                }
                    printf("\n");
        }
}

int main (void)
{
    SCREEN_X = 80;
    SCREEN_Y = 40;

    /* calc mandelbrot set */
    mandelbrot(tofp(-2),tofp(-2),tofp(2),tofp(2));

    /* Done */
    return 0;
}

