/*
** fgetc.c
**
** (C) Copyright 1998, 2002 Ullrich von Bassewitz (uz@cc65.org)
**
*/



#include <stdio.h>
#include <unistd.h>
#include "_file.h"



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



int __fastcall__ fgetc (register FILE* f)
{
    unsigned char c;

    /* Check if the file is open or if there is an error condition */
    if ((f->f_flags & _FOPEN) == 0 || (f->f_flags & (_FERROR | _FEOF)) != 0) {
        return EOF;
    }

    /* If we have a pushed back character, return it */
    if (f->f_flags & _FPUSHBACK) {
        f->f_flags &= ~_FPUSHBACK;
        return f->f_pushback;
    }

    /* Read one byte */
    switch (read (f->f_fd, &c, 1)) {

        case -1:
            /* Error */
            f->f_flags |= _FERROR;
            return EOF;

        case 0:
            /* EOF */
            f->f_flags |= _FEOF;
            return EOF;

        default:
            /* Char read */
            return c;

    }
}



