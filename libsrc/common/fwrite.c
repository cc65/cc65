/*
 * fwrite.c
 *
 * Ullrich von Bassewitz, 04.06.1998
 */



#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include "_file.h"



size_t fwrite (void* buf, size_t size, size_t count, FILE* f)
{
    int bytes;

    /* Is the file open? */
    if ((f->f_flags & _FOPEN) == 0) {
       	_errno = EINVAL;	   	/* File not open */
    	return -1;
    }

    /* Did we have an error */
    if ((f->f_flags & _FERROR) != 0) {
    	/* Cannot write to stream */
       	return 0;
    }

    /* How many bytes to write? */
    bytes = size * count;

    if (bytes) {
	/* Write the data. */
       	if (write (f->f_fd, buf, bytes) == -1) {
	    /* Write error */
	    f->f_flags |= _FERROR;
	    return -1;
	}
    }

    /* Don't waste time with expensive calculations, assume the write was
     * complete and return the count of items.
     */
    return count;
}



