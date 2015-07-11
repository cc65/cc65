/*****************************************************************************/
/*                                                                           */
/*                                 attrtab.h                                 */
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



#ifndef ATTRTAB_H
#define ATTRTAB_H



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



typedef enum attr_t {

    /* Styles */
    atDefault      = 0x0000,    /* Default style */
    atCode         = 0x0001,
    atIllegal      = 0x0002,
    atByteTab      = 0x0003,    /* Same as illegal */
    atDByteTab     = 0x0004,
    atWordTab      = 0x0005,
    atDWordTab     = 0x0006,
    atAddrTab      = 0x0007,
    atRtsTab       = 0x0008,
    atTextTab      = 0x0009,
    atSkip         = 0x000A,    /* Skip code completely */

    /* Label flags */
    atNoLabel      = 0x0000,    /* No label for this address */
    atExtLabel     = 0x0010,    /* External label */
    atIntLabel     = 0x0020,    /* Internally generated label */
    atDepLabel     = 0x0040,    /* Dependent label */
    atUnnamedLabel = 0x0080,    /* Unnamed label */

    atLabelDefined = 0x0100,    /* True if we defined the label */

    atStyleMask    = 0x000F,    /* Output style */
    atLabelMask    = 0x00F0,    /* Label information */

    /* Segment */
    atSegment      = 0x0100,    /* Code is in a segment */
    atSegmentEnd   = 0x0200,    /* Segment end */
    atSegmentStart = 0x0400,    /* Segment start */
} attr_t;



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



void AddrCheck (unsigned Addr);
/* Check if the given address has a valid range */

attr_t GetAttr (unsigned Addr);
/* Return the attribute for the given address */

int SegmentDefined (unsigned Start, unsigned End);
/* Return true if the atSegment bit is set somewhere in the given range */

int IsSegmentEnd (unsigned Addr);
/* Return true if a segment ends at the given address */

int IsSegmentStart (unsigned Addr);
/* Return true if a segment starts at the given address */

unsigned GetGranularity (attr_t Style);
/* Get the granularity for the given style */

void MarkRange (unsigned Start, unsigned End, attr_t Attr);
/* Mark a range with the given attribute */

void MarkAddr (unsigned Addr, attr_t Attr);
/* Mark an address with an attribute */

attr_t GetStyleAttr (unsigned Addr);
/* Return the style attribute for the given address */

attr_t GetLabelAttr (unsigned Addr);
/* Return the label attribute for the given address */



/* End of attrtab.h */
#endif
