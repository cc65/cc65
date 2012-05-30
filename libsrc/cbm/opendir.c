#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include "dir.h"



DIR* __fastcall__ opendir (const char*)
{
    unsigned char buffer[8+16+1+7];
    int count;
    DIR d;
    DIR* dir = 0;

    /* Setup file name and offset */
    d.name[0] = '$';
    d.name[1] = '\0';
    d.offs    = 0;

    /* Open the directory on disk for reading */
    d.fd = open (d.name, O_RDONLY);
    if (d.fd >= 0) {

        /* Skip the disk header */
        count = read (d.fd, buffer, sizeof (buffer));
        if (count == sizeof (buffer)) {

            /* Allocate memory for the DIR structure returned */
            dir = malloc (sizeof (*dir));

            /* Copy the contents of d */
            if (dir) {
                memcpy (dir, &d, sizeof (d));
            } else {
                /* Set an appropriate error code */
                errno = ENOMEM;
            }
        } else if (count >= 0) {
            /* Short read - need to set an error code */
            errno = EIO;
        }
    }

    /* Done */
    return dir;
}



