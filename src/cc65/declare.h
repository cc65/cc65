/*
 * declare.h
 *
 * Ullrich von Bassewitz, 20.06.1998
 */



#ifndef DECLARE_H
#define DECLARE_H



#include "scanner.h"
#include "symtab.h"



/*****************************************************************************/
/*		      		     Data		     		     */
/*****************************************************************************/



/* Masks for the Flags field in DeclSpec */
#define DS_DEF_STORAGE		0x0001U	/* Default storage class used */
#define DS_DEF_TYPE		0x0002U	/* Default type used */

/* Result of ParseDeclSpec */
typedef struct DeclSpec DeclSpec;
struct DeclSpec {
    unsigned	StorageClass;  	 	/* One of the SC_xxx flags */
    type	Type [MAXTYPELEN];	/* Type of the declaration spec */
    unsigned	Flags;			/* Bitmapped flags */
};

/* Result of ParseDecl */
typedef struct Declaration Declaration;
struct Declaration {
    ident     	Ident;			/* The identifier if any, else empty */
    type      	Type [MAXTYPELEN];	/* The type */

    /* Working variables */
    type*	T;			/* Used to build Type */
};

/* Modes for ParseDecl */
#define DM_NEED_IDENT	0U		/* We must have an identifier */
#define DM_NO_IDENT	1U		/* We won't read an identifier */
#define DM_ACCEPT_IDENT	2U		/* We will accept an id if there is one */



/*****************************************************************************/
/* 		      		     Code				     */
/*****************************************************************************/



type* ParseType (type* Type);
/* Parse a complete type specification */

void ParseDecl (const DeclSpec* Spec, Declaration* D, unsigned Mode);
/* Parse a variable, type or function declaration */

void ParseDeclSpec (DeclSpec* D, unsigned DefStorage, int DefType);
/* Parse a declaration specification */

void ParseInit (type* tptr);
/* Parse initialization of variables */



/* End of declare.h */

#endif



