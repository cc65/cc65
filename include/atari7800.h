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
#define COLOR_WHITE             0x0F

/* TGI color defines (default palette) */
#define TGI_COLOR_BLACK         COLOR_BLACK
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

#include <_tia.h>
#define TIA (*(struct __tia*)0x0000)

#include <_riot.h>
#define RIOT (*(struct __riot*)0x0280)

#include <_maria.h>
#define MARIA (*(struct __maria*)0x0020)

/*
 * Alternative MARIA register definitions.
 */
#define BKGRND      MARIA.bkgrnd
#define BACKGRND    MARIA.bkgrnd
#define P0C1        MARIA.p0c1
#define P0C2        MARIA.p0c2
#define P0C3        MARIA.p0c3
#define WSYNC       MARIA.wsync
#define P1C1        MARIA.p1c1
#define P1C2        MARIA.p1c2
#define P1C3        MARIA.p1c3
#define MSTAT       MARIA.mstat
#define P2C1        MARIA.p2c1
#define P2C2        MARIA.p2c2
#define P2C3        MARIA.p2c3
#define DPPH        MARIA.dpph
#define DPH         MARIA.dpph
#define P3C1        MARIA.p3c1
#define P3C2        MARIA.p3c2
#define P3C3        MARIA.p3c3
#define DPPL        MARIA.dppl
#define DPL         MARIA.dppl
#define P4C1        MARIA.p4c1
#define P4C2        MARIA.p4c2
#define P4C3        MARIA.p4c3
#define CHBASE      MARIA.chbase
#define P5C1        MARIA.p5c1
#define P5C2        MARIA.p5c2
#define P5C3        MARIA.p5c3
#define OFFSET      MARIA.offset
#define P6C1        MARIA.p6c1
#define P6C2        MARIA.p6c2
#define P6C3        MARIA.p6c3
#define CTRL        MARIA.ctrl
#define P7C1        MARIA.p7c1
#define P7C2        MARIA.p7c2
#define P7C3        MARIA.p7c3

/*
 * MSTAT register constants
 */
#define MSTAT_VBLANK    0x80    /* vblank is on when bit 7 is set */

/* End of atari7800.h */
#endif
