/*****************************************************************************/
/*                                                                           */
/*				   symdefs.h   	       	       	       	     */
/*                                                                           */
/*               Scope definitions for the bin65 binary utils                */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2011,      Ullrich von Bassewitz                                      */
/*                Roemerstrasse 52                                           */
/*                D-70794 Filderstadt                                        */
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



#ifndef SCOPEDEFS_H
#define SCOPEDEFS_H



/*****************************************************************************/
/*     	       	    		     Data				     */
/*****************************************************************************/



/* Scope types */
enum {
    SCOPETYPE_GLOBAL,                       /* Global level */
    SCOPETYPE_FILE,                         /* File level */
    SCOPETYPE_PROC,                         /* .PROC */
    SCOPETYPE_SCOPE,                        /* .SCOPE */
    SCOPETYPE_HAS_DATA = SCOPETYPE_SCOPE,   /* Last scope that contains data */
    SCOPETYPE_STRUCT,                       /* .STRUCT/.UNION */
    SCOPETYPE_ENUM,                         /* .ENUM */
    SCOPETYPE_UNDEF    = 0xFF
};



/* End of scopedefs.h */

#endif



