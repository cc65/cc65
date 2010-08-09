/*****************************************************************************/
/*                                                                           */
/*                                 dbgtest.c                                 */
/*                                                                           */
/*                   Test file for the cc65 dbginfo module                   */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2010,      Ullrich von Bassewitz                                      */
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
#include "dbginfo.h"



static void ErrorFunc (const struct cc65_parseerror* E)
/* Handle errors or warnings that occur while parsing a debug file */
{
    fprintf (stderr,
             "%s:%s(%lu): %s\n",
             E->type? "Error" : "Warning",
             E->name,
             (unsigned long) E->line,
             E->errormsg);
}



static void Usage (void)
/* Print usage information and exit */
{
    printf ("Usage: dbgtest debug-file\n");
    exit (1);
}



int main (int argc, char** argv)
{
    const char*         Input;
    cc65_dbginfo        Info;
    cc65_sourceinfo*    Sources;
    cc65_segmentinfo*   Segments;
    cc65_lineinfo*      Lines;
    cc65_symbolinfo*    Symbols;
    unsigned            I;
    unsigned long       Addr;


    /* Input file is argument */
    if (argc != 2) {
        Usage ();
    }
    Input = argv[1];

    /* Read the file */
    Info = cc65_read_dbginfo (Input, ErrorFunc);
    if (Info == 0) {
        fprintf (stderr, "Error reading input file - aborting\n");
        return 1;
    }
    printf ("Input file \"%s\" successfully read\n", Input);

    /* Output a list of files */
    printf ("List of source files:\n");
    Sources = cc65_get_sourcelist (Info);
    for (I = 0; I < Sources->count; ++I) {
        printf ("  %s\n", Sources->data[I].source_name);
    }
    cc65_free_sourceinfo (Info, Sources);

    /* Output a list of segments */
    printf ("Segments processed when linking:\n");
    Segments = cc65_get_segmentlist (Info);
    for (I = 0; I < Segments->count; ++I) {
        printf ("  %-20s $%06lX $%04lX",
                Segments->data[I].segment_name,
                (unsigned long) Segments->data[I].segment_start,
                (unsigned long) Segments->data[I].segment_size);
        if (Segments->data[I].output_name) {
            printf ("  %-20s $%06lX",
                    Segments->data[I].output_name,
                    Segments->data[I].output_offs);
        }
        putchar ('\n');
    }
    cc65_free_segmentinfo (Info, Segments);

    /* Check one line */
    printf ("Requesting line info for crt0.s(59):\n");
    Lines = cc65_lineinfo_byname (Info, "crt0.s", 59);
    if (Lines == 0) {
        printf ("  Not found\n");
    } else {
        printf ("  Code range is $%04X-$%04X\n",
                Lines->data[0].line_start,
                Lines->data[0].line_end);
        cc65_free_lineinfo (Info, Lines);
    }

    /* Output debug information for all addresses in the complete 6502 address
     * space. This is also sort of a benchmark for the search algorithms.
     */
    printf ("Line info:\n");
    for (Addr = 0; Addr < 0x10000; ++Addr) {
        Lines = cc65_lineinfo_byaddr (Info, Addr);
        if (Lines) {
            unsigned I;
            printf ("  $%04lX: ", Addr);
            for (I = 0; I < Lines->count; ++I) {
                if (I > 0) {
                    printf (", ");
                }
                printf ("%s(%lu)",
                        Lines->data[I].source_name,
                        (unsigned long) Lines->data[I].source_line);
                if (Lines->data[I].output_name) {
                    printf ("  %s($%06lX)",
                            Lines->data[I].output_name,
                            Lines->data[I].output_offs);

                }
            }
            printf ("\n");
            cc65_free_lineinfo (Info, Lines);
        }
    }

    /* Check for address of main */
    printf ("Requesting address of _main:\n");
    Symbols = cc65_symbol_byname (Info, "_main");
    if (Symbols == 0) {
        printf ("  Not found\n");
    } else {                             
        unsigned I;
        for (I = 0; I < Symbols->count; ++I) {
            printf ("  %-20s = %04lX\n", 
                    Symbols->data[I].symbol_name,
                    Symbols->data[I].symbol_value);
        }
        cc65_free_symbolinfo (Info, Symbols);
    }


    /* Free the debug info */
    cc65_free_dbginfo (Info);

    return 0;
}



