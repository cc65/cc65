/*
 * vsscanf.c
 *
 * (C) Copyright 2002 Ullrich von Bassewitz (uz@cc65.org)
 *
 */



#include <stdio.h>
#include "_scanf.h"



/*****************************************************************************/
/*     	    	     		     Code				     */
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



int vsscanf (const char* str, const char* format, va_list ap)
/* Standard C function */
{
    struct indesc id;

    /* Initialize the indesc struct. We leave all fields uninitialized that we
     * don't need
     */
    id.fin  = (infunc) get;
    id.buf  = (char*) str;
    id.ridx = 0;

    /* Call the internal function and return the result */
    return _scanf (&id, format, ap);
}



