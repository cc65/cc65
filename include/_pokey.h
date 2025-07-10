/*****************************************************************************/
/*                                                                           */
/*                                 _pokey.h                                  */
/*                                                                           */
/*                Internal include file, do not use directly                 */
/*                                                                           */
/* POKEY, Pot Keyboard Integrated Circuit, is a digital I/O chip designed    */
/* for the Atari 8-bit family of home computers; it combines functions for   */
/* sampling (ADC) potentiometers (such as game paddles) and scan matrices of */
/* switches (such as a computer keyboard) as well as sound generation.       */
/* It produces four voices of distinctive square wave sound, either as clear */
/* tones or modified with a number of distortion settings. - Wikipedia       */
/* "POKEY" article.                                                          */
/*                                                                           */
/*                                                                           */
/* (C) 2000 Freddy Offenga <taf_offenga@yahoo.com>                           */
/* 2019-01-16: Bill Kendrick <nbs@sonic.net>: More defines for registers     */
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



#ifndef __POKEY_H
#define __POKEY_H



/*****************************************************************************/
/* Define a structure with the POKEY register offsets for write (W)          */
/*****************************************************************************/

struct __pokey_write {
    unsigned char   audf1;  /* audio channel #1 frequency */
    unsigned char   audc1;  /* audio channel #1 control */
    unsigned char   audf2;  /* audio channel #2 frequency */
    unsigned char   audc2;  /* audio channel #2 control */
    unsigned char   audf3;  /* audio channel #3 frequency */
    unsigned char   audc3;  /* audio channel #3 control */
    unsigned char   audf4;  /* audio channel #4 frequency */
    unsigned char   audc4;  /* audio channel #4 control */
    unsigned char   audctl; /* audio control */
    unsigned char   stimer; /* start pokey timers */

    unsigned char   skrest;
    /* reset serial port status reg.;
    ** Reset BITs 5 - 7 of the serial port status register (SKCTL) to "1"
    */

    unsigned char   potgo;  /* start paddle scan sequence (see "ALLPOT") */
    unsigned char   unuse1; /* unused */
    unsigned char   serout; /* serial port data output */
    unsigned char   irqen;  /* interrupt request enable */
    unsigned char   skctl;  /* serial port control */
};


/*****************************************************************************/
/* (W) AUDC1-4 register values                                               */
/*****************************************************************************/

/* Meaningful values for the distortion bits.
** The first process is to divide the clock value by the frequency,
** then mask the output using the polys in the order below;
** finally, the result is divided by two.
*/
#define AUDC_POLYS_5_17  0x00
#define AUDC_POLYS_5     0x20 /* Same as 0x60 */
#define AUDC_POLYS_5_4   0x40
#define AUDC_POLYS_17    0x80
#define AUDC_POLYS_NONE  0xA0 /* Same as 0xE0 */
#define AUDC_POLYS_4     0xC0

/* When set, the volume value in AUDC1-4 bits 0-3 is sent directly to the speaker;
** it is not modulated with the frequency specified in the AUDF1-4 registers.
** (See "De Re Atari" Chapter 7: Sound)
*/
#define AUDC_VOLUME_ONLY 0x10


/*****************************************************************************/
/* (W) AUDCTL register values                                                */
/*****************************************************************************/

#define AUDCTL_CLOCKBASE_15HZ     0x01 /* Switch main clock base from 64 KHz to 15 KHz */
#define AUDCTL_HIGHPASS_CHAN2     0x02 /* Insert high pass filter into channel two, clocked by channel four */
#define AUDCTL_HIGHPASS_CHAN1     0x04 /* Insert high pass filter into channel one, clocked by channel two */
#define AUDCTL_JOIN_CHAN34        0x08 /* Join channels four and three (16 bit) */
#define AUDCTL_JOIN_CHAN12        0x10 /* Join channels two and one (16 bit) */
#define AUDCTL_CLOCK_CHAN3_179MHZ 0x20 /* Clock channel three with 1.79 MHz */
#define AUDCTL_CLOCK_CHAN1_179MHZ 0x40 /* Clock channel one with 1.79 MHz */
#define AUDCTL_9BIT_POLY          0x80 /* Makes the 17 bit poly counter into nine bit poly (see also: RANDOM) */


/*****************************************************************************/
/* (W) IRQEN register values                                                 */
/*****************************************************************************/

#define IRQEN_TIMER_1                  0x01 /* The POKEY timer one interrupt is enabled */
#define IRQEN_TIMER_2                  0x02 /* The POKEY timer two interrupt is enabled */
#define IRQEN_TIMER_4                  0x04 /* The POKEY timer four interrupt is enabled */
#define IRQEN_SERIAL_TRANS_FINISHED    0x08 /* The serial out transmission finished interrupt is enabled */
#define IRQEN_SERIAL_OUT_DATA_REQUIRED 0x10 /* The serial output data required interrupt is enabled */
#define IRQEN_SERIAL_IN_DATA_READY     0x20 /* The serial input data ready interrupt is enabled. */
#define IRQEN_OTHER_KEY                0x40 /* The "other key" interrupt is enabled */
#define IRQEN_BREAK_KEY                0x80 /* The BREAK key is enabled */


/*****************************************************************************/
/* (W) SKCTL register values                                                 */
/*****************************************************************************/

#define SKCTL_KEYBOARD_DEBOUNCE 0x01 /* Enable keyboard debounce circuits */
#define SKCTL_KEYBOARD_SCANNING 0x02 /* Enable keyboard scanning circuit */

/* Fast pot scan
** The pot scan counter completes its sequence in two TV line times instead of
** one frame time (228 scan lines). Not as accurate as the normal pot scan
*/
#define SKCTL_FAST_POT_SCAN     0x04

/* POKEY two-tone mode
** Serial output is transmitted as a two-tone signal rather than a logic true/false.
*/
#define SKCTL_TWO_TONE_MODE     0x08

/* Force break (serial output to zero) */
#define SKCTL_FORCE_BREAK       0x80


/* Bits 4, 5, and 6 of SKCTL set Serial Mode Control: */

/* Trans. & Receive rates set by external clock; Also internal clock phase reset to zero. */
#define SKCTL_SER_MODE_TX_EXT_RX_EXT       0x00

/* Trans. rate set by external clock; Receive asynch. (ch. 4) (CH3 and CH4). */
#define SKCTL_SER_MODE_TX_EXT_RX_ASYNC     0x10

/* Trans. & Receive rates set by Chan. 4; Chan. 4 output on Bi-Direct. clock line. */
#define SKCTL_SER_MODE_TX_CH4_RX_CH4_BIDIR 0x20

/* N.B.: Bit combination 0,1,1 not useful */

/* Trans. rate set by Chan. 4; Receive rate set by external clock. */
#define SKCTL_SER_MODE_TX_CH4_RX_EXT       0x40

/* N.B.: Bit combination 1,0,1 not useful */

/* Trans. rate set by Chan. 2; Receive rate set by Chan. 4; Chan. 4 out on Bi-Direct. clock line. */
#define SKCTL_SER_MODE_TX_CH2_RX_CH4_BIDIR 0x60

/* Trans. rate set by Chan. 2; Receive asynch. (chan 3 & 4); Bi-Direct. clock not used (tri-state condition). */
#define SKCTL_SER_MODE_TX_CH4_RX_ASYNC     0x70


/*****************************************************************************/
/* Define a structure with the POKEY register offsets for read (R)           */
/*****************************************************************************/

struct __pokey_read {
    unsigned char   pot0;   /* paddle 0 value */
    unsigned char   pot1;   /* paddle 1 value */
    unsigned char   pot2;   /* paddle 2 value */
    unsigned char   pot3;   /* paddle 3 value */
    unsigned char   pot4;   /* paddle 4 value */
    unsigned char   pot5;   /* paddle 5 value */
    unsigned char   pot6;   /* paddle 6 value */
    unsigned char   pot7;   /* paddle 7 value */
    unsigned char   allpot; /* eight paddle port status (see "POTGO") */
    unsigned char   kbcode; /* keyboard code */
    unsigned char   random; /* random number generator */
    unsigned char   unuse2; /* unused */
    unsigned char   unuse3; /* unused */
    unsigned char   serin;  /* serial port input */
    unsigned char   irqst;  /* interrupt request status */
    unsigned char   skstat; /* serial port status */
};


/*****************************************************************************/
/* (R) SKSTAT register values                                                */
/*****************************************************************************/

#define SKSTAT_SERIN_SHIFTREG_BUSY         0x02 /* Serial input shift register busy */
#define SKSTAT_LASTKEY_PRESSED             0x04 /* the last key is still pressed */
#define SKSTAT_SHIFTKEY_PRESSED            0x08 /* the [Shift] key is pressed */
#define SKSTAT_DATA_READ_INGORING_SHIFTREG 0x10 /* Data can be read directly from the serial input port, ignoring the shift register. */
#define SKSTAT_KEYBOARD_OVERRUN            0x20 /* Keyboard over-run; Reset BITs 7, 6 and 5 (latches) to 1, using SKREST */
#define SKSTAT_INPUT_OVERRUN               0x40 /* Serial data input over-run. Reset latches as above. */
#define SKSTAT_INPUT_FRAMEERROR            0x80 /* Serial data input frame error caused by missing or extra bits. Reset latches as above. */


/* KBCODE, internal keyboard codes for Atari 8-bit computers,
** are #defined as "KEY_..." in "atari.h".
** Note some keys are not read via KBCODE:
** - Reset
** - Start, Select, and Option; see CONSOL in "gtia.h"
** - Break
*/


/* End of _pokey.h */
#endif /* #ifndef __POKEY_H */
