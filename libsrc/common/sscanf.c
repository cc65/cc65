/*
 * sscanf.c
 *
 * (C) Copyright 2001-2002 Ullrich von Bassewitz (uz@cc65.org)
 *
 */



#include <stdio.h>



/*****************************************************************************/
/*     	    	  		     Code				     */
/*****************************************************************************/



int sscanf (const char* str, const char* format, ...)
/* Standard C function */
{
    va_list ap;

    /* Setup for variable arguments */
    va_start (ap, format);

    /* Call vsscanf(). Since we know that va_end won't do anything, we will
     * save the call and return the value directly.
     */
    return vsscanf (str, format, ap);
}



