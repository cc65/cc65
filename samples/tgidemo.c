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
        printf ("%s: %d\n", S, Error);
        exit (EXIT_FAILURE);
    }
}



static void DoWarning (void)
/* Warn the user that the TGI driver is needed for this program */
{
    printf ("Warning: This program needs the TGI\n"
            "driver on disk! Press 'y' if you have\n"
            "it - any other key exits.\n");
    if (tolower (cgetc ()) != 'y') {
        exit (EXIT_SUCCESS);
    }
    printf ("Ok. Please wait patiently...\n");
}



static void DoCircles (void)
{
    static const unsigned char Palette[2] = { TGI_COLOR_WHITE, TGI_COLOR_ORANGE };
    unsigned char I;
    unsigned char Color = COLOR_FORE;
    unsigned X = MaxX / 2;
    unsigned Y = MaxY / 2;

    tgi_setpalette (Palette);
    while (!kbhit ()) {
        tgi_setcolor (COLOR_FORE);
        tgi_line (0, 0, MaxX, MaxY);
        tgi_line (0, MaxY, MaxX, 0);
        tgi_setcolor (Color);
        for (I = 10; I < 240; I += 10) {
            tgi_ellipse (X, Y, I, tgi_imulround (I, AspectRatio));
        }
        Color = Color == COLOR_FORE ? COLOR_BACK : COLOR_FORE;
    }

    cgetc ();
    tgi_clear ();
}



static void DoCheckerboard (void)
{
    static const unsigned char Palette[2] = { TGI_COLOR_WHITE, TGI_COLOR_BLACK };
    unsigned X, Y;
    unsigned char Color;

    tgi_setpalette (Palette);
    Color = COLOR_BACK;
    while (1) {
        for (Y = 0; Y <= MaxY; Y += 10) {
            for (X = 0; X <= MaxX; X += 10) {
                tgi_setcolor (Color);
                tgi_bar (X, Y, X+9, Y+9);
                Color = Color == COLOR_FORE ? COLOR_BACK : COLOR_FORE;
                if (kbhit ()) {
                    cgetc ();
                    tgi_clear ();
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

    /* Determine zero and aplitude */
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
        Y = (int) (((long) Amp * -cc65_sin (I)) / 256);

        /* Draw the line */
        tgi_lineto (XOrigin + X, YOrigin + Y);
    }

    cgetc ();
    tgi_clear ();
}



static void DoLines (void)
{
    static const unsigned char Palette[2] = { TGI_COLOR_WHITE, TGI_COLOR_BLACK };
    unsigned X;

    tgi_setpalette (Palette);
    tgi_setcolor (COLOR_FORE);

    for (X = 0; X <= MaxY; X += 10) {
        tgi_line (0, 0, MaxY, X);
        tgi_line (0, 0, X, MaxY);
        tgi_line (MaxY, MaxY, 0, MaxY-X);
        tgi_line (MaxY, MaxY, MaxY-X, 0);
    }

    cgetc ();
    tgi_clear ();
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
    tgi_clear ();

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
