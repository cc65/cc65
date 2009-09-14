/*****************************************************************************/
/*                                                                           */
/*                                 apple2.h				     */
/*                                                                           */
/*                   Apple ][ system specific definitions		     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000  Kevin Ruland, <kevin@rodin.wustl.edu>                           */
/* (C) 2003  Ullrich von Bassewitz, <uz@cc65.org>                            */
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



#ifndef _APPLE2_H
#define _APPLE2_H



/* Check for errors */
#if !defined(__APPLE2__) && !defined(__APPLE2ENH__)
#  error This module may only be used when compiling for the Apple ][!
#endif



/*****************************************************************************/
/*                                   Data				     */
/*****************************************************************************/



/* Color Defines */
#define COLOR_BLACK     0x00
#define COLOR_GREEN     0x01
#define COLOR_VIOLET    0x02
#define COLOR_WHITE     0x03
#define COLOR_BLACK2    0x04
#define COLOR_ORANGE    0x05
#define COLOR_BLUE      0x06
#define COLOR_WHITE2    0x07

#define LORES_BLACK     0x00
#define LORES_MAGENTA   0x01
#define LORES_DARKBLUE  0x02
#define LORES_VIOLET    0x03
#define LORES_DARKGREEN 0x04
#define LORES_GRAY      0x05
#define LORES_BLUE      0x06
#define LORES_CYAN      0x07
#define LORES_BROWN     0x08
#define LORES_ORANGE    0x09
#define LORES_GRAY2     0x0A
#define LORES_PINK      0x0B
#define LORES_GREEN     0x0C
#define LORES_YELLOW    0x0D
#define LORES_AQUA      0x0E
#define LORES_WHITE     0x0F

/* Characters codes */
#define CH_ENTER        0x0D
#define CH_ESC          0x1B
#define CH_CURS_LEFT    0x08
#define CH_CURS_RIGHT   0x15

#define CH_ULCORNER     '+'
#define CH_URCORNER     '+'
#define CH_LLCORNER     '+'
#define CH_LRCORNER     '+'
#define CH_TTEE         '+'
#define CH_BTEE         '+'
#define CH_LTEE         '+'
#define CH_RTEE         '+'
#define CH_CROSS        '+'

/* Return codes for get_ostype */
#define APPLE_UNKNOWN 0x00
#define APPLE_II      0x10              /* Apple ][			*/
#define APPLE_IIPLUS  0x11              /* Apple ][+			*/
#define APPLE_IIIEM   0x20              /* Apple /// (emulation)	*/
#define APPLE_IIE     0x30              /* Apple //e			*/
#define APPLE_IIEENH  0x31              /* Apple //e (enhanced)		*/
#define APPLE_IIECARD 0x40              /* Apple //e Option Card	*/
#define APPLE_IIC     0x50              /* Apple //c			*/
#define APPLE_IIC35   0x51              /* Apple //c (3.5 ROM)		*/
#define APPLE_IICEXP  0x53              /* Apple //c (Mem. Exp.)	*/
#define APPLE_IICREV  0x54              /* Apple //c (Rev. Mem. Exp.)	*/
#define APPLE_IICPLUS 0x55              /* Apple //c Plus		*/
#define APPLE_IIGS    0x80		/* Apple IIgs			*/
#define APPLE_IIGS1   0x81		/* Apple IIgs (ROM 1)		*/
#define APPLE_IIGS3   0x83		/* Apple IIgs (ROM 3)		*/

extern unsigned char _dos_type;
/* Valid _dos_type values:
 *
 * AppleDOS 3.3   - 0x00
 * ProDOS 8 1.0.1 - 0x10
 * ProDOS 8 1.0.2 - 0x10
 * ProDOS 8 1.1.1 - 0x11
 * ProDOS 8 1.2   - 0x12
 * ProDOS 8 1.3   - 0x13
 * ProDOS 8 1.4   - 0x14
 * ProDOS 8 1.5   - 0x15
 * ProDOS 8 1.6   - 0x16
 * ProDOS 8 1.7   - 0x17
 * ProDOS 8 1.8   - 0x18
 * ProDOS 8 1.9   - 0x18 (!)
 * ProDOS 8 2.0.1 - 0x21
 * ProDOS 8 2.0.2 - 0x22
 * ProDOS 8 2.0.3 - 0x23
 */



/*****************************************************************************/
/*                                   Code				     */
/*****************************************************************************/



unsigned char get_ostype (void);
/* Get the machine type. Returns one of the APPLE_xxx codes. */

void rebootafterexit (void);
/* Reboot machine after program termination has completed. */

/* The following #defines will cause the matching functions calls in conio.h
 * to be overlaid by macros with the same names, saving the function call
 * overhead.
 */
#define _textcolor(color)       COLOR_WHITE
#define _bgcolor(color)         COLOR_BLACK
#define _bordercolor(color)     COLOR_BLACK



/* End of apple2.h */
#endif
