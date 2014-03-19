/*****************************************************************************/
/*                                                                           */
/*                                 listing.h                                 */
/*                                                                           */
/*                Listing support for the ca65 crossassembler                */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000-2011, Ullrich von Bassewitz                                      */
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



#ifndef LISTING_H
#define LISTING_H



/* ca65 */
#include "fragment.h"



/*****************************************************************************/
/*                                 Forwards                                  */
/*****************************************************************************/



struct StrBuf;



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Length of the header of a listing line */
#define LINE_HEADER_LEN         24

/* One listing line as it is stored in memory */
typedef struct ListLine ListLine;
struct ListLine {
    ListLine*           Next;           /* Pointer to next line */
    Fragment*           FragList;       /* List of fragments for this line */
    Fragment*           FragLast;       /* Last entry in fragment list */
    unsigned long       PC;             /* Program counter for this line */
    unsigned char       Reloc;          /* Relocatable mode? */
    unsigned char       File;           /* From which file is the line? */
    unsigned char       Depth;          /* Include depth */
    unsigned char       Output;         /* Should we output this line? */
    unsigned char       ListBytes;      /* How many bytes at max? */
    char                Line[1];        /* Line with dynamic length */
};

/* Single linked list of lines */
extern ListLine*        LineList;       /* List of listing lines */
extern ListLine*        LineCur;        /* Current listing line */
extern ListLine*        LineLast;       /* Last listing line */

/* Page formatting */
#define MIN_PAGE_LEN    32
#define MAX_PAGE_LEN    127
extern int              PageLength;     /* Length of a listing page */

/* Byte for one listing line */
#define MIN_LIST_BYTES  4
#define MAX_LIST_BYTES  255



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



void NewListingLine (const struct StrBuf* Line, unsigned char File,
                     unsigned char Depth);
/* Create a new ListLine struct */

void EnableListing (void);
/* Enable output of lines to the listing */

void DisableListing (void);
/* Disable output of lines to the listing */

void SetListBytes (int Bytes);
/* Set the maximum number of bytes listed for one line */

void InitListingLine (void);
/* Initialize the current listing line */

void CreateListing (void);
/* Create the listing */



/* End of listing.h */

#endif
