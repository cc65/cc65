/*
 * Ullrich von Bassewitz, 11.08.1998
 *
 * int fgetc (FILE* f);
 */



#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include "_file.h"



int fgetc (FILE* f)
{
    char c;

    /* Check if the file is open or if there is an error condition */
    if ((f->f_flags & _FOPEN) == 0 || (f->f_flags & (_FERROR | _FEOF)) != 0) {
    	return -1;
    }

    /* Read the byte */
    switch (read (f->f_fd, &c, 1)) {

        case -1:
	    /* Error */
	    f->f_flags |= _FERROR;
	    return -1;

        case 0:
	    /* EOF */
	    f->f_flags |= _FEOF;
	    return -1;

        default:
	    /* Char read */
	    return ((int) c) & 0xFF;

    }
}



