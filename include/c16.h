/*****************************************************************************/
/*                                                                           */
/*                                   c16.h                                   */
/*                                                                           */
/*                      C16 system specific definitions                      */
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



#ifndef _C16_H
#define _C16_H



/* Check for errors */
#if !defined(__C16__)
#  error This module may only be used when compiling for the C16!
#endif



/* Include the base header file for the 264 series. */
#include <cbm264.h>



/* The addresses of the static drivers */
extern void c16_ram_emd[];
extern void c16_stdjoy_joy[];  /* Referred to by joy_static_stddrv[] */



/* End of c16.h */
#endif
