/*****************************************************************************/
/*                                                                           */
/*				 tgtcfg.c				     */
/*                                                                           */
/*		 Target machine configurations the ld65 linker		     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2000 Ullrich von Bassewitz                                       */
/*               Wacholderweg 14                                             */
/*               D-70597 Stuttgart                                           */
/* EMail:        uz@musoftware.de                                            */
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



#include "binfmt.h"
#include "tgtcfg.h"



/*****************************************************************************/
/*			     Target configurations			     */
/*****************************************************************************/


				  
/* An empty config */
static const char CfgEmpty[] = "";
		     
/* Actual target configurations, converted into C strings by a perl script */
#include "apple2.inc"
#include "atari.inc"
#include "c128.inc"
#include "c64.inc"
#include "cbm610.inc"
#include "geos.inc"
#include "none.inc"
#include "pet.inc"
#include "plus4.inc"



/*****************************************************************************/
/*     	      	    		     Data     	 			     */
/*****************************************************************************/



/* Target configurations for all systems */
const TargetDesc Targets [TGT_COUNT] = {
    {  	BINFMT_BINARY,  CfgNone 	},
    {  	BINFMT_BINARY,	CfgAtari	},
    {  	BINFMT_BINARY,	CfgC64 		},
    {  	BINFMT_BINARY,	CfgC128		},
    {  	BINFMT_BINARY,	CfgEmpty	},	/* Ace */
    {  	BINFMT_BINARY,	CfgPlus4	},
    {  	BINFMT_BINARY,  CfgCBM610	},
    {  	BINFMT_BINARY,	CfgPET		},
    {  	BINFMT_BINARY,	CfgEmpty	},	/* NES */
    {   BINFMT_BINARY,	CfgApple2	},
    {   BINFMT_BINARY,  CfgGeos		},
};



					  
