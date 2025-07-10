
#include <stdlib.h>
#include <stdio.h>
#include <joystick.h>

/* define 0 to link the standard driver statically */
/* #define DYN_DRV         0 */

#ifndef DYN_DRV
#  define DYN_DRV       1
#endif

#define USECONIO

#ifdef USECONIO
#include <conio.h>
#define PRINTF  cprintf
#define CR "\n\r"
#else
#define PRINTF  printf
#define CR "\n"
#endif

int main (void)
{
    unsigned char num_joy;
    unsigned char raw_value;
    unsigned char i;
#if DYN_DRV
    unsigned char err;
#endif
#ifdef USECONIO
    unsigned char y;
    clrscr();
#endif
    PRINTF("Driver init..." CR);

#if DYN_DRV
    /* Load and initialize the standard driver driver */
    if ((err = joy_load_driver (joy_stddrv))) {
        PRINTF ("Driver load error (code %d)." CR
                "Warning: This program needs the JOY" CR
                "driver on disk!" CR, err);
        exit (EXIT_FAILURE);
    }
    PRINTF("Driver loaded OK" CR);
#else
    /* Install the standard driver */
    joy_install (joy_static_stddrv);
#endif

    num_joy = joy_count();

    PRINTF("Driver reported %d joysticks." CR "waiting for input..." CR, num_joy);

    /* wait for something to happen on any joystick input */
    {
        unsigned char wait = 1;
        while (wait) {
            for (i = 0; i < num_joy; ++i) {
                raw_value = joy_read(i);
                if (raw_value) {
                    wait = 0;
                    break;
                }
            }
        }
    }

    /* read all joysticks and print the raw value(s) */
#ifdef USECONIO
    y = wherey();
#endif
    while (1) {
#ifdef USECONIO
        gotoxy(0, y);
#endif
        for (i = 0; i < num_joy; ++i) {
            raw_value = joy_read(i);
            PRINTF("%02x ", raw_value);
        }
        PRINTF(CR);
    }

#if DYN_DRV
    /* Unload the driver */
    joy_unload ();
#else
    /* Uninstall the driver */
    joy_uninstall ();
#endif

    /* Done */
    PRINTF ("Done" CR);
    return EXIT_SUCCESS;
}
