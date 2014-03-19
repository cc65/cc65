/*****************************************************************************/
/*                                                                           */
/*                                 objdefs.h                                 */
/*                                                                           */
/*                          Object file definitions                          */
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



#ifndef OBJDEFS_H
#define OBJDEFS_H



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Defines for magic and version */
#define OBJ_MAGIC       0x616E7A55
#define OBJ_VERSION     0x0011

/* Size of an object file header */
#define OBJ_HDR_SIZE    (24*4)

/* Flag bits */
#define OBJ_FLAGS_DBGINFO       0x0001  /* File has debug info */
#define OBJ_HAS_DBGINFO(x)      (((x) & OBJ_FLAGS_DBGINFO) != 0)



/* Header structure */
typedef struct ObjHeader ObjHeader;
struct ObjHeader {
    unsigned long       Magic;          /* 32: Magic number */
    unsigned            Version;        /* 16: Version number */
    unsigned            Flags;          /* 16: flags */
    unsigned long       OptionOffs;     /* 32: Offset to option table */
    unsigned long       OptionSize;     /* 32: Size of options */
    unsigned long       FileOffs;       /* 32: Offset to file table */
    unsigned long       FileSize;       /* 32: Size of files */
    unsigned long       SegOffs;        /* 32: Offset to segment table */
    unsigned long       SegSize;        /* 32: Size of segment table */
    unsigned long       ImportOffs;     /* 32: Offset to import list */
    unsigned long       ImportSize;     /* 32: Size of import list */
    unsigned long       ExportOffs;     /* 32: Offset to export list */
    unsigned long       ExportSize;     /* 32: Size of export list */
    unsigned long       DbgSymOffs;     /* 32: Offset to list of debug symbols */
    unsigned long       DbgSymSize;     /* 32: Size of debug symbols */
    unsigned long       LineInfoOffs;   /* 32: Offset to list of line infos */
    unsigned long       LineInfoSize;   /* 32: Size of line infos */
    unsigned long       StrPoolOffs;    /* 32: Offset to string pool */
    unsigned long       StrPoolSize;    /* 32: Size of string pool */
    unsigned long       AssertOffs;     /* 32: Offset to assertion table */
    unsigned long       AssertSize;     /* 32: Size of assertion table */
    unsigned long       ScopeOffs;      /* 32: Offset into scope table */
    unsigned long       ScopeSize;      /* 32: Size of scope table */
    unsigned long       SpanOffs;       /* 32: Offset into span table */
    unsigned long       SpanSize;       /* 32: Size of span table */
};



/* End of objdefs.h */

#endif
