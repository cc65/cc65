/*****************************************************************************/
/*                                                                           */
/*                                _suzy.h                                    */
/*                                                                           */
/* Atari Lynx, Suzy chip register hardware structures                        */
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

#ifndef __SUZY_H
#define __SUZY_H

/* JOYSTICK bit definitions */
enum {
  JOYPAD_RIGHT   = 0x10,
  JOYPAD_LEFT    = 0x20,
  JOYPAD_DOWN    = 0x40,
  JOYPAD_UP      = 0x80,
  BUTTON_OPTION1 = 0x08,
  BUTTON_OPTION2 = 0x04,
  BUTTON_INNER   = 0x02,
  BUTTON_OUTER   = 0x01
};

/* SWITCHES bit definitions */
enum {
    CART1_IO_INACTIVE = 0x04,
    CART0_IO_INACTIVE = 0x02,
    BUTTON_PAUSE      = 0x01
};

/* SPRCTL0 bit definitions */
enum {
  BPP_4            = 0xC0,
  BPP_3            = 0x80,
  BPP_2            = 0x40,
  BPP_1            = 0x00,
  HFLIP            = 0x20,
  VFLIP            = 0x10,
  TYPE_SHADOW      = 0x07,
  TYPE_XOR         = 0x06,
  TYPE_NONCOLL     = 0x05,
  TYPE_NORMAL      = 0x04,
  TYPE_BOUNDARY    = 0x03,
  TYPE_BSHADOW     = 0x02,
  TYPE_BACKNONCOLL = 0x01,
  TYPE_BACKGROUND  = 0x00
};

/* SPRCTL1 bit definitions */
enum {
  LITERAL  = 0x80,
  PACKED   = 0x00,
  ALGO3    = 0x40,
  RENONE   = 0x00,
  REHV     = 0x10,
  REHVS    = 0x20,
  REHVST   = 0x30,
  REUSEPAL = 0x08,
  SKIP     = 0x04,
  DRAWUP   = 0x02,
  DRAWLEFT = 0x01
};

/* Sprite control block (SCB) definitions */

/* SCB with all attributes */
typedef struct SCB_REHVST_PAL {
  unsigned char sprctl0;
  unsigned char sprctl1;
  unsigned char sprcoll;
  char *next;
  unsigned char *data;
  signed int hpos;
  signed int vpos;
  unsigned int hsize;
  unsigned int vsize;
  unsigned int stretch;
  unsigned int tilt;
  unsigned char penpal[8];
} SCB_REHVST_PAL;

/* SCB without pallette */
typedef struct SCB_REHVST {
  unsigned char sprctl0;
  unsigned char sprctl1;
  unsigned char sprcoll;
  char *next;
  unsigned char *data;
  signed int hpos;
  signed int vpos;
  unsigned int hsize;
  unsigned int vsize;
  unsigned int stretch;
  unsigned int tilt;
} SCB_REHVST;

/* SCB without stretch/tilt */
typedef struct SCB_REHV {
  unsigned char sprctl0;
  unsigned char sprctl1;
  unsigned char sprcoll;
  char *next;
  unsigned char *data;
  signed int hpos;
  signed int vpos;
  unsigned int hsize;
  unsigned int vsize;
} SCB_REHV;

/* SCB without stretch/tilt, with penpal */
typedef struct SCB_REHV_PAL {
  unsigned char sprctl0;
  unsigned char sprctl1;
  unsigned char sprcoll;
  char *next;
  unsigned char *data;
  signed int hpos;
  signed int vpos;
  unsigned int hsize;
  unsigned int vsize;
  unsigned char penpal[8];
} SCB_REHV_PAL;

/* SCB without tilt/penpal */
typedef struct SCB_REHVS {
  unsigned char sprctl0;
  unsigned char sprctl1;
  unsigned char sprcoll;
  char *next;
  unsigned char *data;
  signed int hpos;
  signed int vpos;
  unsigned int hsize;
  unsigned int vsize;
  unsigned int stretch;
} SCB_REHVS;

/* SCB without tilt, with penpal */
typedef struct SCB_REHVS_PAL {
  unsigned char sprctl0;
  unsigned char sprctl1;
  unsigned char sprcoll;
  char *next;
  unsigned char *data;
  signed int hpos;
  signed int vpos;
  unsigned int hsize;
  unsigned int vsize;
  unsigned int stretch;
  unsigned char penpal[8];
} SCB_REHVS_PAL;

/* SCB without size/stretch/tilt/penpal */
typedef struct SCB_RENONE {
  unsigned char sprctl0;
  unsigned char sprctl1;
  unsigned char sprcoll;
  char *next;
  unsigned char *data;
  signed int hpos;
  signed int vpos;
} SCB_RENONE;

/* SCB without size/str/tilt, with penpal */
typedef struct SCB_RENONE_PAL {
  unsigned char sprctl0;
  unsigned char sprctl1;
  unsigned char sprcoll;
  char *next;
  unsigned char *data;
  signed int hpos;
  signed int vpos;
  unsigned char penpal[8];
} SCB_RENONE_PAL;

typedef struct PENPAL_4 {
  unsigned char penpal[8];
} PENPAL_4;

typedef struct PENPAL_3 {
  unsigned char penpal[4];
} PENPAL_3;

typedef struct PENPAL_2 {
  unsigned char penpal[2];
} PENPAL_2;

typedef struct PENPAL_1 {
  unsigned char penpal[1];
} PENPAL_1;

/* SPRGO bit definitions */
enum {
    SPRITE_GO = 0x01,  /* sprite process start bit */
    EVER_ON   = 0x04   /* everon detector enable */
};

/* SPRSYS bit definitions for write operations */
enum {
    SIGNMATH   = 0x80,  /* signed math */
    ACCUMULATE = 0x40,  /* accumulate multiplication results */
    NO_COLLIDE = 0x20,  /* do not collide with any sprites (also SPRCOLL bit definition) */
    VSTRETCH   = 0x10,  /* stretch v */
    LEFTHAND   = 0x08,
    CLR_UNSAFE = 0x04,  /* unsafe access reset */
    SPRITESTOP = 0x02   /* request to stop sprite process */
};

/* SPRSYS bit definitions for read operations */
enum {
    MATHWORKING   = 0x80,  /* math operation in progress */
    MATHWARNING   = 0x40,  /* accumulator overflow on multiple or divide by zero */
    MATHCARRY     = 0x20,  /* last carry bit */
    VSTRETCHING   = 0x10,
    LEFTHANDED    = 0x08,
    UNSAFE_ACCESS = 0x04,  /* unsafe access performed */
    SPRITETOSTOP  = 0x02,  /* requested to stop */
    SPRITEWORKING = 0x01   /* sprite process is active */
};

/* Suzy hardware registers */
struct __suzy {
  unsigned char *tmpadr;        /* 0xFC00  Temporary address */
  unsigned int tiltacc;         /* 0xFC02  Tilt accumulator */
  unsigned int hoff;            /* 0xFC04  Offset to H edge of screen */
  unsigned int voff;            /* 0xFC06  Offset to V edge of screen */
  unsigned char *sprbase;       /* 0xFC08  Base address of sprite */
  unsigned char *colbase;       /* 0xFC0A  Base address of collision buffer */
  unsigned char *vidadr;        /* 0xFC0C  Current vid buffer address */
  unsigned char *coladr;        /* 0xFC0E  Current col buffer address */
  unsigned char *scbnext;       /* 0xFC10  Address of next SCB */
  unsigned char *sprdline;      /* 0xFC12  start of sprite data line address */
  unsigned int hposstrt;        /* 0xFC14  start hpos */
  unsigned int vposstrt;        /* 0xFC16  start vpos */
  unsigned int sprhsize;        /* 0xFC18  sprite h size */
  unsigned int sprvsize;        /* 0xFC1A  sprite v size */
  unsigned int stretchl;        /* 0xFC1C  H size adder */
  unsigned int tilt;            /* 0xFC1E  H pos adder */
  unsigned int sprdoff;         /* 0xFC20  offset to next sprite data line */
  unsigned int sprvpos;         /* 0xFC22  current vpos */
  unsigned int colloff;         /* 0xFC24  offset to collision depository */
  unsigned int vsizeacc;        /* 0xFC26  vertical size accumulator */
  unsigned int hsizeoff;        /* 0xFC28  horizontal size offset */
  unsigned int vsizeoff;        /* 0xFC2A  vertical size offset */
  unsigned char *scbaddr;       /* 0xFC2C  address of current SCB */
  unsigned char *procaddr;      /* 0xFC2E  address of current spr data proc */
  unsigned char unused0[32];    /* 0xFC30 - 0xFC4F  reserved/unused */
  unsigned char unused1[2];     /* 0xFC50 - 0xFC51  do not use */
  unsigned char mathd;          /* 0xFC52 */
  unsigned char mathc;          /* 0xFC53 */
  unsigned char mathb;          /* 0xFC54 */
  unsigned char matha;          /* 0xFC55  write starts a multiply operation */
  unsigned char mathp;          /* 0xFC56 */
  unsigned char mathn;          /* 0xFC57 */
  unsigned char unused2[8];     /* 0xFC58 - 0xFC5F  do not use */
  unsigned char mathh;          /* 0xFC60 */
  unsigned char mathg;          /* 0xFC61 */
  unsigned char mathf;          /* 0xFC62 */
  unsigned char mathe;          /* 0xFC63  write starts a divide operation */
  unsigned char unused3[8];     /* 0xFC64 - 0xFC6B  do not use */
  unsigned char mathm;          /* 0xFC6C */
  unsigned char mathl;          /* 0xFC6D */
  unsigned char mathk;          /* 0xFC6E */
  unsigned char mathj;          /* 0xFC6F */
  unsigned char unused4[16];    /* 0xFC70 - 0xFC7F  do not use */
  unsigned char sprctl0;        /* 0xFC80  sprite control bits 0 */
  unsigned char sprctl1;        /* 0xFC81  sprite control bits 1 */
  unsigned char sprcoll;        /* 0xFC82  sprite collision number */
  unsigned char sprinit;        /* 0xFC83  sprite initialization bits */
  unsigned char unused5[4];     /* 0xFC84 - 0xFC87  unused */
  unsigned char suzyhrev;       /* 0xFC88  suzy hardware rev */
  unsigned char suzysrev;       /* 0xFC89  suzy software rev */
  unsigned char unused6[6];     /* 0xFC8A - 0xFC8F  unused */
  unsigned char suzybusen;      /* 0xFC90  suzy bus enable */
  unsigned char sprgo;          /* 0xFC91  sprite process start bit */
  unsigned char sprsys;         /* 0xFC92  sprite system control bits */
  unsigned char unused7[29];    /* 0xFC93 - 0xFCAF  unused */
  unsigned char joystick;       /* 0xFCB0  joystick and buttons */
  unsigned char switches;       /* 0xFCB1  other switches */
  unsigned char cart0;          /* 0xFCB2  cart0 r/w */
  unsigned char cart1;          /* 0xFCB3  cart1 r/w */
  unsigned char unused8[8];     /* 0xFCB4 - 0xFCBF  unused */
  unsigned char leds;           /* 0xFCC0  leds */
  unsigned char unused9;        /* 0xFCC1  unused */
  unsigned char parstat;        /* 0xFCC2  parallel port status */
  unsigned char pardata;        /* 0xFCC3  parallel port data */
  unsigned char howie;          /* 0xFCC4  howie (?) */
                                /* 0xFCC5 - 0xFCFF  unused */
};

/* Hardware math registers */
#define FACTOR_A *(unsigned int *) 0xFC54
#define FACTOR_B *(unsigned int *) 0xFC52
#define PRODUCT0 *(unsigned int *) 0xFC60
#define PRODUCT1 *(unsigned int *) 0xFC62
#define PRODUCT *(long *) 0xFC60

#define DIVIDEND0 *(unsigned int *) 0xFC60
#define DIVIDEND1 *(unsigned int *) 0xFC62
#define DIVIDEND *(long *) 0xFC60
#define DIVISOR *(unsigned int *) 0xFC56
#define QUOTIENT0 *(unsigned int *) 0xFC52
#define QUOTIENT1 *(unsigned int *) 0xFC54
#define QUOTIENT *(long *) 0xFC52
#define REMAINDER0 *(unsigned int *) 0xFC6C
#define REMAINDER1 *(unsigned int *) 0xFC6E
#define REMAINDER *(long *) 0xFC6C

/* Deprecated definitions */

/* MAPCTL $FFF9 */
#define HIGHSPEED       0x80
#define VECTORSPACE     0x08
#define ROMSPACE        0x04
#define MIKEYSPACE      0x02
#define SUZYSPACE       0x01

#endif
