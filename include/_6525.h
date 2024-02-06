/*****************************************************************************/
/*                                                                           */
/*                                  _6525.h                                  */
/*                                                                           */
/*                Internal include file, do not use directly                 */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2000 Ullrich von Bassewitz                                       */
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



#ifndef __6525_H
#define __6525_H



/* Define a structure with the 6525 register offsets. The shadow registers
** (if port C is unused) are currently not implemented, we would need a
** union to do that, however that would introduce an additional name.
*/
struct __6525 {
    unsigned char       pra;            /* Port register A */
    unsigned char       prb;            /* Port register B */
    unsigned char       prc;            /* Port register C */
    unsigned char       ddra;           /* Data direction register A */
    unsigned char       ddrb;           /* Data direction register B */
    unsigned char       ddrc;           /* Data direction register C */
    unsigned char       cr;             /* Control register */
    unsigned char       air;            /* Active interrupt register */
};



/* End of _6525.h */
#endif



