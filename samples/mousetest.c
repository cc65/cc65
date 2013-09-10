/*
** Test/demo program for mouse usage.
** Will work for the C64/C128/CBM510/Atari/Apple2.
**
** 2001-09-13, Ullrich von Bassewitz
** 2013-09-05, Greg King
**
*/



#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <mouse.h>
#include <pen.h>
#include <conio.h>
#include <ctype.h>
#include <dbg.h>

#define max(a,b)  (((a) > (b)) ? (a) : (b))
#define min(a,b)  (((a) < (b)) ? (a) : (b))



#ifdef MOUSE_DRIVER

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



#if defined(__C64__) || defined(__C128__) || defined(__CBM510__)

/* Addresses of data for sprite 0 */
#if defined(__C64__)
#  define SPRITE0_DATA  ((unsigned char[64])0x0340)
#  define SPRITE0_PTR   ((unsigned char *)0x07F8)
#elif defined(__C128__)
#  define SPRITE0_DATA  ((unsigned char[64])0x0E00)
#  define SPRITE0_PTR   ((unsigned char *)0x07F8)
#elif defined(__CBM510__)
#  define SPRITE0_DATA  ((unsigned char[64])0xF400)
#  define SPRITE0_PTR   ((unsigned char *)0xF3F8)
#endif

/* The mouse sprite (an arrow) */
static const unsigned char MouseSprite[64] = {
    0xFE, 0x00, 0x00,
    0xFC, 0x00, 0x00,
    0xF8, 0x00, 0x00,
    0xFC, 0x00, 0x00,
    0xDE, 0x00, 0x00,
    0x8F, 0x00, 0x00,
    0x07, 0x80, 0x00,
    0x03, 0xC0, 0x00,
    0x01, 0xE0, 0x00,
    0x00, 0xF0, 0x00,
    0x00, 0x78, 0x00,
    0x00, 0x38, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00
};
#endif



static void __fastcall__ CheckError (const char* S, unsigned char Error)
{
    if (Error != MOUSE_ERR_OK) {
        cprintf ("\n%s: %s(%u)\r\n", S, mouse_geterrormsg (Error), Error);

        /* Wait for a key-press, so that some platforms can show the error
        ** message before they remove the current screen.
        */
        cgetc();
        exit (EXIT_FAILURE);
    }
}



#if DYN_DRV

/* Points to the dynamic driver's name. */
static const char *mouse_name;



static void DoWarning (void)
/* Warn the user that a driver is needed for this program. */
{
    cprintf ("Warning: This program needs\r\n"
             "the driver with the name\r\n"
             "    %s\r\n"
             "on a disk! Press 'y' if you have it;\r\n"
             "or, any other key to exit.\r\n", mouse_stddrv);
    if (tolower (cgetc ()) != 'y') {
        exit (EXIT_SUCCESS);
    }
    cprintf ("OK. Please wait patiently...\r\n");
}
#endif



static void __fastcall__ ShowState (unsigned char Jailed, unsigned char Invisible)
/* Display jail and cursor states. */
{
    cclearxy (0, 7, 32);
    gotoxy (0, 7);
    cprintf ("Pointer is %svisible%s.", Invisible? "in" : "", Jailed? " and jailed" : "");
}



#if DYN_DRV
int main (int argc, char *argv[])
#else
int main (void)
#endif
{
    struct mouse_info info;
    struct mouse_box full_box, small_box;
    unsigned char width, height;
    char C;
    bool Invisible = true, Done = false, Jailed = false;

    /* Initialize the debugger */
    DbgInit (0);

    /* Set dark-on-light colors.  Clear the screen. */
#ifdef __CBM__
    (void) bordercolor (COLOR_GRAY2);
    (void) bgcolor (COLOR_WHITE);
    (void) textcolor (COLOR_GRAY1);
#else
    (void) bordercolor (COLOR_BLUE);
    (void) bgcolor (COLOR_WHITE);
    (void) textcolor (COLOR_BLACK);
#endif
    cursor (0);
    clrscr ();

    /* The pointer should be created before the driver is installed,
    ** in case a lightpen driver needs it during calibration.
    */

#if defined(__C64__) || defined(__C128__) || defined(__CBM510__)
    /* Copy the sprite data */
    memcpy ((void*) SPRITE0_DATA, MouseSprite, sizeof (MouseSprite));

    /* Set the VIC-II sprite pointer. */
    *SPRITE0_PTR = ((unsigned) SPRITE0_DATA & 0x3FFF) / sizeof SPRITE0_DATA;

    /* Set the color of sprite 0 */
#  ifdef __CBM510__
    pokebsys ((unsigned) &VIC.spr0_color, COLOR_BLACK);
#  else
    VIC.spr0_color = COLOR_BLACK;
#  endif
#endif

    /* If a lightpen driver is installed, then it can get a calibration value
    ** from this file (if it exists).  Or, the user can adjust the pen; and,
    ** the value will be put into this file, for the next time.
    ** (Other drivers will ignore this.)
    */
#if defined(__C64__) || defined(__C128__) || defined(__CBM510__)
    pen_adjust ("pen.dat");
#endif

#if DYN_DRV
    /* If a dynamically loadable driver is named on the command line,
    ** then use that driver instead of the standard one.
    */
    if (argc > 1) {
        mouse_name = argv[1];
    } else {
        /* Output a warning about the standard driver that is needed. */
        DoWarning ();
        mouse_name = mouse_stddrv;
    }

    /* Load and install the driver. */
    CheckError ("mouse_load_driver",
                mouse_load_driver (&mouse_def_callbacks, mouse_name));
#else
    /* Install the driver. */
    CheckError ("mouse_install",
                mouse_install (&mouse_def_callbacks,
#  ifdef MOUSE_DRIVER
                               MOUSE_DRIVER
#  else
                               mouse_static_stddrv
#  endif
                               ));
#endif

    /* Get the initial bounding box. */
    mouse_getbox (&full_box);

    screensize (&width, &height);

top:
    clrscr ();

    /* Print a help line */
    cputs (" d)ebug  h)ide   q)uit   s)how   j)ail");

    /* Put a cross at the center of the screen. */
    gotoxy (width / 2 - 3, height / 2 - 1);
#if defined(__CBM__)
    cprintf ("%3u,%3u\r\n%*s\xDB", width / 2 * 8 + 4, height / 2 * 8 + 4,
             width / 2, "");
#else
    cprintf ("%3u,%3u\r\n%*s+", width / 2 * 8 + 4, height / 2 * 8 + 4,
             width / 2, "");
#endif

    /* Test loop */
    ShowState (Jailed, Invisible);
    do {
        /* Get the current co-ordinates and button states; and, print them. */
        mouse_info (&info);
        gotoxy (0, 2);
        cprintf (" X  = %3d\r\n", info.pos.x);
        cprintf (" Y  = %3d\r\n", info.pos.y);
        cprintf (" B1 = %c\r\n", (info.buttons & MOUSE_BTN_LEFT) ?
#ifdef __CBM__
                 0x5F
#else
                 'v'
#endif
                 : '^');
        cprintf (" B2 = %c", (info.buttons & MOUSE_BTN_RIGHT) ?
#ifdef __CBM__
                 0x5F
#else
                 'v'
#endif
                 : '^');

        /* Handle user input */
        if (kbhit ()) {
            cclearxy (1, 9, 23);
            switch (tolower (C = cgetc ())) {
                case 'd':
                    BREAK();

                    /* The debugger might have changed the colors.
                    ** Restore them.
                    */
#ifdef __CBM__
                    (void) bordercolor (COLOR_GRAY2);
                    (void) bgcolor (COLOR_WHITE);
                    (void) textcolor (COLOR_GRAY1);
#else
                    (void) bordercolor (COLOR_BLUE);
                    (void) bgcolor (COLOR_WHITE);
                    (void) textcolor (COLOR_BLACK);
#endif

                    /* The debugger changed the screen; restore it. */
                    goto top;

                case 'h':
                    mouse_hide ();
                    ShowState (Jailed, ++Invisible);
                    break;

                case 'j':
                    if (Jailed) {
                        mouse_setbox (&full_box);
                        Jailed = false;
                    } else {
                        small_box.minx = max (info.pos.x - 10, full_box.minx);
                        small_box.miny = max (info.pos.y - 10, full_box.miny);
                        small_box.maxx = min (info.pos.x + 10, full_box.maxx);
                        small_box.maxy = min (info.pos.y + 10, full_box.maxy);
                        mouse_setbox (&small_box);
                        Jailed = true;
                    }
                    ShowState (Jailed, Invisible);
                    break;

                case 's':
                    mouse_show ();
                    if (Invisible) {
                        ShowState (Jailed, --Invisible);
                    }
                    break;

                case 'q':
                    Done = true;
                    break;

                default:
                    gotoxy (1, 9);
                    cprintf ("Spurious character: $%02X", C);
            }
        }
    } while (!Done);

#if DYN_DRV
    /* Uninstall and unload the driver. */
    CheckError ("mouse_unload", mouse_unload ());
#else
    /* Uninstall the static driver. */
    CheckError ("mouse_uninstall", mouse_uninstall ());
#endif

    /* Say goodbye */
    cputsxy (0, height / 2 + 3, "Goodbye!");
    return EXIT_SUCCESS;
}
