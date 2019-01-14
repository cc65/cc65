/*****************************************************************************/
/*                                                                           */
/*                                  _pia.h                                   */
/*                                                                           */
/*                Internal include file, do not use directly                 */
/*                                                                           */
/* The Peripheral Interface Adapter (PIA) chip provides parallel I/O         */
/* interfacing; it was used in Atari 400/800 and Commodore PET family of     */
/* computers, for joystick and interrupts.                                   */
/* - Sources; various + Wikpedia article on "Peripheral Interface Adapter"   */
/*                                                                           */
/*                                                                           */
/* (C) 2000 Freddy Offenga <taf_offenga@yahoo.com>                           */
/* 2019-01-13: Bill Kendrick <nbs@sonic.net>: Defines for registers          */
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



#ifndef __PIA_H
#define __PIA_H


/* Define a structure with the PIA register offsets */
struct __pia {
    unsigned char   porta;  /* port A data r/w */
/* Paddle 0-3 triggers (per PORTA bits) */
#define PORTA_PTRIG3 0x80
#define PORTA_PTRIG2 0x40
#define PORTA_PTRIG1 0x08
#define PORTA_PTRIG0 0x04


    unsigned char   portb;  /* port B data r/w */
/* Paddle 4-7 triggers (per PORTB bits); only 400/800 had four controller ports */
#define PORTB_PTRIG7 0x80
#define PORTB_PTRIG6 0x40
#define PORTB_PTRIG5 0x08
#define PORTB_PTRIG4 0x04

    /* See also: "JOY_xxx_MASK" in "atari.h" */

    unsigned char   pactl;  /* port A control */
    unsigned char   pbctl;  /* port B control */

#define PxCTL_IRQ_ENABLE    0x01 /* (W) Peripheral A interrupt (IRQ) enable. */
  /* One equals enable. Set by the OS but available to the user;
     reset on powerup. */
#define PxCTL_BIT1          0x02 /* "Set to zero" */
#define PxCTL_ADDRESSING    0x04 /* (W) Controls PORTA addressing */
  /* One equals PORTA register; zero equals direction control register */
#define PACTL_MOTOR_CONTROL 0x08 /* (W) Peripheral motor control line (turn the cassette on
                     or off; zero equals on) */
#define PBCTL_PERIPH_CMD_IDENT 0x08 /* Peripheral command identification (serial bus command) */
#define PxCTL_BIT4          0x10 /* "Set to one" */
#define PxCTL_BIT5          0x20 /* "Set to one" */
#define PxCTL_BIT6          0x40 /* "Set to zero" */
#define PxCTL_IRQ_STATUS    0x80 /* Peripheral interrupt (IRQ) status bit. */
  /* Set by Peripherals (PORTA / PORTB). Reset by reading PORTA / PORTB*/
};



/* End of _pia.h */
#endif
