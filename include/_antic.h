/*****************************************************************************/
/*                                                                           */
/*				  _antic.h			       	     */
/*                                                                           */
/*		    Internal include file, do not use directly		     */
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


#ifndef __ANTIC_H
#define __ANTIC_H

/* Define a structure with the antic register offsets */
struct __antic {
    unsigned char   dmactl; /* direct memory access control */
    unsigned char   chactl; /* character mode control */
    unsigned char   dlistl; /* display list pointer low-byte */
    unsigned char   dlisth; /* display list pointer high-byte */
    unsigned char   hscrol; /* horizontal scroll enable */
    unsigned char   vscrol; /* vertical scroll enable */
    unsigned char   unuse0; /* unused */
    unsigned char   pmbase; /* msb of p/m base address */
    unsigned char   unuse1; /* unused */
    unsigned char   chbase; /* character base address */
    unsigned char   wsync;  /* wait for horizontal synchronization */
    unsigned char   vcount; /* vertical line counter */
    unsigned char   penh;   /* light pen horizontal position */
    unsigned char   penv;   /* light pen vertical position */
    unsigned char   nmien;  /* non-maskable interrupt enable */
    unsigned char   nmires; /* nmi reset/status */
};

/* End of _antic.h */
#endif /* #ifndef __ANTIC_H */

