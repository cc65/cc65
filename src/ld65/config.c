/*****************************************************************************/
/*                                                                           */
/*				   config.c				     */
/*                                                                           */
/*		 Target configuration file for the ld65 linker		     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2000 Ullrich von Bassewitz                                       */
/*               Wacholderweg 14                                             */
/*               D-70597 Stuttgart                                           */
/* EMail:        uz@musoftware.de                                            */
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
#include "check.h"
#include "bitops.h"
#include "print.h"
#include "xmalloc.h"

/* ld65 */
#include "bin.h"
#include "binfmt.h"
#include "condes.h"
#include "error.h"
#include "exports.h"
#include "global.h"
#include "o65.h"
#include "scanner.h"
#include "config.h"



/*****************************************************************************/
/*     	      	    		     Data				     */
/*****************************************************************************/



/* File list */
static File*	    	FileList; 	/* Single linked list */
static unsigned	    	FileCount;  	/* Number of entries in the list */



/* Memory list */
static Memory*	    	MemoryList; 	/* Single linked list */
static Memory*		MemoryLast;	/* Last element in list */
static unsigned	    	MemoryCount;	/* Number of entries in the list */

/* Memory attributes */
#define MA_START       	0x0001
#define MA_SIZE        	0x0002
#define MA_TYPE        	0x0004
#define MA_FILE        	0x0008
#define MA_DEFINE      	0x0010
#define MA_FILL	       	0x0020
#define MA_FILLVAL     	0x0040



/* Segment list */
SegDesc*	       	SegDescList;	/* Single linked list */
unsigned	       	SegDescCount;	/* Number of entries in list */

/* Segment attributes */
#define SA_TYPE	       	0x0001
#define SA_LOAD		0x0002
#define SA_RUN		0x0004
#define SA_ALIGN	0x0008
#define SA_DEFINE	0x0010
#define SA_OFFSET	0x0020
#define SA_START	0x0040



/* Descriptor holding information about the binary formats */
static BinDesc*	BinFmtDesc	= 0;
static O65Desc* O65FmtDesc	= 0;

/* Attributes for the o65 format */
static unsigned O65Attr	= 0;
#define OA_OS		0x0001
#define OA_TYPE		0x0002
#define OA_VERSION	0x0004
#define OA_OSVERSION	0x0008
#define OA_TEXT		0x0010
#define OA_DATA		0x0020
#define OA_BSS		0x0040
#define OA_ZP		0x0080



/*****************************************************************************/
/*				   Forwards				     */
/*****************************************************************************/



static File* NewFile (const char* Name);
/* Create a new file descriptor and insert it into the list */



/*****************************************************************************/
/*				List management				     */
/*****************************************************************************/



static File* FindFile (const char* Name)
/* Find a file with a given name. */
{
    File* F = FileList;
    while (F) {
 	if (strcmp (F->Name, Name) == 0) {
 	    return F;
 	}
 	F = F->Next;
    }
    return 0;
}



static File* GetFile (const char* Name)
/* Get a file entry with the given name. Create a new one if needed. */
{
    File* F = FindFile (Name);
    if (F == 0) {
	/* Create a new one */
	F = NewFile (Name);
    }
    return F;
}



static void FileInsert (File* F, Memory* M)
/* Insert the memory area into the files list */
{
    M->F = F;
    if (F->MemList == 0) {
	/* First entry */
	F->MemList = M;
    } else {
	F->MemLast->FNext = M;
    }
    F->MemLast = M;
}



static Memory* CfgFindMemory (const char* Name)
/* Find the memory are with the given name. Return NULL if not found */
{
    Memory* M = MemoryList;
    while (M) {
       	if (strcmp (M->Name, Name) == 0) {
       	    return M;
       	}
       	M = M->Next;
    }
    return 0;
}



static Memory* CfgGetMemory (const char* Name)
/* Find the memory are with the given name. Print an error on an invalid name */
{
    Memory* M = CfgFindMemory (Name);
    if (M == 0) {
 	CfgError ("Invalid memory area `%s'", Name);
    }
    return M;
}



static SegDesc* CfgFindSegDesc (const char* Name)
/* Find the segment descriptor with the given name, return NULL if not found. */
{
    SegDesc* S = SegDescList;
    while (S) {
     	if (strcmp (S->Name, Name) == 0) {
    	    /* Found */
    	    return S;
       	}
     	S = S->Next;
    }

    /* Not found */
    return 0;
}



static void SegDescInsert (SegDesc* S)
/* Insert a segment descriptor into the list of segment descriptors */
{
    /* Insert the struct into the list */
    S->Next = SegDescList;
    SegDescList = S;
    ++SegDescCount;
}



static void MemoryInsert (Memory* M, SegDesc* S)
/* Insert the segment descriptor into the memory area list */
{
    /* Create a new node for the entry */
    MemListNode* N = xmalloc (sizeof (MemListNode));
    N->Seg  = S;
    N->Next = 0;

    if (M->SegLast == 0) {
       	/* First entry */
       	M->SegList = N;
    } else {
 	M->SegLast->Next = N;
    }
    M->SegLast = N;
}



/*****************************************************************************/
/*			   Constructors/Destructors			     */
/*****************************************************************************/



static File* NewFile (const char* Name)
/* Create a new file descriptor and insert it into the list */
{
    /* Get the length of the name */
    unsigned Len = strlen (Name);

    /* Allocate memory */
    File* F = xmalloc (sizeof (File) + Len);

    /* Initialize the fields */
    F->Flags   = 0;
    F->Format  = BINFMT_DEFAULT;
    F->MemList = 0;
    F->MemLast = 0;
    memcpy (F->Name, Name, Len);
    F->Name [Len] = '\0';

    /* Insert the struct into the list */
    F->Next  = FileList;
    FileList = F;
    ++FileCount;

    /* ...and return it */
    return F;
}



static Memory* NewMemory (const char* Name)
/* Create a new memory section and insert it into the list */
{
    /* Get the length of the name */
    unsigned Len = strlen (Name);

    /* Check for duplicate names */
    Memory* M =	CfgFindMemory (Name);
    if (M) {
	CfgError ("Memory area `%s' defined twice", Name);
    }

    /* Allocate memory */
    M = xmalloc (sizeof (Memory) + Len);

    /* Initialize the fields */
    M->Next	 = 0;
    M->FNext     = 0;
    M->Attr      = 0;
    M->Flags     = 0;
    M->Start     = 0;
    M->Size      = 0;
    M->FillLevel = 0;
    M->FillVal   = 0;
    M->SegList   = 0;
    M->SegLast   = 0;
    M->F         = 0;
    memcpy (M->Name, Name, Len);
    M->Name [Len] = '\0';

    /* Insert the struct into the list */
    if (MemoryLast == 0) {
	/* First element */
	MemoryList = M;
    } else {
	MemoryLast->Next = M;
    }
    MemoryLast = M;
    ++MemoryCount;

    /* ...and return it */
    return M;
}



static SegDesc* NewSegDesc (const char* Name)
/* Create a segment descriptor */
{
    Segment* Seg;

    /* Get the length of the name */
    unsigned Len = strlen (Name);

    /* Check for duplicate names */
    SegDesc* S = CfgFindSegDesc (Name);
    if (S) {
	CfgError ("Segment `%s' defined twice", Name);
    }

    /* Verify that the given segment does really exist */
    Seg = SegFind (Name);
    if (Seg == 0) {
       	CfgWarning ("Segment `%s' does not exist", Name);
    }

    /* Allocate memory */
    S = xmalloc (sizeof (SegDesc) + Len);

    /* Initialize the fields */
    S->Next    = 0;
    S->Seg     = Seg;
    S->Attr    = 0;
    S->Flags   = 0;
    S->Align   = 0;
    memcpy (S->Name, Name, Len);
    S->Name [Len] = '\0';

    /* ...and return it */
    return S;
}



static void FreeSegDesc (SegDesc* S)
/* Free a segment descriptor */
{
    xfree (S);
}



/*****************************************************************************/
/*     	       	       	      	     Code     				     */
/*****************************************************************************/



static void FlagAttr (unsigned* Flags, unsigned Mask, const char* Name)
/* Check if the item is already defined. Print an error if so. If not, set
 * the marker that we have a definition now.
 */
{
    if (*Flags & Mask) {
    	CfgError ("%s is already defined", Name);
    }
    *Flags |= Mask;
}



static void AttrCheck (unsigned Attr, unsigned Mask, const char* Name)
/* Check that a mandatory attribute was given */
{
    if ((Attr & Mask) == 0) {
	CfgError ("%s attribute is missing", Name);
    }
}



static void ParseMemory (void)
/* Parse a MEMORY section */
{
    static const IdentTok Attributes [] = {
       	{   "START",  	CFGTOK_START    },
	{   "SIZE",	CFGTOK_SIZE     },
        {   "TYPE",     CFGTOK_TYPE     },
        {   "FILE",     CFGTOK_FILE     },
        {   "DEFINE",   CFGTOK_DEFINE   },
  	{   "FILL",	CFGTOK_FILL     },
       	{   "FILLVAL", 	CFGTOK_FILLVAL  },
    };
    static const IdentTok Types [] = {
       	{   "RO",    	CFGTOK_RO       },
       	{   "RW",   	CFGTOK_RW       },
    };

    while (CfgTok == CFGTOK_IDENT) {

	/* Create a new entry on the heap */
       	Memory* M = NewMemory (CfgSVal);

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

		case CFGTOK_START:
		    FlagAttr (&M->Attr, MA_START, "START");
		    CfgAssureInt ();
		    M->Start = CfgIVal;
		    break;

	      	case CFGTOK_SIZE:
		    FlagAttr (&M->Attr, MA_SIZE, "SIZE");
		    CfgAssureInt ();
	      	    M->Size = CfgIVal;
		    break;

		case CFGTOK_TYPE:
		    FlagAttr (&M->Attr, MA_TYPE, "TYPE");
      		    CfgSpecialToken (Types, ENTRY_COUNT (Types), "Type");
		    if (CfgTok == CFGTOK_RO) {
			M->Flags |= MF_RO;
		    }
		    break;

	        case CFGTOK_FILE:
		    FlagAttr (&M->Attr, MA_FILE, "FILE");
		    CfgAssureStr ();
       	       	    /* Get the file entry and insert the memory area */
	    	    FileInsert (GetFile (CfgSVal), M);
		    break;

	        case CFGTOK_DEFINE:
 		    FlagAttr (&M->Attr, MA_DEFINE, "DEFINE");
		    /* Map the token to a boolean */
		    CfgBoolToken ();
		    if (CfgTok == CFGTOK_TRUE) {
	  	    	M->Flags |= MF_DEFINE;
		    }
		    break;

	        case CFGTOK_FILL:
 		    FlagAttr (&M->Attr, MA_FILL, "FILL");
		    /* Map the token to a boolean */
		    CfgBoolToken ();
		    if (CfgTok == CFGTOK_TRUE) {
	  	    	M->Flags |= MF_FILL;
		    }
		    break;

	      	case CFGTOK_FILLVAL:
		    FlagAttr (&M->Attr, MA_FILLVAL, "FILLVAL");
		    CfgAssureInt ();
		    CfgRangeCheck (0, 0xFF);
	      	    M->FillVal = (unsigned char) CfgIVal;
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

	/* Check for mandatory parameters */
       	AttrCheck (M->Attr, MA_START, "START");
	AttrCheck (M->Attr, MA_SIZE, "SIZE");

	/* If we don't have a file name for output given, use the default
	 * file name.
	 */
	if ((M->Attr & MA_FILE) == 0) {
	    FileInsert (GetFile (OutputName), M);
	}
    }
}



static void ParseFiles (void)
/* Parse a FILES section */
{
    static const IdentTok Attributes [] = {
       	{   "FORMAT",  	CFGTOK_FORMAT   },
    };
    static const IdentTok Formats [] = {
       	{   "O65",     	CFGTOK_O65	     },
       	{   "BIN",     	CFGTOK_BIN      },
       	{   "BINARY",   CFGTOK_BIN      },
    };


    /* Parse all files */
    while (CfgTok != CFGTOK_RCURLY) {

	File* F;

	/* We expect a string value here */
	CfgAssureStr ();

	/* Search for the file, it must exist */
       	F = FindFile (CfgSVal);
	if (F == 0) {
	    CfgError ("No such file: `%s'", CfgSVal);
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
		  	Error ("Cannot set a file format twice");
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
}



static void ParseSegments (void)
/* Parse a SEGMENTS section */
{
    static const IdentTok Attributes [] = {
       	{   "LOAD",  	CFGTOK_LOAD     },
	{   "RUN",    	CFGTOK_RUN      },
        {   "TYPE",     CFGTOK_TYPE     },
        {   "ALIGN",    CFGTOK_ALIGN    },
        {   "DEFINE",   CFGTOK_DEFINE   },
	{   "OFFSET", 	CFGTOK_OFFSET   },
	{   "START",  	CFGTOK_START    },
    };
    static const IdentTok Types [] = {
       	{   "RO",     	CFGTOK_RO       },
       	{   "RW",     	CFGTOK_RW       },
       	{   "BSS",    	CFGTOK_BSS      },
	{   "ZP",     	CFGTOK_ZP	},
	{   "WP",     	CFGTOK_WPROT	},
	{   "WPROT",  	CFGTOK_WPROT	},
    };

    unsigned Count;

    while (CfgTok == CFGTOK_IDENT) {

	SegDesc* S;

	/* Create a new entry on the heap */
       	S = NewSegDesc (CfgSVal);

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

		case CFGTOK_LOAD:
	      	    FlagAttr (&S->Attr, SA_LOAD, "LOAD");
		    S->Load = CfgGetMemory (CfgSVal);
		    break;

		case CFGTOK_RUN:
      		    FlagAttr (&S->Attr, SA_RUN, "RUN");
 		    S->Run = CfgGetMemory (CfgSVal);
		    break;

		case CFGTOK_TYPE:
 		    FlagAttr (&S->Attr, SA_TYPE, "TYPE");
       		    CfgSpecialToken (Types, ENTRY_COUNT (Types), "Type");
		    switch (CfgTok) {
       	       	       	case CFGTOK_RO:	   S->Flags |= SF_RO;               break;
			case CFGTOK_RW:	   /* Default */		    break;
		     	case CFGTOK_BSS:   S->Flags |= SF_BSS;              break;
		     	case CFGTOK_ZP:	   S->Flags |= (SF_BSS | SF_ZP);    break;
		     	case CFGTOK_WPROT: S->Flags |= (SF_RO | SF_WPROT);  break;
		     	default:	   Internal ("Unexpected token: %d", CfgTok);
		    }
		    break;

	        case CFGTOK_ALIGN:
		    CfgAssureInt ();
		    FlagAttr (&S->Attr, SA_ALIGN, "ALIGN");
		    CfgRangeCheck (1, 0x10000);
		    S->Align = BitFind (CfgIVal);
		    if ((0x01UL << S->Align) != CfgIVal) {
		     	CfgError ("Alignment must be a power of 2");
		    }
		    S->Flags |= SF_ALIGN;
		    break;

	        case CFGTOK_DEFINE:
 		    FlagAttr (&S->Attr, SA_DEFINE, "DEFINE");
		    /* Map the token to a boolean */
		    CfgBoolToken ();
		    if (CfgTok == CFGTOK_TRUE) {
	  	     	S->Flags |= SF_DEFINE;
		    }
		    break;

	        case CFGTOK_OFFSET:
	    	    CfgAssureInt ();
		    FlagAttr (&S->Attr, SA_OFFSET, "OFFSET");
		    CfgRangeCheck (1, 0x1000000);
		    S->Addr   = CfgIVal;
		    S->Flags |= SF_OFFSET;
		    break;

	        case CFGTOK_START:
      		    CfgAssureInt ();
		    FlagAttr (&S->Attr, SA_START, "START");
		    CfgRangeCheck (1, 0x1000000);
		    S->Addr   = CfgIVal;
		    S->Flags |= SF_START;
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

	/* Check for mandatory parameters */
	AttrCheck (S->Attr, SA_LOAD, "LOAD");

 	/* Set defaults for stuff not given */
	if ((S->Attr & SA_RUN) == 0) {
	    S->Attr |= SA_RUN;
	    S->Run = S->Load;
	} else {
	    /* Both attributes given */
	    S->Flags |= SF_LOAD_AND_RUN;
	}
	if ((S->Attr & SA_ALIGN) == 0) {
	    S->Attr |= SA_ALIGN;
	    S->Align = 0;
	}

	/* If the segment is marked as BSS style, check that there's no
	 * initialized data in the segment.
	 */
	if ((S->Flags & SF_BSS) != 0 && !IsBSSType (S->Seg)) {
	    Warning ("%s(%u): Segment with type `bss' contains initialized data",
		     CfgGetName (), CfgErrorLine);
	}

      	/* Don't allow read/write data to be put into a readonly area */
      	if ((S->Flags & SF_RO) == 0) {
       	    if (S->Run->Flags & MF_RO) {
      		CfgError ("Cannot put r/w segment `%s' in r/o memory area `%s'",
      		     	  S->Name, S->Run->Name);
      	    }
      	}

      	/* Only one of ALIGN, START and OFFSET may be used */
       	Count = ((S->Flags & SF_ALIGN)  != 0) +
      	       	((S->Flags & SF_OFFSET) != 0) +
      		((S->Flags & SF_START)  != 0);
      	if (Count > 1) {
       	    CfgError ("Only one of ALIGN, START, OFFSET may be used");
      	}

      	/* If this segment does exist in any of the object files, insert the
      	 * descriptor into the list of segment descriptors. Otherwise discard
      	 * it silently, because the segment pointer in the descriptor is
      	 * invalid.
      	 */
      	if (S->Seg != 0) {
	    /* Insert the descriptor into the list of all descriptors */
	    SegDescInsert (S);
      	    /* Insert the segment into the memory area list */
      	    MemoryInsert (S->Run, S);
      	    if ((S->Flags & SF_LOAD_AND_RUN) != 0) {
      		/* We have a separate RUN area given */
      		MemoryInsert (S->Load, S);
      	    }
      	} else {
      	    /* Segment does not exist, discard the descriptor */
      	    FreeSegDesc (S);
      	}
    }
}



static void ParseO65 (void)
/* Parse the o65 format section */
{
    static const IdentTok Attributes [] = {
       	{   "EXPORT",  	 CFGTOK_EXPORT   	},
	{   "IMPORT",	 CFGTOK_IMPORT		},
        {   "TYPE",      CFGTOK_TYPE     	},
       	{   "OS",      	 CFGTOK_OS       	},
    };
    static const IdentTok Types [] = {
       	{   "SMALL",   	 CFGTOK_SMALL    	},
       	{   "LARGE",   	 CFGTOK_LARGE    	},
    };
    static const IdentTok OperatingSystems [] = {
       	{   "LUNIX",   	 CFGTOK_LUNIX     	},
       	{   "OSA65",   	 CFGTOK_OSA65    	},
    };

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
	      	/* We expect an identifier */
		CfgAssureIdent ();
      		/* Check if we have this symbol defined already. The entry
      		 * routine will check this also, but we get a more verbose
      		 * error message when checking it here.
      		 */
      	 	if (O65GetExport (O65FmtDesc, CfgSVal) != 0) {
      	 	    CfgError ("Duplicate exported symbol: `%s'", CfgSVal);
      	 	}
		/* Insert the symbol into the table */
		O65SetExport (O65FmtDesc, CfgSVal);
	    	break;

	    case CFGTOK_IMPORT:
	      	/* We expect an identifier */
		CfgAssureIdent ();
      		/* Check if we have this symbol defined already. The entry
      		 * routine will check this also, but we get a more verbose
      		 * error message when checking it here.
      		 */
      	 	if (O65GetImport (O65FmtDesc, CfgSVal) != 0) {
      	 	    CfgError ("Duplicate imported symbol: `%s'", CfgSVal);
      	 	}
		/* Insert the symbol into the table */
		O65SetImport (O65FmtDesc, CfgSVal);
	    	break;

	    case CFGTOK_TYPE:
		/* Cannot have this attribute twice */
		FlagAttr (&O65Attr, OA_TYPE, "TYPE");
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
	    	     	CfgError ("Unexpected type token");
	    	}
	    	break;

	    case CFGTOK_OS:
		/* Cannot use this attribute twice */
		FlagAttr (&O65Attr, OA_OS, "OS");
		/* Get the operating system */
	    	CfgSpecialToken (OperatingSystems, ENTRY_COUNT (OperatingSystems), "OS type");
		switch (CfgTok) {

		    case CFGTOK_LUNIX:
			O65SetOS (O65FmtDesc, O65OS_LUNIX);
			break;

		    case CFGTOK_OSA65:
			O65SetOS (O65FmtDesc, O65OS_OSA65);
			break;

		    default:
			CfgError ("Unexpected OS token");
		}
		break;

	    default:
		FAIL ("Unexpected attribute token");

	}

	/* Skip the attribute value and an optional comma */
	CfgNextTok ();
	CfgOptionalComma ();
    }
}



static void ParseFormats (void)
/* Parse a target format section */
{
    static const IdentTok Formats [] = {
       	{   "O65",     	CFGTOK_O65      },
       	{   "BIN",     	CFGTOK_BIN      },
       	{   "BINARY",   CFGTOK_BIN      },
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

	    case CFGTOK_BIN:
		/* No attribibutes available */
		break;

	    default:
		Error ("Unexpected format token");
	}

	/* Skip the semicolon */
	CfgConsumeSemi ();
    }
}



static void ParseConDes (void)
/* Parse the CONDES feature */
{
    static const IdentTok Attributes [] = {
       	{   "SEGMENT",		CFGTOK_SEGMENT		},
	{   "LABEL",  		CFGTOK_LABEL  		},
	{   "COUNT",		CFGTOK_COUNT		},
	{   "TYPE",		CFGTOK_TYPE   		},
	{   "ORDER",		CFGTOK_ORDER		},
    };

    static const IdentTok Types [] = {
       	{   "CONSTRUCTOR",	CFGTOK_CONSTRUCTOR	},
	{   "DESTRUCTOR",      	CFGTOK_DESTRUCTOR	},
    };

    static const IdentTok Orders [] = {
	{   "DECREASING",      	CFGTOK_DECREASING	},
       	{   "INCREASING",      	CFGTOK_INCREASING	},
    };

    /* Attribute values. */
    char SegName[sizeof (CfgSVal)];
    char Label[sizeof (CfgSVal)];
    char Count[sizeof (CfgSVal)];
    /* Initialize to avoid gcc warnings: */
    int Type = -1;
    ConDesOrder Order = cdIncreasing;

    /* Bitmask to remember the attributes we got already */
    enum {
	atNone		= 0x0000,
	atSegName	= 0x0001,
	atLabel		= 0x0002,
	atCount		= 0x0004,
	atType		= 0x0008,
	atOrder		= 0x0010
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

	    case CFGTOK_SEGMENT:
	  	/* Don't allow this twice */
		FlagAttr (&AttrFlags, atSegName, "SEGMENT");
	      	/* We expect an identifier */
		CfgAssureIdent ();
		/* Remember the value for later */
		strcpy (SegName, CfgSVal);
	    	break;

	    case CFGTOK_LABEL:
	       	/* Don't allow this twice */
		FlagAttr (&AttrFlags, atLabel, "LABEL");
	      	/* We expect an identifier */
		CfgAssureIdent ();
		/* Remember the value for later */
		strcpy (Label, CfgSVal);
		break;

	    case CFGTOK_COUNT:
	       	/* Don't allow this twice */
		FlagAttr (&AttrFlags, atCount, "COUNT");
	      	/* We expect an identifier */
		CfgAssureIdent ();
		/* Remember the value for later */
		strcpy (Count, CfgSVal);
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
		     	case CFGTOK_CONSTRUCTOR: Type = CD_TYPE_CON;	break;
		     	case CFGTOK_DESTRUCTOR:	 Type = CD_TYPE_DES;	break;
		     	default: FAIL ("Unexpected type token");
		    }
		}
		break;

	    case CFGTOK_ORDER:
	       	/* Don't allow this twice */
		FlagAttr (&AttrFlags, atOrder, "ORDER");
		CfgSpecialToken (Orders, ENTRY_COUNT (Orders), "Order");
		switch (CfgTok) {
		    case CFGTOK_DECREASING: Order = cdDecreasing;	break;
		    case CFGTOK_INCREASING: Order = cdIncreasing;	break;
		    default: FAIL ("Unexpected order token");
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
	CfgError ("CONDES attributes for type %d are already defined", Type);
    }

    /* Define the attributes */
    ConDesSetSegName (Type, SegName);
    ConDesSetLabel (Type, Label);
    if (AttrFlags & atCount) {
	ConDesSetCountSym (Type, Count);
    }
    if (AttrFlags & atOrder) {
	ConDesSetOrder (Type, Order);
    }
}



static void ParseFeatures (void)
/* Parse a features section */
{
    static const IdentTok Features [] = {
       	{   "CONDES",  	CFGTOK_CONDES	},
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

	    default:
		Error ("Unexpected feature token");
	}

	/* Skip the semicolon */
	CfgConsumeSemi ();
    }
}



static void ParseSymbols (void)
/* Parse a symbols section */
{
    while (CfgTok == CFGTOK_IDENT) {

	long Val;

	/* Remember the name */
	char Name [sizeof (CfgSVal)];
	strcpy (Name, CfgSVal);
	CfgNextTok ();

	/* Allow an optional assignment */
	CfgOptionalAssign ();

	/* Make sure the next token is an integer, read and skip it */
	CfgAssureInt ();
	Val = CfgIVal;
	CfgNextTok ();

	/* Generate an export with the given value */
	CreateConstExport (Name, Val);

	/* Skip the semicolon */
	CfgConsumeSemi ();
    }
}



static void ParseConfig (void)
/* Parse the config file */
{
    static const IdentTok BlockNames [] = {
	{   "MEMORY",  	CFGTOK_MEMORY	},
	{   "FILES",    CFGTOK_FILES    },
        {   "SEGMENTS", CFGTOK_SEGMENTS },
	{   "FORMATS", 	CFGTOK_FORMATS  },
	{   "FEATURES", CFGTOK_FEATURES	},
	{   "SYMBOLS",	CFGTOK_SYMBOLS 	},
    };
    cfgtok_t BlockTok;

    do {

	/* Read the block ident */
       	CfgSpecialToken (BlockNames, ENTRY_COUNT (BlockNames), "Block identifier");
	BlockTok = CfgTok;
	CfgNextTok ();

	/* Expected a curly brace */
	CfgConsume (CFGTOK_LCURLY, "`{' expected");

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
	CfgConsume (CFGTOK_RCURLY, "`}' expected");

    } while (CfgTok != CFGTOK_EOF);
}



void CfgRead (void)
/* Read the configuration */
{
    /* Create the descriptors for the binary formats */
    BinFmtDesc = NewBinDesc ();
    O65FmtDesc = NewO65Desc ();

    /* If we have a config name given, open the file, otherwise we will read
     * from a buffer.
     */
    CfgOpenInput ();

    /* Parse the file */
    ParseConfig ();

    /* Close the input file */
    CfgCloseInput ();
}



static void CreateRunDefines (Memory* M, SegDesc* S, unsigned long Addr)
/* Create the defines for a RUN segment */
{
    char Buf [256];

    sprintf (Buf, "__%s_RUN__", S->Name);
    CreateMemExport (Buf, M, Addr - M->Start);
    sprintf (Buf, "__%s_SIZE__", S->Name);
    CreateConstExport (Buf, S->Seg->Size);
    S->Flags |= SF_RUN_DEF;
}



static void CreateLoadDefines (Memory* M, SegDesc* S, unsigned long Addr)
/* Create the defines for a LOAD segment */
{
    char Buf [256];

    sprintf (Buf, "__%s_LOAD__", S->Name);
    CreateMemExport (Buf, M, Addr - M->Start);
    S->Flags |= SF_LOAD_DEF;
}



void CfgAssignSegments (void)
/* Assign segments, define linker symbols where requested */
{
    /* Walk through each of the memory sections. Add up the sizes and check
     * for an overflow of the section. Assign the start addresses of the
     * segments while doing this.
     */
    Memory* M = MemoryList;
    while (M) {

     	/* Get the start address of this memory area */
     	unsigned long Addr = M->Start;

     	/* Walk through the segments in this memory area */
     	MemListNode* N = M->SegList;
     	while (N) {

     	    /* Get the segment from the node */
     	    SegDesc* S = N->Seg;

     	    /* Handle ALIGN and OFFSET/START */
     	    if (S->Flags & SF_ALIGN) {
     		/* Align the address */
     		unsigned long Val = (0x01UL << S->Align) - 1;
     		Addr = (Addr + Val) & ~Val;
     	    } else if (S->Flags & (SF_OFFSET | SF_START)) {
     		/* Give the segment a fixed starting address */
     		unsigned long NewAddr = S->Addr;
     		if (S->Flags & SF_OFFSET) {
     		    /* An offset was given, no address, make an address */
     		    NewAddr += M->Start;
     		}
       	       	if (Addr > NewAddr) {
     		    /* Offset already too large */
     		    if (S->Flags & SF_OFFSET) {
     		        Error ("Offset too small in `%s', segment `%s'",
     		     	       M->Name, S->Name);
     		    } else {
     		     	Error ("Start address too low in `%s', segment `%s'",
     		     	       M->Name, S->Name);
     		    }
     		}
     		Addr = NewAddr;
     	    }

       	    /* If this is the run area, set the start address of this segment */
     	    if (S->Run == M) {
     	        S->Seg->PC = Addr;
     	    }

     	    /* Increment the fill level of the memory area and check for an
     	     * overflow.
     	     */
     	    M->FillLevel = Addr + S->Seg->Size - M->Start;
     	    if (M->FillLevel > M->Size) {
     	     	Error ("Memory area overflow in `%s', segment `%s' (%lu bytes)",
     	 	       M->Name, S->Name, M->FillLevel - M->Size);
     	    }

     	    /* If requested, define symbols for the start and size of the
     	     * segment.
     	     */
     	    if (S->Flags & SF_DEFINE) {
		if ((S->Flags & SF_LOAD_AND_RUN) && S->Run == S->Load) {
		    /* RUN and LOAD given and in one memory area.
		     * Be careful: We will encounter this code twice, the
		     * first time when walking the RUN list, second time when
		     * walking the LOAD list. Be sure to define only the
		     * relevant symbols on each walk.
		     */
		    if (S->Load == M) {
		 	if ((S->Flags & SF_LOAD_DEF) == 0) {
			    CreateLoadDefines (M, S, Addr);
		 	} else {
		 	    CHECK ((S->Flags & SF_RUN_DEF) == 0);
		 	    CreateRunDefines (M, S, Addr);
			}
		    }
		} else {
		    /* RUN and LOAD in different memory areas, or RUN not
		     * given, so RUN defaults to LOAD. In the latter case, we
		     * have only one copy of the segment in the area.
		     */
		    if (S->Run == M) {
			CreateRunDefines (M, S, Addr);
		    }
		    if (S->Load == M) {
			CreateLoadDefines (M, S, Addr);
		    }
		}
     	    }

     	    /* Calculate the new address */
     	    Addr += S->Seg->Size;

	    /* Next segment */
	    N = N->Next;
	}

	/* If requested, define symbols for start and size of the memory area */
	if (M->Flags & MF_DEFINE) {
	    char Buf [256];
	    sprintf (Buf, "__%s_START__", M->Name);
	    CreateMemExport (Buf, M, 0);
	    sprintf (Buf, "__%s_SIZE__", M->Name);
	    CreateConstExport (Buf, M->Size);
	    sprintf (Buf, "__%s_LAST__", M->Name);
	    CreateConstExport (Buf, M->FillLevel);
	}

	/* Next memory area */
	M = M->Next;
    }
}



void CfgWriteTarget (void)
/* Write the target file(s) */
{
    Memory* M;

    /* Walk through the files list */
    File* F = FileList;
    while (F) {
  	/* We don't need to look at files with no memory areas */
  	if (F->MemList) {

  	    /* Is there an output file? */
  	    if (strlen (F->Name) > 0) {

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

  		    default:
  		        Internal ("Invalid binary format: %u", F->Format);

  		}

  	    } else {

  		/* No output file. Walk through the list and mark all segments
       	       	 * loading into these memory areas in this file as dumped.
  		 */
  		M = F->MemList;
  		while (M) {

		    MemListNode* N;

		    /* Debugging */
       	       	    Print (stdout, 2, "Skipping `%s'...\n", M->Name);

  		    /* Walk throught the segments */
  		    N = M->SegList;
  		    while (N) {
		      	if (N->Seg->Load == M) {
  		      	    /* Load area - mark the segment as dumped */
  		      	    N->Seg->Seg->Dumped = 1;
  		      	}

		      	/* Next segment node */
		      	N = N->Next;
		    }
		    /* Next memory area */
		    M = M->FNext;
		}
	    }
	}

	/* Next file */
	F = F->Next;
    }
}



