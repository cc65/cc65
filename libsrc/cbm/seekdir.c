/*
** 2012-06-03, Ullrich von Bassewitz. Based on code by Groepaz.
** 2014-07-16, Greg King
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

    /* Make sure that we have a reasonable value for offs.  We reject
    ** negative numbers by converting them to (very high) unsigned values.
    */
    if ((unsigned long)offs > 0x1000uL) {
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
    o = dir->off = (unsigned)offs;
    while (o) {

        /* Determine size of next chunk to read */
        if (o > sizeof (buf)) {
            count = sizeof (buf);
            o -= sizeof (buf);
        } else {
            count = (unsigned char)o;
            o = 0;
        }

        /* Skip */
        if (!_dirread (dir, buf, count)) {
            return;
        }
    }
}



