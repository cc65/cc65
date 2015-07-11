/*****************************************************************************/
/*                                                                           */
/*                                 target.h                                  */
/*                                                                           */
/*                           Target specification                            */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000-2011, Ullrich von Bassewitz                                      */
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



#ifndef TARGET_H
#define TARGET_H



/* common */
#include "cpu.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Supported target systems */
typedef enum {
    TGT_UNKNOWN = -1,           /* Not specified or invalid target */
    TGT_NONE,
    TGT_MODULE,
    TGT_ATARI,
    TGT_ATARI5200,
    TGT_ATARIXL,
    TGT_VIC20,
    TGT_C16,
    TGT_C64,
    TGT_C128,
    TGT_PLUS4,
    TGT_CBM510,
    TGT_CBM610,
    TGT_OSIC1P,
    TGT_PET,
    TGT_BBC,
    TGT_APPLE2,
    TGT_APPLE2ENH,
    TGT_GEOS_CBM,
    TGT_GEOS_APPLE,
    TGT_LUNIX,
    TGT_ATMOS,
    TGT_NES,
    TGT_SUPERVISION,
    TGT_LYNX,
    TGT_SIM6502,
    TGT_SIM65C02,
    TGT_PCENGINE,
    TGT_COUNT                   /* Number of target systems */
} target_t;

/* Collection of target properties */
typedef struct TargetProperties TargetProperties;
struct TargetProperties {
    const char              Name[12];   /* Name of the target */
    cpu_t                   DefaultCPU; /* Default CPU for this target */
    unsigned char           BinFmt;     /* Default binary format for this target */
    const unsigned char*    CharMap;    /* Character translation table */
};

/* Target system */
extern target_t         Target;

/* Types of available output formats */
#define BINFMT_DEFAULT          0       /* Default (binary) */
#define BINFMT_BINARY           1       /* Straight binary format */
#define BINFMT_O65              2       /* Andre Fachats o65 format */



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



target_t FindTarget (const char* Name);
/* Find a target by name and return the target id. TGT_UNKNOWN is returned if
** the given name is no valid target.
*/

const TargetProperties* GetTargetProperties (target_t Target);
/* Return the properties for a target */

const char* GetTargetName (target_t Target);
/* Return the name of a target */



/* End of target.h */

#endif
