/*
 * sscanf.c
 *
 * (C) Copyright 2001 Ullrich von Bassewitz (uz@cc65.org)
 *
 */



#include <stdio.h>
#include <string.h>
#include "_scanf.h"



/*****************************************************************************/
/*     	    	  		     Code				     */
/*****************************************************************************/



int sscanf (const char* str, const char* format, ...)
/* Standard C function */
{
    struct indesc id;
    va_list ap;

    /* Initialize the indesc struct. We leave all fields uninitialized that we
     * don't need
     */
    id.buf  = (char*) str;
    id.fill = strlen (str);

    /* Setup for variable arguments */
    va_start (ap, format);

    /* Call the internal function. Since we know that va_end won't do anything,
     * we will save the call and return the value directly.
     */
    return _scanf (&id, format, ap);
}



