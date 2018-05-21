#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <conio.h>
#include <joystick.h>

#ifdef JOYSTICK_DRIVER

/* A statically linked driver was named on the compiler's command line.
** Make sure that it is used instead of a dynamic one.
*/
#  undef DYN_DRV
#  define DYN_DRV       0

/*
** link existing drivers like this:
**
** cl65 -DJOYSTICK_DRIVER=c64_hitjoy_joy -o joy-test.prg joy-test.c
**
** for testing a new driver you will have to uncomment the define below, and
** link your driver like this:
**
** co65 ../../target/c64/drv/joy/c64-hitjoy.joy -o hitjoy.s --code-label _hitjoy
** cl65 -DJOYSTICK_DRIVER=hitjoy -o joy-test.prg joy-test.c hitjoy.s
**
*/

/* extern char JOYSTICK_DRIVER; */

#else

/* Use a dynamically loaded driver, by default. */
#  ifndef DYN_DRV
#    define DYN_DRV     1
#  endif
#endif


int main (void)
{
    unsigned char j;
    unsigned char count;
    unsigned char i;
    unsigned char Res;
    unsigned char ch, kb;

    clrscr ();

#if DYN_DRV
    Res = joy_load_driver (joy_stddrv);
#elif defined(JOYSTICK_DRIVER)
    Res = joy_install (&JOYSTICK_DRIVER);
#else
    Res = joy_install (&joy_static_stddrv);
#endif

    if (Res != JOY_ERR_OK) {
        cprintf ("Error in joy_load_driver: %u\r\n", Res);
#if DYN_DRV
        cprintf ("os: %u, %s\r\n", _oserror, _stroserror (_oserror));
#endif
        exit (EXIT_FAILURE);
    }

    count = joy_count ();
#if defined(__ATARI5200__) || defined(__CREATIVISION__)
    cprintf ("JOYSTICKS: %d", count);
#else
    cprintf ("Driver supports %d joystick(s)", count);
#endif
    while (1) {
        for (i = 0; i < count; ++i) {
            gotoxy (0, i+1);
            j = joy_read (i);
#if defined(__ATARI5200__) || defined(__CREATIVISION__)
            cprintf ("%1d:%-3s%-3s%-3s%-3s%-3s %02x",
                     i,
                     JOY_UP(j)?    " U " : " - ",
                     JOY_DOWN(j)?  " D " : " - ",
                     JOY_LEFT(j)?  " L " : " - ",
                     JOY_RIGHT(j)? " R " : " - ",
                     JOY_BTN_1(j)? " 1 " : " - ", j);
#else
            cprintf ("%2d: %-6s%-6s%-6s%-6s%-6s %02x",
                     i,
                     JOY_UP(j)?    "  up  " : " ---- ",
                     JOY_DOWN(j)?  " down " : " ---- ",
                     JOY_LEFT(j)?  " left " : " ---- ",
                     JOY_RIGHT(j)? "right " : " ---- ",
                     JOY_BTN_1(j)? "button" : " ---- ", j);
#endif
        }

        /* show pressed key, so we can verify keyboard is working */
        kb = kbhit ();
        ch = kb ? cgetc () : ' ';
        gotoxy (1, i+2);
        revers (kb);
        cprintf ("kbd: %c", ch);
        revers (0);
    }
    return 0;
}
