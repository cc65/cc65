/*****************************************************************************/
/*                                                                           */
/*                                   ace.h                                   */
/*                                                                           */
/*                      ACE system-specific definitions                      */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2015, Ullrich von Bassewitz                                      */
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



#ifndef _ACE_H
#define _ACE_H



/* Check for errors */
#if !defined(__ACE__)
#  error This module may only be used when compiling for the ACE os!
#endif



#ifndef _STDDEF_H
#include <stddef.h>
#endif



struct aceDirentBuf {
    unsigned long   ad_size;            /* Size in bytes */
    unsigned char   ad_date [8];        /* YY:YY:MM:DD:HH:MM:SS:TW */
    char            ad_type [4];        /* File type as ASCIIZ string */
    unsigned char   ad_flags;           /* File flags */
    unsigned char   ad_usage;           /* More flags */
    unsigned char   ad_namelen;         /* Length of name */
    char            ad_name [17];       /* Name itself, ASCIIZ */
};

int __cdecl__ aceDirOpen (char* dir);
int __cdecl__ aceDirClose (int handle);
int __cdecl__ aceDirRead (int handle, struct aceDirentBuf* buf);

/* Type of an ACE key. Key in low byte, shift mask in high byte */
typedef unsigned int aceKey;

/* #defines for the shift mask returned by aceConGetKey */
#define aceSH_KEY               0x00FF  /* Mask key itself */
#define aceSH_MASK              0xFF00  /* Mask shift mask */
#define aceSH_EXT               0x2000  /* Extended key */
#define aceSH_CAPS              0x1000  /* Caps lock key */
#define aceSH_ALT               0x0800  /* Alternate key */
#define aceSH_CTRL              0x0400  /* Ctrl key */
#define aceSH_CBM               0x0200  /* Commodore key */
#define aceSH_SHIFT             0x0100  /* Shift key */

/* #defines for the options in aceConSetOpt/aceConGetOpt */
#define aceOP_PUTMASK           1       /* Console put mask */
#define aceOP_CHARCOLOR         2       /* Character color */
#define aceOP_CHARATTR          3       /* Character attribute */
#define aceOP_FILLCOLOR         4       /* Fill color */
#define aceOP_FILLATTR          5       /* Fill attribute */
#define aceOP_CRSCOLOR          6       /* Cursor color */
#define aceOP_CRSWRAP           7       /* Force cursor wrap */
#define aceOP_SHSCROLL          8       /* Shift keys for scrolling */
#define aceOP_MOUSCALE          9       /* Mouse scaling */
#define aceOP_RPTDELAY          10      /* Key repeat delay */
#define aceOP_RPTRATE           11      /* Key repeat rate */

/* Console functions */
void __cdecl__ aceConWrite (char* buf, size_t count);
void __cdecl__ aceConPutLit (int c);
void __cdecl__ aceConPos (unsigned x, unsigned y);
void __cdecl__ aceConGetPos (unsigned* x, unsigned* y);
unsigned aceConGetX (void);
unsigned aceConGetY (void);
char __cdecl__* aceConInput (char* buf, unsigned initial);
int aceConStopKey (void);
aceKey aceConGetKey (void);
int __cdecl__ aceConKeyAvail (aceKey* key);
void __cdecl__ aceConKeyMat (char* matrix);
void __cdecl__ aceConSetOpt (unsigned char opt, unsigned char val);
int __cdecl__ aceConGetOpt (unsigned char opt);

/* Misc stuff */
int __cdecl__ aceMiscIoPeek (unsigned addr);
void __cdecl__ aceMiscIoPoke (unsigned addr, unsigned char val);



/* End of ace.h */
#endif



