/*
 * Ullrich von Bassewitz, 2012-05-30. Based on code by Groepaz.
 */



#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <cbm.h>
#include "dir.h"

#include <stdio.h>


struct dirent* __fastcall__ readdir (register DIR* dir)
{
    static struct dirent entry;
    register unsigned char* b;
    register unsigned char i;
    unsigned char count;
    unsigned char j;
    unsigned char s;
    unsigned char buffer[0x40];


    /* Remember the directory offset for this entry */
    entry.d_off = dir->off;

    /* Skip the basic line-link */
    if (!_dirskip (2, dir)) {
        /* errno already set */
        goto exit;
    }
    dir->off += 2;

    /* Read the number of blocks */
    if (read (dir->fd, &entry.d_blocks, sizeof (entry.d_blocks)) !=
        sizeof (entry.d_blocks)) {
        goto error;
    }

    /* Read the next file entry into the buffer */
    for (count = 0, b = buffer; count < sizeof (buffer); ++count, ++b) {
        if (read (dir->fd, b, 1) != 1) {
            /* ### Check for EOF */
            goto error;
        }
        if (*b == '\0') {
            break;
        }
    }

    /* End of directory is reached if the buffer contains "blocks free". It is
     * sufficient here to check for the leading 'b'. To avoid problems if we're
     * called again, read until end of directory.
     */
    if (count > 0 && buffer[0] == 'b') {
        while (read (dir->fd, buffer, 1) == 1) ;
        return 0;
    }

    /* Bump the directory offset */
    dir->off += count;

    /* Parse the buffer for the filename and file type */
    i = 0;
    j = 0;
    s = 0;
    b = buffer;
    while (i < count) {
        switch (s) {

            case 0:
                /* Searching for start of file name */
                if (*b == '"') {
                    s = 1;
                }
                break;

            case 1:
                /* Within file name */
                if (*b == '"') {
                    entry.d_name[j] = '\0';
                    entry.d_namlen = j;
                    s = 2;
                } else if (j < sizeof (entry.d_name) - 1) {
                    entry.d_name[j] = *b;
                    ++j;
                }
                break;

            case 2:
                /* Searching for file type */
                if (*b != ' ') {
                    entry.d_type = _cbm_filetype (*b);
                    if (*b == 'd') {
                        /* May be DEL or DIR, check next char */
                        s = 3;
                    } else {
                        /* Done */
                        return &entry;
                    }
                }
                break;

            case 3:
                /* Distinguish DEL or DIR file type entries */
                switch (*b) {
                    case 'e':                                   break;
                    case 'i': entry.d_type = CBM_T_DIR;         break;
                    default:  entry.d_type = CBM_T_OTHER;       break;
                }
                return &entry;
        }
        ++i;
        ++b;
    }

    /* Something went wrong - terminating quote not found */
error:
    _errno = EIO;
exit:
    return 0;
}



