/*****************************************************************************/
/*                                                                           */
/* Atari VCS 7800 TIA & RIOT registers addresses                             */
/*                                                                           */
/* Karri Kaksonen  (karri@sipo.fi), 2022                                     */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/



#ifndef _ATARI7800_H
#define _ATARI7800_H

/* Check for errors */
#if !defined(__ATARI7800__)
#  error This module may only be used when compiling for the Atari 7800!
#endif

/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Color defines */
#define COLOR_BLACK             0x00
#define COLOR_GREY              0x01
#define COLOR_LIGHTGREY         0x02
#define COLOR_WHITE             0x03

/* TGI color defines (default palette) */
#define TGI_COLOR_BLACK         COLOR_BLACK
#define TGI_COLOR_GREY          COLOR_GREY
#define TGI_COLOR_LIGHTGREY     COLOR_LIGHTGREY
#define TGI_COLOR_WHITE         COLOR_WHITE

/* Masks for joy_read */
#define JOY_RIGHT_MASK          0x80
#define JOY_LEFT_MASK           0x40
#define JOY_DOWN_MASK           0x20
#define JOY_UP_MASK             0x10
#define JOY_BTN_1_MASK          0x01
#define JOY_BTN_2_MASK          0x02

#define JOY_BTN_A_MASK          JOY_BTN_1_MASK
#define JOY_BTN_B_MASK          JOY_BTN_2_MASK

#define JOY_BTN_A(v)            ((v) & JOY_BTN_A_MASK)
#define JOY_BTN_B(v)            ((v) & JOY_BTN_B_MASK)

/* No support for dynamically loadable drivers */
#define DYN_DRV 0

unsigned char get_tv(void);           /* get TV system */

#include <_tia.h>
#define TIA (*(struct __tia*)0x0000)

#include <_riot.h>
#define RIOT (*(struct __riot*)0x0280)

#include <_maria.h>
#define MARIA (*(struct __maria*)0x0020)

/* End of atari7800.h */
#endif
