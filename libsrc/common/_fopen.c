/*
 * _fopen.c
 *
 * Ullrich von bassewitz, 17.06.1997
 */



#include <fcntl.h>
#include <errno.h>
#include "_file.h"



static unsigned char amode_to_bmode (const char* mode)
/* Convert ASCII mode (like for fopen) to binary mode (for open) */
{
    unsigned char binmode;

    switch (*mode++) {
        case 'w':
            binmode = O_WRONLY | O_CREAT | O_TRUNC;
            break;
        case 'r':
            binmode = O_RDONLY;
            break;
        case 'a':
            binmode = O_WRONLY | O_CREAT | O_APPEND;
            break;
       default:
            return 0;  /* invalid char */
    }

    while (1) {
        switch (*mode++) {
            case '+':
                /* always to r/w in addition to anything already set */
                binmode |= O_RDWR;
                break;
            case 'b':
                /* currently ignored */
                break;
            case '\0':
                /* end of mode string reached */
                return binmode;
            default:
                /* invalid char in mode string */
                return 0;
        }
    }
}



FILE* _fopen (const char* name, const char* mode, FILE* f)
/* Open the specified file and fill the descriptor values into f */
{
    int 	  fd;
    unsigned char binmode;


    /* Convert ASCII mode to binary mode */
    if ((binmode = amode_to_bmode (mode)) == 0) {
	/* Invalid mode */
    	_errno = EINVAL;
        return 0;
    }

    /* Open the file */
    fd = open (name, binmode);
    if (fd == -1) {
       	/* Error - _oserror is set */
       	return 0;
    }

    /* Remember fd, mark the file as opened */
    f->f_fd    = fd;
    f->f_flags = _FOPEN;

    /* Return the file descriptor */
    return f;
}



