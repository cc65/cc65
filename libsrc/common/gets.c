/*
 * gets.c
 *
 * Ullrich von Bassewitz, 11.08.1998
 */



#include <stdio.h>
#include "_file.h"



/*****************************************************************************/
/*     	    	     		     Code				     */
/*****************************************************************************/



char* __fastcall__ gets (char* s)
{
    int c;
    int i = 0;

    do {

       	/* Get next character */
       	if ((c = fgetc (stdin)) == EOF) {
       	    /* Error or EOF */
       	    s [i] = 0;
       	    if (stdin->f_flags & _FERROR) {
       	    	/* ERROR */
       	    	return 0;
       	    } else {
       	        /* EOF */
		if (i) {
		    return s;
		} else {
		    return 0;
		}
     	    }
       	}

       	/* One char more */
       	s [i++] = c;

    } while (c != '\n');

    /* Replace newline by NUL */
    s [i-1] = '\0';

    /* Done */
    return s;
}



		   
