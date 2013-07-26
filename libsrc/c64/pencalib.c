/*
** Calibrate lightpen drivers to the current video hardware.
**
** 2013-07-25, Greg King
**
*/


#include <conio.h>
#include <mouse.h>
#include <pen.h>


#define COMMAND1 "Adjust by clicking on line."
#define COMMAND2 "Finish by clicking off bar."


/*
** There is a delay between when the VIC sends its signal, and when the display
** shows that signal.  There is another delay between the display and when
** the lightpen says that it saw that signal. Each display and pen is different.
** Therefore, the driver must be calibrated to them.  A white bar is painted on
** the screen; and, a line is drawn down the middle of it.  When the user clicks
** on that line, the difference between its position and where the VIC thinks
** that the pen is pointing becomes an offset that is subtracted from what the
** VIC sees.
*/
void __fastcall__ pen_calibrate (unsigned char *XOffset)
{
    unsigned char oldBg = bgcolor (COLOR_BLUE);
    unsigned char oldText = textcolor (COLOR_GRAY3);
    unsigned char oldRev = revers (1);
    unsigned char sprite0Color = VIC.spr_color[0];
    unsigned char width, width2, height, height4, height8;
    struct mouse_info info;

    screensize (&width, &height);
    width2 = width / 2;
    height4 = height / 4;
    height8 = height4 * 8;

    /* Draw a bar and line. */

    clrscr ();
    cclearxy (0, height4, height4 * width);
    cvlinexy (width2, height4 + 1, height4 - 2);
    revers (0);

    /* Print instructions. */

    cputsxy (width2 - (sizeof COMMAND1) / 2, height / 2 + 1, COMMAND1);
    cputsxy (width2 - (sizeof COMMAND2) / 2, height / 2 + 3, COMMAND2);

    VIC.spr_color[0] = COLOR_GRAY2;
    mouse_show ();
    mouse_move (width2 * 8, height8 / 2);

    for (;;) {
        /* Wait for the main button to be released. */

        do ; while ((mouse_buttons () & MOUSE_BTN_LEFT));

        /* Wait for the main button to be pressed. */

        do {
            mouse_info (&info);
        } while (!(info.buttons & MOUSE_BTN_LEFT));

        /* Find out if the pen is on or off the bar. */

        if (info.pos.y < height8 || info.pos.y >= height8 * 2) {
            break;
        }

        /* On the bar; adjust the offset. */
        /* Characters are eight pixels wide.
        ** The VIC-II sees every other pixel;
        ** so, we use half of the difference.
        */

        *XOffset += (info.pos.x - (width2 * 8 + 8/2)) / 2;
    }

    /* Off the bar; wait for the main button to be released. */

    do ; while ((mouse_buttons () & MOUSE_BTN_LEFT));

    mouse_hide ();
    VIC.spr_color[0] = sprite0Color;
    revers (oldRev);
    textcolor (oldText);
    bgcolor (oldBg);
    clrscr ();
}
