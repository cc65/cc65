/*****************************************************************************/
/*                                                                           */
/*				   symtab.c				     */
/*                                                                           */
/*		Symbol table management for the cc65 C compiler		     */
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



#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

/* common */
#include "check.h"
#include "hashstr.h"
#include "xmalloc.h"

/* cc65 */
#include "asmcode.h"
#include "asmlabel.h"
#include "codegen.h"
#include "datatype.h"
#include "declare.h"
#include "error.h"
#include "funcdesc.h"
#include "global.h"
#include "symentry.h"
#include "typecmp.h"
#include "symtab.h"



/*****************************************************************************/
/*	       	      	   	     Data				     */
/*****************************************************************************/



/* An empty symbol table */
SymTable	EmptySymTab = {
    0, 		/* PrevTab */
    0,		/* SymHead */
    0, 		/* SymTail */
    0,		/* SymCount */
    1,		/* Size */
    { 0	}	/* Tab[1] */
};

/* Symbol table sizes */
#define SYMTAB_SIZE_GLOBAL     	211U
#define SYMTAB_SIZE_FUNCTION	 29U
#define SYMTAB_SIZE_BLOCK	 13U
#define SYMTAB_SIZE_STRUCT	 19U
#define SYMTAB_SIZE_LABEL	  7U

/* Predefined lexical levels */
#define LEX_LEVEL_GLOBAL	1U

/* The current and root symbol tables */
static unsigned		LexicalLevel   	= 0;	/* For safety checks */
static SymTable*	SymTab0	       	= 0;
static SymTable*	SymTab         	= 0;
static SymTable*	TagTab0     	= 0;
static SymTable*	TagTab      	= 0;
static SymTable*	LabelTab       	= 0;



/*****************************************************************************/
/*	     			struct SymTable				     */
/*****************************************************************************/



static SymTable* NewSymTable (unsigned Size)
/* Create and return a symbol table for the given lexical level */
{
    unsigned I;

    /* Allocate memory for the table */
    SymTable* S = xmalloc (sizeof (SymTable) + (Size-1) * sizeof (SymEntry*));

    /* Initialize the symbol table structure */
    S->PrevTab	= 0;
    S->SymHead	= 0;
    S->SymTail	= 0;
    S->SymCount	= 0;
    S->Size	= Size;
    for (I = 0; I < Size; ++I) {
    	S->Tab[I] = 0;
    }

    /* Return the symbol table */
    return S;
}



static void FreeSymTable (SymTable* S)
/* Free the given symbo table including all symbols */
{
    /* Free all symbols */
    SymEntry* Sym = S->SymHead;
    while (Sym) {
	SymEntry* NextSym = Sym->NextSym;
	FreeSymEntry (Sym);
	Sym = NextSym;
    }

    /* Free the table itself */
    xfree (S);
}



/*****************************************************************************/
/*	       		   Check symbols in a table		       	     */
/*****************************************************************************/



static void CheckSymTable (SymTable* Tab)
/* Check a symbol table for open references, unused symbols ... */
{
    SymEntry* Entry = Tab->SymHead;
    while (Entry) {

     	/* Get the storage flags for tne entry */
     	unsigned Flags = Entry->Flags;

	/* Ignore typedef entries */
	if ((Flags & SC_TYPEDEF) != SC_TYPEDEF) {

	    /* Check if the symbol is one with storage, and it if it was
	     * defined but not used.
	     */
	    if (((Flags & SC_AUTO) || (Flags & SC_STATIC)) && (Flags & SC_EXTERN) == 0) {
		if ((Flags & SC_DEF) && !(Flags & SC_REF)) {
		    if (Flags & SC_PARAM) {
			Warning ("Parameter `%s' is never used", Entry->Name);
		    } else {
			Warning ("`%s' is defined but never used", Entry->Name);
		    }
		}
	    }

	    /* If the entry is a label, check if it was defined in the function */
	    if (Flags & SC_LABEL) {
		if ((Flags & SC_DEF) == 0) {
		    /* Undefined label */
		    Error (ERR_UNDEFINED_LABEL, Entry->Name);
		} else if ((Flags & SC_REF) == 0) {
		    /* Defined but not used */
		    Warning ("`%s' is defined but never used", Entry->Name);
		}
	    }

	}

     	/* Next entry */
     	Entry = Entry->NextSym;
    }
}



/*****************************************************************************/
/*			  Handling of lexical levels			     */
/*****************************************************************************/



void EnterGlobalLevel (void)
/* Enter the program global lexical level */
{
    /* Safety */
    PRECONDITION (++LexicalLevel == LEX_LEVEL_GLOBAL);

    /* Create and assign the symbol table */
    SymTab0 = SymTab = NewSymTable (SYMTAB_SIZE_GLOBAL);

    /* Create and assign the tag table */
    TagTab0 = TagTab = NewSymTable (SYMTAB_SIZE_GLOBAL);
}



void LeaveGlobalLevel (void)
/* Leave the program global lexical level */
{
    /* Safety */
    PRECONDITION (LexicalLevel-- == LEX_LEVEL_GLOBAL);

    /* Check the tables */
    CheckSymTable (SymTab0);

    /* Dump the tables if requested */
    if (Debug) {
	PrintSymTable (SymTab0, stdout, "Global symbol table");
	PrintSymTable (TagTab0, stdout, "Global tag table");
    }

    /* Don't delete the symbol and struct tables! */
    SymTab0 = SymTab = 0;
    TagTab0 = TagTab = 0;
}



void EnterFunctionLevel (void)
/* Enter function lexical level */
{
    SymTable* S;

    /* New lexical level */
    ++LexicalLevel;

    /* Get a new symbol table and make it current */
    S = NewSymTable (SYMTAB_SIZE_FUNCTION);
    S->PrevTab = SymTab;
    SymTab     = S;

    /* Get a new tag table and make it current */
    S = NewSymTable (SYMTAB_SIZE_FUNCTION);
    S->PrevTab = TagTab;
    TagTab  = S;

    /* Create and assign a new label table */
    LabelTab = NewSymTable (SYMTAB_SIZE_LABEL);
}



void RememberFunctionLevel (struct FuncDesc* F)
/* Remember the symbol tables for the level and leave the level without checks */
{
    /* Leave the lexical level */
    --LexicalLevel;

    /* Remember the tables */
    F->SymTab = SymTab;
    F->TagTab = TagTab;

    /* Don't delete the tables */
    SymTab = SymTab->PrevTab;
    TagTab = TagTab->PrevTab;
}



void ReenterFunctionLevel (struct FuncDesc* F)
/* Reenter the function lexical level using the existing tables from F */
{
    /* New lexical level */
    ++LexicalLevel;

    /* Make the tables current again */
    F->SymTab->PrevTab = SymTab;
    SymTab = F->SymTab;

    F->TagTab->PrevTab = TagTab;
    TagTab = F->TagTab;

    /* Create and assign a new label table */
    LabelTab = NewSymTable (SYMTAB_SIZE_LABEL);
}



void LeaveFunctionLevel (void)
/* Leave function lexical level */
{
    /* Leave the lexical level */
    --LexicalLevel;

    /* Check the tables */
    CheckSymTable (SymTab);
    CheckSymTable (LabelTab);

    /* Drop the label table if it is empty */
    if (LabelTab->SymCount == 0) {
	FreeSymTable (LabelTab);
    }

    /* Don't delete the tables */
    SymTab = SymTab->PrevTab;
    TagTab = TagTab->PrevTab;
    LabelTab  = 0;
}



void EnterBlockLevel (void)
/* Enter a nested block in a function */
{
    SymTable* S;

    /* New lexical level */
    ++LexicalLevel;

    /* Get a new symbol table and make it current */
    S = NewSymTable (SYMTAB_SIZE_BLOCK);
    S->PrevTab 	= SymTab;
    SymTab     	= S;

    /* Get a new tag table and make it current */
    S = NewSymTable (SYMTAB_SIZE_BLOCK);
    S->PrevTab = TagTab;
    TagTab     = S;
}



void LeaveBlockLevel (void)
/* Leave a nested block in a function */
{
    /* Leave the lexical level */
    --LexicalLevel;

    /* Check the tables */
    CheckSymTable (SymTab);

    /* Don't delete the tables */
    SymTab = SymTab->PrevTab;
    TagTab = TagTab->PrevTab;
}



void EnterStructLevel (void)
/* Enter a nested block for a struct definition */
{
    SymTable* S;

    /* Get a new symbol table and make it current. Note: Structs and enums
     * nested in struct scope are NOT local to the struct but visible in the
     * outside scope. So we will NOT create a new struct or enum table.
     */
    S = NewSymTable (SYMTAB_SIZE_BLOCK);
    S->PrevTab 	= SymTab;
    SymTab     	= S;
}



void LeaveStructLevel (void)
/* Leave a nested block for a struct definition */
{
    /* Don't delete the table */
    SymTab = SymTab->PrevTab;
}



/*****************************************************************************/
/*	       	      	       	Find functions				     */
/*****************************************************************************/



static SymEntry* FindSymInTable (const SymTable* T, const char* Name, unsigned Hash)
/* Search for an entry in one table */
{
    /* Get the start of the hash chain */
    SymEntry* E = T->Tab [Hash % T->Size];
    while (E) {
	/* Compare the name */
	if (strcmp (E->Name, Name) == 0) {
	    /* Found */
	    return E;
	}
 	/* Not found, next entry in hash chain */
	E = E->NextHash;
    }

    /* Not found */
    return 0;
}



static SymEntry* FindSymInTree (const SymTable* Tab, const char* Name)
/* Find the symbol with the given name in the table tree that starts with T */
{
    /* Get the hash over the name */
    unsigned Hash = HashStr (Name);

    /* Check all symbol tables for the symbol */
    while (Tab) {
	/* Try to find the symbol in this table */
	SymEntry* E = FindSymInTable (Tab, Name, Hash);

	/* Bail out if we found it */
	if (E != 0) {
	    return E;
	}

	/* Repeat the search in the next higher lexical level */
	Tab = Tab->PrevTab;
    }

    /* Not found */
    return 0;
}



SymEntry* FindSym (const char* Name)
/* Find the symbol with the given name */
{
    return FindSymInTree (SymTab, Name);
}



SymEntry* FindLocalSym (const char* Name)
/* Find the symbol with the given name in the current symbol table only */
{
    return FindSymInTable (SymTab, Name, HashStr (Name));
}



SymEntry* FindTagSym (const char* Name)
/* Find the symbol with the given name in the tag table */
{
    return FindSymInTree (TagTab, Name);
}



SymEntry* FindStructField (const type* Type, const char* Name)
/* Find a struct field in the fields list */
{
    SymEntry* Field = 0;

    /* The given type may actually be a pointer to struct */
    if (Type[0] == T_PTR) {
    	++Type;
    }

    /* Non-structs do not have any struct fields... */
    if (IsClassStruct (Type)) {

    	const SymTable* Tab;

    	/* Get a pointer to the struct/union type */
     	const SymEntry* Struct = (const SymEntry*) Decode (Type+1);
    	CHECK (Struct != 0);

    	/* Get the field symbol table from the struct entry.
    	 * Beware: The table may not exist.
    	 */
    	Tab = Struct->V.S.SymTab;

    	/* Now search in the struct symbol table */
    	if (Tab) {
       	    Field = FindSymInTable (Struct->V.S.SymTab, Name, HashStr (Name));
    	}
    }

    return Field;
}



/*****************************************************************************/
/*	  		 Add stuff to the symbol table			     */
/*****************************************************************************/



static void AddSymEntry (SymTable* T, SymEntry* S)
/* Add a symbol to a symbol table */
{
    /* Get the hash value for the name */
    unsigned Hash = HashStr (S->Name) % T->Size;

    /* Insert the symbol into the list of all symbols in this level */
    if (T->SymTail) {
       	T->SymTail->NextSym = S;
    }
    S->PrevSym = T->SymTail;
    T->SymTail = S;
    if (T->SymHead == 0) {
	/* First symbol */
	T->SymHead = S;
    }
    T->SymCount++;

    /* Insert the symbol into the hash chain */
    S->NextHash  = T->Tab[Hash];
    T->Tab[Hash] = S;

    /* Tell the symbol in which table it is */
    S->Owner = T;
}



SymEntry* AddStructSym (const char* Name, unsigned Size, SymTable* Tab)
/* Add a struct/union entry and return it */
{
    /* Do we have an entry with this name already? */
    SymEntry* Entry = FindSymInTable (TagTab, Name, HashStr (Name));
    if (Entry) {

     	/* We do have an entry. This may be a forward, so check it. */
     	if ((Entry->Flags & SC_STRUCT) == 0) {
	    /* Existing symbol is not a struct */
	    Error (ERR_SYMBOL_KIND);
	} else if (Size > 0 && Entry->V.S.Size > 0) {
     	    /* Both structs are definitions. */
     	    Error (ERR_MULTIPLE_DEFINITION, Name);
     	} else {
	    /* Define the struct size if it is given */
	    if (Size > 0) {
	  	Entry->V.S.SymTab = Tab;
		Entry->V.S.Size   = Size;
	    }
	}

    } else {

    	/* Create a new entry */
    	Entry = NewSymEntry (Name, SC_STRUCT);

    	/* Set the struct data */
    	Entry->V.S.SymTab = Tab;
 	Entry->V.S.Size	  = Size;

    	/* Add it to the current table */
    	AddSymEntry (TagTab, Entry);
    }

    /* Return the entry */
    return Entry;
}



SymEntry* AddEnumSym (const char* Name, int Val)
/* Add an enum symbol to the symbol table and return it */
{
    /* Do we have an entry with this name already? */
    SymEntry* Entry = FindSymInTable (SymTab, Name, HashStr (Name));
    if (Entry) {
	if (Entry->Flags != SC_ENUM) {
	    Error (ERR_SYMBOL_KIND);
	} else {
	    Error (ERR_MULTIPLE_DEFINITION, Name);
	}
	return Entry;
    }

    /* Create a new entry */
    Entry = NewSymEntry (Name, SC_ENUM);

    /* Enum values are ints */
    Entry->Type	= TypeDup (type_int);

    /* Set the enum data */
    Entry->V.EnumVal = Val;

    /* Add the entry to the symbol table */
    AddSymEntry (SymTab, Entry);

    /* Return the entry */
    return Entry;
}



SymEntry* AddLabelSym (const char* Name, unsigned Flags)
/* Add a goto label to the label table */
{
    /* Do we have an entry with this name already? */
    SymEntry* Entry = FindSymInTable (LabelTab, Name, HashStr (Name));
    if (Entry) {

     	if ((Entry->Flags & SC_DEF) != 0 && (Flags & SC_DEF) != 0) {
     	    /* Trying to define the label more than once */
       	    Error (ERR_MULTIPLE_DEFINITION, Name);
     	}
     	Entry->Flags |= Flags;

    } else {

     	/* Create a new entry */
     	Entry = NewSymEntry (Name, SC_LABEL | Flags);

     	/* Set a new label number */
     	Entry->V.Label = GetLabel ();

     	/* Add the entry to the label table */
     	AddSymEntry (LabelTab, Entry);

    }

    /* Return the entry */
    return Entry;
}



SymEntry* AddLocalSym (const char* Name, type* Type, unsigned Flags, int Offs)
/* Add a local symbol and return the symbol entry */
{
    /* Do we have an entry with this name already? */
    SymEntry* Entry = FindSymInTable (SymTab, Name, HashStr (Name));
    if (Entry) {

    	/* We have a symbol with this name already */
     	Error (ERR_MULTIPLE_DEFINITION, Name);

    } else {

	/* Create a new entry */
     	Entry = NewSymEntry (Name, Flags);

     	/* Set the symbol attributes */
     	Entry->Type   = TypeDup (Type);
	Entry->V.Offs = Offs;

     	/* Add the entry to the symbol table */
     	AddSymEntry (SymTab, Entry);

    }

    /* Return the entry */
    return Entry;
}



SymEntry* AddGlobalSym (const char* Name, type* Type, unsigned Flags)
/* Add an external or global symbol to the symbol table and return the entry */
{
    /* Functions must be inserted in the global symbol table */
    SymTable* Tab = IsTypeFunc (Type)? SymTab0 : SymTab;

    /* Do we have an entry with this name already? */
    SymEntry* Entry = FindSymInTable (Tab, Name, HashStr (Name));
    if (Entry) {

    	type* EType;

    	/* We have a symbol with this name already */
     	if (Entry->Flags & SC_TYPE) {
     	    Error (ERR_MULTIPLE_DEFINITION, Name);
     	    return Entry;
     	}

    	/* Get the type string of the existing symbol */
    	EType = Entry->Type;

    	/* If we are handling arrays, the old entry or the new entry may be an
    	 * incomplete declaration. Accept this, and if the exsting entry is
    	 * incomplete, complete it.
    	 */
    	if (IsTypeArray (Type) && IsTypeArray (EType)) {

    	    /* Get the array sizes */
    	    unsigned Size  = Decode (Type + 1);
    	    unsigned ESize = Decode (EType + 1);

    	    if ((Size != 0 && ESize != 0) ||
    	  	TypeCmp (Type+DECODE_SIZE+1, EType+DECODE_SIZE+1) < TC_EQUAL) {
    	  	/* Types not identical: Conflicting types */
    	  	Error (ERR_CONFLICTING_TYPES, Name);
    	    } else {
    	  	/* Check if we have a size in the existing definition */
    	  	if (ESize == 0) {
    	  	    /* Existing, size not given, use size from new def */
    	  	    Encode (EType + 1, Size);
    	  	}
    	    }

       	} else {
	    /* New type must be identical */
	    if (TypeCmp (EType, Type) < TC_EQUAL) {
     	     	Error (ERR_CONFLICTING_TYPES, Name);
	    }

	    /* In case of a function, use the new type descriptor, since it
	     * contains pointers to the new symbol tables that are needed if
	     * an actual function definition follows.
	     */
	    if (IsTypeFunc (Type)) {
		CopyEncode (Type+1, EType+1);
	    }
     	}

	/* Add the new flags */
     	Entry->Flags |= Flags;

    } else {

	/* Create a new entry */
     	Entry = NewSymEntry (Name, Flags);

     	/* Set the symbol attributes */
     	Entry->Type = TypeDup (Type);

     	/* Add the entry to the symbol table */
     	AddSymEntry (Tab, Entry);
    }

    /* Return the entry */
    return Entry;
}



/*****************************************************************************/
/*			      	     Code				     */
/*****************************************************************************/



SymTable* GetSymTab (void)
/* Return the current symbol table */
{
    return SymTab;
}



int SymIsLocal (SymEntry* Sym)
/* Return true if the symbol is defined in the highest lexical level */
{
    return (Sym->Owner == SymTab || Sym->Owner == TagTab);
}



void MakeZPSym (const char* Name)
/* Mark the given symbol as zero page symbol */
{
    /* Get the symbol table entry */
    SymEntry* Entry = FindSymInTable (SymTab, Name, HashStr (Name));

    /* Mark the symbol as zeropage */
    if (Entry) {
	Entry->Flags |= SC_ZEROPAGE;
    } else {
     	Error (ERR_UNDEFINED_SYMBOL, Name);
    }
}



void PrintSymTable (const SymTable* Tab, FILE* F, const char* Header, ...)
/* Write the symbol table to the given file */
{
    unsigned Len;
    const SymEntry* Entry;

    /* Print the header */
    va_list ap;
    va_start (ap, Header);
    fputc ('\n', F);
    Len = vfprintf (F, Header, ap);
    va_end (ap);
    fputc ('\n', F);

    /* Underline the header */
    while (Len--) {
    	fputc ('=', F);
    }
    fputc ('\n', F);

    /* Dump the table */
    Entry = Tab->SymHead;
    if (Entry == 0) {
	fprintf (F, "(empty)\n");
    } else {
	while (Entry) {
	    DumpSymEntry (F, Entry);
	    Entry = Entry->NextSym;
     	}
    }
    fprintf (F, "\n\n\n");
}



void EmitExternals (void)
/* Write import/export statements for external symbols */
{
    SymEntry* Entry;

    AddEmptyLine ();

    Entry = SymTab->SymHead;
    while (Entry) {
	unsigned Flags = Entry->Flags;
       	if (Flags & SC_EXTERN) {
     	    /* Only defined or referenced externs */
     	    if ((Flags & SC_REF) != 0 && (Flags & SC_DEF) == 0) {
     		/* An import */
     		g_defimport (Entry->Name, Flags & SC_ZEROPAGE);
     	    } else if (Flags & SC_DEF) {
     		/* An export */
     		g_defexport (Entry->Name, Flags & SC_ZEROPAGE);
     	    }
     	}
	Entry = Entry->NextSym;
    }
}



