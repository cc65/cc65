#include <stdlib.h>
#include <mouse.h>
#include <conio.h>



int main (void)
{
    struct mouse_pos pos;

    clrscr ();
    mouse_init (1, 1, MOUSE_C64);
/*    mouse_box (0, 29, 344, 250); */
    while (1) {
	if (kbhit()) {
	    switch (cgetc()) {
		case 's':   mouse_show ();	break;
		case 'h':   mouse_hide ();	break;
		case 'q':   mouse_done ();	exit (0);
	    }
	}
	mouse_pos (&pos);
	gotoxy (0, 0);
	cprintf ("%04X", pos.x);
	gotoxy (0, 1);
	cprintf ("%04X", pos.y);
    }
    return 0;
}



