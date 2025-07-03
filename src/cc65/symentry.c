/*****************************************************************************/
/*                                                                           */
/*                                symentry.c                                 */
/*                                                                           */
/*               Symbol table entries for the cc65 C compiler                */
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



#include <string.h>

/* common */
#include "xmalloc.h"

/* cc65 */
#include "anonname.h"
#include "asmlabel.h"
#include "declare.h"
#include "error.h"
#include "symentry.h"



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



SymEntry* NewSymEntry (const char* Name, unsigned Flags)
/* Create a new symbol table with the given name */
{
    /* Get the length of the name */
    unsigned Len = strlen (Name);

    /* Allocate memory for the symbol entry */
    SymEntry* E = xmalloc (sizeof (SymEntry) + Len);

    /* Initialize the entry */
    E->NextHash = 0;
    E->PrevSym  = 0;
    E->NextSym  = 0;
    E->Owner    = 0;
    E->Flags    = Flags;
    E->Type     = 0;
    E->Attr     = 0;
    E->AsmName  = 0;
    memset (&E->V, 0, sizeof (E->V));
    memcpy (E->Name, Name, Len+1);

    /* Return the new entry */
    return E;
}



void FreeSymEntry (SymEntry* E)
/* Free a symbol entry */
{
    unsigned i;

    TypeFree (E->Type);
    xfree (E->AsmName);

    if ((E->Flags & SC_TYPEMASK) == SC_LABEL) {
        for (i = 0; i < CollCount (E->V.L.DefsOrRefs); i++) {
            xfree (CollAt (E->V.L.DefsOrRefs, i));
        }

        DoneCollection (E->V.L.DefsOrRefs);
    }

    xfree (E);
}



void DumpSymEntry (FILE* F, const SymEntry* E)
/* Dump the given symbol table entry to the file in readable form */
{
    typedef const struct {
        const char*         Name;
        unsigned            Val;
    } SCFlagTable;

    static SCFlagTable Types[] = {
        { "SC_NONE",        SC_NONE             },
        { "SC_STRUCT",      SC_STRUCT           },
        { "SC_UNION",       SC_UNION            },
        { "SC_ENUM",        SC_ENUM             },
        { "SC_LABEL",       SC_LABEL            },
        { "SC_BITFIELD",    SC_BITFIELD         },
        { "SC_TYPEDEF",     SC_TYPEDEF          },
        { "SC_ENUMERATOR",  SC_ENUMERATOR       },
        { "SC_FUNC",        SC_FUNC             },
        { "SC_ARRAY",       SC_ARRAY            },
    };

    static SCFlagTable Storages[] = {
        { "SC_AUTO",        SC_AUTO             },
        { "SC_REGISTER",    SC_REGISTER         },
        { "SC_STATIC",      SC_STATIC           },
        { "SC_EXTERN",      SC_EXTERN           },
    };

    static SCFlagTable Properties[] = {
        { "SC_CONST",       SC_CONST            },
        { "SC_STRUCTFIELD", SC_STRUCTFIELD      },
        { "SC_PARAM",       SC_PARAM            },
        { "SC_DEFTYPE",     SC_DEFTYPE          },
        { "SC_ZEROPAGE",    SC_ZEROPAGE         },
        { "SC_HAVEALIGN",   SC_HAVEALIGN        },
        { "SC_HAVEATTR",    SC_HAVEATTR         },
        { "SC_TU_STORAGE",  SC_TU_STORAGE       },
        { "SC_ASSIGN_INIT", SC_ASSIGN_INIT      },
        { "SC_ALIAS",       SC_ALIAS            },
        { "SC_FICTITIOUS",  SC_FICTITIOUS       },
        { "SC_HAVEFAM",     SC_HAVEFAM          },
        { "SC_HAVECONST",   SC_HAVECONST        },
    };

    static SCFlagTable Status[] = {
        { "SC_DEF",         SC_DEF              },
        { "SC_REF",         SC_REF              },
        { "SC_GOTO",        SC_GOTO             },
        { "SC_GOTO_IND",    SC_GOTO_IND         },
        { "SC_LOCALSCOPE",  SC_LOCALSCOPE       },
        { "SC_NOINLINEDEF", SC_NOINLINEDEF      },
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
    fprintf (F, "    Flags:");
    /* Symbol types */
    if ((SymFlags & SC_TYPEMASK) != 0) {
        for (I = 0; I < sizeof (Types) / sizeof (Types[0]); ++I) {
            if ((SymFlags & SC_TYPEMASK) == Types[I].Val) {
                SymFlags &= ~SC_TYPEMASK;
                fprintf (F, " %s", Types[I].Name);
                break;
            }
        }
    }
    /* Storage classes */
    if ((SymFlags & SC_STORAGEMASK) != 0) {
        for (I = 0; I < sizeof (Storages) / sizeof (Storages[0]); ++I) {
            if ((SymFlags & SC_STORAGEMASK) == Storages[I].Val) {
                SymFlags &= ~SC_STORAGEMASK;
                fprintf (F, " %s", Storages[I].Name);
                break;
            }
        }
    }
    /* Special property flags */
    for (I = 0; I < sizeof (Properties) / sizeof (Properties[0]) && SymFlags != 0; ++I) {
        if ((SymFlags & Properties[I].Val) == Properties[I].Val) {
            SymFlags &= ~Properties[I].Val;
            fprintf (F, " %s", Properties[I].Name);
        }
    }
    /* Status flags */
    for (I = 0; I < sizeof (Status) / sizeof (Status[0]) && SymFlags != 0; ++I) {
        if ((SymFlags & Status[I].Val) == Status[I].Val) {
            SymFlags &= ~Status[I].Val;
            fprintf (F, " %s", Status[I].Name);
        }
    }
    if (SymFlags != 0) {
        fprintf (F, " 0x%05X", SymFlags);
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



int SymIsOutputFunc (const SymEntry* Sym)
/* Return true if this is a function that must be output */
{
    /* Symbol must be a function which is defined and either extern or
    ** static and referenced.
    */
    return IsTypeFunc (Sym->Type)                       &&
           SymIsDef (Sym)                               &&
           ((Sym->Flags & SC_REF) ||
            (Sym->Flags & SC_STORAGEMASK) != SC_STATIC);
}



const DeclAttr* SymGetAttr (const SymEntry* Sym, DeclAttrType AttrType)
/* Return an attribute for this symbol or NULL if the attribute does not exist */
{
    /* Beware: We may not even have a collection */
    if (Sym->Attr) {
        unsigned I;
        for (I = 0; I < CollCount (Sym->Attr); ++I) {

            /* Get the next attribute */
            const DeclAttr* A = CollConstAt (Sym->Attr, I);

            /* If this is the one we're searching for, return it */
            if (A->AttrType == AttrType) {
                return A;
            }
        }
    }

    /* Not found */
    return 0;
}



void SymUseAttr (SymEntry* Sym, struct Declarator* D)
/* Use the attributes from the declarator for this symbol */
{
    /* We cannot specify attributes twice */
    if ((Sym->Flags & SC_HAVEATTR) != 0) {
        if (D->Attributes != 0) {
            Error ("Attributes must be specified in the first declaration");
        }
        return;
    }

    /* Move the attributes */
    Sym->Attr = D->Attributes;
    D->Attributes = 0;
    Sym->Flags |= SC_HAVEATTR;
}



void SymSetAsmName (SymEntry* Sym)
/* Set the assembler name for an external symbol from the name of the symbol.
** The symbol must have no assembler name set yet.
*/
{
    unsigned Len;

    /* Cannot be used to change the name */
    PRECONDITION (Sym->AsmName == 0);

    /* The assembler name starts with an underline */
    Len = strlen (Sym->Name);
    Sym->AsmName = xmalloc (Len + 2);
    Sym->AsmName[0] = '_';
    memcpy (Sym->AsmName+1, Sym->Name, Len+1);
}



void SymCvtRegVarToAuto (SymEntry* Sym)
/* Convert a register variable to an auto variable */
{
    /* Change the storage class */
    Sym->Flags = (Sym->Flags & ~SC_STORAGEMASK) | SC_AUTO;

    /* Transfer the stack offset from register save area to actual offset */
    Sym->V.Offs = Sym->V.R.SaveOffs;
}



void SymChangeType (SymEntry* Sym, const Type* T)
/* Change the type of the given symbol */
{
    TypeFree (Sym->Type);
    Sym->Type = TypeDup (T);
}



void SymChangeAsmName (SymEntry* Sym, const char* NewAsmName)
/* Change the assembler name of the symbol */
{
    xfree (Sym->AsmName);
    Sym->AsmName = xstrdup (NewAsmName);
}



int SymHasAnonName (const SymEntry* Sym)
/* Return true if the symbol entry has an anonymous name */
{
    return IsAnonName (Sym->Name);
}
