
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
#include <conio.h>

#if defined(__CBM__)
#include <cbm.h>
#elif defined(__APPLE2__)
#include <apple2.h>
#endif

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
#if !defined(__ATARI__)
        printf ("dirent.d_blocks : %10u\n",   E->d_blocks);
#endif
        printf ("dirent.d_type   : %10d\n",   E->d_type);
#if !defined(__APPLE2__) && !defined(__ATARI__)
        printf ("telldir()       : %10lu\n",  telldir (D));
#endif
        printf ("---\n");
        if (!go) {
            switch (cgetc ()) {
                case 'g':
                    go = 1;
                    break;

                case 'q':
                    goto done;
#if !defined(__APPLE2__) && !defined(__ATARI__)
                case 'r':
                    seekdir (D, E->d_off);
                    break;
#endif
#if !defined(__ATARI__)
                case 's':
                    rewinddir (D);
                    break;
#endif

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
