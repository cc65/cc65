/*
 * _sid.h
 *
 * Ullrich von Bassewitz, 22.09.1998
 *
 * Internal include file, do not use directly.
 *
 */



#ifndef __SID_H
#define __SID_H



/* Define a structure with the sid register offsets */
struct __sid_voice {
    unsigned   	       	freq;		/* Frequency */
    unsigned 		pw;		/* Pulse width */
    unsigned char	ctrl;		/* Control register */
    unsigned char	ad;		/* Attack/decay */
    unsigned char	sr;		/* Sustain/release */
};
struct __sid {
    struct __sid_voice	v1;     	/* Voice 1 */
    struct __sid_voice 	v2;     	/* Voice 2 */
    struct __sid_voice	v3;     	/* Voice 3 */
    unsigned		flt_freq;   	/* Filter frequency */
    unsigned char	flt_ctrl;	/* Filter control register */
    unsigned char	amp;		/* Amplitude */
    unsigned char	ad1;		/* A/D converter 1 */
    unsigned char	noise;		/* Noise generator */
    unsigned char	read3;		/* Value of voice 3 */
};



/* End of _sid.h */
#endif



