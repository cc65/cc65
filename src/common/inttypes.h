/*****************************************************************************/
/*                                                                           */
/*                                inttypes.h                                 */
/*                                                                           */
/*                           Define integer types                            */
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



#ifndef INTTYPES_H
#define INTTYPES_H



/* If we have stdint.h, include it, otherwise try some quesswork on types.
** gcc and msvc don't define __STDC_VERSION__ without special flags, so check
** for them explicitly. Undefined symbols are replaced by zero, so a check for
** defined(__GNUC__) or defined(_MSC_VER) is not necessary.
*/
#if (__STDC_VERSION__ >= 199901) || (__GNUC__ >= 3) || (_MSC_VER >= 1600)
#include <stdint.h>
#else

/* Assume long is the largest type available, and assume that pointers can be
** safely converted into this type and back.
*/
typedef long intptr_t;
typedef unsigned long uintptr_t;
typedef long intmax_t;
typedef unsigned long uintmax_t;



#endif



/* End of inttypes.h */

#endif
