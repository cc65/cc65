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
#include "objdefs.h"
#include "optdefs.h"
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
    printf ("      Offset:                %8lu\n", Offset);
    printf ("      Size:                  %8lu\n", Size);
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
    printf ("    Magic:                 0x%08lX\n", H.Magic);

    /* Version */
    printf ("    Version:               %10u\n", H.Version);

    /* Flags */
    printf ("    Flags:                     0x%04X (", H.Flags);
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
    long     Size;
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
    printf ("    Count:                      %5u\n", Count);

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
	    case OPT_AUTHOR: 	TypeDesc = "OPT_AUTHOR";	break;
	    case OPT_TRANSLATOR:TypeDesc = "OPT_TRANSLATOR";	break;
	    case OPT_COMPILER:	TypeDesc = "OPT_COMPILER";	break;
	    case OPT_OS:     	TypeDesc = "OPT_OS";		break;
	    case OPT_DATETIME:	TypeDesc = "OPT_DATETIME";	break;
	    default:	     	TypeDesc = "OPT_UNKNOWN";	break;
	}

	/* Print the header */
	printf ("    Option %u:\n", I);

	/* Print the data */
	printf ("      Type:                      0x%02X (%s)\n", Type, TypeDesc);
	switch (ArgType) {

	    case OPT_ARGSTR:
	     	ArgStr = ReadMallocedStr (F);
	    	ArgLen = strlen (ArgStr);
	     	printf ("      Data:%*s\"%s\"\n", 24-ArgLen, "", ArgStr);
	     	Size -= 1 + ArgLen + 1;
	     	xfree (ArgStr);
	     	break;

	    case OPT_ARGNUM:
	     	ArgNum = Read32 (F);
	     	printf ("      Data:%26lu", ArgNum);
		if (Type == OPT_DATETIME) {
		    /* Print the time as a string */
		    time_t T = (time_t) ArgNum;
   		    printf (" (%.24s)", asctime (localtime (&T)));
		}
		printf ("\n");
	     	Size -= 1 + 4;
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



