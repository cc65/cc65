/*
** Ullrich von Bassewitz, 2012-05-30. Based on code by Groepaz.
*/



#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <cbm.h>
#include "dir.h"

#include <stdio.h>


struct dirent* __fastcall__ readdir (register DIR* dir)
{
    register unsigned char* b;
    register unsigned char i;
    register unsigned char count;
    static unsigned char s;
    static unsigned char j;
    unsigned char buffer[0x40];
    static struct dirent entry;


    /* Remember the directory offset for this entry */
    entry.d_off = dir->off;

    /* Skip the basic line-link */
    if (!_dirread (dir, buffer, 2)) {
        /* errno already set */
        goto exitpoint;
    }

    /* Read the number of blocks */
    if (!_dirread (dir, &entry.d_blocks, sizeof (entry.d_blocks))) {
        goto exitpoint;
    }

    /* Read the next file entry into the buffer */
    for (count = 0, b = buffer; count < sizeof (buffer); ++b) {
        if (!_dirread1 (dir, b)) {
            goto exitpoint;
        }
        ++count;
        if (*b == '\0') {
            break;
        }
    }

    /* Bump the directory offset and include the bytes for line-link and size */
    dir->off += count + 4;

    /* End of directory is reached if the buffer contains "blocks free". It is
    ** sufficient here to check for the leading 'b'. buffer will contain at
    ** least one byte if we come here.
    */
    if (buffer[0] == 'b') {
        goto exitpoint;
    }

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
                    /* End of file name found. */
                    entry.d_name[j] = '\0';
                    entry.d_namlen = j;
                    if (entry.d_off > 2) {
                        /* Proceed with file type */
                        s = 2;
                    } else {
                        /* This is a disk header, so we're done */
                        entry.d_type = _CBM_T_HEADER;
                        return &entry;
                    }
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
                    case 'i': entry.d_type = _CBM_T_DIR;        break;
                    default:  entry.d_type = _CBM_T_OTHER;      break;
                }
                return &entry;
        }
        ++i;
        ++b;
    }

    /* Something went wrong when parsing the directory entry */
    _errno = EIO;
exitpoint:
    return 0;
}



