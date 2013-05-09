/*****************************************************************************/
/*                                                                           */
/*                               rewinddir.c                                 */
/*                                                                           */
/*                          Reset directory stream                           */
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



#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include "dir.h"



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



void __fastcall__ rewinddir (register DIR* dir)
{
    /* Rewind directory file */
    if (lseek (dir->fd, 0, SEEK_SET)) {

        /* Read directory key block */
        if (read (dir->fd,
                  dir->block.bytes,
                  sizeof (dir->block)) == sizeof (dir->block)) {

            /* Skip directory header entry */
            dir->current_entry = 1;

            /* Return success */
            return;
        }
    }

    /* Assert that no subsequent readdir() finds an active entry */
    memset (dir->block.bytes, 0, sizeof (dir->block));

    /* Return failure */
}
