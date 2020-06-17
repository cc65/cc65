/*****************************************************************************/
/*                                                                           */
/*                                   c64.h                                   */
/*                                                                           */
/*                      C64 system-specific definitions                      */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2013 Ullrich von Bassewitz                                       */
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



#ifndef _C64_H
#define _C64_H



/* Check for errors */
#if !defined(__C64__)
#  error This module may only be used when compiling for the C64!
#endif



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Additional key defines */
#define CH_F1                   133
#define CH_F2                   137
#define CH_F3                   134
#define CH_F4                   138
#define CH_F5                   135
#define CH_F6                   139
#define CH_F7                   136
#define CH_F8                   140

/* Color defines */
#define COLOR_BLACK             0x00
#define COLOR_WHITE             0x01
#define COLOR_RED               0x02
#define COLOR_CYAN              0x03
#define COLOR_VIOLET            0x04
#define COLOR_PURPLE            COLOR_VIOLET
#define COLOR_GREEN             0x05
#define COLOR_BLUE              0x06
#define COLOR_YELLOW            0x07
#define COLOR_ORANGE            0x08
#define COLOR_BROWN             0x09
#define COLOR_LIGHTRED          0x0A
#define COLOR_GRAY1             0x0B
#define COLOR_GRAY2             0x0C
#define COLOR_LIGHTGREEN        0x0D
#define COLOR_LIGHTBLUE         0x0E
#define COLOR_GRAY3             0x0F

/* TGI color defines */
#define TGI_COLOR_BLACK         COLOR_BLACK
#define TGI_COLOR_WHITE         COLOR_WHITE
#define TGI_COLOR_RED           COLOR_RED
#define TGI_COLOR_CYAN          COLOR_CYAN
#define TGI_COLOR_VIOLET        COLOR_VIOLET
#define TGI_COLOR_PURPLE        COLOR_PURPLE
#define TGI_COLOR_GREEN         COLOR_GREEN
#define TGI_COLOR_BLUE          COLOR_BLUE
#define TGI_COLOR_YELLOW        COLOR_YELLOW
#define TGI_COLOR_ORANGE        COLOR_ORANGE
#define TGI_COLOR_BROWN         COLOR_BROWN
#define TGI_COLOR_LIGHTRED      COLOR_LIGHTRED
#define TGI_COLOR_GRAY1         COLOR_GRAY1
#define TGI_COLOR_GRAY2         COLOR_GRAY2
#define TGI_COLOR_LIGHTGREEN    COLOR_LIGHTGREEN
#define TGI_COLOR_LIGHTBLUE     COLOR_LIGHTBLUE
#define TGI_COLOR_GRAY3         COLOR_GRAY3

/* Masks for joy_read */
#define JOY_UP_MASK             0x01
#define JOY_DOWN_MASK           0x02
#define JOY_LEFT_MASK           0x04
#define JOY_RIGHT_MASK          0x08
#define JOY_BTN_1_MASK          0x10

/* Define hardware */
#include <_vic2.h>
#define VIC     (*(struct __vic2*)0xD000)

#include <_sid.h>
#define SID     (*(struct __sid*)0xD400)

#include <_6526.h>
#define CIA1    (*(struct __6526*)0xDC00)
#define CIA2    (*(struct __6526*)0xDD00)

/* Define special memory areas */
#define COLOR_RAM       ((unsigned char*)0xD800)

/* Return codes for get_ostype */
#define C64_OS_US       0xAA            /* US version */
#define C64_OS_PET64    0x64            /* PET 64 */
#define C64_OS_SX64     0x43            /* SX-64 */
#define C64_EU_NEW      0x03
#define C64_EU_OLD      0x00
#define C64_DTV         0xFF            /* C64 DTV */



/*****************************************************************************/
/*                                 Variables                                 */
/*****************************************************************************/



/* The addresses of the static drivers */
extern void c64_65816_emd[];
extern void c64_c256k_emd[];
extern void c64_dqbb_emd[];
extern void c64_georam_emd[];
extern void c64_isepic_emd[];
extern void c64_kerberos_emd[];
extern void c64_ram_emd[];
extern void c64_ramcart_emd[];
extern void c64_reu_emd[];
extern void c64_vdc_emd[];
extern void dtv_himem_emd[];
extern void c64_hitjoy_joy[];
extern void c64_numpad_joy[];
extern void c64_ptvjoy_joy[];
extern void c64_stdjoy_joy[];           /* Referred to by joy_static_stddrv[] */
extern void c64_1351_mou[];             /* Referred to by mouse_static_stddrv[] */
extern void c64_joy_mou[];
extern void c64_inkwell_mou[];
extern void c64_pot_mou[];
extern void c64_swlink_ser[];
extern void c64_hi_tgi[];               /* Referred to by tgi_static_stddrv[] */



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



unsigned char get_ostype (void);
/* Get the ROM version. Returns one of the C64_OS_xxx codes. */



/* End of c64.h */
#endif
