/*
 * strdup.c
 *
 * Ullrich von Bassewitz, 11.08.1998
 */



#include <string.h>
#include <stdlib.h>



char* strdup (char* s)
{
    char* p;
    p = malloc (strlen (s) + 1);
    if (p) {
	strcpy (p, s);
    }
    return p;
}



