/*
 * fopen.c
 *
 * Ullrich von Bassewitz, 17.06.1998
 */



#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include "_file.h"



FILE* fopen (const char* name, const char* mode)
{
    FILE* f;

    /* Find a free file slot */
    if (!(f = _fdesc ())) {
       	/* No slots */
       	_errno = EMFILE;		/* Too many files */
       	return 0;
    }

    /* Open the file and return the descriptor */
    return _fopen (name, mode, f);
}



