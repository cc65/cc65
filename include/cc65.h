/*****************************************************************************/
/*                                                                           */
/*                                  cc65.h                                   */
/*                                                                           */
/*          Target independent but cc65 specific utility functions           */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2009,      Ullrich von Bassewitz                                      */
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



#ifndef _CC65_H
#define _CC65_H



/*****************************************************************************/
/*	       	   		     Code				     */
/*****************************************************************************/



unsigned long __fastcall__ cc65_umul16x16r32 (unsigned rhs, unsigned lhs);
/* Multiplicate two unsigned 16 bit to yield an unsigned 32 bit result */

int __fastcall__ cc65_sin (unsigned x);
/* Return the sine of the argument, which must be in range 0..360. The result
 * is in 8.8 fixed point format, which means that 1.0 = $100 and -1.0 = $FF00.
 */

int __fastcall__ cc65_cos (unsigned x);
/* Return the cosine of the argument, which must be in range 0..360. The result
 * is in 8.8 fixed point format, which means that 1.0 = $100 and -1.0 = $FF00.
 */



/* End of cc65.h */
#endif



