/*****************************************************************************/
/*                                                                           */
/*				  joystick.h				     */
/*                                                                           */
/*		 Read the joystick on systems that support it		     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2001 Ullrich von Bassewitz                                       */
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



#ifndef _JOYSTICK_H
#define _JOYSTICK_H



/* Define __JOYSTICK__ for systems that support a joystick */
#if defined(__ATARI__) || defined(__C64__) || defined(__C128__) || defined(__PLUS4__) || defined(__CBM510__)
#  define __JOYSTICK__
#else
#  error The target system does not support a joystick!
# endif

/* Argument for the function */
#define JOY_1		0
#define JOY_2		1

/* Result codes of the function. The actual code is a bitwise or
 * of one or more of the following values.
 */
#define JOY_UP       	0x01
#define JOY_DOWN	0x02
#define JOY_LEFT	0x04
#define JOY_RIGHT	0x08
#define JOY_FIRE	0x10



unsigned __fastcall__ readjoy (unsigned char joy);
/* Read the joystick. The argument is one of JOY_1/JOY2 */



/* End of joystick.h */
#endif



