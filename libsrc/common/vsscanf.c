/*
 * vsscanf.c
 *
 * (C) Copyright 2002 Ullrich von Bassewitz (uz@cc65.org)
 *
 */



#include <stdio.h>
#include "_scanf.h"



/*****************************************************************************/
/*     	    	     		     Data				     */
/*****************************************************************************/



struct sscanfdata {
    const char* str;            /* Pointer to input string */
    unsigned    index;          /* Read index */
};



/*****************************************************************************/
/*     	    	     		     Code				     */
/*****************************************************************************/



static int __fastcall__ get (struct sscanfdata* d)
/* Read a character from the input string and return it */
{
    char C;
    if (C = d->str[d->index]) {
    	/* Increment index only if end not reached */
       	++d->index;
        return C;
    } else {
        return EOF;
    }
}



static int __fastcall__ unget (int c, struct sscanfdata* d)
/* Push back a character onto the input stream */
{
    /* We do assume here that the _scanf routine will not push back anything
     * not read, so we can ignore c safely and won't check the index.
     */
    --d->index;
    return c;
}



int __fastcall__ vsscanf (const char* str, const char* format, va_list ap)
/* Standard C function */
{
    struct sscanfdata sd;
    struct scanfdata d;

    /* Initialize the data structs. The sscanfdata struct will be passed back
     * to the get and unget functions by _scanf.
     */
    d.get    = (getfunc) get;
    d.unget  = (ungetfunc) unget,
    d.data   = &sd;
    sd.str   = str;
    sd.index = 0;

    /* Call the internal function and return the result */
    return _scanf (&d, format, ap);
}



