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
#include <errno.h>

/* common */
#include "check.h"
#include "debugflag.h"
#include "hashfunc.h"
#include "xmalloc.h"

/* cc65 */
#include "anonname.h"
#include "asmcode.h"
#include "asmlabel.h"
#include "codegen.h"
#include "datatype.h"
#include "declare.h"
#include "error.h"
#include "funcdesc.h"
#include "function.h"
#include "global.h"
#include "input.h"
#include "stackptr.h"
#include "symentry.h"
#include "typecmp.h"
#include "typeconv.h"
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
static unsigned         LexLevelDepth   = 0;    /* For safety checks */
static LexicalLevel*    CurrentLex        = 0;
static SymTable*        SymTab0         = 0;
static SymTable*        SymTab          = 0;
static SymTable*        TagTab0         = 0;
static SymTable*        TagTab          = 0;
static SymTable*        FieldTab        = 0;
static SymTable*        LabelTab        = 0;
static SymTable*        SPAdjustTab     = 0;
static SymTable*        FailSafeTab     = 0;    /* For errors */

static FILE* DebugTableFile = 0;

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
                            Warning ("Parameter '%s' is never used", Entry->Name);
                        }
                    } else if (Flags & SC_FUNC) {
                        if (IS_Get (&WarnUnusedFunc)) {
                            Warning ("Function '%s' is defined but never used", Entry->Name);
                        }
                    } else {
                        if (IS_Get (&WarnUnusedVar)) {
                            Warning ("Variable '%s' is defined but never used", Entry->Name);
                        }
                    }
                }
            }

            /* If the entry is a label, check if it was defined in the function */
            if (Flags & SC_LABEL) {
                if (!SymIsDef (Entry)) {
                    /* Undefined label */
                    Error ("Undefined label: '%s'", Entry->Name);
                } else if (!SymIsRef (Entry)) {
                    /* Defined but not used */
                    if (IS_Get (&WarnUnusedLabel)) {
                        Warning ("Label '%s' is defined but never used", Entry->Name);
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



unsigned GetLexicalLevelDepth (void)
/* Return the current lexical level depth */
{
    return LexLevelDepth;
}



unsigned GetLexicalLevel (void)
/* Return the current lexical level */
{
    if (CurrentLex != 0) {
        return CurrentLex->CurrentLevel;
    }
    return LEX_LEVEL_NONE;
}



void PushLexicalLevel (unsigned NewLevel)
/* Enter the specified lexical level */
{
    LexicalLevel* L = xmalloc (sizeof (LexicalLevel));
    L->PrevLex = CurrentLex;
    CurrentLex = L;
    CurrentLex->CurrentLevel = NewLevel;
    ++LexLevelDepth;
}



void PopLexicalLevel (void)
/* Exit the current lexical level */
{
    LexicalLevel* L;
    PRECONDITION (CurrentLex != 0 && LexLevelDepth > 0);
    L = CurrentLex;
    CurrentLex = L->PrevLex;
    xfree (L);
    --LexLevelDepth;
}

void EnterGlobalLevel (void)
/* Enter the program global lexical level */
{
    const char* OutName = NULL;
    if (!SB_IsEmpty (&DebugTableName)) {
        OutName = SB_GetConstBuf (&DebugTableName);
    }

    if (OutName) {
        /* Open the table file */
        DebugTableFile = fopen (OutName, "w");
        if (DebugTableFile == 0) {
            Error ("Cannot create table dump file '%s': %s", OutName, strerror (errno));
        }
    }
    else if (Debug) {
        DebugTableFile = stdout;
    }

    /* Safety */
    PRECONDITION (GetLexicalLevel () == LEX_LEVEL_NONE);

    /* Enter global lexical level */
    PushLexicalLevel (LEX_LEVEL_GLOBAL);

    /* Create and assign the symbol table */
    SymTab0 = SymTab = NewSymTable (SYMTAB_SIZE_GLOBAL);

    /* Create and assign the tag table */
    TagTab0 = TagTab = NewSymTable (SYMTAB_SIZE_GLOBAL);

    /* Create and assign the table of SP adjustment symbols */
    SPAdjustTab = NewSymTable (SYMTAB_SIZE_GLOBAL);

    /* Create and assign the table of fictitious symbols used with errors */
    FailSafeTab = NewSymTable (SYMTAB_SIZE_GLOBAL);
}

void LeaveGlobalLevel (void)
/* Leave the program global lexical level */
{
    /* Safety */
    PRECONDITION (GetLexicalLevel () == LEX_LEVEL_GLOBAL);

    /* Check the tables */
    CheckSymTable (SymTab0);

    /* Dump the tables if requested */
    if (DebugTableFile) {
        SymEntry* Entry;
        StrBuf* Header;

        PrintSymTable (SymTab0, DebugTableFile, "Global symbol table");
        PrintSymTable (TagTab0, DebugTableFile, "Global tag table");

        Entry = TagTab0->SymHead;
        if (Entry) {
            fputs ("\nGlobal struct and union definitions", DebugTableFile);
            fputs ("\n=========================\n", DebugTableFile);

            do {
                if (!((Entry->Flags & SC_STRUCT) || (Entry->Flags & SC_UNION)) || !Entry->V.S.SymTab) {
                    continue;
                }

                Header = NewStrBuf();
                if(Entry->Flags & SC_STRUCT) {
                    SB_AppendStr (Header, "SC_STRUCT: ");
                }
                else {
                    SB_AppendStr (Header, "SC_UNION: ");
                }
                SB_AppendStr (Header, Entry->Name);
                SB_Terminate (Header);

                PrintSymTable (Entry->V.S.SymTab, DebugTableFile, SB_GetConstBuf (Header));
            } while ((Entry = Entry->NextSym));
        }

        /* Close the file */
        if (DebugTableFile != stdout && fclose (DebugTableFile) != 0) {
            Error ("Error closing table dump file '%s': %s", SB_GetConstBuf(&DebugTableName), strerror (errno));
        }
    }

    /* Don't delete the symbol and struct tables! */
    SymTab = 0;
    TagTab = 0;

    /* Exit global lexical level */
    PopLexicalLevel ();
}



void EnterFunctionLevel (void)
/* Enter function lexical level */
{
    SymTable* S;

    /* Enter function lexical level */
    PushLexicalLevel (LEX_LEVEL_FUNCTION);

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
    /* Safety */
    PRECONDITION (GetLexicalLevel () == LEX_LEVEL_FUNCTION);

    /* Leave function lexical level */
    PopLexicalLevel ();

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
    /* Enter function lexical level */
    PushLexicalLevel (LEX_LEVEL_FUNCTION);

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
    /* Safety */
    PRECONDITION (GetLexicalLevel () == LEX_LEVEL_FUNCTION);

    /* Leave function lexical level */
    PopLexicalLevel ();

    /* Check the tables */
    CheckSymTable (SymTab);
    CheckSymTable (LabelTab);

    /* Dump the tables if requested */
    if (DebugTableFile) {
        StrBuf* SymbolHeader = NewStrBuf();

        SB_AppendStr (SymbolHeader, "SC_FUNC: ");
        SB_AppendStr (SymbolHeader, CurrentFunc->FuncEntry->AsmName);
        SB_AppendStr (SymbolHeader, ": Symbol table");
        SB_Terminate (SymbolHeader);

        PrintSymTable (SymTab, DebugTableFile, SB_GetConstBuf(SymbolHeader));
    }

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

    /* Enter block lexical level */
    PushLexicalLevel (LEX_LEVEL_BLOCK);

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
    /* Safety */
    PRECONDITION (GetLexicalLevel () == LEX_LEVEL_BLOCK);

    /* Leave block lexical level */
    PopLexicalLevel ();

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

    /* Enter struct lexical level */
    PushLexicalLevel (LEX_LEVEL_STRUCT);

    /* Get a new symbol table and make it current. Note: Structs and enums
    ** nested in struct scope are NOT local to the struct but visible in the
    ** outside scope. So we will NOT create a new struct or enum table.
    */
    S = NewSymTable (SYMTAB_SIZE_STRUCT);
    S->PrevTab  = FieldTab;
    FieldTab    = S;
}



void LeaveStructLevel (void)
/* Leave a nested block for a struct definition */
{
    /* Safety */
    PRECONDITION (GetLexicalLevel () == LEX_LEVEL_STRUCT);

    /* Leave struct lexical level */
    PopLexicalLevel ();

    /* Don't delete the table */
    FieldTab = FieldTab->PrevTab;
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

        while (E != 0 && (E->Flags & SC_ALIAS) == SC_ALIAS) {
            /* Get the aliased entry */
            E = E->V.A.Field;
        }

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



SymEntry FindStructField (const Type* T, const char* Name)
/* Find a struct/union field in the fields list.
** Return the info about the found field symbol filled in an entry struct by
** value, or an empty entry struct if the field is not found.
*/
{
    SymEntry* Field = 0;
    SymEntry  Res;
    int       Offs  = 0;

    /* The given type may actually be a pointer to struct/union */
    if (IsTypePtr (T)) {
        ++T;
    }

    /* Only structs/unions have struct/union fields... */
    if (IsClassStruct (T)) {

        /* Get a pointer to the struct/union tag */
        const SymEntry* TagSym = GetESUTagSym (T);
        CHECK (TagSym != 0);

        /* Now search in the struct/union symbol table. Beware: The table may
        ** not exist.
        */
        if (TagSym->V.S.SymTab) {
            Field = FindSymInTable (TagSym->V.S.SymTab, Name, HashStr (Name));

            if (Field != 0) {
                Offs = Field->V.Offs;
            }

            while (Field != 0 && (Field->Flags & SC_ALIAS) == SC_ALIAS) {
                /* Get the real field */
                Field = Field->V.A.Field;
            }
        }
    }

    if (Field != 0) {
        Res = *Field;
        Res.V.Offs = Offs;
    } else {
        memset (&Res, 0, sizeof(SymEntry));
    }

    return Res;
}



/*****************************************************************************/
/*                       Add stuff to the symbol table                       */
/*****************************************************************************/



static int IsDistinctRedef (const Type* lhst, const Type* rhst, typecmpcode_t Code, typecmpflag_t Flags)
/* Return if type compatibility result is "worse" than Code or if any bit of
** qualifier Flags is set.
*/
{
    typecmp_t Result = TypeCmp (lhst, rhst);
    if (Result.C < Code || (Result.F & TCF_MASK_QUAL & Flags) != 0) {
        return 1;
    }
    return 0;
}


static int HandleSymRedefinition (SymEntry* Sym, const Type* T, unsigned Flags)
/* Check and handle redefinition of existing symbols.
** Complete array sizes and function descriptors as well.
** Return true if there *is* an error.
*/
{
    /* Get the type info of the existing symbol */
    Type*    E_Type   = Sym->Type;
    unsigned E_SCType = Sym->Flags & SC_TYPEMASK;
    unsigned SCType   = Flags & SC_TYPEMASK;

    /* Some symbols may be redeclared if certain requirements are met */
    if (E_SCType == SC_TYPEDEF) {

        /* Existing typedefs cannot be redeclared as anything different */
        if (SCType == SC_TYPEDEF) {
            if (IsDistinctRedef (E_Type, T, TC_IDENTICAL, TCF_MASK_QUAL)) {
                Error ("Conflicting types for typedef '%s'", Sym->Name);
                Note ("'%s' vs '%s'", GetFullTypeName (T), GetFullTypeName (E_Type));
                Sym = 0;
            }
        } else {
            Error ("Redefinition of typedef '%s' as different kind of symbol", Sym->Name);
            Sym = 0;
        }

    } else if ((Sym->Flags & SC_FUNC) == SC_FUNC) {

        /* In case of a function, use the new type descriptor, since it
        ** contains pointers to the new symbol tables that are needed if
        ** an actual function definition follows. Be sure not to use the
        ** new descriptor if it contains a function declaration with an
        ** empty parameter list.
        */
        if (IsTypeFunc (T)) {

            /* Check for duplicate function definitions */
            if (SymIsDef (Sym) && (Flags & SC_DEF) == SC_DEF) {
                Error ("Body for function '%s' has already been defined",
                        Sym->Name);
                Sym = 0;
            } else {
                /* New type must be compatible with the composite prototype */
                if (IsDistinctRedef (E_Type, T, TC_EQUAL, TCF_MASK_QUAL)) {
                    Error ("Conflicting function types for '%s'", Sym->Name);
                    Note ("'%s' vs '%s'", GetFullTypeName (T), GetFullTypeName (E_Type));
                    Sym = 0;
                } else {
                    /* Refine the existing composite prototype with this new
                    ** one.
                    */
                    RefineFuncDesc (Sym->Type, T);
                }
            }

        } else {
            Error ("Redefinition of function '%s' as different kind of symbol", Sym->Name);
            Sym = 0;
        }

    } else {

        /* Redeclarations of ESU types are checked elsewhere */
        if (IsTypeArray (T) && IsTypeArray (E_Type)) {

            /* Get the array sizes */
            long Size  = GetElementCount (T);
            long ESize = GetElementCount (E_Type);

            /* If we are handling arrays, the old entry or the new entry may be
            ** an incomplete declaration. Accept this, and if the exsting entry
            ** is incomplete, complete it.
            */
            if ((Size != UNSPECIFIED && ESize != UNSPECIFIED && Size != ESize) ||
                IsDistinctRedef (E_Type + 1, T + 1, TC_IDENTICAL, TCF_MASK_QUAL)) {
                /* Conflicting element types */
                Error ("Conflicting array types for '%s[]'", Sym->Name);
                Note ("'%s' vs '%s'", GetFullTypeName (T), GetFullTypeName (E_Type));
                Sym = 0;
            } else {
                /* Check if we have a size in the existing definition */
                if (ESize == UNSPECIFIED) {
                    /* Existing, size not given, use size from new def */
                    SetElementCount (E_Type, Size);
                }
            }

        } else {

            /* New type must be equivalent */
            if (SCType != E_SCType) {
                Error ("Redefinition of '%s' as different kind of symbol", Sym->Name);
                Sym = 0;
            } else if (IsDistinctRedef (E_Type, T, TC_EQUAL, TCF_MASK_QUAL)) {
                Error ("Conflicting types for '%s'", Sym->Name);
                Note ("'%s' vs '%s'", GetFullTypeName (T), GetFullTypeName (E_Type));
                Sym = 0;
            } else if (E_SCType == SC_ENUMERATOR) {
                /* Enumerators aren't allowed to be redeclared at all, even if
                ** all occurences are identical. The current code logic won't
                ** get here, but let's just do it.
                */
                Error ("Redeclaration of enumerator constant '%s'", Sym->Name);
                Sym = 0;
            } else if (Flags & SC_STRUCTFIELD) {
                Error ("Duplicate member '%s'", Sym->Name);
            }
        }
    }

    /* Return if there are any errors */
    return Sym == 0;
}



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



SymEntry* AddEnumSym (const char* Name, unsigned Flags, const Type* Type, SymTable* Tab, unsigned* DSFlags)
/* Add an enum tag entry and return it */
{
    SymTable* CurTagTab = TagTab;
    SymEntry* TagEntry;

    if ((Flags & SC_FICTITIOUS) == 0) {
        /* Do we have an entry with this name already? */
        TagEntry = FindSymInTable (CurTagTab, Name, HashStr (Name));
    } else {
        /* Add a fictitious symbol in the fail-safe table */
        TagEntry = 0;
        CurTagTab = FailSafeTab;
    }

    if (TagEntry) {

        /* We do have an entry. This may be a forward, so check it. */
        if ((TagEntry->Flags & SC_TYPEMASK) != SC_ENUM) {
            /* Existing symbol is not an enum */
            Error ("Symbol '%s' is already different kind", Name);
            TagEntry = 0;
        } else if (Type != 0) {
            /* Define the struct size if the underlying type is given. */
            if (TagEntry->V.E.Type != 0) {
                /* Both are definitions. */
                Error ("Multiple definition for 'enum %s'", Name);
                TagEntry = 0;
            } else {
                TagEntry->V.E.SymTab = Tab;
                TagEntry->V.E.Type   = Type;
                TagEntry->Flags     &= ~SC_DECL;
                TagEntry->Flags     |= SC_DEF;

                /* Remember this is the first definition of this type */
                if (DSFlags != 0) {
                    *DSFlags |= DS_NEW_TYPE_DEF;
                }
            }
        }

        if (TagEntry == 0) {
            /* Use the fail-safe table for fictitious symbols */
            CurTagTab = FailSafeTab;
        }
    }

    if (TagEntry == 0) {

        /* Create a new entry */
        TagEntry = NewSymEntry (Name, SC_ENUM);

        /* Set the enum type data */
        TagEntry->V.E.SymTab = Tab;
        TagEntry->V.E.Type   = Type;

        if (Type != 0) {
            TagEntry->Flags |= SC_DEF;
        }

        /* Remember this is the first definition of this type */
        if (CurTagTab != FailSafeTab && DSFlags != 0) {
            if ((TagEntry->Flags & SC_DEF) != 0) {
                *DSFlags |= DS_NEW_TYPE_DEF;
            }
            *DSFlags |= DS_NEW_TYPE_DECL;
        }

        /* Add it to the current table */
        AddSymEntry (CurTagTab, TagEntry);
    }

    /* Return the entry */
    return TagEntry;
}



SymEntry* AddStructSym (const char* Name, unsigned Flags, unsigned Size, SymTable* Tab, unsigned* DSFlags)
/* Add a struct/union tag entry and return it */
{
    SymTable* CurTagTab = TagTab;
    SymEntry* TagEntry;
    unsigned  SCType = (Flags & SC_TYPEMASK);

    /* SCType must be struct or union */
    PRECONDITION (SCType == SC_STRUCT || SCType == SC_UNION);

    if ((Flags & SC_FICTITIOUS) == 0) {
        /* Do we have an entry with this name already? */
        TagEntry = FindSymInTable (CurTagTab, Name, HashStr (Name));
    } else {
        /* Add a fictitious symbol in the fail-safe table */
        TagEntry = 0;
        CurTagTab = FailSafeTab;
    }

    if (TagEntry) {

        /* We do have an entry. This may be a forward, so check it. */
        if ((TagEntry->Flags & SC_TYPEMASK) != SCType) {
            /* Existing symbol is not a struct */
            Error ("Symbol '%s' is already different kind", Name);
            TagEntry = 0;
        } else if ((TagEntry->Flags & Flags & SC_DEF) == SC_DEF) {
            /* Both structs are definitions. */
            if (SCType == SC_STRUCT) {
                Error ("Multiple definition for 'struct %s'", Name);
            } else {
                Error ("Multiple definition for 'union %s'", Name);
            }
            TagEntry = 0;
        } else {
            /* Define the struct size if it is a definition */
            if ((Flags & SC_DEF) == SC_DEF) {
                TagEntry->Flags      = Flags;
                TagEntry->V.S.SymTab = Tab;
                TagEntry->V.S.Size   = Size;

                /* Remember this is the first definition of this type */
                if (DSFlags != 0) {
                    *DSFlags |= DS_NEW_TYPE_DEF;
                }
            }
        }

        if (TagEntry == 0) {
            /* Use the fail-safe table for fictitious symbols */
            CurTagTab = FailSafeTab;
        }
    }

    if (TagEntry == 0) {

        /* Create a new entry */
        TagEntry = NewSymEntry (Name, Flags);

        /* Set the struct data */
        TagEntry->V.S.SymTab = Tab;
        TagEntry->V.S.Size   = Size;

        /* Remember this is the first definition of this type */
        if (CurTagTab != FailSafeTab && DSFlags != 0) {
            if ((TagEntry->Flags & SC_DEF) != 0) {
                *DSFlags |= DS_NEW_TYPE_DEF;
            }
            *DSFlags |= DS_NEW_TYPE_DECL;
        }

        /* Add it to the current tag table */
        AddSymEntry (CurTagTab, TagEntry);
    }

    /* Return the entry */
    return TagEntry;
}



SymEntry* AddBitField (const char* Name, const Type* T, unsigned Offs,
                       unsigned BitOffs, unsigned BitWidth, int SignednessSpecified)
/* Add a bit field to the local symbol table and return the symbol entry */
{
    /* Do we have an entry with this name already? */
    SymEntry* Entry = FindSymInTable (FieldTab, Name, HashStr (Name));
    if (Entry) {

        /* We have a symbol with this name already */
        Error ("Multiple definition for bit-field '%s'", Name);

    } else {

        /* Create a new entry */
        Entry = NewSymEntry (Name, SC_BITFIELD);

        /* Set the symbol attributes. Bit-fields are always integral types. */
        Entry->Type   = NewBitFieldOf (T, BitOffs, BitWidth);
        Entry->V.Offs = Offs;

        if (!SignednessSpecified) {
            /* int is treated as signed int everywhere except bit-fields; switch it to unsigned,
            ** since this is allowed for bit-fields and avoids sign-extension, so is much faster.
            ** enums set SignednessSpecified to 1 to avoid this adjustment.  Character types
            ** actually distinguish 3 types of char; char may either be signed or unsigned, which
            ** is controlled by `--signed-chars`.  In bit-fields, however, we perform the same
            ** `char -> unsigned char` adjustment that is performed with other integral types.
            */
            CHECK ((Entry->Type->C & T_MASK_SIGN) == T_SIGN_SIGNED ||
                   IsRankChar (Entry->Type));
            Entry->Type[0].C &= ~T_MASK_SIGN;
            Entry->Type[0].C |= T_SIGN_UNSIGNED;
            Entry->Type[1].C &= ~T_MASK_SIGN;
            Entry->Type[1].C |= T_SIGN_UNSIGNED;
        }

        /* Add the entry to the symbol table */
        AddSymEntry (FieldTab, Entry);

    }

    /* Return the entry */
    return Entry;
}



SymEntry* AddConstSym (const char* Name, const Type* T, unsigned Flags, long Val)
/* Add an constant symbol to the symbol table and return it */
{
    /* Do we have an entry with this name already? */
    SymEntry* Entry = FindSymInTable (SymTab, Name, HashStr (Name));
    if (Entry) {
        if ((Entry->Flags & SC_CONST) != SC_CONST) {
            Error ("Symbol '%s' is already different kind", Name);
        } else {
            Error ("Multiple definition for constant '%s'", Name);
        }
        return Entry;
    }

    /* Create a new entry */
    Entry = NewSymEntry (Name, Flags);

    /* We only have integer constants for now */
    Entry->Type = TypeDup (T);

    /* Set the constant data */
    Entry->V.ConstVal = Val;

    /* Add the entry to the symbol table */
    AddSymEntry (SymTab, Entry);

    /* Return the entry */
    return Entry;
}



DefOrRef* AddDefOrRef (SymEntry* E, unsigned Flags)
/* Add definition or reference to the SymEntry and preserve its attributes */
{
    DefOrRef *DOR;

    DOR = xmalloc (sizeof (DefOrRef));
    CollAppend (E->V.L.DefsOrRefs, DOR);
    DOR->Line = GetCurrentLineNum ();
    DOR->LocalsBlockId = (size_t)CollLast (&CurrentFunc->LocalsBlockStack);
    DOR->Flags = Flags;
    DOR->StackPtr = StackPtr;
    DOR->Depth = CollCount (&CurrentFunc->LocalsBlockStack);
    DOR->LateSP_Label = GetLocalDataLabel ();

    return DOR;
}



unsigned short FindSPAdjustment (const char* Name)
/* Search for an entry in the table of SP adjustments */
{
    SymEntry* Entry = FindSymInTable (SPAdjustTab, Name, HashStr (Name));

    if (!Entry) {
        Internal ("No SP adjustment label entry found");
    }

    return Entry->V.SPAdjustment;
}



SymEntry* AddLabelSym (const char* Name, unsigned Flags)
/* Add a C goto label to the label table */
{
    unsigned i;
    DefOrRef *DOR, *NewDOR;
    /* We juggle it so much that a shortcut will help with clarity */
    Collection *AIC = &CurrentFunc->LocalsBlockStack;

    /* Do we have an entry with this name already? */
    SymEntry* Entry = FindSymInTable (LabelTab, Name, HashStr (Name));
    if (Entry) {

        if (SymIsDef (Entry) && (Flags & SC_DEF) != 0) {
            /* Trying to define the label more than once */
            Error ("Label '%s' is defined more than once", Name);
        }

        NewDOR = AddDefOrRef (Entry, Flags);

        /* Walk through all occurrences of the label so far and evaluate
        ** their relationship with the one passed to the function.
        */
        for (i = 0; i < CollCount (Entry->V.L.DefsOrRefs); i++) {
            DOR = CollAt (Entry->V.L.DefsOrRefs, i);

            if ((DOR->Flags & SC_DEF) && (Flags & SC_REF) && (Flags & (SC_GOTO | SC_GOTO_IND))) {
                /* We're processing a goto and here is its destination label.
                ** This means the difference between SP values is already known,
                ** so we simply emit the SP adjustment code.
                */
                if (StackPtr != DOR->StackPtr) {
                    g_space (StackPtr - DOR->StackPtr);
                }

                /* Are we jumping into a block with initalization of an object that
                ** has automatic storage duration? Let's emit a warning.
                */
                if ((size_t)CollLast (AIC) != DOR->LocalsBlockId &&
                    (CollCount (AIC) < DOR->Depth ||
                    (size_t)CollAt (AIC, DOR->Depth - 1) != DOR->LocalsBlockId)) {
                    Warning ("Goto at line %d to label %s jumps into a block with "
                    "initialization of an object that has automatic storage duration",
                    GetCurrentLineNum (), Name);
                }
            }


            if ((DOR->Flags & SC_REF) && (DOR->Flags & (SC_GOTO | SC_GOTO_IND)) && (Flags & SC_DEF)) {
                /* We're processing a label, let's update all gotos encountered
                ** so far
                */
                if (DOR->Flags & SC_GOTO) {
                    SymEntry *E;
                    g_userodata ();
                    g_defdatalabel (DOR->LateSP_Label);
                    g_defdata (CF_CONST | CF_INT, StackPtr - DOR->StackPtr, 0);

                    /* Optimizer will need the information about the value of SP adjustment
                    ** later, so let's preserve it.
                    */
                    E = NewSymEntry (LocalDataLabelName (DOR->LateSP_Label), SC_SPADJUSTMENT);
                    E->V.SPAdjustment = StackPtr - DOR->StackPtr;
                    AddSymEntry (SPAdjustTab, E);
                }

                /* Are we jumping into a block with initalization of an object that
                ** has automatic storage duration? Let's emit a warning.
                */
                if ((size_t)CollLast (AIC) != DOR->LocalsBlockId &&
                    (CollCount (AIC) >= DOR->Depth ||
                    (size_t)CollLast (AIC) >= (size_t)DOR->Line))
                    Warning ("Goto at line %d to label %s jumps into a block with "
                    "initialization of an object that has automatic storage duration",
                    DOR->Line, Name);
             }

        }

        Entry->Flags |= Flags;

    } else {

        /* Create a new entry */
        Entry = NewSymEntry (Name, SC_LABEL | Flags);

        /* Set a new label number */
        Entry->V.L.Label = GetLocalLabel ();
        Entry->V.L.IndJumpFrom = NULL;

        /* Create Collection for label definition and references */
        Entry->V.L.DefsOrRefs = NewCollection ();
        NewDOR = AddDefOrRef (Entry, Flags);

        /* Generate the assembler name of the label */
        Entry->AsmName = xstrdup (LocalLabelName (Entry->V.L.Label));

        /* Add the entry to the label table */
        AddSymEntry (LabelTab, Entry);

    }

    /* We are processing a goto, but the label has not yet been defined */
    if (!SymIsDef (Entry) && (Flags & SC_REF) && (Flags & SC_GOTO)) {
        g_lateadjustSP (NewDOR->LateSP_Label);
    }

    /* Return the entry */
    return Entry;
}



SymEntry* AddLocalSym (const char* Name, const Type* T, unsigned Flags, int Offs)
/* Add a local or struct/union field symbol and return the symbol entry */
{
    SymTable* Tab = (Flags & SC_STRUCTFIELD) == 0 ? SymTab : FieldTab;
    ident Ident;

    /* Do we have an entry with this name already? */
    SymEntry* Entry = FindSymInTable (Tab, Name, HashStr (Name));
    if (Entry) {

        /* We have a symbol with this name already */
        if (HandleSymRedefinition (Entry, T, Flags)) {
            Entry = 0;
        } else if ((Flags & SC_ESUTYPEMASK) != SC_TYPEDEF) {
            /* Redefinitions are not allowed */
            if (SymIsDef (Entry) && (Flags & SC_DEF) == SC_DEF) {
                Error ("Multiple definition of '%s'", Entry->Name);
                Entry = 0;
            } else if ((Flags & (SC_AUTO | SC_REGISTER)) != 0 &&
                       (Entry->Flags & SC_EXTERN) != 0) {
                /* Check for local storage class conflict */
                Error ("Declaration of '%s' with no linkage follows extern declaration",
                       Name);
                Entry = 0;
            } else {
                /* If a static declaration follows a non-static declaration,
                ** then it is an error.
                */
                if ((Flags & SC_DEF)            &&
                    (Flags & SC_EXTERN) == 0    &&
                    (Entry->Flags & SC_EXTERN) != 0) {
                    Error ("Static declaration of '%s' follows extern declaration", Name);
                    Entry = 0;
                }
            }
        }

        if (Entry == 0) {
            if ((Flags & SC_PARAM) != 0) {
                /* Use anonymous names */
                Name = AnonName (Ident, "param");
            } else {
                /* Use the fail-safe table for fictitious symbols */
                Tab = FailSafeTab;
            }
        }
    }

    if (Entry == 0) {
        /* Create a new entry */
        Entry = NewSymEntry (Name, Flags);

        /* Set the symbol attributes */
        Entry->Type = TypeDup (T);

        if ((Flags & SC_STRUCTFIELD) == SC_STRUCTFIELD ||
            (Flags & SC_ESUTYPEMASK) == SC_TYPEDEF) {
            if ((Flags & SC_ALIAS) != SC_ALIAS) {
                Entry->V.Offs = Offs;
            }
        } else if ((Flags & SC_AUTO) == SC_AUTO) {
            Entry->V.Offs = Offs;
        } else if ((Flags & SC_REGISTER) == SC_REGISTER) {
            Entry->V.R.RegOffs  = Offs;
            Entry->V.R.SaveOffs = StackPtr;
        } else if ((Flags & SC_EXTERN) == SC_EXTERN ||
                   (Flags & SC_FUNC) == SC_FUNC) {
            Entry->V.L.Label = Offs;
            SymSetAsmName (Entry);
        } else if ((Flags & SC_STATIC) == SC_STATIC) {
            /* Generate the assembler name from the data label number */
            Entry->V.L.Label = Offs;
            Entry->AsmName = xstrdup (LocalDataLabelName (Entry->V.L.Label));
        } else if ((Flags & SC_ALIAS) == SC_ALIAS) {
            /* Just clear the info */
            Entry->V.A.Field = 0;
            Entry->V.A.ANumber = 0;
            Entry->V.A.Offs = 0;
        } else {
            Internal ("Invalid flags in AddLocalSym: %04X", Flags);
        }

        /* Add the entry to the symbol table */
        AddSymEntry (Tab, Entry);
    }

    /* Return the entry */
    return Entry;
}



SymEntry* AddGlobalSym (const char* Name, const Type* T, unsigned Flags)
/* Add an external or global symbol to the symbol table and return the entry */
{
    /* Add the new declaration to the global symbol table if no errors */
    SymTable* Tab = SymTab0;

    /* Only search this name in the local and global symbol tables */
    SymEntry* Entry = 0;
    SymEntry* Alias = 0;

    if (SymTab != SymTab0) {
        Alias = Entry = FindLocalSym (Name);
        while (Entry && (Entry->Flags & SC_ALIAS) == SC_ALIAS) {
            /* Get the aliased entry */
            Entry = Entry->V.A.Field;
        }
    }

    if (Entry == 0) {
        Entry = FindGlobalSym (Name);
    }

    if (Entry) {
        /* We have a symbol with this name already */
        if (HandleSymRedefinition (Entry, T, Flags)) {
            Entry = 0;
        } else if ((Entry->Flags & (SC_AUTO | SC_REGISTER)) != 0) {
            /* Check for local storage class conflict */
            Error ("Extern declaration of '%s' follows declaration with no linkage",
                   Name);
            Entry = 0;
        } else if ((Flags & SC_ESUTYPEMASK) != SC_TYPEDEF) {
            /* If a static declaration follows a non-static declaration, then the result is undefined.
            ** Most compilers choose to either give an error at compile time,
            ** or remove the extern property for a link time error if used.
            */
            if (SymTab == SymTab0           &&
                (Flags & SC_EXTERN) == 0    &&
                (Entry->Flags & SC_EXTERN) != 0) {
                Error ("Static declaration of '%s' follows non-static declaration", Name);
            } else if ((Flags & SC_EXTERN) != 0                                     &&
                       (Entry->Owner == SymTab0 || (Entry->Flags & SC_DEF) != 0)    &&
                       (Entry->Flags & SC_EXTERN) == 0) {
                /* It is OK if a global extern declaration follows a global
                ** non-static declaration, but an error if either of them is
                ** local, as the two would be referring to different objects.
                ** It is an error as well if a global non-static declaration
                ** follows a global static declaration.
                */
                if (Entry->Owner == SymTab0) {
                    if ((Flags & SC_STORAGE) == 0) {
                        /* Linkage must be unchanged.
                        ** The C standard specifies that a later extern declaration will be ignored,
                        ** and will use the previous linkage instead. Giving a warning for this case.
                        */
                        Flags &= ~SC_EXTERN;
                        Warning ("Extern declaration of '%s' follows static declaration, extern ignored", Name);
                    } else {
                        Error ("Non-static declaration of '%s' follows static declaration", Name);
                    }
                } else {
                    Error ("Extern declaration of '%s' follows static declaration", Name);
                    Entry = 0;
                }
            }

            if (Entry) {
                /* Add the new flags */
                Entry->Flags |= Flags;
            }
        }

        if (Entry == 0) {
            /* Use the fail-safe table for fictitious symbols */
            Tab = FailSafeTab;
        }
    }

    if (Entry == 0) {

        /* Create a new entry */
        Entry = NewSymEntry (Name, Flags);

        /* Set the symbol attributes */
        Entry->Type = TypeDup (T);

        /* If this is a function, clear additional fields */
        if (IsTypeFunc (T)) {
            Entry->V.F.Seg = 0;
        }

        /* Add the assembler name of the symbol */
        SymSetAsmName (Entry);

        /* Add the entry to the symbol table */
        AddSymEntry (Tab, Entry);

    }

    /* Add an alias of the global symbol to the local symbol table */
    if (Tab == SymTab0 && SymTab != SymTab0 && Entry->Owner != SymTab && Alias == 0) {
        Alias = AddLocalSym (Name, T, SC_ALIAS, 0);
        Alias->V.A.Field = Entry;
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



SymTable* GetFieldSymTab (void)
/* Return the current field symbol table */
{
    return FieldTab;
}



SymTable* GetLabelSymTab (void)
/* Return the global symbol table */
{
    return LabelTab;
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
        Error ("Undefined symbol: '%s'", Name);
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
        if (GetLexicalLevel () == LEX_LEVEL_GLOBAL) {
            Head = "\t.dbg\t\tsym";
        } else {
            Head = "\t.dbg\tsym";
        }
        Sym = SymTab->SymHead;
        while (Sym) {
            if ((Sym->Flags & (SC_CONST | SC_TYPEMASK)) == 0) {
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
