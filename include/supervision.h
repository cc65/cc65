/*****************************************************************************/
/*                                                                           */
/*                               supervision.h                               */
/*                                                                           */
/*                     Supervision specific definitions                      */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* 2003 Peter Trauner (trap@utanet.at)                                       */
/*                                                                           */
/*                                                                           */
/* This software is provided "as-is," without any expressed or implied       */
/* warranty.  In no event will the authors be held liable for any damages    */
/* arising from the use of this software.                                    */
/*                                                                           */
/* Permission is granted to anyone to use this software for any purpose,     */
/* including commercial applications, and to alter and redistribute it       */
/* freely, subject to the following restrictions:                            */
/*                                                                           */
/* 1. The origin of this software must not be misrepresented; you must not   */
/*    claim that you wrote the original software.  If you use this software  */
/*    in a product, an acknowledgment, in the product's documentation,       */
/*    would be appreciated, but is not required.                             */
/* 2. Alterred source versions must be marked plainly as such,               */
/*    and must not be misrepresented as being the original software.         */
/* 3. This notice may not be removed or alterred                             */
/*    from any source distribution.                                          */
/*                                                                           */
/*****************************************************************************/



#ifndef _SUPERVISION_H
#define _SUPERVISION_H



/* Check for errors */
#if !defined(__SUPERVISION__)
#  error This module may only be used when compiling for the Supervision!
#endif



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



struct __sv_lcd {
    unsigned char       width;
    unsigned char       height;
    unsigned char       xpos;
    unsigned char       ypos;
};
#define SV_LCD  ((struct __sv_lcd*)0x2000)

struct __sv_tone {
    unsigned            delay;
    unsigned char       control;
    unsigned char       timer;
};
#define SV_RIGHT ((struct __sv_tone*)0x2010)
#define SV_LEFT  ((struct __sv_tone*)0x2014)

struct __sv_noise {
    unsigned char       volume;         /* and frequency */
    unsigned char       timer;
    unsigned char       control;
};
#define SV_NOISE ((struct __sv_noise*)0x2028)

struct __io_port {
    unsigned char       in;
    unsigned char       out;
};
#define IO_PORT  ((struct __io_port*)(0x2021)

struct __sv_dma {
    unsigned            start;
    unsigned char       size;
    unsigned char       control;
    unsigned char       on;
};
#define SV_DMA   ((struct __sv_dma*)0x2018)

#define SV_CONTROL (*(unsigned char*)0x2020)

#define SV_BANK  (*(unsigned char*)0x2026)
#define SV_BANK_COMBINE(nmi,irq_timer,irq_dma,lcd_on, timer_prescale, bank) \
        ((nmi)?1:0)|((irq_timer)?2:0)|((irq_dma)?4:0)|((lcd_on)?8:0) \
        |((timer_prescale)?0x10:0)|((bank)<<5)

#define SV_VIDEO ((unsigned char*)0x4000)
#define SV_TIMER_COUNT (*(unsigned char*)0x2023)



/* Counters incremented asynchronously!
** If you want more complex, copy the crt0.s file from the libsrc/supervision
** directory and code them yourself (in assembler)
*/
extern unsigned char sv_nmi_counter;
extern unsigned char sv_timer_irq_counter;
extern unsigned char sv_timer_dma_counter;



/* End of supervision.h */
#endif


