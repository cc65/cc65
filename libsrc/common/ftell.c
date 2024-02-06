/*
** ftell.c
**
** Christian Groessler, 2000-08-07
** Ullrich von Bassewitz, 2004-05-13
*/



#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include "_file.h"



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



long __fastcall__ ftell (register FILE* f)
{
    long pos;

    /* Is the file open? */
    if ((f->f_flags & _FOPEN) == 0) {
        _seterrno (EINVAL);                /* File not open */
        return -1L;
    }

    /* Call the low level function */
    pos = lseek (f->f_fd, 0L, SEEK_CUR);

    /* If we didn't have an error, correct the return value in case we have
    ** a pushed back character.
    */
    if (pos > 0 && (f->f_flags & _FPUSHBACK)) {
        --pos;
    }

    /* -1 for error, comes from lseek() */
    return pos;
}

