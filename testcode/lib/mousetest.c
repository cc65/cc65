#include <stdlib.h>
#include <string.h>
#include <mouse.h>
#include <conio.h>



#if defined(__C64__) || defined(__C128__)

/* Address of data for sprite 1 */
#define SPRITE1_DATA    0x340

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
    0x00,  120, 0x00,
    0x00,   60, 0x00,
    0x00,   30, 0x00,
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

#endif


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

    /* Clear the screen, set white on black */
    bordercolor (COLOR_BLACK);
    bgcolor (COLOR_BLACK);
    textcolor (COLOR_GRAY3);
    cursor (0);
    clrscr ();

    /* Print a help line */
    cputsxy (0, 0, "s = show    h = hide    q = quit");

#if defined(__C64__) || defined(__C128__)
    /* Copy the sprite data */
    memcpy ((void*) SPRITE1_DATA, MouseSprite, sizeof (MouseSprite));

    /* Set the VIC sprite pointer */
    *(unsigned char*)0x7F8 = SPRITE1_DATA / 64;

    /* Set the color of sprite 0 */
    VIC.spr0_color = COLOR_WHITE;

    /* Initialize the mouse */
    mouse_init (0, 1, MOUSE_C64);
#endif

    /* Move the mouse to the center of the screen */
    mouse_move (160, 100);

    /* Test loop */
    ShowState (Invisible = 1);
    while (1) {
	if (kbhit()) {
	    switch (cgetc()) {
	       	case 's':
	   	    if (Invisible) {
	   	     	ShowState (--Invisible);
	   	 	mouse_show ();
	       	    }
	   	    break;
	   	case 'h':
 	   	    ShowState (++Invisible);
	   	    mouse_hide ();
	   	    break;
	   	case 'q':
	   	    mouse_done ();
	   	    exit (0);
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
    return 0;
}



		 
