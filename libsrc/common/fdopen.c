/*
 * fopen.c
 *
 * Ullrich von Bassewitz, 17.06.1998
 */



#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include "_file.h"



FILE* fdopen (int handle, char* /*mode*/)
{
    FILE* f;

    /* Find a free file slot */
    if (!(f = _fdesc ())) {
       	/* No slots */
       	errno = EMFILE;    	      	/* Too many files */
       	return 0;
    }

    /* Insert the handle, and return the descriptor */
    f->f_fd    = handle;
    f->f_flags = _FOPEN;

    /* Return the file descriptor */
    return f;
}




