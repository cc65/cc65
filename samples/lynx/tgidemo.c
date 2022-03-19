/* Tgidemo modified for the Atari Lynx.
**
** Press any of the Lynx's option buttons to go to the next screen.
*/

#include <cc65.h>
#include <conio.h>
#include <tgi.h>
#include <time.h>


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



/* The Lynx draws too fast.  This function delays
** the drawing so that we can watch it.
*/
static void wait (unsigned char ticks)
{
    clock_t T = clock () + ticks;

    while (clock () < T) {}
}



static void DoCircles (void)
{
    unsigned char I;
    unsigned char Color = COLOR_BACK;
    const unsigned X = MaxX / 2;
    const unsigned Y = MaxY / 2;
    const unsigned Limit = (X < Y) ? Y : X;

    tgi_setcolor (COLOR_FORE);
    tgi_clear ();

    tgi_line (0, 0, MaxX, MaxY);
    tgi_line (0, MaxY, MaxX, 0);
    while (!kbhit ()) {
        Color = (Color == COLOR_FORE) ? COLOR_BACK : COLOR_FORE;
        tgi_setcolor (Color);
        for (I = 10; I <= Limit; I += 10) {
            tgi_ellipse (X, Y, I, tgi_imulround (I, AspectRatio));
            wait (9);
        }
    }

    cgetc ();
}



static void DoCheckerboard (void)
{
    unsigned X, Y;
    unsigned char Color = COLOR_BACK;

    tgi_clear ();

    while (1) {
        for (Y = 0; Y <= MaxY - 2; Y += 10) {
            for (X = 0; X <= MaxX; X += 10) {
                Color = (Color == COLOR_FORE) ? COLOR_BACK : COLOR_FORE;
                tgi_setcolor (Color);
                tgi_bar (X, Y, X+9, Y+9);
                if (kbhit ()) {
                    cgetc ();
                    return;
                }
                wait (1);
            }
            Color = Color == COLOR_FORE ? COLOR_BACK : COLOR_FORE;
        }
        Color = Color == COLOR_FORE ? COLOR_BACK : COLOR_FORE;
    }
}



static void DoDiagram (void)
{
    int XOrigin, YOrigin;
    int Amp;
    int X, Y;
    unsigned I;

    tgi_setcolor (COLOR_FORE);
    tgi_clear ();

    /* Determine zero and amplitude */
    YOrigin = MaxY / 2;
    XOrigin = 10;
    Amp     = (MaxY - 19) / 2;

    /* Y axis */
    tgi_line (XOrigin, 10, XOrigin, MaxY-10);
    tgi_line (XOrigin-2, 12, XOrigin, 10);
    tgi_lineto (XOrigin+2, 12);

    /* X axis */
    tgi_line (XOrigin, YOrigin, MaxX-10, YOrigin);
    tgi_line (MaxX-12, YOrigin-2, MaxX-10, YOrigin);
    tgi_lineto (MaxX-12, YOrigin+2);

    /* Sine */
    tgi_gotoxy (XOrigin, YOrigin);
    for (I = 0; I <= 360; ++I) {
        /* Calculate the next points */
        X = (int)(((long)(MaxX - 19) * I) / 360);
        Y = (int)(((long)Amp * -_sin (I)) / 256);

        /* Draw the line */
        tgi_lineto (XOrigin + X, YOrigin + Y);
    }

    cgetc ();
}



static void DoLines (void)
{
    unsigned X;
    const unsigned Min = (MaxX < MaxY) ? MaxX : MaxY;

    tgi_setcolor (COLOR_FORE);
    tgi_clear ();

    for (X = 0; X <= Min; X += 10) {
        tgi_line (0, 0, Min, X);
        tgi_line (0, 0, X, Min);
        tgi_line (Min, Min, 0, Min-X);
        tgi_line (Min, Min, Min-X, 0);
        wait (9);
    }

    cgetc ();
}



void main (void)
{
    /* Install the driver */
    tgi_install (tgi_static_stddrv);
    tgi_init ();

    /* Get stuff from the driver */
    MaxX = tgi_getmaxx ();
    MaxY = tgi_getmaxy ();
    AspectRatio = tgi_getaspectratio ();

    /* Do graphics stuff */
    DoCircles ();
    DoCheckerboard ();
    DoDiagram ();
    DoLines ();
}
