/*
 * _pia.h
 *
 * Freddy Offenga, 4/9/2000
 *
 * Internal include file, do not use directly.
 *
 */


#ifndef __PIA_H
#define __PIA_H


/* Define a structure with the pia register offsets */
struct __pia {
    unsigned char   porta;  /* port A data r/w */
    unsigned char   portb;  /* port B data r/w */
    unsigned char   pactl;  /* port A control */
    unsigned char   pbctl;  /* port B control */
};

/* End of _pia.h */
#endif

