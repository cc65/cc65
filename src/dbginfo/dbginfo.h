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
/*                                   Data                                    */
/*****************************************************************************/



/* Data types used for addresses, sizes and line numbers. Change to "unsigned
** long" if you ever want to run the code on a 16-bit machine.
*/
typedef unsigned cc65_line;             /* Used to store line numbers */
typedef unsigned cc65_addr;             /* Used to store (65xx) addresses */
typedef unsigned cc65_size;             /* Used to store (65xx) sizes */

/* A value that is used to mark invalid ids */
#define CC65_INV_ID     (~0U)



/*****************************************************************************/
/*                             Debug info files                              */
/*****************************************************************************/



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
typedef void (*cc65_errorfunc) (const cc65_parseerror*);

/* Pointer to an opaque data structure containing information from the debug
** info file. Actually a handle to the data in the file.
*/
typedef const void* cc65_dbginfo;



cc65_dbginfo cc65_read_dbginfo (const char* filename, cc65_errorfunc errorfunc);
/* Parse the debug info file with the given name. On success, the function
** will return a pointer to an opaque cc65_dbginfo structure, that must be
** passed to the other functions in this module to retrieve information.
** errorfunc is called in case of warnings and errors. If the file cannot be
** read successfully, NULL is returned.
*/

void cc65_free_dbginfo (cc65_dbginfo Handle);
/* Free debug information read from a file */



/*****************************************************************************/
/*                                 C Symbols                                 */
/*****************************************************************************/



/* C symbol kinds */
typedef enum {
    CC65_CSYM_FUNC,
    CC65_CSYM_VAR
} cc65_csym_kind;

/* C object storage classes */
typedef enum {
    CC65_CSYM_AUTO,             /* Auto = on stack */
    CC65_CSYM_REG,              /* Register = in register bank */
    CC65_CSYM_STATIC,           /* Static = static storage */
    CC65_CSYM_EXTERN            /* Extern = static storage + external visibility */
} cc65_csym_sc;

/* C symbol information */
typedef struct cc65_csymdata cc65_csymdata;
struct cc65_csymdata {
    unsigned            csym_id;        /* The internal c symbol id */
    unsigned char       csym_kind;      /* Kind of c symbol */
    unsigned char       csym_sc;        /* Storage class of c symbol */
    int                 csym_offs;      /* Offset for auto and register */
    unsigned            type_id;        /* Id of the data type */ 
    unsigned            symbol_id;      /* Attached asm symbol if any */
    unsigned            scope_id;       /* Scope of c symbol */
    const char*         csym_name;      /* Name of the symbol */
};

typedef struct cc65_csyminfo cc65_csyminfo;
struct cc65_csyminfo {
    unsigned            count;          /* Number of data sets that follow */
    cc65_csymdata       data[1];        /* Data sets, number is dynamic */
};



const cc65_csyminfo* cc65_get_csymlist (cc65_dbginfo handle);
/* Return a list of all c symbols */

const cc65_csyminfo* cc65_csym_byid (cc65_dbginfo handle, unsigned id);
/* Return information about a c symbol with a specific id. The function
** returns NULL if the id is invalid (no such c symbol) and otherwise a
** cc65_csyminfo structure with one entry that contains the requested
** symbol information.
*/

const cc65_csyminfo* cc65_cfunc_bymodule (cc65_dbginfo handle, unsigned module_id);
/* Return the list of C functions (not symbols!) for a specific module. If
** the module id is invalid, the function will return NULL, otherwise a
** (possibly empty) c symbol list.
*/

const cc65_csyminfo* cc65_cfunc_byname (cc65_dbginfo handle, const char* name);
/* Return a list of all C functions with the given name that have a
** definition.
*/

const cc65_csyminfo* cc65_csym_byscope (cc65_dbginfo handle, unsigned scope_id);
/* Return all C symbols for a scope. The function will return NULL if the
** given id is invalid.
*/

void cc65_free_csyminfo (cc65_dbginfo handle, const cc65_csyminfo* info);
/* Free a c symbol info record */



/*****************************************************************************/
/*                                 Libraries                                 */
/*****************************************************************************/



/* Library information */
typedef struct cc65_librarydata cc65_librarydata;
struct cc65_librarydata {
    unsigned            library_id;     /* The internal library id */
    const char*         library_name;   /* Name of the library */
};

typedef struct cc65_libraryinfo cc65_libraryinfo;
struct cc65_libraryinfo {
    unsigned            count;          /* Number of data sets that follow */
    cc65_librarydata    data[1];        /* Data sets, number is dynamic */
};



const cc65_libraryinfo* cc65_get_librarylist (cc65_dbginfo handle);
/* Return a list of all libraries */

const cc65_libraryinfo* cc65_library_byid (cc65_dbginfo handle, unsigned id);
/* Return information about a library with a specific id. The function
** returns NULL if the id is invalid (no such library) and otherwise a
** cc65_libraryinfo structure with one entry that contains the requested
** library information.
*/

void cc65_free_libraryinfo (cc65_dbginfo handle, const cc65_libraryinfo* info);
/* Free a library info record */



/*****************************************************************************/
/*                                 Line info                                 */
/*****************************************************************************/



/* Type of line */
typedef enum {
    CC65_LINE_ASM,                      /* Assembler source */
    CC65_LINE_EXT,                      /* Externally supplied (= C) */
    CC65_LINE_MACRO,                    /* Macro expansion */
} cc65_line_type;

/* Line information */
typedef struct cc65_linedata cc65_linedata;
struct cc65_linedata {
    unsigned            line_id;        /* Internal id of this record */
    unsigned            source_id;      /* Id of the source file */
    cc65_line           source_line;    /* Line number */
    cc65_line_type      line_type;      /* Type of line */
    unsigned            count;          /* Nesting counter for macros */
};

typedef struct cc65_lineinfo cc65_lineinfo;
struct cc65_lineinfo {
    unsigned            count;          /* Number of data sets that follow */
    cc65_linedata       data[1];        /* Data sets, number is dynamic */
};



const cc65_lineinfo* cc65_line_byid (cc65_dbginfo handle, unsigned id);
/* Return information about a line with a specific id. The function
** returns NULL if the id is invalid (no such line) and otherwise a
** cc65_lineinfo structure with one entry that contains the requested
** module information.
*/

const cc65_lineinfo* cc65_line_bynumber (cc65_dbginfo handle,
                                         unsigned source_id,
                                         cc65_line line);
/* Return line information for a source file/line number combination. The
** function returns NULL if no line information was found.
*/

const cc65_lineinfo* cc65_line_bysource (cc65_dbginfo Handle, unsigned source_id);
/* Return line information for a source file. The function returns NULL if the
** file id is invalid.
*/

const cc65_lineinfo* cc65_line_bysymdef (cc65_dbginfo handle, unsigned symbol_id);
/* Return line information for the definition of a symbol. The function
** returns NULL if the symbol id is invalid, otherwise a list of line infos.
*/

const cc65_lineinfo* cc65_line_bysymref (cc65_dbginfo handle, unsigned symbol_id);
/* Return line information for all references of a symbol. The function
** returns NULL if the symbol id is invalid, otherwise a list of line infos.
*/

const cc65_lineinfo* cc65_line_byspan (cc65_dbginfo handle, unsigned span_id);
/* Return line information for a a span. The function returns NULL if the
** span id is invalid, otherwise a list of line infos.
*/

void cc65_free_lineinfo (cc65_dbginfo handle, const cc65_lineinfo* info);
/* Free line info returned by one of the other functions */



/*****************************************************************************/
/*                                  Modules                                  */
/*****************************************************************************/



/* Module information
** Notes:
**   - scope_id contains CC65_INV_ID if the module was compiled without
**     debug information.
*/
typedef struct cc65_moduledata cc65_moduledata;
struct cc65_moduledata {
    unsigned            module_id;      /* The internal module id */
    const char*         module_name;    /* Name of the module */
    unsigned            source_id;      /* Id of the module main file */
    unsigned            library_id;     /* Id of the library if any */
    unsigned            scope_id;       /* Id of the main scope */
};

typedef struct cc65_moduleinfo cc65_moduleinfo;
struct cc65_moduleinfo {
    unsigned            count;          /* Number of data sets that follow */
    cc65_moduledata     data[1];        /* Data sets, number is dynamic */
};



const cc65_moduleinfo* cc65_get_modulelist (cc65_dbginfo handle);
/* Return a list of all modules */

const cc65_moduleinfo* cc65_module_byid (cc65_dbginfo handle, unsigned id);
/* Return information about a module with a specific id. The function
** returns NULL if the id is invalid (no such module) and otherwise a
** cc65_moduleinfo structure with one entry that contains the requested
** module information.
*/

void cc65_free_moduleinfo (cc65_dbginfo handle, const cc65_moduleinfo* info);
/* Free a module info record */



/*****************************************************************************/
/*                                   Spans                                   */
/*****************************************************************************/



/* Span information */
typedef struct cc65_spandata cc65_spandata;
struct cc65_spandata {
    unsigned            span_id;        /* The internal span id */
    cc65_addr           span_start;     /* Start of the span */
    cc65_addr           span_end;       /* End of the span (inclusive!) */
    unsigned            segment_id;     /* Id of the segment */
    unsigned            type_id;        /* Id of the type of this span */
    unsigned            line_count;     /* Number of lines attached */
    unsigned            scope_count;    /* Number of scopes attached */
};

typedef struct cc65_spaninfo cc65_spaninfo;
struct cc65_spaninfo {
    unsigned            count;          /* Number of data sets that follow */
    cc65_spandata       data[1];        /* Data sets, number is dynamic */
};



const cc65_spaninfo* cc65_get_spanlist (cc65_dbginfo handle);
/* Return a list of all spans. BEWARE: Large! */

const cc65_spaninfo* cc65_span_byid (cc65_dbginfo handle, unsigned id);
/* Return information about a span with a specific id. The function
** returns NULL if the id is invalid (no such span) and otherwise a
** cc65_spaninfo structure with one entry that contains the requested
** span information.
*/

const cc65_spaninfo* cc65_span_byaddr (cc65_dbginfo handle,
                                       unsigned long addr);
/* Return span information for the given address. The function returns NULL
** if no spans were found for this address.
*/

const cc65_spaninfo* cc65_span_byline (cc65_dbginfo handle, unsigned line_id);
/* Return span information for the given source line. The function returns NULL
** if the line id is invalid, otherwise the spans for this line (possibly zero).
*/

const cc65_spaninfo* cc65_span_byscope (cc65_dbginfo handle, unsigned scope_id);
/* Return span information for the given scope. The function returns NULL if
** the scope id is invalid, otherwise the spans for this scope (possibly zero).
*/

void cc65_free_spaninfo (cc65_dbginfo handle, const cc65_spaninfo* info);
/* Free a span info record */



/*****************************************************************************/
/*                               Source files                                */
/*****************************************************************************/



/* Source file information */
typedef struct cc65_sourcedata cc65_sourcedata;
struct cc65_sourcedata {
    unsigned            source_id;      /* The internal file id */
    const char*         source_name;    /* Name of the file */
    unsigned long       source_size;    /* Size of file */
    unsigned long       source_mtime;   /* Modification time */
};

typedef struct cc65_sourceinfo cc65_sourceinfo;
struct cc65_sourceinfo {
    unsigned            count;          /* Number of data sets that follow */
    cc65_sourcedata     data[1];        /* Data sets, number is dynamic */
};



const cc65_sourceinfo* cc65_get_sourcelist (cc65_dbginfo handle);
/* Return a list of all source files */

const cc65_sourceinfo* cc65_source_byid (cc65_dbginfo handle, unsigned id);
/* Return information about a source file with a specific id. The function
** returns NULL if the id is invalid (no such source file) and otherwise a
** cc65_sourceinfo structure with one entry that contains the requested
** source file information.
*/

const cc65_sourceinfo* cc65_source_bymodule (cc65_dbginfo handle,
                                             unsigned module_id);
/* Return information about the source files used to build a module. The
** function returns NULL if the module id is invalid (no such module) and
** otherwise a cc65_sourceinfo structure with one entry per source file.
*/

void cc65_free_sourceinfo (cc65_dbginfo handle, const cc65_sourceinfo* info);
/* Free a source info record */



/*****************************************************************************/
/*                                  Scopes                                   */
/*****************************************************************************/



/* Scope information */
typedef enum {
    CC65_SCOPE_GLOBAL,                  /* Global scope */
    CC65_SCOPE_MODULE,                  /* Module scope */
    CC65_SCOPE_SCOPE,                   /* .PROC/.SCOPE */
    CC65_SCOPE_STRUCT,                  /* .STRUCT */
    CC65_SCOPE_ENUM,                    /* .ENUM */
} cc65_scope_type;

typedef struct cc65_scopedata cc65_scopedata;
struct cc65_scopedata {
    unsigned            scope_id;       /* Id of scope */
    const char*         scope_name;     /* Name of scope */
    cc65_scope_type     scope_type;     /* Type of scope */
    cc65_size           scope_size;     /* Size of scope, 0 if unknown */
    unsigned            parent_id;      /* Id of parent scope */
    unsigned            symbol_id;      /* Id of scope symbol if any */
    unsigned            module_id;      /* Id of the module */
};

typedef struct cc65_scopeinfo cc65_scopeinfo;
struct cc65_scopeinfo {
    unsigned            count;          /* Number of data sets that follow */
    cc65_scopedata      data[1];        /* Data sets, number is dynamic */
};



const cc65_scopeinfo* cc65_get_scopelist (cc65_dbginfo handle);
/* Return a list of all scopes in the debug information */

const cc65_scopeinfo* cc65_scope_byid (cc65_dbginfo handle, unsigned id);
/* Return the scope with a given id. The function returns NULL if no scope
** with this id was found.
*/

const cc65_scopeinfo* cc65_scope_bymodule (cc65_dbginfo handle, unsigned module_id);
/* Return the list of scopes for one module. The function returns NULL if no
** scope with the given id was found.
*/

const cc65_scopeinfo* cc65_scope_byname (cc65_dbginfo handle, const char* name);
/* Return the list of scopes with a given name. Returns NULL if no scope with
** the given name was found, otherwise a non empty scope list.
*/

const cc65_scopeinfo* cc65_scope_byspan (cc65_dbginfo handle, unsigned span_id);
/* Return scope information for a a span. The function returns NULL if the
** span id is invalid, otherwise a list of line scopes.
*/

const cc65_scopeinfo* cc65_childscopes_byid (cc65_dbginfo handle, unsigned id);
/* Return the direct child scopes of a scope with a given id. The function
** returns NULL if no scope with this id was found, otherwise a list of the
** direct childs.
*/

void cc65_free_scopeinfo (cc65_dbginfo Handle, const cc65_scopeinfo* Info);
/* Free a scope info record */



/*****************************************************************************/
/*                                 Segments                                  */
/*****************************************************************************/



/* Segment information.
** Notes:
**   - output_name may be NULL if the data wasn't written to the output file
**     (example: bss segment)
**   - output_offs is invalid if there is no output_name, and may not be of
**     much use in case of a relocatable output file
*/
typedef struct cc65_segmentdata cc65_segmentdata;
struct cc65_segmentdata {
    unsigned            segment_id;     /* The internal segment id */
    const char*         segment_name;   /* Name of the segment */
    cc65_addr           segment_start;  /* Start address of segment */
    cc65_size           segment_size;   /* Size of segment */
    const char*         output_name;    /* Output file this seg was written to */
    unsigned long       output_offs;    /* Offset of this seg in output file */
};

typedef struct cc65_segmentinfo cc65_segmentinfo;
struct cc65_segmentinfo {
    unsigned            count;          /* Number of data sets that follow */
    cc65_segmentdata    data[1];        /* Data sets, number is dynamic */
};



const cc65_segmentinfo* cc65_get_segmentlist (cc65_dbginfo handle);
/* Return a list of all segments referenced in the debug information */

const cc65_segmentinfo* cc65_segment_byid (cc65_dbginfo handle, unsigned id);
/* Return information about a segment with a specific id. The function returns
** NULL if the id is invalid (no such segment) and otherwise a cc65_segmentinfo
** structure with one entry that contains the requested segment information.
*/

const cc65_segmentinfo* cc65_segment_byname (cc65_dbginfo handle,
                                             const char* name);
/* Return information about a segment with a specific name. The function
** returns NULL if no segment with this name exists and otherwise a
** cc65_segmentinfo structure with one entry that contains the requested
** information.
*/

void cc65_free_segmentinfo (cc65_dbginfo handle, const cc65_segmentinfo* info);
/* Free a segment info record */



/*****************************************************************************/
/*                                  Symbols                                  */
/*****************************************************************************/



/* Symbol information */
typedef enum {
    CC65_SYM_EQUATE,
    CC65_SYM_LABEL,                     /* Some sort of address */
    CC65_SYM_IMPORT,                    /* An import */
} cc65_symbol_type;

/* Notes:
**  - If the symbol is segment relative, the segment id gives segment
**    information, otherwise it contains CC65_INV_ID.
**  - If the type is CC65_SYM_IMPORT, export_id may contain the id of the
**    export. This is not the case if the module contaiing the export doesn't
**    have debug information.
**  - For an import, the fields symbol_value and segment_id are taken from
**    the export, if it is available, since imports have no value or segments
**    by itself.
**  - For an import symbol_size doesn't have a meaning.
**  - For normal symbols (not cheap locals) parent_id contains CC65_INV_ID,
**    for cheap locals it contains the symbol id of the parent symbol.
*/
typedef struct cc65_symboldata cc65_symboldata;
struct cc65_symboldata {
    unsigned            symbol_id;      /* Id of symbol */
    const char*         symbol_name;    /* Name of symbol */
    cc65_symbol_type    symbol_type;    /* Type of symbol */
    cc65_size           symbol_size;    /* Size of symbol, 0 if unknown */
    long                symbol_value;   /* Value of symbol */
    unsigned            export_id;      /* For imports: Matching export */
    unsigned            segment_id;     /* Id of segment if any */
    unsigned            scope_id;       /* The scope this symbol is in */
    unsigned            parent_id;      /* Parent symbol for cheap locals */
};

typedef struct cc65_symbolinfo cc65_symbolinfo;
struct cc65_symbolinfo {
    unsigned            count;          /* Number of data sets that follow */
    cc65_symboldata     data[1];        /* Data sets, number is dynamic */
};



const cc65_symbolinfo* cc65_symbol_byid (cc65_dbginfo handle, unsigned id);
/* Return the symbol with a given id. The function returns NULL if no symbol
** with this id was found.
*/

const cc65_symbolinfo* cc65_symbol_byname (cc65_dbginfo handle, const char* name);
/* Return a list of symbols with a given name. The function returns NULL if
** no symbol with this name was found.
*/

const cc65_symbolinfo* cc65_symbol_byscope (cc65_dbginfo handle,
                                            unsigned scope_id);
/* Return a list of symbols in the given scope. This includes cheap local
** symbols, but not symbols in subscopes. The function returns NULL if the
** scope id is invalid (no such scope) and otherwise a - possibly empty -
** symbol list.
*/

const cc65_symbolinfo* cc65_symbol_inrange (cc65_dbginfo handle,
                                            cc65_addr start, cc65_addr end);
/* Return a list of labels in the given range. end is inclusive. The function
** return NULL if no symbols within the given range are found. Non label
** symbols are ignored and not returned.
*/

void cc65_free_symbolinfo (cc65_dbginfo handle, const cc65_symbolinfo* info);
/* Free a symbol info record */



/*****************************************************************************/
/*                                   Types                                   */
/*****************************************************************************/



/* Type information */
typedef enum {
    CC65_TYPE_VOID,
    CC65_TYPE_BYTE,
    CC65_TYPE_WORD,
    CC65_TYPE_DBYTE,
    CC65_TYPE_DWORD,
    CC65_TYPE_PTR,
    CC65_TYPE_FARPTR,
    CC65_TYPE_ARRAY,

    /* The following ones are currently unavailable: */
    CC65_TYPE_UNION,
    CC65_TYPE_STRUCT,
    CC65_TYPE_FUNC,
} cc65_typetoken;


/* A type is a linked list of typedata structures. In case of arrays, the
** structure will contain an element count and the element type. In case of
** pointers, the structure will contain the type of the data, the pointer
** points to (currently, there are only VOID pointers).
** The next pointer points to the next entry in the list. It is NULL if the
** end of the list is reached.
*/
typedef struct cc65_typedata cc65_typedata;
struct cc65_typedata {
    cc65_typedata*                next;         /* Pointer to next entry */
    cc65_typetoken                what;         /* What kind of type is it? */
    cc65_size                     size;         /* The size of the data */

    /* Depending on "what", one of the members of this union follows */
    union {

        /* In case of CC65_TYPE_PTR or CC65_TYPE_FARPTR */
        struct {
            const cc65_typedata*  ind_type;     /* Type the pointer points to */
        } ptr;

        /* In case of CC65_TYPE_ARRAY */
        struct {
            cc65_size             ele_count;    /* Element count */
            const cc65_typedata*  ele_type;     /* Element type */
        } array;

    } data;
};



const cc65_typedata* cc65_type_byid (cc65_dbginfo handle, unsigned id);
/* Return the data for the type with the given id. The function returns NULL
** if no type with this id was found.
*/

void cc65_free_typedata (cc65_dbginfo Handle, const cc65_typedata* data);
/* Free a symbol info record */



/* Allow usage from C++ */
#ifdef __cplusplus
}
#endif



/* End of dbginfo.h */
#endif



