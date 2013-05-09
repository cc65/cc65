
/*
        first test for posix directory routines for the c64
        kludges:
        -   currently uses cbm_open, which conflicts with standard i/o,
                which in turn makes it infact kindof unuseable. this can
                be easily changed however, since the only reason not to use
                open/read was that it currently appends ,u,r to filenames
        -   the offset in current dir stream should better be calculated
                from the values returned by "read".
        -   the type flag isnt filled in atm.
        -   scandir/alphasort/versionsort is missing
        -   some bits are currently untested (ie, unused in the testprogram)
        27/02/2003 gpz
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <cbm.h>
#include <conio.h>


int main(void)
{
    char* name = ".";
    unsigned char go = 0;
    DIR *D;
    register struct dirent* E;

    /* Explain usage and wait for a key */
    printf ("Use the following keys:\n"
            "  g -> go ahead without stop\n"
            "  q -> quit directory listing\n"
            "  r -> return to last entry\n"
            "  s -> seek back to start\n"
            "Press any key to start ...\n");
    cgetc ();

    /* Open the directory */
    D = opendir (name);
    if (D == 0) {
        printf("error opening %s: %s\n", name, strerror (errno));
        return 1;
    }

    /* Output the directory */
    errno = 0;
    printf("contents of \"%s\":\n", name);
    while ((E = readdir (D)) != 0) {
        printf ("dirent.d_name[] : \"%s\"\n", E->d_name);
        printf ("dirent.d_blocks : %10u\n",   E->d_blocks);
        printf ("dirent.d_type   : %10d\n",   E->d_type);
        printf ("telldir()       : %10lu\n",  telldir (D));
        printf ("---\n");
        if (!go) {
            switch (cgetc ()) {
                case 'g':
                    go = 1;
                    break;

                case 'q':
                    goto done;

                case 'r':
                    seekdir (D, E->d_off);
                    break;

                case 's':
                    rewinddir (D);
                    break;

            }
        }
    }

done:
    if (errno == 0) {
        printf ("Done\n");
    } else {
        printf("Done: %d (%s)\n", errno, strerror (errno));
    }

    /* Close the directory */
    closedir (D);

    return 0;
}
