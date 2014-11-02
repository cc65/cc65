/*
** fputc.c
**
** Ullrich von Bassewitz, 02.06.1998
*/



#include <stdio.h>
#include <unistd.h>
#include "_file.h"



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



int __fastcall__ fputc (int c, register FILE* f)
{
    /* Check if the file is open or if there is an error condition */
    if ((f->f_flags & _FOPEN) == 0 || (f->f_flags & (_FERROR | _FEOF)) != 0) {
        goto ReturnEOF;
    }

    /* Write the byte */
    if (write (f->f_fd, &c, 1) != 1) {
        /* Error */
        f->f_flags |= _FERROR;
ReturnEOF:
        return EOF;
    }

    /* Return the byte written */
    return c & 0xFF;
}



