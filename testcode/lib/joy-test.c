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

#if DYN_DRV
    unsigned char Res = joy_load_driver (joy_stddrv);
#elif defined(JOYSTICK_DRIVER)
    unsigned char Res = joy_install (&JOYSTICK_DRIVER);
#else
    unsigned char Res = joy_install (&joy_static_stddrv);
#endif

    if (Res != JOY_ERR_OK) {
        cprintf ("Error in joy_load_driver: %u\r\n", Res);
#if DYN_DRV
        cprintf ("os: %u, %s\r\n", _oserror, _stroserror (_oserror));
#endif
        exit (EXIT_FAILURE);
    }

    clrscr ();
    count = joy_count ();
#ifdef __ATARI5200__
    cprintf ("JOYSTICKS: %d", count);
#else
    cprintf ("Driver supports %d joystick(s)", count);
#endif
    while (1) {
        for (i = 0; i < count; ++i) {
            gotoxy (0, i+1);
            j = joy_read (i);
#ifdef __ATARI5200__
            cprintf ("%1d:%-3s%-3s%-3s%-3s%-3s%-3s",
                     i,
                     (j & joy_masks[JOY_UP])?    " U " : " u ",
                     (j & joy_masks[JOY_DOWN])?  " D " : " d ",
                     (j & joy_masks[JOY_LEFT])?  " L " : " l ",
                     (j & joy_masks[JOY_RIGHT])? " R " : " r ",
                     (j & joy_masks[JOY_FIRE])?  " 1 " : "   ",
                     (j & joy_masks[JOY_FIRE2])? " 2 " : "   ");
#else
            cprintf ("%2d: %-6s%-6s%-6s%-6s%-6s%-6s",
                     i,
                     (j & joy_masks[JOY_UP])?    "  up  " : " ---- ",
                     (j & joy_masks[JOY_DOWN])?  " down " : " ---- ",
                     (j & joy_masks[JOY_LEFT])?  " left " : " ---- ",
                     (j & joy_masks[JOY_RIGHT])? "right " : " ---- ",
                     (j & joy_masks[JOY_FIRE])?  " fire " : " ---- ",
                     (j & joy_masks[JOY_FIRE2])? "fire2 " : " ---- ");
#endif
        }
    }
    return 0;
}
