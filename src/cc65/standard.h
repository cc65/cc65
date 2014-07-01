/*****************************************************************************/
/*                                                                           */
/*                                standard.h                                 */
/*                                                                           */
/*                       Language standard definitions                       */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2004      Ullrich von Bassewitz                                       */
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



#ifndef STANDARD_H
#define STANDARD_H



/* common */
#include "intstack.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Supported standards. */
typedef enum standard_t {
    STD_UNKNOWN = -1,
    STD_C89,
    STD_C99,
    STD_CC65,

    /* Special constants */
    STD_COUNT,                  /* Number of supported standards */
    STD_DEFAULT = STD_CC65      /* Default standard if none given */
} standard_t;

/* Current language standard */
extern IntStack         Standard;               /* Language standard */



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



standard_t FindStandard (const char* Name);
/* Find a standard by name. Returns one of the constants defined above.
** STD_UNKNOWN is returned if Name doesn't match a standard.
*/



/* End of standard.h */

#endif
