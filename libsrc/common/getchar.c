/*
 * getchar.c
 *
 * Ullrich von Bassewitz, 11.12.1998
 */



#include <stdio.h>
#undef getchar		/* This is usually declared as a macro */



/*****************************************************************************/
/*     	    	     		     Code				     */
/*****************************************************************************/



int __fastcall__ getchar (void)
{
    return fgetc (stdin);
}



