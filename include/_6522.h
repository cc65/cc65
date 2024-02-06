/*****************************************************************************/
/*                                                                           */
/*                                  _6522.h                                  */
/*                                                                           */
/*                Internal include file, do not use directly                 */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2004 Stefan Haubenthal <polluks@sdf.lonestar.org>                     */
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



#ifndef __VIA_H
#define __VIA_H



/* Define a structure with the 6522 register offsets.
** NOTE: The timer registers are not declared as 16 bit registers, because
** the order in which the two 8 bit halves are written is important, and
** the compiler doesn't guarantee any order when writing 16 bit values.
*/
struct __6522 {
    unsigned char       prb;            /* Port register B */
    unsigned char       pra;            /* Port register A */
    unsigned char       ddrb;           /* Data direction register B */
    unsigned char       ddra;           /* Data direction register A */
    unsigned char       t1_lo;          /* Timer 1, low byte */
    unsigned char       t1_hi;          /* Timer 1, high byte */
    unsigned char       t1l_lo;         /* Timer 1 latch, low byte */
    unsigned char       t1l_hi;         /* Timer 1 latch, high byte */
    unsigned char       t2_lo;          /* Timer 2, low byte */
    unsigned char       t2_hi;          /* Timer 2, high byte */
    unsigned char       sr;             /* Shift register */
    unsigned char       acr;            /* Auxiliary control register */
    unsigned char       pcr;            /* Peripheral control register */
    unsigned char       ifr;            /* Interrupt flag register */
    unsigned char       ier;            /* Interrupt enable register */
    unsigned char       pra2;           /* Port register A w/o handshake */
};



/* End of _6522.h */
#endif



