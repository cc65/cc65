/*****************************************************************************/
/*                                                                           */
/*                                 config.h                                  */
/*                                                                           */
/*               Target configuration file for the ld65 linker               */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2012, Ullrich von Bassewitz                                      */
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



#ifndef CONFIG_H
#define CONFIG_H



/* common */
#include "coll.h"
#include "filepos.h"

/* ld65 */
#include "lineinfo.h"
#include "segments.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Forward for struct MemoryArea */
struct MemoryArea;

/* File list entry */
typedef struct File File;
struct File {
    unsigned            Name;           /* Name index of the file */
    unsigned            Flags;
    unsigned            Format;         /* Output format */
    unsigned long       Size;           /* Size of the generated file */
    Collection          MemoryAreas;    /* List of memory areas in this file */
};

/* Segment descriptor entry */
typedef struct SegDesc SegDesc;
struct SegDesc {
    unsigned            Name;           /* Index of the name */
    LineInfo*           LI;             /* Position of definition */
    Segment*            Seg;            /* Pointer to segment structure */
    unsigned            Attr;           /* Attributes for segment */
    unsigned            Flags;          /* Set of bitmapped flags */
    unsigned char       FillVal;        /* Fill value for this segment */
    struct MemoryArea*  Load;           /* Load memory section */
    struct MemoryArea*  Run;            /* Run memory section */
    unsigned long       Addr;           /* Start address or offset into segment */
    unsigned long       RunAlignment;   /* Run area alignment if given */
    unsigned long       LoadAlignment;  /* Load area alignment if given */
};

/* Segment flags */
#define SF_RO           0x0001          /* Read only segment */
#define SF_BSS          0x0002          /* Segment is BSS style segment */
#define SF_ZP           0x0004          /* Zeropage segment (o65 only) */
#define SF_DEFINE       0x0008          /* Define start and size */
#define SF_ALIGN        0x0010          /* Align segment in run area */
#define SF_ALIGN_LOAD   0x0020          /* Align segment in load area */
#define SF_OFFSET       0x0040          /* Segment has offset in memory */
#define SF_START        0x0080          /* Segment has fixed start address */
#define SF_OPTIONAL     0x0100          /* Segment is optional (must not exist) */
#define SF_RUN_DEF      0x0200          /* RUN symbols already defined */
#define SF_LOAD_DEF     0x0400          /* LOAD symbols already defined */
#define SF_FILLVAL      0x0800          /* Segment has separate fill value */



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



void CfgRead (void);
/* Read the configuration */

unsigned CfgProcess (void);
/* Process the config file after reading in object files and libraries. This
** includes postprocessing of the config file data but also assigning segments
** and defining segment/memory area related symbols. The function will return
** the number of memory area overflows (so zero means anything went ok).
** In case of overflows, a short mapfile can be generated later, to ease the
** task of rearranging segments for the user.
*/

void CfgWriteTarget (void);
/* Write the target file(s) */



/* End of config.h */

#endif
