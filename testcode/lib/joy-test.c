#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <conio.h>
#include <joystick.h>


int main (void)
{
    unsigned char j;
    unsigned char count;
    unsigned char i;

#ifdef __NES__
    extern void *co65_joy;
    unsigned char Res = joy_install (&co65_joy);
#else
    unsigned char Res = joy_load_driver (joy_stddrv);
#endif
    if (Res != JOY_ERR_OK) {
       	cprintf ("Error in joy_load_driver: %u\r\n", Res);
        cprintf ("os: %u, %s\r\n", _oserror, _stroserror (_oserror));
       	exit (EXIT_FAILURE);
    }

    clrscr ();
    count = joy_count ();
    cprintf ("Driver supports %d joystick(s)", count);
    while (1) {
	for (i = 0; i < count; ++i) {
	    gotoxy (0, i+1);
	    j = joy_read (i);
	    cprintf ("%2d: %-6s%-6s%-6s%-6s%-6s%-6s",
		     i,
       	       	     (j & joy_masks[JOY_UP])?    "  up  " : " ---- ",
	    	     (j & joy_masks[JOY_DOWN])?	 " down " : " ---- ",
	    	     (j & joy_masks[JOY_LEFT])?  " left " : " ---- ",
	    	     (j & joy_masks[JOY_RIGHT])? "right " : " ---- ",
	    	     (j & joy_masks[JOY_FIRE])?  " fire " : " ---- ",
	    	     (j & joy_masks[JOY_FIRE2])? "fire2 " : " ---- ");
	}
    }
    return 0;
}

