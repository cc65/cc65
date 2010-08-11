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



static void PrintSourceData (const cc65_sourcedata* D)
/* Print the data for one source file */
{
    printf ("  %s\n", D->source_name);
}



static void PrintSegmentData (const cc65_segmentdata* D)
/* Print the data for one segment */
{
    printf ("  %-20s $%06lX $%04lX",
            D->segment_name,
            (unsigned long) D->segment_start,
            (unsigned long) D->segment_size);
    if (D->output_name) {
        printf ("  %-20s $%06lX", D->output_name, D->output_offs);
    }
    putchar ('\n');
}



static void PrintLineData (const cc65_linedata* D)
/* Print the data for one source line */
{
    printf ("  %s(%lu)", D->source_name, (unsigned long) D->source_line);
    if (D->output_name) {
        printf (" [%s($%06lX)]", D->output_name, D->output_offs);
    }
    putchar ('\n');
}



static void PrintSymbolData (const cc65_symboldata* D)
/* Print the data for one symbol */
{
    printf ("  %-20s = %04lX\n", D->symbol_name, D->symbol_value);
}



static void PrintSourceInfo (cc65_sourceinfo* Sources)
/* Output the list of source files */
{
    unsigned I;
    if (Sources) {
        for (I = 0; I < Sources->count; ++I) {
            PrintSourceData (Sources->data + I);
        }
    }
}



static void PrintSegmentInfo (cc65_segmentinfo* Segments)
/* Output the list of segments */
{
    unsigned I;
    if (Segments) {
        for (I = 0; I < Segments->count; ++I) {
            PrintSegmentData (Segments->data + I);
        }
    }
}



static void PrintLineInfo (const cc65_lineinfo* Info)
/* Print a list of line infos */
{
    unsigned I;
    if (Info) {
        for (I = 0; I < Info->count; ++I) {
            PrintLineData (Info->data + I);
        }
    }
}



static void PrintSymbolInfo (const cc65_symbolinfo* Symbols)
/* Print a list of symbol infos */
{
    unsigned I;
    if (Symbols) {
        for (I = 0; I < Symbols->count; ++I) {
            PrintSymbolData (Symbols->data + I);
        }
    }
}



int main (int argc, char** argv)
{
    const char*         Input;
    cc65_dbginfo        Info;
    cc65_sourceinfo*    Sources;
    cc65_segmentinfo*   Segments;
    cc65_lineinfo*      Lines;
    cc65_symbolinfo*    Symbols;
    cc65_addr           Addr;


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
    PrintSourceInfo (Sources);
    cc65_free_sourceinfo (Info, Sources);

    /* Output a list of segments */
    printf ("Segments processed when linking:\n");
    Segments = cc65_get_segmentlist (Info);
    PrintSegmentInfo (Segments);
    cc65_free_segmentinfo (Info, Segments);

    /* Check one line */
    printf ("Requesting line info for crt0.s(59):\n");
    Lines = cc65_lineinfo_byname (Info, "crt0.s", 59);
    if (Lines == 0) {
        printf ("  Not found\n");
    } else {
        PrintLineInfo (Lines);
        cc65_free_lineinfo (Info, Lines);
    }

    /* Output debug information for all addresses in the complete 6502 address
     * space. This is also sort of a benchmark for the search algorithms.
     */
    printf ("Line info:\n");
    for (Addr = 0; Addr < 0x10000; ++Addr) {
        Lines = cc65_lineinfo_byaddr (Info, Addr);
        if (Lines) {
            printf ("  $%04lX:\n", (unsigned long) Addr);
            PrintLineInfo (Lines);
            cc65_free_lineinfo (Info, Lines);
        }
    }

    /* Check for address of main */
    printf ("Requesting address of _main:\n");
    Symbols = cc65_symbol_byname (Info, "_main");
    if (Symbols == 0) {
        printf ("  Not found\n");
        Addr = 0x800;
    } else {
        PrintSymbolInfo (Symbols);
        Addr = Symbols->data[0].symbol_value;
        cc65_free_symbolinfo (Info, Symbols);
    }

    /* Print symbols for the next $100 bytes starting from main (or 0x800) */
    printf ("Requesting labels for $%04lX-$%04lX:\n",           
            (unsigned long) Addr, (unsigned long) Addr + 0xFF);
    Symbols = cc65_symbol_inrange (Info, Addr, Addr + 0xFF);
    if (Symbols == 0) {
        printf ("  None found\n");
    } else {
        PrintSymbolInfo (Symbols);
        cc65_free_symbolinfo (Info, Symbols);
    }

    /* Free the debug info */
    cc65_free_dbginfo (Info);

    return 0;
}



