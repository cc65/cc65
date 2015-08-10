/*****************************************************************************/
/*                                                                           */
/*                                 symtab.c                                  */
/*                                                                           */
/*              Symbol table management for the cc65 C compiler              */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000-2013, Ullrich von Bassewitz                                      */
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



#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

/* common */
#include "check.h"
#include "debugflag.h"
#include "hashfunc.h"
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
#include "stackptr.h"
#include "symentry.h"
#include "typecmp.h"
#include "symtab.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* An empty symbol table */
SymTable        EmptySymTab = {
    0,          /* PrevTab */
    0,          /* SymHead */
    0,          /* SymTail */
    0,          /* SymCount */
    1,          /* Size */
    { 0 }       /* Tab[1] */
};

/* Symbol table sizes */
#define SYMTAB_SIZE_GLOBAL      211U
#define SYMTAB_SIZE_FUNCTION     29U
#define SYMTAB_SIZE_BLOCK        13U
#define SYMTAB_SIZE_STRUCT       19U
#define SYMTAB_SIZE_LABEL         7U

/* The current and root symbol tables */
static unsigned         LexicalLevel    = 0;    /* For safety checks */
static SymTable*        SymTab0         = 0;
static SymTable*        SymTab          = 0;
static SymTable*        TagTab0         = 0;
static SymTable*        TagTab          = 0;
static SymTable*        LabelTab        = 0;



/*****************************************************************************/
/*                              struct SymTable                              */
/*****************************************************************************/



static SymTable* NewSymTable (unsigned Size)
/* Create and return a symbol table for the given lexical level */
{
    unsigned I;

    /* Allocate memory for the table */
    SymTable* S = xmalloc (sizeof (SymTable) + (Size-1) * sizeof (SymEntry*));

    /* Initialize the symbol table structure */
    S->PrevTab  = 0;
    S->SymHead  = 0;
    S->SymTail  = 0;
    S->SymCount = 0;
    S->Size     = Size;
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
/*                         Check symbols in a table                          */
/*****************************************************************************/



static void CheckSymTable (SymTable* Tab)
/* Check a symbol table for open references, unused symbols ... */
{
    SymEntry* Entry = Tab->SymHead;
    while (Entry) {

        /* Get the storage flags for tne entry */
        unsigned Flags = Entry->Flags;

        /* Ignore typedef entries */
        if (!SymIsTypeDef (Entry)) {

            /* Check if the symbol is one with storage, and it if it was
            ** defined but not used.
            */
            if (((Flags & SC_AUTO) || (Flags & SC_STATIC)) && (Flags & SC_EXTERN) == 0) {
                if (SymIsDef (Entry) && !SymIsRef (Entry) &&
                    !SymHasAttr (Entry, atUnused)) {
                    if (Flags & SC_PARAM) {
                        if (IS_Get (&WarnUnusedParam)) {
                            Warning ("Parameter `%s' is never used", Entry->Name);
                        }
                    } else {
                        if (IS_Get (&WarnUnusedVar)) {
                            Warning ("`%s' is defined but never used", Entry->Name);
                        }
                    }
                }
            }

            /* If the entry is a label, check if it was defined in the function */
            if (Flags & SC_LABEL) {
                if (!SymIsDef (Entry)) {
                    /* Undefined label */
                    Error ("Undefined label: `%s'", Entry->Name);
                } else if (!SymIsRef (Entry)) {
                    /* Defined but not used */
                    if (IS_Get (&WarnUnusedLabel)) {
                        Warning ("`%s' is defined but never used", Entry->Name);
                    }
                }
            }

        }

        /* Next entry */
        Entry = Entry->NextSym;
    }
}



/*****************************************************************************/
/*                        Handling of lexical levels                         */
/*****************************************************************************/



unsigned GetLexicalLevel (void)
/* Return the current lexical level */
{
    return LexicalLevel;
}



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
    SymTab = 0;
    TagTab = 0;
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
    S = NewSymTable (SYMTAB_SIZE_LABEL);
    S->PrevTab = LabelTab;
    LabelTab = S;
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
    LabelTab = LabelTab->PrevTab;
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
    S->PrevTab  = SymTab;
    SymTab      = S;

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
    ** nested in struct scope are NOT local to the struct but visible in the
    ** outside scope. So we will NOT create a new struct or enum table.
    */
    S = NewSymTable (SYMTAB_SIZE_BLOCK);
    S->PrevTab  = SymTab;
    SymTab      = S;
}



void LeaveStructLevel (void)
/* Leave a nested block for a struct definition */
{
    /* Don't delete the table */
    SymTab = SymTab->PrevTab;
}



/*****************************************************************************/
/*                              Find functions                               */
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



SymEntry* FindGlobalSym (const char* Name)
/* Find the symbol with the given name in the global symbol table only */
{
    return FindSymInTable (SymTab0, Name, HashStr (Name));
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



SymEntry* FindStructField (const Type* T, const char* Name)
/* Find a struct field in the fields list */
{
    SymEntry* Field = 0;

    /* The given type may actually be a pointer to struct */
    if (IsTypePtr (T)) {
        ++T;
    }

    /* Non-structs do not have any struct fields... */
    if (IsClassStruct (T)) {

        /* Get a pointer to the struct/union type */
        const SymEntry* Struct = GetSymEntry (T);
        CHECK (Struct != 0);

        /* Now search in the struct symbol table. Beware: The table may not
        ** exist.
        */
        if (Struct->V.S.SymTab) {
            Field = FindSymInTable (Struct->V.S.SymTab, Name, HashStr (Name));
        }
    }

    return Field;
}



/*****************************************************************************/
/*                       Add stuff to the symbol table                       */
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
    ++T->SymCount;

    /* Insert the symbol into the hash chain */
    S->NextHash  = T->Tab[Hash];
    T->Tab[Hash] = S;

    /* Tell the symbol in which table it is */
    S->Owner = T;
}



SymEntry* AddStructSym (const char* Name, unsigned Type, unsigned Size, SymTable* Tab)
/* Add a struct/union entry and return it */
{
    SymEntry* Entry;

    /* Type must be struct or union */
    PRECONDITION (Type == SC_STRUCT || Type == SC_UNION);

    /* Do we have an entry with this name already? */
    Entry = FindSymInTable (TagTab, Name, HashStr (Name));
    if (Entry) {

        /* We do have an entry. This may be a forward, so check it. */
        if ((Entry->Flags & SC_TYPEMASK) != Type) {
            /* Existing symbol is not a struct */
            Error ("Symbol `%s' is already different kind", Name);
        } else if (Size > 0 && Entry->V.S.Size > 0) {
            /* Both structs are definitions. */
            Error ("Multiple definition for `%s'", Name);
        } else {
            /* Define the struct size if it is given */
            if (Size > 0) {
                Entry->V.S.SymTab = Tab;
                Entry->V.S.Size   = Size;
            }
        }

    } else {

        /* Create a new entry */
        Entry = NewSymEntry (Name, Type);

        /* Set the struct data */
        Entry->V.S.SymTab = Tab;
        Entry->V.S.Size   = Size;

        /* Add it to the current table */
        AddSymEntry (TagTab, Entry);
    }

    /* Return the entry */
    return Entry;
}



SymEntry* AddBitField (const char* Name, unsigned Offs, unsigned BitOffs, unsigned Width)
/* Add a bit field to the local symbol table and return the symbol entry */
{
    /* Do we have an entry with this name already? */
    SymEntry* Entry = FindSymInTable (SymTab, Name, HashStr (Name));
    if (Entry) {

        /* We have a symbol with this name already */
        Error ("Multiple definition for `%s'", Name);

    } else {

        /* Create a new entry */
        Entry = NewSymEntry (Name, SC_BITFIELD);

        /* Set the symbol attributes. Bit-fields are always of type unsigned */
        Entry->Type         = type_uint;
        Entry->V.B.Offs     = Offs;
        Entry->V.B.BitOffs  = BitOffs;
        Entry->V.B.BitWidth = Width;

        /* Add the entry to the symbol table */
        AddSymEntry (SymTab, Entry);

    }

    /* Return the entry */
    return Entry;
}



SymEntry* AddConstSym (const char* Name, const Type* T, unsigned Flags, long Val)
/* Add an constant symbol to the symbol table and return it */
{
    /* Enums must be inserted in the global symbol table */
    SymTable* Tab = ((Flags & SC_ENUM) == SC_ENUM)? SymTab0 : SymTab;

    /* Do we have an entry with this name already? */
    SymEntry* Entry = FindSymInTable (Tab, Name, HashStr (Name));
    if (Entry) {
        if ((Entry->Flags & SC_CONST) != SC_CONST) {
            Error ("Symbol `%s' is already different kind", Name);
        } else {
            Error ("Multiple definition for `%s'", Name);
        }
        return Entry;
    }

    /* Create a new entry */
    Entry = NewSymEntry (Name, Flags);

    /* Enum values are ints */
    Entry->Type = TypeDup (T);

    /* Set the enum data */
    Entry->V.ConstVal = Val;

    /* Add the entry to the symbol table */
    AddSymEntry (Tab, Entry);

    /* Return the entry */
    return Entry;
}



SymEntry* AddLabelSym (const char* Name, unsigned Flags)
/* Add a goto label to the label table */
{
    /* Do we have an entry with this name already? */
    SymEntry* Entry = FindSymInTable (LabelTab, Name, HashStr (Name));
    if (Entry) {

        if (SymIsDef (Entry) && (Flags & SC_DEF) != 0) {
            /* Trying to define the label more than once */
            Error ("Label `%s' is defined more than once", Name);
        }
        Entry->Flags |= Flags;

    } else {

        /* Create a new entry */
        Entry = NewSymEntry (Name, SC_LABEL | Flags);

        /* Set a new label number */
        Entry->V.Label = GetLocalLabel ();

        /* Generate the assembler name of the label */
        Entry->AsmName = xstrdup (LocalLabelName (Entry->V.Label));

        /* Add the entry to the label table */
        AddSymEntry (LabelTab, Entry);

    }

    /* Return the entry */
    return Entry;
}



SymEntry* AddLocalSym (const char* Name, const Type* T, unsigned Flags, int Offs)
/* Add a local symbol and return the symbol entry */
{
    /* Do we have an entry with this name already? */
    SymEntry* Entry = FindSymInTable (SymTab, Name, HashStr (Name));
    if (Entry) {

        /* We have a symbol with this name already */
        Error ("Multiple definition for `%s'", Name);

    } else {

        /* Create a new entry */
        Entry = NewSymEntry (Name, Flags);

        /* Set the symbol attributes */
        Entry->Type = TypeDup (T);
        if ((Flags & SC_AUTO) == SC_AUTO) {
            Entry->V.Offs = Offs;
        } else if ((Flags & SC_REGISTER) == SC_REGISTER) {
            Entry->V.R.RegOffs  = Offs;
            Entry->V.R.SaveOffs = StackPtr;
        } else if ((Flags & SC_EXTERN) == SC_EXTERN) {
            Entry->V.Label = Offs;
            SymSetAsmName (Entry);
        } else if ((Flags & SC_STATIC) == SC_STATIC) {
            /* Generate the assembler name from the label number */
            Entry->V.Label = Offs;
            Entry->AsmName = xstrdup (LocalLabelName (Entry->V.Label));
        } else if ((Flags & SC_STRUCTFIELD) == SC_STRUCTFIELD) {
            Entry->V.Offs = Offs;
        } else {
            Internal ("Invalid flags in AddLocalSym: %04X", Flags);
        }

        /* Add the entry to the symbol table */
        AddSymEntry (SymTab, Entry);

    }

    /* Return the entry */
    return Entry;
}



SymEntry* AddGlobalSym (const char* Name, const Type* T, unsigned Flags)
/* Add an external or global symbol to the symbol table and return the entry */
{
    /* There is some special handling for functions, so check if it is one */
    int IsFunc = IsTypeFunc (T);

    /* Functions must be inserted in the global symbol table */
    SymTable* Tab = IsFunc? SymTab0 : SymTab;

    /* Do we have an entry with this name already? */
    SymEntry* Entry = FindSymInTable (Tab, Name, HashStr (Name));
    if (Entry) {

        Type* EType;

        /* We have a symbol with this name already */
        if (Entry->Flags & SC_TYPE) {
            Error ("Multiple definition for `%s'", Name);
            return Entry;
        }

        /* Get the type string of the existing symbol */
        EType = Entry->Type;

        /* If we are handling arrays, the old entry or the new entry may be an
        ** incomplete declaration. Accept this, and if the exsting entry is
        ** incomplete, complete it.
        */
        if (IsTypeArray (T) && IsTypeArray (EType)) {

            /* Get the array sizes */
            long Size  = GetElementCount (T);
            long ESize = GetElementCount (EType);

            if ((Size != UNSPECIFIED && ESize != UNSPECIFIED && Size != ESize) ||
                TypeCmp (T + 1, EType + 1) < TC_EQUAL) {
                /* Types not identical: Conflicting types */
                Error ("Conflicting types for `%s'", Name);
                return Entry;
            } else {
                /* Check if we have a size in the existing definition */
                if (ESize == UNSPECIFIED) {
                    /* Existing, size not given, use size from new def */
                    SetElementCount (EType, Size);
                }
            }

        } else {
            /* New type must be identical */
            if (TypeCmp (EType, T) < TC_EQUAL) {
                Error ("Conflicting types for `%s'", Name);
                return Entry;
            }

            /* In case of a function, use the new type descriptor, since it
            ** contains pointers to the new symbol tables that are needed if
            ** an actual function definition follows. Be sure not to use the
            ** new descriptor if it contains a function declaration with an
            ** empty parameter list.
            */
            if (IsFunc) {
                /* Get the function descriptor from the new type */
                FuncDesc* F = GetFuncDesc (T);
                /* Use this new function descriptor if it doesn't contain
                ** an empty parameter list.
                */
                if ((F->Flags & FD_EMPTY) == 0) {
                    Entry->V.F.Func = F;
                    SetFuncDesc (EType, F);
                }
            }
        }

        /* If a static declaration follows a non-static declaration, then
        ** warn about the conflict.  (It will compile a public declaration.)
        */
        if ((Flags & SC_EXTERN) == 0 && (Entry->Flags & SC_EXTERN) != 0) {
            Warning ("static declaration follows non-static declaration of `%s'.", Name);
        }

        /* An extern declaration must not change the current linkage. */
        if (IsFunc || (Flags & (SC_EXTERN | SC_DEF)) == SC_EXTERN) {
            Flags &= ~SC_EXTERN;
        }

        /* If a public declaration follows a static declaration, then
        ** warn about the conflict.  (It will compile a public declaration.)
        */
        if ((Flags & SC_EXTERN) != 0 && (Entry->Flags & SC_EXTERN) == 0) {
            Warning ("public declaration follows static declaration of `%s'.", Name);
        }

        /* Add the new flags */
        Entry->Flags |= Flags;

    } else {

        /* Create a new entry */
        Entry = NewSymEntry (Name, Flags);

        /* Set the symbol attributes */
        Entry->Type = TypeDup (T);

        /* If this is a function, set the function descriptor and clear
        ** additional fields.
        */
        if (IsFunc) {
            Entry->V.F.Func = GetFuncDesc (Entry->Type);
            Entry->V.F.Seg  = 0;
        }

        /* Add the assembler name of the symbol */
        SymSetAsmName (Entry);

        /* Add the entry to the symbol table */
        AddSymEntry (Tab, Entry);
    }

    /* Return the entry */
    return Entry;
}



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



SymTable* GetSymTab (void)
/* Return the current symbol table */
{
    return SymTab;
}



SymTable* GetGlobalSymTab (void)
/* Return the global symbol table */
{
    return SymTab0;
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
        Error ("Undefined symbol: `%s'", Name);
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

    Entry = SymTab->SymHead;
    while (Entry) {
        unsigned Flags = Entry->Flags;
        if (Flags & SC_EXTERN) {
            /* Only defined or referenced externs */
            if (SymIsRef (Entry) && !SymIsDef (Entry)) {
                /* An import */
                g_defimport (Entry->Name, Flags & SC_ZEROPAGE);
            } else if (SymIsDef (Entry)) {
                /* An export */
                g_defexport (Entry->Name, Flags & SC_ZEROPAGE);
            }
        }
        Entry = Entry->NextSym;
    }
}



void EmitDebugInfo (void)
/* Emit debug infos for the locals of the current scope */
{
    const char* Head;
    const SymEntry* Sym;

    /* Output info for locals if enabled */
    if (DebugInfo) {
        /* For cosmetic reasons in the output file, we will insert two tabs
        ** on global level and just one on local level.
        */
        if (LexicalLevel == LEX_LEVEL_GLOBAL) {
            Head = "\t.dbg\t\tsym";
        } else {
            Head = "\t.dbg\tsym";
        }
        Sym = SymTab->SymHead;
        while (Sym) {
            if ((Sym->Flags & (SC_CONST|SC_TYPE)) == 0) {
                if (Sym->Flags & SC_AUTO) {
                    AddTextLine ("%s, \"%s\", \"00\", auto, %d",
                                 Head, Sym->Name, Sym->V.Offs);
                } else if (Sym->Flags & SC_REGISTER) {
                    AddTextLine ("%s, \"%s\", \"00\", register, \"regbank\", %d",
                                 Head, Sym->Name, Sym->V.R.RegOffs);

                } else if (SymIsRef (Sym) && !SymIsDef (Sym)) {
                    AddTextLine ("%s, \"%s\", \"00\", %s, \"%s\"",
                                 Head, Sym->Name,
                                 (Sym->Flags & SC_EXTERN)? "extern" : "static",
                                 Sym->AsmName);
                }
            }
            Sym = Sym->NextSym;
        }
    }
}
