/*
 * _6525.h
 *
 * Ullrich von Bassewitz, 22.09.1998
 *
 * Internal include file, do not use directly.
 *
 */



#ifndef __6525_H
#define __6525_H



/* Define a structure with the 6525 register offsets. The shadow registers
 * (if port C is unused) are currently not implemented, we would need a
 * union to do that, however that would introduce an additional name.
 */
struct __6525 {
    unsigned char	pra;		/* Port register A */
    unsigned char	prb;		/* Port register B */
    unsigned char      	prc;		/* Port register C */
    unsigned char	ddra;		/* Data direction register A */
    unsigned char	ddrb;		/* Data direction register B */
    unsigned char	ddrc;  	       	/* Data direction register C */
    unsigned char	cr;		/* Control register */
    unsigned char	air;		/* Active interrupt register */
};



/* End of _6525.h */
#endif



