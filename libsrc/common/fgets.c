/*
 * Ullrich von Bassewitz, 11.08.1998
 *
 * char* fgets (char* s, int size, FILE* f);
 */



#include <stdio.h>
#include <errno.h>
#include "_file.h"



char* fgets (char* s, unsigned size, FILE* f)
{
    int i, c;

    /* We do not handle the case "size == 0" here */
    i = 0; --size;
    while (i < size) {

       	/* Get next character */
       	c = fgetc (f);
       	if (c == EOF) {
       	    s [i] = 0;
       	    /* Error or EOF */
       	    if (f->f_flags & _FERROR) {
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

	/* Stop at end of line */
	if (c == '\n') {
 	    break;
	}
    }

    /* Replace newline by NUL */
    s [i-1] = '\0';

    /* Done */
    return s;
}




