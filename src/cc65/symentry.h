/*****************************************************************************/
/*                                                                           */
/*				  symentry.h				     */
/*                                                                           */
/*		 Symbol table entries for the cc65 C compiler		     */
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



#ifndef SYMENTRY_H
#define SYMENTRY_H



#include <stdio.h>

#include "datatype.h"



/*****************************************************************************/
/*	       	  	       	struct SymEntry				     */
/*****************************************************************************/



/* Storage classes and flags */
#define SC_AUTO     	0x0001U
#define SC_REGISTER    	0x0002U	/* Register variable, is in static storage */
#define SC_STATIC    	0x0004U
#define SC_EXTERN    	0x0008U

#define SC_ENUM	     	0x0030U	/* An enum (numeric constant) */
#define SC_CONST	0x0020U	/* A numeric constant with a type */
#define SC_LABEL       	0x0040U	/* A goto label */
#define SC_PARAM       	0x0080U	/* This is a function parameter */
#define SC_FUNC		0x0100U	/* Function entry */

#define SC_STORAGE     	0x0400U	/* Symbol with associated storage */
#define SC_DEFAULT     	0x0800U	/* Flag: default storage class was used */

#define SC_DEF       	0x1000U	/* Symbol is defined */
#define SC_REF 	     	0x2000U /* Symbol is referenced */

#define SC_TYPE	       	0x4000U	/* This is a type, struct, typedef, etc. */
#define SC_STRUCT      	0x4001U	/* Struct or union */
#define SC_SFLD	       	0x4002U	/* Struct or union field */
#define SC_TYPEDEF     	0x4003U	/* A typedef */

#define SC_ZEROPAGE  	0x8000U	/* Symbol marked as zeropage */



/* Symbol table entry */
typedef struct SymEntry SymEntry;
struct SymEntry {
    SymEntry*  			NextHash; /* Next entry in hash list */
    SymEntry*  			PrevSym;  /* Previous symbol in dl list */
    SymEntry*  			NextSym;  /* Next symbol double linked list */
    SymEntry*  			Link;  	  /* General purpose single linked list */
    struct SymTable*		Owner; 	  /* Symbol table the symbol is in */
    unsigned   			Flags; 	  /* Symbol flags */
    type*      			Type;  	  /* Symbol type */

    /* Data that differs for the different symbol types */
    union {

	/* Offset for locals or struct members */
	int    			Offs;

	/* Label name for static symbols */
	unsigned		Label;

	/* Value for constants (including enums) */
       	long			ConstVal;

	/* Data for structs/unions */
	struct {
	    struct SymTable*	SymTab;	  /* Member symbol table */
	    unsigned		Size;  	  /* Size of the union/struct */
	} S;

	/* Data for functions */
	struct FuncDesc*	Func;	  /* Function descriptor */

    } V;
    char       	   	       Name[1];	/* Name, dynamically allocated */
};



/*****************************************************************************/
/*	       	  	       	     Code				     */
/*****************************************************************************/



SymEntry* NewSymEntry (const char* Name, unsigned Flags);
/* Create a new symbol table with the given name */

void FreeSymEntry (SymEntry* E);
/* Free a symbol entry */

void DumpSymEntry (FILE* F, const SymEntry* E);
/* Dump the given symbol table entry to the file in readable form */

int IsTypeDef (const SymEntry* E);
/* Return true if the given entry is a typedef entry */

void ChangeSymType (SymEntry* Entry, type* Type);
/* Change the type of the given symbol */



/* End of symentry.h */
#endif



