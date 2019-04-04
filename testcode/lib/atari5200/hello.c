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



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



int main (void)
{
    unsigned char XSize, YSize;
    unsigned char PosY;

    /* Set screen colors */
    (void) textcolor (COLOR_WHITE);
    (void) bordercolor (COLOR_BLACK);
    (void) bgcolor (COLOR_BLACK);

    /* Clear the screen, put cursor in upper left corner */
    clrscr ();

    /* Ask for the screen size */
    screensize (&XSize, &YSize);

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
    textcolor (0); /* switch to color #0 */
    cputsxy(3, ++PosY, "COLOR 0");
    textcolor (1); /* switch to color #1 */
    cputsxy(3, ++PosY, "COLOR 1");
    textcolor (2); /* switch to color #2 */
    cputsxy(3, ++PosY, "COLOR 2");
    textcolor (3); /* switch to color #3 */ /* color #3 is the background color. So written text isn't visible. */
    cputsxy(3, ++PosY, "COLOR 3");

#if defined(__NES__) || defined(__PCE__) || defined(__GAMATE__) || defined(__ATARI5200__)

    /* Wait for the user to press a button */
    joy_install (joy_static_stddrv);
    while (!joy_read (JOY_1)) ;
    joy_uninstall ();

#else

    /* Wait for the user to press a key */
    cgetc ();

#endif

    /* Clear the screen again */
    clrscr ();

    /* Done */
    return EXIT_SUCCESS;
}
