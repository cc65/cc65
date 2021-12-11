#include <stdio.h>
#include <stdlib.h>
#include <sys/utsname.h>


int main (void)
{                  
    /* Get the uname data */
    struct utsname buf;
    if (uname (&buf) != 0) {
        perror ("uname");
        return EXIT_FAILURE;
    }

    /* Print it */      
    printf ("sysname:  \"%s\"\n", buf.sysname);
    printf ("nodename: \"%s\"\n", buf.nodename);
    printf ("release:  \"%s\"\n", buf.release);
    printf ("version:  \"%s\"\n", buf.version);
    printf ("machine:  \"%s\"\n", buf.machine);

    /* Done */
    return EXIT_SUCCESS;
}


