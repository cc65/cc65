/*****************************************************************************/
/*                                                                           */
/*                                 segment.h                                 */
/*                                                                           */
/*                         Segment handling for da65                         */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2007-2014, Ullrich von Bassewitz                                      */
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



#ifndef SEGMENT_H
#define SEGMENT_H



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



void AddAbsSegment (uint32_t Start, uint32_t End, const char* Name);
/* Add an absolute segment to the segment table */

char* GetSegmentStartName (uint32_t Addr);
/* Return the name of the segment which starts at the given address */

unsigned GetSegmentAddrSize (uint32_t Addr);
/* Return the address size of the segment which starts at the given address */



/* End of segment.h */
#endif
