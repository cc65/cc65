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
    /* Fetch the message that corresponds to errno */
    const char* errormsg = strerror (errno);

    /* Different output depending on msg */
    if (msg) {
        fprintf (stderr, "%s: %s\n", msg, errormsg);
    } else {
        fprintf (stderr, "%s\n", errormsg);
    }
}



