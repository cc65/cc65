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



static char get (struct indesc* d)
/* Read a character from the input string and return it */
{
    char C;
    if (C = d->buf[d->ridx]) {
	/* Increment index only if end not reached */
	++d->ridx;
    }
    return C;
}



int sscanf (const char* str, const char* format, ...)
/* Standard C function */
{
    struct indesc id;
    va_list ap;

    /* Initialize the indesc struct. We leave all fields uninitialized that we
     * don't need
     */
    id.fin  = (infunc) get;
    id.buf  = (char*) str;
    id.ridx = 0;

    /* Setup for variable arguments */
    va_start (ap, format);

    /* Call the internal function. Since we know that va_end won't do anything,
     * we will save the call and return the value directly.
     */
    return _scanf (&id, format, ap);
}



