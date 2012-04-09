/*****************************************************************************/
/*                                                                           */
/*		  		   cpudata.h				     */
/*                                                                           */
/*                   CPU data passed from the CPU plugins                    */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2012,      Ullrich von Bassewitz                                      */
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



#ifndef CPUDATA_H
#define CPUDATA_H



/*****************************************************************************/
/*                                     Data                                  */
/*****************************************************************************/



/* Version defines */
#define CPUDATA_VER_MAJOR       1U
#define CPUDATA_VER_MINOR       0U

/* Forwards */
struct SimData;

/* CPUData structure */
typedef struct CPUData CPUData;
struct CPUData {
    const char*     CPUName;            /* Name of the chip */
    unsigned        MajorVersion;       /* Version information */
    unsigned        MinorVersion;                             

    /* -- Exported functions -- */

    void (*Init) (const struct SimData* Data);
    /* Initialize the CPU module */

    void* (*CreateInstance) (void* CfgInfo);
    /* Create an instance of the CPU. Return the instance data pointer */

    void (*DestroyInstance) (void* Data);
    /* Destroy an instance of the CPU */

    void (*Reset) (void* Data);
    /* Generate a CPU RESET */

    void (*IRQRequest) (void* Data);
    /* Generate an IRQ */

    void (*NMIRequest) (void* Data);
    /* Generate an NMI */

    unsigned (*ExecuteInsn) (void* Data);
    /* Execute one CPU instruction. Return the number of clock cycles for the
     * executed instruction.
     */

    unsigned long (*GetCycles) (void* Data);
    /* Return the total number of clock cycles executed */
};



/* End of cpudata.h */

#endif



