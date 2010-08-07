/*****************************************************************************/
/*                                                                           */
/*				  lineinfo.h                                 */
/*                                                                           */
/*			Source file line info structure                      */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2001-2010, Ullrich von Bassewitz                                      */
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



#ifndef LINEINFO_H
#define LINEINFO_H



#include <stdio.h>

/* common */
#include "coll.h"
#include "filepos.h"



/*****************************************************************************/
/*	       			   Forwards                                  */
/*****************************************************************************/



struct ObjData;
struct Segment;



/*****************************************************************************/
/*  	       	 		     Data                                    */
/*****************************************************************************/



typedef struct CodeRange CodeRange;
struct CodeRange {
    struct Segment*     Seg;            /* Segment of this code range */
    unsigned long       Offs;           /* Offset of code range */
    unsigned long       Size;           /* Size of code range */
};



typedef struct LineInfo LineInfo;
struct LineInfo {
    struct FileInfo*    File;	        /* File struct for this line */
    FilePos             Pos;            /* File position */
    Collection          Fragments;      /* Fragments for this line */
    Collection          CodeRanges;     /* Code ranges for this line */
};



/*****************************************************************************/
/*     	       	      	      	     Code			     	     */
/*****************************************************************************/



LineInfo* NewLineInfo (struct ObjData* O, const FilePos* Pos);
/* Create and return a new LineInfo struct */

LineInfo* ReadLineInfo (FILE* F, struct ObjData* O);
/* Read a line info from a file and return it */

void RelocLineInfo (struct Segment* S);
/* Relocate the line info for a segment. */



/* End of lineinfo.h */
#endif



