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



/* common */
#include "objdefs.h"

/* od65 */
#include "fileio.h"
#include "dump.h"



/*****************************************************************************/
/*    				     Code				     */
/*****************************************************************************/



static void DumpHeaderSection (const char* Name,
			       unsigned long Offset,
			       unsigned long Size)
/* Dump a header section */
{
    printf ("    %s:\n", Name);
    printf ("      Offset:           %8lu\n", Offset);
    printf ("      Size:             %8lu\n", Size);
}



void DumpHeader (FILE* F, unsigned long Offset)
/* Dump the header of the given object file */
{
    ObjHeader H;

    /* Output a header */
    printf ("  Header:\n");

    /* Seek to the header position */
    fseek (F, 0, SEEK_SET);

    /* Read the header */
    ReadObjHeader (F, &H);

    /* Now dump the information */

    /* Magic */
    printf ("    Magic:            0x%08lX\n", H.Magic);

    /* Version */
    printf ("    Version:          %10u\n", H.Version);

    /* Flags */
    printf ("    Flags:                0x%04X (", H.Flags);
    if (H.Flags & OBJ_FLAGS_DBGINFO) {
	printf ("OBJ_FLAGS_DBGINFO");
    }
    printf (")\n");

    /* Options */
    DumpHeaderSection ("Options", H.OptionOffs, H.OptionSize);

    /* Files */
    DumpHeaderSection ("Files", H.FileOffs, H.FileSize);

    /* Segments */
    DumpHeaderSection ("Segments", H.SegOffs, H.SegSize);

    /* Imports */
    DumpHeaderSection ("Imports", H.ImportOffs, H.ImportSize);

    /* Exports */
    DumpHeaderSection ("Exports", H.ExportOffs, H.ExportSize);

    /* Debug symbols */
    DumpHeaderSection ("Debug symbols", H.DbgSymOffs, H.DbgSymSize);
}



