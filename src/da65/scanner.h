/*****************************************************************************/
/*                                                                           */
/*                                 scanner.h                                 */
/*                                                                           */
/*           Configuration file scanner for the da65 disassembler            */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000-2011, Ullrich von Bassewitz                                      */
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



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Info file tokens */
typedef enum token_t {
    INFOTOK_NONE,
    INFOTOK_INTCON,
    INFOTOK_STRCON,
    INFOTOK_CHARCON,
    INFOTOK_IDENT,
    INFOTOK_LCURLY,
    INFOTOK_RCURLY,
    INFOTOK_SEMI,
    INFOTOK_COMMA,
    INFOTOK_EQ,
    INFOTOK_COLON,
    INFOTOK_DOT,
    INFOTOK_EOF,

    /* Special tokens */
    INFOTOK_GLOBAL,
    INFOTOK_RANGE,
    INFOTOK_LABEL,
    INFOTOK_ASMINC,
    INFOTOK_SEGMENT,

    /* Global section */
    INFOTOK_ARGUMENT_COLUMN,
    INFOTOK_COMMENT_COLUMN,
    INFOTOK_COMMENTS,
    INFOTOK_CPU,
    INFOTOK_HEXOFFS,
    INFOTOK_INPUTNAME,
    INFOTOK_INPUTOFFS,
    INFOTOK_INPUTSIZE,
    INFOTOK_LABELBREAK,
    INFOTOK_MNEMONIC_COLUMN,
    INFOTOK_NL_AFTER_JMP,
    INFOTOK_NL_AFTER_RTS,
    INFOTOK_OUTPUTNAME,
    INFOTOK_PAGELENGTH,
    INFOTOK_STARTADDR,
    INFOTOK_TEXT_COLUMN,

    /* Range section */
    INFOTOK_START,
    INFOTOK_END,
    INFOTOK_TYPE,

    INFOTOK_CODE,
    INFOTOK_BYTETAB,
    INFOTOK_DBYTETAB,
    INFOTOK_WORDTAB,
    INFOTOK_DWORDTAB,
    INFOTOK_ADDRTAB,
    INFOTOK_RTSTAB,
    INFOTOK_TEXTTAB,
    INFOTOK_SKIP,

    /* Label section */
    INFOTOK_NAME,
    INFOTOK_COMMENT,
    INFOTOK_ADDR,
    INFOTOK_SIZE,

    /* ASMINC section */
    INFOTOK_FILE,
    INFOTOK_COMMENTSTART,
    INFOTOK_IGNOREUNKNOWN,

    /* */
    INFOTOK_TRUE,
    INFOTOK_FALSE
} token_t;


/* Mapping table entry, special identifier --> token */
typedef struct IdentTok IdentTok;
struct IdentTok {
    const char*         Ident;          /* Identifier */
    token_t             Tok;            /* Token for identifier */
};
#define ENTRY_COUNT(s)  (sizeof (s) / sizeof (s [0]))



/* Current token and attributes */
#define CFG_MAX_IDENT_LEN  255
extern unsigned         InfoTok;
extern char             InfoSVal[CFG_MAX_IDENT_LEN+1];
extern long             InfoIVal;

/* Error location */
extern unsigned         InfoErrorLine;
extern unsigned         InfoErrorCol;



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



void InfoWarning (const char* Format, ...);
/* Print a warning message adding file name and line number of the config file */

void InfoError (const char* Format, ...);
/* Print an error message adding file name and line number of the config file */

void InfoNextTok (void);
/* Read the next token from the input stream */

void InfoConsume (unsigned T, const char* Msg);
/* Skip a token, print an error message if not found */

void InfoConsumeLCurly (void);
/* Consume a left curly brace */

void InfoConsumeRCurly (void);
/* Consume a right curly brace */

void InfoConsumeSemi (void);
/* Consume a semicolon */

void InfoConsumeColon (void);
/* Consume a colon */

void InfoOptionalComma (void);
/* Consume a comma if there is one */

void InfoOptionalAssign (void);
/* Consume an equal sign if there is one */

void InfoAssureInt (void);
/* Make sure the next token is an integer */

void InfoAssureStr (void);
/* Make sure the next token is a string constant */

void InfoAssureChar (void);
/* Make sure the next token is a char constant */

void InfoAssureIdent (void);
/* Make sure the next token is an identifier */

void InfoRangeCheck (long Lo, long Hi);
/* Check the range of InfoIVal */

void InfoSpecialToken (const IdentTok* Table, unsigned Size, const char* Name);
/* Map an identifier to one of the special tokens in the table */

void InfoBoolToken (void);
/* Map an identifier or integer to a boolean token */

void InfoSetName (const char* Name);
/* Set a name for a config file */

const char* InfoGetName (void);
/* Get the name of the config file */

int InfoAvail ();
/* Return true if we have an info file given */

void InfoOpenInput (void);
/* Open the input file if we have one */

void InfoCloseInput (void);
/* Close the input file if we have one */



/* End of scanner.h */

#endif
