/*****************************************************************************/
/*                                                                           */
/*				  declattr.c				     */
/*                                                                           */
/*			    Declaration attributes			     */
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



/* cc65 */
#include "error.h"
#include "scanner.h"
#include "symtab.h"
#include "typecmp.h"
#include "declattr.h"



/*****************************************************************************/
/*	   	    		     Data				     */
/*****************************************************************************/



/* Attribute names */
static const char* const AttrNames [atCount] = {
    "alias",
};



/*****************************************************************************/
/* 	   	    	  	     Code				     */
/*****************************************************************************/



static attrib_t FindAttribute (const char* Attr)
/* Search the attribute and return the corresponding attribute constant.
 * Return atNone if the attribute name is not known.
 */
{
    attrib_t A;

    /* For now do a linear search */
    for (A = 0; A < atCount; ++A) {
	if (strcmp (Attr, AttrNames[A]) == 0) {
	    /* Found */
       	    return A;
	}
    }

    /* Not found */
    return atNone;
}



static void AliasAttrib (const Declaration* D, DeclAttr* A)
/* Handle the "alias" attribute */
{
    SymEntry* Sym;

    /* Comma expected */
    ConsumeComma ();

    /* The next identifier is the name of the alias symbol */
    if (CurTok.Tok != TOK_IDENT) {
       	Error (ERR_IDENT_EXPECTED);
    	return;
    }

    /* Lookup the symbol for this name, it must exist */
    Sym = FindSym (CurTok.Ident);
    if (Sym == 0) {
    	Error (ERR_UNKNOWN_IDENT, CurTok.Ident);
    	NextToken ();
    	return;
    }

    /* Since we have the symbol entry now, skip the name */
    NextToken ();

    /* Check if the types of the symbols are identical */
    if (TypeCmp (D->Type, Sym->Type) < TC_EQUAL) {
	/* Types are not identical */
	Error (ERR_INCOMPATIBLE_TYPES);
	return;
    }

    /* Attribute is verified, set the stuff in the attribute description */
    A->AttrType = atAlias;
    A->V.Sym	= Sym;
}



void ParseAttribute (const Declaration* D, DeclAttr* A)
/* Parse an additional __attribute__ modifier */
{
    attrib_t AttrType;

    /* Initialize the attribute description with "no attribute" */
    A->AttrType = atNone;

    /* Do we have an attribute? */
    if (CurTok.Tok != TOK_ATTRIBUTE) {
    	/* No attribute, bail out */
    	return;
    }

    /* Skip the attribute token */
    NextToken ();

    /* Expect two(!) open braces */
    ConsumeLParen ();
    ConsumeLParen ();

    /* Identifier follows */
    if (CurTok.Tok != TOK_IDENT) {
       	Error (ERR_IDENT_EXPECTED);
    	/* We should *really* try to recover here, but for now: */
    	return;
    }

    /* Map the attribute name to its id, then skip the identifier */
    AttrType = FindAttribute (CurTok.Ident);
    NextToken ();

    /* Handle possible attributes */
    switch (AttrType) {

	case atAlias:
	    AliasAttrib (D, A);
	    break;

	default:
	    /* Attribute not known, maybe typo */
	    Error (ERR_ILLEGAL_ATTRIBUTE);
	    break;
    }

    /* Read the two closing braces */
    ConsumeRParen ();
    ConsumeRParen ();
}



