/*****************************************************************************/
/*                                                                           */
/*				  symentry.c				     */
/*                                                                           */
/*		 Symbol table entries for the cc65 C compiler		     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000-2002 Ullrich von Bassewitz                                       */
/*               Wacholderweg 14                                             */
/*               D-70597 Stuttgart                                           */
/* EMail:        uz@cc65.org                                                 */
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
#include "anonname.h"
#include "symentry.h"



/*****************************************************************************/
/*	       	  	  	     Code				     */
/*****************************************************************************/



SymEntry* NewSymEntry (const char* Name, unsigned Flags)
/* Create a new symbol table with the given name */
{
    /* Get the length of the name */
    unsigned Len = strlen (Name);

    /* Allocate memory for the symbol entry */
    SymEntry* E = xmalloc (sizeof (SymEntry) + Len);

    /* Initialize the entry */
    E->NextHash	= 0;
    E->PrevSym	= 0;
    E->NextSym	= 0;
    E->Link	= 0;
    E->Owner	= 0;
    E->Flags	= Flags;
    E->Type	= 0;
    E->AsmName  = 0;
    memcpy (E->Name, Name, Len+1);

    /* Return the new entry */
    return E;
}



void FreeSymEntry (SymEntry* E)
/* Free a symbol entry */
{
    TypeFree (E->Type);
    xfree (E->AsmName);
    xfree (E);
}



void DumpSymEntry (FILE* F, const SymEntry* E)
/* Dump the given symbol table entry to the file in readable form */
{
    static const struct {
    	const char*    	    Name;
    	unsigned       	    Val;
    } Flags [] = {
	/* Beware: Order is important! */
      	{ "SC_TYPEDEF",	    SC_TYPEDEF	        },
       	{ "SC_STRUCTFIELD", SC_STRUCTFIELD	},
      	{ "SC_STRUCT", 	    SC_STRUCT	        },
    	{ "SC_AUTO",   	    SC_AUTO	        },
    	{ "SC_REGISTER",    SC_REGISTER	        },
    	{ "SC_STATIC",      SC_STATIC	        },
    	{ "SC_EXTERN", 	    SC_EXTERN	        },
    	{ "SC_ENUM",   	    SC_ENUM	        },
	{ "SC_CONST",  	    SC_CONST	        },
    	{ "SC_LABEL",	    SC_LABEL	        },
    	{ "SC_PARAM",	    SC_PARAM	        },
	{ "SC_FUNC",	    SC_FUNC	        },
    	{ "SC_STORAGE",	    SC_STORAGE 	        },
    	{ "SC_DEF",	    SC_DEF	        },
    	{ "SC_REF",	    SC_REF	        },
      	{ "SC_ZEROPAGE",    SC_ZEROPAGE	        },
    };

    unsigned I;
    unsigned SymFlags;

    /* Print the name */
    fprintf (F, "%s:\n", E->Name);

    /* Print the assembler name if we have one */
    if (E->AsmName) {
        fprintf (F, "    AsmName: %s\n", E->AsmName);
    }

    /* Print the flags */
    SymFlags = E->Flags;
    fprintf (F, "    Flags: ");
    for (I = 0; I < sizeof (Flags) / sizeof (Flags[0]) && SymFlags != 0; ++I) {
     	if ((SymFlags & Flags[I].Val) == Flags[I].Val) {
     	    SymFlags &= ~Flags[I].Val;
     	    fprintf (F, "%s ", Flags[I].Name);
     	}
    }
    if (SymFlags != 0) {
	fprintf (F, "%04X", SymFlags);
    }
    fprintf (F, "\n");

    /* Print the type */
    fprintf (F, "    Type:  ");
    if (E->Type) {
     	PrintType (F, E->Type);
    } else {
     	fprintf (F, "(none)");
    }
    fprintf (F, "\n");
}



void CvtRegVarToAuto (SymEntry* Sym)
/* Convert a register variable to an auto variable */
{
    /* Change the storage class */
    Sym->Flags = (Sym->Flags & ~(SC_REGISTER | SC_STATIC | SC_EXTERN)) | SC_AUTO;

    /* Transfer the stack offset from register save area to actual offset */
    Sym->V.Offs = Sym->V.R.SaveOffs;
}



void ChangeSymType (SymEntry* Entry, type* Type)
/* Change the type of the given symbol */
{
    TypeFree (Entry->Type);
    Entry->Type = TypeDup (Type);
}



void ChangeAsmName (SymEntry* Entry, const char* NewAsmName)
/* Change the assembler name of the symbol */
{
    xfree (Entry->AsmName);
    Entry->AsmName = xstrdup (NewAsmName);
}



int HasAnonName (const SymEntry* Entry)
/* Return true if the symbol entry has an anonymous name */
{
    return IsAnonName (Entry->Name);
}



