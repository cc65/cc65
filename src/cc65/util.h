/*
 * util.h
 *
 * Ullrich von Bassewitz, 18.06.1998
 */



#ifndef UTIL_H
#define UTIL_H



/*****************************************************************************/
/*	      			     code		     		     */
/*****************************************************************************/



int IsQuoteChar (char c);
/* Return true if c is a single or double quote */

int powerof2 (unsigned long val);
/* Return the exponent if val is a power of two. Return -1 if val is not a
 * power of two.
 */



/* End of util.h */

#endif



