/*
 * vfscanf.c
 *
 * Ullrich von Bassewitz (uz@cc65.org), 2004-11-26
 *
 */



#include <stdio.h>
#include "_scanf.h"



/*****************************************************************************/
/*     	    	     		     Code				     */
/*****************************************************************************/



int __fastcall__ vfscanf (FILE* f, const char* format, va_list ap)
/* Standard C function */
{
    struct scanfdata d;

    /* Initialize the data struct. We do only need the given file as user data,
     * since the get and ungetc are crafted so they match the standard fgetc
     * and ungetc functions.
     */
    d.get    = (getfunc) fgetc,
    d.unget  = (ungetfunc) ungetc,
    d.data   = f;

    /* Call the internal function and return the result */
    return _scanf (&d, format, ap);
}



