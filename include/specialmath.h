/*****************************************************************************/
/*                                                                           */
/*                              specialmath.h                                */
/*                                                                           */
/*                 Optimized math routines for special usages                */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2019 Christian 'Irgendwer' Krueger                                    */
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

#ifndef _SPECIALMATH_H
#define _SPECIALMATH_H


/* Multiply an 8 bit unsigned value by 20 and return the 16 bit unsigned result */

unsigned int __fastcall__ _mul20(unsigned char value);


/* Multiply an 8 bit unsigned value by 40 and return the 16 bit unsigned result */

unsigned int __fastcall__ _mul40(unsigned char value);



/* End of specialmath.h */
#endif
