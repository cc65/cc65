/*
** puts.c
**
** Ullrich von Bassewitz, 11.08.1998
*/



#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "_file.h"



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



int __fastcall__ puts (const char* s)
{
    static char nl = '\n';

    /* Assume stdout is always open */
    if (write (stdout->f_fd, s, strlen (s)) < 0 ||
        write (stdout->f_fd, &nl, 1)        < 0) {
        stdout->f_flags |= _FERROR;
        return -1;
    }

    /* Done */
    return 0;
}



