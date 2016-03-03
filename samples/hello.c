/*
** Fancy hello world program using cc65.
**
** Ullrich von Bassewitz (ullrich@von-bassewitz.de)
**
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

    /* Set screen colors, hide the cursor */
    textcolor (COLOR_WHITE);
    bordercolor (COLOR_BLACK);
    bgcolor (COLOR_BLACK);
    cursor (0);

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

#if defined(__NES__) || defined(__PCE__) || defined(__GAMATE__)

    /* Wait for the user to press a button */
    joy_install (joy_static_stddrv);
    while (!joy_read (JOY_1)) ;
    joy_uninstall ();

#else

    /* Wait for the user to press a key */
    (void) cgetc ();

#endif

    /* Clear the screen again */
    clrscr ();

    /* Done */
    return EXIT_SUCCESS;
}
