/*
** Test program for mouse drivers.
** Supportsthe C64/C128/CBM510/Atari/Apple2.
**
** 2001-09-13, Ullrich von Bassewitz
** 2013-09-05, Greg King
**
** Compile with "-DSTATIC_MOUSE" to statically link all available drivers.
** Compile with "-DMOUSE_DRIVER=<driver_sym>" to statically link the given driver.
**     E.g., -DMOUSE_DRIVER=atrst_mou to just link with the Atari ST mouse driver.
*/



#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <mouse.h>
#include <pen.h>
#include <conio.h>
#include <ctype.h>
#include <dbg.h>
#include <cc65.h>

#define max(a,b)  (((a) > (b)) ? (a) : (b))
#define min(a,b)  (((a) < (b)) ? (a) : (b))

extern int getsp(void);

#define NO_DEBUG
#define NO_JAIL

#ifdef __ATARI__
extern const struct mouse_callbacks mouse_pm_callbacks;
extern const struct mouse_callbacks mouse_txt_callbacks;
//#define MOUSE_CALLBACK mouse_def_callbacks
#define MOUSE_CALLBACK mouse_pm_callbacks
//#define MOUSE_CALLBACK mouse_txt_callbacks
#else
#define MOUSE_CALLBACK mouse_def_callbacks
#endif

#if defined(MOUSE_DRIVER) || defined(STATIC_MOUSE)

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



#ifdef __CBM__

/* Set dark-on-light colors. */
const unsigned char mouse_def_pointercolor = COLOR_BLACK;

#endif



static void __fastcall__ CheckError (const char* S, unsigned char Error)
{
    if (Error != MOUSE_ERR_OK) {
        cprintf ("\n%s: %s(%u)\r\n", S, mouse_geterrormsg (Error), Error);

        /* Wait for a key-press, so that some platforms can show the error
        ** message before they remove the current screen.
        */
        if (doesclrscrafterexit ()) {
            cgetc ();
        }
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

#else

unsigned char *mouse_drv_use;
#endif


#ifdef __ATARI__
#ifdef __ATARIXL__
#define MSENAME_EXT "X"
#define MSESTAT_0 atrxjoy_mou
#define MSESTAT_1 atrxst_mou
#define MSESTAT_2 atrxami_mou
#define MSESTAT_3 atrxtrk_mou
#define MSESTAT_4 atrxtt_mou
#else
#define MSENAME_EXT ""
#define MSESTAT_0 atrjoy_mou
#define MSESTAT_1 atrst_mou
#define MSESTAT_2 atrami_mou
#define MSESTAT_3 atrtrk_mou
#define MSESTAT_4 atrtt_mou
#endif
#define MSENAME_0 "ATR" MSENAME_EXT "JOY.MOU"
#define MSENAME_1 "ATR" MSENAME_EXT "ST.MOU"
#define MSENAME_2 "ATR" MSENAME_EXT "AMI.MOU"
#define MSENAME_3 "ATR" MSENAME_EXT "TRK.MOU"
#define MSENAME_4 "ATR" MSENAME_EXT "TT.MOU"
#elif defined(__C64__) || defined(__C128__)
#ifdef __C64__
#define MSENAME_EXT "c64-"
#define MSESTAT_0 c64_joy_mou
#define MSESTAT_1 c64_1351_mou
#define MSESTAT_2 c64_inkwell_mou
#define MSESTAT_3 c64_pot_mou
#else
#define MSENAME_EXT "c128-"
#define MSESTAT_0 c128_joy_mou
#define MSESTAT_1 c128_1351_mou
#define MSESTAT_2 c128_inkwell_mou
#define MSESTAT_3 c128_pot_mou
#endif
#define MSENAME_0 MSENAME_EXT "joy.mou"
#define MSENAME_1 MSENAME_EXT "1351.mou"
#define MSENAME_2 MSENAME_EXT "inkwell.mou"
#define MSENAME_3 MSENAME_EXT "pot.mou"
#endif


static void __fastcall__ ShowState (unsigned char Jailed, unsigned char Invisible)
/* Display jail and cursor states. */
{
    cclearxy (0, 7, 32);
    gotoxy (0, 7);
    cprintf ("Pointer is %svisible%s.", Invisible? "in" : "", Jailed? " and jailed" : "");
}

#ifdef __ATARIXL__
extern char _HIDDEN_RAM_SIZE__, _HIDDEN_RAM_LAST__, _HIDDEN_RAM_START__;
#endif

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

#ifdef __ATARIXL__
    cprintf ("adding heap: $%04X bytes at $%04X\r\n",
             &_HIDDEN_RAM_SIZE__ - (&_HIDDEN_RAM_LAST__ - &_HIDDEN_RAM_START__),
             &_HIDDEN_RAM_LAST__);

    _heapadd (&_HIDDEN_RAM_LAST__, (size_t)(&_HIDDEN_RAM_SIZE__ - (&_HIDDEN_RAM_LAST__ - &_HIDDEN_RAM_START__)));
    cgetc ();
#endif

#ifndef NO_DEBUG
    /* Initialize the debugger */
    DbgInit (0);
#endif

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
#if defined(__ATARI__) || defined(__C64__) || defined(__C128__)
        char selection, flag = 0;
        cprintf ("Select mouse driver:\r\n"
                 "  0 - Joystick\r\n"
#ifdef __ATARI__
                 "  1 - ST Mouse\r\n"
                 "  2 - Amiga Mouse\r\n"
                 "  3 - Atari Trakball\r\n"
                 "  4 - Atari TouchPad\r\n"
#else
                 "  1 - 1351 Mouse\r\n"
                 "  2 - Inkwell Mouse\r\n"
                 "  3 - Paddle\r\n"
#endif
                 "Enter selection: ");
        while (1) {
            switch (selection = cgetc ()) {
            case '0': mouse_name = MSENAME_0; flag = 1; break;
            case '1': mouse_name = MSENAME_1; flag = 1; break;
            case '2': mouse_name = MSENAME_2; flag = 1; break;
            case '3': mouse_name = MSENAME_3; flag = 1; break;
#ifdef __ATARI__
            case '4': mouse_name = MSENAME_4; flag = 1; break;
#endif
            }
            if (flag) break;
        }
        cprintf ("%c\r\nOK, loading \"%s\",\r\nplease wait patiently...\r\n", selection, mouse_name);
#else
        /* Output a warning about the standard driver that is needed. */
        DoWarning ();
        mouse_name = mouse_stddrv;
#endif
    }

    /* Load and install the driver. */
    CheckError ("mouse_load_driver",
                mouse_load_driver (&MOUSE_CALLBACK, mouse_name));
#else  /* not DYN_DRV */
#if !defined(MOUSE_DRIVER) && (defined(__ATARI__) || defined(__C64__) || defined(__C128__))
    {
        char selection, flag = 0;
        cprintf ("Select mouse driver:\r\n"
                 "  0 - Joystick\r\n"
#ifdef __ATARI__
                 "  1 - ST Mouse\r\n"
                 "  2 - Amiga Mouse\r\n"
                 "  3 - Atari Trakball\r\n"
                 "  4 - Atari TouchPad\r\n"
#else
                 "  1 - 1351 Mouse\r\n"
                 "  2 - Inkwell Mouse\r\n"
                 "  3 - Paddle\r\n"
#endif
                 "Enter selection: ");
        while (1) {
            switch (selection = cgetc ()) {
            case '0': mouse_drv_use = MSESTAT_0; flag = 1; break;
            case '1': mouse_drv_use = MSESTAT_1; flag = 1; break;
            case '2': mouse_drv_use = MSESTAT_2; flag = 1; break;
            case '3': mouse_drv_use = MSESTAT_3; flag = 1; break;
#ifdef __ATARI__
            case '4': mouse_drv_use = MSESTAT_4; flag = 1; break;
#endif
            }
            if (flag) break;
        }
    }
#else
    mouse_drv_use = mouse_static_stddrv;
#endif

    /* Install the driver. */
    CheckError ("mouse_install",
                mouse_install (&MOUSE_CALLBACK,
#  ifdef MOUSE_DRIVER
                               MOUSE_DRIVER
#  else
#if defined(__ATARI__) || defined(__C64__) || defined(__C128__)
                               mouse_drv_use
#else
                               mouse_static_stddrv
#endif
#  endif
                               ));
#endif

#ifndef NO_JAIL
    /* Get the initial bounding box. */
    mouse_getbox (&full_box);
#endif

    screensize (&width, &height);

top:
    clrscr ();

    /* Print a help line */
    cputs (" d)ebug  h)ide   q)uit   s)how   j)ail");

    gotoxy (1, 20);
    cprintf ("SP: $%04X", getsp());

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
#ifndef NO_DEBUG
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
#endif
                case 'h':
                    mouse_hide ();
                    ShowState (Jailed, ++Invisible);
                    break;

#ifndef NO_JAIL
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
#endif
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
