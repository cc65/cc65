/*
 * _pbi.h
 *
 * Freddy Offenga, 4/25/2000
 *
 * Internal include file, do not use directly.
 * Atari parallel bus definitions
 *
 */


#ifndef __PBI_H
#define __PBI_H

/* parallel bus interface area */
#define PBI		((unsigned char*)0xD100)

/* parallel device IRQ status */
#define PDVI		((unsigned char*)0xD1FF)

/* parallel device select */
#define PDVS		((unsigned char*)0xD1FF)

/* parallel bus interface RAM area */
#define PBIRAM		((unsigned char*)0xD600)

/* parallel device ID 1 */
#define PDID1		((unsigned char*)0xD803)

/* parallel device I/O vector */
#define PDIDV		((unsigned char*)0xD805)

/* parallel device IRQ vector */
#define PDIRQV		((unsigned char*)0xD808)

/* parallel device ID 2 */
#define PDID2		((unsigned char*)0xD80B)

/* parallel device vector table */
#define PDVV		((unsigned char*)0xD80D)

/* End of _pbi.h */
#endif

