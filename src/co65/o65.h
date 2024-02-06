/*****************************************************************************/
/*                                                                           */
/*                                   o65.h                                   */
/*                                                                           */
/*               Definitions and code for the o65 file format                */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2002-2003 Ullrich von Bassewitz                                       */
/*               Roemerstrasse 52                                            */
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



/* This files exports structures and constants to handle the o65 relocatable
** file format as defined by Andre Fachat. See the original document under
**
**      http://www.6502.org/users/andre/o65/fileformat.html
**
** for more information.
*/



#ifndef _O65_H
#define _O65_H



/* common */
#include "coll.h"



/*****************************************************************************/
/*                                  Defines                                  */
/*****************************************************************************/



/* Define a structure for the o65 file header */
typedef struct O65Header O65Header;
struct O65Header {
    char            marker[2];  /* Non-C64 marker */
    char            magic[3];   /* o65 magic */
    char            version;    /* Version number */
    unsigned        mode;       /* Mode word */
    unsigned long   tbase;      /* Original text (code) segment address */
    unsigned long   tlen;       /* Size of text (code) segment */
    unsigned long   dbase;      /* Original data segment address */
    unsigned long   dlen;       /* Size of data segment */
    unsigned long   bbase;      /* Original bss segment address */
    unsigned long   blen;       /* Size of bss segment */
    unsigned long   zbase;      /* Original zp segment address */
    unsigned long   zlen;       /* Size of zp segment */
    unsigned long   stack;      /* Stacksize needed */
};

/* o65 option */
typedef struct O65Option O65Option;
struct O65Option {
    unsigned char   Len;        /* Option length */
    unsigned char   Type;       /* Option type */
    unsigned char   Data[1];    /* Option data (dynamically allocated) */
};

/* o65 relocation entry */
typedef struct O65Reloc O65Reloc;
struct O65Reloc {
    unsigned long   Offs;       /* Offset in segment */
    unsigned char   Type;       /* Relocation type */
    unsigned char   SegID;      /* Segment ID */
    unsigned        Val;        /* Any offset value needed for relocation */
    unsigned long   SymIdx;     /* Index into list of imported symbols */
};

/* o65 import */
typedef struct O65Import O65Import;
struct O65Import {
    char            Name[1];    /* Name of the import (dynamically allocated) */
};

/* o65 export */
typedef struct O65Export O65Export;
struct O65Export {
    unsigned char   SegID;      /* Segment ID */
    unsigned long   Val;        /* Relocation value */
    char            Name[1];    /* Name of the export (dynamically allocated) */
};

/* Complete o65 file data */
typedef struct O65Data O65Data;
struct O65Data {
    O65Header       Header;     /* File header */
    Collection      Options;    /* O65 options */
    unsigned char*  Text;       /* Text segment data (unrelocated) */
    unsigned char*  Data;       /* Data segment data (unrelocated) */
    Collection      TextReloc;  /* Relocation entries for the text segment */
    Collection      DataReloc;  /* Relocation entries for the data segment */
    Collection      Imports;    /* Imported symbols */
    Collection      Exports;    /* Exported symbols */
};



/* Marker, magic and version number */
#define O65_MARKER_0            0x01
#define O65_MARKER_1            0x00
#define O65_MAGIC_0             0x6F    /* 'o' */
#define O65_MAGIC_1             0x36    /* '6' */
#define O65_MAGIC_2             0x35    /* '5' */
#define O65_VERSION             0x00

/* Defines for the mode word */
#define O65_CPU_65816           0x8000  /* Executable is for 65816 */
#define O65_CPU_6502            0x0000  /* Executable is for the 6502 */
#define O65_CPU_MASK            0x8000  /* Mask to extract CPU type */

#define O65_RELOC_PAGE          0x4000  /* Page wise relocation */
#define O65_RELOC_BYTE          0x0000  /* Byte wise relocation */
#define O65_RELOC_MASK          0x4000  /* Mask to extract relocation type */

#define O65_SIZE_32BIT          0x2000  /* All size words are 32bit */
#define O65_SIZE_16BIT          0x0000  /* All size words are 16bit */
#define O65_SIZE_MASK           0x2000  /* Mask to extract size */

#define O65_FTYPE_OBJ           0x1000  /* Object file */
#define O65_FTYPE_EXE           0x0000  /* Executable file */
#define O65_FTYPE_MASK          0x1000  /* Mask to extract type */

#define O65_ADDR_SIMPLE         0x0800  /* Simple addressing */
#define O65_ADDR_DEFAULT        0x0000  /* Default addressing */
#define O65_ADDR_MASK           0x0800  /* Mask to extract addressing */

#define O65_ALIGN_1             0x0000  /* Bytewise alignment */
#define O65_ALIGN_2             0x0001  /* Align words */
#define O65_ALIGN_4             0x0002  /* Align longwords */
#define O65_ALIGN_256           0x0003  /* Align pages (256 bytes) */
#define O65_ALIGN_MASK          0x0003  /* Mask to extract alignment */

/* The mode word as generated by the ld65 linker */
#define O65_MODE_CC65           (O65_CPU_6502    |      \
                                 O65_RELOC_BYTE  |      \
                                 O65_SIZE_16BIT  |      \
                                 O65_FTYPE_EXE   |      \
                                 O65_ADDR_SIMPLE |      \
                                 O65_ALIGN_1)

/* The four o65 segment types. */
#define O65_SEGID_UNDEF         0x00
#define O65_SEGID_ABS           0x01
#define O65_SEGID_TEXT          0x02
#define O65_SEGID_DATA          0x03
#define O65_SEGID_BSS           0x04
#define O65_SEGID_ZP            0x05
#define O65_SEGID_MASK          0x07

/* Relocation type codes */
#define O65_RTYPE_WORD          0x80
#define O65_RTYPE_HIGH          0x40
#define O65_RTYPE_LOW           0x20
#define O65_RTYPE_SEGADDR       0xC0
#define O65_RTYPE_SEG           0xA0
#define O65_RTYPE_MASK          0xE0

/* Segment IDs */
#define O65_SEGID_UNDEF         0x00
#define O65_SEGID_ABS           0x01
#define O65_SEGID_TEXT          0x02
#define O65_SEGID_DATA          0x03
#define O65_SEGID_BSS           0x04
#define O65_SEGID_ZP            0x05
#define O65_SEGID_MASK          0x07

/* Option tags */
#define O65_OPT_FILENAME        0
#define O65_OPT_OS              1
#define O65_OPT_ASM             2
#define O65_OPT_AUTHOR          3
#define O65_OPT_TIMESTAMP       4

/* Operating system codes for O65_OPT_OS */
#define O65_OS_OSA65            1
#define O65_OS_LUNIX            2
#define O65_OS_CC65_MODULE      3



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



O65Data* ReadO65File (const char* Name);
/* Read a complete o65 file into dynamically allocated memory and return the
** created O65Data struct.
*/

const char* GetO65OSName (unsigned char OS);
/* Return the name of the operating system given by OS */

const char* GetO65OptionText (const O65Option* O);
/* Return the data of the given option as a readable text. The function returns
** a pointer to a static buffer that is reused on the next call, so if in doubt,
** make a copy (and no, the function is not thread safe).
*/



/* End of o65.h */

#endif
