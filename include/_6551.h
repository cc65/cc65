/*
 * _6551.h
 *
 * Ullrich von Bassewitz, 22.09.1998
 *
 * Internal include file, do not use directly.
 *
 */



#ifndef __6551_H
#define __6551_H



/* Define a structure with the 6551 register offsets */
struct __6551 {
    unsigned char      	data;  	       	/* Data register */
    unsigned char      	status;	       	/* Status register */
    unsigned char      	cmd;   	       	/* Command register */
    unsigned char	ctrl;		/* Control register */
};



/* End of _6551.h */
#endif



