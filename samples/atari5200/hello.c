/*
** Fancy hello world program using cc65.
**
** Ullrich von Bassewitz (ullrich@von-bassewitz.de)
**
** TEST version for atari5200 conio, using all four colors
*/



#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <joystick.h>



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



static const char Text [] = "Hello world!";
static unsigned char colors[] = { COLOR_WHITE, COLOR_GREEN, COLOR_RED, COLOR_BLACK };



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



int main (void)
{
    unsigned char XSize, YSize;
    unsigned char PosY;
    unsigned char i = 0;

    /* Set screen colors */
    (void) textcolor (COLOR_WHITE);
    (void) bordercolor (COLOR_BLACK);
    (void) bgcolor (COLOR_BLACK);

    /* Clear the screen, put cursor in upper left corner */
    clrscr ();

    /* Ask for the screen size */
    screensize (&XSize, &YSize);

    /* Install joystick driver */
    joy_install (joy_static_stddrv);

    while (1) {
        /* Draw a border around the screen */

        /* Top line */
        cputc (CH_ULCORNER);
        chline (XSize - 2);
        cputc (CH_URCORNER);

        /* Vertical line, left side */
        cvlinexy (0, 1, YSize - 2);

        /* Bottom line */
        cputc (CH_LLCORNER);
        chline (XSize - 2);
        cputc (CH_LRCORNER);

        /* Vertical line, right side */
        cvlinexy (XSize - 1, 1, YSize - 2);

        /* Write the greeting in the mid of the screen */
        gotoxy ((XSize - strlen (Text)) / 2, YSize / 2);
        cprintf ("%s", Text);

        PosY = wherey ();
        textcolor (colors[i]); /* switch to color #0 */
        cputsxy(3, ++PosY, "COLOR 0");
        textcolor ((colors[i] + 1) & 3); /* switch to color #1 */
        cputsxy(3, ++PosY, "COLOR 1");
        textcolor ((colors[i] + 2) & 3); /* switch to color #2 */
        cputsxy(3, ++PosY, "COLOR 2");
        textcolor ((colors[i] + 3) & 3); /* switch to color #3 */ /* color #3 is the background color. So written text isn't visible. */
        cputsxy(3, ++PosY, "COLOR 3");

        /* Wait for the user to press and release a button */
        while (!joy_read (JOY_1))
            ;
        while (joy_read (JOY_1))
            ;

        i = (i + 1) & 3;

        /* Change colors */
        textcolor (colors[i]);
        bgcolor ((colors[i] + 3) & 3);

        /* Clear the screen again */
        clrscr ();
    }
    /* not reached */

    joy_uninstall ();

    /* Done */
    return EXIT_SUCCESS;
}
