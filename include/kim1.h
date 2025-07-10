/*****************************************************************************/
/*                                                                           */
/*                                  kim1.h                                   */
/*                                                                           */
/*                     KIM-1 system-specific definitions                     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2022      Dave Plummer                                                */
/* Email:        davepl@davepl.com                                           */
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

#ifndef _KIM1_H
#define _KIM1_H

/* Check for errors */
#if !defined(__KIM1__)
#  error This module may only be used when compiling for the KIM-1!
#endif



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/

/*****************************************************************************/
/*                                 Hardware                                  */
/*****************************************************************************/

/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/

/* Read from tape */
int __fastcall__ loadt (unsigned char);

/* Write to tape */
int __fastcall__ dumpt (unsigned char, const void*, const void*);


/* Write to 7-segment LED display. Due to hardware limitations it only
** displays briefly, so must be called repeatedly to update the
** display.
**/
void __fastcall__ scandisplay(unsigned char left, unsigned char middle, unsigned char right);

/*
** Get a keypress from the keypad. Returns $00-$0F(0-F), $10(AD), $11(DA), $12(+),
** $13(GO), $14(PC) or $15 for no keypress.
**/
int __fastcall__ getkey();

/* End of kim1.h */
#endif
