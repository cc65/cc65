/*
 * putchar.c
 *
 * Ullrich von Bassewitz, 11.12.1998
 */



#include <stdio.h>
#undef putchar	  	/* This is usually declared as a macro */



/*****************************************************************************/
/*     	    	     		     Code				     */
/*****************************************************************************/



int __fastcall__ putchar (int c)
{
    return fputc (c, stdout);
}



