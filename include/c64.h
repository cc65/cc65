/*
 * c64.h
 *
 * Ullrich von Bassewitz, 12.08.1998
 */



#ifndef _C64_H
#define _C64_H



/* Additional key defines */
#define CH_F1			133
#define CH_F2			137
#define CH_F3			134
#define CH_F4			138
#define CH_F5			135
#define CH_F6			139
#define CH_F7			136
#define CH_F8			140



/* Color defines */
#define COLOR_BLACK  	       	0x00
#define COLOR_WHITE  	       	0x01
#define COLOR_RED    	       	0x02
#define COLOR_CYAN      	0x03
#define COLOR_VIOLET 	       	0x04
#define COLOR_GREEN  	       	0x05
#define COLOR_BLUE   	       	0x06
#define COLOR_YELLOW 	       	0x07
#define COLOR_ORANGE 	       	0x08
#define COLOR_BROWN  	       	0x09
#define COLOR_LIGHTRED       	0x0A
#define COLOR_GRAY1  	       	0x0B
#define COLOR_GRAY2  	       	0x0C
#define COLOR_LIGHTGREEN     	0x0D
#define COLOR_LIGHTBLUE      	0x0E
#define COLOR_GRAY3  	       	0x0F



/* Define hardware */
#include <_vic.h>
#define VIC	(*(struct __vic*)0xD000)

#include <_sid.h>
#define	SID	(*(struct __sid*)0xD400)

#include <_6526.h>
#define CIA1	(*(struct __6526*)0xDC00)
#define CIA2	(*(struct __6526*)0xDD00)



/* Define special memory areas */
#define COLOR_RAM	((unsigned char*)0xD800)



/* End of c64.h */
#endif



