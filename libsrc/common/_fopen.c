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
    char	  c;
    char 	  flag = 0;
    unsigned char binmode = 0;

    c = *mode++;
    switch(c) {
        case 'w':
            binmode = O_WRONLY;
            break;
        case 'r':
            binmode = O_RDONLY;
            break;
        case 'a':
            binmode = O_WRONLY | O_APPEND;
            break;
       default:
            return 0;  /* invalid char */
    }

    while (c = *mode++) {
        switch(c) {
            case '+':
                binmode = (binmode & ~15) | O_RDWR;
                break;
            case 'b':
                /* currently ignored */
                break;
            default:
                return 0;  /* invalid char */
        }
    }
    return binmode;
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



