/* Atari Lynx version of samples/hello.c, using TGI instead of conio */

#include <tgi.h>


/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/

static const char Text[] = "Hello world!";


/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/

void main (void)
{
    unsigned int XMax, YMax;

    tgi_install (tgi_static_stddrv);
    tgi_init ();

    /* Set screen color. */
    tgi_setcolor (TGI_COLOR_WHITE);

    /* Clear the screen. */
    tgi_clear();

    /* Ask for the screen size. */
    XMax = tgi_getmaxx ();
    YMax = tgi_getmaxy ();

    /* Draw a frame around the screen. */
    tgi_line (0, 0, XMax, 0);
    tgi_lineto (XMax, YMax);
    tgi_lineto (0, YMax);
    tgi_lineto (0, 0);

    /* Write the greeting in the middle of the screen. */
    tgi_outtextxy ((tgi_getxres () - tgi_gettextwidth (Text)) / 2,
                   (tgi_getyres () - tgi_gettextheight (Text)) / 2, Text);
}
