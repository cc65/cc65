/*****************************************************************************/
/*                                                                           */
/*                                  sym1.h                                   */
/*                                                                           */
/*                     Sym-1 system-specific definitions                     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2020      Wayne Parham                                                */
/* EMail:        wayne@parhamdata.com                                        */
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



#ifndef _SYM1_H
#define _SYM1_H



/* Check for errors */
#if !defined(__SYM1__)
#  error This module may only be used when compiling for the Sym-1!
#endif



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Display character definitions */
#define DISP_1         0x06   // '1'
#define DISP_2         0x5B   // '2'
#define DISP_3         0x4F   // '3'
#define DISP_4         0x66   // '4'
#define DISP_5         0x6D   // '5'
#define DISP_6         0x7C   // '6'
#define DISP_7         0x07   // '7'
#define DISP_8         0x7F   // '8'
#define DISP_9         0x67   // '9'
#define DISP_0         0x3F   // '0'
#define DISP_A         0x77   // 'A'
#define DISP_b         0x7C   // 'b'
#define DISP_C         0x39   // 'C'
#define DISP_c         0x58   // 'c'
#define DISP_d         0x5E   // 'd'
#define DISP_E         0x79   // 'E'
#define DISP_e         0x7B   // 'e'
#define DISP_F         0x71   // 'F'
#define DISP_G         0x7D   // 'G'
#define DISP_g         0x6F   // 'g'
#define DISP_H         0x76   // 'H'
#define DISP_h         0x74   // 'h'
#define DISP_I         0x06   // 'I'
#define DISP_i         0x04   // 'i'
#define DISP_J         0x1E   // 'J'
#define DISP_K         0x74   // 'K'
#define DISP_L         0x38   // 'L'
#define DISP_M_1       0x33   // 'M'
#define DISP_M_2       0x27   // 2nd half
#define DISP_n         0x54   // 'n'
#define DISP_O         0x3F   // 'O'
#define DISP_o         0x5C   // 'o'
#define DISP_P         0x73   // 'P'
#define DISP_q         0x67   // 'q'
#define DISP_r         0x50   // 'r'
#define DISP_S         0x6D   // 'S'
#define DISP_t         0x46   // 't'
#define DISP_U         0x3E   // 'U'
#define DISP_u         0x1C   // 'u'
#define DISP_V_1       0x64   // 'V'
#define DISP_V_2       0x52   // 2nd half
#define DISP_W_1       0x3C   // 'W'
#define DISP_W_2       0x1E   // 2nd half
#define DISP_Y         0x6E   // 'Y'
#define DISP_Z         0x5B   // 'Z'
#define DISP_SPACE     0x00   // ' '
#define DISP_PERIOD    0x80   // '.'
#define DISP_HYPHEN    0x40   // '-'
#define DISP_APOSTR    0x20   // '''
#define DISP_EQUAL     0x41   // '='
#define DISP_3_BAR     0x49   // '='
#define DISP_BOTTOM    0x08   // '_'
#define DISP_TOP       0x01   // Top segment
#define DISP_LEFT      0x30   // '|' Left side, both segments
#define DISP_RIGHT     0x06   // '|' Right side, both segments
#define DISP_DEGREE    0x63   // 'o' An 'o' character in the upper segments
#define DISP_HAT       0x23   // 'n' An 'n' character in the upper segments
#define DISP_FORK      0x62   // 'u' A  'u' character in the upper segments
#define DISP_SLASH     0x51   // '/'
#define DISP_BACKSLASH 0x34   // '\'
#define DISP_TOP_RIGHT 0x02   // Top right segment
#define DISP_TOP_LEFT  0x20   // Top left segment
#define DISP_LOW_RIGHT 0x04   // Lower right segment
#define DISP_LOW_LEFT  0x10   // Lower left segment


/*****************************************************************************/
/*                                 Hardware                                  */
/*****************************************************************************/



#include <_6522.h>
#define VIA1    (*(struct __6522*)0xA000)    // U25
#define VIA2    (*(struct __6522*)0xA800)    // U28
#define VIA3    (*(struct __6522*)0xAC00)    // U29


struct _display {
    unsigned char d0;   // left-most seven-segment display
    unsigned char d1;   // second seven-segment display
    unsigned char d2;   // third seven-segment display
    unsigned char d3;   // fouth seven-segment display
    unsigned char d4;   // fifth seven-segment display
    unsigned char d5;   // sixth seven-segment display
    unsigned char d6;   // buffer byte to the right
};
#define DISPLAY (*(struct _display*)0xA640)



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



void beep (void);
/* Beep sound. */

void fdisp (void);
/* Flash display */

int __fastcall__ loadt (unsigned char);
/* Read from tape */

int __fastcall__ dumpt (unsigned char, const void*, const void*);
/* Write to tape */



/* End of sym1.h */
#endif
