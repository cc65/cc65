/*
 * scanf.c
 *
 * Ullrich von Bassewitz (uz@cc65.org), 2004-11-26
 *
 */



#include <stdio.h>



/*****************************************************************************/
/*     	    	  		     Code				     */
/*****************************************************************************/



int scanf (const char* format, ...)
/* Standard C function */
{
    va_list ap;

    /* Setup for variable arguments */
    va_start (ap, format);

    /* Call vfscanf(). Since we know that va_end won't do anything, we will
     * save the call and return the value directly.
     */
    return vfscanf (stdin, format, ap);
}




