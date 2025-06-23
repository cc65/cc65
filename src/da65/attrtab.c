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



#include <inttypes.h>

/* common */
#include "xmalloc.h"

/* da65 */
#include "cpu.h"
#include "error.h"
#include "attrtab.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Attribute structure how it is found in the attribute table */
typedef struct Attribute Attribute;
struct Attribute {
    struct Attribute*   Next;           /* Next entry in linked list */
    uint32_t            Addr;           /* The full address */
    attr_t              Attr;           /* Actual attribute */
};

/* Attributes use a hash table and a linear list for collision resolution. The
** hash function is easy and effective. It evaluates just the lower bits of
** the address.
*/
#define ATTR_HASH_SIZE          8192u   /* Must be power of two */
static Attribute* AttributeTab[ATTR_HASH_SIZE];



/*****************************************************************************/
/*                             struct Attribute                              */
/*****************************************************************************/



static Attribute* NewAttribute (uint32_t Addr, attr_t Attr)
/* Create a new attribute structure and return it */
{
    /* Create a new attribute */
    Attribute* A = xmalloc (sizeof (Attribute));

    /* Fill in the data */
    A->Next = 0;
    A->Addr = Addr;
    A->Attr = Attr;

    /* Return the attribute just created */
    return A;
}



static uint32_t GetAttributeHash (uint32_t Addr)
/* Get the hash for an attribute at the given address */
{
    return (Addr & (ATTR_HASH_SIZE - 1));
}



static Attribute* FindAttribute (uint32_t Addr)
/* Search for an attribute for the given address and return it. Returns NULL
** if no attribute exists for the address.
*/
{
    Attribute* A = AttributeTab[GetAttributeHash (Addr)];
    while (A) {
        if (A->Addr == Addr) {
            break;
        }
        A = A->Next;
    }
    return A;
}



static void InsertAttribute (Attribute* A)
/* Insert an attribute into the hash table */
{
    uint32_t Hash = GetAttributeHash (A->Addr);
    A->Next = AttributeTab[Hash];
    AttributeTab[Hash] = A;
}



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



void AddrCheck (uint32_t Addr)
/* Check if the given address has a valid range */
{
    if (Addr >= 0x10000 && CPU != CPU_65816) {
        Error ("Address out of range: $%04" PRIX32, Addr);
    }
}



attr_t GetAttr (uint32_t Addr)
/* Return the attribute for the given address */
{
    /* As a small optimization we cache the last used attribute so when the
    ** function is called several times with the same address we do already
    ** know it.
    */
    static const Attribute* A = 0;
    if (A != 0 && A->Addr == Addr) {
        return A->Attr;
    }

    /* Check the given address */
    AddrCheck (Addr);

    /* Return the attribute */
    A = FindAttribute (Addr);
    return A? A->Attr : atDefault;
}



int SegmentDefined (uint32_t Start, uint32_t End)
/* Return true if the atSegment bit is set somewhere in the given range */
{
    while (Start <= End) {
        if (GetAttr (Start++) & atSegment) {
            return 1;
        }
    }
    return 0;
}



int IsSegmentEnd (uint32_t Addr)
/* Return true if a segment ends at the given address */
{
    return (GetAttr (Addr) & atSegmentEnd) != 0x0000;
}



int IsSegmentStart (uint32_t Addr)
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
        case atSkip:     return 1;
        default:
            Internal ("GetGranularity called for style = %d", Style);
            return 0;
    }
}



void MarkRange (uint32_t Start, uint32_t End, attr_t Attr)
/* Mark a range with the given attribute */
{
    /* Do it easy here... */
    while (Start <= End) {
        MarkAddr (Start++, Attr);
    }
}



void MarkAddr (uint32_t Addr, attr_t Attr)
/* Mark an address with an attribute */
{
    /* Check the given address */
    AddrCheck (Addr);

    /* Get an existing attribute entry */
    Attribute* A = FindAttribute (Addr);

    /* We must not have more than one style bit */
    if (A != 0 && (Attr & atStyleMask) != 0) {
        if ((A->Attr & atStyleMask) != 0) {
            Error ("Duplicate style for address %04" PRIX32, Addr);
        }
    }

    /* Set the style */
    if (A) {
        A->Attr |= Attr;
    } else {
        InsertAttribute (NewAttribute (Addr, Attr));
    }
}



attr_t GetStyleAttr (uint32_t Addr)
/* Return the style attribute for the given address */
{
    /* Check the given address */
    AddrCheck (Addr);

    /* Return the attribute */
    return (GetAttr (Addr) & atStyleMask);
}



attr_t GetLabelAttr (uint32_t Addr)
/* Return the label attribute for the given address */
{
    /* Check the given address */
    AddrCheck (Addr);

    /* Return the attribute */
    return (GetAttr (Addr) & atLabelMask);
}
