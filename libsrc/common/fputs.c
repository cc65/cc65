/*
 * int fputs (const char* s, FILE* f);
 *
 * Ullrich von Bassewitz, 11.08.1998
 */



#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include "_file.h"



int fputs (char* s, FILE* f)
{
    /* Check if the file is open or if there is an error condition */
    if ((f->f_flags & _FOPEN) == 0 || (f->f_flags & (_FERROR | _FEOF)) != 0) {
    	return -1;
    }

    /* Write the string */
    return write (f->f_fd, s, strlen (s));
}



