/*****************************************************************************/
/*                                                                           */
/*				    error.h				     */
/*                                                                           */
/*		  Error handling for the ca65 macroassembler		     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2000 Ullrich von Bassewitz                                       */
/*               Wacholderweg 14                                             */
/*               D-70597 Stuttgart                                           */
/* EMail:        uz@musoftware.de                                            */
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



#ifndef ERROR_H
#define ERROR_H



#include "scanner.h"



/*****************************************************************************/
/*	  		   	     Data		     		     */
/*****************************************************************************/



/* Warning numbers */
enum Warnings {
    WARN_NONE,	      	    	      	/* No warning */
    WARN_MASK_ERROR,
    WARN_SYM_NOT_REFERENCED,
    WARN_IMPORT_NOT_REFERENCED,
    WARN_CANNOT_TRACK_STATUS,
    WARN_COUNT	     	    	      	/* Warning count */
};

/* Error numbers */
enum Errors {
    ERR_NONE,  	       	       	      	/* No error */
    ERR_NOT_IMPLEMENTED,		/* Command/operation not implemented */
    ERR_CANNOT_OPEN_INCLUDE,
    ERR_INCLUDE_NESTING,
    ERR_INVALID_CHAR,
    ERR_HEX_DIGIT_EXPECTED,
    ERR_DIGIT_EXPECTED,
    ERR_01_EXPECTED,
    ERR_NUM_OVERFLOW,
    ERR_PSEUDO_EXPECTED,
    ERR_TOO_MANY_CHARS,
    ERR_COLON_EXPECTED,
    ERR_LPAREN_EXPECTED,
    ERR_RPAREN_EXPECTED,
    ERR_RBRACK_EXPECTED,
    ERR_COMMA_EXPECTED,
    ERR_ONOFF_EXPECTED,
    ERR_Y_EXPECTED,
    ERR_X_EXPECTED,
    ERR_INTCON_EXPECTED,
    ERR_STRCON_EXPECTED,
    ERR_CHARCON_EXPECTED,
    ERR_CONSTEXPR_EXPECTED,
    ERR_IDENT_EXPECTED,
    ERR_ENDMACRO_EXPECTED,
    ERR_OPTION_KEY_EXPECTED,
    ERR_816_MODE_ONLY,
    ERR_USER,
    ERR_STRING_TOO_LONG,
    ERR_NEWLINE_IN_STRING,
    ERR_ILLEGAL_CHARCON,
    ERR_ILLEGAL_ADDR_MODE,
    ERR_ILLEGAL_LOCALSTART,
    ERR_ILLEGAL_LOCAL_USE,
    ERR_ILLEGAL_SEGMENT,
    ERR_ILLEGAL_SEG_ATTR,
    ERR_ILLEGAL_MACPACK,
    ERR_ILLEGAL_FEATURE,
    ERR_SYNTAX,
    ERR_SYM_ALREADY_DEFINED,
    ERR_SYM_UNDEFINED,
    ERR_SYM_ALREADY_IMPORT,
    ERR_SYM_ALREADY_EXPORT,
    ERR_EXPORT_UNDEFINED,
    ERR_EXPORT_MUST_BE_CONST,
    ERR_IF_NESTING,
    ERR_UNEXPECTED_EOL,
    ERR_UNEXPECTED,
    ERR_DIV_BY_ZERO,
    ERR_MOD_BY_ZERO,
    ERR_RANGE,
    ERR_TOO_MANY_PARAMS,
    ERR_MACRO_PARAM_EXPECTED,
    ERR_CIRCULAR_REFERENCE,
    ERR_SYM_REDECL_MISMATCH,
    ERR_ALIGN,
    ERR_DUPLICATE_ELSE,
    ERR_OPEN_IF,
    ERR_OPEN_PROC,
    ERR_SEG_ATTR_MISMATCH,
    ERR_CPU_NOT_SUPPORTED,
    ERR_COUNTER_UNDERFLOW,
    ERR_UNDEFINED_LABEL,
    ERR_COUNT  	      	       	      	/* Error count */
};

/* Fatal errors */
enum Fatals {
    FAT_NONE,
    FAT_MAX_INPUT_FILES,
    FAT_OUT_OF_MEMORY,
    FAT_TOO_MANY_SEGMENTS,
    FAT_STRING_TOO_LONG,
    FAT_CANNOT_OPEN_INPUT,
    FAT_CANNOT_STAT_INPUT,
    FAT_CANNOT_OPEN_OUTPUT,
    FAT_CANNOT_WRITE_OUTPUT,
    FAT_CANNOT_OPEN_LISTING,
    FAT_CANNOT_WRITE_LISTING,
    FAT_CANNOT_READ_LISTING,
    FAT_MACRO_NESTING,
    FAT_TOO_MANY_SYMBOLS,
    FAT_COUNT	    	  	      	/* Fatal error count */
};



/* Warning levels */
extern unsigned        	WarnLevel;

/* Messages for internal compiler errors */
extern const char _MsgCheckFailed [];
extern const char _MsgPrecondition [];
extern const char _MsgFail [];

/* Statistics */
extern unsigned ErrorCount;
extern unsigned WarningCount;



/*****************************************************************************/
/*  	       	       	 	     Code				     */
/*****************************************************************************/



void Warning (unsigned WarnNum, ...);
/* Print warning message. */

void PWarning (const FilePos* Pos, unsigned WarnNum, ...);
/* Print warning message giving an explicit file and position. */

void Error (unsigned ErrNum, ...);
/* Print an error message */

void PError (const FilePos* Pos, unsigned ErrNum, ...);
/* Print an error message giving an explicit file and position. */

void ErrorSkip (unsigned ErrNum, ...);
/* Print an error message and skip the rest of the line */

void Fatal (unsigned FatNum, ...);
/* Print a message about a fatal error and die */

void Internal (const char* Format, ...);
/* Print a message about an internal compiler error and die. */

#define CHECK(c)       							\
    if (!(c)) 	       	       	       	       	       	     		\
 	Internal (_MsgCheckFailed, #c, c, __FILE__, __LINE__)

#define PRECONDITION(c)							\
    if (!(c)) 	       	       	       	       	       	     		\
       	Internal (_MsgPrecondition, #c, c, __FILE__, __LINE__)

#define FAIL(s)	       							\
    Internal (_MsgFail, s, __FILE__, __LINE__)



/* End of error.h */

#endif




