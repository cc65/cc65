/*****************************************************************************/
/*                                                                           */
/*				   config.h				     */
/*                                                                           */
/*		 Target configuration file for the ld65 linker		     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2003 Ullrich von Bassewitz                                       */
/*               Römerstrasse 52                                             */
/*               D-70794 Filderstadt                                         */
/* EMail:        uz@cc65.org                                                 */
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



#include "segments.h"



/*****************************************************************************/
/*     	      	    		     Data				     */
/*****************************************************************************/



/* File list entry */
typedef struct File File;
struct File {
    unsigned            Name;           /* Name index of the file */
    File*     		Next;	  	/* Pointer to next entry in list */
    unsigned		Flags;
    unsigned		Format;		/* Output format */
    struct Memory*	MemList;  	/* List of memory areas in this file */
    struct Memory* 	MemLast;  	/* Last memory area in this file */
};

/* Segment list node. Needed because there are two lists (RUN & LOAD) */
typedef struct MemListNode MemListNode;
struct MemListNode {
    MemListNode*  	Next;	 	/* Next entry */
    struct SegDesc*	Seg;	 	/* Segment */
};

/* Memory list entry */
typedef struct Memory Memory;
struct Memory {
    unsigned            Name;           /* Name index of the memory section */
    Memory*  	    	Next;	  	/* Pointer to next entry in list */
    Memory*    	       	FNext;	  	/* Next in file list */
    unsigned   	       	Attr;	  	/* Which values are valid? */
    unsigned 		Flags;	  	/* Set of bitmapped flags */
    unsigned long   	Start;	  	/* Start address */
    unsigned long      	Size; 	  	/* Length of memory section */
    unsigned long      	FillLevel;	/* Actual fill level of segment */
    unsigned char   	FillVal;  	/* Value used to fill rest of seg */
    MemListNode* 	SegList;  	/* List of segments for this section */
    MemListNode* 	SegLast;  	/* Last segment in this section */
    File*    	    	F;    	  	/* File that contains the entry */
};

/* Segment descriptor entry */
typedef struct SegDesc SegDesc;
struct SegDesc {
    unsigned            Name;           /* Index of the name */
    SegDesc*   	      	Next;	  	/* Pointer to next entry in list */
    Segment*   	      	Seg; 	  	/* Pointer to segment structure */
    unsigned   	      	Attr;	  	/* Attributes for segment */
    unsigned   	      	Flags;	  	/* Set of bitmapped flags */
    Memory*           	Load;  	       	/* Load memory section */
    Memory*    	      	Run;	  	/* Run memory section */
    unsigned long      	Addr; 		/* Start address or offset into segment */
    unsigned char     	Align;	  	/* Alignment if given */
};

/* Segment list */
extern SegDesc*	       	SegDescList;	/* Single linked list */
extern unsigned	       	SegDescCount;	/* Number of entries in list */

/* Memory flags */
#define MF_DEFINE      	0x0001	  	/* Define start and size */
#define MF_FILL	       	0x0002	  	/* Fill segment */
#define MF_RO	       	0x0004	  	/* Read only memory area */

/* Segment flags */
#define SF_RO  	      	0x0001	  	/* Read only segment */
#define SF_BSS 	      	0x0002	  	/* Segment is BSS style segment */
#define SF_ZP  	      	0x0004		/* Zeropage segment (o65 only) */
#define SF_WPROT      	0x0008		/* Write protected segment */
#define SF_DEFINE      	0x0010	  	/* Define start and size */
#define SF_ALIGN      	0x0020	  	/* Align the segment */
#define SF_OFFSET     	0x0040		/* Segment has offset in memory */
#define SF_START      	0x0080	  	/* Segment has fixed start address */
#define SF_OPTIONAL     0x0100          /* Segment is optional (must not exist) */
#define SF_LOAD_AND_RUN	0x1000 	       	/* LOAD and RUN given */
#define SF_RUN_DEF     	0x2000		/* RUN symbols already defined */
#define SF_LOAD_DEF   	0x4000		/* LOAD symbols already defined */



/*****************************************************************************/
/*     	       	       	       	     Code     				     */
/*****************************************************************************/



void CfgRead (void);
/* Read the configuration */

void CfgAssignSegments (void);
/* Assign segments, define linker symbols where requested */

void CfgWriteTarget (void);
/* Write the target file(s) */



/* End of config.h */

#endif





