/*****************************************************************************/
/*                                                                           */
/*                                 dbginfo.h                                 */
/*                                                                           */
/*                         cc65 debug info handling                          */
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



#ifndef DBGINFO_H
#define DBGINFO_H



/*****************************************************************************/
/*     	      	       		     Data    				     */
/*****************************************************************************/



/* Data types used for addresses and line numbers. Change to "unsigned long"
 * if you ever want to run the code on a 16-bit machine.
 */
typedef unsigned cc65_line;             /* Used to store line numbers */
typedef unsigned cc65_addr;             /* Use to store (65xx) addresses */

/* Pointer to an opaque data structure containing information from the debug
 * info file. Actually a handle to the data in the file.
 */
typedef void* cc65_dbginfo;

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
    const char*     source_name;        /* Name of the file */
    unsigned long   source_size;        /* Size of file */
    unsigned long   source_mtime;       /* Modification time */
    cc65_line       source_line;        /* Line number */
    cc65_addr       line_start;         /* Start address for this line */
    cc65_addr       line_end;           /* End address for this line */
    const char*     output_name;        /* Output file */
    unsigned long   output_offs;        /* Offset in output file */
};

typedef struct cc65_lineinfo cc65_lineinfo;
struct cc65_lineinfo {
    unsigned            count;          /* Number of data sets that follow */
    cc65_linedata       data[1];        /* Data sets, number is dynamic */
};

/* Source file information */
typedef struct cc65_sourcedata cc65_sourcedata;
struct cc65_sourcedata {
    const char*     source_name;        /* Name of the file */
    unsigned long   source_size;        /* Size of file */
    unsigned long   source_mtime;       /* Modification time */
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
    const char*     segment_name;       /* Name of the segment */
    cc65_addr       segment_start;      /* Start address of segment */
    cc65_addr       segment_size;       /* Size of segment */
    const char*     output_name;        /* Output file this seg was written to */
    unsigned long   output_offs;        /* Offset of this seg in output file */
};

typedef struct cc65_segmentinfo cc65_segmentinfo;
struct cc65_segmentinfo {
    unsigned            count;          /* Number of data sets that follow */
    cc65_segmentdata    data[1];        /* Data sets, number is dynamic */
};



/*****************************************************************************/
/*     	      	       		     Code  				     */
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

cc65_sourceinfo* cc65_get_sourcelist (cc65_dbginfo handle);
/* Return a list of all source files */

void cc65_free_sourceinfo (cc65_dbginfo handle, cc65_sourceinfo* info);
/* Free a source info record */

cc65_segmentinfo* cc65_get_segmentlist (cc65_dbginfo handle);
/* Return a list of all segments referenced in the debug information */

void cc65_free_segmentinfo (cc65_dbginfo handle, cc65_segmentinfo* info);
/* Free a segment info record */



/* End of dbginfo.h */

#endif




