/*
** Ullrich von Bassewitz, 2012-06-03. Based on code by Groepaz.
*/

#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include "dir.h"



void __fastcall__ seekdir (register DIR* dir, long offs)
{
    unsigned      o;
    unsigned char count;
    unsigned char buf[128];

    /* Make sure we have a reasonable value for offs */
    if (offs > 0x1000) {
        errno = EINVAL;
        return;
    }

    /* Close the directory file descriptor */
    close (dir->fd);

    /* Reopen it using the old name */
    dir->fd = open (dir->name, O_RDONLY);
    if (dir->fd < 0) {
        /* Oops! */
        return;
    }

    /* Skip until we've reached the target offset in the directory */
    o = dir->off = offs;
    while (o) {

        /* Determine size of next chunk to read */
        if (o > sizeof (buf)) {  
            count = sizeof (buf);
            o -= sizeof (buf);
        } else {
            count = offs;
            o = 0;
        }

        /* Skip */
        if (!_dirread (dir, buf, count)) {
            return;
        }
    }
}



