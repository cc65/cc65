/*
** Internal include file, do not use directly.
** Written by Ullrich von Bassewitz. Based on code by Groepaz.
*/



#ifndef _DIR_H
#define _DIR_H



#include <dirent.h>



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



struct DIR {
    int         fd;             /* File descriptor for directory */
    unsigned    off;            /* Current byte offset in directory */
    char        name[16+1];     /* Name passed to opendir */
};



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



unsigned char __fastcall__ _dirread (DIR* dir, void* buf, unsigned char count);
/* Read characters from the directory into the supplied buffer. Makes sure,
** errno is set in case of a short read. Return true if the read was
** successful and false otherwise.
*/

unsigned char __fastcall__ _dirread1 (DIR* dir, void* buf);
/* Read one byte from the directory into the supplied buffer. Makes sure,
** errno is set in case of a short read. Return true if the read was
** successful and false otherwise.
*/



/* End of dir.h */
#endif



