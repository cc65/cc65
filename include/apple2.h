/*****************************************************************************/
/*									     */
/*				   apple2.h				     */
/*									     */
/*		     Apple ][ system specific definitions		     */
/*									     */
/*									     */
/*									     */
/* (C) 2000  Kevin Ruland, <kevin@rodin.wustl.edu>			     */
/* (C) 2003  Ullrich von Bassewitz, <uz@cc65.org>			     */
/*									     */
/*									     */
/* This software is provided 'as-is', without any expressed or implied	     */
/* warranty.  In no event will the authors be held liable for any damages    */
/* arising from the use of this software.				     */
/*									     */
/* Permission is granted to anyone to use this software for any purpose,     */
/* including commercial applications, and to alter it and redistribute it    */
/* freely, subject to the following restrictions:			     */
/*									     */
/* 1. The origin of this software must not be misrepresented; you must not   */
/*    claim that you wrote the original software. If you use this software   */
/*    in a product, an acknowledgment in the product documentation would be  */
/*    appreciated but is not required.					     */
/* 2. Altered source versions must be plainly marked as such, and must not   */
/*    be misrepresented as being the original software. 		     */
/* 3. This notice may not be removed or altered from any source 	     */
/*    distribution.							     */
/*									     */
/*****************************************************************************/



#ifndef _APPLE2_H
#define _APPLE2_H



/* Check for errors */
#if !defined(__APPLE2__) && !defined(__APPLE2ENH__)
#  error This module may only be used when compiling for the Apple ][!
#endif



/*****************************************************************************/
/*				     Data				     */
/*****************************************************************************/



/* Color Defines
 * Since Apple2 does not support color text these defines are only
 * used to get the library to compile correctly.  They should not be used
 * in user code
 */
#define COLOR_BLACK	0x00
#define COLOR_WHITE	0x01

/* Characters codes */
#define CH_ENTER	0x0D
#define CH_ESC		0x1B
#define CH_CURS_LEFT	0x08
#define CH_CURS_RIGHT	0x15

#define CH_ULCORNER	'+'
#define CH_URCORNER	'+'
#define CH_LLCORNER	'+'
#define CH_LRCORNER	'+'
#define CH_TTEE 	'+'
#define CH_BTEE 	'+'
#define CH_LTEE 	'+'
#define CH_RTEE 	'+'
#define CH_CROSS	'+'

/* Return codes for get_ostype */
#define APPLE_UNKNOWN 0x00
#define APPLE_II      0x10		/* Apple ][			*/
#define APPLE_IIPLUS  0x11		/* Apple ][+			*/
#define APPLE_IIIEM   0x20		/* Apple /// (emulation)	*/
#define APPLE_IIE     0x30		/* Apple //e			*/
#define APPLE_IIEENH  0x31		/* Apple //e (enhanced)		*/
#define APPLE_IIECARD 0x40		/* Apple //e Option Card	*/
#define APPLE_IIC     0x50		/* Apple //c			*/
#define APPLE_IIC35   0x51		/* Apple //c (3.5 ROM)		*/
#define APPLE_IICEXP  0x53		/* Apple //c (Mem. Exp.)	*/
#define APPLE_IICREV  0x54		/* Apple //c (Rev. Mem. Exp.)	*/
#define APPLE_IICPLUS 0x55		/* Apple //c Plus		*/
#define	APPLE_IIGS    0x80		/* Apple IIgs			*/
#define	APPLE_IIGS1   0x81		/* Apple IIgs (ROM 1)		*/
#define	APPLE_IIGS3   0x83		/* Apple IIgs (ROM 3)		*/


/*****************************************************************************/
/*				     Code				     */
/*****************************************************************************/



unsigned char get_ostype (void);
/* Get the machine type. Returns one of the APPLE_xxx codes. */



/* End of apple2.h */
#endif
