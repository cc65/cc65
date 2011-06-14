/*****************************************************************************/
/*                                                                           */
/*                                 dbginfo.h                                 */
/*                                                                           */
/*                         cc65 debug info handling                          */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2010-2011, Ullrich von Bassewitz                                      */
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



#ifndef DBGINFO_H
#define DBGINFO_H



/* Allow usage from C++ */
#ifdef __cplusplus
extern "C" {
#endif



/*****************************************************************************/
/*     	      	       		     Data    			  	     */
/*****************************************************************************/



/* Data types used for addresses, sizes and line numbers. Change to "unsigned
 * long" if you ever want to run the code on a 16-bit machine.
 */
typedef unsigned cc65_line;             /* Used to store line numbers */
typedef unsigned cc65_addr;             /* Used to store (65xx) addresses */
typedef unsigned cc65_size;             /* Used to store (65xx) sizes */

/* Pointer to an opaque data structure containing information from the debug
 * info file. Actually a handle to the data in the file.
 */
typedef void* cc65_dbginfo;

/* A value that is used to mark invalid ids */
#define CC65_INV_ID     (~0U)

/* Severity for cc65_parseerror */
typedef enum {
    CC65_WARNING,
    CC65_ERROR,
} cc65_error_severity;

/* Warnings/errors in cc65_read_dbginfo are passed via this struct */
typedef struct cc65_parseerror cc65_parseerror;
struct cc65_parseerror {
    cc65_error_severity type;           /* Type of error */
    const char*         name;           /* Name of input file */
    cc65_line           line;           /* Error line */
    unsigned            column;         /* Error column */
    char                errormsg[1];    /* Error message */
};

/* Function that is called in case of parse errors */
typedef void (*cc65_errorfunc) (const struct cc65_parseerror*);

/* Type of line */
typedef enum {
    CC65_LINE_ASM,                      /* Assembler source */
    CC65_LINE_EXT,                      /* Externally supplied (= C) */
    CC65_LINE_MACRO,                    /* Macro expansion */
} cc65_line_type;

/* Line information.
 * Notes:
 *   - line_end is inclusive
 *   - output_name may be NULL if the data wasn't written to the output file
 *     (example: bss segment)
 *   - output_offs is invalid if there is no output_name, and may not be of
 *     much use in case of a relocatable output file
 */
typedef struct cc65_linedata cc65_linedata;
struct cc65_linedata {
    const char*         source_name;    /* Name of the file */
    unsigned long       source_size;    /* Size of file */
    unsigned long       source_mtime;   /* Modification time */
    cc65_line           source_line;    /* Line number */
    cc65_addr           line_start;     /* Start address for this line */
    cc65_addr           line_end;       /* End address for this line */
    const char*         output_name;    /* Output file */
    unsigned long       output_offs;    /* Offset in output file */
    cc65_line_type      line_type;      /* Type of line */
    unsigned            count;          /* Nesting counter for macros */
};

typedef struct cc65_lineinfo cc65_lineinfo;
struct cc65_lineinfo {
    unsigned            count;          /* Number of data sets that follow */
    cc65_linedata       data[1];        /* Data sets, number is dynamic */
};

/* Source file information */
typedef struct cc65_sourcedata cc65_sourcedata;
struct cc65_sourcedata {
    unsigned            id;             /* The internal file id */
    const char*         source_name;    /* Name of the file */
    unsigned long       source_size;    /* Size of file */
    unsigned long       source_mtime;   /* Modification time */
};

typedef struct cc65_sourceinfo cc65_sourceinfo;
struct cc65_sourceinfo {
    unsigned            count;          /* Number of data sets that follow */
    cc65_sourcedata     data[1];        /* Data sets, number is dynamic */
};

/* Segment information.
 * Notes:
 *   - output_name may be NULL if the data wasn't written to the output file
 *     (example: bss segment)
 *   - output_offs is invalid if there is no output_name, and may not be of
 *     much use in case of a relocatable output file
 */
typedef struct cc65_segmentdata cc65_segmentdata;
struct cc65_segmentdata {
    unsigned            id;             /* The internal segment id */
    const char*         segment_name;   /* Name of the segment */
    cc65_addr           segment_start;  /* Start address of segment */
    cc65_addr           segment_size;   /* Size of segment */
    const char*         output_name;    /* Output file this seg was written to */
    unsigned long       output_offs;    /* Offset of this seg in output file */
};

typedef struct cc65_segmentinfo cc65_segmentinfo;
struct cc65_segmentinfo {
    unsigned            count;          /* Number of data sets that follow */
    cc65_segmentdata    data[1];        /* Data sets, number is dynamic */
};

/* Symbol information */
typedef enum {
    CC65_SYM_EQUATE,
    CC65_SYM_LABEL                      /* Some sort of address */
} cc65_symbol_type;

typedef struct cc65_symboldata cc65_symboldata;
struct cc65_symboldata {
    const char*         symbol_name;    /* Name of symbol */
    cc65_symbol_type    symbol_type;    /* Type of symbol */
    cc65_size           symbol_size;    /* Size of symbol, 0 if unknown */
    long                symbol_value;   /* Value of symbol */
    unsigned            symbol_segment; /* If the symbol is segment relative,
                                         * this contains the id of segment,
                                         * otherwise CC65_INV_ID 
                                         */
};

typedef struct cc65_symbolinfo cc65_symbolinfo;
struct cc65_symbolinfo {
    unsigned            count;          /* Number of data sets that follow */
    cc65_symboldata     data[1];        /* Data sets, number is dynamic */
};



/*****************************************************************************/
/*                             Debug info files                              */
/*****************************************************************************/



cc65_dbginfo cc65_read_dbginfo (const char* filename, cc65_errorfunc errorfunc);
/* Parse the debug info file with the given name. On success, the function
 * will return a pointer to an opaque cc65_dbginfo structure, that must be
 * passed to the other functions in this module to retrieve information.
 * errorfunc is called in case of warnings and errors. If the file cannot be
 * read successfully, NULL is returned.
 */

void cc65_free_dbginfo (cc65_dbginfo Handle);
/* Free debug information read from a file */



/*****************************************************************************/
/*                                 Line info                                 */
/*****************************************************************************/



cc65_lineinfo* cc65_lineinfo_byaddr (cc65_dbginfo handle, unsigned long addr);
/* Return line information for the given address. The function returns NULL
 * if no line information was found.
 */

cc65_lineinfo* cc65_lineinfo_byname (cc65_dbginfo handle, const char* filename,
                                     cc65_line line);
/* Return line information for a file/line number combination. The function
 * returns NULL if no line information was found.
 */

void cc65_free_lineinfo (cc65_dbginfo handle, cc65_lineinfo* info);
/* Free line info returned by one of the other functions */



/*****************************************************************************/
/*                               Source files                                */
/*****************************************************************************/



cc65_sourceinfo* cc65_get_sourcelist (cc65_dbginfo handle);
/* Return a list of all source files */

cc65_sourceinfo* cc65_sourceinfo_byid (cc65_dbginfo handle, unsigned id);
/* Return information about a source file with a specific id. The function
 * returns NULL if the id is invalid (no such source file) and otherwise a
 * cc65_sourceinfo structure with one entry that contains the requested
 * source file information.
 */

void cc65_free_sourceinfo (cc65_dbginfo handle, cc65_sourceinfo* info);
/* Free a source info record */



/*****************************************************************************/
/*                                 Segments                                  */
/*****************************************************************************/



cc65_segmentinfo* cc65_get_segmentlist (cc65_dbginfo handle);
/* Return a list of all segments referenced in the debug information */

cc65_segmentinfo* cc65_segmentinfo_byid (cc65_dbginfo handle, unsigned id);
/* Return information about a segment with a specific id. The function returns
 * NULL if the id is invalid (no such segment) and otherwise a cc65_segmentinfo
 * structure with one entry that contains the requested segment information.
 */

void cc65_free_segmentinfo (cc65_dbginfo handle, cc65_segmentinfo* info);
/* Free a segment info record */



/*****************************************************************************/
/*                                  Symbols                                  */
/*****************************************************************************/



cc65_symbolinfo* cc65_symbol_byname (cc65_dbginfo handle, const char* name);
/* Return a list of symbols with a given name. The function returns NULL if
 * no symbol with this name was found.
 */

cc65_symbolinfo* cc65_symbol_inrange (cc65_dbginfo handle,
                                      cc65_addr start, cc65_addr end);
/* Return a list of labels in the given range. end is inclusive. The function
 * return NULL if no symbols within the given range are found. Non label
 * symbols are ignored and not returned.
 */

void cc65_free_symbolinfo (cc65_dbginfo Handle, cc65_symbolinfo* Info);
/* Free a symbol info record */



/* Allow usage from C++ */
#ifdef __cplusplus
}
#endif



/* End of dbginfo.h */
#endif



