/*****************************************************************************/
/*                                                                           */
/*				   reginfo.h                                 */
/*                                                                           */
/*			  6502 register tracking info                        */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2001-2002  Ullrich von Bassewitz                                      */
/*                Wacholderweg 14                                            */
/*                D-70597 Stuttgart                                          */
/* EMail:         uz@cc65.org                                                */
/*                                                                           */
/*                                                                           */
/* This software is provided 'as-is', without any expressed or implied       */
/* warranty.  In no event will the authors be held liable for any damages    */
/* arising from the use of this software.                                    */
/*                                                                           */
/* Permission is granted to anyone to use this software for any purpose,     */
/* including commercial applications, and to alter it and redistribute it    */
/* freely, subject to the following restrictions:                            */
/*                                                                           */
/* 1. The origin of this software must not be misrepresented; you must not   */
/*    claim that you wrote the original software. If you use this software   */
/*    in a product, an acknowledgment in the product documentation would be  */
/*    appreciated but is not required.                                       */
/* 2. Altered source versions must be plainly marked as such, and must not   */
/*    be misrepresented as being the original software.                      */
/* 3. This notice may not be removed or altered from any source              */
/*    distribution.                                                          */
/*                                                                           */
/*****************************************************************************/



#ifndef REGINFO_H
#define REGINFO_H



/* common */
#include "inline.h"



/*****************************************************************************/
/*  	       	   	       	     Data				     */
/*****************************************************************************/



/* Encoding for an unknown register value */
#define UNKNOWN_REGVAL  -1

/* Register contents */
typedef struct RegContents RegContents;
struct RegContents {
    short      	RegA;
    short       RegX;
    short       RegY;
    short       SRegLo;
    short       SRegHi;
    short	Tmp1;
};

/* Register change info */
typedef struct RegInfo RegInfo;
struct RegInfo {
    RegContents	In;             /* Incoming register values */
    RegContents Out;            /* Outgoing register values */
    RegContents Out2;		/* Alternative outgoing reg values for branches */
};



/*****************************************************************************/
/*     	       	      	   	     Code		    		     */
/*****************************************************************************/



void RC_Invalidate (RegContents* C);
/* Invalidate all registers */

void RC_InvalidateZP (RegContents* C);
/* Invalidate all ZP registers */

#if defined(HAVE_INLINE)
INLINE int RegValIsKnown (short Val)
/* Return true if the register value is known */
{
    return (Val >= 0);
}
#else
#  define RegValIsKnown(S)      ((S) >= 0)
#endif

#if defined(HAVE_INLINE)
INLINE int RegValIsUnknown (short Val)
/* Return true if the register value is not known */
{
    return (Val < 0);
}
#else
#  define RegValIsUnknown(S)      ((S) < 0)
#endif

RegInfo* NewRegInfo (const RegContents* RC);
/* Allocate a new register info, initialize and return it. If RC is not
 * a NULL pointer, it is used to initialize both, the input and output
 * registers. If the pointer is NULL, all registers are set to unknown.
 */

void FreeRegInfo (RegInfo* RI);
/* Free a RegInfo struct */





/* End of reginfo.h */
#endif




