/*****************************************************************************/
/*                                                                           */
/*				    dump.c				     */
/*                                                                           */
/*	    Dump subroutines for the od65 object file dump utility	     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000     Ullrich von Bassewitz                                        */
/*              Wacholderweg 14                                              */
/*              D-70597 Stuttgart                                            */
/* EMail:       uz@musoftware.de                                             */
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



#include <time.h>

/* common */
#include "exprdefs.h"
#include "filepos.h"
#include "objdefs.h"
#include "optdefs.h"
#include "segdefs.h"
#include "symdefs.h"
#include "xmalloc.h"

/* od65 */
#include "error.h"
#include "fileio.h"
#include "dump.h"



/*****************************************************************************/
/*    		    		     Code   				     */
/*****************************************************************************/



static void DumpObjHeaderSection (const char* Name,
		   	          unsigned long Offset,
			          unsigned long Size)
/* Dump a header section */
{
    printf ("    %s:\n", Name);
    printf ("      Offset:%24lu\n", Offset);
    printf ("      Size:  %24lu\n", Size);
}



static char* TimeToStr (unsigned long Time)
/* Convert the time into a string and return it */
{
    /* Get the time and convert to string */
    time_t T = (time_t) Time;
    char*  S = asctime (localtime (&T));

    /* Remove the trailing newline */
    unsigned Len = strlen (S);
    if (Len > 0 && S[Len-1] == '\n') {
	S[Len-1 ] = '\0';
    }

    /* Return the time string */
    return S;
}



static void SkipExpr (FILE* F)
/* Skip an expression from the given file */
{
    /* Read the node tag and handle NULL nodes */
    unsigned char Op = Read8 (F);
    if (Op == EXPR_NULL) {
     	return;
    }

    /* Check the tag and handle the different expression types */
    if (EXPR_IS_LEAF (Op)) {
	switch (Op) {

	    case EXPR_LITERAL:
	   	(void) Read32Signed (F);
	   	break;

	    case EXPR_SYMBOL:
	   	/* Read the import number */
	   	(void) Read16 (F);
	   	break;

	    case EXPR_SEGMENT:
	   	/* Read the segment number */
	   	(void) Read8 (F);
	   	break;

	    default:
	   	Error ("Invalid expression op: %02X", Op);

	}

    } else {

    	/* Not a leaf node */
       	SkipExpr (F);
	SkipExpr (F);

    }
}



static unsigned SkipFragment (FILE* F)
/* Skip a fragment from the given file and return the size */
{
    FilePos Pos;
    unsigned long Size;

    /* Read the fragment type */
    unsigned char Type = Read8 (F);

    /* Handle the different fragment types */
    switch (Type) {

	case FRAG_LITERAL8:
       	    Size = Read8 (F);
	    break;

	case FRAG_LITERAL16:
	    Size = Read16 (F);
	    break;

	case FRAG_LITERAL24:
	    Size = Read24 (F);
	    break;

	case FRAG_LITERAL32:
	    Size = Read32 (F);
	    break;

	case FRAG_EXPR8:
	case FRAG_EXPR16:
	case FRAG_EXPR24:
	case FRAG_EXPR32:
	case FRAG_SEXPR8:
	case FRAG_SEXPR16:
	case FRAG_SEXPR24:
	case FRAG_SEXPR32:
	    Size = Type & FRAG_BYTEMASK;
	    break;

	case FRAG_FILL:
	    Size = Read16 (F);
	    break;

	default:
	    Error ("Unknown fragment type: 0x%02X", Type);
	    /* NOTREACHED */
	    return 0;
    }



    /* Now read the fragment data */
    switch (Type & FRAG_TYPEMASK) {

	case FRAG_LITERAL:
	    /* Literal data */
	    FileSeek (F, ftell (F) + Size);
	    break;

	case FRAG_EXPR:
	case FRAG_SEXPR:
	    /* An expression */
	    SkipExpr (F);
	    break;

    }

    /* Skip the file position of the fragment */
    ReadFilePos (F, &Pos);

    /* Return the size */
    return Size;
}



void DumpObjHeader (FILE* F, unsigned long Offset)
/* Dump the header of the given object file */
{
    ObjHeader H;

    /* Seek to the header position */
    FileSeek (F, Offset);

    /* Read the header */
    ReadObjHeader (F, &H);

    /* Now dump the information */

    /* Output a header */
    printf ("  Header:\n");

    /* Magic */
    printf ("    Magic:%17s0x%08lX\n", "", H.Magic);

    /* Version */
    printf ("    Version:%25u\n", H.Version);

    /* Flags */
    printf ("    Flags:%21s0x%04X  (", "", H.Flags);
    if (H.Flags & OBJ_FLAGS_DBGINFO) {
    	printf ("OBJ_FLAGS_DBGINFO");
    }
    printf (")\n");

    /* Options */
    DumpObjHeaderSection ("Options", H.OptionOffs, H.OptionSize);

    /* Files */
    DumpObjHeaderSection ("Files", H.FileOffs, H.FileSize);

    /* Segments */
    DumpObjHeaderSection ("Segments", H.SegOffs, H.SegSize);

    /* Imports */
    DumpObjHeaderSection ("Imports", H.ImportOffs, H.ImportSize);

    /* Exports */
    DumpObjHeaderSection ("Exports", H.ExportOffs, H.ExportSize);

    /* Debug symbols */
    DumpObjHeaderSection ("Debug symbols", H.DbgSymOffs, H.DbgSymSize);
}



void DumpObjOptions (FILE* F, unsigned long Offset)
/* Dump the file options */
{
    ObjHeader H;
    unsigned Count;
    unsigned I;

    /* Seek to the header position */
    FileSeek (F, Offset);

    /* Read the header */
    ReadObjHeader (F, &H);

    /* Seek to the start of the options */
    FileSeek (F, Offset + H.OptionOffs);

    /* Output a header */
    printf ("  Options:\n");

    /* Read the number of options and print it */
    Count = Read16 (F);
    printf ("    Count:%27u\n", Count);

    /* Read and print all options */
    for (I = 0; I < Count; ++I) {

	unsigned long ArgNum;
	char*  	      ArgStr;
	unsigned      ArgLen;

	/* Read the type of the option */
	unsigned char Type = Read8 (F);

       	/* Get the type of the argument */
	unsigned char ArgType = Type & OPT_ARGMASK;

 	/* Determine which option follows */
	const char* TypeDesc;
	switch (Type) {
       	    case OPT_COMMENT:  	TypeDesc = "OPT_COMMENT";	break;
	    case OPT_AUTHOR:  	TypeDesc = "OPT_AUTHOR";	break;
	    case OPT_TRANSLATOR:TypeDesc = "OPT_TRANSLATOR";	break;
	    case OPT_COMPILER:	TypeDesc = "OPT_COMPILER";	break;
	    case OPT_OS:      	TypeDesc = "OPT_OS";		break;
	    case OPT_DATETIME:	TypeDesc = "OPT_DATETIME";	break;
	    default:	      	TypeDesc = "OPT_UNKNOWN";	break;
	}

	/* Print the header */
	printf ("    Index:%27u\n", I);

	/* Print the data */
	printf ("      Type:%22s0x%02X  (%s)\n", "", Type, TypeDesc);
	switch (ArgType) {

	    case OPT_ARGSTR:
	     	ArgStr = ReadMallocedStr (F);
	    	ArgLen = strlen (ArgStr);
	     	printf ("      Data:%*s\"%s\"\n", 24-ArgLen, "", ArgStr);
	     	xfree (ArgStr);
	     	break;

	    case OPT_ARGNUM:
	     	ArgNum = Read32 (F);
	     	printf ("      Data:%26lu", ArgNum);
		if (Type == OPT_DATETIME) {
		    /* Print the time as a string */
   		    printf ("  (%s)", TimeToStr (ArgNum));
		}
		printf ("\n");
	     	break;

	    default:
	     	/* Unknown argument type. This means that we cannot determine
	     	 * the option length, so we cannot proceed.
	     	 */
	     	Error ("Unknown option type: 0x%02X", Type);
	     	break;
	}
    }
}



void DumpObjFiles (FILE* F, unsigned long Offset)
/* Dump the source files */
{
    ObjHeader H;
    unsigned Count;
    unsigned I;

    /* Seek to the header position */
    FileSeek (F, Offset);

    /* Read the header */
    ReadObjHeader (F, &H);

    /* Seek to the start of the options */
    FileSeek (F, Offset + H.FileOffs);

    /* Output a header */
    printf ("  Files:\n");

    /* Read the number of files and print it */
    Count = Read8 (F);
    printf ("    Count:%27u\n", Count);

    /* Read and print all files */
    for (I = 0; I < Count; ++I) {

	/* Read the data for one file */
	unsigned long MTime = Read32 (F);
	unsigned long Size  = Read32 (F);
	char*	      Name  = ReadMallocedStr (F);
	unsigned      Len   = strlen (Name);

	/* Print the header */
	printf ("    Index:%27u\n", I);

	/* Print the data */
	printf ("      Name:%*s\"%s\"\n", 24-Len, "", Name);
       	printf ("      Size:%26lu\n", Size);
	printf ("      Modification time:%13lu  (%s)\n", MTime, TimeToStr (MTime));

	/* Free the Name */
	xfree (Name);
    }
}



void DumpObjSegments (FILE* F, unsigned long Offset)
/* Dump the segments in the object file */
{
    ObjHeader H;
    unsigned Count;
    unsigned I;
    unsigned FragCount;

    /* Seek to the header position */
    FileSeek (F, Offset);

    /* Read the header */
    ReadObjHeader (F, &H);

    /* Seek to the start of the options */
    FileSeek (F, Offset + H.SegOffs);

    /* Output a header */
    printf ("  Segments:\n");

    /* Read the number of segments and print it */
    Count = Read8 (F);
    printf ("    Count:%27u\n", Count);

    /* Read and print all segments */
    for (I = 0; I < Count; ++I) {

	/* Read the data for one segments */
	char*	      Name  = ReadMallocedStr (F);
	unsigned      Len   = strlen (Name);
	unsigned long Size  = Read32 (F);
	unsigned      Align = (1U << Read8 (F));
	unsigned char Type  = Read8 (F);

	/* Get the description for the type */
	const char* TypeDesc;
	switch (Type) {
	    case SEGTYPE_DEFAULT:	TypeDesc = "SEGTYPE_DEFAULT";	break;
	    case SEGTYPE_ABS:  		TypeDesc = "SEGTYPE_ABS";	break;
	    case SEGTYPE_ZP:   		TypeDesc = "SEGTYPE_ZP";	break;
	    case SEGTYPE_FAR:  		TypeDesc = "SEGTYPE_FAR";	break;
	    default:	       	   	TypeDesc = "SEGTYPE_UNKNOWN";	break;
	}

	/* Print the header */
	printf ("    Index:%27u\n", I);

	/* Print the data */
	printf ("      Name:%*s\"%s\"\n", 24-Len, "", Name);
       	printf ("      Size:%26lu\n", Size);
	printf ("      Alignment:%21u\n", Align);
	printf ("      Type:%22s0x%02X  (%s)\n", "", Type, TypeDesc);

	/* Free the Name */
	xfree (Name);

	/* Skip the fragments for this segment, counting them */
	FragCount = 0;
	while (Size > 0) {
	    unsigned FragSize = SkipFragment (F);
	    if (FragSize > Size) {
	    	/* OOPS - file data invalid */
	    	Error ("Invalid fragment data - file corrupt!");
	    }
	    Size -= FragSize;
	    ++FragCount;
	}

	/* Print the fragment count */
       	printf ("      Fragment count:%16u\n", FragCount);
    }
}



void DumpObjImports (FILE* F, unsigned long Offset)
/* Dump the imports in the object file */
{
    ObjHeader H;
    unsigned  Count;
    unsigned  I;
    FilePos   Pos;

    /* Seek to the header position */
    FileSeek (F, Offset);

    /* Read the header */
    ReadObjHeader (F, &H);

    /* Seek to the start of the options */
    FileSeek (F, Offset + H.ImportOffs);

    /* Output a header */
    printf ("  Imports:\n");

    /* Read the number of imports and print it */
    Count = Read16 (F);
    printf ("    Count:%27u\n", Count);

    /* Read and print all imports */
    for (I = 0; I < Count; ++I) {

	const char* TypeDesc;

       	/* Read the data for one import */
       	unsigned char Type  = Read8 (F);
	char* 	      Name  = ReadMallocedStr (F);
	unsigned      Len   = strlen (Name);
	ReadFilePos (F, &Pos);

	/* Get a description for the type */
	switch (Type) {
	    case IMP_ZP:	TypeDesc = "IMP_ZP";		break;
	    case IMP_ABS:	TypeDesc = "IMP_ABS";		break;
	    default:		TypeDesc = "IMP_UNKNOWN";	break;
	}

	/* Print the header */
	printf ("    Index:%27u\n", I);

	/* Print the data */
       	printf ("      Type:%22s0x%02X  (%s)\n", "", Type, TypeDesc);
	printf ("      Name:%*s\"%s\"\n", 24-Len, "", Name);

	/* Free the Name */
	xfree (Name);
    }
}



void DumpObjExports (FILE* F, unsigned long Offset)
/* Dump the exports in the object file */
{
    ObjHeader H;
    unsigned  Count;
    unsigned  I;
    FilePos   Pos;

    /* Seek to the header position */
    FileSeek (F, Offset);

    /* Read the header */
    ReadObjHeader (F, &H);

    /* Seek to the start of the options */
    FileSeek (F, Offset + H.ExportOffs);

    /* Output a header */
    printf ("  Exports:\n");

    /* Read the number of exports and print it */
    Count = Read16 (F);
    printf ("    Count:%27u\n", Count);

    /* Read and print all exports */
    for (I = 0; I < Count; ++I) {

 	const char* TypeDesc;

       	/* Read the data for one export */
       	unsigned char Type  = Read8 (F);
	char* 	      Name  = ReadMallocedStr (F);
	unsigned      Len   = strlen (Name);
	if (Type & EXP_EXPR) {
	    SkipExpr (F);
	} else {
	    (void) Read32 (F);
	}
	ReadFilePos (F, &Pos);

	/* Get a description for the type */
	switch (Type) {
	    case EXP_ABS|EXP_CONST:	TypeDesc = "EXP_ABS,EXP_CONST";	break;
	    case EXP_ZP|EXP_CONST:	TypeDesc = "EXP_ZP,EXP_CONST";	break;
	    case EXP_ABS|EXP_EXPR:	TypeDesc = "EXP_ABS,EXP_EXPR";	break;
       	    case EXP_ZP|EXP_EXPR:	TypeDesc = "EXP_ZP,EXP_EXPR";	break;
	    default:			TypeDesc = "EXP_UNKNOWN";	break;
	}

	/* Print the header */
	printf ("    Index:%27u\n", I);

	/* Print the data */
       	printf ("      Type:%22s0x%02X  (%s)\n", "", Type, TypeDesc);
	printf ("      Name:%*s\"%s\"\n", 24-Len, "", Name);

	/* Free the Name */
	xfree (Name);
    }
}



