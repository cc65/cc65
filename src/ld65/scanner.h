/*****************************************************************************/
/*                                                                           */
/*				   scanner.h				     */
/*                                                                           */
/*		Configuration file scanner for the ld65 linker		     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998     Ullrich von Bassewitz                                        */
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
#define	CFGTOK_NONE		0
#define CFGTOK_INTCON		1
#define CFGTOK_STRCON		2
#define CFGTOK_IDENT		3
#define CFGTOK_LCURLY		4
#define CFGTOK_RCURLY		5
#define CFGTOK_SEMI		6
#define CFGTOK_COMMA		7
#define CFGTOK_EQ		8
#define CFGTOK_COLON  	 	9
#define CFGTOK_DOT		10
#define CFGTOK_EOF		11

/* Special identifiers */
#define CFGTOK_MEMORY          	20
#define CFGTOK_FILES      	21
#define CFGTOK_SEGMENTS   	22
#define CFGTOK_FORMATS		23

#define CFGTOK_START		30
#define CFGTOK_SIZE		31
#define CFGTOK_TYPE 		32
#define CFGTOK_FILE   		33
#define CFGTOK_DEFINE 		34
#define CFGTOK_FILL		35
#define CFGTOK_FILLVAL		36
#define CFGTOK_EXPORT 		37
#define CFGTOK_IMPORT		38
#define CFGTOK_OS      	       	39
#define CFGTOK_FORMAT		40

#define CFGTOK_LOAD   		50
#define CFGTOK_RUN     	       	51
#define CFGTOK_ALIGN   		52
#define CFGTOK_OFFSET  		53

#define CFGTOK_RO      	       	60
#define CFGTOK_RW      	       	61
#define CFGTOK_BSS     	       	62
#define CFGTOK_ZP		63
#define CFGTOK_WPROT		64

#define CFGTOK_O65     	       	70
#define CFGTOK_BIN     	       	71

#define CFGTOK_SMALL		80
#define CFGTOK_LARGE		81

#define CFGTOK_TRUE   		90
#define CFGTOK_FALSE  		91

#define CFGTOK_LUNIX		100
#define CFGTOK_OSA65		101



/* Mapping table entry, special identifier --> token */
typedef struct IdentTok_ IdentTok;
struct IdentTok_ {
    const char*	 	Ident;	     	/* Identifier */
    unsigned	 	Tok;	     	/* Token for identifier */
};
#define ENTRY_COUNT(s) 	(sizeof (s) / sizeof (s [0]))



/* Current token and attributes */
#define CFG_MAX_IDENT_LEN  255
extern unsigned		CfgTok;
extern char    	       	CfgSVal [CFG_MAX_IDENT_LEN+1];
extern unsigned long	CfgIVal;

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



