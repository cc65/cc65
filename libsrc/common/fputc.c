/*
 * fputc.c
 *
 * Ullrich von Bassewitz, 02.06.1998
 */



#include <stdio.h>
#include <unistd.h>
#include "_file.h"



/*****************************************************************************/
/*     	    	     		     Code				     */
/*****************************************************************************/



int __fastcall__ fputc (int c, FILE* f)
{
    /* Check if the file is open or if there is an error condition */
    if ((f->f_flags & _FOPEN) == 0 || (f->f_flags & (_FERROR | _FEOF)) != 0) {
    	return EOF;
    }

    /* Write the byte (knows about byte order!) */
    if (write (f->f_fd, &c, 1) <= 0) {
   	/* Error */
	f->f_flags |= _FERROR;
	return EOF;
    }

    /* Return the byte written */
    return c & 0xFF;
}



