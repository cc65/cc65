/*
 * int fclose (FILE* f);
 */



#include <fcntl.h>
#include <errno.h>
#include "_file.h"



int fclose (FILE* f)
{
    if ((f->f_flags & _FOPEN) == 0) {
	/* File is not open */
       	_errno = EINVAL;       	       	/* File not input */
	return -1;
    }

    /* Reset the flags and close the file */
    f->f_flags = _FCLOSED;
    return close (f->f_fd);
}



