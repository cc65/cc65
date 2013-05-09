/*****************************************************************************/
/*                                                                           */
/*                                  _sid.h                                   */
/*                                                                           */
/*                Internal include file, do not use directly                 */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2000 Ullrich von Bassewitz                                       */
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



#ifndef __SID_H
#define __SID_H



/* Define a structure with the sid register offsets */
struct __sid_voice {
    unsigned            freq;           /* Frequency */
    unsigned            pw;             /* Pulse width */
    unsigned char       ctrl;           /* Control register */
    unsigned char       ad;             /* Attack/decay */
    unsigned char       sr;             /* Sustain/release */
};
struct __sid {
    struct __sid_voice  v1;             /* Voice 1 */
    struct __sid_voice  v2;             /* Voice 2 */
    struct __sid_voice  v3;             /* Voice 3 */
    unsigned            flt_freq;       /* Filter frequency */
    unsigned char       flt_ctrl;       /* Filter control register */
    unsigned char       amp;            /* Amplitude */
    unsigned char       ad1;            /* A/D converter 1 */
    unsigned char       ad2;            /* A/D converter 2 */
    unsigned char       noise;          /* Noise generator */
    unsigned char       read3;          /* Value of voice 3 */
};



/* End of _sid.h */
#endif



