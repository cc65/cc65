/*****************************************************************************/
/*                                                                           */
/*				   scanner.h				     */
/*                                                                           */
/*	     Configuration file scanner for the da65 disassembler	     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000     Ullrich von Bassewitz                                        */
/*              Wacholderweg 14                                              */
/*              D-70597 Stuttgart                                            */
/* EMail:       uz@musoftware.de                                             */
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
/*     	       	       	       	     Data     				     */
/*****************************************************************************/



/* Config file tokens */
typedef enum token_t {
    CFGTOK_NONE,
    CFGTOK_INTCON,
    CFGTOK_STRCON,
    CFGTOK_IDENT,
    CFGTOK_LCURLY,
    CFGTOK_RCURLY,
    CFGTOK_SEMI,
    CFGTOK_COMMA,
    CFGTOK_EQ,
    CFGTOK_COLON,
    CFGTOK_DOT,
    CFGTOK_EOF,

    /* Special tokens */
    CFGTOK_GLOBAL,
    CFGTOK_RANGE,
    CFGTOK_LABEL,

    /* Global section */
    CFGTOK_INPUTNAME,
    CFGTOK_OUTPUTNAME,
    CFGTOK_PAGELENGTH,
    CFGTOK_STARTADDR,

    /* Range section */
    CFGTOK_START,
    CFGTOK_END,
    CFGTOK_TYPE,

    CFGTOK_CODE,
    CFGTOK_BYTETAB,
    CFGTOK_WORDTAB,
    CFGTOK_DWORDTAB,
    CFGTOK_ADDRTAB,
    CFGTOK_RTSTAB,

    /* Label section */
    CFGTOK_NAME,
    CFGTOK_ADDR,
    CFGTOK_SIZE,

    /* */
    CFGTOK_TRUE,
    CFGTOK_FALSE
} token_t;


/* Mapping table entry, special identifier --> token */
typedef struct IdentTok_ IdentTok;
struct IdentTok_ {
    const char*	 	Ident;	     	/* Identifier */
    token_t	 	Tok;	     	/* Token for identifier */
};
#define ENTRY_COUNT(s) 	(sizeof (s) / sizeof (s [0]))



/* Current token and attributes */
#define CFG_MAX_IDENT_LEN  255
extern unsigned		CfgTok;
extern char    	       	CfgSVal [CFG_MAX_IDENT_LEN+1];
extern long	        CfgIVal;

/* Error location */
extern unsigned        	CfgErrorLine;
extern unsigned        	CfgErrorCol;



/*****************************************************************************/
/*     	       	       	       	     Code     				     */
/*****************************************************************************/



void CfgWarning (const char* Format, ...);
/* Print a warning message adding file name and line number of the config file */

void CfgError (const char* Format, ...);
/* Print an error message adding file name and line number of the config file */

void CfgNextTok (void);
/* Read the next token from the input stream */

void CfgConsume (unsigned T, const char* Msg);
/* Skip a token, print an error message if not found */

void CfgConsumeLCurly (void);
/* Consume a left curly brace */

void CfgConsumeRCurly (void);
/* Consume a right curly brace */

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

void CfgRangeCheck (long Lo, long Hi);
/* Check the range of CfgIVal */

void CfgSpecialToken (const IdentTok* Table, unsigned Size, const char* Name);
/* Map an identifier to one of the special tokens in the table */

void CfgBoolToken (void);
/* Map an identifier or integer to a boolean token */

void CfgSetName (const char* Name);
/* Set a name for a config file */

const char* CfgGetName (void);
/* Get the name of the config file */

void CfgSetBuf (const char* Buf);
/* Set a memory buffer for the config */

int CfgAvail (void);
/* Return true if we have a configuration available */

void CfgOpenInput (void);
/* Open the input file if we have one */

void CfgCloseInput (void);
/* Close the input file if we have one */



/* End of scanner.h */
#endif



