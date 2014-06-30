/*****************************************************************************/
/*                                                                           */
/*                                 dbginfo.c                                 */
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



#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <assert.h>
#include <errno.h>

#include "dbginfo.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Use this for debugging - beware, lots of output */
#define DEBUG           0

/* Version numbers of the debug format we understand */
#define VER_MAJOR       2U
#define VER_MINOR       0U

/* Dynamic strings */
typedef struct StrBuf StrBuf;
struct StrBuf {
    char*       Buf;                    /* Pointer to buffer */
    unsigned    Len;                    /* Length of the string */
    unsigned    Allocated;              /* Size of allocated memory */
};

/* Initializer for a string buffer */
#define STRBUF_INITIALIZER      { 0, 0, 0 }

/* An array of unsigneds/pointers that grows if needed. C guarantees that a
** pointer to a union correctly converted points to each of its members.
** So what we do here is using union entries that contain an unsigned
** (used to store ids until they're resolved) and pointers to actual items
** in storage.
*/
typedef union CollEntry CollEntry;
union CollEntry {
    void*               Ptr;
    unsigned            Id;
};

typedef struct Collection Collection;
struct Collection {
    unsigned            Count;          /* Number of items in the list */
    unsigned            Size;           /* Size of allocated array */
    CollEntry*          Items;          /* Array with dynamic size */
};

/* Initializer for static collections */
#define COLLECTION_INITIALIZER  { 0, 0, 0 }

/* Span info management. The following table has as many entries as there
** are addresses active in spans. Each entry lists the spans for this address.
*/
typedef struct SpanInfoListEntry SpanInfoListEntry;
struct SpanInfoListEntry {
    cc65_addr           Addr;           /* Unique address */
    unsigned            Count;          /* Number of SpanInfos for this address */
    void*               Data;           /* Either SpanInfo* or SpanInfo** */
};

typedef struct SpanInfoList SpanInfoList;
struct SpanInfoList {
    unsigned            Count;          /* Number of entries */
    SpanInfoListEntry*  List;           /* Dynamic array with entries */
};

/* Input tokens */
typedef enum {

    TOK_INVALID,                        /* Invalid token */
    TOK_EOF,                            /* End of file reached */

    TOK_INTCON,                         /* Integer constant */
    TOK_STRCON,                         /* String constant */

    TOK_EQUAL,                          /* = */
    TOK_COMMA,                          /* , */
    TOK_MINUS,                          /* - */
    TOK_PLUS,                           /* + */
    TOK_EOL,                            /* \n */

    TOK_FIRST_KEYWORD,
    TOK_ABSOLUTE = TOK_FIRST_KEYWORD,   /* ABSOLUTE keyword */
    TOK_ADDRSIZE,                       /* ADDRSIZE keyword */
    TOK_AUTO,                           /* AUTO keyword */
    TOK_COUNT,                          /* COUNT keyword */
    TOK_CSYM,                           /* CSYM keyword */
    TOK_DEF,                            /* DEF keyword */
    TOK_ENUM,                           /* ENUM keyword */
    TOK_EQUATE,                         /* EQUATE keyword */
    TOK_EXPORT,                         /* EXPORT keyword */
    TOK_EXTERN,                         /* EXTERN keyword */
    TOK_FILE,                           /* FILE keyword */
    TOK_FUNC,                           /* FUNC keyword */
    TOK_GLOBAL,                         /* GLOBAL keyword */
    TOK_ID,                             /* ID keyword */
    TOK_IMPORT,                         /* IMPORT keyword */
    TOK_INFO,                           /* INFO keyword */
    TOK_LABEL,                          /* LABEL keyword */
    TOK_LIBRARY,                        /* LIBRARY keyword */
    TOK_LINE,                           /* LINE keyword */
    TOK_LONG,                           /* LONG_keyword */
    TOK_MAJOR,                          /* MAJOR keyword */
    TOK_MINOR,                          /* MINOR keyword */
    TOK_MODULE,                         /* MODULE keyword */
    TOK_MTIME,                          /* MTIME keyword */
    TOK_NAME,                           /* NAME keyword */
    TOK_OFFS,                           /* OFFS keyword */
    TOK_OUTPUTNAME,                     /* OUTPUTNAME keyword */
    TOK_OUTPUTOFFS,                     /* OUTPUTOFFS keyword */
    TOK_PARENT,                         /* PARENT keyword */
    TOK_REF,                            /* REF keyword */
    TOK_REGISTER,                       /* REGISTER keyword */
    TOK_RO,                             /* RO keyword */
    TOK_RW,                             /* RW keyword */
    TOK_SC,                             /* SC keyword */
    TOK_SCOPE,                          /* SCOPE keyword */
    TOK_SEGMENT,                        /* SEGMENT keyword */
    TOK_SIZE,                           /* SIZE keyword */
    TOK_SPAN,                           /* SPAN keyword */
    TOK_START,                          /* START keyword */
    TOK_STATIC,                         /* STATIC keyword */
    TOK_STRUCT,                         /* STRUCT keyword */
    TOK_SYM,                            /* SYM keyword */
    TOK_TYPE,                           /* TYPE keyword */
    TOK_VALUE,                          /* VALUE keyword */
    TOK_VAR,                            /* VAR keyword */
    TOK_VERSION,                        /* VERSION keyword */
    TOK_ZEROPAGE,                       /* ZEROPAGE keyword */
    TOK_LAST_KEYWORD = TOK_ZEROPAGE,

    TOK_IDENT,                          /* To catch unknown keywords */
} Token;

/* Data structure containing information from the debug info file. A pointer
** to this structure is passed as handle to callers from the outside.
*/
typedef struct DbgInfo DbgInfo;
struct DbgInfo {

    /* First we have all items in collections sorted by id. The ids are
    ** continous, so an access by id is almost as cheap as an array access.
    ** The collections are also used when the objects are deleted, so they're
    ** actually the ones that "own" the items.
    */
    Collection          CSymInfoById;   /* C symbol infos sorted by id */
    Collection          FileInfoById;   /* File infos sorted by id */
    Collection          LibInfoById;    /* Library infos sorted by id */
    Collection          LineInfoById;   /* Line infos sorted by id */
    Collection          ModInfoById;    /* Module infos sorted by id */
    Collection          ScopeInfoById;  /* Scope infos sorted by id */
    Collection          SegInfoById;    /* Segment infos sorted by id */
    Collection          SpanInfoById;   /* Span infos sorted by id */
    Collection          SymInfoById;    /* Symbol infos sorted by id */
    Collection          TypeInfoById;   /* Type infos sorted by id */

    /* Collections with other sort criteria */
    Collection          CSymFuncByName; /* C functions sorted by name */
    Collection          FileInfoByName; /* File infos sorted by name */
    Collection          ModInfoByName;  /* Module info sorted by name */
    Collection          ScopeInfoByName;/* Scope infos sorted by name */
    Collection          SegInfoByName;  /* Segment infos sorted by name */
    Collection          SymInfoByName;  /* Symbol infos sorted by name */
    Collection          SymInfoByVal;   /* Symbol infos sorted by value */

    /* Other stuff */
    SpanInfoList        SpanInfoByAddr; /* Span infos sorted by unique address */

    /* Info data */
    unsigned long       MemUsage;       /* Memory usage for the data */
    unsigned            MajorVersion;   /* Major version number of loaded file */
    unsigned            MinorVersion;   /* Minor version number of loaded file */
    char                FileName[1];    /* Name of input file */
};

/* Data used when parsing the debug info file */
typedef struct InputData InputData;
struct InputData {
    const char*         FileName;       /* Name of input file */
    cc65_line           Line;           /* Current line number */
    unsigned            Col;            /* Current column number */
    cc65_line           SLine;          /* Line number at start of token */
    unsigned            SCol;           /* Column number at start of token */
    unsigned            Errors;         /* Number of errors */
    FILE*               F;              /* Input file */
    int                 C;              /* Input character */
    Token               Tok;            /* Token from input stream */
    unsigned long       IVal;           /* Integer constant */
    StrBuf              SVal;           /* String constant */
    cc65_errorfunc      Error;          /* Function called in case of errors */
    DbgInfo*            Info;           /* Pointer to debug info */
};

/* Typedefs for the item structures. Do also serve as forwards */
typedef struct CSymInfo CSymInfo;
typedef struct FileInfo FileInfo;
typedef struct LibInfo LibInfo;
typedef struct LineInfo LineInfo;
typedef struct ModInfo ModInfo;
typedef struct ScopeInfo ScopeInfo;
typedef struct SegInfo SegInfo;
typedef struct SpanInfo SpanInfo;
typedef struct SymInfo SymInfo;
typedef struct TypeInfo TypeInfo;

/* Internally used c symbol info struct */
struct CSymInfo {
    unsigned            Id;             /* Id of file */
    unsigned short      Kind;           /* Kind of C symbol */
    unsigned short      SC;             /* Storage class of C symbol */
    int                 Offs;           /* Offset */
    union {
        unsigned        Id;             /* Id of attached asm symbol */
        SymInfo*        Info;           /* Pointer to attached asm symbol */
    } Sym;
    union {
        unsigned        Id;             /* Id of type */
        TypeInfo*       Info;           /* Pointer to type */
    } Type;
    union {
        unsigned        Id;             /* Id of scope */
        ScopeInfo*      Info;           /* Pointer to scope */
    } Scope;
    char                Name[1];        /* Name of file with full path */
};

/* Internally used file info struct */
struct FileInfo {
    unsigned            Id;             /* Id of file */
    unsigned long       Size;           /* Size of file */
    unsigned long       MTime;          /* Modification time */
    Collection          ModInfoByName;  /* Modules in which this file is used */
    Collection          LineInfoByLine; /* Line infos sorted by line */
    char                Name[1];        /* Name of file with full path */
};

/* Internally used library info struct */
struct LibInfo {
    unsigned            Id;             /* Id of library */
    char                Name[1];        /* Name of library with path */
};

/* Internally used line info struct */
struct LineInfo {
    unsigned            Id;             /* Id of line info */
    cc65_line           Line;           /* Line number */
    union {
        unsigned        Id;             /* Id of file */
        FileInfo*       Info;           /* Pointer to file info */
    } File;
    cc65_line_type      Type;           /* Type of line */
    unsigned            Count;          /* Nesting counter for macros */
    Collection          SpanInfoList;   /* List of spans for this line */
};

/* Internally used module info struct */
struct ModInfo {
    unsigned            Id;             /* Id of library */
    union {
        unsigned        Id;             /* Id of main source file */
        FileInfo*       Info;           /* Pointer to file info */
    } File;
    union {
        unsigned        Id;             /* Id of library if any */
        LibInfo*        Info;           /* Pointer to library info */
    } Lib;
    ScopeInfo*          MainScope;      /* Pointer to main scope */
    Collection          CSymFuncByName; /* C functions by name */
    Collection          FileInfoByName; /* Files for this module */
    Collection          ScopeInfoByName;/* Scopes for this module */
    char                Name[1];        /* Name of module with path */
};

/* Internally used scope info struct */
struct ScopeInfo {
    unsigned            Id;             /* Id of scope */
    cc65_scope_type     Type;           /* Type of scope */
    cc65_size           Size;           /* Size of scope */
    union {
        unsigned        Id;             /* Id of module */
        ModInfo*        Info;           /* Pointer to module info */
    } Mod;
    union {
        unsigned        Id;             /* Id of parent scope */
        ScopeInfo*      Info;           /* Pointer to parent scope */
    } Parent;
    union {
        unsigned        Id;             /* Id of label symbol */
        SymInfo*        Info;           /* Pointer to label symbol */
    } Label;
    CSymInfo*           CSymFunc;       /* C function for this scope */
    Collection          SpanInfoList;   /* List of spans for this scope */
    Collection          SymInfoByName;  /* Symbols in this scope */
    Collection*         CSymInfoByName; /* C symbols for this scope */
    Collection*         ChildScopeList; /* Child scopes of this scope */
    char                Name[1];        /* Name of scope */
};

/* Internally used segment info struct */
struct SegInfo {
    unsigned            Id;             /* Id of segment */
    cc65_addr           Start;          /* Start address of segment */
    cc65_size           Size;           /* Size of segment */
    char*               OutputName;     /* Name of output file */
    unsigned long       OutputOffs;     /* Offset in output file */
    char                Name[1];        /* Name of segment */
};

/* Internally used span info struct */
struct SpanInfo {
    unsigned            Id;             /* Id of span */
    cc65_addr           Start;          /* Start of span */
    cc65_addr           End;            /* End of span */
    union {
        unsigned        Id;             /* Id of segment */
        SegInfo*        Info;           /* Pointer to segment */
    } Seg;
    union {
        unsigned        Id;             /* Id of type */
        TypeInfo*       Info;           /* Pointer to type */
    } Type;
    Collection*         ScopeInfoList;  /* Scopes for this span */
    Collection*         LineInfoList;   /* Lines for this span */
};

/* Internally used symbol info struct */
struct SymInfo {
    unsigned            Id;             /* Id of symbol */
    cc65_symbol_type    Type;           /* Type of symbol */
    long                Value;          /* Value of symbol */
    cc65_size           Size;           /* Size of symbol */
    union {
        unsigned        Id;             /* Id of export if any */
        SymInfo*        Info;           /* Pointer to export if any */
    } Exp;
    union {
        unsigned        Id;             /* Id of segment if any */
        SegInfo*        Info;           /* Pointer to segment if any */
    } Seg;
    union {
        unsigned        Id;             /* Id of symbol scope */
        ScopeInfo*      Info;           /* Pointer to symbol scope */
    } Scope;
    union {
        unsigned        Id;             /* Parent symbol if any */
        SymInfo*        Info;           /* Pointer to parent symbol if any */
    } Parent;
    CSymInfo*           CSym;           /* Corresponding C symbol */
    Collection*         ImportList;     /* List of imports if this is an export */
    Collection*         CheapLocals;    /* List of cheap local symbols */
    Collection          DefLineInfoList;/* Line info of symbol definition */
    Collection          RefLineInfoList;/* Line info of symbol references */
    char                Name[1];        /* Name of symbol */
};

/* Internally used type info struct */
struct TypeInfo {
    unsigned            Id;             /* Id of type */
    cc65_typedata       Data[1];        /* Data, dynamically allocated */
};

/* A structure used when parsing a type string into a set of cc65_typedata
** structures.
*/
typedef struct TypeParseData TypeParseData;
struct TypeParseData {
    TypeInfo*           Info;
    unsigned            ItemCount;
    unsigned            ItemIndex;
    cc65_typedata*      ItemData;
    const StrBuf*       Type;
    unsigned            Pos;
    unsigned            Error;
};



/*****************************************************************************/
/*                                 Forwards                                  */
/*****************************************************************************/



static void NextToken (InputData* D);
/* Read the next token from the input stream */



/*****************************************************************************/
/*                             Memory allocation                             */
/*****************************************************************************/



static void* xmalloc (size_t Size)
/* Allocate memory, check for out of memory condition. Do some debugging */
{
    void* P = 0;

    /* Allow zero sized requests and return NULL in this case */
    if (Size) {

        /* Allocate memory */
        P = malloc (Size);

        /* Check for errors */
        assert (P != 0);
    }

    /* Return a pointer to the block */
    return P;
}



static void* xrealloc (void* P, size_t Size)
/* Reallocate a memory block, check for out of memory */
{
    /* Reallocate the block */
    void* N = realloc (P, Size);

    /* Check for errors */
    assert (N != 0 || Size == 0);

    /* Return the pointer to the new block */
    return N;
}



static void xfree (void* Block)
/* Free the block, do some debugging */
{
    free (Block);
}



/*****************************************************************************/
/*                              Dynamic strings                              */
/*****************************************************************************/



static void SB_Done (StrBuf* B)
/* Free the data of a string buffer (but not the struct itself) */
{
    if (B->Allocated) {
        xfree (B->Buf);
    }
}



static void SB_Realloc (StrBuf* B, unsigned NewSize)
/* Reallocate the string buffer space, make sure at least NewSize bytes are
** available.
*/
{
    /* Get the current size, use a minimum of 8 bytes */
    unsigned NewAllocated = B->Allocated;
    if (NewAllocated == 0) {
        NewAllocated = 8;
    }

    /* Round up to the next power of two */
    while (NewAllocated < NewSize) {
        NewAllocated *= 2;
    }

    /* Reallocate the buffer. Beware: The allocated size may be zero while the
    ** length is not. This means that we have a buffer that wasn't allocated
    ** on the heap.
    */
    if (B->Allocated) {
        /* Just reallocate the block */
        B->Buf = xrealloc (B->Buf, NewAllocated);
    } else {
        /* Allocate a new block and copy */
        B->Buf = memcpy (xmalloc (NewAllocated), B->Buf, B->Len);
    }

    /* Remember the new block size */
    B->Allocated = NewAllocated;
}



static void SB_CheapRealloc (StrBuf* B, unsigned NewSize)
/* Reallocate the string buffer space, make sure at least NewSize bytes are
** available. This function won't copy the old buffer contents over to the new
** buffer and may be used if the old contents are overwritten later.
*/
{
    /* Get the current size, use a minimum of 8 bytes */
    unsigned NewAllocated = B->Allocated;
    if (NewAllocated == 0) {
        NewAllocated = 8;
    }

    /* Round up to the next power of two */
    while (NewAllocated < NewSize) {
        NewAllocated *= 2;
    }

    /* Free the old buffer if there is one */
    if (B->Allocated) {
        xfree (B->Buf);
    }

    /* Allocate a fresh block */
    B->Buf = xmalloc (NewAllocated);

    /* Remember the new block size */
    B->Allocated = NewAllocated;
}



static unsigned SB_GetLen (const StrBuf* B)
/* Return the length of the buffer contents */
{
    return B->Len;
}



static char* SB_GetBuf (StrBuf* B)
/* Return a buffer pointer */
{
    return B->Buf;
}



static const char* SB_GetConstBuf (const StrBuf* B)
/* Return a buffer pointer */
{
    return B->Buf;
}



static char SB_At (const StrBuf* B, unsigned Pos)
/* Return the character from a specific position */
{
    assert (Pos <= B->Len);
    return B->Buf[Pos];
}



static void SB_Terminate (StrBuf* B)
/* Zero terminate the given string buffer. NOTE: The terminating zero is not
** accounted for in B->Len, if you want that, you have to use AppendChar!
*/
{
    unsigned NewLen = B->Len + 1;
    if (NewLen > B->Allocated) {
        SB_Realloc (B, NewLen);
    }
    B->Buf[B->Len] = '\0';
}



static void SB_Clear (StrBuf* B)
/* Clear the string buffer (make it empty) */
{
    B->Len = 0;
}



static void SB_CopyBuf (StrBuf* Target, const char* Buf, unsigned Size)
/* Copy Buf to Target, discarding the old contents of Target */
{
    if (Size) {
        if (Target->Allocated < Size) {
            SB_CheapRealloc (Target, Size);
        }
        memcpy (Target->Buf, Buf, Size);
    }
    Target->Len = Size;
}



static void SB_Copy (StrBuf* Target, const StrBuf* Source)
/* Copy Source to Target, discarding the old contents of Target */
{
    SB_CopyBuf (Target, Source->Buf, Source->Len);
}



static void SB_AppendChar (StrBuf* B, int C)
/* Append a character to a string buffer */
{
    unsigned NewLen = B->Len + 1;
    if (NewLen > B->Allocated) {
        SB_Realloc (B, NewLen);
    }
    B->Buf[B->Len] = (char) C;
    B->Len = NewLen;
}



static char* SB_StrDup (const StrBuf* B)
/* Return the contents of B as a dynamically allocated string. The string
** will always be NUL terminated.
*/
{
    /* Allocate memory */
    char* S = xmalloc (B->Len + 1);

    /* Copy the string */
    memcpy (S, B->Buf, B->Len);

    /* Terminate it */
    S[B->Len] = '\0';

    /* And return the result */
    return S;
}



/*****************************************************************************/
/*                                Collections                                */
/*****************************************************************************/



static Collection* CollInit (Collection* C)
/* Initialize a collection and return it. */
{
    /* Intialize the fields. */
    C->Count = 0;
    C->Size  = 0;
    C->Items = 0;

    /* Return the new struct */
    return C;
}



static Collection* CollNew (void)
/* Allocate a new collection, initialize and return it */
{
    return CollInit (xmalloc (sizeof (Collection)));
}



static void CollDone (Collection* C)
/* Free the data for a collection. This will not free the data contained in
** the collection.
*/
{
    /* Free the pointer array */
    xfree (C->Items);

    /* Clear the fields, so the collection may be reused (or CollDone called)
    ** again
    */
    C->Count = 0;
    C->Size  = 0;
    C->Items = 0;
}



static void CollFree (Collection* C)
/* Free a dynamically allocated collection */
{
    /* Accept NULL pointers */
    if (C) {
        xfree (C->Items);
        xfree (C);
    }
}



static unsigned CollCount (const Collection* C)
/* Return the number of items in the collection. Return 0 if C is NULL. */
{
    return C? C->Count : 0;
}



static void CollMove (Collection* Source, Collection* Target)
/* Move all data from one collection to another. This function will first free
** the data in the target collection, and - after the move - clear the data
** from the source collection.
*/
{
    /* Free the target collection data */
    xfree (Target->Items);

    /* Now copy the whole bunch over */
    *Target = *Source;

    /* Empty Source */
    Source->Count = 0;
    Source->Size  = 0;
    Source->Items = 0;
}



static void CollGrow (Collection* C, unsigned Size)
/* Grow the collection C so it is able to hold Size items without a resize
** being necessary. This can be called for performance reasons if the number
** of items to be placed in the collection is known in advance.
*/
{
    CollEntry* NewItems;

    /* Ignore the call if the collection is already large enough */
    if (Size <= C->Size) {
        return;
    }

    /* Grow the collection */
    C->Size = Size;
    NewItems = xmalloc (C->Size * sizeof (CollEntry));
    memcpy (NewItems, C->Items, C->Count * sizeof (CollEntry));
    xfree (C->Items);
    C->Items = NewItems;
}



static void CollPrepareInsert (Collection* C, unsigned Index)
/* Prepare for insertion of the data at a given position in the collection */
{
    /* Check for invalid indices */
    assert (Index <= C->Count);

    /* Grow the array if necessary */
    if (C->Count >= C->Size) {
        /* Must grow */
        CollGrow (C, (C->Size == 0)? 1 : C->Size * 2);
    }

    /* Move the existing elements if needed */
    if (C->Count != Index) {
        memmove (C->Items+Index+1, C->Items+Index, (C->Count-Index) * sizeof (void*));
    }
    ++C->Count;
}



static void CollInsert (Collection* C, void* Item, unsigned Index)
/* Insert the data at the given position in the collection */
{
    /* Prepare for insertion (free the given slot) */
    CollPrepareInsert (C, Index);

    /* Store the new item */
    C->Items[Index].Ptr = Item;
}



static void CollInsertId (Collection* C, unsigned Id, unsigned Index)
/* Insert the data at the given position in the collection */
{
    /* Prepare for insertion (free the given slot) */
    CollPrepareInsert (C, Index);

    /* Store the new item */
    C->Items[Index].Id = Id;
}



static void CollReplace (Collection* C, void* Item, unsigned Index)
/* Replace the item at the given position by a new one */
{
    /* Check the index */
    assert (Index < C->Count);

    /* Replace the element */
    C->Items[Index].Ptr = Item;
}



static void CollReplaceExpand (Collection* C, void* Item, unsigned Index)
/* If Index is a valid index for the collection, replace the item at this
** position by the one passed. If the collection is too small, expand it,
** filling unused pointers with NULL, then add the new item at the given
** position.
*/
{
    if (Index < C->Count) {
        /* Collection is already large enough */
        C->Items[Index].Ptr = Item;
    } else {
        /* Must expand the collection */
        unsigned Size = C->Size;
        if (Size == 0) {
            Size = 2;
        }
        while (Index >= Size) {
            Size *= 2;
        }
        CollGrow (C, Size);

        /* Fill up unused slots with NULL */
        while (C->Count < Index) {
            C->Items[C->Count++].Ptr = 0;
        }

        /* Fill in the item */
        C->Items[C->Count++].Ptr = Item;
    }
}



static void CollAppend (Collection* C, void* Item)
/* Append an item to the end of the collection */
{
    /* Insert the item at the end of the current list */
    CollInsert (C, Item, C->Count);
}



static void CollAppendId (Collection* C, unsigned Id)
/* Append an id to the end of the collection */
{
    /* Insert the id at the end of the current list */
    CollInsertId (C, Id, C->Count);
}



static void* CollAt (const Collection* C, unsigned Index)
/* Return the item at the given index */
{
    /* Check the index */
    assert (Index < C->Count);

    /* Return the element */
    return C->Items[Index].Ptr;
}



static unsigned CollIdAt (const Collection* C, unsigned Index)
/* Return the id at the given index */
{
    /* Check the index */
    assert (Index < C->Count);

    /* Return the element */
    return C->Items[Index].Id;
}



static void CollQuickSort (Collection* C, int Lo, int Hi,
                           int (*Compare) (const void*, const void*))
/* Internal recursive sort function. */
{
    /* Get a pointer to the items */
    CollEntry* Items = C->Items;

    /* Quicksort */
    while (Hi > Lo) {
        int I = Lo + 1;
        int J = Hi;
        while (I <= J) {
            while (I <= J && Compare (Items[Lo].Ptr, Items[I].Ptr) >= 0) {
                ++I;
            }
            while (I <= J && Compare (Items[Lo].Ptr, Items[J].Ptr) < 0) {
                --J;
            }
            if (I <= J) {
                /* Swap I and J */
                CollEntry Tmp = Items[I];
                Items[I]  = Items[J];
                Items[J]  = Tmp;
                ++I;
                --J;
            }
        }
        if (J != Lo) {
            /* Swap J and Lo */
            CollEntry Tmp = Items[J];
            Items[J]  = Items[Lo];
            Items[Lo] = Tmp;
        }
        if (J > (Hi + Lo) / 2) {
            CollQuickSort (C, J + 1, Hi, Compare);
            Hi = J - 1;
        } else {
            CollQuickSort (C, Lo, J - 1, Compare);
            Lo = J + 1;
        }
    }
}



static void CollSort (Collection* C, int (*Compare) (const void*, const void*))
/* Sort the collection using the given compare function. */
{
    if (C->Count > 1) {
        CollQuickSort (C, 0, C->Count-1, Compare);
    }
}



/*****************************************************************************/
/*                              Debugging stuff                              */
/*****************************************************************************/



#if DEBUG

/* Output */
#define DBGPRINT(format, ...)   printf ((format), __VA_ARGS__)



static void DumpFileInfo (Collection* FileInfos)
/* Dump a list of file infos */
{
    unsigned I;

    /* File info */
    for (I = 0; I < CollCount (FileInfos); ++I) {
        const FileInfo* FI = CollAt (FileInfos, I);
        printf ("File info %u:\n"
                "  Name:   %s\n"
                "  Size:   %lu\n"
                "  MTime:  %lu\n",
                FI->Id,
                FI->Name,
                (unsigned long) FI->Size,
                (unsigned long) FI->MTime);
    }
}



static void DumpOneLineInfo (unsigned Num, LineInfo* LI)
/* Dump one line info entry */
{
    printf ("  Index:  %u\n"
            "    File:   %s\n"
            "    Line:   %lu\n"
            "    Range:  0x%06lX-0x%06lX\n"
            "    Type:   %u\n"
            "    Count:  %u\n",
            Num,
            LI->File.Info->Name,
            (unsigned long) LI->Line,
            (unsigned long) LI->Start,
            (unsigned long) LI->End,
            LI->Type,
            LI->Count);
}



static void DumpSpanInfo (SpanInfoList* L)
/* Dump a list of span infos */
{
    unsigned I, J;

    /* Line info */
    for (I = 0; I < L->Count; ++I) {
        const SpanInfoListEntry* E = &L->List[I];
        printf ("Addr:   %lu\n", (unsigned long) E->Addr);
        if (E->Count == 1) {
            DumpOneLineInfo (0, E->Data);
        } else {
            for (J = 0; J < E->Count; ++J) {
                DumpOneLineInfo (J, ((LineInfo**) E->Data)[J]);
            }
        }
    }
}



static void DumpData (InputData* D)
/* Dump internal data to stdout for debugging */
{
    /* Dump data */
    DumpFileInfo (&D->Info->FileInfoById);
    DumpLineInfo (&D->Info->LineInfoByAddr);
}
#else

/* No output */
#ifdef __WATCOMC__
static void DBGPRINT(const char* format, ...) {}
#else
#define DBGPRINT(format, ...)
#endif

#endif



/*****************************************************************************/
/*                             Helper functions                              */
/*****************************************************************************/



static unsigned GetId (const void* Data)
/* Return the id of one of the info structures. All structures have the Id
** field as first member, and the C standard allows converting a union pointer
** to the data type of the first member, so this is safe and portable.
*/
{
    if (Data) {
        return *(const unsigned*)Data;
    } else {
        return CC65_INV_ID;
    }
}



static unsigned HexValue (char C)
/* Convert the ascii representation of a hex nibble into the hex nibble */
{
    if (isdigit (C)) {
        return C - '0';
    } else if (islower (C)) {
        return C - 'a' + 10;
    } else {
        return C - 'A' + 10;
    }
}



static void ParseError (InputData* D, cc65_error_severity Type, const char* Msg, ...)
/* Call the user supplied parse error function */
{
    va_list             ap;
    int                 MsgSize;
    cc65_parseerror*    E;

    /* Test-format the error message so we know how much space to allocate */
    va_start (ap, Msg);
    MsgSize = vsnprintf (0, 0, Msg, ap);
    va_end (ap);

    /* Allocate memory */
    E = xmalloc (sizeof (*E) + MsgSize);

    /* Write data to E */
    E->type   = Type;
    E->name   = D->FileName;
    E->line   = D->SLine;
    E->column = D->SCol;
    va_start (ap, Msg);
    vsnprintf (E->errormsg, MsgSize+1, Msg, ap);
    va_end (ap);

    /* Call the caller:-) */
    D->Error (E);

    /* Free the data structure */
    xfree (E);

    /* Count errors */
    if (Type == CC65_ERROR) {
        ++D->Errors;
    }
}



static void SkipLine (InputData* D)
/* Error recovery routine. Skip tokens until EOL or EOF is reached */
{
    while (D->Tok != TOK_EOL && D->Tok != TOK_EOF) {
        NextToken (D);
    }
}



static void UnexpectedToken (InputData* D)
/* Call ParseError with a message about an unexpected input token */
{
    ParseError (D, CC65_ERROR, "Unexpected input token %d", D->Tok);
    SkipLine (D);
}



static void UnknownKeyword (InputData* D)
/* Print a warning about an unknown keyword in the file. Try to do smart
** recovery, so if later versions of the debug information add additional
** keywords, this code may be able to at least ignore them.
*/
{
    /* Output a warning */
    ParseError (D, CC65_WARNING, "Unknown keyword \"%s\" - skipping",
                SB_GetConstBuf (&D->SVal));

    /* Skip the identifier */
    NextToken (D);

    /* If an equal sign follows, ignore anything up to the next line end
    ** or comma. If a comma or line end follows, we're already done. If
    ** we have none of both, we ignore the remainder of the line.
    */
    if (D->Tok == TOK_EQUAL) {
        NextToken (D);
        while (D->Tok != TOK_COMMA && D->Tok != TOK_EOL && D->Tok != TOK_EOF) {
            NextToken (D);
        }
    } else if (D->Tok != TOK_COMMA && D->Tok != TOK_EOL && D->Tok != TOK_EOF) {
        SkipLine (D);
    }
}



/*****************************************************************************/
/*                               C symbol info                               */
/*****************************************************************************/



static CSymInfo* NewCSymInfo (const StrBuf* Name)
/* Create a new CSymInfo struct and return it */
{
    /* Allocate memory */
    CSymInfo* S = xmalloc (sizeof (CSymInfo) + SB_GetLen (Name));

    /* Initialize it */
    memcpy (S->Name, SB_GetConstBuf (Name), SB_GetLen (Name) + 1);

    /* Return it */
    return S;
}



static void FreeCSymInfo (CSymInfo* S)
/* Free a CSymInfo struct */
{
    /* Free the structure itself */
    xfree (S);
}



static cc65_csyminfo* new_cc65_csyminfo (unsigned Count)
/* Allocate and return a cc65_csyminfo struct that is able to hold Count
** entries. Initialize the count field of the returned struct.
*/
{
    cc65_csyminfo* S = xmalloc (sizeof (*S) - sizeof (S->data[0]) +
                                Count * sizeof (S->data[0]));
    S->count = Count;
    return S;
}



static void CopyCSymInfo (cc65_csymdata* D, const CSymInfo* S)
/* Copy data from a CSymInfo struct to a cc65_csymdata struct */
{
    D->csym_id      = S->Id;
    D->csym_kind    = S->Kind;
    D->csym_sc      = S->SC;
    D->csym_offs    = S->Offs;
    D->type_id      = GetId (S->Type.Info);
    D->symbol_id    = GetId (S->Sym.Info);
    D->scope_id     = GetId (S->Scope.Info);
    D->csym_name    = S->Name;
}



static int CompareCSymInfoByName (const void* L, const void* R)
/* Helper function to sort c symbol infos in a collection by name */
{
    /* Sort by symbol name, then by id */
    int Res = strcmp (((const CSymInfo*) L)->Name, ((const CSymInfo*) R)->Name);
    if (Res == 0) {
        Res = (int)((const CSymInfo*) L)->Id - (int)((const CSymInfo*) R)->Id;
    }
    return Res;
}



/*****************************************************************************/
/*                                 File info                                 */
/*****************************************************************************/



static FileInfo* NewFileInfo (const StrBuf* Name)
/* Create a new FileInfo struct and return it */
{
    /* Allocate memory */
    FileInfo* F = xmalloc (sizeof (FileInfo) + SB_GetLen (Name));

    /* Initialize it */
    CollInit (&F->ModInfoByName);
    CollInit (&F->LineInfoByLine);
    memcpy (F->Name, SB_GetConstBuf (Name), SB_GetLen (Name) + 1);

    /* Return it */
    return F;
}



static void FreeFileInfo (FileInfo* F)
/* Free a FileInfo struct */
{
    /* Delete the collections */
    CollDone (&F->ModInfoByName);
    CollDone (&F->LineInfoByLine);

    /* Free the file info structure itself */
    xfree (F);
}



static cc65_sourceinfo* new_cc65_sourceinfo (unsigned Count)
/* Allocate and return a cc65_sourceinfo struct that is able to hold Count
** entries. Initialize the count field of the returned struct.
*/
{
    cc65_sourceinfo* S = xmalloc (sizeof (*S) - sizeof (S->data[0]) +
                                  Count * sizeof (S->data[0]));
    S->count = Count;
    return S;
}



static void CopyFileInfo (cc65_sourcedata* D, const FileInfo* F)
/* Copy data from a FileInfo struct to a cc65_sourcedata struct */
{
    D->source_id    = F->Id;
    D->source_name  = F->Name;
    D->source_size  = F->Size;
    D->source_mtime = F->MTime;
}



static int CompareFileInfoByName (const void* L, const void* R)
/* Helper function to sort file infos in a collection by name */
{
    /* Sort by file name. If names are equal, sort by timestamp,
    ** then sort by size. Which means, identical files will go
    ** together.
    */
    int Res = strcmp (((const FileInfo*) L)->Name,
                      ((const FileInfo*) R)->Name);
    if (Res != 0) {
        return Res;
    }
    if (((const FileInfo*) L)->MTime > ((const FileInfo*) R)->MTime) {
        return 1;
    } else if (((const FileInfo*) L)->MTime < ((const FileInfo*) R)->MTime) {
        return -1;
    }
    if (((const FileInfo*) L)->Size > ((const FileInfo*) R)->Size) {
        return 1;
    } else if (((const FileInfo*) L)->Size < ((const FileInfo*) R)->Size) {
        return -1;
    } else {
        return 0;
    }
}



/*****************************************************************************/
/*                               Library info                                */
/*****************************************************************************/



static LibInfo* NewLibInfo (const StrBuf* Name)
/* Create a new LibInfo struct, intialize and return it */
{
    /* Allocate memory */
    LibInfo* L = xmalloc (sizeof (LibInfo) + SB_GetLen (Name));

    /* Initialize the name */
    memcpy (L->Name, SB_GetConstBuf (Name), SB_GetLen (Name) + 1);

    /* Return it */
    return L;
}



static void FreeLibInfo (LibInfo* L)
/* Free a LibInfo struct */
{
    xfree (L);
}



static cc65_libraryinfo* new_cc65_libraryinfo (unsigned Count)
/* Allocate and return a cc65_libraryinfo struct that is able to hold Count
** entries. Initialize the count field of the returned struct.
*/
{
    cc65_libraryinfo* L = xmalloc (sizeof (*L) - sizeof (L->data[0]) +
                                   Count * sizeof (L->data[0]));
    L->count = Count;
    return L;
}



static void CopyLibInfo (cc65_librarydata* D, const LibInfo* L)
/* Copy data from a LibInfo struct to a cc65_librarydata struct */
{
    D->library_id   = L->Id;
    D->library_name = L->Name;
}



/*****************************************************************************/
/*                                 Line info                                 */
/*****************************************************************************/



static LineInfo* NewLineInfo (void)
/* Create a new LineInfo struct and return it */
{
    /* Allocate memory */
    LineInfo* L = xmalloc (sizeof (LineInfo));

    /* Initialize and return it */
    CollInit (&L->SpanInfoList);
    return L;
}



static void FreeLineInfo (LineInfo* L)
/* Free a LineInfo struct */
{
    CollDone (&L->SpanInfoList);
    xfree (L);
}



static cc65_lineinfo* new_cc65_lineinfo (unsigned Count)
/* Allocate and return a cc65_lineinfo struct that is able to hold Count
** entries. Initialize the count field of the returned struct.
*/
{
    cc65_lineinfo* L = xmalloc (sizeof (*L) - sizeof (L->data[0]) +
                                Count * sizeof (L->data[0]));
    L->count = Count;
    return L;
}



static void CopyLineInfo (cc65_linedata* D, const LineInfo* L)
/* Copy data from a LineInfo struct to a cc65_linedata struct */
{
    D->line_id          = L->Id;
    D->source_id        = L->File.Info->Id;
    D->source_line      = L->Line;
    D->line_type        = L->Type;
    D->count            = L->Count;
}



static int CompareLineInfoByLine (const void* L, const void* R)
/* Helper function to sort line infos in a collection by line. */
{
    int Left  = ((const LineInfo*) L)->Line;
    int Right = ((const LineInfo*) R)->Line;
    return Left - Right;
}



/*****************************************************************************/
/*                                Module info                                */
/*****************************************************************************/



static ModInfo* NewModInfo (const StrBuf* Name)
/* Create a new ModInfo struct, intialize and return it */
{
    /* Allocate memory */
    ModInfo* M = xmalloc (sizeof (ModInfo) + SB_GetLen (Name));

    /* Initialize it */
    M->MainScope = 0;
    CollInit (&M->CSymFuncByName);
    CollInit (&M->FileInfoByName);
    CollInit (&M->ScopeInfoByName);
    memcpy (M->Name, SB_GetConstBuf (Name), SB_GetLen (Name) + 1);

    /* Return it */
    return M;
}



static void FreeModInfo (ModInfo* M)
/* Free a ModInfo struct */
{
    /* Free the collections */
    CollDone (&M->CSymFuncByName);
    CollDone (&M->FileInfoByName);
    CollDone (&M->ScopeInfoByName);

    /* Free the structure itself */
    xfree (M);
}



static cc65_moduleinfo* new_cc65_moduleinfo (unsigned Count)
/* Allocate and return a cc65_moduleinfo struct that is able to hold Count
** entries. Initialize the count field of the returned struct.
*/
{
    cc65_moduleinfo* M = xmalloc (sizeof (*M) - sizeof (M->data[0]) +
                                  Count * sizeof (M->data[0]));
    M->count = Count;
    return M;
}



static void CopyModInfo (cc65_moduledata* D, const ModInfo* M)
/* Copy data from a ModInfo struct to a cc65_moduledata struct */
{
    D->module_id    = M->Id;
    D->module_name  = M->Name;
    D->source_id    = M->File.Info->Id;
    D->library_id   = GetId (M->Lib.Info);
    D->scope_id     = GetId (M->MainScope);
}



static int CompareModInfoByName (const void* L, const void* R)
/* Helper function to sort module infos in a collection by name */
{
    /* Compare module name */
    return strcmp (((const ModInfo*) L)->Name, ((const ModInfo*) R)->Name);
}



/*****************************************************************************/
/*                                Scope info                                 */
/*****************************************************************************/



static ScopeInfo* NewScopeInfo (const StrBuf* Name)
/* Create a new ScopeInfo struct, intialize and return it */
{
    /* Allocate memory */
    ScopeInfo* S = xmalloc (sizeof (ScopeInfo) + SB_GetLen (Name));

    /* Initialize the fields as necessary */
    S->CSymFunc = 0;
    CollInit (&S->SpanInfoList);
    CollInit (&S->SymInfoByName);
    S->CSymInfoByName = 0;
    S->ChildScopeList = 0;
    memcpy (S->Name, SB_GetConstBuf (Name), SB_GetLen (Name) + 1);

    /* Return it */
    return S;
}



static void FreeScopeInfo (ScopeInfo* S)
/* Free a ScopeInfo struct */
{
    CollDone (&S->SpanInfoList);
    CollDone (&S->SymInfoByName);
    CollFree (S->CSymInfoByName);
    CollFree (S->ChildScopeList);
    xfree (S);
}



static cc65_scopeinfo* new_cc65_scopeinfo (unsigned Count)
/* Allocate and return a cc65_scopeinfo struct that is able to hold Count
** entries. Initialize the count field of the returned struct.
*/
{
    cc65_scopeinfo* S = xmalloc (sizeof (*S) - sizeof (S->data[0]) +
                                 Count * sizeof (S->data[0]));
    S->count = Count;
    return S;
}



static void CopyScopeInfo (cc65_scopedata* D, const ScopeInfo* S)
/* Copy data from a ScopeInfo struct to a cc65_scopedata struct */
{
    D->scope_id     = S->Id;
    D->scope_name   = S->Name;
    D->scope_type   = S->Type;
    D->scope_size   = S->Size;
    D->parent_id    = GetId (S->Parent.Info);
    D->symbol_id    = GetId (S->Label.Info);
    D->module_id    = S->Mod.Info->Id;
}



static int CompareScopeInfoByName (const void* L, const void* R)
/* Helper function to sort scope infos in a collection by name */
{
    const ScopeInfo* Left  = L;
    const ScopeInfo* Right = R;

    /* Compare scope name, then id */
    int Res = strcmp (Left->Name, Right->Name);
    if (Res == 0) {
        Res = (int)Left->Id - (int)Right->Id;
    }
    return Res;
}



/*****************************************************************************/
/*                               Segment info                                */
/*****************************************************************************/



static SegInfo* NewSegInfo (const StrBuf* Name, unsigned Id,
                            cc65_addr Start, cc65_addr Size,
                            const StrBuf* OutputName, unsigned long OutputOffs)
/* Create a new SegInfo struct and return it */
{
    /* Allocate memory */
    SegInfo* S = xmalloc (sizeof (SegInfo) + SB_GetLen (Name));

    /* Initialize it */
    S->Id         = Id;
    S->Start      = Start;
    S->Size       = Size;
    if (SB_GetLen (OutputName) > 0) {
        /* Output file given */
        S->OutputName = SB_StrDup (OutputName);
        S->OutputOffs = OutputOffs;
    } else {
        /* No output file given */
        S->OutputName = 0;
        S->OutputOffs = 0;
    }
    memcpy (S->Name, SB_GetConstBuf (Name), SB_GetLen (Name) + 1);

    /* Return it */
    return S;
}



static void FreeSegInfo (SegInfo* S)
/* Free a SegInfo struct */
{
    xfree (S->OutputName);
    xfree (S);
}



static cc65_segmentinfo* new_cc65_segmentinfo (unsigned Count)
/* Allocate and return a cc65_segmentinfo struct that is able to hold Count
** entries. Initialize the count field of the returned struct.
*/
{
    cc65_segmentinfo* S = xmalloc (sizeof (*S) - sizeof (S->data[0]) +
                                   Count * sizeof (S->data[0]));
    S->count = Count;
    return S;
}



static void CopySegInfo (cc65_segmentdata* D, const SegInfo* S)
/* Copy data from a SegInfo struct to a cc65_segmentdata struct */
{
    D->segment_id    = S->Id;
    D->segment_name  = S->Name;
    D->segment_start = S->Start;
    D->segment_size  = S->Size;
    D->output_name   = S->OutputName;
    D->output_offs   = S->OutputOffs;
}



static int CompareSegInfoByName (const void* L, const void* R)
/* Helper function to sort segment infos in a collection by name */
{
    /* Sort by file name */
    return strcmp (((const SegInfo*) L)->Name,
                   ((const SegInfo*) R)->Name);
}



/*****************************************************************************/
/*                                 Span info                                 */
/*****************************************************************************/



static SpanInfo* NewSpanInfo (void)
/* Create a new SpanInfo struct, intialize and return it */
{
    /* Allocate memory */
    SpanInfo* S = xmalloc (sizeof (SpanInfo));

    /* Initialize and return it */
    S->ScopeInfoList = 0;
    S->LineInfoList = 0;
    return S;
}



static void FreeSpanInfo (SpanInfo* S)
/* Free a SpanInfo struct */
{
    CollFree (S->ScopeInfoList);
    CollFree (S->LineInfoList);
    xfree (S);
}



static cc65_spaninfo* new_cc65_spaninfo (unsigned Count)
/* Allocate and return a cc65_spaninfo struct that is able to hold Count
** entries. Initialize the count field of the returned struct.
*/
{
    cc65_spaninfo* S = xmalloc (sizeof (*S) - sizeof (S->data[0]) +
                                Count * sizeof (S->data[0]));
    S->count = Count;
    return S;
}



static void CopySpanInfo (cc65_spandata* D, const SpanInfo* S)
/* Copy data from a SpanInfo struct to a cc65_spandata struct */
{
    D->span_id      = S->Id;
    D->span_start   = S->Start;
    D->span_end     = S->End;
    D->segment_id   = S->Seg.Info->Id;
    D->type_id      = GetId (S->Type.Info);
    D->scope_count  = CollCount (S->ScopeInfoList);
    D->line_count   = CollCount (S->LineInfoList);
}



static int CompareSpanInfoByAddr (const void* L, const void* R)
/* Helper function to sort span infos in a collection by address. Span infos
** with smaller start address are considered smaller. If start addresses are
** equal, line spans with smaller end address are considered smaller. This
** means, that when CompareSpanInfoByAddr is used for sorting, a range with
** identical start addresses will have smaller spans first, followed by
** larger spans.
*/
{
    /* Sort by start of span */
    if (((const SpanInfo*) L)->Start > ((const SpanInfo*) R)->Start) {
        return 1;
    } else if (((const SpanInfo*) L)->Start < ((const SpanInfo*) R)->Start) {
        return -1;
    } else if (((const SpanInfo*) L)->End > ((const SpanInfo*) R)->End) {
        return 1;
    } else if (((const SpanInfo*) L)->End < ((const SpanInfo*) R)->End) {
        return -1;
    } else {
        return 0;
    }
}



/*****************************************************************************/
/*                                Symbol info                                */
/*****************************************************************************/



static SymInfo* NewSymInfo (const StrBuf* Name)
/* Create a new SymInfo struct, intialize and return it */
{
    /* Allocate memory */
    SymInfo* S = xmalloc (sizeof (SymInfo) + SB_GetLen (Name));

    /* Initialize it as necessary */
    S->CSym        = 0;
    S->ImportList  = 0;
    S->CheapLocals = 0;
    CollInit (&S->DefLineInfoList);
    CollInit (&S->RefLineInfoList);
    memcpy (S->Name, SB_GetConstBuf (Name), SB_GetLen (Name) + 1);

    /* Return it */
    return S;
}



static void FreeSymInfo (SymInfo* S)
/* Free a SymInfo struct */
{
    CollFree (S->ImportList);
    CollFree (S->CheapLocals);
    CollDone (&S->DefLineInfoList);
    CollDone (&S->RefLineInfoList);
    xfree (S);
}



static cc65_symbolinfo* new_cc65_symbolinfo (unsigned Count)
/* Allocate and return a cc65_symbolinfo struct that is able to hold Count
** entries. Initialize the count field of the returned struct.
*/
{
    cc65_symbolinfo* S = xmalloc (sizeof (*S) - sizeof (S->data[0]) +
                                  Count * sizeof (S->data[0]));
    S->count = Count;
    return S;
}



static void CopySymInfo (cc65_symboldata* D, const SymInfo* S)
/* Copy data from a SymInfo struct to a cc65_symboldata struct */
{
    SegInfo* Seg;

    D->symbol_id        = S->Id;
    D->symbol_name      = S->Name;
    D->symbol_type      = S->Type;
    D->symbol_size      = S->Size;

    /* If this is an import, it doesn't have a value or segment. Use the data
    ** from the matching export instead.
    */
    if (S->Exp.Info) {
        /* This is an import, because it has a matching export */
        D->export_id    = S->Exp.Info->Id;
        D->symbol_value = S->Exp.Info->Value;
        Seg             = S->Exp.Info->Seg.Info;
    } else {
        D->export_id    = CC65_INV_ID;
        D->symbol_value = S->Value;
        Seg             = S->Seg.Info;
    }
    if (Seg) {
        D->segment_id   = Seg->Id;
    } else {
        D->segment_id   = CC65_INV_ID;
    }
    D->scope_id         = S->Scope.Info->Id;
    if (S->Parent.Info) {
        D->parent_id    = S->Parent.Info->Id;
    } else {
        D->parent_id    = CC65_INV_ID;
    }
}



static int CompareSymInfoByName (const void* L, const void* R)
/* Helper function to sort symbol infos in a collection by name */
{
    /* Sort by symbol name */
    return strcmp (((const SymInfo*) L)->Name,
                   ((const SymInfo*) R)->Name);
}



static int CompareSymInfoByVal (const void* L, const void* R)
/* Helper function to sort symbol infos in a collection by value */
{
    /* Sort by symbol value. If both are equal, sort by symbol name so it
    ** looks nice when such a list is returned.
    */
    if (((const SymInfo*) L)->Value > ((const SymInfo*) R)->Value) {
        return 1;
    } else if (((const SymInfo*) L)->Value < ((const SymInfo*) R)->Value) {
        return -1;
    } else {
        return CompareSymInfoByName (L, R);
    }
}



/*****************************************************************************/
/*                                 Type info                                 */
/*****************************************************************************/



/* The following definitions are actually just taken from gentype.h */

/* Size of a data type */
#define GT_SIZE_1               0x00U
#define GT_SIZE_2               0x01U
#define GT_SIZE_3               0x02U
#define GT_SIZE_4               0x03U
#define GT_SIZE_MASK            0x07U

#define GT_GET_SIZE(x)          (((x) & GT_SIZE_MASK) + 1U)

/* Sign of the data type */
#define GT_UNSIGNED             0x00U
#define GT_SIGNED               0x08U
#define GT_SIGN_MASK            0x08U

#define GT_HAS_SIGN(x)          (((x) & GT_SIZE_MASK) == GT_SIGNED)

/* Byte order */
#define GT_LITTLE_ENDIAN        0x00U
#define GT_BIG_ENDIAN           0x10U
#define GT_BYTEORDER_MASK       0x10U

#define GT_IS_LITTLE_ENDIAN(x)  (((x) & GT_BYTEORDER_MASK) == GT_LITTLE_ENDIAN)
#define GT_IS_BIG_ENDIAN(x)     (((x) & GT_BYTEORDER_MASK) == GT_BIG_ENDIAN)

/* Type of the data. */
#define GT_TYPE_VOID            0x00U
#define GT_TYPE_INT             0x20U
#define GT_TYPE_PTR             0x40U
#define GT_TYPE_FLOAT           0x60U
#define GT_TYPE_ARRAY           0x80U
#define GT_TYPE_FUNC            0xA0U
#define GT_TYPE_STRUCT          0xC0U
#define GT_TYPE_UNION           0xE0U
#define GT_TYPE_MASK            0xE0U

#define GT_GET_TYPE(x)          ((x) & GT_TYPE_MASK)
#define GT_IS_INTEGER(x)        (GT_GET_TYPE(x) == GT_TYPE_INTEGER)
#define GT_IS_POINTER(x)        (GT_GET_TYPE(x) == GT_TYPE_POINTER)
#define GT_IS_FLOAT(x)          (GT_GET_TYPE(x) == GT_TYPE_FLOAT)
#define GT_IS_ARRAY(x)          (GT_GET_TYPE(x) == GT_TYPE_ARRAY)
#define GT_IS_FUNCTION(x)       (GT_GET_TYPE(x) == GT_TYPE_FUNCTION)
#define GT_IS_STRUCT(x)         (GT_GET_TYPE(x) == GT_TYPE_STRUCT)
#define GT_IS_UNION(x)          (GT_GET_TYPE(x) == GT_TYPE_UNION)

/* Combined values for the 6502 family */
#define GT_VOID         (GT_TYPE_VOID)
#define GT_BYTE         (GT_TYPE_INT | GT_LITTLE_ENDIAN | GT_UNSIGNED | GT_SIZE_1)
#define GT_WORD         (GT_TYPE_INT | GT_LITTLE_ENDIAN | GT_UNSIGNED | GT_SIZE_2)
#define GT_DWORD        (GT_TYPE_INT | GT_LITTLE_ENDIAN | GT_UNSIGNED | GT_SIZE_4)
#define GT_DBYTE        (GT_TYPE_PTR | GT_BIG_ENDIAN    | GT_UNSIGNED | GT_SIZE_2)
#define GT_PTR          (GT_TYPE_PTR | GT_LITTLE_ENDIAN | GT_UNSIGNED | GT_SIZE_2)
#define GT_FAR_PTR      (GT_TYPE_PTR | GT_LITTLE_ENDIAN | GT_UNSIGNED | GT_SIZE_3)
#define GT_ARRAY(size)  (GT_TYPE_ARRAY | ((size) - 1))



static void FreeTypeInfo (TypeInfo* T)
/* Free a TypeInfo struct */
{
    xfree (T);
}



static void InitTypeParseData (TypeParseData* P, const StrBuf* Type,
                               unsigned ItemCount)
/* Initialize a TypeParseData structure */
{
    P->Info      = xmalloc (sizeof (*P->Info) - sizeof (P->Info->Data[0]) +
                            ItemCount * sizeof (P->Info->Data[0]));
    P->ItemCount = ItemCount;
    P->ItemIndex = 0;
    P->ItemData  = P->Info->Data;
    P->Type      = Type;
    P->Pos       = 0;
    P->Error     = 0;
}



static cc65_typedata* TypeFromString (TypeParseData* P)
/* Parse a type string and return a set of typedata structures. Will be called
** recursively. Will set P->Error and return NULL in case of problems.
*/
{
    cc65_typedata*  Data;
    unsigned char   B;
    unsigned        I;
    unsigned        Count;

    /* Allocate a new entry */
    if (P->ItemIndex >= P->ItemCount) {
        P->Error = 1;
        return 0;
    }
    Data = &P->ItemData[P->ItemIndex++];

    /* Assume no following node */
    Data->next = 0;

    /* Get the next char, then skip it */
    if (P->Pos >= SB_GetLen (P->Type)) {
        P->Error = 1;
        return 0;
    }
    B = SB_At (P->Type, P->Pos++);

    switch (B) {

        /* We only handle those that are currently in use */
        case GT_VOID:
            Data->what = CC65_TYPE_VOID;
            Data->size = 0;
            break;

        case GT_BYTE:
            Data->what = CC65_TYPE_BYTE;
            Data->size = GT_GET_SIZE (B);
            break;

        case GT_WORD:
            Data->what = CC65_TYPE_WORD;
            Data->size = GT_GET_SIZE (B);
            break;

        case GT_DWORD:
            Data->what = CC65_TYPE_DWORD;
            Data->size = GT_GET_SIZE (B);
            break;

        case GT_DBYTE:
            Data->what = CC65_TYPE_DBYTE;
            Data->size = GT_GET_SIZE (B);
            break;

        case GT_PTR:
            Data->what = CC65_TYPE_PTR;
            Data->data.ptr.ind_type = TypeFromString (P);
            Data->size = GT_GET_SIZE (B);
            break;

        case GT_FAR_PTR:
            Data->what = CC65_TYPE_FARPTR;
            Data->data.ptr.ind_type = TypeFromString (P);
            Data->size = GT_GET_SIZE (B);
            break;

        default:
            if (GT_GET_TYPE (B) == GT_TYPE_ARRAY) {
                Count = 0;
                I = GT_GET_SIZE (B);
                if (I == 0 || I > 4) {
                    P->Error = 1;
                    break;
                }
                P->Pos += I;
                if (P->Pos > SB_GetLen (P->Type)) {
                    P->Error = 1;
                    break;
                }
                while (I) {
                    Count <<= 8;
                    Count |= (unsigned char)  SB_At (P->Type, P->Pos - I);
                    --I;
                }
                Data->what = CC65_TYPE_ARRAY;
                Data->data.array.ele_count = Count;
                Data->data.array.ele_type = TypeFromString (P);
                if (!P->Error) {
                    Data->size = Data->data.array.ele_count *
                                 Data->data.array.ele_type->size;
                }
            } else {
                /* OOPS! */
                P->Error = 1;
            }
            break;
    }

    /* Return our structure or NULL in case of errors */
    return P->Error? 0 : Data;
}




static TypeInfo* ParseTypeString (InputData* D, StrBuf* Type)
/* Check if the string T contains a valid type string. Convert it from readable
** to binary. Calculate how many cc65_typedata structures are necessary when it
** is converted. Convert the string into a set of cc65_typedata structures and
** return them.
*/
{
    unsigned        I;
    unsigned        Count;
    const char*     A;
    char*           B;
    TypeParseData   P;


    /* The length must not be zero and divideable by two */
    unsigned Length = SB_GetLen (Type);
    if (Length < 2 || (Length & 0x01) != 0) {
        ParseError (D, CC65_ERROR, "Type value has invalid length");
        return 0;
    }

    /* The string must consist completely of hex digit chars */
    A = SB_GetConstBuf (Type);
    for (I = 0; I < Length; ++I) {
        if (!isxdigit (A[I])) {
            ParseError (D, CC65_ERROR, "Type value contains invalid characters");
            return 0;
        }
    }

    /* Convert the type to binary */
    B = SB_GetBuf (Type);
    while (A < SB_GetConstBuf (Type) + Length) {
        /* Since we know, there are only hex digits, there can't be any errors */
        *B++ = (HexValue (A[0]) << 4) | HexValue (A[1]);
        A += 2;
    }
    Type->Len = (Length /= 2);

    /* Get a pointer to the type data, then count the number of cc65_typedata
    ** items needed.
    */
    A = SB_GetConstBuf (Type);
    Count = 0;
    I = 0;
    while (I < Length) {
        switch (A[I]) {
            /* We only handle those that are currently in use */
            case GT_VOID:
            case GT_BYTE:
            case GT_WORD:
            case GT_DWORD:
            case GT_DBYTE:
            case GT_PTR:
            case GT_FAR_PTR:
                ++Count;
                ++I;
                break;

            default:
                if (GT_GET_TYPE (A[I]) == GT_TYPE_ARRAY) {
                    ++Count;
                    I += GT_GET_SIZE (A[I]) + 1;
                } else {
                    /* Unknown type in type string */
                    ParseError (D, CC65_ERROR, "Unkown type in type value");
                    return 0;
                }
                break;
        }
    }

    /* Check that we're even */
    if (I != Length) {
        ParseError (D, CC65_ERROR, "Syntax error in type in type value");
        return 0;
    }

    /* Initialize the data structure for parsing the type string */
    InitTypeParseData (&P, Type, Count);

    /* Parse the type string and check for errors */
    if (TypeFromString (&P) == 0 || P.ItemCount != P.ItemIndex) {
        ParseError (D, CC65_ERROR, "Error parsing the type value");
        FreeTypeInfo (P.Info);
        return 0;
    }

    /* Return the result of the parse operation */
    return P.Info;
}



/*****************************************************************************/
/*                               SpanInfoList                                */
/*****************************************************************************/



static void InitSpanInfoList (SpanInfoList* L)
/* Initialize a span info list */
{
    L->Count = 0;
    L->List  = 0;
}



static void CreateSpanInfoList (SpanInfoList* L, Collection* SpanInfos)
/* Create a SpanInfoList from a Collection with span infos. The collection
** must be sorted by ascending start addresses.
*/
{
    unsigned I, J;
    SpanInfo* S;
    SpanInfoListEntry* List;
    unsigned StartIndex;
    cc65_addr Start;
    cc65_addr Addr;
    cc65_addr End;

    /* Initialize and check if there's something to do */
    L->Count = 0;
    L->List  = 0;
    if (CollCount (SpanInfos) == 0) {
        /* No entries */
        return;
    }

    /* Step 1: Determine the number of unique address entries needed */
    S = CollAt (SpanInfos, 0);
    L->Count += (S->End - S->Start) + 1;
    End = S->End;
    for (I = 1; I < CollCount (SpanInfos); ++I) {

        /* Get next entry */
        S = CollAt (SpanInfos, I);

        /* Check for additional unique addresses in this span info */
        if (S->Start > End) {
            L->Count += (S->End - S->Start) + 1;
            End = S->End;
        } else if (S->End > End) {
            L->Count += (S->End - End);
            End = S->End;
        }

    }

    /* Step 2: Allocate memory and initialize it */
    L->List = List = xmalloc (L->Count * sizeof (*List));
    for (I = 0; I < L->Count; ++I) {
        List[I].Count = 0;
        List[I].Data  = 0;
    }

    /* Step 3: Determine the number of entries per unique address */
    List = L->List;
    S = CollAt (SpanInfos, 0);
    StartIndex = 0;
    Start = S->Start;
    End = S->End;
    for (J = StartIndex, Addr = S->Start; Addr <= S->End; ++J, ++Addr) {
        List[J].Addr = Addr;
        ++List[J].Count;
    }
    for (I = 1; I < CollCount (SpanInfos); ++I) {

        /* Get next entry */
        S = CollAt (SpanInfos, I);

        /* Determine the start index of the next range. Line infos are sorted
        ** by ascending start address, so the start address of the next entry
        ** is always larger than the previous one - we don't need to check
        ** that.
        */
        if (S->Start <= End) {
            /* Range starts within out already known linear range */
            StartIndex += (unsigned) (S->Start - Start);
            Start = S->Start;
            if (S->End > End) {
                End = S->End;
            }
        } else {
            /* Range starts after the already known */
            StartIndex += (unsigned) (End - Start) + 1;
            Start = S->Start;
            End = S->End;
        }
        for (J = StartIndex, Addr = S->Start; Addr <= S->End; ++J, ++Addr) {
            List[J].Addr = Addr;
            ++List[J].Count;
        }
    }

    /* Step 4: Allocate memory for the indirect tables */
    for (I = 0, List = L->List; I < L->Count; ++I, ++List) {

        /* For a count of 1, we store the pointer to the lineinfo for this
        ** address in the Data pointer directly. For counts > 1, we allocate
        ** an array of pointers and reset the counter, so we can use it as
        ** an index later. This is dangerous programming since it disables
        ** all possible checks!
        */
        if (List->Count > 1) {
            List->Data = xmalloc (List->Count * sizeof (SpanInfo*));
            List->Count = 0;
        }
    }

    /* Step 5: Enter the data into the table */
    List = L->List;
    S = CollAt (SpanInfos, 0);
    StartIndex = 0;
    Start = S->Start;
    End = S->End;
    for (J = StartIndex, Addr = S->Start; Addr <= S->End; ++J, ++Addr) {
        assert (List[J].Addr == Addr);
        if (List[J].Count == 1 && List[J].Data == 0) {
            List[J].Data = S;
        } else {
            ((SpanInfo**) List[J].Data)[List[J].Count++] = S;
        }
    }
    for (I = 1; I < CollCount (SpanInfos); ++I) {

        /* Get next entry */
        S = CollAt (SpanInfos, I);

        /* Determine the start index of the next range. Line infos are sorted
        ** by ascending start address, so the start address of the next entry
        ** is always larger than the previous one - we don't need to check
        ** that.
        */
        if (S->Start <= End) {
            /* Range starts within out already known linear range */
            StartIndex += (unsigned) (S->Start - Start);
            Start = S->Start;
            if (S->End > End) {
                End = S->End;
            }
        } else {
            /* Range starts after the already known */
            StartIndex += (unsigned) (End - Start) + 1;
            Start = S->Start;
            End = S->End;
        }
        for (J = StartIndex, Addr = S->Start; Addr <= S->End; ++J, ++Addr) {
            assert (List[J].Addr == Addr);
            if (List[J].Count == 1 && List[J].Data == 0) {
                List[J].Data = S;
            } else {
                ((SpanInfo**) List[J].Data)[List[J].Count++] = S;
            }
        }
    }
}



static void DoneSpanInfoList (SpanInfoList* L)
/* Delete the contents of a span info list */
{
    unsigned I;

    /* Delete the span info and the indirect data */
    for (I = 0; I < L->Count; ++I) {

        /* Get a pointer to the entry */
        SpanInfoListEntry* E = &L->List[I];

        /* Check for indirect memory */
        if (E->Count > 1) {
            /* SpanInfo addressed indirectly */
            xfree (E->Data);
        }
    }

    /* Delete the list */
    xfree (L->List);
}



/*****************************************************************************/
/*                                Debug info                                 */
/*****************************************************************************/



static DbgInfo* NewDbgInfo (const char* FileName)
/* Create a new DbgInfo struct and return it */
{
    /* Get the length of the name */
    unsigned Len = strlen (FileName);

    /* Allocate memory */
    DbgInfo* Info = xmalloc (sizeof (DbgInfo) + Len);

    /* Initialize it */
    CollInit (&Info->CSymInfoById);
    CollInit (&Info->FileInfoById);
    CollInit (&Info->LibInfoById);
    CollInit (&Info->LineInfoById);
    CollInit (&Info->ModInfoById);
    CollInit (&Info->ScopeInfoById);
    CollInit (&Info->SegInfoById);
    CollInit (&Info->SpanInfoById);
    CollInit (&Info->SymInfoById);
    CollInit (&Info->TypeInfoById);

    CollInit (&Info->CSymFuncByName);
    CollInit (&Info->FileInfoByName);
    CollInit (&Info->ModInfoByName);
    CollInit (&Info->ScopeInfoByName);
    CollInit (&Info->SegInfoByName);
    CollInit (&Info->SymInfoByName);
    CollInit (&Info->SymInfoByVal);

    InitSpanInfoList (&Info->SpanInfoByAddr);

    Info->MemUsage     = 0;
    Info->MajorVersion = 0;
    Info->MinorVersion = 0;
    memcpy (&Info->FileName, FileName, Len+1);

    /* Return it */
    return Info;
}



static void FreeDbgInfo (DbgInfo* Info)
/* Free a DbgInfo struct */
{
    unsigned I;

    /* First, free the items in the collections */
    for (I = 0; I < CollCount (&Info->CSymInfoById); ++I) {
        FreeCSymInfo (CollAt (&Info->CSymInfoById, I));
    }
    for (I = 0; I < CollCount (&Info->FileInfoById); ++I) {
        FreeFileInfo (CollAt (&Info->FileInfoById, I));
    }
    for (I = 0; I < CollCount (&Info->LibInfoById); ++I) {
        FreeLibInfo (CollAt (&Info->LibInfoById, I));
    }
    for (I = 0; I < CollCount (&Info->LineInfoById); ++I) {
        FreeLineInfo (CollAt (&Info->LineInfoById, I));
    }
    for (I = 0; I < CollCount (&Info->ModInfoById); ++I) {
        FreeModInfo (CollAt (&Info->ModInfoById, I));
    }
    for (I = 0; I < CollCount (&Info->ScopeInfoById); ++I) {
        FreeScopeInfo (CollAt (&Info->ScopeInfoById, I));
    }
    for (I = 0; I < CollCount (&Info->SegInfoById); ++I) {
        FreeSegInfo (CollAt (&Info->SegInfoById, I));
    }
    for (I = 0; I < CollCount (&Info->SpanInfoById); ++I) {
        FreeSpanInfo (CollAt (&Info->SpanInfoById, I));
    }
    for (I = 0; I < CollCount (&Info->SymInfoById); ++I) {
        FreeSymInfo (CollAt (&Info->SymInfoById, I));
    }
    for (I = 0; I < CollCount (&Info->TypeInfoById); ++I) {
        FreeTypeInfo (CollAt (&Info->TypeInfoById, I));
    }

    /* Free the memory used by the id collections */
    CollDone (&Info->CSymInfoById);
    CollDone (&Info->FileInfoById);
    CollDone (&Info->LibInfoById);
    CollDone (&Info->LineInfoById);
    CollDone (&Info->ModInfoById);
    CollDone (&Info->ScopeInfoById);
    CollDone (&Info->SegInfoById);
    CollDone (&Info->SpanInfoById);
    CollDone (&Info->SymInfoById);
    CollDone (&Info->TypeInfoById);

    /* Free the memory used by the other collections */
    CollDone (&Info->CSymFuncByName);
    CollDone (&Info->FileInfoByName);
    CollDone (&Info->ModInfoByName);
    CollDone (&Info->ScopeInfoByName);
    CollDone (&Info->SegInfoByName);
    CollDone (&Info->SymInfoByName);
    CollDone (&Info->SymInfoByVal);

    /* Free span info */
    DoneSpanInfoList (&Info->SpanInfoByAddr);

    /* Free the structure itself */
    xfree (Info);
}



/*****************************************************************************/
/*                            Scanner and parser                             */
/*****************************************************************************/



static int DigitVal (int C)
/* Return the value for a numeric digit. Return -1 if C is invalid */
{
    if (isdigit (C)) {
        return C - '0';
    } else if (isxdigit (C)) {
        return toupper (C) - 'A' + 10;
    } else {
        return -1;
    }
}



static void NextChar (InputData* D)
/* Read the next character from the input. Count lines and columns */
{
    /* Check if we've encountered EOF before */
    if (D->C >= 0) {
        if (D->C == '\n') {
            ++D->Line;
            D->Col = 0;
        }
        D->C = fgetc (D->F);
        ++D->Col;
    }
}



static void NextToken (InputData* D)
/* Read the next token from the input stream */
{
    static const struct KeywordEntry  {
        const char      Keyword[12];
        Token           Tok;
    } KeywordTable[] = {
        { "abs",        TOK_ABSOLUTE    },
        { "addrsize",   TOK_ADDRSIZE    },
        { "auto",       TOK_AUTO        },
        { "count",      TOK_COUNT       },
        { "csym",       TOK_CSYM        },
        { "def",        TOK_DEF         },
        { "enum",       TOK_ENUM        },
        { "equ",        TOK_EQUATE      },
        { "exp",        TOK_EXPORT      },
        { "ext",        TOK_EXTERN      },
        { "file",       TOK_FILE        },
        { "func",       TOK_FUNC        },
        { "global",     TOK_GLOBAL      },
        { "id",         TOK_ID          },
        { "imp",        TOK_IMPORT      },
        { "info",       TOK_INFO        },
        { "lab",        TOK_LABEL       },
        { "lib",        TOK_LIBRARY     },
        { "line",       TOK_LINE        },
        { "long",       TOK_LONG        },
        { "major",      TOK_MAJOR       },
        { "minor",      TOK_MINOR       },
        { "mod",        TOK_MODULE      },
        { "mtime",      TOK_MTIME       },
        { "name",       TOK_NAME        },
        { "offs",       TOK_OFFS        },
        { "oname",      TOK_OUTPUTNAME  },
        { "ooffs",      TOK_OUTPUTOFFS  },
        { "parent",     TOK_PARENT      },
        { "ref",        TOK_REF         },
        { "reg",        TOK_REGISTER    },
        { "ro",         TOK_RO          },
        { "rw",         TOK_RW          },
        { "sc",         TOK_SC          },
        { "scope",      TOK_SCOPE       },
        { "seg",        TOK_SEGMENT     },
        { "size",       TOK_SIZE        },
        { "span",       TOK_SPAN        },
        { "start",      TOK_START       },
        { "static",     TOK_STATIC      },
        { "struct",     TOK_STRUCT      },
        { "sym",        TOK_SYM         },
        { "type",       TOK_TYPE        },
        { "val",        TOK_VALUE       },
        { "var",        TOK_VAR         },
        { "version",    TOK_VERSION     },
        { "zp",         TOK_ZEROPAGE    },
    };


    /* Skip whitespace */
    while (D->C == ' ' || D->C == '\t' || D->C == '\r') {
        NextChar (D);
    }

    /* Remember the current position as start of the next token */
    D->SLine = D->Line;
    D->SCol  = D->Col;

    /* Identifier? */
    if (D->C == '_' || isalpha (D->C)) {

        const struct KeywordEntry* Entry;

        /* Read the identifier */
        SB_Clear (&D->SVal);
        while (D->C == '_' || isalnum (D->C)) {
            SB_AppendChar (&D->SVal, D->C);
            NextChar (D);
        }
        SB_Terminate (&D->SVal);

        /* Search the identifier in the keyword table */
        Entry = bsearch (SB_GetConstBuf (&D->SVal),
                         KeywordTable,
                         sizeof (KeywordTable) / sizeof (KeywordTable[0]),
                         sizeof (KeywordTable[0]),
                         (int (*)(const void*, const void*)) strcmp);
        if (Entry == 0) {
            D->Tok = TOK_IDENT;
        } else {
            D->Tok = Entry->Tok;
        }
        return;
    }

    /* Number? */
    if (isdigit (D->C)) {
        int Base = 10;
        int Val;
        if (D->C == '0') {
            NextChar (D);
            if (toupper (D->C) == 'X') {
                NextChar (D);
                Base = 16;
            } else {
                Base = 8;
            }
        } else {
            Base = 10;
        }
        D->IVal = 0;
        while ((Val = DigitVal (D->C)) >= 0 && Val < Base) {
            D->IVal = D->IVal * Base + Val;
            NextChar (D);
        }
        D->Tok = TOK_INTCON;
        return;
    }

    /* Other characters */
    switch (D->C) {

        case '-':
            NextChar (D);
            D->Tok = TOK_MINUS;
            break;

        case '+':
            NextChar (D);
            D->Tok = TOK_PLUS;
            break;

        case ',':
            NextChar (D);
            D->Tok = TOK_COMMA;
            break;

        case '=':
            NextChar (D);
            D->Tok = TOK_EQUAL;
            break;

        case '\"':
            SB_Clear (&D->SVal);
            NextChar (D);
            while (1) {
                if (D->C == '\n' || D->C == EOF) {
                    ParseError (D, CC65_ERROR, "Unterminated string constant");
                    break;
                }
                if (D->C == '\"') {
                    NextChar (D);
                    break;
                }
                SB_AppendChar (&D->SVal, D->C);
                NextChar (D);
            }
            SB_Terminate (&D->SVal);
            D->Tok = TOK_STRCON;
            break;

        case '\n':
            NextChar (D);
            D->Tok = TOK_EOL;
            break;

        case EOF:
            D->Tok = TOK_EOF;
            break;

        default:
            ParseError (D, CC65_ERROR, "Invalid input character `%c'", D->C);

    }
}



static int TokenIsKeyword (Token Tok)
/* Return true if the given token is a keyword */
{
    return (Tok >= TOK_FIRST_KEYWORD && Tok <= TOK_LAST_KEYWORD);
}



static int TokenFollows (InputData* D, Token Tok, const char* Name)
/* Check for a specific token that follows. */
{
    if (D->Tok != Tok) {
        ParseError (D, CC65_ERROR, "%s expected", Name);
        SkipLine (D);
        return 0;
    } else {
        return 1;
    }
}



static int IntConstFollows (InputData* D)
/* Check for an integer constant */
{
    return TokenFollows (D, TOK_INTCON, "Integer constant");
}



static int StrConstFollows (InputData* D)
/* Check for a string literal */
{
    return TokenFollows (D, TOK_STRCON, "String literal");
}



static int Consume (InputData* D, Token Tok, const char* Name)
/* Check for a token and consume it. Return true if the token was comsumed,
** return false otherwise.
*/
{
    if (TokenFollows (D, Tok, Name)) {
        NextToken (D);
        return 1;
    } else {
        return 0;
    }
}



static int ConsumeEqual (InputData* D)
/* Consume an equal sign */
{
    return Consume (D, TOK_EQUAL, "'='");
}



static void ConsumeEOL (InputData* D)
/* Consume an end-of-line token, if we aren't at end-of-file */
{
    if (D->Tok != TOK_EOF) {
        if (D->Tok != TOK_EOL) {
            ParseError (D, CC65_ERROR, "Extra tokens in line");
            SkipLine (D);
        }
        NextToken (D);
    }
}



static void ParseCSym (InputData* D)
/* Parse a CSYM line */
{
    /* Most of the following variables are initialized with a value that is
    ** overwritten later. This is just to avoid compiler warnings.
    */
    unsigned            Id = 0;
    StrBuf              Name = STRBUF_INITIALIZER;
    int                 Offs = 0;
    cc65_csym_sc        SC = CC65_CSYM_AUTO;
    unsigned            ScopeId = 0;
    unsigned            SymId = CC65_INV_ID;
    unsigned            TypeId = CC65_INV_ID;
    CSymInfo*           S;
    enum {
        ibNone          = 0x0000,

        ibId            = 0x0001,
        ibOffs          = 0x0002,
        ibName          = 0x0004,
        ibSC            = 0x0008,
        ibScopeId       = 0x0010,
        ibSymId         = 0x0020,
        ibType          = 0x0040,

        ibRequired      = ibId | ibName | ibSC | ibScopeId | ibType,
    } InfoBits = ibNone;

    /* Skip the CSYM token */
    NextToken (D);

    /* More stuff follows */
    while (1) {

        Token Tok;

        /* Something we know? */
        if (D->Tok != TOK_ID            && D->Tok != TOK_NAME           &&
            D->Tok != TOK_OFFS          && D->Tok != TOK_SC             &&
            D->Tok != TOK_SCOPE         && D->Tok != TOK_SYM            &&
            D->Tok != TOK_TYPE) {

            /* Try smart error recovery */
            if (D->Tok == TOK_IDENT || TokenIsKeyword (D->Tok)) {
                UnknownKeyword (D);
                continue;
            }

            /* Done */
            break;
        }

        /* Remember the token, skip it, check for equal */
        Tok = D->Tok;
        NextToken (D);
        if (!ConsumeEqual (D)) {
            goto ErrorExit;
        }

        /* Check what the token was */
        switch (Tok) {

            case TOK_ID:
                if (!IntConstFollows (D)) {
                    goto ErrorExit;
                }
                Id = D->IVal;
                NextToken (D);
                InfoBits |= ibId;
                break;

            case TOK_NAME:
                if (!StrConstFollows (D)) {
                    goto ErrorExit;
                }
                SB_Copy (&Name, &D->SVal);
                SB_Terminate (&Name);
                InfoBits |= ibName;
                NextToken (D);
                break;

            case TOK_OFFS:
                Offs = 1;
                if (D->Tok == TOK_MINUS) {
                    Offs = -1;
                    NextToken (D);
                }
                if (!IntConstFollows (D)) {
                    goto ErrorExit;
                }
                Offs *= (int) D->IVal;
                InfoBits |= ibOffs;
                NextToken (D);
                break;

            case TOK_SC:
                switch (D->Tok) {
                    case TOK_AUTO:      SC = CC65_CSYM_AUTO;    break;
                    case TOK_EXTERN:    SC = CC65_CSYM_EXTERN;  break;
                    case TOK_REGISTER:  SC = CC65_CSYM_REG;     break;
                    case TOK_STATIC:    SC = CC65_CSYM_STATIC;  break;
                    default:
                        ParseError (D, CC65_ERROR, "Invalid storage class token");
                        break;
                }
                InfoBits |= ibSC;
                NextToken (D);
                break;

            case TOK_SCOPE:
                if (!IntConstFollows (D)) {
                    goto ErrorExit;
                }
                ScopeId = D->IVal;
                NextToken (D);
                InfoBits |= ibScopeId;
                break;

            case TOK_SYM:
                if (!IntConstFollows (D)) {
                    goto ErrorExit;
                }
                SymId = D->IVal;
                NextToken (D);
                InfoBits |= ibSymId;
                break;

            case TOK_TYPE:
                if (!IntConstFollows (D)) {
                    goto ErrorExit;
                }
                TypeId = D->IVal;
                NextToken (D);
                InfoBits |= ibType;
                break;

            default:
                /* NOTREACHED */
                UnexpectedToken (D);
                goto ErrorExit;

        }

        /* Comma or done */
        if (D->Tok != TOK_COMMA) {
            break;
        }
        NextToken (D);
    }

    /* Check for end of line */
    if (D->Tok != TOK_EOL && D->Tok != TOK_EOF) {
        UnexpectedToken (D);
        SkipLine (D);
        goto ErrorExit;
    }

    /* Check for required and/or matched information */
    if ((InfoBits & ibRequired) != ibRequired) {
        ParseError (D, CC65_ERROR, "Required attributes missing");
        goto ErrorExit;
    }

    /* Symbol only valid if storage class not auto */
    if (((InfoBits & ibSymId) != 0) && (SC == CC65_CSYM_AUTO)) {
        ParseError (D, CC65_ERROR, "Only non auto symbols can have a symbol attached");
        goto ErrorExit;
    }

    /* Create the symbol info */
    S = NewCSymInfo (&Name);
    S->Id         = Id;
    S->Kind       = CC65_CSYM_VAR;
    S->SC         = SC;
    S->Offs       = Offs;
    S->Sym.Id     = SymId;
    S->Type.Id    = TypeId;
    S->Scope.Id   = ScopeId;

    /* Remember it */
    CollReplaceExpand (&D->Info->CSymInfoById, S, Id);

ErrorExit:
    /* Entry point in case of errors */
    SB_Done (&Name);
    return;
}



static void ParseFile (InputData* D)
/* Parse a FILE line */
{
    unsigned      Id = 0;
    unsigned long Size = 0;
    unsigned long MTime = 0;
    Collection    ModIds = COLLECTION_INITIALIZER;
    StrBuf        Name = STRBUF_INITIALIZER;
    FileInfo*     F;
    enum {
        ibNone      = 0x00,
        ibId        = 0x01,
        ibName      = 0x02,
        ibSize      = 0x04,
        ibMTime     = 0x08,
        ibModId     = 0x10,
        ibRequired  = ibId | ibName | ibSize | ibMTime | ibModId,
    } InfoBits = ibNone;

    /* Skip the FILE token */
    NextToken (D);

    /* More stuff follows */
    while (1) {

        Token Tok;

        /* Something we know? */
        if (D->Tok != TOK_ID            && D->Tok != TOK_MODULE         &&
            D->Tok != TOK_MTIME         && D->Tok != TOK_NAME           &&
            D->Tok != TOK_SIZE) {

            /* Try smart error recovery */
            if (D->Tok == TOK_IDENT || TokenIsKeyword (D->Tok)) {
                UnknownKeyword (D);
                continue;
            }

            /* Done */
            break;
        }

        /* Remember the token, skip it, check for equal */
        Tok = D->Tok;
        NextToken (D);
        if (!ConsumeEqual (D)) {
            goto ErrorExit;
        }

        /* Check what the token was */
        switch (Tok) {

            case TOK_ID:
                if (!IntConstFollows (D)) {
                    goto ErrorExit;
                }
                Id = D->IVal;
                InfoBits |= ibId;
                NextToken (D);
                break;

            case TOK_MTIME:
                if (!IntConstFollows (D)) {
                    goto ErrorExit;
                }
                MTime = D->IVal;
                NextToken (D);
                InfoBits |= ibMTime;
                break;

            case TOK_MODULE:
                while (1) {
                    if (!IntConstFollows (D)) {
                        goto ErrorExit;
                    }
                    CollAppendId (&ModIds, (unsigned) D->IVal);
                    NextToken (D);
                    if (D->Tok != TOK_PLUS) {
                        break;
                    }
                    NextToken (D);
                }
                InfoBits |= ibModId;
                break;

            case TOK_NAME:
                if (!StrConstFollows (D)) {
                    goto ErrorExit;
                }
                SB_Copy (&Name, &D->SVal);
                SB_Terminate (&Name);
                InfoBits |= ibName;
                NextToken (D);
                break;

            case TOK_SIZE:
                if (!IntConstFollows (D)) {
                    goto ErrorExit;
                }
                Size = D->IVal;
                NextToken (D);
                InfoBits |= ibSize;
                break;

            default:
                /* NOTREACHED */
                UnexpectedToken (D);
                goto ErrorExit;

        }

        /* Comma or done */
        if (D->Tok != TOK_COMMA) {
            break;
        }
        NextToken (D);
    }

    /* Check for end of line */
    if (D->Tok != TOK_EOL && D->Tok != TOK_EOF) {
        UnexpectedToken (D);
        SkipLine (D);
        goto ErrorExit;
    }

    /* Check for required information */
    if ((InfoBits & ibRequired) != ibRequired) {
        ParseError (D, CC65_ERROR, "Required attributes missing");
        goto ErrorExit;
    }

    /* Create the file info and remember it */
    F = NewFileInfo (&Name);
    F->Id       = Id;
    F->Size     = Size;
    F->MTime    = MTime;
    CollMove (&ModIds, &F->ModInfoByName);
    CollReplaceExpand (&D->Info->FileInfoById, F, Id);
    CollAppend (&D->Info->FileInfoByName, F);

ErrorExit:
    /* Entry point in case of errors */
    CollDone (&ModIds);
    SB_Done (&Name);
    return;
}



static void ParseInfo (InputData* D)
/* Parse an INFO line */
{
    /* Skip the INFO token */
    NextToken (D);

    /* More stuff follows */
    while (1) {

        Token Tok;

        /* Something we know? */
        if (D->Tok != TOK_CSYM          && D->Tok != TOK_FILE           &&
            D->Tok != TOK_LIBRARY       && D->Tok != TOK_LINE           &&
            D->Tok != TOK_MODULE        && D->Tok != TOK_SCOPE          &&
            D->Tok != TOK_SEGMENT       && D->Tok != TOK_SPAN           &&
            D->Tok != TOK_SYM           && D->Tok != TOK_TYPE) {

            /* Try smart error recovery */
            if (D->Tok == TOK_IDENT || TokenIsKeyword (D->Tok)) {
                UnknownKeyword (D);
                continue;
            }

            /* Done */
            break;
        }

        /* Remember the token, skip it, check for equal, check for an integer
        ** constant.
        */
        Tok = D->Tok;
        NextToken (D);
        if (!ConsumeEqual (D)) {
            goto ErrorExit;
        }
        if (!IntConstFollows (D)) {
            goto ErrorExit;
        }

        /* Check what the token was */
        switch (Tok) {

            case TOK_CSYM:
                CollGrow (&D->Info->CSymInfoById,  D->IVal);
                break;

            case TOK_FILE:
                CollGrow (&D->Info->FileInfoById,   D->IVal);
                CollGrow (&D->Info->FileInfoByName, D->IVal);
                break;

            case TOK_LIBRARY:
                CollGrow (&D->Info->LibInfoById, D->IVal);
                break;

            case TOK_LINE:
                CollGrow (&D->Info->LineInfoById, D->IVal);
                break;

            case TOK_MODULE:
                CollGrow (&D->Info->ModInfoById,   D->IVal);
                CollGrow (&D->Info->ModInfoByName, D->IVal);
                break;

            case TOK_SCOPE:
                CollGrow (&D->Info->ScopeInfoById, D->IVal);
                CollGrow (&D->Info->ScopeInfoByName, D->IVal);
                break;

            case TOK_SEGMENT:
                CollGrow (&D->Info->SegInfoById,   D->IVal);
                CollGrow (&D->Info->SegInfoByName, D->IVal);
                break;

            case TOK_SPAN:
                CollGrow (&D->Info->SpanInfoById,  D->IVal);
                break;

            case TOK_SYM:
                CollGrow (&D->Info->SymInfoById,   D->IVal);
                CollGrow (&D->Info->SymInfoByName, D->IVal);
                CollGrow (&D->Info->SymInfoByVal,  D->IVal);
                break;

            case TOK_TYPE:
                CollGrow (&D->Info->TypeInfoById,  D->IVal);
                break;

            default:
                /* NOTREACHED */
                UnexpectedToken (D);
                goto ErrorExit;

        }

        /* Skip the number */
        NextToken (D);

        /* Comma or done */
        if (D->Tok != TOK_COMMA) {
            break;
        }
        NextToken (D);
    }

    /* Check for end of line */
    if (D->Tok != TOK_EOL && D->Tok != TOK_EOF) {
        UnexpectedToken (D);
        SkipLine (D);
        goto ErrorExit;
    }

ErrorExit:
    /* Entry point in case of errors */
    return;
}



static void ParseLibrary (InputData* D)
/* Parse a LIBRARY line */
{
    unsigned      Id = 0;
    StrBuf        Name = STRBUF_INITIALIZER;
    LibInfo*      L;
    enum {
        ibNone      = 0x00,
        ibId        = 0x01,
        ibName      = 0x02,
        ibRequired  = ibId | ibName,
    } InfoBits = ibNone;

    /* Skip the LIBRARY token */
    NextToken (D);

    /* More stuff follows */
    while (1) {

        Token Tok;

        /* Something we know? */
        if (D->Tok != TOK_ID            && D->Tok != TOK_NAME) {

            /* Try smart error recovery */
            if (D->Tok == TOK_IDENT || TokenIsKeyword (D->Tok)) {
                UnknownKeyword (D);
                continue;
            }

            /* Done */
            break;
        }

        /* Remember the token, skip it, check for equal */
        Tok = D->Tok;
        NextToken (D);
        if (!ConsumeEqual (D)) {
            goto ErrorExit;
        }

        /* Check what the token was */
        switch (Tok) {

            case TOK_ID:
                if (!IntConstFollows (D)) {
                    goto ErrorExit;
                }
                Id = D->IVal;
                InfoBits |= ibId;
                NextToken (D);
                break;

            case TOK_NAME:
                if (!StrConstFollows (D)) {
                    goto ErrorExit;
                }
                SB_Copy (&Name, &D->SVal);
                SB_Terminate (&Name);
                InfoBits |= ibName;
                NextToken (D);
                break;

            default:
                /* NOTREACHED */
                UnexpectedToken (D);
                goto ErrorExit;

        }

        /* Comma or done */
        if (D->Tok != TOK_COMMA) {
            break;
        }
        NextToken (D);
    }

    /* Check for end of line */
    if (D->Tok != TOK_EOL && D->Tok != TOK_EOF) {
        UnexpectedToken (D);
        SkipLine (D);
        goto ErrorExit;
    }

    /* Check for required information */
    if ((InfoBits & ibRequired) != ibRequired) {
        ParseError (D, CC65_ERROR, "Required attributes missing");
        goto ErrorExit;
    }

    /* Create the library info and remember it */
    L = NewLibInfo (&Name);
    L->Id = Id;
    CollReplaceExpand (&D->Info->LibInfoById, L, Id);

ErrorExit:
    /* Entry point in case of errors */
    SB_Done (&Name);
    return;
}



static void ParseLine (InputData* D)
/* Parse a LINE line */
{
    unsigned        Id = CC65_INV_ID;
    unsigned        FileId = CC65_INV_ID;
    Collection      SpanIds = COLLECTION_INITIALIZER;
    cc65_line       Line = 0;
    cc65_line_type  Type = CC65_LINE_ASM;
    unsigned        Count = 0;
    LineInfo*       L;
    enum {
        ibNone      = 0x00,

        ibCount     = 0x01,
        ibFileId    = 0x02,
        ibId        = 0x04,
        ibLine      = 0x08,
        ibSpanId    = 0x20,
        ibType      = 0x40,

        ibRequired  = ibFileId | ibId | ibLine,
    } InfoBits = ibNone;

    /* Skip the LINE token */
    NextToken (D);

    /* More stuff follows */
    while (1) {

        Token Tok;

        /* Something we know? */
        if (D->Tok != TOK_COUNT   && D->Tok != TOK_FILE         &&
            D->Tok != TOK_ID      && D->Tok != TOK_LINE         &&
            D->Tok != TOK_SPAN    && D->Tok != TOK_TYPE) {

            /* Try smart error recovery */
            if (D->Tok == TOK_IDENT || TokenIsKeyword (D->Tok)) {
                UnknownKeyword (D);
                continue;
            }

            /* Done */
            break;
        }

        /* Remember the token, skip it, check for equal */
        Tok = D->Tok;
        NextToken (D);
        if (!ConsumeEqual (D)) {
            goto ErrorExit;
        }

        /* Check what the token was */
        switch (Tok) {

            case TOK_FILE:
                if (!IntConstFollows (D)) {
                    goto ErrorExit;
                }
                FileId = D->IVal;
                InfoBits |= ibFileId;
                NextToken (D);
                break;

            case TOK_ID:
                if (!IntConstFollows (D)) {
                    goto ErrorExit;
                }
                Id = D->IVal;
                InfoBits |= ibId;
                NextToken (D);
                break;

            case TOK_LINE:
                if (!IntConstFollows (D)) {
                    goto ErrorExit;
                }
                Line = (cc65_line) D->IVal;
                NextToken (D);
                InfoBits |= ibLine;
                break;

            case TOK_SPAN:
                while (1) {
                    if (!IntConstFollows (D)) {
                        goto ErrorExit;
                    }
                    CollAppendId (&SpanIds, (unsigned) D->IVal);
                    NextToken (D);
                    if (D->Tok != TOK_PLUS) {
                        break;
                    }
                    NextToken (D);
                }
                InfoBits |= ibSpanId;
                break;

            case TOK_TYPE:
                if (!IntConstFollows (D)) {
                    goto ErrorExit;
                }
                Type = (cc65_line_type) D->IVal;
                InfoBits |= ibType;
                NextToken (D);
                break;

            case TOK_COUNT:
                if (!IntConstFollows (D)) {
                    goto ErrorExit;
                }
                Count = D->IVal;
                InfoBits |= ibCount;
                NextToken (D);
                break;

            default:
                /* NOTREACHED */
                UnexpectedToken (D);
                goto ErrorExit;

        }

        /* Comma or done */
        if (D->Tok != TOK_COMMA) {
            break;
        }
        NextToken (D);
    }

    /* Check for end of line */
    if (D->Tok != TOK_EOL && D->Tok != TOK_EOF) {
        UnexpectedToken (D);
        SkipLine (D);
        goto ErrorExit;
    }

    /* Check for required information */
    if ((InfoBits & ibRequired) != ibRequired) {
        ParseError (D, CC65_ERROR, "Required attributes missing");
        goto ErrorExit;
    }

    /* Create the line info and remember it */
    L = NewLineInfo ();
    L->Id       = Id;
    L->Line     = Line;
    L->File.Id  = FileId;
    L->Type     = Type;
    L->Count    = Count;
    CollMove (&SpanIds, &L->SpanInfoList);
    CollReplaceExpand (&D->Info->LineInfoById, L, Id);

ErrorExit:
    /* Entry point in case of errors */
    CollDone (&SpanIds);
    return;
}



static void ParseModule (InputData* D)
/* Parse a MODULE line */
{
    /* Most of the following variables are initialized with a value that is
    ** overwritten later. This is just to avoid compiler warnings.
    */
    unsigned            Id = CC65_INV_ID;
    StrBuf              Name = STRBUF_INITIALIZER;
    unsigned            FileId = CC65_INV_ID;
    unsigned            LibId = CC65_INV_ID;
    ModInfo*            M;
    enum {
        ibNone          = 0x000,

        ibFileId        = 0x001,
        ibId            = 0x002,
        ibName          = 0x004,
        ibLibId         = 0x008,

        ibRequired      = ibId | ibName | ibFileId,
    } InfoBits = ibNone;

    /* Skip the MODULE token */
    NextToken (D);

    /* More stuff follows */
    while (1) {

        Token Tok;

        /* Something we know? */
        if (D->Tok != TOK_FILE          && D->Tok != TOK_ID             &&
            D->Tok != TOK_NAME          && D->Tok != TOK_LIBRARY) {

            /* Try smart error recovery */
            if (D->Tok == TOK_IDENT || TokenIsKeyword (D->Tok)) {
                UnknownKeyword (D);
                continue;
            }

            /* Done */
            break;
        }

        /* Remember the token, skip it, check for equal */
        Tok = D->Tok;
        NextToken (D);
        if (!ConsumeEqual (D)) {
            goto ErrorExit;
        }

        /* Check what the token was */
        switch (Tok) {

            case TOK_FILE:
                if (!IntConstFollows (D)) {
                    goto ErrorExit;
                }
                FileId = D->IVal;
                InfoBits |= ibFileId;
                NextToken (D);
                break;

            case TOK_ID:
                if (!IntConstFollows (D)) {
                    goto ErrorExit;
                }
                Id = D->IVal;
                InfoBits |= ibId;
                NextToken (D);
                break;

            case TOK_NAME:
                if (!StrConstFollows (D)) {
                    goto ErrorExit;
                }
                SB_Copy (&Name, &D->SVal);
                SB_Terminate (&Name);
                InfoBits |= ibName;
                NextToken (D);
                break;

            case TOK_LIBRARY:
                if (!IntConstFollows (D)) {
                    goto ErrorExit;
                }
                LibId = D->IVal;
                InfoBits |= ibLibId;
                NextToken (D);
                break;

            default:
                /* NOTREACHED */
                UnexpectedToken (D);
                goto ErrorExit;

        }

        /* Comma or done */
        if (D->Tok != TOK_COMMA) {
            break;
        }
        NextToken (D);
    }

    /* Check for end of line */
    if (D->Tok != TOK_EOL && D->Tok != TOK_EOF) {
        UnexpectedToken (D);
        SkipLine (D);
        goto ErrorExit;
    }

    /* Check for required and/or matched information */
    if ((InfoBits & ibRequired) != ibRequired) {
        ParseError (D, CC65_ERROR, "Required attributes missing");
        goto ErrorExit;
    }

    /* Create the scope info */
    M = NewModInfo (&Name);
    M->File.Id = FileId;
    M->Id      = Id;
    M->Lib.Id  = LibId;

    /* ... and remember it */
    CollReplaceExpand (&D->Info->ModInfoById, M, Id);
    CollAppend (&D->Info->ModInfoByName, M);

ErrorExit:
    /* Entry point in case of errors */
    SB_Done (&Name);
    return;
}



static void ParseScope (InputData* D)
/* Parse a SCOPE line */
{
    /* Most of the following variables are initialized with a value that is
    ** overwritten later. This is just to avoid compiler warnings.
    */
    unsigned            Id = CC65_INV_ID;
    cc65_scope_type     Type = CC65_SCOPE_MODULE;
    cc65_size           Size = 0;
    StrBuf              Name = STRBUF_INITIALIZER;
    unsigned            ModId = CC65_INV_ID;
    unsigned            ParentId = CC65_INV_ID;
    Collection          SpanIds = COLLECTION_INITIALIZER;
    unsigned            SymId = CC65_INV_ID;
    ScopeInfo*          S;
    enum {
        ibNone          = 0x000,

        ibId            = 0x001,
        ibModId         = 0x002,
        ibName          = 0x004,
        ibParentId      = 0x008,
        ibSize          = 0x010,
        ibSpanId        = 0x020,
        ibSymId         = 0x040,
        ibType          = 0x080,

        ibRequired      = ibId | ibModId | ibName,
    } InfoBits = ibNone;

    /* Skip the SCOPE token */
    NextToken (D);

    /* More stuff follows */
    while (1) {

        Token Tok;

        /* Something we know? */
        if (D->Tok != TOK_ID            && D->Tok != TOK_MODULE         &&
            D->Tok != TOK_NAME          && D->Tok != TOK_PARENT         &&
            D->Tok != TOK_SIZE          && D->Tok != TOK_SPAN           &&
            D->Tok != TOK_SYM           && D->Tok != TOK_TYPE) {

            /* Try smart error recovery */
            if (D->Tok == TOK_IDENT || TokenIsKeyword (D->Tok)) {
                UnknownKeyword (D);
                continue;
            }

            /* Done */
            break;
        }

        /* Remember the token, skip it, check for equal */
        Tok = D->Tok;
        NextToken (D);
        if (!ConsumeEqual (D)) {
            goto ErrorExit;
        }

        /* Check what the token was */
        switch (Tok) {

            case TOK_ID:
                if (!IntConstFollows (D)) {
                    goto ErrorExit;
                }
                Id = D->IVal;
                InfoBits |= ibId;
                NextToken (D);
                break;

            case TOK_MODULE:
                if (!IntConstFollows (D)) {
                    goto ErrorExit;
                }
                ModId = D->IVal;
                InfoBits |= ibModId;
                NextToken (D);
                break;

            case TOK_NAME:
                if (!StrConstFollows (D)) {
                    goto ErrorExit;
                }
                SB_Copy (&Name, &D->SVal);
                SB_Terminate (&Name);
                InfoBits |= ibName;
                NextToken (D);
                break;

            case TOK_PARENT:
                if (!IntConstFollows (D)) {
                    goto ErrorExit;
                }
                ParentId = D->IVal;
                NextToken (D);
                InfoBits |= ibParentId;
                break;

            case TOK_SIZE:
                if (!IntConstFollows (D)) {
                    goto ErrorExit;
                }
                Size = (cc65_size) D->IVal;
                InfoBits |= ibSize;
                NextToken (D);
                break;

            case TOK_SPAN:
                while (1) {
                    if (!IntConstFollows (D)) {
                        goto ErrorExit;
                    }
                    CollAppendId (&SpanIds, (unsigned) D->IVal);
                    NextToken (D);
                    if (D->Tok != TOK_PLUS) {
                        break;
                    }
                    NextToken (D);
                }
                InfoBits |= ibSpanId;
                break;

            case TOK_SYM:
                if (!IntConstFollows (D)) {
                    goto ErrorExit;
                }
                SymId = D->IVal;
                NextToken (D);
                InfoBits |= ibSymId;
                break;

            case TOK_TYPE:
                switch (D->Tok) {
                    case TOK_GLOBAL:    Type = CC65_SCOPE_GLOBAL;       break;
                    case TOK_FILE:      Type = CC65_SCOPE_MODULE;       break;
                    case TOK_SCOPE:     Type = CC65_SCOPE_SCOPE;        break;
                    case TOK_STRUCT:    Type = CC65_SCOPE_STRUCT;       break;
                    case TOK_ENUM:      Type = CC65_SCOPE_ENUM;         break;
                    default:
                        ParseError (D, CC65_ERROR,
                                    "Unknown value for attribute \"type\"");
                        SkipLine (D);
                        goto ErrorExit;
                }
                NextToken (D);
                InfoBits |= ibType;
                break;

            default:
                /* NOTREACHED */
                UnexpectedToken (D);
                goto ErrorExit;

        }

        /* Comma or done */
        if (D->Tok != TOK_COMMA) {
            break;
        }
        NextToken (D);
    }

    /* Check for end of line */
    if (D->Tok != TOK_EOL && D->Tok != TOK_EOF) {
        UnexpectedToken (D);
        SkipLine (D);
        goto ErrorExit;
    }

    /* Check for required and/or matched information */
    if ((InfoBits & ibRequired) != ibRequired) {
        ParseError (D, CC65_ERROR, "Required attributes missing");
        goto ErrorExit;
    }

    /* Create the scope info ... */
    S = NewScopeInfo (&Name);
    S->Id        = Id;
    S->Type      = Type;
    S->Size      = Size;
    S->Mod.Id    = ModId;
    S->Parent.Id = ParentId;
    S->Label.Id  = SymId;
    CollMove (&SpanIds, &S->SpanInfoList);

    /* ... and remember it */
    CollReplaceExpand (&D->Info->ScopeInfoById, S, Id);
    CollAppend (&D->Info->ScopeInfoByName, S);

ErrorExit:
    /* Entry point in case of errors */
    CollDone (&SpanIds);
    SB_Done (&Name);
    return;
}



static void ParseSegment (InputData* D)
/* Parse a SEGMENT line */
{
    unsigned        Id = 0;
    cc65_addr       Start = 0;
    cc65_addr       Size = 0;
    StrBuf          Name = STRBUF_INITIALIZER;
    StrBuf          OutputName = STRBUF_INITIALIZER;
    unsigned long   OutputOffs = 0;
    SegInfo*        S;
    enum {
        ibNone      = 0x000,

        ibAddrSize  = 0x001,
        ibId        = 0x002,
        ibOutputName= 0x004,
        ibOutputOffs= 0x008,
        ibName      = 0x010,
        ibSize      = 0x020,
        ibStart     = 0x040,
        ibType      = 0x080,

        ibRequired  = ibId | ibName | ibStart | ibSize | ibAddrSize | ibType,
    } InfoBits = ibNone;

    /* Skip the SEGMENT token */
    NextToken (D);

    /* More stuff follows */
    while (1) {

        Token Tok;

        /* Something we know? */
        if (D->Tok != TOK_ADDRSIZE      && D->Tok != TOK_ID         &&
            D->Tok != TOK_NAME          && D->Tok != TOK_OUTPUTNAME &&
            D->Tok != TOK_OUTPUTOFFS    && D->Tok != TOK_SIZE       &&
            D->Tok != TOK_START         && D->Tok != TOK_TYPE) {

            /* Try smart error recovery */
            if (D->Tok == TOK_IDENT || TokenIsKeyword (D->Tok)) {
                UnknownKeyword (D);
                continue;
            }
            /* Done */
            break;
        }

        /* Remember the token, skip it, check for equal */
        Tok = D->Tok;
        NextToken (D);
        if (!ConsumeEqual (D)) {
            goto ErrorExit;
        }

        /* Check what the token was */
        switch (Tok) {

            case TOK_ADDRSIZE:
                NextToken (D);
                InfoBits |= ibAddrSize;
                break;

            case TOK_ID:
                if (!IntConstFollows (D)) {
                    goto ErrorExit;
                }
                Id = D->IVal;
                InfoBits |= ibId;
                NextToken (D);
                break;

            case TOK_NAME:
                if (!StrConstFollows (D)) {
                    goto ErrorExit;
                }
                SB_Copy (&Name, &D->SVal);
                SB_Terminate (&Name);
                InfoBits |= ibName;
                NextToken (D);
                break;

            case TOK_OUTPUTNAME:
                if (!StrConstFollows (D)) {
                    goto ErrorExit;
                }
                SB_Copy (&OutputName, &D->SVal);
                SB_Terminate (&OutputName);
                InfoBits |= ibOutputName;
                NextToken (D);
                break;

            case TOK_OUTPUTOFFS:
                if (!IntConstFollows (D)) {
                    goto ErrorExit;
                }
                OutputOffs = D->IVal;
                NextToken (D);
                InfoBits |= ibOutputOffs;
                break;

            case TOK_SIZE:
                if (!IntConstFollows (D)) {
                    goto ErrorExit;
                }
                Size = D->IVal;
                NextToken (D);
                InfoBits |= ibSize;
                break;

            case TOK_START:
                if (!IntConstFollows (D)) {
                    goto ErrorExit;
                }
                Start = (cc65_addr) D->IVal;
                NextToken (D);
                InfoBits |= ibStart;
                break;

            case TOK_TYPE:
                NextToken (D);
                InfoBits |= ibType;
                break;

            default:
                /* NOTREACHED */
                UnexpectedToken (D);
                goto ErrorExit;

        }

        /* Comma or done */
        if (D->Tok != TOK_COMMA) {
            break;
        }
        NextToken (D);
    }

    /* Check for end of line */
    if (D->Tok != TOK_EOL && D->Tok != TOK_EOF) {
        UnexpectedToken (D);
        SkipLine (D);
        goto ErrorExit;
    }

    /* Check for required and/or matched information */
    if ((InfoBits & ibRequired) != ibRequired) {
        ParseError (D, CC65_ERROR, "Required attributes missing");
        goto ErrorExit;
    }
    InfoBits &= (ibOutputName | ibOutputOffs);
    if (InfoBits != ibNone && InfoBits != (ibOutputName | ibOutputOffs)) {
        ParseError (D, CC65_ERROR,
                    "Attributes \"outputname\" and \"outputoffs\" must be paired");
        goto ErrorExit;
    }

    /* Fix OutputOffs if not given */
    if (InfoBits == ibNone) {
        OutputOffs = 0;
    }

    /* Create the segment info and remember it */
    S = NewSegInfo (&Name, Id, Start, Size, &OutputName, OutputOffs);
    CollReplaceExpand (&D->Info->SegInfoById, S, Id);
    CollAppend (&D->Info->SegInfoByName, S);

ErrorExit:
    /* Entry point in case of errors */
    SB_Done (&Name);
    SB_Done (&OutputName);
    return;
}



static void ParseSpan (InputData* D)
/* Parse a SPAN line */
{
    unsigned        Id = 0;
    cc65_addr       Start = 0;
    cc65_addr       Size = 0;
    unsigned        SegId = CC65_INV_ID;
    unsigned        TypeId = CC65_INV_ID;
    SpanInfo*       S;
    enum {
        ibNone      = 0x000,

        ibId        = 0x01,
        ibSegId     = 0x02,
        ibSize      = 0x04,
        ibStart     = 0x08,
        ibType      = 0x10,

        ibRequired  = ibId | ibSegId | ibSize | ibStart,
    } InfoBits = ibNone;

    /* Skip the SEGMENT token */
    NextToken (D);

    /* More stuff follows */
    while (1) {

        Token Tok;

        /* Something we know? */
        if (D->Tok != TOK_ID    && D->Tok != TOK_SEGMENT        &&
            D->Tok != TOK_SIZE  && D->Tok != TOK_START          &&
            D->Tok != TOK_TYPE) {

            /* Try smart error recovery */
            if (D->Tok == TOK_IDENT || TokenIsKeyword (D->Tok)) {
                UnknownKeyword (D);
                continue;
            }
            /* Done */
            break;
        }

        /* Remember the token, skip it, check for equal */
        Tok = D->Tok;
        NextToken (D);
        if (!ConsumeEqual (D)) {
            goto ErrorExit;
        }

        /* Check what the token was */
        switch (Tok) {

            case TOK_ID:
                if (!IntConstFollows (D)) {
                    goto ErrorExit;
                }
                Id = D->IVal;
                InfoBits |= ibId;
                NextToken (D);
                break;

            case TOK_SEGMENT:
                if (!IntConstFollows (D)) {
                    goto ErrorExit;
                }
                SegId = D->IVal;
                InfoBits |= ibSegId;
                NextToken (D);
                break;

            case TOK_SIZE:
                if (!IntConstFollows (D)) {
                    goto ErrorExit;
                }
                Size = D->IVal;
                NextToken (D);
                InfoBits |= ibSize;
                break;

            case TOK_START:
                if (!IntConstFollows (D)) {
                    goto ErrorExit;
                }
                Start = (cc65_addr) D->IVal;
                NextToken (D);
                InfoBits |= ibStart;
                break;

            case TOK_TYPE:
                if (!IntConstFollows (D)) {
                    goto ErrorExit;
                }
                TypeId = D->IVal;
                NextToken (D);
                InfoBits |= ibType;
                break;

            default:
                /* NOTREACHED */
                UnexpectedToken (D);
                goto ErrorExit;

        }

        /* Comma or done */
        if (D->Tok != TOK_COMMA) {
            break;
        }
        NextToken (D);
    }

    /* Check for end of line */
    if (D->Tok != TOK_EOL && D->Tok != TOK_EOF) {
        UnexpectedToken (D);
        SkipLine (D);
        goto ErrorExit;
    }

    /* Check for required and/or matched information */
    if ((InfoBits & ibRequired) != ibRequired) {
        ParseError (D, CC65_ERROR, "Required attributes missing");
        goto ErrorExit;
    }

    /* Create the span info and remember it */
    S = NewSpanInfo ();
    S->Id       = Id;
    S->Start    = Start;
    S->End      = Start + Size - 1;
    S->Seg.Id   = SegId;
    S->Type.Id  = TypeId;
    CollReplaceExpand (&D->Info->SpanInfoById, S, Id);

ErrorExit:
    /* Entry point in case of errors */
    return;
}



static void ParseSym (InputData* D)
/* Parse a SYM line */
{
    /* Most of the following variables are initialized with a value that is
    ** overwritten later. This is just to avoid compiler warnings.
    */
    Collection          DefLineIds = COLLECTION_INITIALIZER;
    unsigned            ExportId = CC65_INV_ID;
    unsigned            FileId = CC65_INV_ID;
    unsigned            Id = CC65_INV_ID;
    StrBuf              Name = STRBUF_INITIALIZER;
    unsigned            ParentId = CC65_INV_ID;
    Collection          RefLineIds = COLLECTION_INITIALIZER;
    unsigned            ScopeId = CC65_INV_ID;
    unsigned            SegId = CC65_INV_ID;
    cc65_size           Size = 0;
    cc65_symbol_type    Type = CC65_SYM_EQUATE;
    long                Value = 0;

    SymInfo*            S;
    enum {
        ibNone          = 0x0000,

        ibAddrSize      = 0x0001,
        ibDefLineId     = 0x0002,
        ibExportId      = 0x0004,
        ibFileId        = 0x0008,
        ibId            = 0x0010,
        ibParentId      = 0x0020,
        ibRefLineId     = 0x0040,
        ibScopeId       = 0x0080,
        ibSegId         = 0x0100,
        ibSize          = 0x0200,
        ibName          = 0x0400,
        ibType          = 0x0800,
        ibValue         = 0x1000,

        ibRequired      = ibAddrSize | ibId | ibName,
    } InfoBits = ibNone;

    /* Skip the SYM token */
    NextToken (D);

    /* More stuff follows */
    while (1) {

        Token Tok;

        /* Something we know? */
        if (D->Tok != TOK_ADDRSIZE      && D->Tok != TOK_DEF            &&
            D->Tok != TOK_EXPORT        && D->Tok != TOK_FILE           &&
            D->Tok != TOK_ID            && D->Tok != TOK_NAME           &&
            D->Tok != TOK_PARENT        && D->Tok != TOK_REF            &&
            D->Tok != TOK_SCOPE         && D->Tok != TOK_SEGMENT        &&
            D->Tok != TOK_SIZE          && D->Tok != TOK_TYPE           &&
            D->Tok != TOK_VALUE) {

            /* Try smart error recovery */
            if (D->Tok == TOK_IDENT || TokenIsKeyword (D->Tok)) {
                UnknownKeyword (D);
                continue;
            }

            /* Done */
            break;
        }

        /* Remember the token, skip it, check for equal */
        Tok = D->Tok;
        NextToken (D);
        if (!ConsumeEqual (D)) {
            goto ErrorExit;
        }

        /* Check what the token was */
        switch (Tok) {

            case TOK_ADDRSIZE:
                NextToken (D);
                InfoBits |= ibAddrSize;
                break;

            case TOK_DEF:
                while (1) {
                    if (!IntConstFollows (D)) {
                        goto ErrorExit;
                    }
                    CollAppendId (&DefLineIds, (unsigned) D->IVal);
                    NextToken (D);
                    if (D->Tok != TOK_PLUS) {
                        break;
                    }
                    NextToken (D);
                }
                InfoBits |= ibDefLineId;
                break;

            case TOK_EXPORT:
                if (!IntConstFollows (D)) {
                    goto ErrorExit;
                }
                ExportId = D->IVal;
                InfoBits |= ibExportId;
                NextToken (D);
                break;

            case TOK_FILE:
                if (!IntConstFollows (D)) {
                    goto ErrorExit;
                }
                FileId = D->IVal;
                InfoBits |= ibFileId;
                NextToken (D);
                break;

            case TOK_ID:
                if (!IntConstFollows (D)) {
                    goto ErrorExit;
                }
                Id = D->IVal;
                NextToken (D);
                InfoBits |= ibId;
                break;

            case TOK_NAME:
                if (!StrConstFollows (D)) {
                    goto ErrorExit;
                }
                SB_Copy (&Name, &D->SVal);
                SB_Terminate (&Name);
                InfoBits |= ibName;
                NextToken (D);
                break;

            case TOK_PARENT:
                if (!IntConstFollows (D)) {
                    goto ErrorExit;
                }
                ParentId = D->IVal;
                NextToken (D);
                InfoBits |= ibParentId;
                break;

            case TOK_REF:
                while (1) {
                    if (!IntConstFollows (D)) {
                        goto ErrorExit;
                    }
                    CollAppendId (&RefLineIds, (unsigned) D->IVal);
                    NextToken (D);
                    if (D->Tok != TOK_PLUS) {
                        break;
                    }
                    NextToken (D);
                }
                InfoBits |= ibRefLineId;
                break;

            case TOK_SCOPE:
                if (!IntConstFollows (D)) {
                    goto ErrorExit;
                }
                ScopeId = D->IVal;
                NextToken (D);
                InfoBits |= ibScopeId;
                break;

            case TOK_SEGMENT:
                if (!IntConstFollows (D)) {
                    goto ErrorExit;
                }
                SegId = (unsigned) D->IVal;
                InfoBits |= ibSegId;
                NextToken (D);
                break;

            case TOK_SIZE:
                if (!IntConstFollows (D)) {
                    goto ErrorExit;
                }
                Size = (cc65_size) D->IVal;
                InfoBits |= ibSize;
                NextToken (D);
                break;

            case TOK_TYPE:
                switch (D->Tok) {
                    case TOK_EQUATE:    Type = CC65_SYM_EQUATE;         break;
                    case TOK_IMPORT:    Type = CC65_SYM_IMPORT;         break;
                    case TOK_LABEL:     Type = CC65_SYM_LABEL;          break;
                    default:
                        ParseError (D, CC65_ERROR,
                                    "Unknown value for attribute \"type\"");
                        SkipLine (D);
                        goto ErrorExit;
                }
                NextToken (D);
                InfoBits |= ibType;
                break;

            case TOK_VALUE:
                if (!IntConstFollows (D)) {
                    goto ErrorExit;
                }
                Value = D->IVal;
                InfoBits |= ibValue;
                NextToken (D);
                break;

            default:
                /* NOTREACHED */
                UnexpectedToken (D);
                goto ErrorExit;

        }

        /* Comma or done */
        if (D->Tok != TOK_COMMA) {
            break;
        }
        NextToken (D);
    }

    /* Check for end of line */
    if (D->Tok != TOK_EOL && D->Tok != TOK_EOF) {
        UnexpectedToken (D);
        SkipLine (D);
        goto ErrorExit;
    }

    /* Check for required and/or matched information */
    if ((InfoBits & ibRequired) != ibRequired) {
        ParseError (D, CC65_ERROR, "Required attributes missing");
        goto ErrorExit;
    }
    if ((InfoBits & (ibScopeId | ibParentId)) == 0 ||
        (InfoBits & (ibScopeId | ibParentId)) == (ibScopeId | ibParentId)) {
        ParseError (D, CC65_ERROR, "Only one of \"parent\", \"scope\" must be specified");
        goto ErrorExit;
    }

    /* Create the symbol info */
    S = NewSymInfo (&Name);
    S->Id         = Id;
    S->Type       = Type;
    S->Value      = Value;
    S->Size       = Size;
    S->Exp.Id     = ExportId;
    S->Seg.Id     = SegId;
    S->Scope.Id   = ScopeId;
    S->Parent.Id  = ParentId;
    CollMove (&DefLineIds, &S->DefLineInfoList);
    CollMove (&RefLineIds, &S->RefLineInfoList);

    /* Remember it */
    CollReplaceExpand (&D->Info->SymInfoById, S, Id);
    CollAppend (&D->Info->SymInfoByName, S);
    CollAppend (&D->Info->SymInfoByVal, S);

ErrorExit:
    /* Entry point in case of errors */
    CollDone (&DefLineIds);
    CollDone (&RefLineIds);
    SB_Done (&Name);
    return;
}



static void ParseType (InputData* D)
/* Parse a TYPE line */
{
    /* Most of the following variables are initialized with a value that is
    ** overwritten later. This is just to avoid compiler warnings.
    */
    unsigned            Id = CC65_INV_ID;
    StrBuf              Value = STRBUF_INITIALIZER;

    TypeInfo*           T;
    enum {
        ibNone          = 0x0000,

        ibId            = 0x01,
        ibValue         = 0x02,

        ibRequired      = ibId | ibValue,
    } InfoBits = ibNone;

    /* Skip the SYM token */
    NextToken (D);

    /* More stuff follows */
    while (1) {

        Token Tok;

        /* Something we know? */
        if (D->Tok != TOK_ID    && D->Tok != TOK_VALUE) {

            /* Try smart error recovery */
            if (D->Tok == TOK_IDENT || TokenIsKeyword (D->Tok)) {
                UnknownKeyword (D);
                continue;
            }

            /* Done */
            break;
        }

        /* Remember the token, skip it, check for equal */
        Tok = D->Tok;
        NextToken (D);
        if (!ConsumeEqual (D)) {
            goto ErrorExit;
        }

        /* Check what the token was */
        switch (Tok) {

            case TOK_ID:
                if (!IntConstFollows (D)) {
                    goto ErrorExit;
                }
                Id = D->IVal;
                NextToken (D);
                InfoBits |= ibId;
                break;

            case TOK_VALUE:
                if (!StrConstFollows (D)) {
                    goto ErrorExit;
                }
                SB_Copy (&Value, &D->SVal);
                InfoBits |= ibValue;
                NextToken (D);
                break;

            default:
                /* NOTREACHED */
                UnexpectedToken (D);
                goto ErrorExit;

        }

        /* Comma or done */
        if (D->Tok != TOK_COMMA) {
            break;
        }
        NextToken (D);
    }

    /* Check for end of line */
    if (D->Tok != TOK_EOL && D->Tok != TOK_EOF) {
        UnexpectedToken (D);
        SkipLine (D);
        goto ErrorExit;
    }

    /* Check for required and/or matched information */
    if ((InfoBits & ibRequired) != ibRequired) {
        ParseError (D, CC65_ERROR, "Required attributes missing");
        goto ErrorExit;
    }

    /* Parse the type string to create the type info */
    T = ParseTypeString (D, &Value);
    if (T == 0) {
        goto ErrorExit;
    }
    T->Id       = Id;

    /* Remember it */
    CollReplaceExpand (&D->Info->TypeInfoById, T, Id);

ErrorExit:
    /* Entry point in case of errors */
    SB_Done (&Value);
    return;
}



static void ParseVersion (InputData* D)
/* Parse a VERSION line */
{
    enum {
        ibNone      = 0x00,
        ibMajor     = 0x01,
        ibMinor     = 0x02,
        ibRequired  = ibMajor | ibMinor,
    } InfoBits = ibNone;

    /* Skip the VERSION token */
    NextToken (D);

    /* More stuff follows */
    while (D->Tok != TOK_EOL && D->Tok != TOK_EOF) {

        switch (D->Tok) {

            case TOK_MAJOR:
                NextToken (D);
                if (!ConsumeEqual (D)) {
                    goto ErrorExit;
                }
                if (!IntConstFollows (D)) {
                    goto ErrorExit;
                }
                D->Info->MajorVersion = D->IVal;
                NextToken (D);
                InfoBits |= ibMajor;
                break;

            case TOK_MINOR:
                NextToken (D);
                if (!ConsumeEqual (D)) {
                    goto ErrorExit;
                }
                if (!IntConstFollows (D)) {
                    goto ErrorExit;
                }
                D->Info->MinorVersion = D->IVal;
                NextToken (D);
                InfoBits |= ibMinor;
                break;

            case TOK_IDENT:
                /* Try to skip unknown keywords that may have been added by
                ** a later version.
                */
                UnknownKeyword (D);
                break;

            default:
                UnexpectedToken (D);
                SkipLine (D);
                goto ErrorExit;
        }

        /* Comma follows before next attribute */
        if (D->Tok == TOK_COMMA) {
            NextToken (D);
        } else if (D->Tok == TOK_EOL || D->Tok == TOK_EOF) {
            break;
        } else {
            UnexpectedToken (D);
            goto ErrorExit;
        }
    }

    /* Check for required information */
    if ((InfoBits & ibRequired) != ibRequired) {
        ParseError (D, CC65_ERROR, "Required attributes missing");
        goto ErrorExit;
    }

ErrorExit:
    /* Entry point in case of errors */
    return;
}



/*****************************************************************************/
/*                              Data processing                              */
/*****************************************************************************/



static int FindCSymInfoByName (const Collection* CSymInfos, const char* Name,
                               unsigned* Index)
/* Find the C symbol info with a given file name. The function returns true if
** the name was found. In this case, Index contains the index of the first item
** that matches. If the item wasn't found, the function returns false and
** Index contains the insert position for Name.
*/
{
    /* Do a binary search */
    int Lo = 0;
    int Hi = (int) CollCount (CSymInfos) - 1;
    int Found = 0;
    while (Lo <= Hi) {

        /* Mid of range */
        int Cur = (Lo + Hi) / 2;

        /* Get item */
        const CSymInfo* CurItem = CollAt (CSymInfos, Cur);

        /* Compare */
        int Res = strcmp (CurItem->Name, Name);

        /* Found? */
        if (Res < 0) {
            Lo = Cur + 1;
        } else {
            Hi = Cur - 1;
            /* Since we may have duplicates, repeat the search until we've
            ** the first item that has a match.
            */
            if (Res == 0) {
                Found = 1;
            }
        }
    }

    /* Pass back the index. This is also the insert position */
    *Index = Lo;
    return Found;
}



static int FindFileInfoByName (const Collection* FileInfos, const char* Name,
                               unsigned* Index)
/* Find the FileInfo for a given file name. The function returns true if the
** name was found. In this case, Index contains the index of the first item
** that matches. If the item wasn't found, the function returns false and
** Index contains the insert position for Name.
*/
{
    /* Do a binary search */
    int Lo = 0;
    int Hi = (int) CollCount (FileInfos) - 1;
    int Found = 0;
    while (Lo <= Hi) {

        /* Mid of range */
        int Cur = (Lo + Hi) / 2;

        /* Get item */
        const FileInfo* CurItem = CollAt (FileInfos, Cur);

        /* Compare */
        int Res = strcmp (CurItem->Name, Name);

        /* Found? */
        if (Res < 0) {
            Lo = Cur + 1;
        } else {
            Hi = Cur - 1;
            /* Since we may have duplicates, repeat the search until we've
            ** the first item that has a match.
            */
            if (Res == 0) {
                Found = 1;
            }
        }
    }

    /* Pass back the index. This is also the insert position */
    *Index = Lo;
    return Found;
}



static SpanInfoListEntry* FindSpanInfoByAddr (const SpanInfoList* L, cc65_addr Addr)
/* Find the index of a SpanInfo for a given address. Returns 0 if no such
** SpanInfo was found.
*/
{
    /* Do a binary search */
    int Lo = 0;
    int Hi = (int) L->Count - 1;
    while (Lo <= Hi) {

        /* Mid of range */
        int Cur = (Lo + Hi) / 2;

        /* Get item */
        SpanInfoListEntry* CurItem = &L->List[Cur];

        /* Found? */
        if (CurItem->Addr > Addr) {
            Hi = Cur - 1;
        } else if (CurItem->Addr < Addr) {
            Lo = Cur + 1;
        } else {
            /* Found */
            return CurItem;
        }
    }

    /* Not found */
    return 0;
}



static LineInfo* FindLineInfoByLine (const Collection* LineInfos, cc65_line Line)
/* Find the LineInfo for a given line number. The function returns the line
** info or NULL if none was found.
*/
{
    /* Do a binary search */
    int Lo = 0;
    int Hi = (int) CollCount (LineInfos) - 1;
    while (Lo <= Hi) {

        /* Mid of range */
        int Cur = (Lo + Hi) / 2;

        /* Get item */
        LineInfo* CurItem = CollAt (LineInfos, Cur);

        /* Found? */
        if (Line > CurItem->Line) {
            Lo = Cur + 1;
        } else if (Line < CurItem->Line) {
            Hi = Cur - 1;
        } else {
            /* Found */
            return CurItem;
        }
    }

    /* Not found */
    return 0;
}



static SegInfo* FindSegInfoByName (const Collection* SegInfos, const char* Name)
/* Find the SegInfo for a given segment name. The function returns the segment
** info or NULL if none was found.
*/
{
    /* Do a binary search */
    int Lo = 0;
    int Hi = (int) CollCount (SegInfos) - 1;
    while (Lo <= Hi) {

        /* Mid of range */
        int Cur = (Lo + Hi) / 2;

        /* Get item */
        SegInfo* CurItem = CollAt (SegInfos, Cur);

        /* Compare */
        int Res = strcmp (CurItem->Name, Name);

        /* Found? */
        if (Res < 0) {
            Lo = Cur + 1;
        } else if (Res > 0) {
            Hi = Cur - 1;
        } else {
            /* Found */
            return CurItem;
        }
    }

    /* Not found */
    return 0;
}



static int FindScopeInfoByName (const Collection* ScopeInfos, const char* Name,
                                unsigned* Index)
/* Find the ScopeInfo for a given scope name. The function returns true if the
** name was found. In this case, Index contains the index of the first item
** that matches. If the item wasn't found, the function returns false and
** Index contains the insert position for Name.
*/
{
    /* Do a binary search */
    int Lo = 0;
    int Hi = (int) CollCount (ScopeInfos) - 1;
    int Found = 0;
    while (Lo <= Hi) {

        /* Mid of range */
        int Cur = (Lo + Hi) / 2;

        /* Get item */
        const ScopeInfo* CurItem = CollAt (ScopeInfos, Cur);

        /* Compare */
        int Res = strcmp (CurItem->Name, Name);

        /* Found? */
        if (Res < 0) {
            Lo = Cur + 1;
        } else {
            Hi = Cur - 1;
            /* Since we may have duplicates, repeat the search until we've
            ** the first item that has a match.
            */
            if (Res == 0) {
                Found = 1;
            }
        }
    }

    /* Pass back the index. This is also the insert position */
    *Index = Lo;
    return Found;
}



static int FindSymInfoByName (const Collection* SymInfos, const char* Name,
                              unsigned* Index)
/* Find the SymInfo for a given file name. The function returns true if the
** name was found. In this case, Index contains the index of the first item
** that matches. If the item wasn't found, the function returns false and
** Index contains the insert position for Name.
*/
{
    /* Do a binary search */
    int Lo = 0;
    int Hi = (int) CollCount (SymInfos) - 1;
    int Found = 0;
    while (Lo <= Hi) {

        /* Mid of range */
        int Cur = (Lo + Hi) / 2;

        /* Get item */
        const SymInfo* CurItem = CollAt (SymInfos, Cur);

        /* Compare */
        int Res = strcmp (CurItem->Name, Name);

        /* Found? */
        if (Res < 0) {
            Lo = Cur + 1;
        } else {
            Hi = Cur - 1;
            /* Since we may have duplicates, repeat the search until we've
            ** the first item that has a match.
            */
            if (Res == 0) {
                Found = 1;
            }
        }
    }

    /* Pass back the index. This is also the insert position */
    *Index = Lo;
    return Found;
}



static int FindSymInfoByValue (const Collection* SymInfos, long Value,
                               unsigned* Index)
/* Find the SymInfo for a given value. The function returns true if the
** value was found. In this case, Index contains the index of the first item
** that matches. If the item wasn't found, the function returns false and
** Index contains the insert position for the given value.
*/
{
    /* Do a binary search */
    int Lo = 0;
    int Hi = (int) CollCount (SymInfos) - 1;
    int Found = 0;
    while (Lo <= Hi) {

        /* Mid of range */
        int Cur = (Lo + Hi) / 2;

        /* Get item */
        SymInfo* CurItem = CollAt (SymInfos, Cur);

        /* Found? */
        if (Value > CurItem->Value) {
            Lo = Cur + 1;
        } else {
            Hi = Cur - 1;
            /* Since we may have duplicates, repeat the search until we've
            ** the first item that has a match.
            */
            if (Value == CurItem->Value) {
                Found = 1;
            }
        }
    }

    /* Pass back the index. This is also the insert position */
    *Index = Lo;
    return Found;
}



static void ProcessCSymInfo (InputData* D)
/* Postprocess c symbol infos */
{
    unsigned I;

    /* Walk over all c symbols. Resolve the ids and add the c symbols to the
    ** corresponding asm symbols.
    */
    for (I = 0; I < CollCount (&D->Info->CSymInfoById); ++I) {

        /* Get this c symbol info */
        CSymInfo* S = CollAt (&D->Info->CSymInfoById, I);

        /* Resolve the asm symbol */
        if (S->Sym.Id == CC65_INV_ID) {
            S->Sym.Info = 0;
        } else if (S->Sym.Id >= CollCount (&D->Info->SymInfoById)) {
            ParseError (D,
                        CC65_ERROR,
                        "Invalid symbol id %u for c symbol with id %u",
                        S->Sym.Id, S->Id);
            S->Sym.Info = 0;
        } else {
            S->Sym.Info = CollAt (&D->Info->SymInfoById, S->Sym.Id);

            /* For normal (=static) symbols, add a backlink to the symbol but
            ** check that there is not more than one.
            */
            if (S->SC != CC65_CSYM_AUTO && S->SC != CC65_CSYM_REG) {
                if (S->Sym.Info->CSym) {
                    ParseError (D,
                                CC65_ERROR,
                                "Asm symbol id %u has more than one C symbol attached",
                                S->Sym.Info->Id);
                    S->Sym.Info = 0;
                } else {
                    S->Sym.Info->CSym = S;
                }
            }
        }

        /* Resolve the type */
        if (S->Type.Id >= CollCount (&D->Info->TypeInfoById)) {
            ParseError (D,
                        CC65_ERROR,
                        "Invalid type id %u for c symbol with id %u",
                        S->Type.Id, S->Id);
            S->Type.Info = 0;
        } else {
            S->Type.Info = CollAt (&D->Info->TypeInfoById, S->Type.Id);
        }

        /* Resolve the scope */
        if (S->Scope.Id >= CollCount (&D->Info->ScopeInfoById)) {
            ParseError (D,
                        CC65_ERROR,
                        "Invalid scope id %u for c symbol with id %u",
                        S->Scope.Id, S->Id);
            S->Scope.Info = 0;
        } else {
            S->Scope.Info = CollAt (&D->Info->ScopeInfoById, S->Scope.Id);

            /* Add the c symbol to the list of all c symbols for this scope */
            if (S->Scope.Info->CSymInfoByName == 0) {
                S->Scope.Info->CSymInfoByName = CollNew ();
            }
            CollAppend (S->Scope.Info->CSymInfoByName, S);

            /* If the scope has an owner symbol, it's a .PROC scope. If this
            ** symbol is identical to the one attached to the C symbol, this
            ** is actuallay a C function and the scope is the matching scope.
            ** Remember the C symbol in the scope in this case.
            ** Beware: Scopes haven't been postprocessed, so we don't have a
            ** pointer but just an id.
            */
            if (S->Sym.Info && S->Scope.Info->Label.Id == S->Sym.Info->Id) {
                /* This scope is our function scope */
                S->Scope.Info->CSymFunc = S;
                /* Add it to the list of all c functions */
                CollAppend (&D->Info->CSymFuncByName, S);
            }

        }
    }

    /* Walk over all scopes and sort the c symbols by name. */
    for (I = 0; I < CollCount (&D->Info->ScopeInfoById); ++I) {

        /* Get this scope */
        ScopeInfo* S = CollAt (&D->Info->ScopeInfoById, I);

        /* Ignore scopes without C symbols */
        if (CollCount (S->CSymInfoByName) > 1) {
            /* Sort the c symbols for this scope by name */
            CollSort (S->CSymInfoByName, CompareCSymInfoByName);
        }
    }

    /* Sort the main list of all C functions by name */
    CollSort (&D->Info->CSymFuncByName, CompareCSymInfoByName);
}



static void ProcessFileInfo (InputData* D)
/* Postprocess file infos */
{
    /* Walk over all file infos and resolve the module ids */
    unsigned I;
    for (I = 0; I < CollCount (&D->Info->FileInfoById); ++I) {

        /* Get this file info */
        FileInfo* F = CollAt (&D->Info->FileInfoById, I);

        /* Resolve the module ids */
        unsigned J;
        for (J = 0; J < CollCount (&F->ModInfoByName); ++J) {

            /* Get the id of this module */
            unsigned ModId = CollIdAt (&F->ModInfoByName, J);
            if (ModId >= CollCount (&D->Info->ModInfoById)) {
                ParseError (D,
                            CC65_ERROR,
                            "Invalid module id %u for file with id %u",
                            ModId, F->Id);
                CollReplace (&F->ModInfoByName, 0, J);
            } else {

                /* Get a pointer to the module */
                ModInfo* M = CollAt (&D->Info->ModInfoById, ModId);

                /* Replace the id by the pointer */
                CollReplace (&F->ModInfoByName, M, J);

                /* Insert a backpointer into the module */
                CollAppend (&M->FileInfoByName, F);
            }
        }

        /* If we didn't have any errors, sort the modules by name */
        if (D->Errors == 0) {
            CollSort (&F->ModInfoByName, CompareModInfoByName);
        }
    }

    /* Now walk over all modules and sort the file infos by name */
    for (I = 0; I < CollCount (&D->Info->ModInfoById); ++I) {

        /* Get this module info */
        ModInfo* M = CollAt (&D->Info->ModInfoById, I);

        /* Sort the files by name */
        CollSort (&M->FileInfoByName, CompareFileInfoByName);
    }

    /* Sort the file infos by name, so we can do a binary search */
    CollSort (&D->Info->FileInfoByName, CompareFileInfoByName);
}



static void ProcessLineInfo (InputData* D)
/* Postprocess line infos */
{
    unsigned I, J;

    /* Get pointers to the collections */
    Collection* LineInfos = &D->Info->LineInfoById;
    Collection* FileInfos = &D->Info->FileInfoById;

    /* Walk over the line infos and replace the id numbers of file and segment
    ** with pointers to the actual structs. Add the line info to each file
    ** where it is defined. Resolve the spans and add backpointers to the
    ** spans.
    */
    for (I = 0; I < CollCount (LineInfos); ++I) {

        /* Get LineInfo struct */
        LineInfo* L = CollAt (LineInfos, I);

        /* Replace the file id by a pointer to the FileInfo. Add a back
        ** pointer
        */
        if (L->File.Id >= CollCount (FileInfos)) {
            ParseError (D,
                        CC65_ERROR,
                        "Invalid file id %u for line with id %u",
                        L->File.Id, L->Id);
            L->File.Info = 0;
        } else {
            L->File.Info = CollAt (FileInfos, L->File.Id);
            CollAppend (&L->File.Info->LineInfoByLine, L);
        }

        /* Resolve the spans ids */
        for (J = 0; J < CollCount (&L->SpanInfoList); ++J) {

            /* Get the id of this span */
            unsigned SpanId = CollIdAt (&L->SpanInfoList, J);
            if (SpanId >= CollCount (&D->Info->SpanInfoById)) {
                ParseError (D,
                            CC65_ERROR,
                            "Invalid span id %u for line with id %u",
                            SpanId, L->Id);
                CollReplace (&L->SpanInfoList, 0, J);
            } else {

                /* Get a pointer to the span */
                SpanInfo* SP = CollAt (&D->Info->SpanInfoById, SpanId);

                /* Replace the id by the pointer */
                CollReplace (&L->SpanInfoList, SP, J);

                /* Insert a backpointer into the span */
                if (SP->LineInfoList == 0) {
                    SP->LineInfoList = CollNew ();
                }
                CollAppend (SP->LineInfoList, L);
            }
        }
    }

    /* Walk over all files and sort the line infos for each file so we can
    ** do a binary search later.
    */
    for (I = 0; I < CollCount (FileInfos); ++I) {

        /* Get a pointer to this file info */
        FileInfo* F = CollAt (FileInfos, I);

        /* Sort the line infos for this file */
        CollSort (&F->LineInfoByLine, CompareLineInfoByLine);
    }
}



static void ProcessModInfo (InputData* D)
/* Postprocess module infos */
{
    unsigned I;

    /* Walk over all scopes and resolve the ids */
    for (I = 0; I < CollCount (&D->Info->ModInfoById); ++I) {

        /* Get this module info */
        ModInfo* M = CollAt (&D->Info->ModInfoById, I);

        /* Resolve the main file */
        if (M->File.Id >= CollCount (&D->Info->FileInfoById)) {
            ParseError (D,
                        CC65_ERROR,
                        "Invalid file id %u for module with id %u",
                        M->File.Id, M->Id);
            M->File.Info = 0;
        } else {
            M->File.Info = CollAt (&D->Info->FileInfoById, M->File.Id);
        }

        /* Resolve the library */
        if (M->Lib.Id == CC65_INV_ID) {
            M->Lib.Info = 0;
        } else if (M->Lib.Id >= CollCount (&D->Info->LibInfoById)) {
            ParseError (D,
                        CC65_ERROR,
                        "Invalid library id %u for module with id %u",
                        M->Lib.Id, M->Id);
            M->Lib.Info = 0;
        } else {
            M->Lib.Info = CollAt (&D->Info->LibInfoById, M->Lib.Id);
        }
    }

    /* Sort the collection that contains the module info by name */
    CollSort (&D->Info->ModInfoByName, CompareModInfoByName);
}



static void ProcessScopeInfo (InputData* D)
/* Postprocess scope infos */
{
    unsigned I, J;

    /* Walk over all scopes. Resolve the ids and add the scopes to the list
    ** of scopes for a module.
    */
    for (I = 0; I < CollCount (&D->Info->ScopeInfoById); ++I) {

        /* Get this scope info */
        ScopeInfo* S = CollAt (&D->Info->ScopeInfoById, I);

        /* Resolve the module */
        if (S->Mod.Id >= CollCount (&D->Info->ModInfoById)) {
            ParseError (D,
                        CC65_ERROR,
                        "Invalid module id %u for scope with id %u",
                        S->Mod.Id, S->Id);
            S->Mod.Info = 0;
        } else {
            S->Mod.Info = CollAt (&D->Info->ModInfoById, S->Mod.Id);

            /* Add the scope to the list of scopes for this module */
            CollAppend (&S->Mod.Info->ScopeInfoByName, S);

            /* If this is a main scope, add a pointer to the corresponding
            ** module.
            */
            if (S->Parent.Id == CC65_INV_ID) {
                /* No parent means main scope */
                S->Mod.Info->MainScope = S;
            }

            /* If this is the scope that implements a C function, add the
            ** function to the list of all functions in this module.
            */
            if (S->CSymFunc) {
                CollAppend (&S->Mod.Info->CSymFuncByName, S->CSymFunc);
            }
        }

        /* Resolve the parent scope */
        if (S->Parent.Id == CC65_INV_ID) {
            S->Parent.Info = 0;
        } else if (S->Parent.Id >= CollCount (&D->Info->ScopeInfoById)) {
            ParseError (D,
                        CC65_ERROR,
                        "Invalid parent scope id %u for scope with id %u",
                        S->Parent.Id, S->Id);
            S->Parent.Info = 0;
        } else {
            S->Parent.Info = CollAt (&D->Info->ScopeInfoById, S->Parent.Id);

            /* Set a backpointer in the parent */
            if (S->Parent.Info->ChildScopeList == 0) {
                S->Parent.Info->ChildScopeList = CollNew ();
            }
            CollAppend (S->Parent.Info->ChildScopeList, S);
        }

        /* Resolve the label */
        if (S->Label.Id == CC65_INV_ID) {
            S->Label.Info = 0;
        } else if (S->Label.Id >= CollCount (&D->Info->SymInfoById)) {
            ParseError (D,
                        CC65_ERROR,
                        "Invalid label id %u for scope with id %u",
                        S->Label.Id, S->Id);
            S->Label.Info = 0;
        } else {
            S->Label.Info = CollAt (&D->Info->SymInfoById, S->Label.Id);
        }

        /* Resolve the spans ids */
        for (J = 0; J < CollCount (&S->SpanInfoList); ++J) {

            /* Get the id of this span */
            unsigned SpanId = CollIdAt (&S->SpanInfoList, J);
            if (SpanId >= CollCount (&D->Info->SpanInfoById)) {
                ParseError (D,
                            CC65_ERROR,
                            "Invalid span id %u for scope with id %u",
                            SpanId, S->Id);
                CollReplace (&S->SpanInfoList, 0, J);
            } else {

                /* Get a pointer to the span */
                SpanInfo* SP = CollAt (&D->Info->SpanInfoById, SpanId);

                /* Replace the id by the pointer */
                CollReplace (&S->SpanInfoList, SP, J);

                /* Insert a backpointer into the span */
                if (SP->ScopeInfoList == 0) {
                    SP->ScopeInfoList = CollNew ();
                }
                CollAppend (SP->ScopeInfoList, S);
            }
        }
    }

    /* Walk over all modules. If a module doesn't have scopes, it wasn't
    ** compiled with debug info which is ok. If it has debug info, it must
    ** also have a main scope. If there are scopes, sort them by name. Do
    ** also sort C functions in this module by name.
    */
    for (I = 0; I < CollCount (&D->Info->ModInfoById); ++I) {

        /* Get this module */
        ModInfo* M = CollAt (&D->Info->ModInfoById, I);

        /* Ignore modules without any scopes (no debug info) */
        if (CollCount (&M->ScopeInfoByName) == 0) {
            continue;
        }

        /* Must have a main scope */
        if (M->MainScope == 0) {
            ParseError (D,
                        CC65_ERROR,
                        "Module with id %u has no main scope",
                        M->Id);
        }

        /* Sort the scopes for this module by name */
        CollSort (&M->ScopeInfoByName, CompareScopeInfoByName);

        /* Sort the C functions in this module by name */
        CollSort (&M->CSymFuncByName, CompareCSymInfoByName);
    }

    /* Sort the scope infos */
    CollSort (&D->Info->ScopeInfoByName, CompareScopeInfoByName);
}



static void ProcessSegInfo (InputData* D)
/* Postprocess segment infos */
{
    /* Sort the segment infos by name */
    CollSort (&D->Info->SegInfoByName, CompareSegInfoByName);
}



static void ProcessSpanInfo (InputData* D)
/* Postprocess span infos */
{
    unsigned I;

    /* Temporary collection with span infos sorted by address */
    Collection SpanInfoByAddr = COLLECTION_INITIALIZER;

    /* Resize the temporary collection */
    CollGrow (&SpanInfoByAddr, CollCount (&D->Info->SpanInfoById));

    /* Walk over all spans and resolve the ids */
    for (I = 0; I < CollCount (&D->Info->SpanInfoById); ++I) {

        /* Get this span info */
        SpanInfo* S = CollAt (&D->Info->SpanInfoById, I);

        /* Resolve the segment and relocate the span */
        if (S->Seg.Id >= CollCount (&D->Info->SegInfoById)) {
            ParseError (D,
                        CC65_ERROR,
                        "Invalid segment id %u for span with id %u",
                        S->Seg.Id, S->Id);
            S->Seg.Info = 0;
        } else {
            S->Seg.Info = CollAt (&D->Info->SegInfoById, S->Seg.Id);
            S->Start += S->Seg.Info->Start;
            S->End   += S->Seg.Info->Start;
        }

        /* Resolve the type if we have it */
        if (S->Type.Id == CC65_INV_ID) {
            S->Type.Info = 0;
        } else if (S->Type.Id >= CollCount (&D->Info->TypeInfoById)) {
            ParseError (D,
                        CC65_ERROR,
                        "Invalid type id %u for span with id %u",
                        S->Type.Id, S->Id);
            S->Type.Info = 0;
        } else {
            S->Type.Info = CollAt (&D->Info->TypeInfoById, S->Type.Id);
        }

        /* Append this span info to the temporary collection that is later
        ** sorted by address.
        */
        CollAppend (&SpanInfoByAddr, S);
    }

    /* Sort the collection with all span infos by address */
    CollSort (&SpanInfoByAddr, CompareSpanInfoByAddr);

    /* Create the span info list from the span info collection */
    CreateSpanInfoList (&D->Info->SpanInfoByAddr, &SpanInfoByAddr);

    /* Remove the temporary collection */
    CollDone (&SpanInfoByAddr);
}



static void ProcessSymInfo (InputData* D)
/* Postprocess symbol infos */
{
    unsigned I, J;

    /* Walk over the symbols and resolve the references */
    for (I = 0; I < CollCount (&D->Info->SymInfoById); ++I) {

        /* Get the symbol info */
        SymInfo* S = CollAt (&D->Info->SymInfoById, I);

        /* Resolve export */
        if (S->Exp.Id == CC65_INV_ID) {
            S->Exp.Info = 0;
        } else if (S->Exp.Id >= CollCount (&D->Info->SymInfoById)) {
            ParseError (D,
                        CC65_ERROR,
                        "Invalid export id %u for symbol with id %u",
                        S->Exp.Id, S->Id);
            S->Exp.Info = 0;
        } else {
            S->Exp.Info = CollAt (&D->Info->SymInfoById, S->Exp.Id);

            /* Add a backpointer, so the export knows its imports */
            if (S->Exp.Info->ImportList == 0) {
                S->Exp.Info->ImportList = CollNew ();
            }
            CollAppend (S->Exp.Info->ImportList, S);
        }

        /* Resolve segment */
        if (S->Seg.Id == CC65_INV_ID) {
            S->Seg.Info = 0;
        } else if (S->Seg.Id >= CollCount (&D->Info->SegInfoById)) {
            ParseError (D,
                        CC65_ERROR,
                        "Invalid segment id %u for symbol with id %u",
                        S->Seg.Id, S->Id);
            S->Seg.Info = 0;
        } else {
            S->Seg.Info = CollAt (&D->Info->SegInfoById, S->Seg.Id);
        }

        /* Resolve the scope */
        if (S->Scope.Id == CC65_INV_ID) {
            S->Scope.Info = 0;
        } else if (S->Scope.Id >= CollCount (&D->Info->ScopeInfoById)) {
            ParseError (D,
                        CC65_ERROR,
                        "Invalid scope id %u for symbol with id %u",
                        S->Scope.Id, S->Id);
            S->Scope.Info = 0;
        } else {
            S->Scope.Info = CollAt (&D->Info->ScopeInfoById, S->Scope.Id);

            /* Place a backpointer to the symbol in the scope */
            CollAppend (&S->Scope.Info->SymInfoByName, S);
        }

        /* Resolve the parent for cheap locals */
        if (S->Parent.Id == CC65_INV_ID) {
            S->Parent.Info = 0;
        } else if (S->Parent.Id >= CollCount (&D->Info->SymInfoById)) {
            ParseError (D,
                        CC65_ERROR,
                        "Invalid parent id %u for symbol with id %u",
                        S->Parent.Id, S->Id);
            S->Parent.Info = 0;
        } else {
            S->Parent.Info = CollAt (&D->Info->SymInfoById, S->Parent.Id);

            /* Place a backpointer to the cheap local into the parent */
            if (S->Parent.Info->CheapLocals == 0) {
                S->Parent.Info->CheapLocals = CollNew ();
            }
            CollAppend (S->Parent.Info->CheapLocals, S);
        }

        /* Resolve the line infos for the symbol definition */
        for (J = 0; J < CollCount (&S->DefLineInfoList); ++J) {

            /* Get the id of this line info */
            unsigned LineId = CollIdAt (&S->DefLineInfoList, J);
            if (LineId >= CollCount (&D->Info->LineInfoById)) {
                ParseError (D,
                            CC65_ERROR,
                            "Invalid line id %u for symbol with id %u",
                            LineId, S->Id);
                CollReplace (&S->DefLineInfoList, 0, J);
            } else {
                /* Get a pointer to the line info */
                LineInfo* LI = CollAt (&D->Info->LineInfoById, LineId);

                /* Replace the id by the pointer */
                CollReplace (&S->DefLineInfoList, LI, J);
            }
        }

        /* Resolve the line infos for symbol references */
        for (J = 0; J < CollCount (&S->RefLineInfoList); ++J) {

            /* Get the id of this line info */
            unsigned LineId = CollIdAt (&S->RefLineInfoList, J);
            if (LineId >= CollCount (&D->Info->LineInfoById)) {
                ParseError (D,
                            CC65_ERROR,
                            "Invalid line id %u for symbol with id %u",
                            LineId, S->Id);
                CollReplace (&S->RefLineInfoList, 0, J);
            } else {
                /* Get a pointer to the line info */
                LineInfo* LI = CollAt (&D->Info->LineInfoById, LineId);

                /* Replace the id by the pointer */
                CollReplace (&S->RefLineInfoList, LI, J);
            }
        }

    }

    /* Second run. Resolve scopes for cheap locals */
    for (I = 0; I < CollCount (&D->Info->SymInfoById); ++I) {

        /* Get the symbol info */
        SymInfo* S = CollAt (&D->Info->SymInfoById, I);

        /* Resolve the scope */
        if (S->Scope.Info == 0) {
            /* No scope - must have a parent */
            if (S->Parent.Info == 0) {
                ParseError (D,
                            CC65_ERROR,
                            "Symbol with id %u has no parent and no scope",
                            S->Id);
            } else if (S->Parent.Info->Scope.Info == 0) {
                ParseError (D,
                            CC65_ERROR,
                            "Symbol with id %u has parent %u without a scope",
                            S->Id, S->Parent.Info->Id);
            } else {
                S->Scope.Info = S->Parent.Info->Scope.Info;
            }
        }
    }

    /* Walk over the scopes and sort the symbols in the scope by name */
    for (I = 0; I < CollCount (&D->Info->ScopeInfoById); ++I) {

        /* Get the scope info */
        ScopeInfo* S = CollAt (&D->Info->ScopeInfoById, I);

        /* Sort the symbols in this scope by name */
        CollSort (&S->SymInfoByName, CompareSymInfoByName);
    }

    /* Sort the symbol infos */
    CollSort (&D->Info->SymInfoByName, CompareSymInfoByName);
    CollSort (&D->Info->SymInfoByVal,  CompareSymInfoByVal);
}



/*****************************************************************************/
/*                             Debug info files                              */
/*****************************************************************************/



cc65_dbginfo cc65_read_dbginfo (const char* FileName, cc65_errorfunc ErrFunc)
/* Parse the debug info file with the given name. On success, the function
** will return a pointer to an opaque cc65_dbginfo structure, that must be
** passed to the other functions in this module to retrieve information.
** errorfunc is called in case of warnings and errors. If the file cannot be
** read successfully, NULL is returned.
*/
{
    /* Data structure used to control scanning and parsing */
    InputData D = {
        0,                      /* Name of input file */
        1,                      /* Line number */
        0,                      /* Input file */
        0,                      /* Line at start of current token */
        0,                      /* Column at start of current token */
        0,                      /* Number of errors */
        0,                      /* Input file */
        ' ',                    /* Input character */
        TOK_INVALID,            /* Input token */
        0,                      /* Integer constant */
        STRBUF_INITIALIZER,     /* String constant */
        0,                      /* Function called in case of errors */
        0,                      /* Pointer to debug info */
    };
    D.FileName = FileName;
    D.Error    = ErrFunc;

    /* Open the input file */
    D.F = fopen (FileName, "rt");
    if (D.F == 0) {
        /* Cannot open */
        ParseError (&D, CC65_ERROR,
                    "Cannot open input file \"%s\": %s",
                     FileName, strerror (errno));
        return 0;
    }

    /* Create a new debug info struct */
    D.Info = NewDbgInfo (FileName);

    /* Prime the pump */
    NextToken (&D);

    /* The first line in the file must specify version information */
    if (D.Tok != TOK_VERSION) {
        ParseError (&D, CC65_ERROR,
                    "\"version\" keyword missing in first line - this is not "
                    "a valid debug info file");
        goto CloseAndExit;
    }

    /* Parse the version directive */
    ParseVersion (&D);

    /* Do several checks on the version number */
    if (D.Info->MajorVersion < VER_MAJOR) {
        ParseError (
            &D, CC65_ERROR,
            "This is an old version of the debug info format that is no "
            "longer supported. Version found = %u.%u, version supported "
            "= %u.%u",
            D.Info->MajorVersion, D.Info->MinorVersion,
            VER_MAJOR, VER_MINOR
        );
        goto CloseAndExit;
    } else if (D.Info->MajorVersion == VER_MAJOR &&
               D.Info->MinorVersion > VER_MINOR) {
        ParseError (
            &D, CC65_ERROR,
            "This is a slightly newer version of the debug info format. "
            "It might work, but you may get errors about unknown keywords "
            "and similar. Version found = %u.%u, version supported = %u.%u",
            D.Info->MajorVersion, D.Info->MinorVersion,
            VER_MAJOR, VER_MINOR
        );
    } else if (D.Info->MajorVersion > VER_MAJOR) {
        ParseError (
            &D, CC65_WARNING,
            "The format of this debug info file is newer than what we "
            "know. Will proceed but probably fail. Version found = %u.%u, "
            "version supported = %u.%u",
            D.Info->MajorVersion, D.Info->MinorVersion,
            VER_MAJOR, VER_MINOR
        );
    }
    ConsumeEOL (&D);

    /* Parse lines */
    while (D.Tok != TOK_EOF) {

        switch (D.Tok) {

            case TOK_CSYM:
                ParseCSym (&D);
                break;

            case TOK_FILE:
                ParseFile (&D);
                break;

            case TOK_INFO:
                ParseInfo (&D);
                break;

            case TOK_LIBRARY:
                ParseLibrary (&D);
                break;

            case TOK_LINE:
                ParseLine (&D);
                break;

            case TOK_MODULE:
                ParseModule (&D);
                break;

            case TOK_SCOPE:
                ParseScope (&D);
                break;

            case TOK_SEGMENT:
                ParseSegment (&D);
                break;

            case TOK_SPAN:
                ParseSpan (&D);
                break;

            case TOK_SYM:
                ParseSym (&D);
                break;

            case TOK_TYPE:
                ParseType (&D);
                break;

            case TOK_IDENT:
                /* Output a warning, then skip the line with the unknown
                ** keyword that may have been added by a later version.
                */
                ParseError (&D, CC65_WARNING,
                            "Unknown keyword \"%s\" - skipping",
                            SB_GetConstBuf (&D.SVal));

                SkipLine (&D);
                break;

            default:
                UnexpectedToken (&D);

        }

        /* EOL or EOF must follow */
        ConsumeEOL (&D);
    }

CloseAndExit:
    /* Close the file */
    fclose (D.F);

    /* Free memory allocated for SVal */
    SB_Done (&D.SVal);

    /* In case of errors, delete the debug info already allocated and
    ** return NULL
    */
    if (D.Errors > 0) {
        /* Free allocated stuff */
        FreeDbgInfo (D.Info);
        return 0;
    }

    /* We do now have all the information from the input file. Do
    ** postprocessing. Beware: Some of the following postprocessing
    ** depends on the order of the calls.
    */
    ProcessCSymInfo (&D);
    ProcessFileInfo (&D);
    ProcessLineInfo (&D);
    ProcessModInfo (&D);
    ProcessScopeInfo (&D);
    ProcessSegInfo (&D);
    ProcessSpanInfo (&D);
    ProcessSymInfo (&D);

#if DEBUG
    /* Debug output */
    DumpData (&D);
#endif

    /* Return the debug info struct that was created */
    return D.Info;
}



void cc65_free_dbginfo (cc65_dbginfo Handle)
/* Free debug information read from a file */
{
    if (Handle) {
        FreeDbgInfo ((DbgInfo*) Handle);
    }
}



/*****************************************************************************/
/*                                 C symbols                                 */
/*****************************************************************************/



const cc65_csyminfo* cc65_get_csymlist (cc65_dbginfo Handle)
/* Return a list of all c symbols */
{
    const DbgInfo*      Info;
    cc65_csyminfo*      S;
    unsigned            I;

    /* Check the parameter */
    assert (Handle != 0);

    /* The handle is actually a pointer to a debug info struct */
    Info = Handle;

    /* Allocate memory for the data structure returned to the caller */
    S = new_cc65_csyminfo (CollCount (&Info->CSymInfoById));

    /* Fill in the data */
    for (I = 0; I < CollCount (&Info->CSymInfoById); ++I) {
        /* Copy the data */
        CopyCSymInfo (S->data + I, CollAt (&Info->CSymInfoById, I));
    }

    /* Return the result */
    return S;
}



const cc65_csyminfo* cc65_csym_byid (cc65_dbginfo Handle, unsigned Id)
/* Return information about a c symbol with a specific id. The function
** returns NULL if the id is invalid (no such c symbol) and otherwise a
** cc65_csyminfo structure with one entry that contains the requested
** symbol information.
*/
{
    const DbgInfo*      Info;
    cc65_csyminfo*      S;

    /* Check the parameter */
    assert (Handle != 0);

    /* The handle is actually a pointer to a debug info struct */
    Info = Handle;

    /* Check if the id is valid */
    if (Id >= CollCount (&Info->CSymInfoById)) {
        return 0;
    }

    /* Allocate memory for the data structure returned to the caller */
    S = new_cc65_csyminfo (1);

    /* Fill in the data */
    CopyCSymInfo (S->data, CollAt (&Info->CSymInfoById, Id));

    /* Return the result */
    return S;
}



const cc65_csyminfo* cc65_cfunc_bymodule (cc65_dbginfo Handle, unsigned ModId)
/* Return the list of C functions (not symbols!) for a specific module. If
** the module id is invalid, the function will return NULL, otherwise a
** (possibly empty) c symbol list.
*/
{
    const DbgInfo*      Info;
    const ModInfo*      M;
    cc65_csyminfo*      D;
    unsigned            I;


    /* Check the parameter */
    assert (Handle != 0);

    /* The handle is actually a pointer to a debug info struct */
    Info = Handle;

    /* Check if the module id is valid */
    if (ModId >= CollCount (&Info->ModInfoById)) {
        return 0;
    }

    /* Get a pointer to the module info */
    M = CollAt (&Info->ModInfoById, ModId);

    /* Allocate memory for the data structure returned to the caller */
    D = new_cc65_csyminfo (CollCount (&M->CSymFuncByName));

    /* Fill in the data */
    for (I = 0; I < CollCount (&M->CSymFuncByName); ++I) {
        CopyCSymInfo (D->data + I, CollAt (&M->CSymFuncByName, I));
    }

    /* Return the result */
    return D;
}



const cc65_csyminfo* cc65_cfunc_byname (cc65_dbginfo Handle, const char* Name)
/* Return a list of all C functions with the given name that have a
** definition.
*/
{
    const DbgInfo*      Info;
    unsigned            Index;
    unsigned            Count;
    const CSymInfo*     S;
    cc65_csyminfo*      D;
    unsigned            I;


    /* Check the parameter */
    assert (Handle != 0);

    /* The handle is actually a pointer to a debug info struct */
    Info = Handle;

    /* Search for a function with the given name */
    if (!FindCSymInfoByName (&Info->CSymFuncByName, Name, &Index)) {
        return 0;
    }

    /* Count functions with this name */
    Count = 1;
    I = Index;
    while (1) {
        if (++I >= CollCount (&Info->CSymFuncByName)) {
            break;
        }
        S = CollAt (&Info->CSymFuncByName, I);
        if (strcmp (S->Name, Name) != 0) {
            /* Next symbol has another name */
            break;
        }
        ++Count;
    }

    /* Allocate memory for the data structure returned to the caller */
    D = new_cc65_csyminfo (Count);

    /* Fill in the data */
    for (I = 0; I < Count; ++I, ++Index) {
        CopyCSymInfo (D->data + I, CollAt (&Info->CSymFuncByName, Index));
    }

    /* Return the result */
    return D;
}



const cc65_csyminfo* cc65_csym_byscope (cc65_dbginfo Handle, unsigned ScopeId)
/* Return all C symbols for a scope. The function will return NULL if the
** given id is invalid.
*/
{
    const DbgInfo*      Info;
    const ScopeInfo*    S;
    cc65_csyminfo*      D;
    unsigned            I;


    /* Check the parameter */
    assert (Handle != 0);

    /* The handle is actually a pointer to a debug info struct */
    Info = Handle;

    /* Check if the scope id is valid */
    if (ScopeId >= CollCount (&Info->ScopeInfoById)) {
        return 0;
    }

    /* Get a pointer to the scope */
    S = CollAt (&Info->ScopeInfoById, ScopeId);

    /* Allocate memory for the data structure returned to the caller */
    D = new_cc65_csyminfo (CollCount (S->CSymInfoByName));

    /* Fill in the data */
    for (I = 0; I < CollCount (S->CSymInfoByName); ++I) {
        CopyCSymInfo (D->data + I, CollAt (S->CSymInfoByName, I));
    }

    /* Return the result */
    return D;
}



void cc65_free_csyminfo (cc65_dbginfo Handle, const cc65_csyminfo* Info)
/* Free a c symbol info record */
{
    /* Just for completeness, check the handle */
    assert (Handle != 0);

    /* Just free the memory */
    xfree ((cc65_csyminfo*) Info);
}



/*****************************************************************************/
/*                                 Libraries                                 */
/*****************************************************************************/



const cc65_libraryinfo* cc65_get_librarylist (cc65_dbginfo Handle)
/* Return a list of all libraries */
{
    const DbgInfo*      Info;
    cc65_libraryinfo*   D;
    unsigned            I;

    /* Check the parameter */
    assert (Handle != 0);

    /* The handle is actually a pointer to a debug info struct */
    Info = Handle;

    /* Allocate memory for the data structure returned to the caller */
    D = new_cc65_libraryinfo (CollCount (&Info->LibInfoById));

    /* Fill in the data */
    for (I = 0; I < CollCount (&Info->LibInfoById); ++I) {
        /* Copy the data */
        CopyLibInfo (D->data + I, CollAt (&Info->LibInfoById, I));
    }

    /* Return the result */
    return D;
}



const cc65_libraryinfo* cc65_library_byid (cc65_dbginfo Handle, unsigned Id)
/* Return information about a library with a specific id. The function
** returns NULL if the id is invalid (no such library) and otherwise a
** cc65_libraryinfo structure with one entry that contains the requested
** library information.
*/
{
    const DbgInfo*      Info;
    cc65_libraryinfo*   D;

    /* Check the parameter */
    assert (Handle != 0);

    /* The handle is actually a pointer to a debug info struct */
    Info = Handle;

    /* Check if the id is valid */
    if (Id >= CollCount (&Info->LibInfoById)) {
        return 0;
    }

    /* Allocate memory for the data structure returned to the caller */
    D = new_cc65_libraryinfo (1);

    /* Fill in the data */
    CopyLibInfo (D->data, CollAt (&Info->LibInfoById, Id));

    /* Return the result */
    return D;
}



void cc65_free_libraryinfo (cc65_dbginfo Handle, const cc65_libraryinfo* Info)
/* Free a library info record */
{
    /* Just for completeness, check the handle */
    assert (Handle != 0);

    /* Just free the memory */
    xfree ((cc65_libraryinfo*) Info);
}



/*****************************************************************************/
/*                                 Line info                                 */
/*****************************************************************************/



const cc65_lineinfo* cc65_line_byid (cc65_dbginfo Handle, unsigned Id)
/* Return information about a line with a specific id. The function
** returns NULL if the id is invalid (no such line) and otherwise a
** cc65_lineinfo structure with one entry that contains the requested
** module information.
*/
{
    const DbgInfo*      Info;
    cc65_lineinfo*      D;

    /* Check the parameter */
    assert (Handle != 0);

    /* The handle is actually a pointer to a debug info struct */
    Info = Handle;

    /* Check if the id is valid */
    if (Id >= CollCount (&Info->LineInfoById)) {
        return 0;
    }

    /* Allocate memory for the data structure returned to the caller */
    D = new_cc65_lineinfo (1);

    /* Fill in the data */
    CopyLineInfo (D->data, CollAt (&Info->LineInfoById, Id));

    /* Return the result */
    return D;
}



const cc65_lineinfo* cc65_line_bynumber (cc65_dbginfo Handle, unsigned FileId,
                                         cc65_line Line)
/* Return line information for a source file/line number combination. The
** function returns NULL if no line information was found.
*/
{
    const DbgInfo*  Info;
    const FileInfo* F;
    cc65_lineinfo*  D;
    LineInfo*       L = 0;

    /* Check the parameter */
    assert (Handle != 0);

    /* The handle is actually a pointer to a debug info struct */
    Info = Handle;

    /* Check if the source file id is valid */
    if (FileId >= CollCount (&Info->FileInfoById)) {
        return 0;
    }

    /* Get the file */
    F = CollAt (&Info->FileInfoById, FileId);

    /* Search in the file for the given line */
    L = FindLineInfoByLine (&F->LineInfoByLine, Line);

    /* Bail out if we didn't find the line */
    if (L == 0) {
        return 0;
    }

    /* Prepare the struct we will return to the caller */
    D = new_cc65_lineinfo (1);

    /* Copy the data */
    CopyLineInfo (D->data, L);

    /* Return the allocated struct */
    return D;
}



const cc65_lineinfo* cc65_line_bysource (cc65_dbginfo Handle, unsigned FileId)
/* Return line information for a source file. The function returns NULL if the
** file id is invalid.
*/
{
    const DbgInfo*  Info;
    const FileInfo* F;
    cc65_lineinfo*  D;
    unsigned        I;

    /* Check the parameter */
    assert (Handle != 0);

    /* The handle is actually a pointer to a debug info struct */
    Info = Handle;

    /* Check if the source file id is valid */
    if (FileId >= CollCount (&Info->FileInfoById)) {
        return 0;
    }

    /* Get the file */
    F = CollAt (&Info->FileInfoById, FileId);

    /* Prepare the struct we will return to the caller */
    D = new_cc65_lineinfo (CollCount (&F->LineInfoByLine));

    /* Fill in the data */
    for (I = 0; I < CollCount (&F->LineInfoByLine); ++I) {
        /* Copy the data */
        CopyLineInfo (D->data + I, CollAt (&F->LineInfoByLine, I));
    }

    /* Return the allocated struct */
    return D;
}



const cc65_lineinfo* cc65_line_bysymdef (cc65_dbginfo Handle, unsigned SymId)
/* Return line information for the definition of a symbol. The function
** returns NULL if the symbol id is invalid, otherwise a list of line infos.
*/
{
    const DbgInfo*  Info;
    const SymInfo*  S;
    cc65_lineinfo*  D;
    unsigned        I;

    /* Check the parameter */
    assert (Handle != 0);

    /* The handle is actually a pointer to a debug info struct */
    Info = Handle;

    /* Check if the symbol id is valid */
    if (SymId >= CollCount (&Info->SymInfoById)) {
        return 0;
    }

    /* Get the symbol */
    S = CollAt (&Info->SymInfoById, SymId);

    /* Prepare the struct we will return to the caller */
    D = new_cc65_lineinfo (CollCount (&S->DefLineInfoList));

    /* Fill in the data */
    for (I = 0; I < CollCount (&S->DefLineInfoList); ++I) {
        /* Copy the data */
        CopyLineInfo (D->data + I, CollAt (&S->DefLineInfoList, I));
    }

    /* Return the allocated struct */
    return D;
}



const cc65_lineinfo* cc65_line_bysymref (cc65_dbginfo Handle, unsigned SymId)
/* Return line information for all references of a symbol. The function
** returns NULL if the symbol id is invalid, otherwise a list of line infos.
*/
{
    const DbgInfo*  Info;
    const SymInfo*  S;
    cc65_lineinfo*  D;
    unsigned        I;

    /* Check the parameter */
    assert (Handle != 0);

    /* The handle is actually a pointer to a debug info struct */
    Info = Handle;

    /* Check if the symbol id is valid */
    if (SymId >= CollCount (&Info->SymInfoById)) {
        return 0;
    }

    /* Get the symbol */
    S = CollAt (&Info->SymInfoById, SymId);

    /* Prepare the struct we will return to the caller */
    D = new_cc65_lineinfo (CollCount (&S->RefLineInfoList));

    /* Fill in the data */
    for (I = 0; I < CollCount (&S->RefLineInfoList); ++I) {
        /* Copy the data */
        CopyLineInfo (D->data + I, CollAt (&S->RefLineInfoList, I));
    }

    /* Return the allocated struct */
    return D;
}



const cc65_lineinfo* cc65_line_byspan (cc65_dbginfo Handle, unsigned SpanId)
/* Return line information for a a span. The function returns NULL if the
** span id is invalid, otherwise a list of line infos.
*/
{
    const DbgInfo*  Info;
    const SpanInfo* S;
    cc65_lineinfo*  D;
    unsigned        I;

    /* Check the parameter */
    assert (Handle != 0);

    /* The handle is actually a pointer to a debug info struct */
    Info = Handle;

    /* Check if the span id is valid */
    if (SpanId >= CollCount (&Info->SpanInfoById)) {
        return 0;
    }

    /* Get the span */
    S = CollAt (&Info->SpanInfoById, SpanId);

    /* Prepare the struct we will return to the caller */
    D = new_cc65_lineinfo (CollCount (S->LineInfoList));

    /* Fill in the data. Since S->LineInfoList may be NULL, we will use the
    ** count field of the returned data struct instead.
    */
    for (I = 0; I < D->count; ++I) {
        /* Copy the data */
        CopyLineInfo (D->data + I, CollAt (S->LineInfoList, I));
    }

    /* Return the allocated struct */
    return D;
}



void cc65_free_lineinfo (cc65_dbginfo Handle, const cc65_lineinfo* Info)
/* Free line info returned by one of the other functions */
{
    /* Just for completeness, check the handle */
    assert (Handle != 0);

    /* Just free the memory */
    xfree ((cc65_lineinfo*) Info);
}



/*****************************************************************************/
/*                                  Modules                                  */
/*****************************************************************************/



const cc65_moduleinfo* cc65_get_modulelist (cc65_dbginfo Handle)
/* Return a list of all modules */
{
    const DbgInfo*      Info;
    cc65_moduleinfo*    D;
    unsigned            I;

    /* Check the parameter */
    assert (Handle != 0);

    /* The handle is actually a pointer to a debug info struct */
    Info = Handle;

    /* Allocate memory for the data structure returned to the caller */
    D = new_cc65_moduleinfo (CollCount (&Info->ModInfoById));

    /* Fill in the data */
    for (I = 0; I < CollCount (&Info->ModInfoById); ++I) {
        /* Copy the data */
        CopyModInfo (D->data + I, CollAt (&Info->ModInfoById, I));
    }

    /* Return the result */
    return D;
}



const cc65_moduleinfo* cc65_module_byid (cc65_dbginfo Handle, unsigned Id)
/* Return information about a module with a specific id. The function
** returns NULL if the id is invalid (no such module) and otherwise a
** cc65_moduleinfo structure with one entry that contains the requested
** module information.
*/
{
    const DbgInfo*      Info;
    cc65_moduleinfo*    D;

    /* Check the parameter */
    assert (Handle != 0);

    /* The handle is actually a pointer to a debug info struct */
    Info = Handle;

    /* Check if the id is valid */
    if (Id >= CollCount (&Info->ModInfoById)) {
        return 0;
    }

    /* Allocate memory for the data structure returned to the caller */
    D = new_cc65_moduleinfo (1);

    /* Fill in the data */
    CopyModInfo (D->data, CollAt (&Info->ModInfoById, Id));

    /* Return the result */
    return D;
}



void cc65_free_moduleinfo (cc65_dbginfo Handle, const cc65_moduleinfo* Info)
/* Free a module info record */
{
    /* Just for completeness, check the handle */
    assert (Handle != 0);

    /* Just free the memory */
    xfree ((cc65_moduleinfo*) Info);
}



/*****************************************************************************/
/*                                   Spans                                   */
/*****************************************************************************/



const cc65_spaninfo* cc65_get_spanlist (cc65_dbginfo Handle)
/* Return a list of all spans */
{
    const DbgInfo*      Info;
    cc65_spaninfo*      D;
    unsigned            I;

    /* Check the parameter */
    assert (Handle != 0);

    /* The handle is actually a pointer to a debug info struct */
    Info = Handle;

    /* Allocate memory for the data structure returned to the caller */
    D = new_cc65_spaninfo (CollCount (&Info->SpanInfoById));

    /* Fill in the data */
    for (I = 0; I < CollCount (&Info->SpanInfoById); ++I) {
        /* Copy the data */
        CopySpanInfo (D->data + I, CollAt (&Info->SpanInfoById, I));
    }

    /* Return the result */
    return D;
}



const cc65_spaninfo* cc65_span_byid (cc65_dbginfo Handle, unsigned Id)
/* Return information about a span with a specific id. The function
** returns NULL if the id is invalid (no such span) and otherwise a
** cc65_spaninfo structure with one entry that contains the requested
** span information.
*/
{
    const DbgInfo*      Info;
    cc65_spaninfo*      D;

    /* Check the parameter */
    assert (Handle != 0);

    /* The handle is actually a pointer to a debug info struct */
    Info = Handle;

    /* Check if the id is valid */
    if (Id >= CollCount (&Info->SpanInfoById)) {
        return 0;
    }

    /* Allocate memory for the data structure returned to the caller */
    D = new_cc65_spaninfo (1);

    /* Fill in the data */
    CopySpanInfo (D->data, CollAt (&Info->SpanInfoById, Id));

    /* Return the result */
    return D;
}



const cc65_spaninfo* cc65_span_byaddr (cc65_dbginfo Handle, unsigned long Addr)
/* Return span information for the given address. The function returns NULL
** if no spans were found for this address.
*/
{
    const DbgInfo*      Info;
    SpanInfoListEntry* E;
    cc65_spaninfo*  D = 0;

    /* Check the parameter */
    assert (Handle != 0);

    /* The handle is actually a pointer to a debug info struct */
    Info = Handle;

    /* Search for spans that cover this address */
    E = FindSpanInfoByAddr (&Info->SpanInfoByAddr, Addr);

    /* Do we have spans? */
    if (E != 0) {

        unsigned I;

        /* Prepare the struct we will return to the caller */
        D = new_cc65_spaninfo (E->Count);
        if (E->Count == 1) {
            CopySpanInfo (D->data, E->Data);
        } else {
            for (I = 0; I < D->count; ++I) {
                /* Copy data */
                CopySpanInfo (D->data + I, ((SpanInfo**) E->Data)[I]);
            }
        }
    }

    /* Return the struct we've created */
    return D;
}



const cc65_spaninfo* cc65_span_byline (cc65_dbginfo Handle, unsigned LineId)
/* Return span information for the given source line. The function returns NULL
** if the line id is invalid, otherwise the spans for this line (possibly zero).
*/
{
    const DbgInfo*      Info;
    const LineInfo*     L;
    cc65_spaninfo*      D;
    unsigned            I;

    /* Check the parameter */
    assert (Handle != 0);

    /* The handle is actually a pointer to a debug info struct */
    Info = Handle;

    /* Check if the line id is valid */
    if (LineId >= CollCount (&Info->LineInfoById)) {
        return 0;
    }

    /* Get the line with this id */
    L = CollAt (&Info->LineInfoById, LineId);

    /* Allocate memory for the data structure returned to the caller */
    D = new_cc65_spaninfo (CollCount (&L->SpanInfoList));

    /* Fill in the data */
    for (I = 0; I < CollCount (&L->SpanInfoList); ++I) {
        /* Copy the data */
        CopySpanInfo (D->data + I, CollAt (&L->SpanInfoList, I));
    }

    /* Return the result */
    return D;
}



const cc65_spaninfo* cc65_span_byscope (cc65_dbginfo Handle, unsigned ScopeId)
/* Return span information for the given scope. The function returns NULL if
** the scope id is invalid, otherwise the spans for this scope (possibly zero).
*/
{
    const DbgInfo*      Info;
    const ScopeInfo*    S;
    cc65_spaninfo*      D;
    unsigned            I;

    /* Check the parameter */
    assert (Handle != 0);

    /* The handle is actually a pointer to a debug info struct */
    Info = Handle;

    /* Check if the scope id is valid */
    if (ScopeId >= CollCount (&Info->ScopeInfoById)) {
        return 0;
    }

    /* Get the scope with this id */
    S = CollAt (&Info->ScopeInfoById, ScopeId);

    /* Allocate memory for the data structure returned to the caller */
    D = new_cc65_spaninfo (CollCount (&S->SpanInfoList));

    /* Fill in the data */
    for (I = 0; I < CollCount (&S->SpanInfoList); ++I) {
        /* Copy the data */
        CopySpanInfo (D->data + I, CollAt (&S->SpanInfoList, I));
    }

    /* Return the result */
    return D;
}



void cc65_free_spaninfo (cc65_dbginfo Handle, const cc65_spaninfo* Info)
/* Free a span info record */
{
    /* Just for completeness, check the handle */
    assert (Handle != 0);

    /* Just free the memory */
    xfree ((cc65_spaninfo*) Info);
}



/*****************************************************************************/
/*                               Source files                                */
/*****************************************************************************/



const cc65_sourceinfo* cc65_get_sourcelist (cc65_dbginfo Handle)
/* Return a list of all source files */
{
    const DbgInfo*      Info;
    cc65_sourceinfo*    D;
    unsigned            I;

    /* Check the parameter */
    assert (Handle != 0);

    /* The handle is actually a pointer to a debug info struct */
    Info = Handle;

    /* Allocate memory for the data structure returned to the caller. */
    D = new_cc65_sourceinfo (CollCount (&Info->FileInfoById));

    /* Fill in the data */
    for (I = 0; I < CollCount (&Info->FileInfoById); ++I) {
        /* Copy the data */
        CopyFileInfo (D->data + I, CollAt (&Info->FileInfoById, I));
    }

    /* Return the result */
    return D;
}



const cc65_sourceinfo* cc65_source_byid (cc65_dbginfo Handle, unsigned Id)
/* Return information about a source file with a specific id. The function
** returns NULL if the id is invalid (no such source file) and otherwise a
** cc65_sourceinfo structure with one entry that contains the requested
** source file information.
*/
{
    const DbgInfo*      Info;
    cc65_sourceinfo*    D;

    /* Check the parameter */
    assert (Handle != 0);

    /* The handle is actually a pointer to a debug info struct */
    Info = Handle;

    /* Check if the id is valid */
    if (Id >= CollCount (&Info->FileInfoById)) {
        return 0;
    }

    /* Allocate memory for the data structure returned to the caller */
    D = new_cc65_sourceinfo (1);

    /* Fill in the data */
    CopyFileInfo (D->data, CollAt (&Info->FileInfoById, Id));

    /* Return the result */
    return D;
}




const cc65_sourceinfo* cc65_source_bymodule (cc65_dbginfo Handle, unsigned Id)
/* Return information about the source files used to build a module. The
** function returns NULL if the module id is invalid (no such module) and
** otherwise a cc65_sourceinfo structure with one entry per source file.
*/
{
    const DbgInfo*      Info;
    const ModInfo*      M;
    cc65_sourceinfo*    D;
    unsigned            I;

    /* Check the parameter */
    assert (Handle != 0);

    /* The handle is actually a pointer to a debug info struct */
    Info = Handle;

    /* Check if the module id is valid */
    if (Id >= CollCount (&Info->ModInfoById)) {
        return 0;
    }

    /* Get a pointer to the module info */
    M = CollAt (&Info->ModInfoById, Id);

    /* Allocate memory for the data structure returned to the caller */
    D = new_cc65_sourceinfo (CollCount (&M->FileInfoByName));

    /* Fill in the data */
    for (I = 0; I < CollCount (&M->FileInfoByName); ++I) {
        CopyFileInfo (D->data + I, CollAt (&M->FileInfoByName, I));
    }

    /* Return the result */
    return D;
}



void cc65_free_sourceinfo (cc65_dbginfo Handle, const cc65_sourceinfo* Info)
/* Free a source info record */
{
    /* Just for completeness, check the handle */
    assert (Handle != 0);

    /* Free the memory */
    xfree ((cc65_sourceinfo*) Info);
}



/*****************************************************************************/
/*                                  Scopes                                   */
/*****************************************************************************/



const cc65_scopeinfo* cc65_get_scopelist (cc65_dbginfo Handle)
/* Return a list of all scopes in the debug information */
{
    const DbgInfo*      Info;
    cc65_scopeinfo*     D;
    unsigned            I;

    /* Check the parameter */
    assert (Handle != 0);

    /* The handle is actually a pointer to a debug info struct */
    Info = Handle;

    /* Allocate memory for the data structure returned to the caller */
    D = new_cc65_scopeinfo (CollCount (&Info->ScopeInfoById));

    /* Fill in the data */
    for (I = 0; I < CollCount (&Info->ScopeInfoById); ++I) {
        /* Copy the data */
        CopyScopeInfo (D->data + I, CollAt (&Info->ScopeInfoById, I));
    }

    /* Return the result */
    return D;
}



const cc65_scopeinfo* cc65_scope_byid (cc65_dbginfo Handle, unsigned Id)
/* Return the scope with a given id. The function returns NULL if no scope
** with this id was found.
*/
{
    const DbgInfo*      Info;
    cc65_scopeinfo*     D;

    /* Check the parameter */
    assert (Handle != 0);

    /* The handle is actually a pointer to a debug info struct */
    Info = Handle;

    /* Check if the id is valid */
    if (Id >= CollCount (&Info->ScopeInfoById)) {
        return 0;
    }

    /* Allocate memory for the data structure returned to the caller */
    D = new_cc65_scopeinfo (1);

    /* Fill in the data */
    CopyScopeInfo (D->data, CollAt (&Info->ScopeInfoById, Id));

    /* Return the result */
    return D;
}



const cc65_scopeinfo* cc65_scope_bymodule (cc65_dbginfo Handle, unsigned ModId)
/* Return the list of scopes for one module. The function returns NULL if no
** scope with the given id was found.
*/
{
    const DbgInfo*      Info;
    const ModInfo*      M;
    cc65_scopeinfo*     D;
    unsigned            I;

    /* Check the parameter */
    assert (Handle != 0);

    /* The handle is actually a pointer to a debug info struct */
    Info = Handle;

    /* Check if the module id is valid */
    if (ModId >= CollCount (&Info->ModInfoById)) {
        return 0;
    }

    /* Get a pointer to the module info */
    M = CollAt (&Info->ModInfoById, ModId);

    /* Allocate memory for the data structure returned to the caller */
    D = new_cc65_scopeinfo (CollCount (&M->ScopeInfoByName));

    /* Fill in the data */
    for (I = 0; I < CollCount (&M->ScopeInfoByName); ++I) {
        CopyScopeInfo (D->data + I, CollAt (&M->ScopeInfoByName, I));
    }

    /* Return the result */
    return D;
}



const cc65_scopeinfo* cc65_scope_byname (cc65_dbginfo Handle, const char* Name)
/* Return the list of scopes with a given name. Returns NULL if no scope with
** the given name was found, otherwise a non empty scope list.
*/
{
    const DbgInfo*      Info;
    unsigned            Index;
    const ScopeInfo*    S;
    cc65_scopeinfo*     D;
    unsigned            Count;
    unsigned            I;


    /* Check the parameter */
    assert (Handle != 0);

    /* The handle is actually a pointer to a debug info struct */
    Info = Handle;

    /* Search for the first item with the given name */
    if (!FindScopeInfoByName (&Info->ScopeInfoByName, Name, &Index)) {
        /* Not found */
        return 0;
    }

    /* Count scopes with this name */
    Count = 1;
    I = Index;
    while (1) {
        if (++I >= CollCount (&Info->ScopeInfoByName)) {
            break;
        }
        S = CollAt (&Info->ScopeInfoByName, I);
        if (strcmp (S->Name, Name) != 0) {
            /* Next symbol has another name */
            break;
        }
        ++Count;
    }

    /* Allocate memory for the data structure returned to the caller */
    D = new_cc65_scopeinfo (Count);

    /* Fill in the data */
    for (I = 0; I < Count; ++I, ++Index) {
        CopyScopeInfo (D->data + I, CollAt (&Info->ScopeInfoByName, Index));
    }

    /* Return the result */
    return D;
}



const cc65_scopeinfo* cc65_scope_byspan (cc65_dbginfo Handle, unsigned SpanId)
/* Return scope information for a a span. The function returns NULL if the
** span id is invalid, otherwise a list of line scopes.
*/
{
    const DbgInfo*      Info;
    const SpanInfo*     S;
    cc65_scopeinfo*     D;
    unsigned            I;

    /* Check the parameter */
    assert (Handle != 0);

    /* The handle is actually a pointer to a debug info struct */
    Info = Handle;

    /* Check if the span id is valid */
    if (SpanId >= CollCount (&Info->SpanInfoById)) {
        return 0;
    }

    /* Get the span */
    S = CollAt (&Info->SpanInfoById, SpanId);

    /* Prepare the struct we will return to the caller */
    D = new_cc65_scopeinfo (CollCount (S->ScopeInfoList));

    /* Fill in the data. Since D->ScopeInfoList may be NULL, we will use the
    ** count field of the returned data struct instead.
    */
    for (I = 0; I < D->count; ++I) {
        /* Copy the data */
        CopyScopeInfo (D->data + I, CollAt (S->ScopeInfoList, I));
    }

    /* Return the allocated struct */
    return D;
}



const cc65_scopeinfo* cc65_childscopes_byid (cc65_dbginfo Handle, unsigned Id)
/* Return the direct child scopes of a scope with a given id. The function
** returns NULL if no scope with this id was found, otherwise a list of the
** direct childs.
*/
{
    const DbgInfo*      Info;
    cc65_scopeinfo*     D;
    const ScopeInfo*    S;
    unsigned            I;

    /* Check the parameter */
    assert (Handle != 0);

    /* The handle is actually a pointer to a debug info struct */
    Info = Handle;

    /* Check if the id is valid */
    if (Id >= CollCount (&Info->ScopeInfoById)) {
        return 0;
    }

    /* Get the scope */
    S = CollAt (&Info->ScopeInfoById, Id);

    /* Allocate memory for the data structure returned to the caller */
    D = new_cc65_scopeinfo (CollCount (S->ChildScopeList));

    /* Fill in the data */
    for (I = 0; I < D->count; ++I) {
        CopyScopeInfo (D->data + I, CollAt (S->ChildScopeList, I));
    }

    /* Return the result */
    return D;
}



void cc65_free_scopeinfo (cc65_dbginfo Handle, const cc65_scopeinfo* Info)
/* Free a scope info record */
{
    /* Just for completeness, check the handle */
    assert (Handle != 0);

    /* Free the memory */
    xfree ((cc65_scopeinfo*) Info);
}



/*****************************************************************************/
/*                                 Segments                                  */
/*****************************************************************************/



const cc65_segmentinfo* cc65_get_segmentlist (cc65_dbginfo Handle)
/* Return a list of all segments referenced in the debug information */
{
    const DbgInfo*      Info;
    cc65_segmentinfo*   D;
    unsigned            I;

    /* Check the parameter */
    assert (Handle != 0);

    /* The handle is actually a pointer to a debug info struct */
    Info = Handle;

    /* Allocate memory for the data structure returned to the caller */
    D = new_cc65_segmentinfo (CollCount (&Info->SegInfoById));

    /* Fill in the data */
    for (I = 0; I < CollCount (&Info->SegInfoById); ++I) {
        /* Copy the data */
        CopySegInfo (D->data + I, CollAt (&Info->SegInfoById, I));
    }

    /* Return the result */
    return D;
}



const cc65_segmentinfo* cc65_segment_byid (cc65_dbginfo Handle, unsigned Id)
/* Return information about a segment with a specific id. The function returns
** NULL if the id is invalid (no such segment) and otherwise a segmentinfo
** structure with one entry that contains the requested segment information.
*/
{
    const DbgInfo*      Info;
    cc65_segmentinfo*   D;

    /* Check the parameter */
    assert (Handle != 0);

    /* The handle is actually a pointer to a debug info struct */
    Info = Handle;

    /* Check if the id is valid */
    if (Id >= CollCount (&Info->SegInfoById)) {
        return 0;
    }

    /* Allocate memory for the data structure returned to the caller */
    D = new_cc65_segmentinfo (1);

    /* Fill in the data */
    CopySegInfo (D->data, CollAt (&Info->SegInfoById, Id));

    /* Return the result */
    return D;
}



const cc65_segmentinfo* cc65_segment_byname (cc65_dbginfo Handle,
                                             const char* Name)
/* Return information about a segment with a specific name. The function
** returns NULL if no segment with this name exists and otherwise a
** cc65_segmentinfo structure with one entry that contains the requested
** information.
*/
{
    const DbgInfo*      Info;
    const SegInfo*      S;
    cc65_segmentinfo*   D;

    /* Check the parameter */
    assert (Handle != 0);

    /* The handle is actually a pointer to a debug info struct */
    Info = Handle;

    /* Search for the segment */
    S = FindSegInfoByName (&Info->SegInfoByName, Name);
    if (S == 0) {
        return 0;
    }

    /* Allocate memory for the data structure returned to the caller */
    D = new_cc65_segmentinfo (1);

    /* Fill in the data */
    CopySegInfo (D->data, S);

    /* Return the result */
    return D;
}



void cc65_free_segmentinfo (cc65_dbginfo Handle, const cc65_segmentinfo* Info)
/* Free a segment info record */
{
    /* Just for completeness, check the handle */
    assert (Handle != 0);

    /* Free the memory */
    xfree ((cc65_segmentinfo*) Info);
}



/*****************************************************************************/
/*                                  Symbols                                  */
/*****************************************************************************/



const cc65_symbolinfo* cc65_symbol_byid (cc65_dbginfo Handle, unsigned Id)
/* Return the symbol with a given id. The function returns NULL if no symbol
** with this id was found.
*/
{
    const DbgInfo*      Info;
    cc65_symbolinfo*    D;

    /* Check the parameter */
    assert (Handle != 0);

    /* The handle is actually a pointer to a debug info struct */
    Info = Handle;

    /* Check if the id is valid */
    if (Id >= CollCount (&Info->SymInfoById)) {
        return 0;
    }

    /* Allocate memory for the data structure returned to the caller */
    D = new_cc65_symbolinfo (1);

    /* Fill in the data */
    CopySymInfo (D->data, CollAt (&Info->SymInfoById, Id));

    /* Return the result */
    return D;
}



const cc65_symbolinfo* cc65_symbol_byname (cc65_dbginfo Handle, const char* Name)
/* Return a list of symbols with a given name. The function returns NULL if
** no symbol with this name was found.
*/
{
    const DbgInfo*      Info;
    cc65_symbolinfo*    D;
    unsigned            I;
    unsigned            Index;
    unsigned            Count;

    /* Check the parameter */
    assert (Handle != 0);

    /* The handle is actually a pointer to a debug info struct */
    Info = Handle;

    /* Search for the symbol */
    if (!FindSymInfoByName (&Info->SymInfoByName, Name, &Index)) {
        /* Not found */
        return 0;
    }

    /* Index contains the position. Count how many symbols with this name
    ** we have. Skip the first one, since we have at least one.
    */
    Count = 1;
    while ((unsigned) Index + Count < CollCount (&Info->SymInfoByName)) {
        const SymInfo* S = CollAt (&Info->SymInfoByName, (unsigned) Index + Count);
        if (strcmp (S->Name, Name) != 0) {
            break;
        }
        ++Count;
    }

    /* Allocate memory for the data structure returned to the caller */
    D = new_cc65_symbolinfo (Count);

    /* Fill in the data */
    for (I = 0; I < Count; ++I) {
        /* Copy the data */
        CopySymInfo (D->data + I, CollAt (&Info->SymInfoByName, Index++));
    }

    /* Return the result */
    return D;
}



const cc65_symbolinfo* cc65_symbol_byscope (cc65_dbginfo Handle, unsigned ScopeId)
/* Return a list of symbols in the given scope. This includes cheap local
** symbols, but not symbols in subscopes. The function returns NULL if the
** scope id is invalid (no such scope) and otherwise a - possibly empty -
** symbol list.
*/
{
    const DbgInfo*      Info;
    cc65_symbolinfo*    D;
    const ScopeInfo*    S;
    unsigned            I;


    /* Check the parameter */
    assert (Handle != 0);

    /* The handle is actually a pointer to a debug info struct */
    Info = Handle;

    /* Check if the id is valid */
    if (ScopeId >= CollCount (&Info->ScopeInfoById)) {
        return 0;
    }

    /* Get the scope */
    S = CollAt (&Info->ScopeInfoById, ScopeId);

    /* Allocate memory for the data structure returned to the caller */
    D = new_cc65_symbolinfo (CollCount (&S->SymInfoByName));

    /* Fill in the data */
    for (I = 0; I < CollCount (&S->SymInfoByName); ++I) {
        /* Copy the data */
        CopySymInfo (D->data + I, CollAt (&S->SymInfoByName, I));
    }

    /* Return the result */
    return D;
}



const cc65_symbolinfo* cc65_symbol_inrange (cc65_dbginfo Handle, cc65_addr Start,
                                            cc65_addr End)
/* Return a list of labels in the given range. End is inclusive. The function
** return NULL if no symbols within the given range are found. Non label
** symbols are ignored and not returned.
*/
{
    const DbgInfo*      Info;
    Collection          SymInfoList = COLLECTION_INITIALIZER;
    cc65_symbolinfo*    D;
    unsigned            I;
    unsigned            Index;

    /* Check the parameter */
    assert (Handle != 0);

    /* The handle is actually a pointer to a debug info struct */
    Info = Handle;

    /* Search for the symbol. Because we're searching for a range, we cannot
    ** make use of the function result.
    */
    FindSymInfoByValue (&Info->SymInfoByVal, Start, &Index);

    /* Start from the given index, check all symbols until the end address is
    ** reached. Place all symbols into SymInfoList for later.
    */
    for (I = Index; I < CollCount (&Info->SymInfoByVal); ++I) {

        /* Get the item */
        SymInfo* Item = CollAt (&Info->SymInfoByVal, I);

        /* The collection is sorted by address, so if we get a value larger
        ** than the end address, we're done.
        */
        if (Item->Value > (long) End) {
            break;
        }

        /* Ignore non-labels (this will also ignore imports) */
        if (Item->Type != CC65_SYM_LABEL) {
            continue;
        }

        /* Ok, remember this one */
        CollAppend (&SymInfoList, Item);
    }

    /* If we don't have any labels within the range, bail out. No memory has
    ** been allocated for SymInfoList.
    */
    if (CollCount (&SymInfoList) == 0) {
        return 0;
    }

    /* Allocate memory for the data structure returned to the caller */
    D = new_cc65_symbolinfo (CollCount (&SymInfoList));

    /* Fill in the data */
    for (I = 0; I < CollCount (&SymInfoList); ++I) {
        /* Copy the data */
        CopySymInfo (D->data + I, CollAt (&SymInfoList, I));
    }

    /* Free the collection */
    CollDone (&SymInfoList);

    /* Return the result */
    return D;
}



void cc65_free_symbolinfo (cc65_dbginfo Handle, const cc65_symbolinfo* Info)
/* Free a symbol info record */
{
    /* Just for completeness, check the handle */
    assert (Handle != 0);

    /* Free the memory */
    xfree ((cc65_symbolinfo*) Info);
}



/*****************************************************************************/
/*                                   Types                                   */
/*****************************************************************************/



const cc65_typedata* cc65_type_byid (cc65_dbginfo Handle, unsigned Id)
/* Return the data for the type with the given id. The function returns NULL
** if no type with this id was found.
*/
{
    const DbgInfo*        Info;
    const TypeInfo*       T;

    /* Check the parameter */
    assert (Handle != 0);

    /* The handle is actually a pointer to a debug info struct */
    Info = Handle;

    /* Check if the id is valid */
    if (Id >= CollCount (&Info->TypeInfoById)) {
        return 0;
    }

    /* Get the type info with the given id */
    T = CollAt (&Info->TypeInfoById, Id);

    /* We do already have the type data. Return it. */
    return T->Data;
}



void cc65_free_typedata (cc65_dbginfo Handle, const cc65_typedata* data)
/* Free a symbol info record */
{
    /* Just for completeness, check the handle and the data*/
    assert (Handle != 0 && data != 0);

    /* Nothing to do */
}



