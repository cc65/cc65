/*****************************************************************************/
/*                                                                           */
/*                                  _pia.h                                   */
/*                                                                           */
/*                Internal include file, do not use directly                 */
/*                                                                           */
/* The Peripheral Interface Adapter (PIA) chip (a 6520 or 6820) provides     */
/* parallel I/O interfacing; it was used in Atari 400/800 and Commodore PET  */
/* family of computers, for joystick and some interrupts.                    */
/* Sources; various + Wikpedia article on "Peripheral Interface Adapter".    */
/*                                                                           */
/*                                                                           */
/* (C) 2000 Freddy Offenga <taf_offenga@yahoo.com>                           */
/* 2019-01-17: Bill Kendrick <nbs@sonic.net>: Defines for registers          */
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
    unsigned char   portb;  /* port B data r/w */
    unsigned char   pactl;  /* port A control */
    unsigned char   pbctl;  /* port B control */
};


/*****************************************************************************/
/* PORTA and PORTB register bits                                             */
/*****************************************************************************/

/* See also: "JOY_xxx_MASK" in "atari.h" */

/* Paddle 0-3 triggers (per PORTA bits) */
#define PORTA_PTRIG3 0x80
#define PORTA_PTRIG2 0x40
#define PORTA_PTRIG1 0x08
#define PORTA_PTRIG0 0x04


/* On the Atari 400/800, PORTB is the same as PORTA, but for controller ports 3 & 4. */

/* Paddle 4-7 triggers (per PORTB bits); only 400/800 had four controller ports */
#define PORTB_PTRIG7 0x80
#define PORTB_PTRIG6 0x40
#define PORTB_PTRIG5 0x08
#define PORTB_PTRIG4 0x04


/* On the XL series of computers, PORTB has been changed to a memory and
** LED control (1200XL model only) register (read/write):
*/

/* If set, the built-in OS is enabled, and occupies the address range $C000-$FFFF
** (except that the area $D000-$D7FF will only access the hardware registers.)
** If clear, RAM is enabled in this area (again, save for the hole.)
*/
#define PORTB_OSROM            0x01

/* If set, RAM is enabled for the address range $A000-$BFFF.
** If clear, the built-in BASIC ROM is enabled at this address.
** And if there is a cartridge installed in the computer, it makes no difference.
*/
#define PORTB_BASICROM         0x02

/* If set, the corresponding LED is turned off. If clear, the LED will be on.
** (1200XL only)
*/
#define PORTB_LED1             0x04
#define PORTB_LED2             0x08


/* On the XE series of computers, PORTB is a bank-selected memory control register (read/write): */

/* These bits determine which memory bank is visible to the CPU and/or ANTIC chip
** when their Bank Switch bit is set. There are four possible banks of 16KB each.
*/
#define PORTB_BANKSELECT1      0x00
#define PORTB_BANKSELECT2      0x04
#define PORTB_BANKSELECT3      0x08
#define PORTB_BANKSELECT4      0x0C

/* If set, the CPU and/or ANTIC chip will access bank-switched memory mapped to the
** address range $4000-$7FFF.
** If clear, the CPU and/or ANTIC will see normal memory in this region.
*/
#define PORTB_BANKSWITCH_CPU   0x10
#define PORTB_BANKSWITCH_ANTIC 0x20

/* If set, RAM is enabled for the address range $5000-$57FF.
** If clear, the self-test ROM (physically located at $D000-$D7FF, under the hardware registers)
** is remapped to this memory area.
*/
#define PORTB_SELFTEST         0x80


/*****************************************************************************/
/* PACTL and PBCTL register bits                                             */
/*****************************************************************************/

/* (W) Peripheral PA1/PB1 interrupt (IRQ) ("peripheral proceed line available") enable.
** One equals enable. Set by the OS but available to the user; reset on powerup.
** (PxCTL_IRQ_STATUS (R) bit will get set upon interrupt occurance)
*/
#define PxCTL_IRQ_ENABLE         0x01 /* bit 0 */

/* Note: Bit 1 is always set to */

/* (W) Controls PORTA/PORTB addressing
** 1 = PORTA/PORTB register; read/write to controller port
** 0 = direction control register; write to direction controls
**     (allows setting data flow; write 0s & 1s to PORTA/PORTB bits
**     to set which port's pins are read (input), or write (output),
**     respectively)
*/
#define PxCTL_ADDRESSING         0x04 /* bit 2 */

/* (W) Peripheral motor control line; Turn the cassette on or off
** (PACTL-specific register bit)
** 0 = on
** 1 = off
*/
#define PACTL_MOTOR_CONTROL      0x08 /* bit 3 */

/* Peripheral command identification (serial bus command line)
** (PBCTL-specific register bit)
*/
#define PBCTL_PERIPH_CMD_IDENT   0x08 /* bit 3 */

/* Note: Bits 4 & 5 are always set to 1 */

/* Note: Bit 6 is always set to 0 */

/* (R) Peripheral interrupt (IRQ) status bit.
** Set by Peripherals (PORTA / PORTB).  Reset by reading from PORTA / PORTB.
** PACTL's is interrupt status of PROCEED
** PBCTL's is interrupt status of SIO
*/
#define PxCTL_IRQ_STATUS         0x80


/* End of _pia.h */
#endif
