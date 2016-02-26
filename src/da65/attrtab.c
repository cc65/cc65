/*****************************************************************************/
/*                                                                           */
/*                                 attrtab.c                                 */
/*                                                                           */
/*                       Disassembler attribute table                        */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000-2014, Ullrich von Bassewitz                                      */
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



/* da65 */
#include "error.h"
#include "attrtab.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Attribute table */
static unsigned short AttrTab[0x10000];



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



void AddrCheck (unsigned Addr)
/* Check if the given address has a valid range */
{
    if (Addr >= 0x10000) {
        Error ("Address out of range: %08X", Addr);
    }
}



attr_t GetAttr (unsigned Addr)
/* Return the attribute for the given address */
{
    /* Check the given address */
    AddrCheck (Addr);

    /* Return the attribute */
    return AttrTab[Addr];
}



int SegmentDefined (unsigned Start, unsigned End)
/* Return true if the atSegment bit is set somewhere in the given range */
{
    while (Start <= End) {
        if (AttrTab[Start++] & atSegment) {
            return 1;
        }
    }
    return 0;
}



int IsSegmentEnd (unsigned Addr)
/* Return true if a segment ends at the given address */
{
    return (GetAttr (Addr) & atSegmentEnd) != 0x0000;
}



int IsSegmentStart (unsigned Addr)
/* Return true if a segment starts at the given address */
{
    return (GetAttr (Addr) & atSegmentStart) != 0x0000;
}



unsigned GetGranularity (attr_t Style)
/* Get the granularity for the given style */
{
    switch (Style) {
        case atDefault:  return 1;
        case atCode:     return 1;
        case atIllegal:  return 1;
        case atByteTab:  return 1;
        case atDByteTab: return 2;
        case atWordTab:  return 2;
        case atDWordTab: return 4;
        case atAddrTab:  return 2;
        case atRtsTab:   return 2;
        case atTextTab:  return 1;

        case atSkip:
        default:
            Internal ("GetGraularity called for style = %d", Style);
            return 0;
    }
}



void MarkRange (unsigned Start, unsigned End, attr_t Attr)
/* Mark a range with the given attribute */
{
    /* Do it easy here... */
    while (Start <= End) {
        MarkAddr (Start++, Attr);
    }
}



void MarkAddr (unsigned Addr, attr_t Attr)
/* Mark an address with an attribute */
{
    /* Check the given address */
    AddrCheck (Addr);

    /* We must not have more than one style bit */
    if (Attr & atStyleMask) {
        if (AttrTab[Addr] & atStyleMask) {
            Error ("Duplicate style for address %04X", Addr);
        }
    }

    /* Set the style */
    AttrTab[Addr] |= Attr;
}



attr_t GetStyleAttr (unsigned Addr)
/* Return the style attribute for the given address */
{
    /* Check the given address */
    AddrCheck (Addr);

    /* Return the attribute */
    return (AttrTab[Addr] & atStyleMask);
}



attr_t GetLabelAttr (unsigned Addr)
/* Return the label attribute for the given address */
{
    /* Check the given address */
    AddrCheck (Addr);

    /* Return the attribute */
    return (AttrTab[Addr] & atLabelMask);
}
