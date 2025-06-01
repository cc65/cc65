/*
** Tests the CBM-specific directory functions.
**
** 2021-08-12, Greg King
*/

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <conio.h>
#include <cbm.h>


/* device number */
#define UNIT 8

/* directory patterm */
static const char name[] = "$";


static const char* const type[] = {
    "DEL",
    "CBM",
    "DIR",
    "LNK",
    "???",
    "Directory header",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "SEQ",
    "PRG",
    "USR",
    "REL",
    "VRP"
};

static const char* const access[] = {
    "unknown",
    "read-only",
    "write-only",
    "read/write"
};

static const char* const error[] = {
    "",
    "couldn't read it",
    "",
    "couldn't find start of file-name",
    "couldn't find end of file-name",
    "couldn't read file-type",
    "premature end of file"
};


int main(void)
{
    unsigned char go = 0;
    unsigned char rc;
    struct cbm_dirent E;

    /* Explain use, and wait for a key. */
    printf ("use the following keys:\n"
            "  g -> go ahead without stopping\n"
            "  q -> quit directory lister\n"
            "tap any key to start ...\n\n");
    cgetc ();

    /* Open the directory. */
    if (cbm_opendir (1, UNIT, name) != 0) {
        printf("error opening %s:\n %s\n", name, _stroserror (_oserror));
        return 1;
    }

    /* Output the directory. */
    printf("contents of \"%s\":\n", name);
    while ((rc = cbm_readdir (1, &E)) != 2) {
        if (rc != 0) {
            goto oops;
        }

        printf (" name[]: \"%s\"\n", E.name);
        printf (" size  :%6u\n",     E.size);
        printf (" type  : %s\n",     type[E.type]);
        printf (" access: %s\n",     access[E.access > 3 ? 0 : E.access]);
        printf ("----\n");

        if (!go) {
            switch (cgetc ()) {
              case 'q':
                goto done;

              case 'g':
                go = 1;
            }
        }
    }

    printf (" size  :%6u free.\n", E.size);

done:
    /* Close the directory. */
    cbm_closedir (1);
    return 0;

oops:
    if (rc <= 6) {
        printf ("\ndirectory error:\n %s.\n", error[rc]);
    }
    cbm_closedir (1);
    return 1;
}
