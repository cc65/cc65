/*****************************************************************************/
/*                                                                           */
/*                                declattr.c                                 */
/*                                                                           */
/*                          Declaration attributes                           */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2009, Ullrich von Bassewitz                                      */
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



#include <string.h>

/* common */
#include "xmalloc.h"

/* cc65 */
#include "declare.h"
#include "declattr.h"
#include "error.h"
#include "scanner.h"
#include "symtab.h"
#include "typecmp.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Forwards for attribute handlers */
static void NoReturnAttr (Declaration* D);
static void UnusedAttr (Declaration* D);



/* Attribute table */
typedef struct AttrDesc AttrDesc;
struct AttrDesc {
    const char  Name[15];
    void        (*Handler) (Declaration*);
};
static const AttrDesc AttrTable [] = {
    { "__noreturn__",   NoReturnAttr    },
    { "__unused__",     UnusedAttr      },
    { "noreturn",       NoReturnAttr    },
    { "unused",         UnusedAttr      },
};



/*****************************************************************************/
/*                              Struct DeclAttr                              */
/*****************************************************************************/



static DeclAttr* NewDeclAttr (DeclAttrType AttrType)
/* Create a new DeclAttr struct and return it */
{
    /* Allocate memory */
    DeclAttr* A = xmalloc (sizeof (DeclAttr));

    /* Initialize the fields */
    A->AttrType = AttrType;

    /* Return the new struct */
    return A;
}



/*****************************************************************************/
/*                             Helper functions                              */
/*****************************************************************************/



static const AttrDesc* FindAttribute (const char* Attr)
/* Search the attribute and return the corresponding attribute descriptor.
** Return NULL if the attribute name is not known.
*/
{
    unsigned A;

    /* For now do a linear search */
    for (A = 0; A < sizeof (AttrTable) / sizeof (AttrTable[0]); ++A) {
        if (strcmp (Attr, AttrTable[A].Name) == 0) {
            /* Found */
            return AttrTable + A;
        }
    }

    /* Not found */
    return 0;
}



static void ErrorSkip (void)
{
    /* List of tokens to skip */
    static const token_t SkipList[] = { TOK_RPAREN, TOK_SEMI };

    /* Skip until closing brace or semicolon */
    SkipTokens (SkipList, sizeof (SkipList) / sizeof (SkipList[0]));

    /* If we have a closing brace, read it, otherwise bail out */
    if (CurTok.Tok == TOK_RPAREN) {
        /* Read the two closing braces */
        ConsumeRParen ();
        ConsumeRParen ();
    }
}



static void AddAttr (Declaration* D, DeclAttr* A)
/* Add an attribute to a declaration */
{
    /* Allocate the list if necessary, the add the attribute */
    if (D->Attributes == 0) {
        D->Attributes = NewCollection ();
    }
    CollAppend (D->Attributes, A);
}



/*****************************************************************************/
/*                          Attribute handling code                          */
/*****************************************************************************/



static void NoReturnAttr (Declaration* D)
/* Parse the "noreturn" attribute */
{
    /* Add the noreturn attribute */
    AddAttr (D, NewDeclAttr (atNoReturn));
}



static void UnusedAttr (Declaration* D)
/* Parse the "unused" attribute */
{
    /* Add the noreturn attribute */
    AddAttr (D, NewDeclAttr (atUnused));
}



void ParseAttribute (Declaration* D)
/* Parse an additional __attribute__ modifier */
{
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

    /* Read a list of attributes */
    while (1) {

        ident           AttrName;
        const AttrDesc* Attr = 0;

        /* Identifier follows */
        if (CurTok.Tok != TOK_IDENT) {

            /* No attribute name */
            Error ("Attribute name expected");

            /* Skip until end of attribute */
            ErrorSkip ();

            /* Bail out */
            return;
        }

        /* Map the attribute name to its id, then skip the identifier */
        strcpy (AttrName, CurTok.Ident);
        Attr = FindAttribute (AttrName);
        NextToken ();

        /* Did we find a valid attribute? */
        if (Attr) {

            /* Call the handler */
            Attr->Handler (D);

        } else {
            /* Attribute not known, maybe typo */
            Error ("Illegal attribute: `%s'", AttrName);

            /* Skip until end of attribute */
            ErrorSkip ();

            /* Bail out */
            return;
        }

        /* If a comma follows, there's a next attribute. Otherwise this is the
        ** end of the attribute list.
        */
        if (CurTok.Tok != TOK_COMMA) {
            break;
        }
        NextToken ();
    }

    /* The declaration is terminated with two closing braces */
    ConsumeRParen ();
    ConsumeRParen ();
}
