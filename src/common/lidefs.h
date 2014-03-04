/*****************************************************************************/
/*                                                                           */
/*                                 lidefs.h                                  */
/*                                                                           */
/*                     Definitions for line information                      */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2011,      Ullrich von Bassewitz                                      */
/*                Roemerstrasse 52                                           */
/*                70794 Filderstadt                                          */
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



#ifndef LIDEFS_H
#define LIDEFS_H



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Types of line infos. */
#define LI_TYPE_ASM             0U      /* Normal assembler source */
#define LI_TYPE_EXT             1U      /* Externally supplied line info */
#define LI_TYPE_MACRO           2U      /* Macro expansion */
#define LI_TYPE_MACPARAM        3U      /* Macro parameter expansion */

/* Make a combined value from type and count */
#define LI_MAKE_TYPE(T,C)       ((T) | (((unsigned)(C)) << 2U))

/* Extract type and count from a type field */
#define LI_GET_TYPE(T)          ((T) & 0x03U)
#define LI_GET_COUNT(T)         ((unsigned)(T) >> 2)



/* End of lidefs.h */

#endif
