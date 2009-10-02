/*
 * Demo program for mouse usage. Will work for the C64/C128/CBM510/Atari/Apple2
 *
 * Ullrich von Bassewitz, 13.09.2001
 *
 */



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mouse.h>
#include <conio.h>
#include <ctype.h>
#include <dbg.h>



#if defined(__C64__) || defined(__C128__)

/* Address of data for sprite 0 */
#if defined(__C64__)
#  define SPRITE0_DATA    0x0340
#  define SPRITE0_PTR  	  0x07F8
#elif defined(__C128__)
#  define SPRITE0_DATA    0x0E00
#  define SPRITE0_PTR     0x07F8
#endif

/* The mouse sprite (an arrow) */
static const unsigned char MouseSprite[64] = {
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x0F, 0xE0, 0x00,
    0x0F, 0xC0, 0x00,
    0x0F, 0x80, 0x00,
    0x0F, 0xC0, 0x00,
    0x0D, 0xE0, 0x00,
    0x08, 0xF0, 0x00,
    0x00, 0x78, 0x00,
    0x00, 0x3C, 0x00,
    0x00, 0x1E, 0x00,
    0x00, 0x0F, 0x00,
    0x00, 0x07, 0x80,
    0x00, 0x03, 0x80,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00
};

#endif  /* __C64__ or __C128__ */


#define max(a,b)  (((a) > (b)) ? (a) : (b))
#define min(a,b)  (((a) < (b)) ? (a) : (b))



static void CheckError (const char* S, unsigned char Error)
{
    if (Error != MOUSE_ERR_OK) {
        printf ("%s: %s(%d)\n", S, mouse_geterrormsg (Error), Error);
        exit (EXIT_FAILURE);
    }
}



static void DoWarning (void)
/* Warn the user that a mouse driver is needed for this program */
{
    printf ("Warning: This program needs the mouse\n"
            "driver with the name\n"
            "    %s\n"
            "on disk! Press 'y' if you have it or\n"
            "any other key to exit.\n", mouse_stddrv);
    if (tolower (cgetc ()) != 'y') {
        exit (EXIT_SUCCESS);
    }
    printf ("Ok. Please wait patiently...\n");
}



static void ShowState (unsigned char Jailed, unsigned char Invisible)
/* Display jail and cursor state */
{
    gotoxy (0, 6);
    cclear (40);
    gotoxy (0, 6);
    cprintf ("Mouse cursor %svisible%s", Invisible? "in" : "", Jailed? ", jailed" : "");
}



int main (void)
{
    struct mouse_info info;
    struct mouse_box full_box;
    struct mouse_box small_box;
    unsigned char Invisible;
    unsigned char Done;
    unsigned char Jailed;

    /* Initialize the debugger */
    DbgInit (0);

    /* Output a warning about the driver that is needed */
    DoWarning ();

    /* Clear the screen, set white on black */
    (void) bordercolor (COLOR_BLACK);
    (void) bgcolor (COLOR_BLACK);
    (void) textcolor (COLOR_GRAY3);
    cursor (0);
    clrscr ();

#if defined(__C64__) || defined(__C128__) || defined(__CBM510__)
    /* Copy the sprite data */
    memcpy ((void*) SPRITE0_DATA, MouseSprite, sizeof (MouseSprite));

    /* Set the VIC sprite pointer */
    *(unsigned char*)SPRITE0_PTR = SPRITE0_DATA / 64;

    /* Set the color of sprite 0 */
#ifdef __CBM510__
    pokebsys ((unsigned) &VIC.spr0_color, COLOR_WHITE);
#else
    VIC.spr0_color = COLOR_WHITE;
#endif

#endif

    /* Load and install the mouse driver */
    CheckError ("mouse_load_driver",
                mouse_load_driver (&mouse_def_callbacks, mouse_stddrv));

    /* Get the initial mouse bounding box */
    mouse_getbox (&full_box);

    /* Print a help line */
    revers (1);
    cputsxy (0, 0, "d)ebug  h)ide   q)uit   s)how   j)ail   ");
    revers (0);

    /* Test loop */
    Done = 0;
    Jailed = 0;
    Invisible = 1;
    ShowState (Jailed, Invisible);
    while (!Done) {

	/* Get the current mouse coordinates and button states and print them */
	mouse_info (&info);
      	gotoxy (0, 2);
       	cprintf ("X  = %3d", info.pos.x);
      	gotoxy (0, 3);
      	cprintf ("Y  = %3d", info.pos.y);
      	gotoxy (0, 4);
      	cprintf ("LB = %c", (info.buttons & MOUSE_BTN_LEFT)? '1' : '0');
      	gotoxy (0, 5);
      	cprintf ("RB = %c", (info.buttons & MOUSE_BTN_RIGHT)? '1' : '0');

        /* Handle user input */
     	if (kbhit ()) {
     	    switch (tolower (cgetc ())) {

		case 'd':
		    BREAK();
		    break;

		case 'h':
 	   	    ShowState (Jailed, ++Invisible);
	   	    mouse_hide ();
	   	    break;

                case 'j':
                    if (Jailed) {
                        Jailed = 0;
                        mouse_setbox (&full_box);
                    } else {
                        Jailed = 1;
                        small_box.minx = max (info.pos.x - 10, full_box.minx);
                        small_box.miny = max (info.pos.y - 10, full_box.miny);
                        small_box.maxx = min (info.pos.x + 10, full_box.maxx);
                        small_box.maxy = min (info.pos.y + 10, full_box.maxy);
                        mouse_setbox (&small_box);
                    }
                    ShowState (Jailed, Invisible);
                    break;

		case 's':
     	   	    if (Invisible) {
     	   	     	ShowState (Jailed, --Invisible);
     	   	 	mouse_show ();
	       	    }
	   	    break;

	   	case 'q':
		    Done = 1;
		    break;
	    }
	}

    }

    /* Uninstall and unload the mouse driver */
    CheckError ("mouse_unload", mouse_unload ());

    /* Say goodbye */
    clrscr ();
    cputs ("Goodbye!\r\n");

    return EXIT_SUCCESS;
}



