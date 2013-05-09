/*****************************************************************************/
/*                                                                           */
/*                                readdir.c                                  */
/*                                                                           */
/*                           Read directory entry                            */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2005  Oliver Schmidt, <ol.sc@web.de>                                  */
/*                                                                           */
/*                                                                           */
/* This software is provided 'as-is', without any expressed or implied       */
/* warranty.  In no event will the authors be held liable for any damages    */
/* arising from the use of this software.                                    */
/*                                                                           */
/* Permission is granted to anyone to use this software for any purpose,     */
/* including commercial applications, and to alter it and redistribute it    */
/* freely, subject to the following restrictions:                            */
/*                                                                           */
/* 1. The origin of this software must not be misrepresented; you must not   */
/*    claim that you wrote the original software. If you use this software   */
/*    in a product, an acknowledgment in the product documentation would be  */
/*    appreciated but is not required.                                       */
/* 2. Altered source versions must be plainly marked as such, and must not   */
/*    be misrepresented as being the original software.                      */
/* 3. This notice may not be removed or altered from any source              */
/*    distribution.                                                          */
/*                                                                           */
/*****************************************************************************/



#include <stddef.h>
#include <unistd.h>
#include <dirent.h>
#include "dir.h"



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



struct dirent* __fastcall__ readdir (register DIR* dir)
{
    register unsigned char* entry;

    /* Search for the next active directory entry */
    do {

        /* Read next directory block if necessary */
        if (dir->current_entry == dir->entries_per_block) {
            if (read (dir->fd,
                      dir->block.bytes,
                      sizeof (dir->block)) != sizeof (dir->block)) {

                /* Just return failure as read() has */
                /* set errno if (and only if) no EOF */
                return NULL;
            }

            /* Start with first entry in next block */
            dir->current_entry = 0;
        }

        /* Compute pointer to current entry */
        entry = dir->block.content.entries +
                dir->current_entry * dir->entry_length;

        /* Switch to next entry */
        ++dir->current_entry;
    } while (entry[0x00] == 0);

    /* Move creation date/time to allow for next step below */
    *(unsigned long*)&entry[0x1A] = *(unsigned long*)&entry[0x18];

    /* Feature unsigned long access to EOF by extension from 3 to 4 bytes */
    entry[0x18] = 0;

    /* Move file type to allow for next step below */
    entry[0x19] = entry[0x10];

    /* Zero-terminate file name */
    entry[0x01 + (entry[0x00] & 0x0F)] = 0;

    /* Return success */
    return (struct dirent*)&entry[0x01];
}
