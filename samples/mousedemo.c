/*
 * Demo program for mouse usage. Will work for the C64/C128/Atari
 *
 * Ullrich von Bassewitz, 13.09.2001
 *
 */



#include <stdlib.h>
#include <string.h>
#include <mouse.h>
#include <conio.h>
#include <dbg.h>



#if defined(__C64__) || defined(__C128__) || defined(__CBM510__)

/* Address of data for sprite 0 */
#if defined(__C64__)
#  define SPRITE0_DATA    0x0340
#  define SPRITE0_PTR  	  0x07F8
#elif defined(__C128__)
#  define SPRITE0_DATA    0x0E00
#  define SPRITE0_PTR     0x07F8
#elif defined(__CBM510__)
#  define SPRITE0_DATA    0xF800
#  define SPRITE0_PTR     0xF7F8
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



static void ShowState (unsigned char Invisible)
/* Display cursor visibility */
{
    gotoxy (0, 6);
    cclear (40);
    gotoxy (0, 6);
    cprintf ("Mouse cursor %svisible", Invisible? "in" : "");
}



int main (void)
{
    struct mouse_info info;
    unsigned char Invisible;
    unsigned char Done;

    /* Initialize the debugger */
    DbgInit (0);

    /* Clear the screen, set white on black */
    bordercolor (COLOR_BLACK);
    bgcolor (COLOR_BLACK);
    textcolor (COLOR_GRAY3);
    cursor (0);
    clrscr ();

    /* Print a help line */
    revers (1);
    cputsxy (0, 0, "d: debug   h: hide   q: quit   s: show  ");
    revers (0);

#if defined(__C64__) || defined(__C128__) || defined(__CBM510__)
    /* Copy the sprite data */
    memcpy ((void*) SPRITE0_DATA, MouseSprite, sizeof (MouseSprite));

    /* Set the VIC sprite pointer */
    *(unsigned char*)SPRITE0_PTR = SPRITE0_DATA / 64;

    /* Set the color of sprite 0 */
    VIC.spr0_color = COLOR_WHITE;

    /* Initialize the mouse */
    mouse_init (MOUSE_C64);

#elif defined(__ATARI__)

    /* Initialize the mouse */
    mouse_init (MOUSE_TRAKBALL);

#endif

    /* Test loop */
    Done = 0;
    ShowState (Invisible = 1);
    while (!Done) {
     	if (kbhit()) {
     	    switch (cgetc()) {
		case 'd':
		    BREAK();
		    break;
		case 'h':
 	   	    ShowState (++Invisible);
	   	    mouse_hide ();
	   	    break;
		case 's':
     	   	    if (Invisible) {
     	   	     	ShowState (--Invisible);
     	   	 	mouse_show ();
	       	    }
	   	    break;
	   	case 'q':
		    Done = 1;
		    break;
	    }
	}

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

    }

    mouse_done ();
    clrscr ();
    cputs ("Goodbye!\r\n");

    return EXIT_SUCCESS;
}



