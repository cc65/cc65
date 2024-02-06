/*****************************************************************************/
/*                                                                           */
/*                                 config.c                                  */
/*                                                                           */
/*               Target configuration file for the ld65 linker               */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (c) 1998-2013, Ullrich von Bassewitz                                      */
/*                Roemerstrasse 52                                           */
/*                D-70794 Filderstadt                                        */
/* EMail:         uz@cc65.org                                                */
/*                                                                           */
/* With contributions from:                                                  */
/*                                                                           */
/*      - "David M. Lloyd" <david.lloyd@redhat.com>                          */
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



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

/* common */
#include "addrsize.h"
#include "bitops.h"
#include "check.h"
#include "print.h"
#include "segdefs.h"
#include "target.h"
#include "xmalloc.h"
#include "xsprintf.h"

/* ld65 */
#include "alignment.h"
#include "bin.h"
#include "binfmt.h"
#include "cfgexpr.h"
#include "condes.h"
#include "config.h"
#include "error.h"
#include "exports.h"
#include "expr.h"
#include "global.h"
#include "memarea.h"
#include "o65.h"
#include "objdata.h"
#include "scanner.h"
#include "spool.h"
#include "xex.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Remember which sections we had encountered */
static enum {
    SE_NONE     = 0x0000,
    SE_MEMORY   = 0x0001,
    SE_SEGMENTS = 0x0002,
    SE_FEATURES = 0x0004,
    SE_FILES    = 0x0008,
    SE_FORMATS  = 0x0010,
    SE_SYMBOLS  = 0x0020
} SectionsEncountered = SE_NONE;



/* File list */
static Collection       FileList = STATIC_COLLECTION_INITIALIZER;

/* Memory list */
static Collection       MemoryAreas = STATIC_COLLECTION_INITIALIZER;

/* Memory attributes */
#define MA_START        0x0001
#define MA_SIZE         0x0002
#define MA_TYPE         0x0004
#define MA_FILE         0x0008
#define MA_DEFINE       0x0010
#define MA_FILL         0x0020
#define MA_FILLVAL      0x0040
#define MA_BANK         0x0080

/* Segment list */
static Collection       SegDescList = STATIC_COLLECTION_INITIALIZER;

/* Segment attributes */
#define SA_TYPE         0x0001
#define SA_LOAD         0x0002
#define SA_RUN          0x0004
#define SA_ALIGN        0x0008
#define SA_ALIGN_LOAD   0x0010
#define SA_DEFINE       0x0020
#define SA_OFFSET       0x0040
#define SA_START        0x0080
#define SA_OPTIONAL     0x0100
#define SA_FILLVAL      0x0200

/* Symbol types used in the CfgSymbol structure */
typedef enum {
    CfgSymExport,               /* Not really used in struct CfgSymbol */
    CfgSymImport,               /* Dito */
    CfgSymWeak,                 /* Like export but weak */
    CfgSymO65Export,            /* An o65 export */
    CfgSymO65Import,            /* An o65 import */
} CfgSymType;

/* Symbol structure. It is used for o65 imports and exports, but also for
** symbols from the SYMBOLS sections (symbols defined in the config file or
** forced imports).
*/
typedef struct CfgSymbol CfgSymbol;
struct CfgSymbol {
    CfgSymType  Type;           /* Type of symbol */
    LineInfo*   LI;             /* Config file position */
    unsigned    Name;           /* Symbol name */
    ExprNode*   Value;          /* Symbol value if any */
    unsigned    AddrSize;       /* Address size of symbol */
};

/* Collections with symbols */
static Collection       CfgSymbols = STATIC_COLLECTION_INITIALIZER;

/* Descriptor holding information about the binary formats */
static BinDesc* BinFmtDesc      = 0;
static O65Desc* O65FmtDesc      = 0;
static XexDesc* XexFmtDesc      = 0;



/*****************************************************************************/
/*                                 Forwards                                  */
/*****************************************************************************/



static File* NewFile (unsigned Name);
/* Create a new file descriptor and insert it into the list */



/*****************************************************************************/
/*                              List management                              */
/*****************************************************************************/



static File* FindFile (unsigned Name)
/* Find a file with a given name. */
{
    unsigned I;
    for (I = 0; I < CollCount (&FileList); ++I) {
        File* F = CollAtUnchecked (&FileList, I);
        if (F->Name == Name) {
            return F;
        }
    }
    return 0;
}



static File* GetFile (unsigned Name)
/* Get a file entry with the given name. Create a new one if needed. */
{
    File* F = FindFile (Name);
    if (F == 0) {
        /* Create a new one */
        F = NewFile (Name);
    }
    return F;
}



static void FileInsert (File* F, MemoryArea* M)
/* Insert the memory area into the files list */
{
    M->F = F;
    CollAppend (&F->MemoryAreas, M);
}



static MemoryArea* CfgFindMemory (unsigned Name)
/* Find the memory are with the given name. Return NULL if not found */
{
    unsigned I;
    for (I = 0; I < CollCount (&MemoryAreas); ++I) {
        MemoryArea* M = CollAtUnchecked (&MemoryAreas, I);
        if (M->Name == Name) {
            return M;
        }
    }
    return 0;
}



static MemoryArea* CfgGetMemory (unsigned Name)
/* Find the memory are with the given name. Print an error on an invalid name */
{
    MemoryArea* M = CfgFindMemory (Name);
    if (M == 0) {
        CfgError (&CfgErrorPos, "Invalid memory area '%s'", GetString (Name));
    }
    return M;
}



static SegDesc* CfgFindSegDesc (unsigned Name)
/* Find the segment descriptor with the given name, return NULL if not found. */
{
    unsigned I;
    for (I = 0; I < CollCount (&SegDescList); ++I) {
        SegDesc* S = CollAtUnchecked (&SegDescList, I);
        if (S->Name == Name) {
            /* Found */
            return S;
        }
    }

    /* Not found */
    return 0;
}



static void MemoryInsert (MemoryArea* M, SegDesc* S)
/* Insert the segment descriptor into the memory area list */
{
    /* Insert the segment into the segment list of the memory area */
    CollAppend (&M->SegList, S);
}



/*****************************************************************************/
/*                         Constructors/Destructors                          */
/*****************************************************************************/



static CfgSymbol* NewCfgSymbol (CfgSymType Type, unsigned Name)
/* Create a new CfgSymbol structure with the given type and name. The
** current config file position is recorded in the returned struct. The
** created struct is inserted into the CfgSymbols collection and returned.
*/
{
    /* Allocate memory */
    CfgSymbol* Sym = xmalloc (sizeof (CfgSymbol));

    /* Initialize the fields */
    Sym->Type     = Type;
    Sym->LI       = GenLineInfo (&CfgErrorPos);
    Sym->Name     = Name;
    Sym->Value    = 0;
    Sym->AddrSize = ADDR_SIZE_INVALID;

    /* Insert the symbol into the collection */
    CollAppend (&CfgSymbols, Sym);

    /* Return the initialized struct */
    return Sym;
}



static File* NewFile (unsigned Name)
/* Create a new file descriptor and insert it into the list */
{
    /* Allocate memory */
    File* F = xmalloc (sizeof (File));

    /* Initialize the fields */
    F->Name    = Name;
    F->Flags   = 0;
    F->Format  = BINFMT_DEFAULT;
    F->Size    = 0;
    InitCollection (&F->MemoryAreas);

    /* Insert the struct into the list */
    CollAppend (&FileList, F);

    /* ...and return it */
    return F;
}



static MemoryArea* CreateMemoryArea (const FilePos* Pos, unsigned Name)
/* Create a new memory area and insert it into the list */
{
    /* Check for duplicate names */
    MemoryArea* M = CfgFindMemory (Name);
    if (M) {
        CfgError (&CfgErrorPos,
                  "Memory area '%s' defined twice",
                  GetString (Name));
    }

    /* Create a new memory area */
    M = NewMemoryArea (Pos, Name);

    /* Insert the struct into the list ... */
    CollAppend (&MemoryAreas, M);

    /* ...and return it */
    return M;
}



static SegDesc* NewSegDesc (unsigned Name)
/* Create a segment descriptor and insert it into the list */
{

    /* Check for duplicate names */
    SegDesc* S = CfgFindSegDesc (Name);
    if (S) {
        CfgError (&CfgErrorPos, "Segment '%s' defined twice", GetString (Name));
    }

    /* Allocate memory */
    S = xmalloc (sizeof (SegDesc));

    /* Initialize the fields */
    S->Name          = Name;
    S->LI            = GenLineInfo (&CfgErrorPos);
    S->Seg           = 0;
    S->Attr          = 0;
    S->Flags         = 0;
    S->FillVal       = 0;
    S->RunAlignment  = 1;
    S->LoadAlignment = 1;

    /* Insert the struct into the list ... */
    CollAppend (&SegDescList, S);

    /* ...and return it */
    return S;
}



static void FreeSegDesc (SegDesc* S)
/* Free a segment descriptor */
{
    FreeLineInfo (S->LI);
    xfree (S);
}



/*****************************************************************************/
/*                            Config file parsing                            */
/*****************************************************************************/



static void FlagAttr (unsigned* Flags, unsigned Mask, const char* Name)
/* Check if the item is already defined. Print an error if so. If not, set
** the marker that we have a definition now.
*/
{
    if (*Flags & Mask) {
        CfgError (&CfgErrorPos, "%s is already defined", Name);
    }
    *Flags |= Mask;
}



static void AttrCheck (unsigned Attr, unsigned Mask, const char* Name)
/* Check that a mandatory attribute was given */
{
    if ((Attr & Mask) == 0) {
        CfgError (&CfgErrorPos, "%s attribute is missing", Name);
    }
}



static void ParseMemory (void)
/* Parse a MEMORY section */
{
    static const IdentTok Attributes [] = {
        {   "BANK",     CFGTOK_BANK     },
        {   "DEFINE",   CFGTOK_DEFINE   },
        {   "FILE",     CFGTOK_FILE     },
        {   "FILL",     CFGTOK_FILL     },
        {   "FILLVAL",  CFGTOK_FILLVAL  },
        {   "SIZE",     CFGTOK_SIZE     },
        {   "START",    CFGTOK_START    },
        {   "TYPE",     CFGTOK_TYPE     },
    };
    static const IdentTok Types [] = {
        {   "RO",       CFGTOK_RO       },
        {   "RW",       CFGTOK_RW       },
    };

    while (CfgTok == CFGTOK_IDENT) {

        /* Create a new entry on the heap */
        MemoryArea* M = CreateMemoryArea (&CfgErrorPos, GetStrBufId (&CfgSVal));

        /* Skip the name and the following colon */
        CfgNextTok ();
        CfgConsumeColon ();

        /* Read the attributes */
        while (CfgTok == CFGTOK_IDENT) {

            /* Map the identifier to a token */
            cfgtok_t AttrTok;
            CfgSpecialToken (Attributes, ENTRY_COUNT (Attributes), "Attribute");
            AttrTok = CfgTok;

            /* An optional assignment follows */
            CfgNextTok ();
            CfgOptionalAssign ();

            /* Check which attribute was given */
            switch (AttrTok) {

                case CFGTOK_BANK:
                    FlagAttr (&M->Attr, MA_BANK, "BANK");
                    M->BankExpr = CfgExpr ();
                    break;

                case CFGTOK_DEFINE:
                    FlagAttr (&M->Attr, MA_DEFINE, "DEFINE");
                    /* Map the token to a boolean */
                    CfgBoolToken ();
                    if (CfgTok == CFGTOK_TRUE) {
                        M->Flags |= MF_DEFINE;
                    }
                    CfgNextTok ();
                    break;

                case CFGTOK_FILE:
                    FlagAttr (&M->Attr, MA_FILE, "FILE");
                    CfgAssureStr ();
                    /* Get the file entry and insert the memory area */
                    FileInsert (GetFile (GetStrBufId (&CfgSVal)), M);
                    CfgNextTok ();
                    break;

                case CFGTOK_FILL:
                    FlagAttr (&M->Attr, MA_FILL, "FILL");
                    /* Map the token to a boolean */
                    CfgBoolToken ();
                    if (CfgTok == CFGTOK_TRUE) {
                        M->Flags |= MF_FILL;
                    }
                    CfgNextTok ();
                    break;

                case CFGTOK_FILLVAL:
                    FlagAttr (&M->Attr, MA_FILLVAL, "FILLVAL");
                    M->FillVal = (unsigned char) CfgCheckedConstExpr (0, 0xFF);
                    break;

                case CFGTOK_SIZE:
                    FlagAttr (&M->Attr, MA_SIZE, "SIZE");
                    M->SizeExpr = CfgExpr ();
                    break;

                case CFGTOK_START:
                    FlagAttr (&M->Attr, MA_START, "START");
                    M->StartExpr = CfgExpr ();
                    break;

                case CFGTOK_TYPE:
                    FlagAttr (&M->Attr, MA_TYPE, "TYPE");
                    CfgSpecialToken (Types, ENTRY_COUNT (Types), "TYPE");
                    if (CfgTok == CFGTOK_RO) {
                        M->Flags |= MF_RO;
                    }
                    CfgNextTok ();
                    break;

                default:
                    FAIL ("Unexpected attribute token");

            }

            /* Skip an optional comma */
            CfgOptionalComma ();
        }

        /* Skip the semicolon */
        CfgConsumeSemi ();

        /* Check for mandatory parameters */
        AttrCheck (M->Attr, MA_START, "START");
        AttrCheck (M->Attr, MA_SIZE, "SIZE");

        /* If we don't have a file name for output given, use the default
        ** file name.
        */
        if ((M->Attr & MA_FILE) == 0) {
            FileInsert (GetFile (GetStringId (OutputName)), M);
            OutputNameUsed = 1;
        }
    }

    /* Remember we had this section */
    SectionsEncountered |= SE_MEMORY;
}



static void ParseFiles (void)
/* Parse a FILES section */
{
    static const IdentTok Attributes [] = {
        {   "FORMAT",   CFGTOK_FORMAT   },
    };
    static const IdentTok Formats [] = {
        {   "ATARI",    CFGTOK_ATARIEXE },
        {   "O65",      CFGTOK_O65      },
        {   "BIN",      CFGTOK_BIN      },
        {   "BINARY",   CFGTOK_BIN      },
    };


    /* The MEMORY section must preceed the FILES section */
    if ((SectionsEncountered & SE_MEMORY) == 0) {
        CfgError (&CfgErrorPos, "MEMORY must precede FILES");
    }

    /* Parse all files */
    while (CfgTok != CFGTOK_RCURLY) {

        File* F;

        /* We expect a string value here */
        CfgAssureStr ();

        /* Search for the file, it must exist */
        F = FindFile (GetStrBufId (&CfgSVal));
        if (F == 0) {
            CfgError (&CfgErrorPos,
                      "File '%s' not found in MEMORY section",
                      SB_GetConstBuf (&CfgSVal));
        }

        /* Skip the token and the following colon */
        CfgNextTok ();
        CfgConsumeColon ();

        /* Read the attributes */
        while (CfgTok == CFGTOK_IDENT) {

            /* Map the identifier to a token */
            cfgtok_t AttrTok;
            CfgSpecialToken (Attributes, ENTRY_COUNT (Attributes), "Attribute");
            AttrTok = CfgTok;

            /* An optional assignment follows */
            CfgNextTok ();
            CfgOptionalAssign ();

            /* Check which attribute was given */
            switch (AttrTok) {

                case CFGTOK_FORMAT:
                    if (F->Format != BINFMT_DEFAULT) {
                        /* We've set the format already! */
                        CfgError (&CfgErrorPos,
                                  "Cannot set a file format twice");
                    }
                    /* Read the format token */
                    CfgSpecialToken (Formats, ENTRY_COUNT (Formats), "Format");
                    switch (CfgTok) {

                        case CFGTOK_BIN:
                            F->Format = BINFMT_BINARY;
                            break;

                        case CFGTOK_O65:
                            F->Format = BINFMT_O65;
                            break;

                        case CFGTOK_ATARIEXE:
                            F->Format = BINFMT_ATARIEXE;
                            break;

                        default:
                            Error ("Unexpected format token");
                    }
                    break;

                default:
                    FAIL ("Unexpected attribute token");

            }

            /* Skip the attribute value and an optional comma */
            CfgNextTok ();
            CfgOptionalComma ();
        }

        /* Skip the semicolon */
        CfgConsumeSemi ();

    }

    /* Remember we had this section */
    SectionsEncountered |= SE_FILES;
}



static void ParseSegments (void)
/* Parse a SEGMENTS section */
{
    static const IdentTok Attributes [] = {
        {   "ALIGN",            CFGTOK_ALIGN            },
        {   "ALIGN_LOAD",       CFGTOK_ALIGN_LOAD       },
        {   "DEFINE",           CFGTOK_DEFINE           },
        {   "FILLVAL",          CFGTOK_FILLVAL          },
        {   "LOAD",             CFGTOK_LOAD             },
        {   "OFFSET",           CFGTOK_OFFSET           },
        {   "OPTIONAL",         CFGTOK_OPTIONAL         },
        {   "RUN",              CFGTOK_RUN              },
        {   "START",            CFGTOK_START            },
        {   "TYPE",             CFGTOK_TYPE             },
    };
    static const IdentTok Types [] = {
        {   "RO",               CFGTOK_RO               },
        {   "RW",               CFGTOK_RW               },
        {   "BSS",              CFGTOK_BSS              },
        {   "ZP",               CFGTOK_ZP               },
        {   "OVERWRITE",        CFGTOK_OVERWRITE        },
    };

    unsigned Count;

    /* The MEMORY section must preceed the SEGMENTS section */
    if ((SectionsEncountered & SE_MEMORY) == 0) {
        CfgError (&CfgErrorPos, "MEMORY must precede SEGMENTS");
    }

    while (CfgTok == CFGTOK_IDENT) {

        SegDesc* S;

        /* Create a new entry on the heap */
        S = NewSegDesc (GetStrBufId (&CfgSVal));

        /* Skip the name and the following colon */
        CfgNextTok ();
        CfgConsumeColon ();

        /* Read the attributes */
        while (CfgTok == CFGTOK_IDENT) {

            /* Map the identifier to a token */
            cfgtok_t AttrTok;
            CfgSpecialToken (Attributes, ENTRY_COUNT (Attributes), "Attribute");
            AttrTok = CfgTok;

            /* An optional assignment follows */
            CfgNextTok ();
            CfgOptionalAssign ();

            /* Check which attribute was given */
            switch (AttrTok) {

                case CFGTOK_ALIGN:
                    FlagAttr (&S->Attr, SA_ALIGN, "ALIGN");
                    S->RunAlignment = (unsigned) CfgCheckedConstExpr (1, MAX_ALIGNMENT);
                    S->Flags |= SF_ALIGN;
                    break;

                case CFGTOK_ALIGN_LOAD:
                    FlagAttr (&S->Attr, SA_ALIGN_LOAD, "ALIGN_LOAD");
                    S->LoadAlignment = (unsigned) CfgCheckedConstExpr (1, MAX_ALIGNMENT);
                    S->Flags |= SF_ALIGN_LOAD;
                    break;

                case CFGTOK_DEFINE:
                    FlagAttr (&S->Attr, SA_DEFINE, "DEFINE");
                    /* Map the token to a boolean */
                    CfgBoolToken ();
                    if (CfgTok == CFGTOK_TRUE) {
                        S->Flags |= SF_DEFINE;
                    }
                    CfgNextTok ();
                    break;

                case CFGTOK_FILLVAL:
                    FlagAttr (&S->Attr, SA_FILLVAL, "FILLVAL");
                    S->FillVal = (unsigned char) CfgCheckedConstExpr (0, 0xFF);
                    S->Flags |= SF_FILLVAL;
                    break;

                case CFGTOK_LOAD:
                    FlagAttr (&S->Attr, SA_LOAD, "LOAD");
                    S->Load = CfgGetMemory (GetStrBufId (&CfgSVal));
                    CfgNextTok ();
                    break;

                case CFGTOK_OFFSET:
                    FlagAttr (&S->Attr, SA_OFFSET, "OFFSET");
                    S->Addr   = CfgCheckedConstExpr (0, 0x1000000);
                    S->Flags |= SF_OFFSET;
                    break;

                case CFGTOK_OPTIONAL:
                    FlagAttr (&S->Attr, SA_OPTIONAL, "OPTIONAL");
                    CfgBoolToken ();
                    if (CfgTok == CFGTOK_TRUE) {
                        S->Flags |= SF_OPTIONAL;
                    }
                    CfgNextTok ();
                    break;

                case CFGTOK_RUN:
                    FlagAttr (&S->Attr, SA_RUN, "RUN");
                    S->Run = CfgGetMemory (GetStrBufId (&CfgSVal));
                    CfgNextTok ();
                    break;

                case CFGTOK_START:
                    FlagAttr (&S->Attr, SA_START, "START");
                    S->Addr   = CfgCheckedConstExpr (0, 0x1000000);
                    S->Flags |= SF_START;
                    break;

                case CFGTOK_TYPE:
                    FlagAttr (&S->Attr, SA_TYPE, "TYPE");
                    CfgSpecialToken (Types, ENTRY_COUNT (Types), "Type");
                    switch (CfgTok) {
                        case CFGTOK_RO:        S->Flags |= SF_RO;                  break;
                        case CFGTOK_RW:        /* Default */                       break;
                        case CFGTOK_BSS:       S->Flags |= SF_BSS;                 break;
                        case CFGTOK_ZP:        S->Flags |= (SF_BSS | SF_ZP);       break;
                        case CFGTOK_OVERWRITE: S->Flags |= (SF_OVERWRITE | SF_RO); break;
                        default:               Internal ("Unexpected token: %d", CfgTok);
                    }
                    CfgNextTok ();
                    break;

                default:
                    FAIL ("Unexpected attribute token");

            }

            /* Skip an optional comma */
            CfgOptionalComma ();
        }

        /* Check for mandatory parameters */
        AttrCheck (S->Attr, SA_LOAD, "LOAD");

        /* Set defaults for stuff not given */
        if ((S->Attr & SA_RUN) == 0) {
            S->Attr |= SA_RUN;
            S->Run = S->Load;
        }

        /* An attribute of ALIGN_LOAD doesn't make sense if there are no
        ** separate run and load memory areas.
        */
        if ((S->Flags & SF_ALIGN_LOAD) != 0 && (S->Load == S->Run)) {
            CfgWarning (&CfgErrorPos,
                        "ALIGN_LOAD attribute specified, but no separate "
                        "LOAD and RUN memory areas assigned");
            /* Remove the flag */
            S->Flags &= ~SF_ALIGN_LOAD;
        }

        /* If the segment is marked as BSS style, it may not have separate
        ** load and run memory areas, because it's is never written to disk.
        */
        if ((S->Flags & SF_BSS) != 0 && (S->Load != S->Run)) {
            CfgWarning (&CfgErrorPos,
                        "Segment with type 'bss' has both LOAD and RUN "
                        "memory areas assigned");
        }

        /* Don't allow read/write data to be put into a readonly area */
        if ((S->Flags & SF_RO) == 0) {
            if (S->Run->Flags & MF_RO) {
                CfgError (&CfgErrorPos,
                          "Cannot put r/w segment '%s' in r/o memory area '%s'",
                          GetString (S->Name), GetString (S->Run->Name));
            }
        }

        /* Only one of ALIGN, START and OFFSET may be used */
        Count = ((S->Flags & SF_ALIGN)  != 0) +
                ((S->Flags & SF_OFFSET) != 0) +
                ((S->Flags & SF_START)  != 0);
        if (Count > 1) {
            CfgError (&CfgErrorPos,
                      "Only one of ALIGN, START, OFFSET may be used");
        }

        /* Skip the semicolon */
        CfgConsumeSemi ();
    }

    /* Remember we had this section */
    SectionsEncountered |= SE_SEGMENTS;
}



static void ParseO65 (void)
/* Parse the o65 format section */
{
    static const IdentTok Attributes [] = {
        {   "EXPORT",   CFGTOK_EXPORT           },
        {   "IMPORT",   CFGTOK_IMPORT           },
        {   "TYPE",     CFGTOK_TYPE             },
        {   "OS",       CFGTOK_OS               },
        {   "ID",       CFGTOK_ID               },
        {   "VERSION",  CFGTOK_VERSION          },
    };
    static const IdentTok Types [] = {
        {   "SMALL",    CFGTOK_SMALL            },
        {   "LARGE",    CFGTOK_LARGE            },
    };
    static const IdentTok OperatingSystems [] = {
        {   "LUNIX",    CFGTOK_LUNIX            },
        {   "OSA65",    CFGTOK_OSA65            },
        {   "CC65",     CFGTOK_CC65             },
        {   "OPENCBM",  CFGTOK_OPENCBM          },
    };

    /* Bitmask to remember the attributes we got already */
    enum {
        atNone          = 0x0000,
        atOS            = 0x0001,
        atOSVersion     = 0x0002,
        atType          = 0x0004,
        atImport        = 0x0008,
        atExport        = 0x0010,
        atID            = 0x0020,
        atVersion       = 0x0040
    };
    unsigned AttrFlags = atNone;

    /* Remember the attributes read */
    unsigned OS = 0;            /* Initialize to keep gcc happy */
    unsigned Version = 0;

    /* Read the attributes */
    while (CfgTok == CFGTOK_IDENT) {

        /* Map the identifier to a token */
        cfgtok_t AttrTok;
        CfgSpecialToken (Attributes, ENTRY_COUNT (Attributes), "Attribute");
        AttrTok = CfgTok;

        /* An optional assignment follows */
        CfgNextTok ();
        CfgOptionalAssign ();

        /* Check which attribute was given */
        switch (AttrTok) {

            case CFGTOK_EXPORT:
                /* Remember we had this token (maybe more than once) */
                AttrFlags |= atExport;
                /* We expect an identifier */
                CfgAssureIdent ();
                /* Remember it as an export for later */
                NewCfgSymbol (CfgSymO65Export, GetStrBufId (&CfgSVal));
                /* Eat the identifier token */
                CfgNextTok ();
                break;

            case CFGTOK_IMPORT:
                /* Remember we had this token (maybe more than once) */
                AttrFlags |= atImport;
                /* We expect an identifier */
                CfgAssureIdent ();
                /* Remember it as an import for later */
                NewCfgSymbol (CfgSymO65Import, GetStrBufId (&CfgSVal));
                /* Eat the identifier token */
                CfgNextTok ();
                break;

            case CFGTOK_TYPE:
                /* Cannot have this attribute twice */
                FlagAttr (&AttrFlags, atType, "TYPE");
                /* Get the type of the executable */
                CfgSpecialToken (Types, ENTRY_COUNT (Types), "Type");
                switch (CfgTok) {

                    case CFGTOK_SMALL:
                        O65SetSmallModel (O65FmtDesc);
                        break;

                    case CFGTOK_LARGE:
                        O65SetLargeModel (O65FmtDesc);
                        break;

                    default:
                        CfgError (&CfgErrorPos, "Unexpected type token");
                }
                /* Eat the attribute token */
                CfgNextTok ();
                break;

            case CFGTOK_OS:
                /* Cannot use this attribute twice */
                FlagAttr (&AttrFlags, atOS, "OS");
                /* Get the operating system. It may be specified as name or
                ** as a number in the range 1..255.
                */
                if (CfgTok == CFGTOK_INTCON) {
                    CfgRangeCheck (O65OS_MIN, O65OS_MAX);
                    OS = (unsigned) CfgIVal;
                } else {
                    CfgSpecialToken (OperatingSystems, ENTRY_COUNT (OperatingSystems), "OS type");
                    switch (CfgTok) {
                        case CFGTOK_LUNIX:    OS = O65OS_LUNIX;     break;
                        case CFGTOK_OSA65:    OS = O65OS_OSA65;     break;
                        case CFGTOK_CC65:     OS = O65OS_CC65;      break;
                        case CFGTOK_OPENCBM:  OS = O65OS_OPENCBM;   break;
                        default:              CfgError (&CfgErrorPos, "Unexpected OS token");
                    }
                }
                CfgNextTok ();
                break;

            case CFGTOK_ID:
                /* Cannot have this attribute twice */
                FlagAttr (&AttrFlags, atID, "ID");
                /* We're expecting a number in the 0..$FFFF range*/
                ModuleId = (unsigned) CfgCheckedConstExpr (0, 0xFFFF);
                break;

            case CFGTOK_VERSION:
                /* Cannot have this attribute twice */
                FlagAttr (&AttrFlags, atVersion, "VERSION");
                /* We're expecting a number in byte range */
                Version = (unsigned) CfgCheckedConstExpr (0, 0xFF);
                break;

            default:
                FAIL ("Unexpected attribute token");

        }

        /* Skip an optional comma */
        CfgOptionalComma ();
    }

    /* Check if we have all mandatory attributes */
    AttrCheck (AttrFlags, atOS, "OS");

    /* Check for attributes that may not be combined */
    if (OS == O65OS_CC65) {
        if ((AttrFlags & (atImport | atExport)) != 0 && ModuleId < 0x8000) {
            CfgError (&CfgErrorPos,
                      "OS type CC65 may not have imports or exports for ids < $8000");
        }
    } else {
        if (AttrFlags & atID) {
            CfgError (&CfgErrorPos,
                      "Operating system does not support the ID attribute");
        }
    }

    /* Set the O65 operating system to use */
    O65SetOS (O65FmtDesc, OS, Version, ModuleId);
}



static void ParseXex (void)
/* Parse the o65 format section */
{
    static const IdentTok Attributes [] = {
        {   "RUNAD",    CFGTOK_RUNAD            },
        {   "INITAD",   CFGTOK_INITAD           },
    };

    /* Remember the attributes read */
    /* Bitmask to remember the attributes we got already */
    enum {
        atNone          = 0x0000,
        atRunAd         = 0x0001,
    };
    unsigned AttrFlags = atNone;
    Import *RunAd = 0;
    Import *InitAd;
    MemoryArea *InitMem;

    /* Read the attributes */
    while (CfgTok == CFGTOK_IDENT) {

        /* Map the identifier to a token */
        cfgtok_t AttrTok;
        CfgSpecialToken (Attributes, ENTRY_COUNT (Attributes), "Attribute");
        AttrTok = CfgTok;

        /* An optional assignment follows */
        CfgNextTok ();
        CfgOptionalAssign ();

        /* Check which attribute was given */
        switch (AttrTok) {

            case CFGTOK_RUNAD:
                /* Cannot have this attribute twice */
                FlagAttr (&AttrFlags, atRunAd, "RUNAD");
                /* We expect an identifier */
                CfgAssureIdent ();
                /* Generate an import for the symbol */
                RunAd = InsertImport (GenImport (GetStrBufId (&CfgSVal), ADDR_SIZE_ABS));
                /* Remember the file position */
                CollAppend (&RunAd->RefLines, GenLineInfo (&CfgErrorPos));
                /* Eat the identifier token */
                CfgNextTok ();
                break;

            case CFGTOK_INITAD:
                /* We expect a memory area followed by a colon and an identifier */
                CfgAssureIdent ();
                InitMem = CfgGetMemory (GetStrBufId (&CfgSVal));
                CfgNextTok ();
                CfgConsumeColon ();
                CfgAssureIdent ();
                /* Generate an import for the symbol */
                InitAd = InsertImport (GenImport (GetStrBufId (&CfgSVal), ADDR_SIZE_ABS));
                /* Remember the file position */
                CollAppend (&InitAd->RefLines, GenLineInfo (&CfgErrorPos));
                /* Eat the identifier token */
                CfgNextTok ();
                /* Add to XEX */
                if (XexAddInitAd (XexFmtDesc, InitMem, InitAd))
                    CfgError (&CfgErrorPos, "INITAD already given for memory area");
                break;

            default:
                FAIL ("Unexpected attribute token");

        }

        /* Skip an optional comma */
        CfgOptionalComma ();
    }

    /* Set the RUNAD import if we have one */
    if ( RunAd )
        XexSetRunAd (XexFmtDesc, RunAd);
}



static void ParseFormats (void)
/* Parse a target format section */
{
    static const IdentTok Formats [] = {
        {   "O65",      CFGTOK_O65      },
        {   "BIN",      CFGTOK_BIN      },
        {   "BINARY",   CFGTOK_BIN      },
        {   "ATARI",    CFGTOK_ATARIEXE },
    };

    while (CfgTok == CFGTOK_IDENT) {

        /* Map the identifier to a token */
        cfgtok_t FormatTok;
        CfgSpecialToken (Formats, ENTRY_COUNT (Formats), "Format");
        FormatTok = CfgTok;

        /* Skip the name and the following colon */
        CfgNextTok ();
        CfgConsumeColon ();

        /* Parse the format options */
        switch (FormatTok) {

            case CFGTOK_O65:
                ParseO65 ();
                break;

            case CFGTOK_ATARIEXE:
                ParseXex ();
                break;

            case CFGTOK_BIN:
                /* No attribibutes available */
                break;

            default:
                Error ("Unexpected format token");
        }

        /* Skip the semicolon */
        CfgConsumeSemi ();
    }


    /* Remember we had this section */
    SectionsEncountered |= SE_FORMATS;
}



static void ParseConDes (void)
/* Parse the CONDES feature */
{
    static const IdentTok Attributes [] = {
        {   "COUNT",            CFGTOK_COUNT            },
        {   "IMPORT",           CFGTOK_IMPORT           },
        {   "LABEL",            CFGTOK_LABEL            },
        {   "ORDER",            CFGTOK_ORDER            },
        {   "SEGMENT",          CFGTOK_SEGMENT          },
        {   "TYPE",             CFGTOK_TYPE             },
    };

    static const IdentTok Types [] = {
        {   "CONSTRUCTOR",      CFGTOK_CONSTRUCTOR      },
        {   "DESTRUCTOR",       CFGTOK_DESTRUCTOR       },
        {   "INTERRUPTOR",      CFGTOK_INTERRUPTOR      },
    };

    static const IdentTok Orders [] = {
        {   "DECREASING",       CFGTOK_DECREASING       },
        {   "INCREASING",       CFGTOK_INCREASING       },
    };

    /* Attribute values. */
    unsigned Count   = INVALID_STRING_ID;
    unsigned Label   = INVALID_STRING_ID;
    unsigned SegName = INVALID_STRING_ID;
    ConDesImport Import;
    /* Initialize to avoid gcc warnings: */
    int Type = -1;
    ConDesOrder Order = cdIncreasing;

    /* Bitmask to remember the attributes we got already */
    enum {
        atNone          = 0x0000,
        atCount         = 0x0001,
        atImport        = 0x0002,
        atLabel         = 0x0004,
        atOrder         = 0x0008,
        atSegName       = 0x0010,
        atType          = 0x0020,
    };
    unsigned AttrFlags = atNone;

    /* Parse the attributes */
    while (1) {

        /* Map the identifier to a token */
        cfgtok_t AttrTok;
        CfgSpecialToken (Attributes, ENTRY_COUNT (Attributes), "Attribute");
        AttrTok = CfgTok;

        /* An optional assignment follows */
        CfgNextTok ();
        CfgOptionalAssign ();

        /* Check which attribute was given */
        switch (AttrTok) {

            case CFGTOK_COUNT:
                /* Don't allow this twice */
                FlagAttr (&AttrFlags, atCount, "COUNT");
                /* We expect an identifier */
                CfgAssureIdent ();
                /* Remember the value for later */
                Count = GetStrBufId (&CfgSVal);
                break;

            case CFGTOK_IMPORT:
                /* Don't allow this twice */
                FlagAttr (&AttrFlags, atImport, "IMPORT");
                /* We expect an identifier */
                CfgAssureIdent ();
                /* Remember value and position for later */
                Import.Name = GetStrBufId (&CfgSVal);
                Import.Pos = CfgErrorPos;
                Import.AddrSize = ADDR_SIZE_ABS;
                break;

            case CFGTOK_LABEL:
                /* Don't allow this twice */
                FlagAttr (&AttrFlags, atLabel, "LABEL");
                /* We expect an identifier */
                CfgAssureIdent ();
                /* Remember the value for later */
                Label = GetStrBufId (&CfgSVal);
                break;

            case CFGTOK_ORDER:
                /* Don't allow this twice */
                FlagAttr (&AttrFlags, atOrder, "ORDER");
                CfgSpecialToken (Orders, ENTRY_COUNT (Orders), "Order");
                switch (CfgTok) {
                    case CFGTOK_DECREASING: Order = cdDecreasing;       break;
                    case CFGTOK_INCREASING: Order = cdIncreasing;       break;
                    default: FAIL ("Unexpected order token");
                }
                break;

            case CFGTOK_SEGMENT:
                /* Don't allow this twice */
                FlagAttr (&AttrFlags, atSegName, "SEGMENT");
                /* We expect an identifier */
                CfgAssureIdent ();
                /* Remember the value for later */
                SegName = GetStrBufId (&CfgSVal);
                break;

            case CFGTOK_TYPE:
                /* Don't allow this twice */
                FlagAttr (&AttrFlags, atType, "TYPE");
                /* The type may be given as id or numerical */
                if (CfgTok == CFGTOK_INTCON) {
                    CfgRangeCheck (CD_TYPE_MIN, CD_TYPE_MAX);
                    Type = (int) CfgIVal;
                } else {
                    CfgSpecialToken (Types, ENTRY_COUNT (Types), "Type");
                    switch (CfgTok) {
                        case CFGTOK_CONSTRUCTOR: Type = CD_TYPE_CON;    break;
                        case CFGTOK_DESTRUCTOR:  Type = CD_TYPE_DES;    break;
                        case CFGTOK_INTERRUPTOR: Type = CD_TYPE_INT;    break;
                        default: FAIL ("Unexpected type token");
                    }
                }
                break;

            default:
                FAIL ("Unexpected attribute token");

        }

        /* Skip the attribute value */
        CfgNextTok ();

        /* Semicolon ends the ConDes decl, otherwise accept an optional comma */
        if (CfgTok == CFGTOK_SEMI) {
            break;
        } else if (CfgTok == CFGTOK_COMMA) {
            CfgNextTok ();
        }
    }

    /* Check if we have all mandatory attributes */
    AttrCheck (AttrFlags, atSegName, "SEGMENT");
    AttrCheck (AttrFlags, atLabel, "LABEL");
    AttrCheck (AttrFlags, atType, "TYPE");

    /* Check if the condes has already attributes defined */
    if (ConDesHasSegName(Type) || ConDesHasLabel(Type)) {
        CfgError (&CfgErrorPos,
                  "CONDES attributes for type %d are already defined",
                  Type);
    }

    /* Define the attributes */
    ConDesSetSegName (Type, SegName);
    ConDesSetLabel (Type, Label);
    if (AttrFlags & atCount) {
        ConDesSetCountSym (Type, Count);
    }
    if (AttrFlags & atImport) {
        ConDesSetImport (Type, &Import);
    }
    if (AttrFlags & atOrder) {
        ConDesSetOrder (Type, Order);
    }
}



static void ParseStartAddress (void)
/* Parse the STARTADDRESS feature */
{
    static const IdentTok Attributes [] = {
        {   "DEFAULT",  CFGTOK_DEFAULT },
    };


    /* Attribute values. */
    unsigned long DefStartAddr = 0;

    /* Bitmask to remember the attributes we got already */
    enum {
        atNone          = 0x0000,
        atDefault       = 0x0001
    };
    unsigned AttrFlags = atNone;

    /* Parse the attributes */
    while (1) {

        /* Map the identifier to a token */
        cfgtok_t AttrTok;
        CfgSpecialToken (Attributes, ENTRY_COUNT (Attributes), "Attribute");
        AttrTok = CfgTok;

        /* An optional assignment follows */
        CfgNextTok ();
        CfgOptionalAssign ();

        /* Check which attribute was given */
        switch (AttrTok) {

            case CFGTOK_DEFAULT:
                /* Don't allow this twice */
                FlagAttr (&AttrFlags, atDefault, "DEFAULT");
                /* We expect a numeric expression */
                DefStartAddr = CfgCheckedConstExpr (0, 0xFFFFFF);
                break;

            default:
                FAIL ("Unexpected attribute token");

        }

        /* Semicolon ends the ConDes decl, otherwise accept an optional comma */
        if (CfgTok == CFGTOK_SEMI) {
            break;
        } else if (CfgTok == CFGTOK_COMMA) {
            CfgNextTok ();
        }
    }

    /* Check if we have all mandatory attributes */
    AttrCheck (AttrFlags, atDefault, "DEFAULT");

    /* If no start address was given on the command line, use the one given
    ** here
    */
    if (!HaveStartAddr) {
        StartAddr = DefStartAddr;
    }
}



static void ParseFeatures (void)
/* Parse a features section */
{
    static const IdentTok Features [] = {
        {   "CONDES",       CFGTOK_CONDES       },
        {   "STARTADDRESS", CFGTOK_STARTADDRESS },
    };

    while (CfgTok == CFGTOK_IDENT) {

        /* Map the identifier to a token */
        cfgtok_t FeatureTok;
        CfgSpecialToken (Features, ENTRY_COUNT (Features), "Feature");
        FeatureTok = CfgTok;

        /* Skip the name and the following colon */
        CfgNextTok ();
        CfgConsumeColon ();

        /* Parse the format options */
        switch (FeatureTok) {

            case CFGTOK_CONDES:
                ParseConDes ();
                break;

            case CFGTOK_STARTADDRESS:
                ParseStartAddress ();
                break;


            default:
                FAIL ("Unexpected feature token");
        }

        /* Skip the semicolon */
        CfgConsumeSemi ();
    }

    /* Remember we had this section */
    SectionsEncountered |= SE_FEATURES;
}



static void ParseSymbols (void)
/* Parse a symbols section */
{
    static const IdentTok Attributes[] = {
        {   "ADDRSIZE", CFGTOK_ADDRSIZE },
        {   "TYPE",     CFGTOK_TYPE     },
        {   "VALUE",    CFGTOK_VALUE    },
    };

    static const IdentTok AddrSizes [] = {
        {   "ABS",      CFGTOK_ABS      },
        {   "ABSOLUTE", CFGTOK_ABS      },
        {   "DIRECT",   CFGTOK_ZP       },
        {   "DWORD",    CFGTOK_LONG     },
        {   "FAR",      CFGTOK_FAR      },
        {   "LONG",     CFGTOK_LONG     },
        {   "NEAR",     CFGTOK_ABS      },
        {   "ZEROPAGE", CFGTOK_ZP       },
        {   "ZP",       CFGTOK_ZP       },
    };

    static const IdentTok Types [] = {
        {   "EXPORT",   CFGTOK_EXPORT   },
        {   "IMPORT",   CFGTOK_IMPORT   },
        {   "WEAK",     CFGTOK_WEAK     },
    };

    while (CfgTok == CFGTOK_IDENT) {

        /* Bitmask to remember the attributes we got already */
        enum {
            atNone      = 0x0000,
            atAddrSize  = 0x0001,
            atType      = 0x0002,
            atValue     = 0x0004,
        };
        unsigned AttrFlags = atNone;

        ExprNode* Value = 0;
        CfgSymType Type = CfgSymExport;
        unsigned char AddrSize = ADDR_SIZE_ABS;
        Import* Imp;
        Export* Exp;
        CfgSymbol* Sym;

        /* Remember the name */
        unsigned Name = GetStrBufId (&CfgSVal);
        CfgNextTok ();

        /* New syntax - skip the colon */
        CfgNextTok ();

        /* Parse the attributes */
        while (1) {

            /* Map the identifier to a token */
            cfgtok_t AttrTok;
            CfgSpecialToken (Attributes, ENTRY_COUNT (Attributes), "Attribute");
            AttrTok = CfgTok;

            /* Skip the attribute name */
            CfgNextTok ();

            /* An optional assignment follows */
            CfgOptionalAssign ();

            /* Check which attribute was given */
            switch (AttrTok) {

                case CFGTOK_ADDRSIZE:
                    /* Don't allow this twice */
                    FlagAttr (&AttrFlags, atAddrSize, "ADDRSIZE");
                    /* Map the type to a token */
                    CfgSpecialToken (AddrSizes, ENTRY_COUNT (AddrSizes), "AddrSize");
                    switch (CfgTok) {
                        case CFGTOK_ABS:    AddrSize = ADDR_SIZE_ABS;   break;
                        case CFGTOK_FAR:    AddrSize = ADDR_SIZE_FAR;   break;
                        case CFGTOK_LONG:   AddrSize = ADDR_SIZE_LONG;  break;
                        case CFGTOK_ZP:     AddrSize = ADDR_SIZE_ZP;    break;
                        default:
                            Internal ("Unexpected token: %d", CfgTok);
                    }
                    CfgNextTok ();
                    break;

                case CFGTOK_TYPE:
                    /* Don't allow this twice */
                    FlagAttr (&AttrFlags, atType, "TYPE");
                    /* Map the type to a token */
                    CfgSpecialToken (Types, ENTRY_COUNT (Types), "Type");
                    switch (CfgTok) {
                        case CFGTOK_EXPORT:     Type = CfgSymExport;    break;
                        case CFGTOK_IMPORT:     Type = CfgSymImport;    break;
                        case CFGTOK_WEAK:       Type = CfgSymWeak;      break;
                        default:
                            Internal ("Unexpected token: %d", CfgTok);
                    }
                    CfgNextTok ();
                    break;

                case CFGTOK_VALUE:
                    /* Don't allow this twice */
                    FlagAttr (&AttrFlags, atValue, "VALUE");
                    /* Value is an expression */
                    Value = CfgExpr ();
                    break;

                default:
                    FAIL ("Unexpected attribute token");

            }

            /* Semicolon ends the decl, otherwise accept an optional comma */
            if (CfgTok == CFGTOK_SEMI) {
                break;
            } else if (CfgTok == CFGTOK_COMMA) {
                CfgNextTok ();
            }
        }

        /* We must have a type */
        AttrCheck (AttrFlags, atType, "TYPE");

        /* Further actions depend on the type */
        switch (Type) {

            case CfgSymExport:
                /* We must have a value */
                AttrCheck (AttrFlags, atValue, "VALUE");
                /* Create the export */
                Exp = CreateExprExport (Name, Value, AddrSize);
                CollAppend (&Exp->DefLines, GenLineInfo (&CfgErrorPos));
                break;

            case CfgSymImport:
                /* An import must not have a value */
                if (AttrFlags & atValue) {
                    CfgError (&CfgErrorPos, "Imports must not have a value");
                }
                /* Generate the import */
                Imp = InsertImport (GenImport (Name, AddrSize));
                /* Remember the file position */
                CollAppend (&Imp->RefLines, GenLineInfo (&CfgErrorPos));
                break;

            case CfgSymWeak:
                /* We must have a value */
                AttrCheck (AttrFlags, atValue, "VALUE");
                /* Remember the symbol for later */
                Sym = NewCfgSymbol (CfgSymWeak, Name);
                Sym->Value = Value;
                Sym->AddrSize = AddrSize;
                break;

            default:
                Internal ("Unexpected symbol type %d", Type);
        }

        /* Skip the semicolon */
        CfgConsumeSemi ();
    }

    /* Remember we had this section */
    SectionsEncountered |= SE_SYMBOLS;
}



static void ParseConfig (void)
/* Parse the config file */
{
    static const IdentTok BlockNames [] = {
        {   "MEMORY",   CFGTOK_MEMORY   },
        {   "FILES",    CFGTOK_FILES    },
        {   "SEGMENTS", CFGTOK_SEGMENTS },
        {   "FORMATS",  CFGTOK_FORMATS  },
        {   "FEATURES", CFGTOK_FEATURES },
        {   "SYMBOLS",  CFGTOK_SYMBOLS  },
    };
    cfgtok_t BlockTok;

    do {

        /* Read the block ident */
        CfgSpecialToken (BlockNames, ENTRY_COUNT (BlockNames), "Block identifier");
        BlockTok = CfgTok;
        CfgNextTok ();

        /* Expected a curly brace */
        CfgConsume (CFGTOK_LCURLY, "'{' expected");

        /* Read the block */
        switch (BlockTok) {

            case CFGTOK_MEMORY:
                ParseMemory ();
                break;

            case CFGTOK_FILES:
                ParseFiles ();
                break;

            case CFGTOK_SEGMENTS:
                ParseSegments ();
                break;

            case CFGTOK_FORMATS:
                ParseFormats ();
                break;

            case CFGTOK_FEATURES:
                ParseFeatures ();
                break;

            case CFGTOK_SYMBOLS:
                ParseSymbols ();
                break;

            default:
                FAIL ("Unexpected block token");

        }

        /* Skip closing brace */
        CfgConsume (CFGTOK_RCURLY, "'}' expected");

    } while (CfgTok != CFGTOK_EOF);
}



void CfgRead (void)
/* Read the configuration */
{
    /* Create the descriptors for the binary formats */
    BinFmtDesc = NewBinDesc ();
    O65FmtDesc = NewO65Desc ();
    XexFmtDesc = NewXexDesc ();

    /* If we have a config name given, open the file, otherwise we will read
    ** from a buffer.
    */
    CfgOpenInput ();

    /* Parse the file */
    ParseConfig ();

    /* Close the input file */
    CfgCloseInput ();
}



/*****************************************************************************/
/*                          Config file processing                           */
/*****************************************************************************/



static void ProcessSegments (void)
/* Process the SEGMENTS section */
{
    unsigned I;

    /* Walk over the list of segment descriptors */
    I = 0;
    while (I < CollCount (&SegDescList)) {

        /* Get the next segment descriptor */
        SegDesc* S = CollAtUnchecked (&SegDescList, I);

        /* Search for the actual segment in the input files. The function may
        ** return NULL (no such segment), this is checked later.
        */
        S->Seg = SegFind (S->Name);

        /* If the segment is marked as BSS style, and if the segment exists
        ** in any of the object file, check that there's no initialized data
        ** in the segment.
        */
        if ((S->Flags & SF_BSS) != 0 && S->Seg != 0 && !IsBSSType (S->Seg)) {
            CfgWarning (GetSourcePos (S->LI),
                        "Segment '%s' with type 'bss' contains initialized data",
                        GetString (S->Name));
        }

        /* If this segment does exist in any of the object files, insert the
        ** segment into the load/run memory areas. Otherwise print a warning
        ** and discard it, because the segment pointer in the descriptor is
        ** invalid.
        */
        if (S->Seg != 0) {

            /* Insert the segment into the memory area list */
            MemoryInsert (S->Run, S);
            if (S->Load != S->Run) {
                /* We have separate RUN and LOAD areas */
                MemoryInsert (S->Load, S);
            }

            /* Use the fill value from the config */
            S->Seg->FillVal = S->FillVal;

            /* Process the next segment descriptor in the next run */
            ++I;

        } else {

            /* Print a warning if the segment is not optional */
            if ((S->Flags & SF_OPTIONAL) == 0) {
                CfgWarning (&CfgErrorPos,
                            "Segment '%s' does not exist",
                            GetString (S->Name));
            }

            /* Discard the descriptor and remove it from the collection */
            FreeSegDesc (S);
            CollDelete (&SegDescList, I);
        }
    }
}



static void ProcessSymbols (void)
/* Process the SYMBOLS section */
{
    Export* E;

    /* Walk over all symbols */
    unsigned I;
    for (I = 0; I < CollCount (&CfgSymbols); ++I) {

        /* Get the next symbol */
        CfgSymbol* Sym = CollAtUnchecked (&CfgSymbols, I);

        /* Check what it is. */
        switch (Sym->Type) {

            case CfgSymO65Export:
                /* Check if the export symbol is also defined as an import. */
                if (O65GetImport (O65FmtDesc, Sym->Name) != 0) {
                    CfgError (
                        GetSourcePos (Sym->LI),
                        "Exported o65 symbol '%s' cannot also be an o65 import",
                        GetString (Sym->Name)
                    );
                }

                /* Check if we have this symbol defined already. The entry
                ** routine will check this also, but we get a more verbose
                ** error message when checking it here.
                */
                if (O65GetExport (O65FmtDesc, Sym->Name) != 0) {
                    CfgError (
                        GetSourcePos (Sym->LI),
                        "Duplicate exported o65 symbol: '%s'",
                        GetString (Sym->Name)
                    );
                }

                /* Insert the symbol into the table */
                O65SetExport (O65FmtDesc, Sym->Name);
                break;

            case CfgSymO65Import:
                /* Check if the import symbol is also defined as an export. */
                if (O65GetExport (O65FmtDesc, Sym->Name) != 0) {
                    CfgError (
                        GetSourcePos (Sym->LI),
                        "Imported o65 symbol '%s' cannot also be an o65 export",
                        GetString (Sym->Name)
                    );
                }

                /* Check if we have this symbol defined already. The entry
                ** routine will check this also, but we get a more verbose
                ** error message when checking it here.
                */
                if (O65GetImport (O65FmtDesc, Sym->Name) != 0) {
                    CfgError (
                        GetSourcePos (Sym->LI),
                        "Duplicate imported o65 symbol: '%s'",
                        GetString (Sym->Name)
                    );
                }

                /* Insert the symbol into the table */
                O65SetImport (O65FmtDesc, Sym->Name);
                break;

            case CfgSymWeak:
                /* If the symbol is not defined until now, define it */
                if ((E = FindExport (Sym->Name)) == 0 || IsUnresolvedExport (E)) {
                    /* The symbol is undefined, generate an export */
                    E = CreateExprExport (Sym->Name, Sym->Value, Sym->AddrSize);
                    CollAppend (&E->DefLines, Sym->LI);
                }
                break;

            default:
                Internal ("Unexpected symbol type %d", Sym->Type);
                break;
        }
    }

}



static void CreateRunDefines (SegDesc* S, unsigned long SegAddr)
/* Create the defines for a RUN segment */
{
    Export* E;
    StrBuf Buf = STATIC_STRBUF_INITIALIZER;

    /* Define the run address of the segment */
    SB_Printf (&Buf, "__%s_RUN__", GetString (S->Name));
    E = CreateMemoryExport (GetStrBufId (&Buf), S->Run, SegAddr - S->Run->Start);
    CollAppend (&E->DefLines, S->LI);

    /* Define the size of the segment */
    SB_Printf (&Buf, "__%s_SIZE__", GetString (S->Name));
    E = CreateConstExport (GetStrBufId (&Buf), S->Seg->Size);
    CollAppend (&E->DefLines, S->LI);

    S->Flags |= SF_RUN_DEF;
    SB_Done (&Buf);
}



static void CreateLoadDefines (SegDesc* S, unsigned long SegAddr)
/* Create the defines for a LOAD segment */
{
    Export* E;
    StrBuf Buf = STATIC_STRBUF_INITIALIZER;

    /* Define the load address of the segment */
    SB_Printf (&Buf, "__%s_LOAD__", GetString (S->Name));
    E = CreateMemoryExport (GetStrBufId (&Buf), S->Load, SegAddr - S->Load->Start);
    CollAppend (&E->DefLines, S->LI);

    S->Flags |= SF_LOAD_DEF;
    SB_Done (&Buf);
}



unsigned CfgProcess (void)
/* Process the config file, after reading in object files and libraries. This
** includes postprocessing of the config file data; but also assigning segments,
** and defining segment/memory-area related symbols. The function will return
** the number of memory area overflows (so, zero means everything went OK).
** In case of overflows, a short mapfile can be generated later, to ease the
** user's task of re-arranging segments.
*/
{
    unsigned Overflows = 0;
    unsigned I;

    /* Postprocess symbols. We must do that first, since weak symbols are
    ** defined here, which may be needed later.
    */
    ProcessSymbols ();

    /* Postprocess segments */
    ProcessSegments ();

    /* Walk through each of the memory sections. Add up the sizes; and, check
    ** for an overflow of the section. Assign the start addresses of the
    ** segments while doing that.
    */
    for (I = 0; I < CollCount (&MemoryAreas); ++I) {
        unsigned J;
        unsigned long Addr;
        unsigned Overwrites = 0;

        /* Get the next memory area */
        MemoryArea* M = CollAtUnchecked (&MemoryAreas, I);

        /* Remember the offset in the output file */
        M->FileOffs = M->F->Size;

        /* Remember if this is a relocatable memory area */
        M->Relocatable = RelocatableBinFmt (M->F->Format);

        /* Resolve the start address expression, remember the start address,
        ** and mark the memory area as placed.
        */
        if (!IsConstExpr (M->StartExpr)) {
            CfgError (GetSourcePos (M->LI),
                      "Start address of memory area '%s' is not constant",
                      GetString (M->Name));
        }
        Addr = M->Start = GetExprVal (M->StartExpr);
        M->Flags |= MF_PLACED;

        /* If requested, define the symbol for the start of the memory area.
        ** Doing it here means that the expression for the size of the area
        ** may reference this symbol.
        */
        if (M->Flags & MF_DEFINE) {
            Export* E;
            StrBuf Buf = STATIC_STRBUF_INITIALIZER;

            /* Define the start of the memory area */
            SB_Printf (&Buf, "__%s_START__", GetString (M->Name));
            E = CreateMemoryExport (GetStrBufId (&Buf), M, 0);
            CollAppend (&E->DefLines, M->LI);

            SB_Done (&Buf);
        }

        /* Resolve the size expression */
        if (!IsConstExpr (M->SizeExpr)) {
            CfgError (GetSourcePos (M->LI),
                      "Size of memory area '%s' is not constant",
                      GetString (M->Name));
        }
        M->Size = GetExprVal (M->SizeExpr);
        if (M->Size >= 0x80000000) {
            CfgError (GetSourcePos (M->LI),
                      "Size of memory area '%s' is negative: %ld",
                      GetString (M->Name), (long)M->Size);
        }

        /* Walk through the segments in this memory area */
        for (J = 0; J < CollCount (&M->SegList); ++J) {
            /* Get the segment */
            SegDesc* S = CollAtUnchecked (&M->SegList, J);

            /* Remember the start address before handling this segment */
            unsigned long StartAddr = Addr;

            /* For computing FillLevel */
            unsigned long FillLevel;
            unsigned long FillAdded = 0;

            /* Take note of "overwrite" segments and make sure there are no
            ** other segment types following them in current memory region.
            */
            if (S->Flags & SF_OVERWRITE) {
                if (S->Flags & (SF_OFFSET | SF_START)) {
                    ++Overwrites;
                } else {
                    CfgError (GetSourcePos (M->LI),
                              "Segment '%s' of type 'overwrite' requires either"
                              " 'Start' or 'Offset' attribute to be specified",
                              GetString (S->Name));
                }
            } else {
                if (Overwrites > 0) {
                    CfgError (GetSourcePos (M->LI),
                              "Segment '%s' is preceded by at least one segment"
                              " of type 'overwrite'",
                              GetString (S->Name));
                }
            }

            /* Some actions depend on whether this is the load or run memory
            ** area.
            */
            if (S->Run == M) {
                /* This is the run (and maybe load) memory area. Handle
                ** alignment and explict start address and offset.
                */

                /* Check if the alignment for the segment from the linker
                ** config is a multiple for that of the segment.
                ** If START or OFFSET is provided instead of ALIGN, check
                ** if its address fits alignment requirements.
                */
                unsigned long AlignedBy = (S->Flags & SF_START) ? S->Addr
                    : (S->Flags & SF_OFFSET) ? (S->Addr + M->Start)
                    : S->RunAlignment;
                if ((AlignedBy % S->Seg->Alignment) != 0) {
                    /* Segment requires another alignment than configured
                    ** in the linker.
                    */
                    CfgWarning (GetSourcePos (S->LI),
                                "Segment '%s' isn't aligned properly; the"
                                " resulting executable might not be functional.",
                                GetString (S->Name));
                }

                if (S->Flags & SF_ALIGN) {
                    /* Align the address */
                    unsigned long NewAddr = AlignAddr (Addr, S->RunAlignment);

                    /* If the first segment placed in the memory area needs
                    ** fill bytes for the alignment, emit a warning, since
                    ** that is somewhat suspicious.
                    */
                    if (M->FillLevel == 0 && NewAddr > Addr) {
                        CfgWarning (GetSourcePos (S->LI),
                                    "The first segment in memory area '%s' "
                                    "needs fill bytes for alignment.",
                                    GetString (M->Name));
                    }

                    /* Use the aligned address */
                    Addr = NewAddr;

                } else if ((S->Flags & (SF_OFFSET | SF_START)) != 0 &&
                           (M->Flags & MF_OVERFLOW) == 0) {
                    /* Give the segment a fixed starting address */
                    unsigned long NewAddr = S->Addr;

                    if (S->Flags & SF_OFFSET) {
                        /* An offset was given, no address, make an address */
                        NewAddr += M->Start;
                    }

                    if (S->Flags & SF_OVERWRITE) {
                        if (NewAddr < M->Start) {
                            CfgError (GetSourcePos (S->LI),
                                      "Segment '%s' begins before memory area '%s'",
                                      GetString (S->Name), GetString (M->Name));
                        } else {
                            Addr = NewAddr;
                        }
                    } else {
                        if (NewAddr < Addr) {
                            /* Offset already too large */
                            ++Overflows;
                            if (S->Flags & SF_OFFSET) {
                                CfgWarning (GetSourcePos (S->LI),
                                            "Segment '%s' offset is too small in '%s' by %lu byte%s",
                                            GetString (S->Name), GetString (M->Name),
                                            Addr - NewAddr, (Addr - NewAddr == 1) ? "" : "s");
                            } else {
                                CfgWarning (GetSourcePos (S->LI),
                                            "Segment '%s' start address is too low in '%s' by %lu byte%s",
                                            GetString (S->Name), GetString (M->Name),
                                            Addr - NewAddr, (Addr - NewAddr == 1) ? "" : "s");
                            }
                        } else {
                            Addr = NewAddr;
                        }
                    }
                }

                /* Set the start address of this segment, set the readonly flag
                ** in the segment, and remember if the segment is in a
                ** relocatable file or not.
                */
                S->Seg->PC = Addr;
                S->Seg->ReadOnly = (S->Flags & SF_RO) != 0;

                /* Remember the run memory for this segment, which is also a
                ** flag that the segment has been placed.
                */
                S->Seg->MemArea = M;

            } else if (S->Load == M) {
                /* This is the load memory area; *and*, run and load are
                ** different (because of the "else" above). Handle alignment.
                */
                if (S->Flags & SF_ALIGN_LOAD) {
                    /* Align the address */
                    Addr = AlignAddr (Addr, S->LoadAlignment);
                }
            }

            /* If this is the load memory area, and the segment doesn't have a
            ** fill value defined, use the one from the memory area.
            */
            if (S->Load == M && (S->Flags & SF_FILLVAL) == 0) {
                S->Seg->FillVal = M->FillVal;
            }

            /* Increment the fill level of the memory area; and, check for an
            ** overflow.
            */
            FillLevel = Addr + S->Seg->Size - M->Start;
            if (FillLevel > M->Size && (M->Flags & MF_OVERFLOW) == 0) {
                ++Overflows;
                M->Flags |= MF_OVERFLOW;
                CfgWarning (GetSourcePos (M->LI),
                            "Segment '%s' overflows memory area '%s' by %lu byte%s",
                            GetString (S->Name), GetString (M->Name),
                            FillLevel - M->Size, (FillLevel - M->Size == 1) ? "" : "s");
            }
            if (FillLevel > M->FillLevel) {
                /* Regular segments increase FillLevel. Overwrite segments may increase but not decrease FillLevel. */
                FillAdded = FillLevel - M->FillLevel;
                M->FillLevel = FillLevel;
            }

            /* If requested, define symbols for the start and size of the
            ** segment.
            */
            if (S->Flags & SF_DEFINE) {
                if (S->Run == M && (S->Flags & SF_RUN_DEF) == 0) {
                    CreateRunDefines (S, Addr);
                }
                if (S->Load == M && (S->Flags & SF_LOAD_DEF) == 0) {
                    CreateLoadDefines (S, Addr);
                }
            }

            /* Calculate the new address */
            Addr += S->Seg->Size;

            /* If this segment will go out to the file, or its place
            ** in the file will be filled, then increase the file size.
            ** An OVERWRITE segment will only increase the size if it overlapped some of the fill area.
            */
            if (S->Load == M &&
                ((S->Flags & SF_BSS) == 0 || (M->Flags & MF_FILL) != 0)) {
                M->F->Size += (!(S->Flags & SF_OVERWRITE)) ?
                    (Addr - StartAddr) :
                    FillAdded;
            }
        }

        /* If requested, define symbols for start, size, and offset of the
        ** memory area
        */
        if (M->Flags & MF_DEFINE) {
            Export* E;
            StrBuf Buf = STATIC_STRBUF_INITIALIZER;

            /* Define the size of the memory area */
            SB_Printf (&Buf, "__%s_SIZE__", GetString (M->Name));
            E = CreateConstExport (GetStrBufId (&Buf), M->Size);
            CollAppend (&E->DefLines, M->LI);

            /* Define the fill level of the memory area */
            SB_Printf (&Buf, "__%s_LAST__", GetString (M->Name));
            E = CreateMemoryExport (GetStrBufId (&Buf), M, M->FillLevel);
            CollAppend (&E->DefLines, M->LI);

            /* Define the file offset of the memory area. This isn't of much
            ** use for relocatable output files.
            */
            if (!M->Relocatable) {
                SB_Printf (&Buf, "__%s_FILEOFFS__", GetString (M->Name));
                E = CreateConstExport (GetStrBufId (&Buf), M->FileOffs);
                CollAppend (&E->DefLines, M->LI);
            }

            /* Throw away the string buffer */
            SB_Done (&Buf);
        }

        /* If we didn't have an overflow, and are requested to fill the memory
        ** area, account for that in the file size.
        */
        if ((M->Flags & MF_OVERFLOW) == 0 && (M->Flags & MF_FILL) != 0) {
            M->F->Size += (M->Size - M->FillLevel);
        }
    }

    /* Return the number of memory area overflows */
    return Overflows;
}



void CfgWriteTarget (void)
/* Write the target file(s) */
{
    unsigned I;

    /* Walk through the files list */
    for (I = 0; I < CollCount (&FileList); ++I) {

        /* Get this entry */
        File* F = CollAtUnchecked (&FileList, I);

        /* We don't need to look at files with no memory areas */
        if (CollCount (&F->MemoryAreas) > 0) {

            /* Is there an output file? */
            if (SB_GetLen (GetStrBuf (F->Name)) > 0) {

                /* Assign a proper binary format */
                if (F->Format == BINFMT_DEFAULT) {
                    F->Format = DefaultBinFmt;
                }

                /* Call the apropriate routine for the binary format */
                switch (F->Format) {

                    case BINFMT_BINARY:
                        BinWriteTarget (BinFmtDesc, F);
                        break;

                    case BINFMT_O65:
                        O65WriteTarget (O65FmtDesc, F);
                        break;

                    case BINFMT_ATARIEXE:
                        XexWriteTarget (XexFmtDesc, F);
                        break;

                    default:
                        Internal ("Invalid binary format: %u", F->Format);

                }

            } else {

                /* No output file. Walk through the list and mark all segments
                ** loading into these memory areas in this file as dumped.
                */
                unsigned J;
                for (J = 0; J < CollCount (&F->MemoryAreas); ++J) {

                    unsigned K;

                    /* Get this entry */
                    MemoryArea* M = CollAtUnchecked (&F->MemoryAreas, J);

                    /* Debugging */
                    Print (stdout, 2, "Skipping '%s'...\n", GetString (M->Name));

                    /* Walk throught the segments */
                    for (K = 0; K < CollCount (&M->SegList); ++K) {
                        SegDesc* S = CollAtUnchecked (&M->SegList, K);
                        if (S->Load == M) {
                            /* Load area - mark the segment as dumped */
                            S->Seg->Dumped = 1;
                        }
                    }
                }
            }
        }
    }
}
