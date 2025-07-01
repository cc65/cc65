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



#include <stdint.h>

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
    "6502DTV",
    "65SC02",   /* the original CMOS instruction set */
    "65C02",    /* CMOS with Rockwell extensions */
    "65816",
    "sweet16",
    "huc6280",
    "m740",
    "4510",
    "45GS02",
    "W65C02",   /* CMOS with WDC extensions */
    "65CE02",   /* CMOS with CSG extensions */
};

/* Tables with CPU instruction sets
 * NOTE: make sure to only combine the instruction sets that are 100% compatible
 */
const unsigned CPUIsets[CPU_COUNT] = {
    CPU_ISET_NONE,
    CPU_ISET_6502,
    CPU_ISET_6502X   | CPU_ISET_6502,
    CPU_ISET_6502DTV | CPU_ISET_6502,
    CPU_ISET_65SC02  | CPU_ISET_6502,
    CPU_ISET_65C02   | CPU_ISET_6502 | CPU_ISET_65SC02,
    /* 65816 has wai/stp and NO bit manipulation */
    CPU_ISET_65816   | CPU_ISET_6502 | CPU_ISET_65SC02,
    CPU_ISET_SWEET16,
    CPU_ISET_HUC6280 | CPU_ISET_6502 | CPU_ISET_65SC02 | CPU_ISET_65C02,
    CPU_ISET_M740    | CPU_ISET_6502,
    /* 4510 does NOT have indirect-zp (without z), so we can not use 65SC02 */
    CPU_ISET_4510    | CPU_ISET_6502                   | CPU_ISET_65C02 | CPU_ISET_65CE02,
    CPU_ISET_45GS02  | CPU_ISET_6502                   | CPU_ISET_65C02 | CPU_ISET_65CE02 | CPU_ISET_4510,
    CPU_ISET_W65C02  | CPU_ISET_6502 | CPU_ISET_65SC02 | CPU_ISET_65C02,
    CPU_ISET_65CE02  | CPU_ISET_6502                   | CPU_ISET_65C02,
};

/* Defines for capabilities. Currently the entries are uint32_ts but the table
** is deliberately hidden from the outside so it can be extended to 64 bit or
** even more.
*/
#define CAP_BIT(Cap)    (UINT32_C (1) << (Cap))
#define CAP_NONE        UINT32_C (0)
#define CAP_6502        CAP_NONE
#define CAP_6502X       CAP_NONE
#define CAP_6502DTV     CAP_NONE
#define CAP_65SC02                              \
    (CAP_BIT (CAP_CPU_HAS_BITIMM)       |       \
     CAP_BIT (CAP_CPU_HAS_BRA8)         |       \
     CAP_BIT (CAP_CPU_HAS_INA)          |       \
     CAP_BIT (CAP_CPU_HAS_PUSHXY)       |       \
     CAP_BIT (CAP_CPU_HAS_ZPIND)        |       \
     CAP_BIT (CAP_CPU_HAS_STZ))
#define CAP_65C02                               \
    (CAP_BIT (CAP_CPU_HAS_BITIMM)       |       \
     CAP_BIT (CAP_CPU_HAS_BRA8)         |       \
     CAP_BIT (CAP_CPU_HAS_INA)          |       \
     CAP_BIT (CAP_CPU_HAS_PUSHXY)       |       \
     CAP_BIT (CAP_CPU_HAS_ZPIND)        |       \
     CAP_BIT (CAP_CPU_HAS_STZ))
#define CAP_65816                               \
    (CAP_BIT (CAP_CPU_HAS_BITIMM)       |       \
     CAP_BIT (CAP_CPU_HAS_BRA8)         |       \
     CAP_BIT (CAP_CPU_HAS_INA)          |       \
     CAP_BIT (CAP_CPU_HAS_PUSHXY)       |       \
     CAP_BIT (CAP_CPU_HAS_ZPIND)        |       \
     CAP_BIT (CAP_CPU_HAS_STZ))
#define CAP_SWEET16     CAP_NONE
#define CAP_HUC6280                             \
    (CAP_BIT (CAP_CPU_HAS_BITIMM)       |       \
     CAP_BIT (CAP_CPU_HAS_BRA8)         |       \
     CAP_BIT (CAP_CPU_HAS_INA)          |       \
     CAP_BIT (CAP_CPU_HAS_PUSHXY)       |       \
     CAP_BIT (CAP_CPU_HAS_ZPIND)        |       \
     CAP_BIT (CAP_CPU_HAS_STZ))
#define CAP_M740                                \
    (CAP_BIT (CAP_CPU_HAS_BRA8)         |       \
     CAP_BIT (CAP_CPU_HAS_INA))
#define CAP_4510                                \
    (CAP_BIT (CAP_CPU_HAS_BRA8)         |       \
     CAP_BIT (CAP_CPU_HAS_INA)          |       \
     CAP_BIT (CAP_CPU_HAS_PUSHXY))
#define CAP_45GS02                              \
    (CAP_BIT (CAP_CPU_HAS_BRA8)         |       \
     CAP_BIT (CAP_CPU_HAS_INA)          |       \
     CAP_BIT (CAP_CPU_HAS_PUSHXY))
#define CAP_W65C02                              \
    (CAP_BIT (CAP_CPU_HAS_BRA8)         |       \
     CAP_BIT (CAP_CPU_HAS_INA)          |       \
     CAP_BIT (CAP_CPU_HAS_PUSHXY))
#define CAP_65CE02                              \
    (CAP_BIT (CAP_CPU_HAS_BRA8)         |       \
     CAP_BIT (CAP_CPU_HAS_INA)          |       \
     CAP_BIT (CAP_CPU_HAS_PUSHXY))

/* Table containing one capability entry per CPU */
static const uint32_t CPUCaps[CPU_COUNT] = {
    CAP_NONE,                   /* CPU_NONE */
    CAP_6502,                   /* CPU_6502 */
    CAP_6502X,                  /* CPU_6502X */
    CAP_6502DTV,                /* CPU_6502DTV */
    CAP_65SC02,                 /* CPU_65SC02 */
    CAP_65C02,                  /* CPU_65C02 */
    CAP_65816,                  /* CPU_65816 */
    CAP_SWEET16,                /* CPU_SWEET16 */
    CAP_HUC6280,                /* CPU_HUC6280 */
    CAP_M740,                   /* CPU_M740 */
    CAP_4510,                   /* CPU_4510 */
    CAP_45GS02,                 /* CPU_45GS02 */
    CAP_W65C02,                 /* CPU_W65C02 */
    CAP_65CE02,                 /* CPU_65CE02 */
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
            /* Not supported by Sweet16 */
            return (CPU != CPU_SWEET16);

        case ADDR_SIZE_ABS:
            /* Always supported */
            return 1;

        case ADDR_SIZE_FAR:
            /* Supported by "none" and 65816 */
            return (CPU == CPU_NONE || CPU == CPU_65816);

        case ADDR_SIZE_LONG:
            /* "none" supports all sizes */
            return (CPU == CPU_NONE);

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



int CPUHasCap (capability_t Cap)
/* Check if the current CPU has the given capability */
{
    PRECONDITION (CPU >= 0 && CPU < CPU_COUNT);
    return (CPUCaps[CPU] & CAP_BIT (Cap)) != 0;
}
