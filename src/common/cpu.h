/*****************************************************************************/
/*                                                                           */
/*                                   cpu.h                                   */
/*                                                                           */
/*                            CPU specifications                             */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2003      Ullrich von Bassewitz                                       */
/*               Römerstrasse 52                                             */
/*               D-70794 Filderstadt                                         */
/* EMail:        uz@cc65.org                                                 */
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



#ifndef CPU_H
#define CPU_H



/*****************************************************************************/
/*     	       	    		     Data			    	     */
/*****************************************************************************/



/* CPUs */
typedef enum {
    CPU_UNKNOWN = -1,           /* Not specified or invalid target */
    CPU_6502,
    CPU_65C02,
    CPU_65816,
    CPU_SUNPLUS,	        /* Not in the freeware version - sorry */
    CPU_COUNT 	    	        /* Number of different CPUs */
} cpu_t;

/* CPU used */
extern cpu_t CPU;

/* Table with target names */
extern const char* CPUNames [CPU_COUNT];



/*****************************************************************************/
/*     	       	     		     Code      			    	     */
/*****************************************************************************/



cpu_t FindCPU (const char* Name);
/* Find a CPU by name and return the target id. CPU_UNKNOWN is returned if
 * the given name is no valid target.
 */



/* End of cpu.h */

#endif



