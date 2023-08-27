#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <cc65.h>
#include <conio.h>
#include <ctype.h>
#include <modload.h>
#include <tgi.h>



#ifndef DYN_DRV
#  define DYN_DRV       1
#endif

#define COLOR_BACK      TGI_COLOR_BLACK
#define COLOR_FORE      TGI_COLOR_WHITE


/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Driver stuff */
static unsigned MaxX;
static unsigned MaxY;
static unsigned AspectRatio;



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/

static void CheckError (const char* S)
{
    unsigned char Error = tgi_geterror ();

    if (Error != TGI_ERR_OK) {
        printf ("%s: %u\n", S, Error);
        if (doesclrscrafterexit ()) {
            cgetc ();
        }
        exit (EXIT_FAILURE);
    }
}

#if DYN_DRV
static void DoWarning (void)
/* Warn the user that the dynamic TGI driver is needed for this program */
{
    printf ("Warning: This program needs the TGI\n"
            "driver on disk! Press 'y' if you have\n"
            "it - any other key exits.\n");
    if (tolower (cgetc ()) != 'y') {
        exit (EXIT_SUCCESS);
    }
    printf ("OK. Please wait patiently...\n");
}
#endif

static const unsigned char Palette[2] = { TGI_COLOR_WHITE, TGI_COLOR_BLUE };

#if 1
int XRes, YRes;
float xf, xfMin, xfMax, xfDelta, yf, yfMin, yfMax, yfDelta;
float radiusf, zf, zScale;
int x,y;
//  unsigned char YMax;
int YMax;
unsigned int XResHalf;
float yfMinSquare;

float yfD;
float a;
float b;
float yfSquare;

int xoff;
int yoff;

void Dosincos(void) {

    tgi_setpalette (Palette);
    tgi_setcolor (COLOR_FORE);
    tgi_clear ();

  /* Get stuff from the graph lib */
  XRes = tgi_getmaxx() + 1;
//  XRes = 320;
  YRes = tgi_getmaxy() + 1;
//  YRes = 200;
  YMax = YRes - 1;



  /* Compute and draw a 3d function. */
  yfMin = -144.0f;
  yfMax = 144.0f;

  yfDelta = 2.25f;
  zScale = -10.0f;

  XResHalf = XRes * 0.5f;
  yfMinSquare = yfMin * yfMin;

  yoff = -80;
  xoff = +65;

//  for( yf = yfMin; yf < yfMax; yf += yfDelta) {   // FIXME
//  for( yf = yfMin; yf < yfMax; yf = yf + yfDelta) { // FIXME
      for( yf = yfMin; yf < yfMax; yf = yfDelta + yf ) { // FIXME
#if 0
    float yfD;
    float a;
    float b;
    float yfSquare;
#endif
    yoff++;
    xoff--;
//    xfMax = sqrtf( yfMinSquare - yf  * yf);
    xfMax = sqrtf( yfMinSquare - (yf  * yf));
    xfMin = -xfMax; // trigger fnegeax
    xfDelta = 1.0f;

    // Constant terms from the inner loop
    yfD = yf / yfDelta;
    a = XResHalf + yfD;
//    b=  YMax + yfD - 90;
    b=  YMax + yfD - 90.0f;

    yfSquare = yf * yf;

//    for( xf = xfMin; xf < xfMax; xf += xfDelta) {
    for( xf = xfMin; xf < xfMax; xf = xf + xfDelta) {

      radiusf = .0327f * sqrtf( (xf * xf) + yfSquare);

      zf = zScale * (cosf(7.7f * radiusf) +
                     cosf(8.5f * radiusf) +
                     cosf(9.3f * radiusf));


      /* Scale to screen coordinates */
      x = 1.0f * ( xf + a);
      x += xoff;

      y = 0.6f * ( b - zf);
      y += yoff;

      if( y > 0 && y < YRes) {

        tgi_setpixel( x, y);

        /* Clear horizon under y. */
        if( y < YMax) {
            tgi_setcolor (COLOR_BACK);
            tgi_line( x, y + 1, x, YMax);
            tgi_setcolor (COLOR_FORE);
        }
      }
    }
  }
    cgetc();
}
#endif

/*

5   dim xp, yp, r, fl, xr, yr, x, y, f
6   dim ad, yq, xl, q
7   m7 = 504
    e4 = 40
8   yl = 200
    n1 = -1
    n5 = 0.5
    nh = 900
    eh = 100
    hs = 160
    sf = 3136
    vz = 196
    s = 1
    a = 7872
10  dim bi(7), mi(319), ma(319), l(3000), p(30,120)

11  for t = 0 to 7
        bi(7-t) = s
        s = s + s
    next                                        : rem bit table
13  for t = 0 to yl
        if (7 and t) = 0 then a = a + 320
14      l(t) = a
    next                                        : rem line addr table
15  for x = 0 to 319
        mi(x) = yl
        ma(x) = n1
    next                                        : rem  min/max buffer

19  for y = 30 to 0 step n1
        yq = y * y
        xp = int(4 * sqr(nh - yq) + n5)
20      for x= 0 to xp
                r = sqr(x * x / sf + yq /vz)
21              f = cos(r) - cos(3 * r) / 3 + cos(5 * r) / 5 - cos(7 * r) / 7
                p(y, x) = int(e4 * f + n5)
22              fl = 0
                xp = hs + x - y
                yp = eh + y + y - p(y, x)
23              if mi(xp) > yp then mi(xp) = yp: fl = 1
24              if ma(xp) < yp then ma(xp) = yp: fl = 1
25              if fl = 0 or yp < z or yp >= yl then 27
26                  ad = l(yp) + (xp and m7) + (yp and 7)
                    poke ad, peek(ad) or bi(xp and 7)
27              fl = 0
                xp = hs - x - y
28              if mi(xp) > yp then mi(xp) = yp: fl = 1
29              if ma(xp) < yp then ma(xp) = yp: fl = 1
30              if fl = 0 or yp < z or yp >= yl then 32
31                  ad = l(yp) + (xp and m7) + (yp and 7)
                    poke ad, peek(ad) or bi(xp and 7)
32      next
    next

33  for y = -1 to -30 step n1
        yq = y * y
        xp = int(4 * sqr(nh - yq) + n5)
35      for x = 0 to xp
36          fl = 0
            xp = hs + x - y
            yp = eh + y + y - p(-y,x)
37          if mi(xp) > yp then mi(xp) = yp: fl = 1
38          if ma(xp) < yp then ma(xp) = yp: fl = 1
39          if fl = 0 or yp < z or yp >= yl then 41
40              ad = l(yp) + (xp and m7) + (yp and 7)
                poke ad, peek(ad) or bi(xp and 7)
41          fl = 0
            xp = hs - x - y
42          if mi(xp) > yp then mi(xp) = yp: fl = 1
43          if ma(xp) < yp then ma(xp) = yp: fl = 1
44          if fl = 0 or yp < z or yp >= yl then 46
45              ad = l(yp) + (xp and m7) + (yp and 7)
            poke ad, peek(ad) or bi(xp and 7)
46      next
    next
*/

int mi[320];
int ma[320];

int x, y;
int xp, yp;
int yq;
float r;
float f;
int p[30][120];
unsigned char fl;

void DoHat(void) {

    tgi_setpalette (Palette);
    tgi_setcolor (COLOR_FORE);
    tgi_clear ();

    for (x = 0; x < 320; x++) {
        mi[x] = 200;
        ma[x] = -1;
    }

    for (y = 30; y >= 0; y--) {
        yq = y * y;
//        xp = (int) (4.0f * sqrtf(900.0f - yq) + 0.5f);    // Error: Invalid operands for binary operator '-'
        xp = (int) (4.0f * sqrtf((float)(900 - yq)) + 0.5f);

        for (x = 0; x < xp; x++) {
            r = sqrtf((float)x * (float)x / 3136.0f + (float)yq / 196.0f);
            f = cosf(r) - cosf(3.0f * r) / 3.0f + cosf(5.0f * r) / 5.0f - cosf(7.0f * r) / 7.0f;
            p[y][x] = (int)((40.0f * f) + 0.5f);
            fl = 0;
            xp = 160 + x - y;
            yp = 100 + y + y - p[y][x];
            if (mi[xp] > yp) { mi[xp] = yp; fl = 1; }
            if (ma[xp] < yp) { ma[xp] = yp; fl = 1; }
            if (!((fl == 0) || (yp < 0) || (yp >= 200))) {
                tgi_setpixel( xp, yp);
            }
            fl = 0;
            xp = 160 - x - y;

            if (mi[xp] > yp) { mi[xp] = yp; fl = 1; }
            if (ma[xp] < yp) { ma[xp] = yp; fl = 1; }
            if (!((fl == 0) || (yp < 0) || (yp >= 200))) {
                tgi_setpixel( xp, yp);
            }
        }
    }
    cgetc();
}

int main (void)
{
    unsigned char Border;

#if DYN_DRV
    /* Warn the user that the tgi driver is needed */
    DoWarning ();

    /* Load and initialize the driver */
    tgi_load_driver (tgi_stddrv);
    CheckError ("tgi_load_driver");
#else
    /* Install the driver */
    tgi_install (tgi_static_stddrv);
    CheckError ("tgi_install");
#endif

#if 1
    tgi_init ();
    CheckError ("tgi_init");

    /* Get stuff from the driver */
    MaxX = tgi_getmaxx ();
    MaxY = tgi_getmaxy ();
    AspectRatio = tgi_getaspectratio ();

    /* Set the palette, set the border color */
    Border = bordercolor (COLOR_BLACK);
#endif
    /* Do graphics stuff */
    Dosincos();
//    DoHat();

#if DYN_DRV
    /* Unload the driver */
    tgi_unload ();
#else
    /* Uninstall the driver */
    tgi_uninstall ();
#endif

    /* Reset the border */
    (void) bordercolor (Border);

    /* Done */
    printf ("Done\n");
    return EXIT_SUCCESS;
}
