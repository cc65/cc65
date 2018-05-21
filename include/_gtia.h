/*****************************************************************************/
/*                                                                           */
/*                                 _gtia.h                                   */
/*                                                                           */
/*                  Internal include file, do not use directly               */
/*                                                                           */
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


#ifndef __GTIA_H
#define __GTIA_H

/* Define a structure with the gtia register offsets */
struct __gtia_write {
    unsigned char   hposp0; /* 0x00: horizontal position player 0 */
    unsigned char   hposp1; /* 0x01: horizontal position player 1 */
    unsigned char   hposp2; /* 0x02: horizontal position player 2 */
    unsigned char   hposp3; /* 0x03: horizontal position player 3 */
    unsigned char   hposm0; /* 0x04: horizontal position missile 0 */
    unsigned char   hposm1; /* 0x05: horizontal position missile 1 */
    unsigned char   hposm2; /* 0x06: horizontal position missile 2 */
    unsigned char   hposm3; /* 0x07: horizontal position missile 3 */
    unsigned char   sizep0; /* 0x08: size of player 0 */
    unsigned char   sizep1; /* 0x09: size of player 1 */
    unsigned char   sizep2; /* 0x0A: size of player 2 */
    unsigned char   sizep3; /* 0x0B: size of player 3 */
    unsigned char   sizem;  /* 0x0C: size of missiles */
    unsigned char   grafp0; /* 0x0D: graphics shape player 0 */
    unsigned char   grafp1; /* 0x0E: graphics shape player 1 */
    unsigned char   grafp2; /* 0x0F: graphics shape player 2 */
    unsigned char   grafp3; /* 0x10: graphics shape player 3 */
    unsigned char   grafm;  /* 0x11: graphics shape missiles */
    unsigned char   colpm0; /* 0x12: color player and missile 0 */
    unsigned char   colpm1; /* 0x13: color player and missile 1 */
    unsigned char   colpm2; /* 0x14: color player and missile 2 */
    unsigned char   colpm3; /* 0x15: color player and missile 3 */
    unsigned char   colpf0; /* 0x16: color playfield 0 */
    unsigned char   colpf1; /* 0x17: color playfield 1 */
    unsigned char   colpf2; /* 0x18: color playfield 2 */
    unsigned char   colpf3; /* 0x19: color playfield 3 */
    unsigned char   colbk;  /* 0x1A: color background */
    unsigned char   prior;  /* 0x1B: priority selection */
    unsigned char   vdelay; /* 0x1C: vertical delay */
    unsigned char   gractl; /* 0x1D: stick/paddle latch, p/m control */
    unsigned char   hitclr; /* 0x1E: clear p/m collision */
    unsigned char   consol; /* 0x1F: builtin speaker */
};

/* Define a structure with the gtia register offsets */
struct __gtia_read {
    unsigned char   m0pf;       /* 0x00: missile 0 to playfield collision */
    unsigned char   m1pf;       /* 0x01: missile 1 to playfield collision */
    unsigned char   m2pf;       /* 0x02: missile 2 to playfield collision */
    unsigned char   m3pf;       /* 0x03: missile 3 to playfield collision */
    unsigned char   p0pf;       /* 0x04: player 0 to playfield collision */
    unsigned char   p1pf;       /* 0x05: player 1 to playfield collision */
    unsigned char   p2pf;       /* 0x06: player 2 to playfield collision */
    unsigned char   p3pf;       /* 0x07: player 3 to playfield collision */
    unsigned char   m0pl;       /* 0x08: missile 0 to player collision */
    unsigned char   m1pl;       /* 0x09: missile 1 to player collision */
    unsigned char   m2pl;       /* 0x0A: missile 2 to player collision */
    unsigned char   m3pl;       /* 0x0B: missile 3 to player collision */
    unsigned char   p0pl;       /* 0x0C: player 0 to player collision */
    unsigned char   p1pl;       /* 0x0D: player 1 to player collision */
    unsigned char   p2pl;       /* 0x0E: player 2 to player collision */
    unsigned char   p3pl;       /* 0x0F: player 3 to player collision */
    unsigned char   trig0;      /* 0x10: joystick trigger 0 */
    unsigned char   trig1;      /* 0x11: joystick trigger 1 */
    unsigned char   trig2;      /* 0x12: joystick trigger 2 */
    unsigned char   trig3;      /* 0x13: joystick trigger 3 */
    unsigned char   pal;        /* 0x14: pal/ntsc flag */
    unsigned char   unused[10];
    unsigned char   consol;     /* 0x1F: console buttons */
};

/* End of _gtia.h */
#endif /* #ifndef __GTIA_H */

