/*
 * fseek.c
 *
 * Christian Groessler, 07-Aug-2000
 */


#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include "_file.h"


int fseek(FILE* f, long offset, int whence)
{
    long res;

    /* Is the file open? */
    if ((f->f_flags & _FOPEN) == 0) {
        _errno = EINVAL;                /* File not open */
        return 1;
    }

    res = lseek(f->f_fd, offset, whence);
    if (res == -1L) return 1;
    return 0;
}

