/*****************************************************************************/
/*                                                                           */
/*			   	  declattr.c				     */
/*                                                                           */
/*			    Declaration attributes			     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000-2002 Ullrich von Bassewitz                                       */
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



#include <string.h>

/* cc65 */
#include "error.h"
#include "scanner.h"
#include "symtab.h"
#include "typecmp.h"
#include "declattr.h"



/*****************************************************************************/
/*	   	    		     Data				     */
/*****************************************************************************/



/* Forwards for attribute handlers */
static void AliasAttr (const Declaration* D, DeclAttr* A);
static void UnusedAttr (const Declaration* D, DeclAttr* A);
static void ZeroPageAttr (const Declaration* D, DeclAttr* A);



/* Attribute table */
typedef struct AttrDesc AttrDesc;
struct AttrDesc {
    const char	Name[12];
    void	(*Handler) (const Declaration*, DeclAttr*);
};
static const AttrDesc AttrTable [atCount] = {
    { "alias",	       	AliasAttr	},
    { "unused",	       	UnusedAttr	},
    { "zeropage",      	ZeroPageAttr	},
};



/*****************************************************************************/
/* 	   	    	  	     Code				     */
/*****************************************************************************/



static const AttrDesc* FindAttribute (const char* Attr)
/* Search the attribute and return the corresponding attribute descriptor.
 * Return NULL if the attribute name is not known.
 */
{
    unsigned A;

    /* For now do a linear search */
    for (A = 0; A < atCount; ++A) {
       	if (strcmp (Attr, AttrTable[A].Name) == 0) {
	    /* Found */
       	    return AttrTable + A;
	}
    }

    /* Not found */
    return 0;
}



static void AliasAttr (const Declaration* D, DeclAttr* A)
/* Handle the "alias" attribute */
{
    SymEntry* Sym;

    /* Comma expected */
    ConsumeComma ();

    /* The next identifier is the name of the alias symbol */
    if (CurTok.Tok != TOK_IDENT) {
       	Error ("Identifier expected");
    	return;
    }

    /* Lookup the symbol for this name, it must exist */
    Sym = FindSym (CurTok.Ident);
    if (Sym == 0) {
    	Error ("Unknown identifier: `%s'", CurTok.Ident);
    	NextToken ();
    	return;
    }

    /* Since we have the symbol entry now, skip the name */
    NextToken ();

    /* Check if the types of the symbols are identical */
    if (TypeCmp (D->Type, Sym->Type) < TC_EQUAL) {
	/* Types are not identical */
	Error ("Incompatible types");
	return;
    }

    /* Attribute is verified, set the stuff in the attribute description */
    A->AttrType = atAlias;
    A->V.Sym	= Sym;
}



static void UnusedAttr (const Declaration* D attribute ((unused)), DeclAttr* A)
/* Handle the "unused" attribute */
{
    /* No parameters */
    A->AttrType = atUnused;
}



static void ZeroPageAttr (const Declaration* D attribute ((unused)), DeclAttr* A)
/* Handle the "zeropage" attribute */
{
    /* No parameters */
    A->AttrType = atZeroPage;
}



void ParseAttribute (const Declaration* D, DeclAttr* A)
/* Parse an additional __attribute__ modifier */
{
    ident    	    AttrName;
    const AttrDesc* Attr;

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
       	Error ("Identifier expected");
    	/* We should *really* try to recover here, but for now: */
    	return;
    }

    /* Map the attribute name to its id, then skip the identifier */
    strcpy (AttrName, CurTok.Ident);
    Attr = FindAttribute (AttrName);
    NextToken ();

    /* Did we find a valid attribute? */
    if (Attr) {

	/* Call the handler */
	Attr->Handler (D, A);

	/* Read the two closing braces */
	ConsumeRParen ();
	ConsumeRParen ();

    } else {
	/* List of tokens to skip */
	static const token_t SkipList[] = { TOK_LPAREN, TOK_SEMI };

	/* Attribute not known, maybe typo */
	Error ("Illegal attribute: `%s'", AttrName);

	/* Skip until closing brace or semicolon */
	SkipTokens (SkipList, sizeof (SkipList) / sizeof (SkipList[0]));

	/* If we have a closing brace, read it, otherwise bail out */
	if (CurTok.Tok == TOK_LPAREN) {
	    /* Read the two closing braces */
	    ConsumeRParen ();
	    ConsumeRParen ();
	}
    }
}



