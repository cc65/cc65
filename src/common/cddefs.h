/*****************************************************************************/
/*                                                                           */
/*                                 cddefs.h                                  */
/*                                                                           */
/*              Definitions for module constructor/destructors               */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000-2004 Ullrich von Bassewitz                                       */
/*               Roemerstrasse 52                                            */
/*               D-70794 Filderstadt                                         */
/* EMail:        uz@cc65.org                                                 */
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



#ifndef CDDEFS_H
#define CDDEFS_H



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* ConDes types. Count is only 7 because we want to encode 0..count in 3 bits */
#define CD_TYPE_COUNT   7               /* Number of table types */
#define CD_TYPE_MIN     0               /* Minimum numeric type value */
#define CD_TYPE_MAX     6               /* Maximum numeric type value */

/* ConDes priorities, zero is no valid priority and used to mark an empty
** (missing) decl for this type throughout the code.
*/
#define CD_PRIO_NONE    0               /* No priority (no decl) */
#define CD_PRIO_MIN     1               /* Lowest priority */
#define CD_PRIO_DEF     7               /* Default priority */
#define CD_PRIO_MAX     32              /* Highest priority */

/* Predefined types */
#define CD_TYPE_CON     0               /* Constructor */
#define CD_TYPE_DES     1               /* Destructor */
#define CD_TYPE_INT     2               /* Interruptor */

/* When part of an export in an object file, type and priority are encoded in
** one byte. In this case, the following macros access the fields:
*/
#define CD_GET_TYPE(v)          (((v) >> 5) & 0x07)
#define CD_GET_PRIO(v)          (((v) & 0x1F) + 1)

/* Macro to build the byte value: */
#define CD_BUILD(type,prio)     ((((type) & 0x07) << 5) | (((prio) - 1) & 0x1F))



/* End of cddefs.h */

#endif
