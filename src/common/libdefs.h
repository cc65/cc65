/*****************************************************************************/
/*                                                                           */
/*                                 libdefs.h                                 */
/*                                                                           */
/*                         Library file definitions                          */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2011, Ullrich von Bassewitz                                      */
/*                Roemerstrasse 52                                           */
/*                D-70794 Filderstadt                                        */
/* EMail:         uz@cc65.org                                                */
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



#ifndef LIBDEFS_H
#define LIBDEFS_H



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Defines for magic and version */
#define LIB_MAGIC       0x7A55616E
#define LIB_VERSION     0x000D

/* Size of an library file header */
#define LIB_HDR_SIZE    12



/* Header structure for the library */
typedef struct LibHeader LibHeader;
struct LibHeader {
    unsigned long       Magic;          /* 32: Magic number */
    unsigned            Version;        /* 16: Version number */
    unsigned            Flags;          /* 16: flags */
    unsigned long       IndexOffs;      /* 32: Offset to directory */
};



/* End of libdefs.h */

#endif
