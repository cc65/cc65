/*
 * apple2.h
 *
 * Written by Kevin Ruland.
 */



#ifndef _APPLE2_H
#define _APPLE2_H



/* Color Defines
 * Since Apple2 does not support color text these defines are only
 * used to get the library to compile correctly.  They should not be used
 * in user code
 */
#define COLOR_BLACK 	0x00
#define COLOR_WHITE 	0x01



/* Characters codes */
#define CH_DEL 	    	0x7F
#define CH_ESC 	    	0x1B
#define CH_CURS_UP  	0x0B
#define CH_CURS_DOWN 	0x0A

/* These are defined to be OpenApple + NumberKey */
#define CH_F1 	    	0xB1
#define CH_F2 	    	0xB2
#define CH_F3 	    	0xB3
#define CH_F4 	    	0xB4
#define CH_F5 	    	0xB5
#define CH_F6 	    	0xB6
#define CH_F7 	    	0xB7
#define CH_F8 	    	0xB8
#define CH_F9 	    	0xB9
#define CH_F10 	    	0xB0

#define CH_ULCORNER 	'+'
#define CH_URCORNER 	'+'
#define CH_LLCORNER 	'+'
#define CH_LRCORNER 	'+'
#define CH_TTEE     	'+'
#define CH_BTEE     	'+'
#define CH_LTEE     	'+'
#define CH_RTEE     	'+'
#define CH_CROSS    	'+'



/* End of apple2.h */
#endif



