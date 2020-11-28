#include <stdio.h>
#include <stdlib.h>

#include <cbm.h>

void
test (char * what, char * filename, int len)
{
    static char data[1024];
    int rlen;

    printf ("%s", what);

    cbm_open (8, 8, 8, filename);
    rlen = cbm_read (8, data, 1024);
    if (rlen != 1) {
        printf ("\nError: returned %d instead of %d", rlen, len);
        exit (255);
    }
    cbm_close (8);

    printf (" OK\n");
}

void
main ()
{
    test ("Read empty file...", "read-0", 1);
    test ("Read file with 1 byte...", "read-1", 1);
    test ("Read file with 8 bytes...", "read-8", 8);

    printf ("Tests passed.");
}
