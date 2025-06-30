/*****************************************************************************/
/*                                                                           */
/*                               capability.c                                */
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



#include <stdlib.h>

/* ca65 */
#include "capability.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* List of dot keywords with the corresponding ids. */
/* CAUTION: table must be sorted for bsearch. */
struct Capability {
    const char*         Key;
    capability_t        Cap;
} Capabilities [] = {
/* BEGIN SORTED.SH */
    { "CPU_HAS_BRA8",           CAP_CPU_HAS_BRA8        },
    { "CPU_HAS_INA",            CAP_CPU_HAS_INA         },
    { "CPU_HAS_PUSHXY",         CAP_CPU_HAS_PUSHXY      },
    { "CPU_HAS_STZ",            CAP_CPU_HAS_STZ         },
    { "CPU_HAS_ZPIND",          CAP_CPU_HAS_ZPIND       },
/* END SORTED.SH */
};
#define CAP_TABLE_SIZE (sizeof (Capabilities) / sizeof (Capabilities [0]))




/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



static int CmpCapability (const void* K1, const void* K2)
/* Compare function for the capability search */
{
    return strcmp (((struct Capability*)K1)->Key, ((struct Capability*)K2)->Key);
}



capability_t FindCapability (const char* Name)
/* Find the capability with the given name. Returns CAP_INVALID if there is no
** capability with the given name and a capability code >= 0 instead. The
** capability name is expected in upper case.
*/
{
    const struct Capability K = { Name, 0 };
    const struct Capability* C = bsearch (&K, Capabilities, CAP_TABLE_SIZE,
                                          sizeof (Capabilities [0]),
                                          CmpCapability);
    return (C == 0)? CAP_INVALID : C->Cap;
}
