/*****************************************************************************/
/*                                                                           */
/*                                segments.h                                 */
/*                                                                           */
/*                            Segment management                             */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000-2009, Ullrich von Bassewitz                                      */
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



#ifndef SEGMENTS_H
#define SEGMENTS_H



#include <stdio.h>

/* common */
#include "attrib.h"

/* cc65 */
#include "opcodes.h"


/*****************************************************************************/
/*                                 Forwards                                  */
/*****************************************************************************/



struct CodeEntry;
struct CodeLabel;
struct CodeSeg;
struct DataSeg;
struct TextSeg;
struct SymEntry;



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Segment types */
typedef enum segment_t {
    SEG_CODE,
    SEG_RODATA,
    SEG_DATA,
    SEG_BSS,
    SEG_COUNT
} segment_t;

/* A list of all segments used when generating code */
typedef struct Segments Segments;
struct Segments {
    struct TextSeg*     Text;           /* Text segment */
    struct CodeSeg*     Code;           /* Code segment */
    struct DataSeg*     Data;           /* Data segment */
    struct DataSeg*     ROData;         /* Readonly data segment */
    struct DataSeg*     BSS;            /* Segment for uninitialized data */
    segment_t           CurDSeg;        /* Current data segment */
};

/* Pointer to the current segment list. Output goes here. */
extern Segments* CS;

/* Pointer to the global segment list */
extern Segments* GS;



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



void InitSegNames (void);
/* Initialize the segment names */

void SetSegName (segment_t Seg, const char* Name);
/* Set a new name for a segment */

void PushSegName (segment_t Seg, const char* Name);
/* Push the current segment name and set a new name for a segment */

void PopSegName (segment_t Seg);
/* Restore a segment name from the segment name stack */

const char* GetSegName (segment_t Seg);
/* Get the name of the given segment */

Segments* PushSegments (struct SymEntry* Func);
/* Make the new segment list current but remember the old one */

void PopSegments (void);
/* Pop the old segment list (make it current) */

void CreateGlobalSegments (void);
/* Create the global segments and remember them in GS */

void UseDataSeg (segment_t DSeg);
/* For the current segment list, use the data segment DSeg */

struct DataSeg* GetDataSeg (void);
/* Return the current data segment */

void AddTextLine (const char* Format, ...) attribute ((format (printf, 1, 2)));
/* Add a line to the current text segment */

void AddCodeLine (const char* Format, ...) attribute ((format (printf, 1, 2)));
/* Add a line of code to the current code segment */

void AddCode (opc_t OPC, am_t AM, const char* Arg, struct CodeLabel* JumpTo);
/* Add a code entry to the current code segment */

void AddDataLine (const char* Format, ...) attribute ((format (printf, 1, 2)));
/* Add a line of data to the current data segment */

int HaveGlobalCode (void);
/* Return true if the global code segment contains entries (which is an error) */

void RemoveGlobalCode (void);
/* Remove all code from the global code segment. Used for error recovery. */

void OutputSegments (const Segments* S);
/* Output the given segments to the output file */



/* End of segments.h */

#endif
