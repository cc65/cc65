/*
 * Ullrich von Bassewitz, 11.08.1998
 *
 * char* fgets (char* s, int size, FILE* f);
 */



#include <stdio.h>
#include <errno.h>
#include "_file.h"



/*****************************************************************************/
/*     	    	     		     Code				     */
/*****************************************************************************/



char* __fastcall__ fgets (char* s, unsigned size, FILE* f)
{
    unsigned i;
    int c;

    if (size == 0) {
        /* Invalid size */
        _errno = EINVAL;
        return 0;
    }

    /* Read input */
    i = 0;
    while (--size) {

       	/* Get next character */
       	if ((c = fgetc (f)) == EOF) {
       	    s[i] = '\0';
       	    /* Error or EOF */
       	    if ((f->f_flags & _FERROR) != 0 || i == 0) {
       	     	/* ERROR or EOF on first char */
       	     	return 0;
       	    } else {
       	        /* EOF with data already read */
                break;
    	    }
       	}

       	/* One char more */
       	s[i++] = c;

     	/* Stop at end of line */
     	if (c == '\n') {
     	    break;
     	}
    }

    /* Terminate the string */
    s[i] = '\0';

    /* Done */
    return s;
}



