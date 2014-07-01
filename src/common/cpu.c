/*****************************************************************************/
/*                                                                           */
/*                                   cpu.c                                   */
/*                                                                           */
/*                            CPU specifications                             */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2003-2011, Ullrich von Bassewitz                                      */
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



/* common */
#include "addrsize.h"
#include "check.h"
#include "cpu.h"
#include "strutil.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* CPU used */
cpu_t CPU = CPU_UNKNOWN;

/* Table with target names */
const char* CPUNames[CPU_COUNT] = {
    "none",
    "6502",
    "6502X",
    "65SC02",
    "65C02",
    "65816",
    "sweet16",
    "huc6280",
    "m740",
};

/* Tables with CPU instruction sets */
const unsigned CPUIsets[CPU_COUNT] = {
    CPU_ISET_NONE,
    CPU_ISET_6502,
    CPU_ISET_6502 | CPU_ISET_6502X,
    CPU_ISET_6502 | CPU_ISET_65SC02,
    CPU_ISET_6502 | CPU_ISET_65SC02 | CPU_ISET_65C02,
    CPU_ISET_6502 | CPU_ISET_65SC02 | CPU_ISET_65C02 | CPU_ISET_65816,
    CPU_ISET_SWEET16,
    CPU_ISET_6502 | CPU_ISET_65SC02 | CPU_ISET_65C02 | CPU_ISET_HUC6280,
    CPU_ISET_6502 | CPU_ISET_M740,
};



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



int ValidAddrSizeForCPU (unsigned char AddrSize)
/* Check if the given address size is valid for the current CPU */
{
    switch (AddrSize) {
        case ADDR_SIZE_DEFAULT:
            /* Always supported */
            return 1;

        case ADDR_SIZE_ZP:
            /* Not supported by None and Sweet16 */
            return (CPU != CPU_NONE && CPU != CPU_SWEET16);

        case ADDR_SIZE_ABS:
            /* Not supported by None */
            return (CPU != CPU_NONE);

        case ADDR_SIZE_FAR:
            /* Only supported by 65816 */
            return (CPU == CPU_65816);

        case ADDR_SIZE_LONG:
            /* Not supported by any CPU */
            return 0;

        default:
            FAIL ("Invalid address size");
            /* NOTREACHED */
            return 0;
    }
}



cpu_t FindCPU (const char* Name)
/* Find a CPU by name and return the target id. CPU_UNKNOWN is returned if
** the given name is no valid target.
*/
{
    unsigned I;

    /* Check all CPU names */
    for (I = 0; I < CPU_COUNT; ++I) {
        if (StrCaseCmp (CPUNames [I], Name) == 0) {
            return (cpu_t)I;
        }
    }

    /* Not found */
    return CPU_UNKNOWN;
}
