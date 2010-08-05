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
/*     	       	       	 	     Data				     */
/*****************************************************************************/



/* Dynamic strings */
typedef struct StrBuf StrBuf;
struct StrBuf {
    char*       Buf;                    /* Pointer to buffer */
    unsigned    Len;                    /* Length of the string */
    unsigned    Allocated;              /* Size of allocated memory */
};

/* Initializer for a string buffer */
#define STRBUF_INITIALIZER      { 0, 0, 0 }

/* An array of pointers that grows if needed */
typedef struct Collection Collection;
struct Collection {
    unsigned   	       	Count;		/* Number of items in the list */
    unsigned   	       	Size;		/* Size of allocated array */
    void**	    	Items;		/* Array with dynamic size */
};

/* Initializer for static collections */
#define COLLECTION_INITIALIZER  { 0, 0, 0 }



/* Data structure containing information from the debug info file. A pointer
 * to this structure is passed as handle to callers from the outside.
 */
typedef struct DbgInfo DbgInfo;
struct DbgInfo {
    Collection          FileInfos;      /* Collection with file infos */
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

    TOK_ABSOLUTE,                       /* ABSOLUTE keyword */
    TOK_ADDRSIZE,                       /* ADDRSIZE keyword */
    TOK_EQUATE,                         /* EQUATE keyword */
    TOK_FILE,                           /* FILE keyword */
    TOK_LABEL,                          /* LABEL keyword */
    TOK_LINE,                           /* LINE keyword */
    TOK_LONG,                           /* LONG_keyword */
    TOK_MAJOR,                          /* MAJOR keyword */
    TOK_MINOR,                          /* MINOR keyword */
    TOK_MTIME,                          /* MTIME keyword */
    TOK_RANGE,                          /* RANGE keyword */
    TOK_RO,                             /* RO keyword */
    TOK_RW,                             /* RW keyword */
    TOK_SEGMENT,                        /* SEGMENT keyword */
    TOK_SIZE,                           /* SIZE keyword */
    TOK_START,                          /* START keyword */
    TOK_SYM,                            /* SYM keyword */
    TOK_TYPE,                           /* TYPE keyword */
    TOK_VALUE,                          /* VALUE keyword */
    TOK_VERSION,                        /* VERSION keyword */
    TOK_ZEROPAGE,                       /* ZEROPAGE keyword */

    TOK_IDENT,                          /* To catch unknown keywords */
} Token;

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
    unsigned            MajorVersion;   /* Major version number */
    unsigned            MinorVersion;   /* Minor version number */
    Collection          LineInfos;      /* Line information */
    DbgInfo*            Info;           /* Pointer to debug info */
};

/* Internally used file info struct */
typedef struct FileInfo FileInfo;
struct FileInfo {
    unsigned long       Size;           /* Size of file */
    unsigned long       MTime;          /* Modification time */
    cc65_addr           Start;          /* Start address of line infos */
    cc65_addr           End;            /* End address of line infos */
    Collection          LineInfos;      /* Line infos for this file */
    char                FileName[1];    /* Name of file with full path */
};

/* Internally used line info struct */
typedef struct LineInfo LineInfo;
struct LineInfo {
    cc65_addr           Start;          /* Start of data range */
    cc65_addr           End;            /* End of data range */
    cc65_line           Line;           /* Line number */
    FileInfo*           FileInfo;       /* Pointer to file info */
    char                FileName[1];    /* Name of file */
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
 * available.
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
     * length is not. This means that we have a buffer that wasn't allocated
     * on the heap.
     */
    if (B->Allocated) {
        /* Just reallocate the block */
        B->Buf   = xrealloc (B->Buf, NewAllocated);
    } else {
        /* Allocate a new block and copy */
        B->Buf   = memcpy (xmalloc (NewAllocated), B->Buf, B->Len);
    }

    /* Remember the new block size */
    B->Allocated = NewAllocated;
}



static unsigned SB_GetLen (const StrBuf* B)
/* Return the length of the buffer contents */
{
    return B->Len;
}



static const char* SB_GetConstBuf (const StrBuf* B)
/* Return a buffer pointer */
{
    return B->Buf;
}



static void SB_Terminate (StrBuf* B)
/* Zero terminate the given string buffer. NOTE: The terminating zero is not
 * accounted for in B->Len, if you want that, you have to use AppendChar!
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



/*****************************************************************************/
/*                                Collections                                */
/*****************************************************************************/



static Collection* InitCollection (Collection* C)
/* Initialize a collection and return it. */
{
    /* Intialize the fields. */
    C->Count = 0;
    C->Size  = 0;
    C->Items = 0;

    /* Return the new struct */
    return C;
}



static void DoneCollection (Collection* C)
/* Free the data for a collection. This will not free the data contained in
 * the collection.
 */
{
    /* Free the pointer array */
    xfree (C->Items);
}



static unsigned CollCount (const Collection* C)
/* Return the number of items in the collection */
{
    return C->Count;
}



static void CollGrow (Collection* C, unsigned Size)
/* Grow the collection C so it is able to hold Size items without a resize
 * being necessary. This can be called for performance reasons if the number
 * of items to be placed in the collection is known in advance.
 */
{
    void** NewItems;

    /* Ignore the call if the collection is already large enough */
    if (Size <= C->Size) {
        return;
    }

    /* Grow the collection */
    C->Size = Size;
    NewItems = xmalloc (C->Size * sizeof (void*));
    memcpy (NewItems, C->Items, C->Count * sizeof (void*));
    xfree (C->Items);
    C->Items = NewItems;
}



static void CollInsert (Collection* C, void* Item, unsigned Index)
/* Insert the data at the given position in the collection */
{
    /* Check for invalid indices */
    assert (Index <= C->Count);

    /* Grow the array if necessary */
    if (C->Count >= C->Size) {
       	/* Must grow */
        CollGrow (C, (C->Size == 0)? 8 : C->Size * 2);
    }

    /* Move the existing elements if needed */
    if (C->Count != Index) {
       	memmove (C->Items+Index+1, C->Items+Index, (C->Count-Index) * sizeof (void*));
    }
    ++C->Count;

    /* Store the new item */
    C->Items[Index] = Item;
}



static void CollAppend (Collection* C, void* Item)
/* Append an item to the end of the collection */
{
    /* Insert the item at the end of the current list */
    CollInsert (C, Item, C->Count);
}



static void* CollAt (Collection* C, unsigned Index)
/* Return the item at the given index */
{
    /* Check the index */
    assert (Index < C->Count);

    /* Return the element */
    return C->Items[Index];
}



static void* CollFirst (Collection* C)
/* Return the first item in a collection */
{
    /* We must have at least one entry */
    assert (C->Count > 0);

    /* Return the element */
    return C->Items[0];
}



static void* CollLast (Collection* C)
/* Return the last item in a collection */
{
    /* We must have at least one entry */
    assert (C->Count > 0);

    /* Return the element */
    return C->Items[C->Count-1];
}



static void CollDelete (Collection* C, unsigned Index)
/* Remove the item with the given index from the collection. This will not
 * free the item itself, just the pointer. All items with higher indices
 * will get moved to a lower position.
 */
{
    /* Check the index */
    assert (Index < C->Count);

    /* Remove the item pointer */
    --C->Count;
    memmove (C->Items+Index, C->Items+Index+1, (C->Count-Index) * sizeof (void*));
}



static void CollReplace (Collection* C, void* Item, unsigned Index)
/* Replace the item at the given position. The old item will not be freed,
 * just the pointer will get replaced.
 */
{
    /* Check the index */
    assert (Index < C->Count);

    /* Replace the item pointer */
    C->Items[Index] = Item;
}



static void CollQuickSort (Collection* C, int Lo, int Hi,
   	                   int (*Compare) (const void*, const void*))
/* Internal recursive sort function. */
{
    /* Get a pointer to the items */
    void** Items = C->Items;

    /* Quicksort */
    while (Hi > Lo) {
   	int I = Lo + 1;
   	int J = Hi;
   	while (I <= J) {
   	    while (I <= J && Compare (Items[Lo], Items[I]) >= 0) {
   	     	++I;
   	    }
   	    while (I <= J && Compare (Items[Lo], Items[J]) < 0) {
   	     	--J;
   	    }
   	    if (I <= J) {
		/* Swap I and J */
		void* Tmp = Items[I];
		Items[I]  = Items[J];
		Items[J]  = Tmp;
   	     	++I;
   	     	--J;
   	    }
      	}
   	if (J != Lo) {
	    /* Swap J and Lo */
	    void* Tmp = Items[J];
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



void CollSort (Collection* C, int (*Compare) (const void*, const void*))
/* Sort the collection using the given compare function. */
{
    if (C->Count > 1) {
        CollQuickSort (C, 0, C->Count-1, Compare);
    }
}



/*****************************************************************************/
/*                                 Line info                                 */
/*****************************************************************************/



static LineInfo* NewLineInfo (const StrBuf* FileName)
/* Create a new LineInfo struct and return it */
{
    /* Allocate memory */
    LineInfo* L = xmalloc (sizeof (LineInfo) + SB_GetLen (FileName));

    /* Initialize it */
    L->Start    = 0;
    L->End      = 0;
    L->Line     = 0;
    L->FileInfo = 0;
    memcpy (L->FileName, SB_GetConstBuf (FileName), SB_GetLen (FileName) + 1);

    /* Return it */
    return L;
}



static void FreeLineInfo (LineInfo* L)
/* Free a LineInfo struct */
{
    xfree (L);
}



static LineInfo* PreenLineInfo (LineInfo* L, FileInfo* F)
/* Replace the name by file information */
{
    /* Shrink the LineInfo struct removing the FfileName field */
    L = xrealloc (L, sizeof (*L) - 1);

    /* Set the FileInfo pointer instead */
    L->FileInfo = F;

    /* Return the result */
    return L;
}



static int CompareLineInfo (const void* L, const void* R)
/* Helper function to sort line infos in a collection */
{
    /* Sort by start of range */
    if (((const LineInfo*) L)->Start > ((const LineInfo*) R)->Start) {
        return 1;
    } else if (((const LineInfo*) L)->Start < ((const LineInfo*) R)->Start) {
        return -1;
    } else {
        return 0;
    }
}



/*****************************************************************************/
/*                                 File info                                 */
/*****************************************************************************/



static FileInfo* NewFileInfo (const StrBuf* FileName)
/* Create a new FileInfo struct and return it */
{
    /* Allocate memory */
    FileInfo* F = xmalloc (sizeof (FileInfo) + SB_GetLen (FileName));

    /* Initialize it */
    F->Size  = 0;
    F->MTime = 0;
    F->Start = ~(cc65_addr)0;
    F->End   = 0;
    InitCollection (&F->LineInfos);
    memcpy (F->FileName, SB_GetConstBuf (FileName), SB_GetLen (FileName) + 1);

    /* Return it */
    return F;
}



static void FreeFileInfo (FileInfo* F)
/* Free a FileInfo struct */
{
    unsigned I;

    /* Walk through the collection with line infos and delete them */
    for (I = 0; I < CollCount (&F->LineInfos); ++I) {
        FreeLineInfo (CollAt (&F->LineInfos, I));
    }
    DoneCollection (&F->LineInfos);

    /* Free the file info structure itself */
    xfree (F);
}



static int CompareFileInfo (const void* L, const void* R)
/* Helper function to sort file infos in a collection */
{
    /* Sort by file name */
    return strcmp (((const FileInfo*) L)->FileName,
                   ((const FileInfo*) R)->FileName);
}



/*****************************************************************************/
/*                                Debug info                                 */
/*****************************************************************************/



static DbgInfo* NewDbgInfo (void)
/* Create a new DbgInfo struct and return it */
{
    /* Allocate memory */
    DbgInfo* Info = xmalloc (sizeof (DbgInfo));

    /* Initialize it */
    InitCollection (&Info->FileInfos);

    /* Return it */
    return Info;
}



static void FreeDbgInfo (DbgInfo* Info)
/* Free a DbgInfo struct */
{
    unsigned I;

    /* Free file info */
    for (I = 0; I < CollCount (&Info->FileInfos); ++I) {
        FreeFileInfo (CollAt (&Info->FileInfos, I));
    }
    DoneCollection (&Info->FileInfos);

    /* Free the structure itself */
    xfree (Info);
}



/*****************************************************************************/
/*                             Helper functions                              */
/*****************************************************************************/



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



static void MissingAttribute (InputData* D, const char* AttrName)
/* Print an error about a missing attribute */
{
    ParseError (D, CC65_ERROR, "Attribute \"%s\" is mising", AttrName);
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
        D->C = fgetc (D->F);
        if (D->C == '\n') {
            ++D->Line;
            D->Col = 0;
        } else {
            ++D->Col;
        }
    }
}



static void NextToken (InputData* D)
/* Read the next token from the input stream */
{
    static const struct KeywordEntry  {
        const char      Keyword[10];
        Token           Tok;
    } KeywordTable[] = {
        { "absolute",   TOK_ABSOLUTE    },
        { "addrsize",   TOK_ADDRSIZE    },
        { "equate",     TOK_EQUATE      },
        { "file",       TOK_FILE        },
        { "label",      TOK_LABEL       },
        { "line",       TOK_LINE        },
        { "long",       TOK_LONG        },
        { "major",      TOK_MAJOR       },
        { "minor",      TOK_MINOR       },
        { "mtime",      TOK_MTIME       },
        { "range",      TOK_RANGE       },
        { "ro",         TOK_RO          },
        { "rw",         TOK_RW          },
        { "segment",    TOK_SEGMENT     },
        { "size",       TOK_SIZE        },
        { "start",      TOK_START       },
        { "sym",        TOK_SYM         },
        { "type",       TOK_TYPE        },
        { "value",      TOK_VALUE       },
        { "version",    TOK_VERSION     },
        { "zeropage",   TOK_ZEROPAGE    },
    };


    /* Skip whitespace */
    while (D->C == ' ' || D->C == '\t') {
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



static int TokenFollows (InputData* D, Token Tok, const char* Name)
/* Check for a comma */
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



static int StringConstFollows (InputData* D)
/* Check for a string literal */
{
    return TokenFollows (D, TOK_STRCON, "String literal");
}



static int Consume (InputData* D, Token Tok, const char* Name)
/* Check for a token and consume it. Return true if the token was comsumed,
 * return false otherwise.
 */
{
    if (TokenFollows (D, Tok, Name)) {
        NextToken (D);
        return 1;
    } else {
        return 0;
    }
}



static int ConsumeComma (InputData* D)
/* Consume a comma */
{
    return Consume (D, TOK_COMMA, "','");
}



static int ConsumeEqual (InputData* D)
/* Consume an equal sign */
{
    return Consume (D, TOK_EQUAL, "'='");
}



static int ConsumeMinus (InputData* D)
/* Consume a minus sign */
{
    return Consume (D, TOK_MINUS, "'-'");
}



static void ParseFile (InputData* D)
/* Parse a FILE line */
{
    FileInfo* F;
    enum { None = 0x00, Size = 0x01, MTime = 0x02 } InfoBits = None;

    /* Skip the FILE token */
    NextToken (D);

    /* Name follows */
    if (!StringConstFollows (D)) {
        return;
    }

    /* Allocate a new file info */
    F = NewFileInfo (&D->SVal);

    /* Skip the file name */
    NextToken (D);

    /* More stuff follows */
    while (D->Tok != TOK_EOL && D->Tok != TOK_EOF) {

        /* Comma follows before next attribute */
        if (!ConsumeComma (D)) {
            goto ErrorExit;
        }

        switch (D->Tok) {

            case TOK_SIZE:
                NextToken (D);
                if (!ConsumeEqual (D)) {
                    goto ErrorExit;
                }
                if (!IntConstFollows (D)) {
                    goto ErrorExit;
                }
                F->Size = D->IVal;
                NextToken (D);
                InfoBits |= Size;
                break;

            case TOK_MTIME:
                NextToken (D);
                if (!ConsumeEqual (D)) {
                    goto ErrorExit;
                }
                if (!IntConstFollows (D)) {
                    goto ErrorExit;
                }
                F->MTime = D->IVal;
                NextToken (D);
                InfoBits |= MTime;
                break;

            default:
                UnexpectedToken (D);
                SkipLine (D);
                goto ErrorExit;
        }

    }

    /* Check for required information */
    if ((InfoBits & Size) == None) {
        MissingAttribute (D, "size");
        goto ErrorExit;
    }
    if ((InfoBits & MTime) == None) {
        MissingAttribute (D, "mtime");
        goto ErrorExit;
    }

    /* Remember the file info */
    CollAppend (&D->Info->FileInfos, F);

    /* Done */
    return;

ErrorExit:
    /* Entry point in case of errors */
    FreeFileInfo (F);
}



static void ParseLine (InputData* D)
/* Parse a LINE line */
{
    LineInfo* L;
    enum { None = 0x00, Line = 0x01, Range = 0x02 } InfoBits = None;

    /* Skip the LINE token */
    NextToken (D);

    /* File name follows */
    if (!StringConstFollows (D)) {
        return;
    }

    /* Allocate a new line info */
    L = NewLineInfo (&D->SVal);

    /* Skip the file name */
    NextToken (D);

    /* More stuff follows */
    while (D->Tok != TOK_EOL && D->Tok != TOK_EOF) {

        /* Comma follows before next attribute */
        if (!ConsumeComma (D)) {
            goto ErrorExit;
        }

        switch (D->Tok) {

            case TOK_LINE:
                NextToken (D);
                if (!ConsumeEqual (D)) {
                    goto ErrorExit;
                }
                if (!IntConstFollows (D)) {
                    goto ErrorExit;
                }
                L->Line = D->IVal;
                NextToken (D);
                InfoBits |= Line;
                break;

            case TOK_RANGE:
                NextToken (D);
                if (!ConsumeEqual (D)) {
                    goto ErrorExit;
                }
                if (!IntConstFollows (D)) {
                    goto ErrorExit;
                }
                L->Start = (cc65_addr) D->IVal;
                NextToken (D);
                if (!ConsumeMinus (D)) {
                    goto ErrorExit;
                }
                if (!IntConstFollows (D)) {
                    goto ErrorExit;
                }
                L->End = (cc65_addr) D->IVal;
                NextToken (D);
                InfoBits |= Range;
                break;

            default:
                UnexpectedToken (D);
                SkipLine (D);
                goto ErrorExit;
        }

    }

    /* Check for required information */
    if ((InfoBits & Line) == None) {
        MissingAttribute (D, "line");
        goto ErrorExit;
    }
    if ((InfoBits & Range) == None) {
        MissingAttribute (D, "range");
        goto ErrorExit;
    }

    /* Remember the line info */
    CollAppend (&D->LineInfos, L);

    /* Done */
    return;

ErrorExit:
    /* Entry point in case of errors */
    FreeLineInfo (L);
}



static void ParseSegment (InputData* D)
/* Parse a SEGMENT line */
{
    /* Skip the SEGMENT token */
    NextToken (D);

    /* ### */
    SkipLine (D);
}



static void ParseSym (InputData* D)
/* Parse a SYM line */
{
    /* Skip the SYM token */
    NextToken (D);

    /* ### */
    SkipLine (D);
}



static void ParseVersion (InputData* D)
/* Parse a VERSION line */
{
    enum { None = 0x00, Major = 0x01, Minor = 0x02 } InfoBits = None;

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
                D->MajorVersion = D->IVal;
                NextToken (D);
                InfoBits |= Major;
                break;

            case TOK_MINOR:
                NextToken (D);
                if (!ConsumeEqual (D)) {
                    goto ErrorExit;
                }
                if (!IntConstFollows (D)) {
                    goto ErrorExit;
                }
                D->MinorVersion = D->IVal;
                NextToken (D);
                InfoBits |= Minor;
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
    if ((InfoBits & Major) == None) {
        MissingAttribute (D, "major");
        goto ErrorExit;
    }
    if ((InfoBits & Minor) == None) {
        MissingAttribute (D, "minor");
        goto ErrorExit;
    }

ErrorExit:
    /* Entry point in case of errors */
    return;
}



/*****************************************************************************/
/*                              Data processing                              */
/*****************************************************************************/



static FileInfo* FindFileInfo (InputData* D, const char* FileName)
/* Find the FileInfo for a given file name */
{
    /* Get a pointer to the file info collection */
    Collection* FileInfos = &D->Info->FileInfos;

    /* Do a binary search */
    int Lo = 0;
    int Hi = (int) CollCount (FileInfos) - 1;
    while (Lo <= Hi) {

        /* Mid of range */
        int Cur = (Lo + Hi) / 2;

        /* Get item */
        FileInfo* CurItem = CollAt (FileInfos, Cur);

        /* Compare */
        int Res = strcmp (CurItem->FileName, FileName);

        /* Found? */
        if (Res < 0) {
            Lo = Cur + 1;
        } else if (Res > 0) {
            Hi = Cur - 1;
        } else {
            /* Found! */
            return CurItem;
        }
    }

    /* Not found */
    return 0;
}



static void ProcessFileInfo (InputData* D)
/* Postprocess file infos */
{
    /* Get a pointer to the file info collection */
    Collection* FileInfos = &D->Info->FileInfos;

    /* First, sort the file infos, so we can check for duplicates and do
     * binary search.
     */
    CollSort (FileInfos, CompareFileInfo);

    /* Cannot work on an empty collection */
    if (CollCount (FileInfos) > 0) {

        /* Walk through the file infos and check for duplicates. If we find
         * some, warn and remove them, so the file infos are unique after
         * that step.
         */
        FileInfo* F = CollAt (FileInfos, 0);
        unsigned I = 1;
        while (I < CollCount (FileInfos)) {
            FileInfo* Next = CollAt (FileInfos, I);
            if (strcmp (F->FileName, Next->FileName) == 0) {
                /* Warn only if time stamp and/or size is different */
                if (F->Size != Next->Size || F->MTime != Next->MTime) {
                    ParseError (D,
                                CC65_WARNING,
                                "Duplicate file entry for \"%s\"",
                                F->FileName);
                }
                /* Remove the duplicate entry */
                FreeFileInfo (Next);
                CollDelete (FileInfos, I);
            } else {
                /* This one is ok, check the next entry */
                F = Next;
                ++I;
            }
        }
    }
}



static void ProcessLineInfo (InputData* D)
/* Postprocess line infos */
{
    /* Get pointers to the collections */
    Collection* LineInfos = &D->LineInfos;
    Collection* FileInfos = &D->Info->FileInfos;

    /* Walk over the line infos and replace the name by a pointer to the
     * corresponding file info struct. The LineInfo structs will get shrinked
     * in this process. Add the line info to each file where it is defined.
     */
    unsigned I = 0;
    while (I < CollCount (LineInfos)) {

        /* Get LineInfo struct */
        LineInfo* L = CollAt (LineInfos, I);

        /* Find FileInfo that corresponds to name */
        FileInfo* F = FindFileInfo (D, L->FileName);

        /* If we have no corresponding file info, print a warning and remove
         * the line info.
         */
        if (F == 0) {
            ParseError (D,
                        CC65_ERROR,
                        "No file info for file \"%s\"",
                        L->FileName);
            FreeLineInfo (L);
            CollDelete (LineInfos, I);
            continue;
        }

        /* Shrink the line info struct effectively removing the file name
         * but set the pointer to the file info now.
         */
        L = PreenLineInfo (L, F);
        CollReplace (LineInfos, L, I);

        /* Add this line info to the file where it is defined */
        CollAppend (&F->LineInfos, L);

        /* Next one */
        ++I;
    }

    /* Walk over all files and sort the line infos for each file by ascending
     * start address of the range, so we can do a binary search later.
     */
    for (I = 0; I < CollCount (FileInfos); ++I) {

        /* Get a pointer to this file info */
        FileInfo* F = CollAt (FileInfos, I);

        /* Sort the line infos for this file */
        CollSort (&F->LineInfos, CompareLineInfo);

        /* If there are line info entries, place the first and last address
         * of into the FileInfo struct itself, so we can rule out a FileInfo
         * quickly when mapping an address to a line info.
         */
        if (CollCount (&F->LineInfos) > 0) {
            F->Start = ((const LineInfo*) CollFirst (&F->LineInfos))->Start;
            F->End   = ((const LineInfo*) CollLast (&F->LineInfos))->End;
        }

    }
}



static LineInfo* FindLineInfo (FileInfo* F, cc65_addr Addr)
/* Find the LineInfo for a given address */
{
    Collection* LineInfos;
    int         Hi;
    int         Lo;


    /* Each file info contains the first and last address for which line
     * info is available, so we can rule out non matching ones quickly.
     */
    if (Addr < F->Start || Addr > F->End) {
        return 0;
    }

    /* Get a pointer to the line info collection for this file */
    LineInfos = &F->LineInfos;

    /* Do a binary search */
    Lo = 0;
    Hi = (int) CollCount (LineInfos) - 1;
    while (Lo <= Hi) {

        /* Mid of range */
        int Cur = (Lo + Hi) / 2;

        /* Get item */
        LineInfo* CurItem = CollAt (LineInfos, Cur);

        /* Found? */
        if (Addr < CurItem->Start) {
            Hi = Cur - 1;
        } else if (Addr > CurItem->End) {
            Lo = Cur + 1;
        } else {
            /* Found! */
            return CurItem;
        }
    }

    /* Not found */
    return 0;
}



/*****************************************************************************/
/*     	      	       	      	     Code				     */
/*****************************************************************************/



cc65_dbginfo cc65_read_dbginfo (const char* FileName, cc65_errorfunc ErrFunc)
/* Parse the debug info file with the given name. On success, the function
 * will return a pointer to an opaque cc65_dbginfo structure, that must be
 * passed to the other functions in this module to retrieve information.
 * errorfunc is called in case of warnings and errors. If the file cannot be
 * read successfully, NULL is returned.
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
        0,                      /* Major version number */
        0,                      /* Minor version number */
        COLLECTION_INITIALIZER, /* Line information */
        0,                      /* Pointer to debug info */
    };
    D.FileName = FileName;
    D.Error    = ErrFunc;

    /* Open the input file */
    D.F = fopen (D.FileName, "r");
    if (D.F == 0) {
        /* Cannot open */
        ParseError (&D, CC65_ERROR,
                    "Cannot open input file \"%s\": %s",
                     D.FileName, strerror (errno));
        return 0;
    }

    /* Create a new debug info struct */
    D.Info = NewDbgInfo ();

    /* Prime the pump */
    NextToken (&D);

    /* Parse lines */
    while (D.Tok != TOK_EOF) {

        switch (D.Tok) {

            case TOK_FILE:
                ParseFile (&D);
                break;

            case TOK_LINE:
                ParseLine (&D);
                break;

            case TOK_SEGMENT:
                ParseSegment (&D);
                break;

            case TOK_SYM:
                ParseSym (&D);
                break;

            case TOK_VERSION:
                ParseVersion (&D);
                break;

            default:
                UnexpectedToken (&D);

        }

        /* EOL or EOF must follow */
        if (D.Tok != TOK_EOF) {
            if (D.Tok != TOK_EOL) {
                ParseError (&D, 1, "Extra tokens in line");
                SkipLine (&D);
            }
            NextToken (&D);
        }
    }

    /* Close the file */
    fclose (D.F);

    /* Free memory allocated for SVal */
    SB_Done (&D.SVal);

    /* In case of errors, delete the debug info already allocated and
     * return NULL
     */
    if (D.Errors > 0) {
        /* Free allocated stuff */
        unsigned I;
        for (I = 0; I < CollCount (&D.LineInfos); ++I) {
            FreeLineInfo (CollAt (&D.LineInfos, I));
        }
        DoneCollection (&D.LineInfos);
        FreeDbgInfo (D.Info);
        return 0;
    }

    /* We do now have all the information from the file. Do postprocessing. */
    ProcessFileInfo (&D);
    ProcessLineInfo (&D);

    /* Free the collection that contained the line info */
    DoneCollection (&D.LineInfos);

    /* Return the debug info struct that was created */
    return D.Info;
}



void cc65_free_dbginfo (cc65_dbginfo Handle)
/* Free debug information read from a file */
{
    if (Handle) {
        FreeDbgInfo (Handle);
    }
}



cc65_lineinfo* cc65_get_lineinfo (cc65_dbginfo Handle, unsigned long Addr)
/* Return line information for the given address. The function returns 0
 * if no line information was found.
 */
{
    unsigned        I;
    Collection*     FileInfos;
    cc65_lineinfo*  D = 0;

    /* We will place a list of line infos in a collection */
    Collection LineInfos = COLLECTION_INITIALIZER;

    /* Check the parameter */
    assert (Handle != 0);

    /* Walk over all files and search for matching line infos */
    FileInfos = &((DbgInfo*) Handle)->FileInfos;
    for (I = 0; I < CollCount (FileInfos); ++I) {
        /* Check if the file contains line info for this address */
        LineInfo* L = FindLineInfo (CollAt (FileInfos, I), Addr);
        if (L != 0) {
            CollAppend (&LineInfos, L);
        }
    }

    /* Do we have line infos? */
    if (CollCount (&LineInfos) > 0) {

        /* Prepare the struct we will return to the caller */
        D = xmalloc (sizeof (*D) +
                     (CollCount (&LineInfos) - 1) * sizeof (D->data[0]));
        D->count = CollCount (&LineInfos);
        for (I = 0; I < D->count; ++I) {

            /* Pointer to this info */
            LineInfo* L = CollAt (&LineInfos, I);

            /* Copy data */
            D->data[I].name  = L->FileInfo->FileName;
            D->data[I].size  = L->FileInfo->Size;
            D->data[I].mtime = L->FileInfo->MTime;
            D->data[I].line  = L->Line;
            D->data[I].start = L->Start;
            D->data[I].end   = L->End;
        }
    }

    /* Free the line info collection */
    DoneCollection (&LineInfos);

    /* Return the struct we've created */
    return D;
}



void cc65_free_lineinfo (cc65_dbginfo Handle, cc65_lineinfo* Info)
/* Free line info returned by cc65_get_lineinfo() */
{
    /* Just for completeness, check the handle */
    assert (Handle != 0);

    /* Just free the memory */
    xfree (Info);
}



