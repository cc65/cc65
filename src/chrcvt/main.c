/*****************************************************************************/
/*                                                                           */
/*                                  main.c                                   */
/*                                                                           */
/*             Main program of the chrcvt vector font converter              */
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



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

/* common */
#include "cmdline.h"
#include "fname.h"
#include "print.h"
#include "strbuf.h"
#include "xmalloc.h"
#include "version.h"

/* chrcvt */
#include "error.h"



/*
** The following is a corrected doc from the BGI font editor toolkit:
**
**                      BGI Stroke File Format
**
** The structure of Borland .CHR (stroke) files is as follows:
**
** ;  offset 0h is a Borland header:
** ;
**         HeaderSize      equ     080h
**         DataSize        equ     (size of font file)
**         descr           equ     "Triplex font"
**         fname           equ     "TRIP"
**         MajorVersion    equ     1
**         MinorVersion    equ     0
**
**         db      'PK',8,8
**         db      'BGI ',descr,'  V'
**         db      MajorVersion+'0'
**         db      (MinorVersion / 10)+'0',(MinorVersion mod 10)+'0'
**         db      ' - 19 October 1987',0DH,0AH
**         db      'Copyright (c) 1987 Borland International', 0dh,0ah
**         db      0,1ah                           ; null & ctrl-Z = end
**
**         dw      HeaderSize                      ; size of header
**         db      fname                           ; font name
**         dw      DataSize                        ; font file size
**         db      MajorVersion,MinorVersion       ; version #'s
**         db      1,0                             ; minimal version #'s
**
**         db      (HeaderSize - $) DUP (0)        ; pad out to header size
**
** At offset 80h starts data for the file:
**
** ;               80h     '+'  flags stroke file type
** ;               81h-82h  number chars in font file (n)
** ;               83h      undefined
** ;               84h      ASCII value of first char in file
** ;               85h-86h  offset to stroke definitions (8+3n)
** ;               87h      scan flag (normally 0)
** ;               88h      distance from origin to top of capital
** ;               89h      distance from origin to baseline
** ;               8Ah      distance from origin to bottom descender
** ;               8Bh-8Fh  undefined
** ;               90h      offsets to individual character definitions
** ;               90h+2n   width table (one word per character)
** ;               90h+3n   start of character definitions
** ;
** The individual character definitions consist of a variable number of words
** describing the operations required to render a character. Each word
** consists of an (x,y) coordinate pair and a two-bit opcode, encoded as shown
** here:
**
** Byte 1          7   6   5   4   3   2   1   0     bit #
**                op1  <seven bit signed X coord>
**
** Byte 2          7   6   5   4   3   2   1   0     bit #
**                op2  <seven bit signed Y coord>
**
**
**           Opcodes
**
**         op1=0  op2=0  End of character definition.
**         op1=1  op2=0  Move the pointer to (x,y)
**         op1=1  op2=1  Draw from current pointer to (x,y)
*/



/* The target file format is designed to be read by a cc65 compiled program
** more easily. It should not be necessary to load the whole file into a
** buffer to parse it, or seek within the file. Also using less memory if
** possible would be fine. Therefore we use the following structure:
**
** Header portion:
**      .byte   $54, $43, $48, $00              ; "TCH" version
**      .word   <size of data portion>
** Data portion:
**      .byte   <top>                           ; Baseline to top
**      .byte   <bottom>                        ; Baseline to bottom
**      .byte   <height>                        ; Maximum char height
**      .byte   <width>, ...                    ; $5F width bytes
**      .word   <char definition offset>, ...   ; $5F char def offsets
** Character definitions:
**      .word   <converted opcode>, ...
**      .byte   $80
**
** The baseline of the character is assume to be at position zero. top and
** bottom are both positive values. The former extends in positive, the other
** in negative direction of the baseline. height contains the sum of top and
** bottom and is stored here just for easier handling.
**
** The opcodes get converted for easier handling: END is marked by bit 7
** set in the first byte. The second byte of this opcode is not needed.
** Bit 7 of the second byte marks a MOVE (bit 7 = 0) or DRAW (bit 7 = 1).
**
** The number of characters is fixed to $20..$7E (space to tilde), so character
** widths and offsets can be stored in fixed size preallocated tables. The
** space for the character definitions is allocated on the heap, it's size
** is stored in the header.
**
** Above structure allows a program to read the header portion of the file,
** validate it, then read the remainder of the file into memory in one chunk.
** The character definition offsets will then be converted into pointers by
** adding the character definition base pointer to each.
*/



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



static unsigned FilesProcessed = 0;



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



static void Usage (void)
/* Print usage information and exit */
{
    fprintf (stderr,
             "Usage: %s [options] file [options] [file]\n"
             "Short options:\n"
             "  -h\t\t\tHelp (this text)\n"
             "  -v\t\t\tBe more verbose\n"
             "  -V\t\t\tPrint the version number and exit\n"
             "\n"
             "Long options:\n"
             "  --help\t\tHelp (this text)\n"
             "  --verbose\t\tBe more verbose\n"
             "  --version\t\tPrint the version number and exit\n",
             ProgName);
}



static void OptHelp (const char* Opt attribute ((unused)),
                     const char* Arg attribute ((unused)))
/* Print usage information and exit */
{
    Usage ();
    exit (EXIT_SUCCESS);
}



static void OptVerbose (const char* Opt attribute ((unused)),
                        const char* Arg attribute ((unused)))
/* Increase verbosity */
{
    ++Verbosity;
}



static void OptVersion (const char* Opt attribute ((unused)),
                        const char* Arg attribute ((unused)))
/* Print the assembler version */
{
    fprintf (stderr,
             "%s V%s - (C) Copyright 2009, Ullrich von Bassewitz\n",
             ProgName, GetVersionAsString ());
}



static void ConvertChar (StrBuf* Data, const unsigned char* Buf, int Remaining)
/* Convert data for one character. Original data is in Buf, converted data
** will be placed in Data.
*/
{
    /* Convert all drawing vectors for this character */
    while (1) {

        unsigned Op;

        /* Check if we have enough data left */
        if (Remaining < 2) {
            Error ("End of file while parsing character definitions");
        }

        /* Get the next op word */
        Op = (Buf[0] + (Buf[1] << 8)) & 0x8080;

        /* Check the opcode */
        switch (Op) {

            case 0x0000:
                /* End */
                if (SB_IsEmpty (Data)) {
                    /* No ops. We need to add an empty one */
                    SB_AppendChar (Data, 0x00);
                    SB_AppendChar (Data, 0x00);
                }
                /* Add an end marker to the last op in the buffer */
                SB_GetBuf (Data)[SB_GetLen (Data) - 2] |= 0x80;
                return;

            case 0x0080:
                /* Move */
                SB_AppendChar (Data, Buf[0] & 0x7F);
                SB_AppendChar (Data, Buf[1] & 0x7F);
                break;

            case 0x8000:
                /* Invalid opcode */
                Error ("Input file contains invalid opcode 0x8000");
                break;

            case 0x8080:
                /* Draw */
                SB_AppendChar (Data, Buf[0] & 0x7F);
                SB_AppendChar (Data, Buf[1] | 0x80);
                break;
        }

        /* Next Op */
        Buf += 2;
        Remaining -= 2;
    }
}



static void ConvertFile (const char* Input, const char* Output)
/* Convert one vector font file */
{
    /* The header of a BGI vector font file */
    static const unsigned char ChrHeader[] = {
        /* According to the Borland docs, the following should work, but it
        ** doesn't. Seems like there are fonts that work, but don't have the
        ** "BGI" string in the header. So we use just the PK\b\b mark as
        ** a header.
        **
        ** 0x50, 0x4B, 0x08, 0x08, 0x42, 0x47, 0x49, 0x20
        */
        0x50, 0x4B, 0x08, 0x08
    };

    /* The header of a TGI vector font file */
    unsigned char TchHeader[] = {
        0x54, 0x43, 0x48, 0x00,         /* "TCH" version */
        0x00, 0x00,                     /* size of char definitions */
        0x00,                           /* Top */
        0x00,                           /* Baseline */
        0x00,                           /* Bottom */
    };

    long           Size;
    unsigned char* Buf;
    unsigned char* MsgEnd;
    unsigned       FirstChar;
    unsigned       CharCount;
    unsigned       LastChar;
    unsigned       Char;
    unsigned       Offs;
    const unsigned char* OffsetBuf;
    const unsigned char* WidthBuf;
    const unsigned char* VectorBuf;
    StrBuf         Offsets  = AUTO_STRBUF_INITIALIZER;
    StrBuf         VectorData = AUTO_STRBUF_INITIALIZER;


    /* Try to open the file for reading */
    FILE* F = fopen (Input, "rb");
    if (F == 0) {
        Error ("Cannot open input file `%s': %s", Input, strerror (errno));
    }

    /* Seek to the end and determine the size */
    fseek (F, 0, SEEK_END);
    Size = ftell (F);

    /* Seek back to the start of the file */
    fseek (F, 0, SEEK_SET);

    /* Check if the size is reasonable */
    if (Size > 32*1024) {
        Error ("Input file `%s' is too large (max = 32k)", Input);
    } else if (Size < 0x100) {
        Error ("Input file `%s' is too small to be a vector font file", Input);
    }

    /* Allocate memory for the file */
    Buf = xmalloc ((size_t) Size);

    /* Read the file contents into the buffer */
    if (fread (Buf, 1, (size_t) Size, F) != (size_t) Size) {
        Error ("Error reading from input file `%s'", Input);
    }

    /* Close the file */
    (void) fclose (F);

    /* Verify the header */
    if (memcmp (Buf, ChrHeader, sizeof (ChrHeader)) != 0) {
        Error ("Invalid format for `%s': invalid header", Input);
    }
    MsgEnd = memchr (Buf + sizeof (ChrHeader), 0x1A, 0x80);
    if (MsgEnd == 0) {
        Error ("Invalid format for `%s': description not found", Input);
    }
    if (MsgEnd[1] != 0x80 || MsgEnd[2] != 0x00) {
        Error ("Invalid format for `%s': wrong header size", Input);
    }

    /* We expect the file to hold chars from 0x20 (space) to 0x7E (tilde) */
    FirstChar = Buf[0x84];
    CharCount = Buf[0x81] + (Buf[0x82] << 8);
    LastChar  = FirstChar + CharCount - 1;
    if (FirstChar > 0x20 || LastChar < 0x7E) {
        Print (stderr, 1, "FirstChar = $%04X, CharCount = %u\n",
               FirstChar, CharCount);
        Error ("File `%s' doesn't contain the chars we need", Input);
    } else if (LastChar >= 0x100) {
        Error ("File `%s' contains too many character definitions", Input);
    }

    /* Print the copyright from the header */
    Print (stderr, 1, "%.*s\n", (int) (MsgEnd - Buf - 4), Buf+4);

    /* Get pointers to the width table, the offset table and the vector data
    ** table. The first two corrected for 0x20 as first entry.
    */
    OffsetBuf = Buf + 0x90 + ((0x20 - FirstChar) * 2);
    WidthBuf  = Buf + 0x90 + (CharCount * 2) + (0x20 - FirstChar);
    VectorBuf = Buf + 0x90 + (CharCount * 3);

    /* Convert the characters */
    for (Char = 0x20; Char <= 0x7E; ++Char, OffsetBuf += 2) {

        int Remaining;

        /* Add the offset to the offset table */
        Offs = SB_GetLen (&VectorData);
        SB_AppendChar (&Offsets, Offs & 0xFF);
        SB_AppendChar (&Offsets, (Offs >> 8) & 0xFF);

        /* Get the offset of the vector data in the BGI data buffer */
        Offs = OffsetBuf[0] + (OffsetBuf[1] << 8);

        /* Calculate the remaining data in the buffer for this character */
        Remaining = Size - (Offs + (VectorBuf - Buf));

        /* Check if the offset is valid */
        if (Remaining <= 0) {
            Error ("Invalid data offset in input file `%s'", Input);
        }

        /* Convert the vector data and place it into the buffer */
        ConvertChar (&VectorData, VectorBuf + Offs, Remaining);
    }

    /* Complete the TCH header */
    Offs = 3 + 0x5F + 2*0x5F + SB_GetLen (&VectorData);
    TchHeader[4] = Offs & 0xFF;
    TchHeader[5] = (Offs >> 8) & 0xFF;
    TchHeader[6] = Buf[0x88];
    TchHeader[7] = (unsigned char) -(signed char)(Buf[0x8A]);
    TchHeader[8] = TchHeader[6] + TchHeader[7];

    /* The baseline must be zero, otherwise we cannot convert */
    if (Buf[0x89] != 0) {
        Error ("Baseline of font in `%s' is not zero", Input);
    }

    /* If the output file is NULL, use the name of the input file with ".tch"
    ** appended.
    */
    if (Output == 0) {
        Output = MakeFilename (Input, ".tch");
    }

    /* Open the output file */
    F = fopen (Output, "wb");
    if (F == 0) {
        Error ("Cannot open output file `%s': %s", Output, strerror (errno));
    }

    /* Write the header to the output file */
    if (fwrite (TchHeader, 1, sizeof (TchHeader), F) != sizeof (TchHeader)) {
        Error ("Error writing to `%s' (disk full?)", Output);
    }

    /* Write the width table to the output file */
    if (fwrite (WidthBuf, 1, 0x5F, F) != 0x5F) {
        Error ("Error writing to `%s' (disk full?)", Output);
    }

    /* Write the offsets to the output file */
    if (fwrite (SB_GetConstBuf (&Offsets), 1, 0x5F * 2, F) != 0x5F * 2) {
        Error ("Error writing to `%s' (disk full?)", Output);
    }

    /* Write the data to the output file */
    Offs = SB_GetLen (&VectorData);
    if (fwrite (SB_GetConstBuf (&VectorData), 1, Offs, F) != Offs) {
        Error ("Error writing to `%s' (disk full?)", Output);
    }

    /* Close the output file */
    if (fclose (F) != 0) {
        Error ("Error closing to `%s': %s", Output, strerror (errno));
    }

    /* Done */
}



int main (int argc, char* argv [])
/* Assembler main program */
{
    /* Program long options */
    static const LongOpt OptTab[] = {
        { "--help",             0,      OptHelp                 },
        { "--verbose",          0,      OptVerbose              },
        { "--version",          0,      OptVersion              },
    };

    unsigned I;

    /* Initialize the cmdline module */
    InitCmdLine (&argc, &argv, "chrcvt");

    /* Check the parameters */
    I = 1;
    while (I < ArgCount) {

        /* Get the argument */
        const char* Arg = ArgVec[I];

        /* Check for an option */
        if (Arg [0] == '-') {
            switch (Arg [1]) {

                case '-':
                    LongOption (&I, OptTab, sizeof(OptTab)/sizeof(OptTab[0]));
                    break;

                case 'h':
                    OptHelp (Arg, 0);
                    break;

                case 'v':
                    OptVerbose (Arg, 0);
                    break;

                case 'V':
                    OptVersion (Arg, 0);
                    break;

                default:
                    UnknownOption (Arg);
                    break;

            }
        } else {
            /* Filename. Dump it. */
            ConvertFile (Arg, 0);
            ++FilesProcessed;
        }

        /* Next argument */
        ++I;
    }

    /* Print a message if we did not process any files */
    if (FilesProcessed == 0) {
        fprintf (stderr, "%s: No input files\n", ProgName);
    }

    /* Success */
    return EXIT_SUCCESS;
}



