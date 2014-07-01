/*****************************************************************************/
/*                                                                           */
/*                                addrsize.h                                 */
/*                                                                           */
/*                         Address size definitions                          */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2003-2009, Ullrich von Bassewitz                                      */
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



#ifndef ADDRSIZE_H
#define ADDRSIZE_H



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



#define ADDR_SIZE_INVALID       0xFF
#define ADDR_SIZE_DEFAULT       0x00
#define ADDR_SIZE_ZP            0x01
#define ADDR_SIZE_ABS           0x02
#define ADDR_SIZE_FAR           0x03
#define ADDR_SIZE_LONG          0x04



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



const char* AddrSizeToStr (unsigned char AddrSize);
/* Return the name for an address size specifier */

unsigned char AddrSizeFromStr (const char* Str);
/* Return the address size for a given string. Returns ADDR_SIZE_INVALID if
** the string cannot be mapped to an address size.
*/



/* End of addrsize.h */

#endif
