/*****************************************************************************/
/*                                                                           */
/*				     pet.h                                   */
/*                                                                           */
/*		        PET system specific definitions			     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2004 Ullrich von Bassewitz                                       */
/*               Römerstraße 52                                              */
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



/* Color defines */
#define COLOR_BLACK  	       	0x00
#define COLOR_WHITE  	       	0x01



/* Define hardware */
#include <_pia.h>
#define PIA1	(*(struct __pia*)0xE810)
#define PIA2	(*(struct __pia*)0xE820)

#include <_6522.h>
#define VIA    	(*(struct __6522*)0xE840)



/* End of pet.h */
#endif



