#include <stdio.h>
#include <stdlib.h>
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



static void DoCircles (void)
{
    static const unsigned char Palette[2] = { TGI_COLOR_WHITE, TGI_COLOR_BLUE };
    unsigned char I;
    unsigned char Color = COLOR_BACK;
    const unsigned X = MaxX / 2;
    const unsigned Y = MaxY / 2;
    const unsigned Limit = (X < Y) ? Y : X;

    tgi_setpalette (Palette);
    tgi_setcolor (COLOR_FORE);
    tgi_clear ();
    tgi_line (0, 0, MaxX, MaxY);
    tgi_line (0, MaxY, MaxX, 0);
    while (!kbhit ()) {
        Color = (Color == COLOR_FORE) ? COLOR_BACK : COLOR_FORE;
        tgi_setcolor (Color);
        for (I = 10; I <= Limit; I += 10) {
            tgi_ellipse (X, Y, I, tgi_imulround (I, AspectRatio));
        }
    }

    cgetc ();
}



static void DoCheckerboard (void)
{
    static const unsigned char Palette[2] = { TGI_COLOR_WHITE, TGI_COLOR_BLACK };
    unsigned X, Y;
    unsigned char Color = COLOR_BACK;

    tgi_setpalette (Palette);
    tgi_clear ();

    while (1) {
        for (Y = 0; Y <= MaxY; Y += 10) {
            for (X = 0; X <= MaxX; X += 10) {
                Color = (Color == COLOR_FORE) ? COLOR_BACK : COLOR_FORE;
                tgi_setcolor (Color);
                tgi_bar (X, Y, X+9, Y+9);
                if (kbhit ()) {
                    cgetc ();
                    return;
                }
            }
            Color = Color == COLOR_FORE ? COLOR_BACK : COLOR_FORE;
        }
        Color = Color == COLOR_FORE ? COLOR_BACK : COLOR_FORE;
    }
}



static void DoDiagram (void)
{
    static const unsigned char Palette[2] = { TGI_COLOR_WHITE, TGI_COLOR_BLACK };
    int XOrigin, YOrigin;
    int Amp;
    int X, Y;
    unsigned I;

    tgi_setpalette (Palette);
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
    for (I = 0; I <= 360; I += 5) {

        /* Calculate the next points */
        X = (int) (((long) (MaxX - 19) * I) / 360);
        Y = (int) (((long) Amp * -_sin (I)) / 256);

        /* Draw the line */
        tgi_lineto (XOrigin + X, YOrigin + Y);
    }

    cgetc ();
}



static void DoLines (void)
{
    static const unsigned char Palette[2] = { TGI_COLOR_WHITE, TGI_COLOR_BLACK };
    unsigned X;
    const unsigned Min = (MaxX < MaxY) ? MaxX : MaxY;

    tgi_setpalette (Palette);
    tgi_setcolor (COLOR_FORE);
    tgi_clear ();

    for (X = 0; X <= Min; X += 10) {
        tgi_line (0, 0, Min, X);
        tgi_line (0, 0, X, Min);
        tgi_line (Min, Min, 0, Min-X);
        tgi_line (Min, Min, Min-X, 0);
    }

    cgetc ();
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

    tgi_init ();
    CheckError ("tgi_init");

    /* Get stuff from the driver */
    MaxX = tgi_getmaxx ();
    MaxY = tgi_getmaxy ();
    AspectRatio = tgi_getaspectratio ();

    /* Set the palette, set the border color */
    Border = bordercolor (COLOR_BLACK);

    /* Do graphics stuff */
    DoCircles ();
    DoCheckerboard ();
    DoDiagram ();
    DoLines ();

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
