/*
 * fread.c
 *
 * Ullrich von Bassewitz, 02.06.1998
 */



#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include "_file.h"



size_t fread (void* buf, size_t size, size_t count, FILE* f)
{
    int bytes;

    /* Is the file open? */
    if ((f->f_flags & _FOPEN) == 0) {
       	_errno = EINVAL;     	    	/* File not open */
     	return (size_t) -1;
    }

    /* Did we have an error or EOF? */
    if ((f->f_flags & (_FERROR | _FEOF)) != 0) {
       	/* Cannot read from stream */
       	return 0;
    }

    /* How many bytes to read? */
    bytes = size * count;

    if (bytes) {
	/* Read the data. */
	bytes = read (f->f_fd, buf, bytes);
	if (bytes == -1) {
	    /* Read error */
	    f->f_flags |= _FERROR;
	    return (size_t) -1;
	}
	if (bytes == 0) {
	    /* End of file */
	    f->f_flags |= _FEOF;
	    return (size_t) -1;
	}

        /* Unfortunately, we cannot avoid the divide here... */
        return bytes / size;

    } else {

	/* 0 bytes read */
	return count;

    }
}



