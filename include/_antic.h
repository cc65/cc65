/*
 * _antic.h
 *
 * Freddy Offenga, 4/9/2000
 *
 * Internal include file, do not use directly.
 *
 */


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
#endif

