/*****************************************************************************/
/*                                                                           */
/*				    main.c				     */
/*                                                                           */
/*             Main program of the chrcvt vector font converter              */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000-2009, Ullrich von Bassewitz                                      */
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
#include "print.h"
#include "xmalloc.h"
#include "version.h"

/* chrcvt */
#include "error.h"



/*
 * The following is a corrected doc from the BGI font editor toolkit:
 *
 *                      BGI Stroke File Format
 *
 * The structure of Borland .CHR (stroke) files is as follows:
 *
 * ;  offset 0h is a Borland header:
 * ;
 *         HeaderSize      equ     080h
 *         DataSize        equ     (size of font file)
 *         descr           equ     "Triplex font"
 *         fname           equ     "TRIP"
 *         MajorVersion    equ     1
 *         MinorVersion    equ     0
 *
 *         db      'PK',8,8
 *         db      'BGI ',descr,'  V'
 *         db      MajorVersion+'0'
 *         db      (MinorVersion / 10)+'0',(MinorVersion mod 10)+'0'
 *         db      ' - 19 October 1987',0DH,0AH
 *         db      'Copyright (c) 1987 Borland International', 0dh,0ah
 *         db      0,1ah                           ; null & ctrl-Z = end
 *
 *         dw      HeaderSize                      ; size of header
 *         db      fname                           ; font name
 *         dw      DataSize                        ; font file size
 *         db      MajorVersion,MinorVersion       ; version #'s
 *         db      1,0                             ; minimal version #'s
 *
 *         db      (HeaderSize - $) DUP (0)        ; pad out to header size
 *
 * At offset 80h starts data for the file:
 *
 * ;               80h     '+'  flags stroke file type
 * ;               81h-82h  number chars in font file (n)
 * ;               83h      undefined
 * ;               84h      ASCII value of first char in file
 * ;               85h-86h  offset to stroke definitions (8+3n)
 * ;               87h      scan flag (normally 0)
 * ;               88h      distance from origin to top of capital
 * ;               89h      distance from origin to baseline
 * ;               8Ah      distance from origin to bottom descender
 * ;               8Bh-8Fh  undefined
 * ;               90h      offsets to individual character definitions
 * ;               90h+2n   width table (one word per character)
 * ;               90h+3n   start of character definitions
 * ;
 * The individual character definitions consist of a variable number of words
 * describing the operations required to render a character. Each word
 * consists of an (x,y) coordinate pair and a two-bit opcode, encoded as shown
 * here:
 *
 * Byte 1          7   6   5   4   3   2   1   0     bit #
 *                op1  <seven bit signed X coord>
 *
 * Byte 2          7   6   5   4   3   2   1   0     bit #
 *                op2  <seven bit signed Y coord>
 *
 *
 *           Opcodes
 *
 *         op1=0  op2=0  End of character definition.
 *         op1=1  op2=0  Move the pointer to (x,y)
 *         op1=1  op2=1  Draw from current pointer to (x,y)
 */



/* The target file format is designed to be read by a cc65 compiled program
 * more easily. It should not be necessary to load the whole file into a
 * buffer to parse it, or seek within the file. Also using less memory if
 * possible would be fine. Therefore we use the following structure:
 *
 * Header portion:
 *      .byte   $54, $43, $48, $00              ; "TCH" version
 *      .word   <size of following data>
 * Data portion:
 *      .byte   <number of chars in font>       ; Value from $81
 *      .byte   <ascii value of first char>     ; Value from $84
 *      .byte   <top>                           ; Value from $88
 *      .byte   <baseline>                      ; Value from $89
 *      .byte   <bottom>                        ; Negative value from $8A
 *      .word   <char definition offsets>, ...  ; Relative to data portion
 * Character definitions:
 *      .byte   <width>
 *      .word   <converted opcode>, ...         
 *      .byte   $80
 *
 * The opcodes get converted for easier handling: END is marked by bit 7 
 * set in the first byte. The second byte of this opcode is not needed.
 * Bit 7 of the second byte marks a MOVE (bit 7 = 0) or DRAW (bit 7 = 1).
 *
 * The parsing code does not expect the file to contain more than $7D
 * characters (most will contain $5E or similar), therefore the character
 * definition offset will be accessed using the pointer to the data portion
 * plus an offset in an 8 bit index register.
 *
 * Above structure allows a program to read the header portion of the file,
 * validate it, allocate memory for the data portion and read the data portion
 * in one chunk into memory. The character definition offsets will then be
 * converted into pointers by adding the data portion pointer to each.
 */



/*****************************************************************************/
/*     	       	       	       	     Data  			  	     */
/*****************************************************************************/



static unsigned FilesProcessed = 0;



/*****************************************************************************/
/*     	       	     	       	     Code  			  	     */
/*****************************************************************************/



static void Usage (void)
/* Print usage information and exit */
{
    fprintf (stderr,
    	     "Usage: %s [options] file [options] [file]\n"
    	     "Short options:\n"
       	     "  -h\t\t\tHelp (this text)\n"
       	     "  -V\t\t\tPrint the version number and exit\n"
	     "\n"
	     "Long options:\n"
	     "  --help\t\tHelp (this text)\n"
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



static void ConvertFile (const char* Name)
/* Convert one vector font file */
{
    static const unsigned char ChrHeader[] = {
        0x50, 0x4B, 0x08, 0x08, 0x42, 0x47, 0x49, 0x20
    };

    long           Size;
    unsigned char* Buf;
    unsigned char* MsgEnd;

    /* Try to open the file for reading */
    FILE* F = fopen (Name, "rb");
    if (F == 0) {
    	Error ("Cannot open `%s': %s", Name, strerror (errno));
    }

    /* Seek to the end and determine the size */
    fseek (F, 0, SEEK_END);
    Size = ftell (F);

    /* Seek back to the start of the file */
    fseek (F, 0, SEEK_SET);

    /* Check if the size is reasonable */
    if (Size > 32*1024) {
        Error ("File `%s' is too large (max = 32k)", Name);
    } else if (Size < 0x100) {
        Error ("File `%s' is too small to be a vector font file", Name);
    }

    /* Allocate memory for the file */
    Buf = xmalloc ((size_t) Size);

    /* Read the file contents into the buffer */
    if (fread (Buf, 1, (size_t) Size, F) != (size_t) Size) {
        Error ("Error reading from `%s'", Name);
    }

    /* Close the file */
    fclose (F);

    /* Verify the header */
    if (memcmp (Buf, ChrHeader, sizeof (ChrHeader)) != 0) {
        Error ("Invalid format for `%s': invalid header", Name);
    }
    MsgEnd = memchr (Buf + sizeof (ChrHeader), 0x1A, 0x80);
    if (MsgEnd == 0) {
        Error ("Invalid format for `%s': description not found", Name);
    }
    if (MsgEnd[1] != 0x80 || MsgEnd[2] != 0x00) {
        Error ("Invalid format for `%s': wrong header size", Name);
    }

    /* Print the copyright from the header */
    Print (stderr, 1, "%.*s\n", (int) (MsgEnd - Buf - 4), Buf+4);

    /* Convert the buffer into a strokefont structure */


}



int main (int argc, char* argv [])
/* Assembler main program */
{
    /* Program long options */
    static const LongOpt OptTab[] = {
	{ "--help",    		0,	OptHelp			},
       	{ "--verbose", 	       	0,	OptVerbose     	       	},
	{ "--version", 	       	0,	OptVersion		},
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

       	        case 'V':
    		    OptVersion (Arg, 0);
       		    break;

       	       	default:
       	       	    UnknownOption (Arg);
		    break;

     	    }
       	} else {
    	    /* Filename. Dump it. */
	    ConvertFile (Arg);
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



