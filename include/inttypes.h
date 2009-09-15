/*****************************************************************************/
/*                                                                           */
/*                                inttypes.h                                 */
/*                                                                           */
/*                    Format conversion of integer types                     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2002      Ullrich von Bassewitz                                       */
/*               Wacholderweg 14                                             */
/*               D-70597 Stuttgart                                           */
/* EMail:        uz@musoftware.de                                            */
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



/* Note: This file is not fully ISO 9899-1999 compliant because cc65 lacks
 * a 64 bit data types and is not able to return structs > 4 bytes. The 
 * declarations have been adjusted accordingly.
 */



#ifndef _INTTYPES_H
#define _INTTYPES_H



/* inttypes.h must always include stdint.h */
#ifndef _STDINT_H
#include <stdint.h>
#endif



/* Return type of the imaxdiv function (which currently doesn't exist) */
typedef struct {
    intmax_t rem;
    intmax_t quot;
} ldiv_t;


/* End of inttypes.h */
#endif



