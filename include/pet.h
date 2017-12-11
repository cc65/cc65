/*****************************************************************************/
/*                                                                           */
/*                                   pet.h                                   */
/*                                                                           */
/*                      PET system specific definitions                      */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2005 Ullrich von Bassewitz                                       */
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



#ifndef _PET_H
#define _PET_H



/* Check for errors */
#if !defined(__PET__)
#  error This module may only be used when compiling for the CBM PET!
#endif



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Color defines */
#define COLOR_BLACK             0x00
#define COLOR_WHITE             0x01

/* Masks for joy_read */
#define JOY_UP_MASK             0x01
#define JOY_DOWN_MASK           0x02
#define JOY_LEFT_MASK           0x04
#define JOY_RIGHT_MASK          0x08
#define JOY_BTN_1_MASK          0x10

/* Define hardware */
#include <_pia.h>
#define PIA1    (*(struct __pia*)0xE810)
#define PIA2    (*(struct __pia*)0xE820)

#include <_6522.h>
#define VIA     (*(struct __6522*)0xE840)

/* All models from 40xx and above */
#include <_6545.h>
#define CRTC    (*(struct __6545)0xE880)

/* SuperPET only */
#include <_6551.h>
#define ACIA    (*(struct __6551*)0xEFF0)



/*****************************************************************************/
/*                                 Variables                                 */
/*****************************************************************************/



/* The addresses of the static drivers */
extern void pet_ptvjoy_joy[];
extern void pet_stdjoy_joy[];       /* Referred to by joy_static_stddrv[] */



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



/* The following #defines will cause the matching functions calls in conio.h
** to be overlaid by macros with the same names, saving the function call
** overhead.
*/
#define _textcolor(color)       COLOR_WHITE
#define _bgcolor(color)         COLOR_BLACK
#define _bordercolor(color)     COLOR_BLACK
#define _cpeekcolor(color)      COLOR_WHITE



/* End of pet.h */
#endif
