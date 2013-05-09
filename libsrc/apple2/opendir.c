/*****************************************************************************/
/*                                                                           */
/*                                opendir.h                                  */
/*                                                                           */
/*                             Open a directory                              */
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
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <dirent.h>
#include "dir.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



extern char _cwd[FILENAME_MAX];



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



DIR* __fastcall__ opendir (register const char* name) 
{
    register DIR* dir;

    /* Alloc DIR */
    if ((dir = malloc (sizeof (*dir))) == NULL) {

        /* May not have been done by malloc() */
        _directerrno (ENOMEM);

        /* Return failure */
        return NULL;
    }

    /* Interpret dot as current working directory */
    if (*name == '.') {
        name = _cwd;
    }

    /* Open directory file */
    if ((dir->fd = open (name, O_RDONLY)) != -1) {

        /* Read directory key block */
        if (read (dir->fd,
                  dir->block.bytes,
                  sizeof (dir->block)) == sizeof (dir->block)) {

            /* Get directory entry infos from directory header */
            dir->entry_length      = dir->block.bytes[0x23];
            dir->entries_per_block = dir->block.bytes[0x24];

            /* Skip directory header entry */
            dir->current_entry = 1;

            /* Return success */
            return dir;
        }

        /* EOF: Most probably no directory file at all */
        if (_oserror == 0) {
            _directerrno (EINVAL);
        }

        /* Cleanup directory file */
        close (dir->fd);
    }

    /* Cleanup DIR */
    free (dir);

    /* Return failure */
    return NULL;
}
