/*****************************************************************************/
/*                                                                           */
/*                                addrsize.c                                 */
/*                                                                           */
/*                         Address size definitions                          */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2003-2009, Ullrich von Bassewitz                                      */
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
#include "strutil.h"



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



const char* AddrSizeToStr (unsigned char AddrSize)
/* Return the name for an address size specifier */
{
    switch (AddrSize) {
        case ADDR_SIZE_DEFAULT:         return "default";
        case ADDR_SIZE_ZP:              return "zeropage";
        case ADDR_SIZE_ABS:             return "absolute";
        case ADDR_SIZE_FAR:             return "far";
        case ADDR_SIZE_LONG:            return "long";
        default:                        return "unknown";
    }
}



unsigned char AddrSizeFromStr (const char* Str)
/* Return the address size for a given string. Returns ADDR_SIZE_INVALID if
** the string cannot be mapped to an address size.
*/
{
    static const struct {
        const char*     Name;
        unsigned char   AddrSize;
    } AddrSizeTable[] = {
        { "abs",        ADDR_SIZE_ABS     },
        { "absolute",   ADDR_SIZE_ABS     },
        { "default",    ADDR_SIZE_DEFAULT },
        { "direct",     ADDR_SIZE_ZP      },
        { "dword",      ADDR_SIZE_LONG    },
        { "far",        ADDR_SIZE_FAR     },
        { "long",       ADDR_SIZE_LONG    },
        { "near",       ADDR_SIZE_ABS     },
        { "zeropage",   ADDR_SIZE_ZP      },
        { "zp",         ADDR_SIZE_ZP      },
    };
    unsigned I;

    for (I = 0; I < sizeof (AddrSizeTable) / sizeof (AddrSizeTable[0]); ++I) {
        if (StrCaseCmp (Str, AddrSizeTable[I].Name) == 0) {
            /* Found */
            return AddrSizeTable[I].AddrSize;
        }
    }

    /* Not found */
    return ADDR_SIZE_INVALID;
}
