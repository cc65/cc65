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
** Link existing drivers this way:
**
** cl65 -DJOYSTICK_DRIVER=c64_hitjoy_joy -o joy-test.prg joy-test.c
**
** For testing a new driver, you need to uncomment the declaration below,
** and link your driver this way:
**
** co65 ../../target/c64/drv/joy/c64-hitjoy.joy -o hitjoy.s --code-label _hitjoy
** cl65 -DJOYSTICK_DRIVER=hitjoy -o joy-test.prg joy-test.c hitjoy.s
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
    unsigned char i, count;
    unsigned char Res;

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
        cprintf ("OS: %u, %s\r\n", _oserror, _stroserror (_oserror));
#endif
        return EXIT_FAILURE;
    }

    count = joy_count ();
#if defined(__ATARI5200__) || defined(__CREATIVISION__)
    cprintf ("JOYSTICKS: %u.", count);
#else
    cprintf ("Driver supports %u joystick%s", count, count == 1 ? "." : "s.");
#endif
    while (1) {
        for (i = 0; i < count; ++i) {
            j = joy_read (i);
#if defined(__NES__) || defined(__CX16__)
            /* two lines for each device */
            gotoxy (0, i * 2 +1);
            cprintf ("%2u:%-6s%-6s%-6s%-6s\r\n"
                     "   %-6s%-6s%-6s%-6s $%02X",
                     i,
                     JOY_UP(j)    ? "  up  " : " ---- ",
                     JOY_DOWN(j)  ? " down " : " ---- ",
                     JOY_LEFT(j)  ? " left " : " ---- ",
                     JOY_RIGHT(j) ? " right" : " ---- ",
                     JOY_BTN_1(j) ? "btn A " : " ---- ",
                     JOY_BTN_2(j) ? "btn B " : " ---- ",
                     JOY_BTN_3(j) ? "select" : " ---- ",
                     JOY_BTN_4(j) ? " start" : " ---- ",
                     j);
#else
            /* one line for each device */
            gotoxy (0, i + 1);
#  if defined(__ATARI5200__) || defined(__CREATIVISION__)
            cprintf ("%1u:%-3s%-3s%-3s%-3s%-3s %02X",
                     i,
                     JOY_UP(j)    ? " U " : " - ",
                     JOY_DOWN(j)  ? " D " : " - ",
                     JOY_LEFT(j)  ? " L " : " - ",
                     JOY_RIGHT(j) ? " R " : " - ",
                     JOY_BTN_1(j) ? " 1 " : " - ",
                     j);
#  else
            cprintf ("%2u: %-6s%-6s%-6s%-6s%-6s $%02X",
                     i,
                     JOY_UP(j)    ? "  up  " : " ---- ",
                     JOY_DOWN(j)  ? " down " : " ---- ",
                     JOY_LEFT(j)  ? " left " : " ---- ",
                     JOY_RIGHT(j) ? "right " : " ---- ",
                     JOY_BTN_1(j) ? "button" : " ---- ",
                     j);
#  endif
#endif
        }

        /* Show any pressed keys; so that we can verify that the keyboard is working. */
        if (kbhit ()) {
#if defined(__NES__) || defined(__CX16__)
            gotoxy (1, i * 2 + 2);
#else
            gotoxy (1, i + 2);
#endif
            cprintf ("keyboard: $%02X", cgetc ());
        }
    }
    return EXIT_SUCCESS;
}
