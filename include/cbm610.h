/*
 * cbm610.h
 *
 * Ullrich von Bassewitz, 12.08.1998
 */



#ifndef _CBM610_H
#define _CBM610_H



/* Additional key defines */
#define CH_F1			224
#define CH_F2			225
#define CH_F3			226
#define CH_F4			227
#define CH_F5			228
#define CH_F6			229
#define CH_F7			230
#define CH_F8			231
#define CH_F9			232
#define CH_F10			233
#define CH_F11			234
#define CH_F12			235
#define CH_F13			236
#define CH_F14			237
#define CH_F15			238
#define CH_F16			239
#define CH_F17			240
#define CH_F18			241
#define CH_F19			242
#define CH_F20			243



/* Color defines */
#define COLOR_BLACK  	       	0x00
#define COLOR_WHITE  	       	0x01



/* Special routines to write bytes and words in the system bank */
void __fastcall__ pokebsys (unsigned addr, unsigned char val);
void __fastcall__ pokewsys (unsigned addr, unsigned val);



/* Define hardware */
#include <_6545.h>
#define CRTC	(*(struct __6545)0xD800)

#include <_sid.h>
#define	SID	(*(struct __sid*)0xDA00)

#include <_6526.h>
#define CIA	(*(struct __cia*)0xDC00)

#include <_6551.h>
#define ACIA	(*(struct __6551*)0xDD00)

#include <_6525.h>
#define TPI1	(*(struct __6525*)0xDE00)
#define TPI2	(*(struct __6525*)0xDF00)



/* End of cbm610.h */
#endif



