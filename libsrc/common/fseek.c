/*
** fseek.c
**
** Christian Groessler, 2000-08-07
** Ullrich von Bassewitz, 2004-05-12
*/



#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include "_file.h"



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



int __fastcall__ fseek (register FILE* f, long offset, int whence)
{
    long res;

    /* Is the file open? */
    if ((f->f_flags & _FOPEN) == 0) {
        _seterrno (EINVAL);             /* File not open */
        return -1;
    }

    /* If we have a pushed back character, and whence is relative to the
    ** current position, correct the offset.
    */
    if ((f->f_flags & _FPUSHBACK) && whence == SEEK_CUR) {
        --offset;
    }

    /* Do the seek */
    res = lseek(f->f_fd, offset, whence);

    /* If the seek was successful. Discard any effects of the ungetc function,
    ** and clear the end-of-file indicator. Otherwise set the error indicator
    ** on the stream, and return -1. We will check for >= 0 here, because that
    ** saves some code, and we don't have files with 2 gigabytes in size
    ** anyway:-)
    */
    if (res >= 0) {
        f->f_flags &= ~(_FEOF | _FPUSHBACK);
        return 0;
    } else {
        f->f_flags |= _FERROR;
        return -1;
    }
}

