/*
** freopen.c
**
** Ullrich von Bassewitz, 17.06.1998
*/



#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include "_file.h"



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



FILE* __fastcall__ freopen (const char* name, const char* mode, FILE* f)
{
    /* Check if the file is open, if so, close it */
    if ((f->f_flags & _FOPEN) == 0) {
        /* File is not open */
        return (FILE*) _seterrno (EINVAL);      /* File not input */
    }

    /* Close the file. Don't bother setting the flag, it will get
    ** overwritten by _fopen.
    */
    if (close (f->f_fd) < 0) {
        /* An error occurred, errno is already set */
        return 0;
    }

    /* Open the file and return the descriptor */
    return _fopen (name, mode, f);
}



