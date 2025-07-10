/*****************************************************************************/
/*                                                                           */
/*                                _mikey.h                                   */
/*                                                                           */
/* Atari Lynx, Mikey chip register hardware structures                       */
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

#ifndef __MIKEY_H
#define __MIKEY_H

/* Timer structure */
typedef struct _mikey_timer {
  unsigned char reload;
  unsigned char control;
  unsigned char count;
  unsigned char control2;
} _mikey_timer;

typedef struct _mikey_all_timers {
  struct _mikey_timer timer[8];
} _mikey_all_timers;

/* Audio channel structure */
typedef struct _mikey_audio {
  unsigned char volume;
  unsigned char feedback;
  unsigned char dac;
  unsigned char shiftlo;
  unsigned char reload;
  unsigned char control;
  unsigned char count;
  unsigned char other;
} _mikey_audio;

/* Define a structure with the mikey register offsets */
struct __mikey {
  struct _mikey_timer timer0;    /* 0xFD00 */
  struct _mikey_timer timer1;    /* 0xFD04 */
  struct _mikey_timer timer2;    /* 0xFD08 */
  struct _mikey_timer timer3;    /* 0xFD0C */
  struct _mikey_timer timer4;    /* 0xFD10 */
  struct _mikey_timer timer5;    /* 0xFD14 */
  struct _mikey_timer timer6;    /* 0xFD18 */
  struct _mikey_timer timer7;    /* 0xFD1C */
  struct _mikey_audio channel_a; /* 0xFD20 */
  struct _mikey_audio channel_b; /* 0xFD28 */
  struct _mikey_audio channel_c; /* 0xFD30 */
  struct _mikey_audio channel_d; /* 0xFD38 */
  unsigned char attena;          /* 0xFD40  ?? not yet allocated? */
  unsigned char attenb;          /* 0xFD41      | */
  unsigned char attenc;          /* 0xFD42      | */
  unsigned char attend;          /* 0xFD43      | */
  unsigned char panning;         /* 0xFD44      | */
  unsigned char unused0[11];     /* 0xFD45 - 0xFD4F  not used */
  unsigned char mstereo;         /* 0xFD50  stereo control bits */
  unsigned char unused1[47];     /* 0xFD51 - 0xFD7F  not used */
  unsigned char intrst;          /* 0xFD80  interrupt poll 0 */
  unsigned char intset;          /* 0xFD81  interrupt poll 1 */
  unsigned char unused2[2];      /* 0xFD82 - 0xFD83  not used */
  unsigned char magrdy0;         /* 0xFD84  mag tape channel0 ready bit */
  unsigned char magrdy1;         /* 0xFD85  mag tape channel1 ready bit */
  unsigned char audin;           /* 0xFD86  audio in */
  unsigned char sysctl1;         /* 0xFD87  control bits */
  unsigned char mikeyrev;        /* 0xFD88  mikey hardware rev */
  unsigned char mikeysrev;       /* 0xFD89  mikey software rev */
  unsigned char iodir;           /* 0xFD8A  parallel i/o data dir */
  unsigned char iodat;           /* 0xFD8B  parallel data */
  unsigned char serctl;          /* 0xFD8C  serial control register */
  unsigned char serdat;          /* 0xFD8D  serial data */
  unsigned char unused3[2];      /* 0xFD8E - 0xFD8F  not used */
  unsigned char sdoneack;        /* 0xFD90  suzy done acknowledge */
  unsigned char cpusleep;        /* 0xFD91  cpu bus request disable */
  unsigned char dispctl;         /* 0xFD92  video bus request enable, viddma */
  unsigned char pkbkup;          /* 0xFD93  magic 'P' count */
  unsigned char *scrbase;        /* 0xFD94  start address of video display */
  unsigned char unused4[6];      /* 0xFD96 - 0xFD9B  not used */
  unsigned char mtest0;          /* 0xFD9C */
  unsigned char mtest1;          /* 0xFD9D */
  unsigned char mtest2;          /* 0xFD9E */
  unsigned char unused5;         /* 0xFD9F  not used */
  unsigned char palette[32];     /* 0xFDA0 - 0xFDBF  palette 32 bytes */
  unsigned char unused6[64];     /* 0xFDC0 - 0xFDFF  not used */
  unsigned char bootrom[504];    /* 0xFE00 - 0xFFD8  boot rom */
  unsigned char reserved;        /* 0xFFD8  reserved for future hardware */
  unsigned char mapctl;          /* 0xFFF9  map control register */
  struct {
    unsigned char *nmi;          /* 0xFFFA  NMI vector */
    unsigned char *reset;        /* 0xFFFB  reset vector */
    unsigned char *irq;          /* 0xFFFC  IRQ vector */
  } vectors;
};

/* TIM_CONTROLA control bit definitions */
enum {
    ENABLE_INT       = 0x80,
    RESET_DONE       = 0x40,
    ENABLE_RELOAD    = 0x10,
    ENABLE_COUNT     = 0x08
};

/* AUD_CONTROL control bit definitions */
enum {
    FEEDBACK_7       = 0x80,
    ENABLE_INTEGRATE = 0x20
};

/* Audio and timer clock settings for source period */
enum {
    AUD_LINKING      = 0x07,
    AUD_64           = 0x06,
    AUD_32           = 0x05,
    AUD_16           = 0x04,
    AUD_8            = 0x03,
    AUD_4            = 0x02,
    AUD_2            = 0x01,
    AUD_1            = 0x00
};

/* TIM_CONTROLB control bit definitions */
enum {
    TIMER_DONE       = 0x08,
    LAST_CLOCK       = 0x04,
    BORROW_IN        = 0x02,
    BORROW_OUT       = 0x01
};

/* MPAN and MSTEREO registers bit definitions */
enum {
    LEFT3_SELECT     = 0x80,
    LEFT2_SELECT     = 0x40,
    LEFT1_SELECT     = 0x20,
    LEFT0_SELECT     = 0x10,
    RIGHT3_SELECT    = 0x08,
    RIGHT2_SELECT    = 0x04,
    RIGHT1_SELECT    = 0x02,
    RIGHT0_SELECT    = 0x01,
    LEFT_ATTENMASK   = 0xF0,
    RIGHT_ATTENMASK  = 0x0F
};

/* Interrupt Reset and Set bit definitions */
enum {
    TIMER7_INT       = 0x80,
    TIMER6_INT       = 0x40,
    TIMER5_INT       = 0x20,
    TIMER4_INT       = 0x10,
    TIMER3_INT       = 0x08,
    TIMER2_INT       = 0x04,
    TIMER1_INT       = 0x02,
    TIMER0_INT       = 0x01,
    SERIAL_INT       = TIMER4_INT,
    VERTICAL_INT     = TIMER2_INT,
    HORIZONTAL_INT   = TIMER0_INT
};

/* SYSCTL1 bit definitions */
enum {
    POWERON          = 0x02,
    CART_ADDR_STROBE = 0x01
};

/* IODIR and IODAT bit definitions */
enum {
    AUDIN_BIT        = 0x10, /* different from AUDIN address */
    READ_ENABLE      = 0x10, /* same bit for AUDIN_BIT */
    RESTLESS         = 0x08,
    NOEXP            = 0x04, /* if set, redeye is not connected */
    CART_ADDR_DATA   = 0x02,
    CART_POWER_OFF   = 0x02, /* same bit for CART_ADDR_DATA */
    EXTERNAL_POWER   = 0x01
};

/* SERCTL bit definitions for write operations */
enum {
    TXINTEN          = 0x80,
    RXINTEN          = 0x40,
    PAREN            = 0x10,
    RESETERR         = 0x08,
    TXOPEN           = 0x04,
    TXBRK            = 0x02,
    PAREVEN          = 0x01
};

/* SERCTL bit definitions for read operations */
enum {
    TXRDY            = 0x80,
    RXRDY            = 0x40,
    TXEMPTY          = 0x20,
    PARERR           = 0x10,
    OVERRUN          = 0x08,
    FRAMERR          = 0x04,
    RXBRK            = 0x02,
    PARBIT           = 0x01
};

/* DISPCTL bit definitions */
enum {
    DISP_COLOR       = 0x08, /* must be set to 1 */
    DISP_FOURBIT     = 0x04, /* must be set to 1 */
    DISP_FLIP        = 0x02,
    DMA_ENABLE       = 0x01  /* must be set to 1 */
};

/* MTEST0 bit definitions */
enum {
    AT_CNT16         = 0x80,
    AT_TEST          = 0x40,
    XCLKEN           = 0x20,
    UART_TURBO       = 0x10,
    ROM_SEL          = 0x08,
    ROM_TEST         = 0x04,
    M_TEST           = 0x02,
    CPU_TEST         = 0x01
};

/* MTEST1 bit definitions */
enum {
    P_CNT16          = 0x40,
    REF_CNT16        = 0x20,
    VID_TRIG         = 0x10,
    REF_TRIG         = 0x08,
    VID_DMA_DIS      = 0x04,
    REF_FAST         = 0x02,
    REF_DIS          = 0x01
};

/* MTEST2 bit definitions */
enum {
    V_STROBE         = 0x10,
    V_ZERO           = 0x08,
    H_120            = 0x04,
    H_ZERO           = 0x02,
    V_BLANKEF        = 0x01
};

/* MAPCTL bit definitions */
enum {
    TURBO_DISABLE    = 0x80,
    VECTOR_SPACE     = 0x08,
    ROM_SPACE        = 0x04,
    MIKEY_SPACE      = 0x02,
    SUZY_SPACE       = 0x01
};

#endif
