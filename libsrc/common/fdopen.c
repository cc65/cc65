/*
 * fdopen.c
 *
 * Ullrich von Bassewitz, 17.06.1998
 */



#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include "_file.h"



/*****************************************************************************/
/*     	    	     		     Code				     */
/*****************************************************************************/



FILE* __fastcall__ fdopen (int handle, const char* /*mode*/)
{
    FILE* f;

    /* Find a free file slot */
    if (!(f = _fdesc ())) {
       	/* No slots */
       	return (FILE*) _seterrno (EMFILE);      /* Too many files */
    }

    /* Insert the handle, and return the descriptor */
    f->f_fd    = handle;
    f->f_flags = _FOPEN;

    /* Return the file descriptor */
    return f;
}




