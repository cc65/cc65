/*****************************************************************************/
/*                                                                           */
/*				   segdefs.h   	       	       	       	     */
/*                                                                           */
/*		Segment definitions for the bin65 binary utils		     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2002 Ullrich von Bassewitz                                       */
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



#ifndef SEGDEFS_H
#define SEGDEFS_H



/*****************************************************************************/
/*     	       	    	    	     Data				     */
/*****************************************************************************/



/* Available segment types */
#define SEGTYPE_DEFAULT	0
#define SEGTYPE_ABS	1
#define SEGTYPE_ZP	2
#define SEGTYPE_FAR	3

/* Fragment types in the object file */
#define FRAG_TYPEMASK	0x38		/* Mask the type of the fragment */
#define FRAG_BYTEMASK   0x07		/* Mask for byte count */

#define FRAG_LITERAL	0x00		/* Literal data */

#define FRAG_EXPR	0x08		/* Expression */
#define FRAG_EXPR8     	0x09   	       	/* 8 bit expression */
#define FRAG_EXPR16	0x0A		/* 16 bit expression */
#define FRAG_EXPR24	0x0B		/* 24 bit expression */
#define FRAG_EXPR32	0x0C		/* 32 bit expression */

#define FRAG_SEXPR	0x10		/* Signed expression */
#define FRAG_SEXPR8    	0x11 		/* 8 bit signed expression */
#define FRAG_SEXPR16   	0x12 		/* 16 bit signed expression */
#define FRAG_SEXPR24  	0x13		/* 24 bit signed expression */
#define FRAG_SEXPR32  	0x14		/* 32 bit signed expression */

#define FRAG_FILL      	0x20		/* Fill bytes */



/* Segment definition */
typedef struct SegDef SegDef;
struct SegDef {
    char*       Name;           /* Segment name */
    unsigned    Type;           /* Segment type, see above */
};

/* Initializer for static SegDefs */
#define STATIC_SEGDEF_INITIALIZER(name, type) { (name), (type) }



/*****************************************************************************/
/*     	       	    	    	     Code				     */
/*****************************************************************************/



SegDef* NewSegDef (const char* Name, unsigned Type);
/* Create a new segment definition and return it */

void FreeSegDef (SegDef* D);
/* Free a segment definition */

SegDef* DupSegDef (const SegDef* D);
/* Duplicate a segment definition and return it */



/* End of segdefs.h */

#endif



