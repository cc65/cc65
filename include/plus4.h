/*****************************************************************************/
/*                                                                           */
/*                                  plus4.h                                  */
/*                                                                           */
/*                    Plus/4 system specific definitions                     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2006, Ullrich von Bassewitz                                      */
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



#ifndef _PLUS4_H
#define _PLUS4_H



/* Check for errors */
#if !defined(__PLUS4__)
#  error This module may only be used when compiling for the Plus/4!
#endif



/* Include the base header file for the 264 series. */
#include <cbm264.h>

/* Define hardware */
#include <_6551.h>
#define ACIA    (*(struct __6551*)0xFD00)



/* The addresses of the static drivers */
extern void plus4_stdjoy_joy[];   /* Referred to by joy_static_stddrv[] */
extern void plus4_stdser_ser[];   /* Referred to by ser_static_stddrv[] */
extern void ted_hi_tgi[];

/* End of plus4.h */
#endif
