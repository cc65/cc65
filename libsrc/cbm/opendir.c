/*
** Ullrich von Bassewitz, 2012-05-30. Based on code by Groepaz.
*/

#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include "dir.h"



DIR* __fastcall__ opendir (register const char* name)
{
    unsigned char buf[2];
    DIR* dir = 0;
    DIR d;

    /* Setup the actual file name that is sent to the disk. We accept "0:",
    ** "1:" and "." as directory names.
    */
    d.name[0] = '$';
    if (name == 0 || name[0] == '\0' || (name[0] == '.' && name[1] == '\0')) {
        d.name[1] = '\0';
    } else if ((name[0] == '0' || name[0] == '1') && name[1] == ':' && name[2] == '\0') {
        d.name[1] = name[0];
        d.name[2] = '\0';
    } else {
        errno = EINVAL;
        goto exitpoint;
    }

    /* Set the offset of the first entry */
    d.off = sizeof (buf);

    /* Open the directory on disk for reading */
    d.fd = open (d.name, O_RDONLY);
    if (d.fd >= 0) {

        /* Skip the load address */         
        if (_dirread (&d, buf, sizeof (buf))) {

            /* Allocate memory for the DIR structure returned */
            dir = malloc (sizeof (*dir));

            /* Copy the contents of d */
            if (dir) {
                memcpy (dir, &d, sizeof (d));
            } else {
                /* Set an appropriate error code */
                errno = ENOMEM;
            }
        }
    }

exitpoint:
    /* Done */
    return dir;
}



