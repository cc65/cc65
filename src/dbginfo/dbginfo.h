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

/* Line information */
typedef struct cc65_lineinfo cc65_lineinfo;
struct cc65_lineinfo {
    unsigned            count;          /* Count of data sets that follow */
    struct {
        const char*     name;           /* Name of the file */
        unsigned long   size;           /* Size of file */
        unsigned long   mtime;          /* Modification time */
        cc65_line       line;           /* Line number */
        cc65_addr       start;          /* Start address for this line */
        cc65_addr       end;            /* End address for this line */
    }                   data[1];
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

cc65_lineinfo* cc65_get_lineinfo (cc65_dbginfo handle, unsigned long addr);
/* Return line information for the given address. The function returns NULL
 * if no line information was found.
 */

void cc65_free_lineinfo (cc65_dbginfo handle, cc65_lineinfo* info);
/* Free line info returned by cc65_get_lineinfo() */



/* End of dbginfo.h */

#endif




