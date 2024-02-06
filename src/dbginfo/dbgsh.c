/*****************************************************************************/
/*                                                                           */
/*                                  dbgsh.c                                  */
/*                                                                           */
/*                           debug info test shell                           */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2011,      Ullrich von Bassewitz                                      */
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
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <errno.h>

/* common */
#include "attrib.h"
#include "chartype.h"
#include "cmdline.h"
#include "coll.h"

/* dbginfo */
#include "dbginfo.h"



/*****************************************************************************/
/*                         Command handler forwards                          */
/*****************************************************************************/



static void CmdHelp (Collection* Args attribute ((unused)));
/* Output a help text */

static void CmdLoad (Collection* Args);
/* Load a debug info file */

static void CmdQuit (Collection* Args attribute ((unused)));
/* Terminate the application */

static void CmdShow (Collection* Args);
/* Show items from the debug info file */

static void CmdShowHelp (Collection* Args);
/* Print help for the show command */

static void CmdShowChildScopes (Collection* Args);
/* Show child scopes from the debug info file */

static void CmdShowCSymbol (Collection* Args);
/* Show c symbols from the debug info file */

static void CmdShowFunction (Collection* Args);
/* Show C functions from the debug info file */

static void CmdShowLibrary (Collection* Args);
/* Show libraries from the debug info file */

static void CmdShowLine (Collection* Args);
/* Show lines from the debug info file */

static void CmdShowModule (Collection* Args);
/* Show modules from the debug info file */

static void CmdShowScope (Collection* Args);
/* Show scopes from the debug info file */

static void CmdShowSegment (Collection* Args);
/* Show segments from the debug info file */

static void CmdShowSource (Collection* Args);
/* Show source files from the debug info file */

static void CmdShowSpan (Collection* Args);
/* Show spans from the debug info file */

static void CmdShowSymbol (Collection* Args);
/* Show symbols */

static void CmdShowSymDef (Collection* Args);
/* Show lines from the debug info file */

static void CmdShowSymRef (Collection* Args);
/* Show lines from the debug info file */

static void CmdShowType (Collection* Args);
/* Show types from the debug info file */

static void CmdUnload (Collection* Args attribute ((unused)));
/* Unload a debug info file */



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Terminate flag - end program when set to true */
static int Terminate = 0;

/* The debug file data */
static cc65_dbginfo     Info = 0;

/* Error and warning counters */
static unsigned FileErrors   = 0;
static unsigned FileWarnings = 0;

/* Type of an id */
enum {
    InvalidId,
    CSymbolId,
    LibraryId,
    LineId,
    ModuleId,
    ScopeId,
    SegmentId,
    SourceId,
    SpanId,
    SymbolId,
    TypeId
};

/* Structure that contains a command description */
typedef struct CmdEntry CmdEntry;
struct CmdEntry {
    char        Cmd[12];
    const char* Help;
    int         ArgCount;
    void        (*Func) (Collection*);
};

/* Table with main commands */
static const CmdEntry MainCmds[] = {
    {
        "exit",
        0,
        1,
        CmdQuit
    }, {
        "help",
        "Show available commands",
        1,
        CmdHelp
    }, {
        "load",
        "Load a debug info file",
        2,
        CmdLoad
    }, {
        "quit",
        "Terminate the shell",
        1,
        CmdQuit
    }, {
        "show",
        "Show items from the info file",
        -2,
        CmdShow
    }, {
        "unload",
        "Unload a debug info file",
        1,
        CmdUnload
    },
};

/* Table with show commands */
static const CmdEntry ShowCmds[] = {
    {
        "childscopes",
        "Show child scopes of other scopes.",
        -2,
        CmdShowChildScopes
    }, {
        "csym",
        0,
        -1,
        CmdShowCSymbol
    }, {
        "csymbol",
        "Show c symbols.",
        -1,
        CmdShowCSymbol
    }, {
        "func",
        0,
        -2,
        CmdShowFunction
    }, {
        "function",
        "Show c functions.",
        -2,
        CmdShowFunction
    }, {
        "help",
        "Show available subcommands.",
        1,
        CmdShowHelp
    }, {
        "line",
        "Show line info. May be followed by one or more line ids.",
        -2,
        CmdShowLine
    }, {
        "library",
        "Show libraries. May be followed by one or more library ids.",
        -1,
        CmdShowLibrary
    }, {
        "module",
        "Show modules. May be followed by one or more module ids.",
        -1,
        CmdShowModule
    }, {
        "scope",
        "Show scopes. May be followed by one or more segment ids.",
        -1,
        CmdShowScope
    }, {
        "segment",
        "Show segments. May be followed by one or more segment ids.",
        -1,
        CmdShowSegment
    }, {
        "source",
        "Show sources. May be followed by one or more source file ids.",
        -1,
        CmdShowSource
    }, {
        "span",
        "Show spans. May be followed by one or more span ids.",
        -1,
        CmdShowSpan
    }, {
        "symbol",
        "Show symbols. May be followed by one or more symbol or scope ids.",
        -2,
        CmdShowSymbol
    }, {
        "symdef",
        "Show where a symbol was defined. May be followed by one or more symbol ids.",
        -2,
        CmdShowSymDef
    }, {
        "symref",
        "Show where a symbol was referenced. May be followed by one or more symbol ids.",
        -2,
        CmdShowSymRef
    }, {
        "type",
        "Show type information. May be followed by one or more type ids.",
        -2,
        CmdShowType
    },
};



/*****************************************************************************/
/*                                  Helpers                                  */
/*****************************************************************************/



static void NewLine (void)
/* Output a newline */
{
    putchar ('\n');
}



static void Print (const char* Format, ...) attribute((format(printf,1,2)));
static void Print (const char* Format, ...)
/* Print a piece of output (no linefeed added) */
{
    va_list ap;
    va_start (ap, Format);
    vprintf (Format, ap);
    va_end (ap);
}



static void PrintLine (const char* Format, ...) attribute((format(printf,1,2)));
static void PrintLine (const char* Format, ...)
/* Print one line of output. The linefeed is supplied by the function */
{
    va_list ap;
    va_start (ap, Format);
    vprintf (Format, ap);
    NewLine ();
    va_end (ap);
}



static void PrintSeparator (void)
/* Print a separator line */
{
    PrintLine ("---------------------------------------------------------------------------");
}



static void FileError (const cc65_parseerror* Info)
/* Callback function - is called in case of errors */
{
    /* Output a message */
    PrintLine ("%s:%s(%lu): %s",
               Info->type? "Error" : "Warning",
               Info->name,
               (unsigned long) Info->line,
               Info->errormsg);

    /* Bump the counters */
    switch (Info->type) {
        case CC65_WARNING:      ++FileWarnings;         break;
        default:                ++FileErrors;           break;
    }
}



static const CmdEntry* FindCmd (const char* Cmd, const CmdEntry* Tab, unsigned Count)
/* Search for a command in the given table */
{
    unsigned I;
    for (I = 0; I < Count; ++I, ++Tab) {
        if (strcmp (Cmd, Tab->Cmd) == 0) {
            return Tab;
        }
    }
    return 0;
}



static void ExecCmd (Collection* Args, const CmdEntry* Tab, unsigned Count)
/* Search for the command in slot 0 of the given collection. If found, check
** the argument count, then execute it. If there are problems, output a
** diagnostic.
*/
{
    /* Search for the command, check number of args, then execute it */
    const char* Cmd = CollAt (Args, 0);
    const CmdEntry* E = FindCmd (Cmd, Tab, Count);
    if (E == 0) {
        PrintLine ("No such command: %s", Cmd);
        return;
    }

    /* Check the number of arguments. Zero means that the function will check
    ** itself. A negative count means that the function needs at least
    ** abs(count) arguments. A positive count means that the function needs
    ** exactly this number of arguments.
    ** Note: The number includes the command itself.
    */
    if (E->ArgCount > 0 && (int)CollCount (Args) != E->ArgCount) {
        /* Argument number mismatch */
        switch (E->ArgCount) {

            case 1:
                PrintLine ("Command doesn't accept an argument");
                return;

            case 2:
                PrintLine ("Command requires an argument");
                return;

            default:
                PrintLine ("Command requires %d arguments", E->ArgCount-1);
                return;
        }
    } else if (E->ArgCount < 0 && (int)CollCount (Args) < -E->ArgCount) {
        /* Argument number mismatch */
        switch (E->ArgCount) {

            case -2:
                PrintLine ("Command requires at least one argument");
                return;

            default:
                PrintLine ("Command requires at least %d arguments", E->ArgCount-1);
                return;
        }
    } else {
        /* Remove the command from the argument list, then execute it */
        CollDelete (Args, 0);
        E->Func (Args);
    }
}



static void PrintHelp (const CmdEntry* Tab, unsigned Count)
/* Output help for one command table */
{
    while (Count--) {
        /* Ignore the commands without help text */
        if (Tab->Help) {
            PrintLine ("%-*s%s", (int) sizeof (Tab->Cmd) + 2, Tab->Cmd, Tab->Help);
        }
        ++Tab;
    }
}



static unsigned FindIdType (const char* TypeName)
/* Find an id type by its name. Returns the type or InvalidId. */
{
    static const struct {
        char            Name[8];
        unsigned        Type;
    } TypeTab[] = {
        {   "l",        LineId          },
        {   "lib",      LibraryId       },
        {   "library",  LibraryId       },
        {   "line",     LineId          },
        {   "m",        ModuleId        },
        {   "mod",      ModuleId        },
        {   "module",   ModuleId        },
        {   "s",        SymbolId        },
        {   "sc",       ScopeId         },
        {   "scope",    ScopeId         },
        {   "seg",      SegmentId       },
        {   "segment",  SegmentId       },
        {   "source",   SourceId        },
        {   "src",      SourceId        },
        {   "sp",       SpanId          },
        {   "span",     SpanId          },
        {   "sym",      SymbolId        },
        {   "symbol",   SymbolId        },
        {   "t",        TypeId          },
        {   "type",     TypeId          },
    };

    unsigned I;
    for (I = 0; I < sizeof(TypeTab) / sizeof(TypeTab[0]); ++I) {
        if (strcmp (TypeName, TypeTab[I].Name) == 0) {
            return TypeTab[I].Type;
        }
    }
    return InvalidId;
}



static int GetId (const char* S, unsigned* Id, unsigned* IdType)
/* Parse a string for an id. If a valid id is found, it is placed in Id and
** the function returns true. If an optional type is found, it is placed in
** IdType, otherwise IdType is left unchanged. If no id is found, the
** function returns false.
*/
{
    char TypeBuf[20];
    char C;
    if (sscanf (S, "%u%c", Id, &C) == 1) {
        /* Just an id found, return it */
        return 1;
    }
    if (sscanf (S, "%19[a-z]:%u%c", TypeBuf, Id, &C) == 2) {
        *IdType = FindIdType (TypeBuf);
        return (*IdType != InvalidId);
    }

    /* Not a valid id */
    return 0;
}



/*****************************************************************************/
/*                      Output functions for item lists                      */
/*****************************************************************************/



static void PrintAddr (cc65_addr Addr, unsigned FieldWidth)
/* Output an address */
{
    Print ("$%06lX", (unsigned long) Addr);
    if (FieldWidth > 7) {
        Print ("%*s", FieldWidth - 7, "");
    }
}



static void PrintNumber (long Num, unsigned Width, unsigned FieldWidth)
/* Output a number */
{
    Print ("%*ld", Width, Num);
    if (FieldWidth > Width) {
        Print ("%*s", FieldWidth - Width, "");
    }
}



static void PrintId (unsigned Id, unsigned FieldWidth)
/* Output an id field */
{
    if (Id == CC65_INV_ID) {
        Print ("   -");
    } else {
        Print ("%4u", Id);
    }
    if (FieldWidth > 4) {
        Print ("%*s", FieldWidth - 4, "");
    }
}



static void PrintSize (cc65_size Size, unsigned FieldWidth)
/* Output a size */
{
    Print ("$%04lX", (unsigned long) Size);
    if (FieldWidth > 5) {
        Print ("%*s", FieldWidth - 5, "");
    }
}



static void PrintTime (time_t T, unsigned FieldWidth)
/* Output a time stamp of some sort */
{
    /* Convert to string */
    char Buf[100];
    unsigned Len = strftime (Buf, sizeof (Buf), "%Y-%m-%d %H:%M:%S", localtime (&T));

    /* Output */
    Print ("%s", Buf);
    if (FieldWidth > Len) {
        Print ("%*s", FieldWidth - Len, "");
    }
}



static void PrintCSymbolHeader (void)
/* Output a header for a list of C symbols */
{
    /* Header */
    PrintLine ("  id  name                        type  kind   sc   offs  symbol scope");
    PrintSeparator ();
}



static void PrintCSymbols (const cc65_csyminfo* S)
/* Output a list of C symbols */
{
    unsigned I;
    const cc65_csymdata* D;

    /* Segments */
    for (I = 0, D = S->data; I < S->count; ++I, ++D) {
        PrintId (D->csym_id, 6);
        Print ("%-28s", D->csym_name);
        PrintId (D->type_id, 6);
        PrintNumber (D->csym_kind, 4, 6);
        PrintNumber (D->csym_sc, 4, 6);
        PrintNumber (D->csym_offs, 4, 8);
        PrintId (D->symbol_id, 6);
        PrintId (D->scope_id, 0);
        NewLine ();
    }
}



static void PrintLibraryHeader (void)
/* Output the header for a library list */
{
    PrintLine ("  id    name");
    PrintSeparator ();
}



static void PrintLibraries (const cc65_libraryinfo* L)
/* Output a list of libraries */
{
    unsigned I;
    const cc65_librarydata* D;

    /* Libraries */
    for (I = 0, D = L->data; I < L->count; ++I, ++D) {
        PrintId (D->library_id, 8);
        Print ("%-24s", D->library_name);
        NewLine ();
    }
}



static void PrintLineHeader (void)
/* Output a header for a line list */
{
    /* Header */
    PrintLine ("  id    source  line    type  count");
    PrintSeparator ();
}



static void PrintLines (const cc65_lineinfo* L)
/* Output a list of lines */
{
    unsigned I;
    const cc65_linedata* D;

    /* Lines */
    for (I = 0, D = L->data; I < L->count; ++I, ++D) {
        PrintId (D->line_id, 8);
        PrintId (D->source_id, 8);
        PrintNumber (D->source_line, 6, 9);
        PrintNumber (D->line_type, 4, 6);
        PrintNumber (D->count, 3, 0);
        NewLine ();
    }
}



static void PrintModuleHeader (void)
/* Output a header for a module list */
{
    /* Header */
    PrintLine ("  id    name                    source  library  scope");
    PrintSeparator ();
}



static void PrintModules (const cc65_moduleinfo* M)
/* Output a list of modules */
{
    unsigned I;
    const cc65_moduledata* D;

    /* Modules */
    for (I = 0, D = M->data; I < M->count; ++I, ++D) {
        PrintId (D->module_id, 8);
        Print ("%-24s", D->module_name);
        PrintId (D->source_id, 8);
        PrintId (D->library_id, 9);
        PrintId (D->scope_id, 6);
        NewLine ();
    }
}



static void PrintScopeHeader (void)
/* Output a header for a list of scopes */
{
    /* Header */
    PrintLine ("  id    name                    type    size   parent  symbol  module");
    PrintSeparator ();
}



static void PrintScopes (const cc65_scopeinfo* S)
/* Output a list of scopes */
{
    unsigned I;
    const cc65_scopedata* D;

    /* Segments */
    for (I = 0, D = S->data; I < S->count; ++I, ++D) {
        PrintId (D->scope_id, 8);
        Print ("%-24s", D->scope_name);
        PrintNumber (D->scope_type, 4, 8);     /* ## */
        PrintSize (D->scope_size, 8);
        PrintId (D->parent_id, 8);
        PrintId (D->symbol_id, 8);
        PrintId (D->module_id, 0);
        NewLine ();
    }
}



static void PrintSegmentHeader (void)
/* Output a header for a list of segments */
{
    /* Header */
    PrintLine ("  id    name            address  size   output file     offs");
    PrintSeparator ();
}



static void PrintSegments (const cc65_segmentinfo* S)
/* Output a list of segments */
{
    unsigned I;
    const cc65_segmentdata* D;

    /* Segments */
    for (I = 0, D = S->data; I < S->count; ++I, ++D) {
        PrintId (D->segment_id, 8);
        Print ("%-16s", D->segment_name);
        PrintAddr (D->segment_start, 9);
        PrintSize (D->segment_size, 7);
        Print ("%-16s", D->output_name? D->output_name : "");
        PrintSize (D->output_offs, 6);
        NewLine ();
    }
}



static void PrintSourceHeader (void)
/* Output a header for a list of source files */
{
    /* Header */
    PrintLine ("  id    name                            size   modification time");
    PrintSeparator ();
}



static void PrintSources (const cc65_sourceinfo* S)
/* Output a list of sources */
{
    unsigned I;
    const cc65_sourcedata* D;

    /* Segments */
    for (I = 0, D = S->data; I < S->count; ++I, ++D) {
        PrintId (D->source_id, 8);
        Print ("%-30s", D->source_name);
        PrintNumber (D->source_size, 7, 9);
        PrintTime (D->source_mtime, 0);
        NewLine ();
    }
}



static void PrintSpanHeader (void)
/* Output a header for a list of spans */
{
    /* Header */
    PrintLine ("  id    start    end     seg   type   lines  scopes");
    PrintSeparator ();
}



static void PrintSpans (const cc65_spaninfo* S)
/* Output a list of spans */
{
    unsigned I;
    const cc65_spandata* D;

    /* Segments */
    for (I = 0, D = S->data; I < S->count; ++I, ++D) {
        PrintId (D->span_id, 7);
        PrintAddr (D->span_start, 8);
        PrintAddr (D->span_end, 9);
        PrintId (D->segment_id, 7);
        PrintId (D->type_id, 6);
        PrintNumber (D->line_count, 6, 7);
        PrintNumber (D->scope_count, 7, 0);
        NewLine ();
    }
}



static void PrintSymbolHeader (void)
/* Output a header for a list of symbols */
{
    /* Header */
    PrintLine ("  id  name                    type  size  value  export  seg  scope parent");
    PrintSeparator ();
}



static void PrintSymbols (const cc65_symbolinfo* S)
/* Output a list of symbols */
{
    unsigned I;
    const cc65_symboldata* D;

    /* Segments */
    for (I = 0, D = S->data; I < S->count; ++I, ++D) {
        PrintId (D->symbol_id, 6);
        Print ("%-24s", D->symbol_name);
        PrintNumber (D->symbol_type, 4, 6);
        PrintNumber (D->symbol_size, 4, 6);
        PrintNumber (D->symbol_value, 5, 7);
        PrintId (D->export_id, 7);
        PrintId (D->segment_id, 6);
        PrintId (D->scope_id, 6);
        PrintId (D->parent_id, 0);
        NewLine ();
    }
}



static void PrintTypeHeader (void)
/* Output a header for a list of types */
{
    /* Header */
    PrintLine ("  id  description");
    PrintSeparator ();
}



static void PrintType (unsigned Id, const cc65_typedata* T)
/* Output one type */
{
    /* Output the id */
    PrintId (Id, 6);

    while (1) {
        switch (T->what) {

            case CC65_TYPE_VOID:
                Print ("VOID");
                goto ExitPoint;

            case CC65_TYPE_BYTE:
                Print ("BYTE");
                goto ExitPoint;

            case CC65_TYPE_WORD:
                Print ("WORD");
                goto ExitPoint;

            case CC65_TYPE_DBYTE:
                Print ("DBYTE");
                goto ExitPoint;

            case CC65_TYPE_DWORD:
                Print ("DWORD");
                goto ExitPoint;

            case CC65_TYPE_FARPTR:
                Print ("FAR ");
                /* FALLTHROUGH */

            case CC65_TYPE_PTR:
                Print ("POINTER TO ");
                T = T->data.ptr.ind_type;
                break;

            case CC65_TYPE_ARRAY:
                Print ("ARRAY[%u] OF ", T->data.array.ele_count);
                T = T->data.array.ele_type;
                break;

            default:
                /* Anything else is currently not implemented */
                Print ("***NOT IMPLEMENTED***");
                goto ExitPoint;
        }
    }

ExitPoint:
    NewLine ();
}



/*****************************************************************************/
/*                            Debug file handling                            */
/*****************************************************************************/



static void UnloadFile (void)
/* Unload the debug info file */
{
    if (Info) {
        cc65_free_dbginfo (Info);
        Info = 0;
    }
}



static int FileIsLoaded (void)
/* Return true if the file is open and has loaded without errors: If not,
** print an error message and return false.
*/
{
    /* File open? */
    if (Info == 0) {
        PrintLine ("No debug info file");
        return 0;
    }

    /* Errors on load? */
    if (FileErrors > 0) {
        PrintLine ("File had load errors!");
        return 0;
    }

    /* Warnings on load? */
    if (FileWarnings > 0) {
        PrintLine ("Beware - file had load warnings!");
    }

    /* Ok */
    return 1;
}



/*****************************************************************************/
/*                             Command handlers                              */
/*****************************************************************************/



static void CmdHelp (Collection* Args attribute ((unused)))
/* Output a help text */
{
    PrintHelp (MainCmds, sizeof (MainCmds) / sizeof (MainCmds[0]));
}



static void CmdLoad (Collection* Args)
/* Load a debug info file */
{
    /* Unload a loaded file */
    UnloadFile ();

    /* Clear the counters */
    FileErrors   = 0;
    FileWarnings = 0;

    /* Open the debug info file */
    Info = cc65_read_dbginfo (CollAt (Args, 0), FileError);

    /* Print a status */
    if (FileErrors > 0) {
        PrintLine ("File loaded with %u errors", FileErrors);
    } else if (FileWarnings > 0) {
        PrintLine ("File loaded with %u warnings", FileWarnings);
    } else {
        PrintLine ("File loaded successfully");
    }
}



static void CmdQuit (Collection* Args attribute ((unused)))
/* Terminate the application */
{
    UnloadFile ();
    Terminate = 1;
}



static void CmdShow (Collection* Args)
/* Show items from the debug info file */
{
    /* Search for the subcommand, check number of args, then execute it */
    ExecCmd (Args, ShowCmds, sizeof (ShowCmds) / sizeof (ShowCmds[0]));
}



static void CmdShowHelp (Collection* Args attribute ((unused)))
/* Print help for the show command */
{
    PrintHelp (ShowCmds, sizeof (ShowCmds) / sizeof (ShowCmds[0]));
}



static void CmdShowChildScopes (Collection* Args)
/* Show child scopes from the debug info file */
{
    const cc65_scopeinfo* S;
    unsigned I;

    /* Be sure a file is loaded */
    if (!FileIsLoaded ()) {
        return;
    }

    /* Output the header */
    PrintScopeHeader ();

    /* Output child scopes for all arguments */
    for (I = 0; I < CollCount (Args); ++I) {

        /* Parse the argument */
        unsigned Id;
        unsigned IdType = ScopeId;
        if (GetId (CollConstAt (Args, I), &Id, &IdType)) {
            /* Fetch list depending on type */
            switch (IdType) {
                case ScopeId:
                    S = cc65_childscopes_byid (Info, Id);
                    break;
                default:
                    S = 0;
                    PrintLine ("Invalid id type");
                    break;
            }
        } else {
            /* Invalid id */
            S = 0;
        }

        /* Output the list */
        if (S) {
            PrintScopes (S);
            cc65_free_scopeinfo (Info, S);
        }
    }
}



static void CmdShowCSymbol (Collection* Args)
/* Show C symbols from the debug info file */
{
    const cc65_csyminfo* S;

    /* Be sure a file is loaded */
    if (!FileIsLoaded ()) {
        return;
    }

    /* Output the header */
    PrintCSymbolHeader ();

    /* No arguments means show all libraries */
    if (CollCount (Args) == 0) {

        /* Fetch the list of c symbols */
        S = cc65_get_csymlist (Info);

        /* Output the c symbols */
        PrintCSymbols (S);

        /* Free the list */
        cc65_free_csyminfo (Info, S);

    } else {

        /* Output c symbols for all arguments */
        unsigned I;
        for (I = 0; I < CollCount (Args); ++I) {

            /* Parse the argument */
            unsigned Id;
            unsigned IdType = CSymbolId;
            if (GetId (CollConstAt (Args, I), &Id, &IdType)) {
                /* Fetch list depending on type */
                switch (IdType) {
                    case CSymbolId:
                        S = cc65_csym_byid (Info, Id);
                        break;
                    case ScopeId:
                        S = cc65_csym_byscope (Info, Id);
                        break;
                    default:
                        S = 0;
                        PrintLine ("Invalid id type");
                        break;
                }
            } else {
                /* Invalid id */
                S = 0;
            }

            /* Output the list */
            if (S) {
                PrintCSymbols (S);
                cc65_free_csyminfo (Info, S);
            }
        }
    }
}



static void CmdShowFunction (Collection* Args)
/* Show C functions from the debug info file */
{
    const cc65_csyminfo* S;
    unsigned I;

    /* Be sure a file is loaded */
    if (!FileIsLoaded ()) {
        return;
    }

    /* Output the header */
    PrintCSymbolHeader ();

    /* Output c symbols for all arguments */
    for (I = 0; I < CollCount (Args); ++I) {

        /* Parse the argument */
        unsigned Id;
        unsigned IdType = ModuleId;
        if (GetId (CollConstAt (Args, I), &Id, &IdType)) {
            /* Fetch list depending on type */
            switch (IdType) {
                case ModuleId:
                    S = cc65_cfunc_bymodule (Info, Id);
                    break;
                default:
                    S = 0;
                    PrintLine ("Invalid id type");
                    break;
            }
        } else {
            /* An invalid id may be a function name */
            S = cc65_cfunc_byname (Info, CollConstAt (Args, I));
        }

        /* Output the list */
        if (S) {
            PrintCSymbols (S);
            cc65_free_csyminfo (Info, S);
        }
    }
}



static void CmdShowLine (Collection* Args)
/* Show lines from the debug info file */
{
    const cc65_lineinfo* L;
    unsigned I;

    /* Be sure a file is loaded */
    if (!FileIsLoaded ()) {
        return;
    }

    /* Output the header */
    PrintLineHeader ();

    for (I = 0; I < CollCount (Args); ++I) {

        /* Parse the argument */
        unsigned Id;
        unsigned IdType = LineId;
        if (GetId (CollConstAt (Args, I), &Id, &IdType)) {
            /* Fetch list depending on type */
            switch (IdType) {

                case LineId:
                    L = cc65_line_byid (Info, Id);
                    break;

                case SourceId:
                    L = cc65_line_bysource (Info, Id);
                    break;

                case SymbolId:
                    /* ### not very clean */
                    L = cc65_line_bysymdef (Info, Id);
                    if (L) {
                        PrintLines (L);
                        cc65_free_lineinfo (Info, L);
                    }
                    L = cc65_line_bysymref (Info, Id);
                    break;

                default:
                    L = 0;
                    PrintLine ("Invalid id type");
                    break;
            }
        } else {
            /* Ignore the invalid id */
            L = 0;
        }

        /* Output the list */
        if (L) {
            PrintLines (L);
            cc65_free_lineinfo (Info, L);
        }

    }
}



static void CmdShowLibrary (Collection* Args)
/* Show libraries from the debug info file */
{
    const cc65_libraryinfo* L;

    /* Be sure a file is loaded */
    if (!FileIsLoaded ()) {
        return;
    }

    /* Output the header */
    PrintLibraryHeader ();

    /* No arguments means show all libraries */
    if (CollCount (Args) == 0) {

        /* Fetch the list of libraries */
        L = cc65_get_librarylist (Info);

        /* Output the libraries */
        PrintLibraries (L);

        /* Free the list */
        cc65_free_libraryinfo (Info, L);

    } else {

        unsigned I;
        for (I = 0; I < CollCount (Args); ++I) {

            /* Parse the argument */
            unsigned Id;
            unsigned IdType = LibraryId;
            if (GetId (CollConstAt (Args, I), &Id, &IdType)) {
                /* Fetch list depending on type */
                switch (IdType) {
                    case LibraryId:
                        L = cc65_library_byid (Info, Id);
                        break;
                    default:
                        L = 0;
                        PrintLine ("Invalid id type");
                        break;
                }
            } else {
                /* Ignore the invalid id */
                L = 0;
            }

            /* Output the list */
            if (L) {
                PrintLibraries (L);
                cc65_free_libraryinfo (Info, L);
            }
        }
    }
}



static void CmdShowModule (Collection* Args)
/* Show modules from the debug info file */
{
    const cc65_moduleinfo* M;

    /* Be sure a file is loaded */
    if (!FileIsLoaded ()) {
        return;
    }

    /* Output the header */
    PrintModuleHeader ();

    /* No arguments means show all modules */
    if (CollCount (Args) == 0) {

        /* Fetch the list of modules */
        M = cc65_get_modulelist (Info);

        /* Output the modules */
        PrintModules (M);

        /* Free the list */
        cc65_free_moduleinfo (Info, M);

    } else {

        unsigned I;
        for (I = 0; I < CollCount (Args); ++I) {

            /* Parse the argument */
            unsigned Id;
            unsigned IdType = ModuleId;
            if (GetId (CollConstAt (Args, I), &Id, &IdType)) {
                /* Fetch list depending on type */
                switch (IdType) {
                    case ModuleId:
                        M = cc65_module_byid (Info, Id);
                        break;
                    default:
                        M = 0;
                        PrintLine ("Invalid id type");
                        break;
                }
            } else {
                /* Ignore the invalid id */
                M = 0;
            }

            /* Output the list */
            if (M) {
                PrintModules (M);
                cc65_free_moduleinfo (Info, M);
            }

        }
    }
}



static void CmdShowScope (Collection* Args)
/* Show scopes from the debug info file */
{
    const cc65_scopeinfo* S;

    /* Be sure a file is loaded */
    if (!FileIsLoaded ()) {
        return;
    }

    /* Output the header */
    PrintScopeHeader ();

    /* No arguments means show all modules */
    if (CollCount (Args) == 0) {

        /* Fetch the list of segments */
        S = cc65_get_scopelist (Info);

        /* Output the segments */
        PrintScopes (S);

        /* Free the list */
        cc65_free_scopeinfo (Info, S);

    } else {

        unsigned I;
        for (I = 0; I < CollCount (Args); ++I) {

            /* Parse the argument */
            unsigned Id;
            unsigned IdType = ScopeId;
            if (GetId (CollConstAt (Args, I), &Id, &IdType)) {
                /* Fetch list depending on type */
                switch (IdType) {
                    case ModuleId:
                        S = cc65_scope_bymodule (Info, Id);
                        break;
                    case ScopeId:
                        S = cc65_scope_byid (Info, Id);
                        break;
                    default:
                        S = 0;
                        PrintLine ("Invalid id type");
                        break;
                }
            } else {
                /* An invalid id may be a scope name */
                S = cc65_scope_byname (Info, CollConstAt (Args, I));
            }

            /* Output the list */
            if (S) {
                PrintScopes (S);
                cc65_free_scopeinfo (Info, S);
            }
        }
    }
}



static void CmdShowSegment (Collection* Args)
/* Show segments from the debug info file */
{
    const cc65_segmentinfo* S;

    /* Be sure a file is loaded */
    if (!FileIsLoaded ()) {
        return;
    }

    /* Output the header */
    PrintSegmentHeader ();

    /* No arguments means show all modules */
    if (CollCount (Args) == 0) {

        /* Fetch the list of segments */
        S = cc65_get_segmentlist (Info);

        /* Output the segments */
        PrintSegments (S);

        /* Free the list */
        cc65_free_segmentinfo (Info, S);

    } else {

        unsigned I;
        for (I = 0; I < CollCount (Args); ++I) {

            /* Parse the argument */
            unsigned Id;
            unsigned IdType = SegmentId;
            if (GetId (CollConstAt (Args, I), &Id, &IdType)) {
                /* Fetch list depending on type */
                switch (IdType) {
                    case SegmentId:
                        S = cc65_segment_byid (Info, Id);
                        break;
                    default:
                        S = 0;
                        PrintLine ("Invalid id type");
                        break;
                }
            } else {
                /* An invalid id may be a segment name */
                S = cc65_segment_byname (Info, CollConstAt (Args, I));
            }

            /* Output the list */
            if (S) {
                PrintSegments (S);
                cc65_free_segmentinfo (Info, S);
            }
        }
    }
}



static void CmdShowSource (Collection* Args)
/* Show source files from the debug info file */
{
    const cc65_sourceinfo* S;

    /* Be sure a file is loaded */
    if (!FileIsLoaded ()) {
        return;
    }

    /* Output the header */
    PrintSourceHeader ();

    /* No arguments means show all modules */
    if (CollCount (Args) == 0) {

        /* Fetch the list of source files */
        S = cc65_get_sourcelist (Info);

        /* Output the source files */
        PrintSources (S);

        /* Free the list */
        cc65_free_sourceinfo (Info, S);

    } else {

        unsigned I;
        for (I = 0; I < CollCount (Args); ++I) {

            /* Parse the argument */
            unsigned Id;
            unsigned IdType = SourceId;
            if (GetId (CollConstAt (Args, I), &Id, &IdType)) {
                /* Fetch list depending on type */
                switch (IdType) {
                    case ModuleId:
                        S = cc65_source_bymodule (Info, Id);
                        break;
                    case SourceId:
                        S = cc65_source_byid (Info, Id);
                        break;
                    default:
                        S = 0;
                        PrintLine ("Invalid id type");
                        break;
                }
            } else {
                /* Ignore the invalid id */
                S = 0;
            }

            /* Output the list */
            if (S) {
                PrintSources (S);
                cc65_free_sourceinfo (Info, S);
            }
        }
    }
}



static void CmdShowSpan (Collection* Args)
/* Show spans from the debug info file */
{
    const cc65_spaninfo* S;
    unsigned I;

    /* Be sure a file is loaded */
    if (!FileIsLoaded ()) {
        return;
    }

    /* Output the header */
    PrintSpanHeader ();

    for (I = 0; I < CollCount (Args); ++I) {

        /* Parse the argument */
        unsigned Id;
        unsigned IdType = SpanId;
        if (GetId (CollConstAt (Args, I), &Id, &IdType)) {
            /* Fetch list depending on type */
            switch (IdType) {
                case LineId:
                    S = cc65_span_byline (Info, Id);
                    break;

                case ScopeId:
                    S = cc65_span_byscope (Info, Id);
                    break;
                case SpanId:
                    S = cc65_span_byid (Info, Id);
                    break;
                default:
                    S = 0;
                    PrintLine ("Invalid id type");
                    break;
            }
        } else {
            /* Ignore the invalid id */
            S = 0;
        }

        /* Output the list */
        if (S) {
            PrintSpans (S);
            cc65_free_spaninfo (Info, S);
        }
    }
}



static void CmdShowSymbol (Collection* Args)
/* Show symbols from the debug info file */
{
    const cc65_symbolinfo* S;
    unsigned I;

    /* Be sure a file is loaded */
    if (!FileIsLoaded ()) {
        return;
    }

    /* Output the header */
    PrintSymbolHeader ();

    for (I = 0; I < CollCount (Args); ++I) {

        /* Parse the argument */
        unsigned Id;
        unsigned IdType = SymbolId;
        if (GetId (CollConstAt (Args, I), &Id, &IdType)) {
            /* Fetch list depending on type */
            switch (IdType) {

                case ScopeId:
                    S = cc65_symbol_byscope (Info, Id);
                    break;

                case SymbolId:
                    S = cc65_symbol_byid (Info, Id);
                    break;

                default:
                    S = 0;
                    PrintLine ("Invalid id type");
                    break;
            }
        } else {
            /* Ignore the invalid id */
            S = 0;
        }

        /* Output the list */
        if (S) {
            PrintSymbols (S);
            cc65_free_symbolinfo (Info, S);
        }
    }
}



static void CmdShowSymDef (Collection* Args)
/* Show symbol definitions from the debug info file */
{
    const cc65_lineinfo* L;
    unsigned I;

    /* Be sure a file is loaded */
    if (!FileIsLoaded ()) {
        return;
    }

    /* Output the header */
    PrintLineHeader ();

    for (I = 0; I < CollCount (Args); ++I) {

        /* Parse the argument */
        unsigned Id;
        unsigned IdType = SymbolId;
        if (GetId (CollConstAt (Args, I), &Id, &IdType)) {
            /* Fetch list depending on type */
            switch (IdType) {

                case SymbolId:
                    L = cc65_line_bysymdef (Info, Id);
                    break;

                default:
                    L = 0;
                    PrintLine ("Invalid id type");
                    break;
            }
        } else {
            /* Ignore the invalid id */
            L = 0;
        }

        /* Output the list */
        if (L) {
            PrintLines (L);
            cc65_free_lineinfo (Info, L);
        }
    }
}



static void CmdShowSymRef (Collection* Args)
/* Show symbol references from the debug info file */
{
    const cc65_lineinfo* L;
    unsigned I;

    /* Be sure a file is loaded */
    if (!FileIsLoaded ()) {
        return;
    }

    /* Output the header */
    PrintLineHeader ();

    for (I = 0; I < CollCount (Args); ++I) {

        /* Parse the argument */
        unsigned Id;
        unsigned IdType = SymbolId;
        if (GetId (CollConstAt (Args, I), &Id, &IdType)) {
            /* Fetch list depending on type */
            switch (IdType) {

                case SymbolId:
                    L = cc65_line_bysymref (Info, Id);
                    break;

                default:
                    L = 0;
                    PrintLine ("Invalid id type");
                    break;
            }
        } else {
            /* Ignore the invalid id */
            L = 0;
        }

        /* Output the list */
        if (L) {
            PrintLines (L);
            cc65_free_lineinfo (Info, L);
        }
    }
}



static void CmdShowType (Collection* Args)
/* Show types from the debug info file */
{
    const cc65_typedata* T;
    const cc65_spaninfo* S;
    unsigned I;

    /* Be sure a file is loaded */
    if (!FileIsLoaded ()) {
        return;
    }

    /* Output the header */
    PrintTypeHeader ();

    for (I = 0; I < CollCount (Args); ++I) {

        /* Parse the argument */
        unsigned Id;
        unsigned IdType = TypeId;
        if (GetId (CollConstAt (Args, I), &Id, &IdType)) {
            /* Fetch list depending on type */
            switch (IdType) {

                case SpanId:
                    S = cc65_span_byid (Info, Id);
                    if (S == 0 || S->count == 0) {
                        T = 0;
                        break;
                    }
                    Id = S->data[0].type_id;
                    /* FALLTHROUGH */

                case TypeId:
                    T = cc65_type_byid (Info, Id);
                    break;

                default:
                    T = 0;
                    PrintLine ("Invalid id type");
                    break;
            }
        } else {
            /* Ignore the invalid id */
            T = 0;
        }

        /* Output the list */
        if (T) {
            PrintType (Id, T);
            cc65_free_typedata (Info, T);
        }
    }
}



static void CmdUnload (Collection* Args attribute ((unused)))
/* Unload a debug info file */
{
    UnloadFile ();
}



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



static int Parse (char* CmdLine, Collection* Args)
/* Parse the command line and store the arguments in Args. Return true if ok,
** false on error.
*/
{
    char* End;

    /* Clear the collection */
    CollDeleteAll (Args);

    /* Parse the command line */
    while (1) {

        /* Break out on end of line */
        if (*CmdLine == '\0') {
            break;
        }

        /* Search for start of next command */
        if (IsSpace (*CmdLine)) {
            ++CmdLine;
            continue;
        }

        /* Allow double quotes to terminate a command */
        if (*CmdLine == '\"' || *CmdLine == '\'') {
            char Term = *CmdLine++;
            End = CmdLine;
            while (*End != Term) {
                if (*End == '\0') {
                    PrintLine ("Unterminated argument");
                    return 0;
                }
                ++End;
            }
            *End++ = '\0';
        } else {
            End = CmdLine;
            while (!IsSpace (*End)) {
                if (*End == '\0') {
                    PrintLine ("Unterminated argument");
                    return 0;
                }
                ++End;
            }
            *End++ = '\0';
        }
        CollAppend (Args, CmdLine);
        CmdLine = End;
    }

    /* Ok */
    return 1;
}



int main (int argc, char* argv[])
/* Main program */
{
    char Input[256];
    Collection Args = STATIC_COLLECTION_INITIALIZER;

    /* Initialize the command line */
    InitCmdLine (&argc, &argv, "dbgsh");

    /* If we have commands on the command line, execute them */
    if (ArgCount > 1) {
        unsigned I;
        for (I = 1; I < ArgCount; ++I) {
            CollAppend (&Args, ArgVec[I]);
        }

        /* Search for the command, check number of args, then execute it */
        ExecCmd (&Args, MainCmds, sizeof (MainCmds) / sizeof (MainCmds[0]));
    }

    /* Loop until program end */
    while (!Terminate) {

        /* Output a prompt, then read the input */
        Print ("dbgsh> ");
        fflush (stdout);
        if (fgets (Input, sizeof (Input), stdin) == 0) {
            PrintLine ("(EOF)");
            break;
        }

        /* Parse the command line */
        if (Parse (Input, &Args) == 0 || CollCount (&Args) == 0) {
            continue;
        }

        /* Search for the command, check number of args, then execute it */
        ExecCmd (&Args, MainCmds, sizeof (MainCmds) / sizeof (MainCmds[0]));
    }

    /* Free arguments */
    DoneCollection (&Args);
    return 0;
}



