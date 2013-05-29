/*
** Test program for lightpen drivers. Will work for the C64/C128.
**
** 2001-09-13, Ullrich von Bassewitz
** 2013-05-24, Greg King
**
*/



#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <mouse.h>
#include <conio.h>
#include <ctype.h>
#include <dbg.h>

#define max(a,b)  (((a) > (b)) ? (a) : (b))
#define min(a,b)  (((a) < (b)) ? (a) : (b))



/* Statically linked driver */
#define DYN_DRV         0



#ifdef __C64__
#  if DYN_DRV
#    define mouse_stddrv "c64-inkwell.mou"
#  else
#    define mouse_static_stddrv c64_inkwell_mou
#  endif
#endif

#ifdef __C128__
#  if DYN_DRV
#    define mouse_stddrv "c128-inkwell.mou"
#  else
#    define mouse_static_stddrv c128_inkwell_mou
#  endif
#endif



#if defined(__C64__) || defined(__C128__)

/* Addresses of data for sprite 0 */
#if defined(__C64__)
#  define SPRITE0_DATA  ((unsigned char[64])0x0340)
#  define SPRITE0_PTR   ((unsigned char *)0x07F8)
#elif defined(__C128__)
#  define SPRITE0_DATA  ((unsigned char[64])0x0E00)
#  define SPRITE0_PTR   ((unsigned char *)0x07F8)
#endif

/* The lightpen sprite (an arrow) */
static const unsigned char PenSprite[64] = {
    0xFF, 0xFF, 0xFF,
    0xFC, 0x00, 0x00,
    0xF8, 0x00, 0x00,
    0xFC, 0x00, 0x00,
    0xDE, 0x00, 0x00,
    0x8F, 0x00, 0x00,
    0x87, 0x80, 0x00,
    0x83, 0xC0, 0x00,
    0x81, 0xE0, 0x00,
    0x80, 0xF0, 0x00,
    0x80, 0x78, 0x00,
    0x80, 0x38, 0x00,
    0x80, 0x00, 0x00,
    0x80, 0x00, 0x00,
    0x80, 0x00, 0x00,
    0x80, 0x00, 0x00,
    0x80, 0x00, 0x00,
    0x80, 0x00, 0x00,
    0x80, 0x00, 0x00,
    0x80, 0x00, 0x00,
    0x80, 0x00, 0x00
};

#endif  /* __C64__ or __C128__ */



static void __fastcall__ CheckError (const char* S, unsigned char Error)
{
    if (Error != MOUSE_ERR_OK) {
        cprintf ("%s: %s(%u)\r\n", S, mouse_geterrormsg (Error), Error);
        exit (EXIT_FAILURE);
    }
}

#if DYN_DRV
static void DoWarning (void)
/* Warn the user that a lightpen driver is needed for this program. */
{
    cprintf ("Warning: This program needs a lightpen\r\n"
             "driver with the name\r\n"
             "    %s\r\n"
             "on disk! Press 'y' if you have it; or,\r\n"
             "any other key to exit.\r\n", mouse_stddrv);
    if (tolower (cgetc ()) != 'y') {
        exit (EXIT_SUCCESS);
    }
    cprintf ("OK. Please wait patiently...\r\n");
}
#endif



static void __fastcall__ ShowState (unsigned char Jailed, unsigned char Invisible)
/* Display jail and cursor states. */
{
    cclearxy (0, 7, 40);
    gotoxy (0, 7);
    cprintf ("Lightpen cursor is %svisible%s.", Invisible? "in" : "", Jailed? " and jailed" : "");
}



int main (void)
{
    struct mouse_info info;
    struct mouse_box full_box, small_box;
    char C;
    unsigned char width, height;
    bool Invisible = false, Done = false, Jailed = false;

    /* Only the VIC-II has a lightpen connection. */
#ifdef __C128__
    videomode (VIDEOMODE_40x25);
#endif

    /* Initiate the debugger. */
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

#if defined(__C64__) || defined(__C128__)
    /* Copy the sprite data. */
    memcpy (SPRITE0_DATA, PenSprite, sizeof PenSprite);

    /* Set the VIC-II sprite pointer. */
    *SPRITE0_PTR = (unsigned) SPRITE0_DATA / sizeof SPRITE0_DATA;

    /* Set the color of sprite 0. */
    VIC.spr0_color = COLOR_BLACK;
#endif

#if DYN_DRV
    /* Output a warning about the driver that is needed. */
    DoWarning ();

    /* Load and install the lightpen driver. */
    CheckError ("mouse_load_driver",
                mouse_load_driver (&mouse_def_callbacks, mouse_stddrv));
#else
    /* Install the lightpen driver. */
    CheckError ("mouse_install",
                mouse_install (&mouse_def_callbacks, mouse_static_stddrv));
#endif

    /* Get the initial lightpen bounding box. */
    mouse_getbox (&full_box);

    screensize (&width, &height);

top:
    /* Print a help line. */
    clrscr ();
/*    revers (1); */
    cputs (" d)ebug  h)ide   q)uit   s)how   j)ail   ");
/*    revers (0); */

    /* Put a cross at the center of the screen. */
    gotoxy (width / 2 - 3, height / 2 - 1);
    cprintf ("%3u,%3u\r\n%*s\xDB", width / 2 * 8 + 4, height / 2 * 8 + 4,
             width / 2, "");

    /* Custom driver debugging line */
    cprintf ("\n\r\n Calibration X-offset = %u", (*(unsigned char *)0x3ff) * 2);

    /* Expose the arrow. */
    mouse_show ();
    ShowState (Jailed, Invisible);

    /* Test loop */
    do {
        /* Get the current lightpen co-ordinates and button states;
        ** and, print them.
        */
        mouse_info (&info);
        gotoxy (0, 2);
        cprintf (" X  = %3d\r\n", info.pos.x);
        cprintf (" Y  = %3d\r\n", info.pos.y);
        cprintf (" B1 = %c\r\n", (info.buttons & MOUSE_BTN_LEFT) ? 0x5F : '^');
        cprintf (" B2 = %c", (info.buttons & MOUSE_BTN_RIGHT) ? 0x5F : '^');

        /* Handle user input. */
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
    /* Uninstall and unload the lightpen driver. */
    CheckError ("mouse_unload", mouse_unload ());
#else
    /* Uninstall the lightpen driver. */
    CheckError ("mouse_uninstall", mouse_uninstall ());
#endif

    /* Say goodbye. */
    cputsxy (0, 16, "Goodbye!");
    return EXIT_SUCCESS;
}
