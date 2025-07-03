/*****************************************************************************/
/*                                                                           */
/*                               capability.h                                */
/*                                                                           */
/*                     Handle CPU or target capabilities                     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2026,     Kugelfuhr                                                   */
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



#ifndef CAPABILITY_H
#define CAPABILITY_H



/* common */
#include "strbuf.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Numeric codes for capabilities */
enum capability_t {
    CAP_INVALID             = -1,
    CAP_CPU_HAS_BRA8        =  0,   /* CPU has a BRA 8-bit instruction */
    CAP_CPU_HAS_INA         =  1,   /* CPU has DEA/INA */
    CAP_CPU_HAS_PUSHXY      =  2,   /* CPU has PHX/PHY/PLX/PLY */
    CAP_CPU_HAS_ZPIND       =  3,   /* CPU has "(zp)" mode (no offset) */
    CAP_CPU_HAS_STZ         =  4,   /* CPU has "store zero" (!) instruction */
    CAP_CPU_HAS_BITIMM      =  5,   /* CPU has "bit #imm" instruction */
};
typedef enum capability_t capability_t;



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



capability_t FindCapability (const char* Name);
/* Find the capability with the given name. Returns CAP_INVALID if there is no
** capability with the given name and a capability code >= 0 instead. The
** capability name is expected in upper case.
*/



/* End of capability.h */

#endif
