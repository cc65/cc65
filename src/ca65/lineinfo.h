/*****************************************************************************/
/*                                                                           */
/*				  lineinfo.h                                 */
/*                                                                           */
/*			Source file line info structure                      */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2001      Ullrich von Bassewitz                                       */
/*               Wacholderweg 14                                             */
/*               D-70597 Stuttgart                                           */
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



#ifndef LINEINFO_H
#define LINEINFO_H



/*****************************************************************************/
/*				     Data                                    */
/*****************************************************************************/



/* The LineInfo structure is shared between several fragments, so we need a
 * reference counter.
 */
typedef struct LineInfo LineInfo;
struct LineInfo {
    LineInfo*       Next;                 /* Pointer to next info in list */
    unsigned   	    Usage;                /* Usage counter */
    unsigned long   LineNum;              /* Line number */
    unsigned        FileIndex;            /* Index of input file */
    unsigned        Index;                /* Index */
};

/* Linked list of all line infos */
extern LineInfo* LineInfoRoot;
extern LineInfo* LineInfoLast;
extern unsigned  LineInfoCount;	  
extern unsigned  LineInfoValid;           /* Valid, that is, used entries */

/* Global pointer to last line info or NULL if not active */
extern LineInfo* CurLineInfo;



/*****************************************************************************/
/*     	       	      	      	     Code			     	     */
/*****************************************************************************/



LineInfo* UseLineInfo (LineInfo* LI);
/* Increase the reference count of the given line info and return it. The
 * function will gracefully accept NULL pointers and do nothing in this case.
 */

void GenLineInfo (unsigned FileIndex, unsigned long LineNum);
/* Generate a new line info */



/* End of lineinfo.h */
#endif



