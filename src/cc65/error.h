/*****************************************************************************/
/*                                                                           */
/*				    error.h				     */
/*                                                                           */
/*		    Error handling for the cc65 C compiler		     */
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



/* common */
#include "attrib.h"



/*****************************************************************************/
/*	  			     Data		     		     */
/*****************************************************************************/



/* Error numbers */
enum Errors {
    ERR_NONE,	       	       		/* No error */
    ERR_SYNTAX,
    ERR_QUOTE_EXPECTED,
    ERR_COLON_EXPECTED,
    ERR_SEMICOLON_EXPECTED,
    ERR_COMMA_EXPECTED,
    ERR_LPAREN_EXPECTED,
    ERR_RPAREN_EXPECTED,
    ERR_LBRACK_EXPECTED,
    ERR_RBRACK_EXPECTED,
    ERR_LCURLY_EXPECTED,
    ERR_RCURLY_EXPECTED,
    ERR_IDENT_EXPECTED,
    ERR_TYPE_EXPECTED,
    ERR_INCOMPATIBLE_TYPES,
    ERR_INCOMPATIBLE_POINTERS,
    ERR_TOO_MANY_FUNC_ARGS,
    ERR_TOO_FEW_FUNC_ARGS,
    ERR_DUPLICATE_MACRO_ARG,
    ERR_VAR_IDENT_EXPECTED,
    ERR_INT_EXPR_EXPECTED,
    ERR_CONST_EXPR_EXPECTED,
    ERR_NO_ACTIVE_LOOP,
    ERR_MULTIPLE_DEFINITION,
    ERR_CONFLICTING_TYPES,
    ERR_STRLIT_EXPECTED,
    ERR_WHILE_EXPECTED,
    ERR_MUST_RETURN_VALUE,
    ERR_CANNOT_RETURN_VALUE,
    ERR_UNEXPECTED_CONTINUE,
    ERR_UNDEFINED_SYMBOL,
    ERR_UNDEFINED_LABEL,
    ERR_TOO_MANY_LOCALS,
    ERR_TOO_MANY_INITIALIZERS,
    ERR_INIT_INCOMPLETE_TYPE,
    ERR_CANNOT_SUBSCRIPT,
    ERR_OP_NOT_ALLOWED,
    ERR_STRUCT_EXPECTED,
    ERR_STRUCT_FIELD_MISMATCH,
    ERR_STRUCT_PTR_EXPECTED,
    ERR_LVALUE_EXPECTED,
    ERR_EXPR_EXPECTED,
    ERR_CPP_EXPR_EXPECTED,
    ERR_ILLEGAL_TYPE,
    ERR_ILLEGAL_FUNC_CALL,
    ERR_ILLEGAL_INDIRECT,
    ERR_ILLEGAL_ADDRESS,
    ERR_ILLEGAL_HEX_DIGIT,
    ERR_ILLEGAL_CHARCONST,
    ERR_ILLEGAL_MODIFIER,
    ERR_ILLEGAL_QUALIFIER,
    ERR_ILLEGAL_STORAGE_CLASS,
    ERR_ILLEGAL_ATTRIBUTE,
    ERR_ILLEGAL_SEG_NAME,
    ERR_DIV_BY_ZERO,
    ERR_MOD_BY_ZERO,
    ERR_RANGE,
    ERR_SYMBOL_KIND,
    ERR_LEVEL_NESTING,
    ERR_MISSING_PARAM_NAME,
    ERR_OLD_STYLE_PROTO,
    ERR_PARAM_DECL,
    ERR_CANNOT_TAKE_ADDR_OF_REG,
    ERR_ILLEGAL_SIZE,
    ERR_FASTCALL,
    ERR_UNKNOWN_SIZE,
    ERR_UNKNOWN_IDENT,
    ERR_DUPLICATE_QUALIFIER,
    ERR_CONST_ASSIGN,
    ERR_QUAL_DIFF,
    ERR_COUNT 	     	    	   	/* Error count */
};

/* Count of errors/warnings */
extern unsigned ErrorCount;
extern unsigned WarningCount;



/*****************************************************************************/
/* 	       	      	 	     code				     */
/*****************************************************************************/



void Warning (const char* Format, ...) attribute ((format (printf, 1, 2)));
/* Print warning message. */

void PPWarning (const char* Format, ...) attribute ((format (printf, 1, 2)));
/* Print warning message. For use within the preprocessor. */

void Error (unsigned ErrNum, ...);
/* Print an error message */

void MError (const char* Format, ...) attribute ((format (printf, 1, 2)));
/* Print an error message */

void PPError (const char* Format, ...) attribute ((format (printf, 1, 2)));
/* Print an error message. For use within the preprocessor.  */

void Fatal (const char* Format, ...) attribute ((noreturn, format (printf, 1, 2)));
/* Print a message about a fatal error and die */

void Internal (char* Format, ...) attribute ((noreturn, format (printf, 1, 2)));
/* Print a message about an internal compiler error and die. */

void ErrorReport (void);
/* Report errors (called at end of compile) */



/* End of error.h */
#endif





