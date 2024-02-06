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

/* (Some specific register values for Atari defined in atari.h) */

/* End of _pia.h */
#endif
