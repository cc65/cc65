/*
 * _vdc.h
 *
 * Ullrich von Bassewitz, 22.09.1998
 *
 * Internal include file, do not use directly.
 *
 */



#ifndef __VDC_H
#define __VDC_H



/* Define a structure with the vdc register offsets */
struct __vdc {
    unsigned char      	ctrl;		/* Control register */
    unsigned char	data;	  	/* Data register */
};



/* End of _vdc.h */
#endif



