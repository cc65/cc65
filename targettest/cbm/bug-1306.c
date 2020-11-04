#include <stdio.h>
#include <stdlib.h>

#include <cbm.h>

void
main ()
{
    char *  data = malloc (1024);
    int     len;

    cbm_open (8, 8, 8, "read.c");

    //cbm_read() always returns at end of file.
    while ((len = cbm_read (8, data, 1024)) > 0)
        printf ("%d ", len);

    /* NOEXIT */
}
