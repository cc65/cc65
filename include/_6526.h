/*
 * _6526.h
 *
 * Ullrich von Bassewitz, 22.09.1998
 *
 * Internal include file, do not use directly.
 *
 */



#ifndef __6526_H
#define __6526_H



/* Define a structure with the 6526 register offsets */
struct __6526 {
    unsigned char	pra;		/* Port register A */
    unsigned char	prb;		/* Port register B */
    unsigned char	ddra;		/* Data direction register A */
    unsigned char	ddrb;		/* Data direction register B */
    unsigned char	ta_lo;		/* Timer A, low byte */
    unsigned char	ta_hi;		/* Timer A, high byte */
    unsigned char      	tb_lo; 	       	/* Timer B, low byte */
    unsigned char	tb_hi;		/* Timer B, high byte */
    unsigned char      	tod_10;		/* TOD, 1/10 sec. */
    unsigned char	tod_sec;	/* TOD, seconds */
    unsigned char	tod_min;	/* TOD, minutes */
    unsigned char	tod_hour;	/* TOD, hours */
    unsigned char	sdr;		/* Serial data register */
    unsigned char	icr;		/* Interrupt control register */
    unsigned char	cra;		/* Control register A */
    unsigned char	crb;		/* Control register B */
};



/* End of _6526.h */
#endif



