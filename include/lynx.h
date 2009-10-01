/*****************************************************************************/
/*                                                                           */
/*                                  lynx.h                                   */
/*                                                                           */
/*                     Lynx system specific definitions                      */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2003      Shawn Jefferson                                             */
/*                                                                           */
/* Adapted with many changes Ullrich von Bassewitz, 2004-10-09               */
/*                                                                           */
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



#ifndef _LYNX_H
#define _LYNX_H



/* Check for errors */
#if !defined(__LYNX__)
#  error This module may only be used when compiling for the Lynx game console!
#endif



/*****************************************************************************/
/*	       	   	  	     Data				     */
/*****************************************************************************/



/* Color defines (default palette) */
#define COLOR_BLACK             0x00
#define COLOR_RED               0x01
#define COLOR_BLUE              0x02
#define COLOR_GREEN             0x03
#define COLOR_VIOLET            0x04
#define COLOR_PURPLE            COLOR_VIOLET
#define COLOR_CYAN              0x05
#define COLOR_PEAGREEN          0x06
#define COLOR_GREY              0x07
#define COLOR_NAVYBLUE          0x08
#define COLOR_LIGHTRED          0x09
#define COLOR_LIGHTBLUE         0x0A
#define COLOR_LIGHTGREEN        0x0B
#define COLOR_LIGHTPURPLE       0x0C
#define COLOR_LIGHTCYAN         0x0D
#define COLOR_YELLOW            0x0E
#define COLOR_WHITE             0x0F



/*****************************************************************************/
/*	       	   		     Code				     */
/*****************************************************************************/



void __fastcall__ lynx_change_framerate (unsigned char rate);
/* Change the framerate, in Hz. Recognized values are 50, 60 and 75. */



/*****************************************************************************/
/*                           Accessing the EEPROM                            */
/*****************************************************************************/



unsigned __fastcall__ lynx_eeprom_read (unsigned char cell);
/* Read a 16 bit word from the given address */

unsigned __fastcall__ lynx_eeprom_write (unsigned char cell, unsigned val);
/* Write the word at the given address */

void __fastcall__ lynx_eeprom_erase (unsigned char cell);
/* Clear the word at the given address */


/*****************************************************************************/
/*                           TGI extras                                      */
/*****************************************************************************/

#define tgi_sprite(spr) tgi_ioctl(0, (unsigned)(spr))
#define tgi_flip() tgi_ioctl(1, 0)
#define tgi_setbgcolor(bgcol) tgi_ioctl(2, (unsigned)(bgcol))
#define tgi_setframerate(rate) tgi_ioctl(3, (unsigned)(rate))
#define tgi_busy() tgi_ioctl(4, 0)
#define tgi_updatedisplay() tgi_ioctl(4, 1)

/* End of lynx.h */
#endif



