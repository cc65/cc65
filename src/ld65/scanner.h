/*****************************************************************************/
/*                                                                           */
/*                                 scanner.h                                 */
/*                                                                           */
/*              Configuration file scanner for the ld65 linker               */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2013, Ullrich von Bassewitz                                      */
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



#ifndef SCANNER_H
#define SCANNER_H



/* common */
#include "filepos.h"
#include "strbuf.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Config file tokens */
typedef enum {
    CFGTOK_NONE,
    CFGTOK_INTCON,              /* Integer constant */
    CFGTOK_STRCON,              /* String constant */
    CFGTOK_IDENT,               /* Identifier */
    CFGTOK_PLUS,
    CFGTOK_MINUS,
    CFGTOK_MUL,
    CFGTOK_DIV,
    CFGTOK_LPAR,
    CFGTOK_RPAR,
    CFGTOK_LCURLY,
    CFGTOK_RCURLY,
    CFGTOK_SEMI,
    CFGTOK_COMMA,
    CFGTOK_EQ,
    CFGTOK_COLON,
    CFGTOK_DOT,
    CFGTOK_EOF,

    /* Special identifiers */
    CFGTOK_MEMORY,
    CFGTOK_FILES,
    CFGTOK_SEGMENTS,
    CFGTOK_FORMATS,
    CFGTOK_FEATURES,
    CFGTOK_SYMBOLS,

    CFGTOK_START,
    CFGTOK_SIZE,
    CFGTOK_TYPE,
    CFGTOK_FILE,
    CFGTOK_DEFINE,
    CFGTOK_BANK,
    CFGTOK_FILL,
    CFGTOK_FILLVAL,
    CFGTOK_EXPORT,
    CFGTOK_IMPORT,
    CFGTOK_OS,
    CFGTOK_ID,
    CFGTOK_VERSION,
    CFGTOK_FORMAT,

    CFGTOK_LOAD,
    CFGTOK_RUN,
    CFGTOK_ALIGN,
    CFGTOK_ALIGN_LOAD,
    CFGTOK_OFFSET,
    CFGTOK_OPTIONAL,

    CFGTOK_RO,
    CFGTOK_RW,
    CFGTOK_BSS,
    CFGTOK_ZP,

    CFGTOK_O65,
    CFGTOK_BIN,

    CFGTOK_SMALL,
    CFGTOK_LARGE,

    CFGTOK_TRUE,
    CFGTOK_FALSE,

    CFGTOK_LUNIX,
    CFGTOK_OSA65,
    CFGTOK_CC65,
    CFGTOK_OPENCBM,

    CFGTOK_CONDES,
    CFGTOK_STARTADDRESS,

    CFGTOK_ADDRSIZE,
    CFGTOK_VALUE,

    CFGTOK_WEAK,

    CFGTOK_ABS,
    CFGTOK_FAR,
    CFGTOK_LONG,

    CFGTOK_SEGMENT,
    CFGTOK_LABEL,
    CFGTOK_COUNT,
    CFGTOK_ORDER,

    CFGTOK_CONSTRUCTOR,
    CFGTOK_DESTRUCTOR,
    CFGTOK_INTERRUPTOR,

    CFGTOK_DECREASING,
    CFGTOK_INCREASING,

    CFGTOK_DEFAULT

} cfgtok_t;



/* Mapping table entry, special identifier --> token */
typedef struct IdentTok IdentTok;
struct IdentTok {
    const char* Ident;          /* Identifier */
    cfgtok_t    Tok;            /* Token for identifier */
};
#define ENTRY_COUNT(s)  (sizeof (s) / sizeof (s [0]))



/* Current token and attributes */
extern cfgtok_t         CfgTok;
extern StrBuf           CfgSVal;
extern unsigned long    CfgIVal;

/* Error location. PLEASE NOTE: I'm abusing the FilePos structure to some
** degree. It is used mostly to hold a file position, where the Name member
** is an index into the source file table of an object file. As used in config
** file processing, the Name member is a string pool index instead. This is
** distinguished by the object file pointer being NULL or not in the structs
** where this is relevant.
*/
extern FilePos          CfgErrorPos;



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



void CfgWarning (const FilePos* Pos, const char* Format, ...) attribute((format(printf,2,3)));
/* Print a warning message adding file name and line number of the config file */

void CfgError (const FilePos* Pos, const char* Format, ...) attribute((format(printf,2,3)));
/* Print an error message adding file name and line number of a given file */

void CfgNextTok (void);
/* Read the next token from the input stream */

void CfgConsume (cfgtok_t T, const char* Msg);
/* Skip a token, print an error message if not found */

void CfgConsumeSemi (void);
/* Consume a semicolon */

void CfgConsumeColon (void);
/* Consume a colon */

void CfgOptionalComma (void);
/* Consume a comma if there is one */

void CfgOptionalAssign (void);
/* Consume an equal sign if there is one */

void CfgAssureInt (void);
/* Make sure the next token is an integer */

void CfgAssureStr (void);
/* Make sure the next token is a string constant */

void CfgAssureIdent (void);
/* Make sure the next token is an identifier */

void CfgRangeCheck (unsigned long Lo, unsigned long Hi);
/* Check the range of CfgIVal */

void CfgSpecialToken (const IdentTok* Table, unsigned Size, const char* Name);
/* Map an identifier to one of the special tokens in the table */

void CfgBoolToken (void);
/* Map an identifier or integer to a boolean token */

void CfgSetName (const char* Name);
/* Set a name for a config file */

int CfgAvail (void);
/* Return true if we have a configuration available */

void CfgOpenInput (void);
/* Open the input file if we have one */

void CfgCloseInput (void);
/* Close the input file if we have one */



/* End of scanner.h */

#endif
