/*****************************************************************************/
/*                                                                           */
/*                                   o65.h                                   */
/*                                                                           */
/*               Definitions and code for the o65 file format                */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2002-2004 Ullrich von Bassewitz                                       */
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



#include <stdio.h>
#include <string.h>
#include <errno.h>

/* common */
#include "chartype.h"
#include "xmalloc.h"

/* co65 */
#include "error.h"
#include "fileio.h"
#include "o65.h"



/*****************************************************************************/
/*                              struct O65Data                               */
/*****************************************************************************/



static O65Data* NewO65Data (void)
/* Create, initialize and return a new O65Data struct */
{
    /* Allocate memory */
    O65Data* D = xmalloc (sizeof (O65Data));

    /* Initialize the fields as needed */
    D->Options      = AUTO_COLLECTION_INITIALIZER;
    D->Text         = 0;
    D->Data         = 0;
    D->TextReloc    = AUTO_COLLECTION_INITIALIZER;
    D->DataReloc    = AUTO_COLLECTION_INITIALIZER;
    D->Imports      = AUTO_COLLECTION_INITIALIZER;
    D->Exports      = AUTO_COLLECTION_INITIALIZER;

    /* Return the new struct */
    return D;
}



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



static unsigned long ReadO65Size (FILE* F, const O65Header* H)
/* Read a size variable (16 or 32 bit, depending on the mode word in the
** header) from the o65 file.
*/
{
    unsigned long Size = 0;     /* Initialize to avoid warnings */
    switch (H->mode & O65_SIZE_MASK) {
        case O65_SIZE_32BIT:    Size = Read32 (F);      break;
        case O65_SIZE_16BIT:    Size = Read16 (F);      break;
        default:                Internal ("Invalid size field value in o65 header");
    }
    return Size;
}



static void ReadO65Header (FILE* F, O65Header* H)
/* Read an o65 header from the given file. The function will call Error if
** something is wrong.
*/
{
    static const char Magic[3] = {
        O65_MAGIC_0, O65_MAGIC_1, O65_MAGIC_2   /* "o65" */
    };

    /* Read the marker and check it */
    ReadData (F, H->marker, sizeof (H->marker));
    if (H->marker[0] != O65_MARKER_0 || H->marker[1] != O65_MARKER_1) {
        Error ("Not an o65 object file: Invalid marker %02X %02X",
               H->marker[0], H->marker[1]);
    }

    /* Read the magic and check it */
    ReadData (F, H->magic, sizeof (H->magic));
    if (memcmp (H->magic, Magic, sizeof (H->magic)) != 0) {
        Error ("Not an o65 object file: Invalid magic %02X %02X %02X",
               H->magic[0], H->magic[1], H->magic[2]);
    }

    /* Read the version number and check it */
    H->version = Read8 (F);
    if (H->version != O65_VERSION) {
        Error ("Invalid o65 version number: %02X", H->version);
    }

    /* Read the mode word */
    H->mode = Read16 (F);

    /* Read the remainder of the header */
    H->tbase = ReadO65Size (F, H);
    H->tlen  = ReadO65Size (F, H);
    H->dbase = ReadO65Size (F, H);
    H->dlen  = ReadO65Size (F, H);
    H->bbase = ReadO65Size (F, H);
    H->blen  = ReadO65Size (F, H);
    H->zbase = ReadO65Size (F, H);
    H->zlen  = ReadO65Size (F, H);
    H->stack = ReadO65Size (F, H);
}



static O65Option* ReadO65Option (FILE* F)
/* Read the next O65 option from the given file. The option is stored into a
** dynamically allocated O65Option struct which is returned. On end of options,
** NULL is returned. On error, Error is called which terminates the program.
*/
{
    O65Option* O;

    /* Read the length of the option and bail out on end of options */
    unsigned char Len = Read8 (F);
    if (Len == 0) {
        return 0;
    }
    if (Len < 2) {
        Error ("Found option with length < 2 (input file corrupt)");
    }
    Len -= 2;

    /* Allocate a new O65Option structure of the needed size */
    O = xmalloc (sizeof (*O) - sizeof (O->Data) + Len);

    /* Assign the length and read the remaining option data */
    O->Len  = Len;
    O->Type = Read8 (F);
    ReadData (F, O->Data, Len);

    /* Return the new struct */
    return O;
}



static O65Import* ReadO65Import (FILE* F)
/* Read an o65 import from the file */
{
    O65Import* I;

    /* Allow identifiers up to 511 bytes */
    char Buf[512];

    /* Read the identifier */
    unsigned Len = 0;
    char C;
    do {
        C = Read8 (F);
        if (Len >= sizeof (Buf)) {
            Error ("Imported identifier exceeds maximum size (%u)",
                   (unsigned) sizeof (Buf));
        }
        Buf[Len++] = C;
    } while (C != '\0');

    /* Allocate an import structure and initialize it */
    I = xmalloc (sizeof (*I) - sizeof (I->Name) + Len);
    memcpy (I->Name, Buf, Len);

    /* Return the new struct */
    return I;
}



static void ReadO65RelocInfo (FILE* F, const O65Data* D, Collection* Reloc)
/* Read relocation data for one segment */
{
    /* Relocation starts at (start address - 1) */
    unsigned long Offs = (unsigned long) -1L;

    while (1) {

        O65Reloc* R;

        /* Read the next relocation offset */
        unsigned char C = Read8 (F);
        if (C == 0) {
            /* End of relocation table */
            break;
        }

        /* Create a new relocation entry */
        R = xmalloc (sizeof (*R));

        /* Handle overflow bytes */
        while (C == 0xFF) {
            Offs += 0xFE;
            C = Read8 (F);
        }

        /* Calculate the final offset */
        R->Offs = (Offs += C);

        /* Read typebyte and segment id */
        C = Read8 (F);
        R->Type   = (C & O65_RTYPE_MASK);
        R->SegID  = (C & O65_SEGID_MASK);

        /* Read an additional relocation value if there is one */
        R->SymIdx = (R->SegID == O65_SEGID_UNDEF)? ReadO65Size (F, &D->Header) : 0;
        switch (R->Type) {

            case O65_RTYPE_HIGH:
                if ((D->Header.mode & O65_RELOC_MASK) == O65_RELOC_BYTE) {
                    /* Low byte follows */
                    R->Val = Read8 (F);
                } else {
                    /* Low byte is zero */
                    R->Val = 0;
                }
                break;

            case O65_RTYPE_SEG:
                /* Low 16 byte of the segment address follow */
                R->Val = Read16 (F);
                break;

            default:
                R->Val = 0;
                break;
        }

        /* Insert this relocation entry into the collection */
        CollAppend (Reloc, R);
    }
}



static O65Export* ReadO65Export (FILE* F, const O65Header* H)
/* Read an o65 export from the file */
{
    O65Export* E;

    /* Allow identifiers up to 511 bytes */
    char Buf[512];

    /* Read the identifier */
    unsigned Len = 0;
    char C;
    do {
        C = Read8 (F);
        if (Len >= sizeof (Buf)) {
            Error ("Exported identifier exceeds maximum size (%u)",
                   (unsigned) sizeof (Buf));
        }
        Buf[Len++] = C;
    } while (C != '\0');

    /* Allocate an export structure and initialize it */
    E = xmalloc (sizeof (*E) - sizeof (E->Name) + Len);
    memcpy (E->Name, Buf, Len);
    E->SegID = Read8 (F);
    E->Val   = ReadO65Size (F, H);

    /* Return the new struct */
    return E;
}



static O65Data* ReadO65Data (FILE* F)
/* Read a complete o65 file into dynamically allocated memory and return the
** created O65Data struct.
*/
{
    unsigned long Count;
    O65Option* O;

    /* Create the struct we're going to return */
    O65Data* D = NewO65Data ();

    /* Read the header */
    ReadO65Header (F, &D->Header);

    /* Read the options */
    while ((O = ReadO65Option (F)) != 0) {
        CollAppend (&D->Options, O);
    }

    /* Allocate space for the text segment and read it */
    D->Text = xmalloc (D->Header.tlen);
    ReadData (F, D->Text, D->Header.tlen);

    /* Allocate space for the data segment and read it */
    D->Data = xmalloc (D->Header.dlen);
    ReadData (F, D->Data, D->Header.dlen);

    /* Read the undefined references list */
    Count = ReadO65Size (F, &D->Header);
    while (Count--) {
        CollAppend (&D->Imports, ReadO65Import (F));
    }

    /* Read the relocation tables for text and data segment */
    ReadO65RelocInfo (F, D, &D->TextReloc);
    ReadO65RelocInfo (F, D, &D->DataReloc);

    /* Read the exported globals list */
    Count = ReadO65Size (F, &D->Header);
    while (Count--) {
        CollAppend (&D->Exports, ReadO65Export (F, &D->Header));
    }

    /* Return the o65 data read from the file */
    return D;
}



O65Data* ReadO65File (const char* Name)
/* Read a complete o65 file into dynamically allocated memory and return the
** created O65Data struct.
*/
{
    O65Data* D;

    /* Open the o65 input file */
    FILE* F = fopen (Name, "rb");
    if (F == 0) {
        Error ("Cannot open `%s': %s", Name, strerror (errno));
    }

    /* Read the file data */
    D = ReadO65Data (F);

    /* Close the input file. Ignore errors since we were only reading */
    fclose (F);

    /* Return the data read */
    return D;
}



const char* GetO65OSName (unsigned char OS)
/* Return the name of the operating system given by OS */
{
    switch (OS) {
        case O65_OS_OSA65:              return "OS/A65";
        case O65_OS_LUNIX:              return "Lunix";
        case O65_OS_CC65_MODULE:        return "cc65 module";
        default:                        return "unknown";
    }
}



const char* GetO65OptionText (const O65Option* O)
/* Return the data of the given option as a readable text. The function returns
** a pointer to a static buffer that is reused on the next call, so if in doubt,
** make a copy (and no, the function is not thread safe).
*/
{
    static char Buf[256];
    unsigned I, J;

    /* Get the length of the text */
    unsigned Len = 0;
    while (Len < O->Len && O->Data[Len] != '\0') {
        ++Len;
    }

    /* Copy into the buffer converting non readable characters */
    I = J = 0;
    while (I < sizeof (Buf) - 1 && J < Len) {
        if (!IsControl (O->Data[J])) {
            Buf[I++] = O->Data[J];
        } else {
            Buf[I++] = '\\';
            if (I >= sizeof (Buf) - 4) {
                --I;
                break;
            }
            switch (O->Data[J]) {
                case '\t':      Buf[I++] = 't'; break;
                case '\b':      Buf[I++] = 'b'; break;
                case '\n':      Buf[I++] = 'n'; break;
                case '\r':      Buf[I++] = 'r'; break;
                case '\v':      Buf[I++] = 'v'; break;
                default:
                    sprintf (Buf + I, "x%02X", O->Data[J]);
                    I += 3;
                    break;
            }
        }
        ++J;
    }

    /* Terminate the string and return it */
    Buf[I] = '\0';
    return Buf;
}
