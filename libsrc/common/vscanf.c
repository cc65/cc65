/*
 * vscanf.c
 *
 * Ullrich von Bassewitz (uz@cc65.org), 2004-11-26
 *
 */



#include <stdio.h>



/*****************************************************************************/
/*     	    	     		     Code				     */
/*****************************************************************************/



int __fastcall__ vscanf (const char* format, va_list ap)
/* Standard C function */
{
    return vfscanf (stdin, format, ap);
}



