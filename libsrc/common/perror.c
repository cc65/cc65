/*
 * perror.c
 *
 * Ullrich von Bassewitz, 01.10.1998
 */



#include <stdio.h>
#include <string.h>
#include <errno.h>



void perror (const char* msg)
{			  
    if (msg) {		      
        fprintf (stderr, "%s: ", msg);
    }
    fprintf (stderr, "%s\n", strerror (errno));
}



