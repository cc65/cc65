/*
 * ftell.c
 *
 * Christian Groessler, 07-Aug-2000
 */


#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include "_file.h"


long ftell(FILE* f)
{
    long pos;

    /* Is the file open? */
    if ((f->f_flags & _FOPEN) == 0) {
        _errno = EINVAL;                /* File not open */
        return -1L;
    }

    pos = lseek(f->f_fd, 0L, SEEK_CUR);
    return pos;    /* -1 for error, comes from lseek() */
}

