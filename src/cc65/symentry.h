/*****************************************************************************/
/*                                                                           */
/*                                symentry.h                                 */
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



#ifndef SYMENTRY_H
#define SYMENTRY_H



#include <stdio.h>

/* common */
#include "coll.h"
#include "inline.h"

/* cc65 */
#include "datatype.h"
#include "declattr.h"



/*****************************************************************************/
/*                                  Forwards                                 */
/*****************************************************************************/



struct SegContext;
struct LiteralPool;
struct CodeEntry;



/*****************************************************************************/
/*                              struct SymEntry                              */
/*****************************************************************************/



/* Symbol types and flags */
#define SC_NONE         0x0000U         /* Nothing */

/* Types of symbols */
#define SC_STRUCT       0x0001U         /* Struct tag */
#define SC_UNION        0x0002U         /* Union tag */
#define SC_ENUM         0x0003U         /* Enum tag */
#define SC_LABEL        0x0004U         /* A goto code label */
#define SC_BITFIELD     0x0005U         /* A bit-field inside a struct or union */
#define SC_TYPEDEF      0x0006U         /* A typedef */
#define SC_ENUMERATOR   0x0007U         /* An enumerator */

/* Note: These symbol types might be checked as bit-flags occasionally.
**       So don't share their unique bits with other symbol types.
*/
#define SC_FUNC         0x0008U         /* A function */
#define SC_ARRAY        0x0010U         /* UNUSED: An array */
#define SC_TYPEMASK     0x001FU         /* Mask for symbol types all above */

/* Additional property of the symbols */
#define SC_CONST        0x0020U         /* A numeric constant with a type */
#define SC_STRUCTFIELD  0x0040U         /* A struct or union field */
#define SC_PARAM        0x0080U         /* A function parameter */
#define SC_DEFTYPE      0x0100U         /* An old-style parameter with default type (=int) */

/* Address property of the symbol */
#define SC_ZEROPAGE     0x0200U         /* Symbol marked as on zeropage */

/* Additional attributes of the symbol */
#define SC_HAVEALIGN    0x0400U         /* UNUSED: Symbol has special alignment */
#define SC_HAVEATTR     0x0800U         /* Symbol has attributes */

/* Special property of declaration */
#define SC_TU_STORAGE   0x1000U         /* Symbol has allocated storage in the TU */
#define SC_ASSIGN_INIT  0x2000U         /* Symbol is to be initialized with assignment code */

#define SC_ALIAS        0x4000U         /* Symbol is an alias */
#define SC_FICTITIOUS   0x8000U         /* Symbol is fictitious (for error recovery) */
#define SC_HAVEFAM      0x010000U       /* Struct/union has a Flexible Array Member */
#define SC_HAVECONST    0x020000U       /* Struct/union has a const member */

/* Status of the symbol */
#define SC_DEF          0x040000U       /* Symbol is defined */
#define SC_REF          0x080000U       /* Symbol is referenced */
#define SC_GOTO         0x100000U       /* Symbol is destination of a goto */
#define SC_GOTO_IND     0x200000U       /* Symbol is destination of an indirect goto */
#define SC_LOCALSCOPE   0x400000U       /* Symbol is invisible in file scope */
#define SC_NOINLINEDEF  0x800000U       /* Symbol may never have an inline definition */

/* To figure out the linkage of an object or function symbol Sym:
** - external linkage:
**    SymIsGlobal (Sym) && (Sym->Flags & SC_STORAGEMASK) != SC_STATIC
** - internal linkage:
**    SymIsGlobal (Sym) && (Sym->Flags & SC_STORAGEMASK) == SC_STATIC
** - no linkage:
**    !SymIsGlobal (Sym)
**
** To figure out the storage class of a symbol by its SC_ flags:
**
** - no explicit storage class specifiers (in file scope):
**    (flags & SC_STORAGEMASK) == SC_NONE
** - no explicit storage class specifiers (in block scope):
**    (flags & SC_STORAGEMASK) == SC_AUTO
** - extern:
**    (flags & SC_STORAGEMASK) == SC_EXTERN
** - static:
**    (flags & SC_STORAGEMASK) == SC_STATIC
** - auto:
**    (flags & SC_STORAGEMASK) == SC_AUTO
** - register:
**    (flags & SC_STORAGEMASK) == SC_REGISTER
** - typedef (per ISO C):
**    (flags & SC_TYPEMASK) == SC_TYPEDEF
**
** Note: SC_TYPEDEF can be also used as a flag.
*/
#define SC_AUTO         0x01000000U     /* Auto storage class */
#define SC_REGISTER     0x02000000U     /* Register storage class */
#define SC_STATIC       0x03000000U     /* Static storage class */
#define SC_EXTERN       0x04000000U     /* Extern storage class */
#define SC_THREAD       0x08000000U     /* UNSUPPORTED: Thread-local storage class */
#define SC_STORAGEMASK  0x0F000000U     /* Storage type mask */

/* Function specifiers */
#define SC_INLINE       0x10000000U     /* Inline function */
#define SC_NORETURN     0x20000000U     /* Noreturn function */



/* Label definition or reference */
typedef struct DefOrRef DefOrRef;
struct DefOrRef {
    unsigned            Line;
    size_t              LocalsBlockId;
    unsigned            Flags;
    int                 StackPtr;
    unsigned            Depth;
    unsigned            LateSP_Label;
};

/* Symbol table entry */
typedef struct SymEntry SymEntry;
struct SymEntry {
    SymEntry*                   NextHash; /* Next entry in hash list */
    SymEntry*                   PrevSym;  /* Previous symbol in dl list */
    SymEntry*                   NextSym;  /* Next symbol double linked list */
    struct SymTable*            Owner;    /* Symbol table the symbol is in */
    unsigned                    Flags;    /* Symbol flags */
    Type*                       Type;     /* Symbol type */
    Collection*                 Attr;     /* Attribute list if any */
    char*                       AsmName;  /* Assembler name if any */

    /* Data that differs for the different symbol types */
    union {

        /* Offset for locals */
        int                     Offs;

        /* Register bank offset and offset of the saved copy on stack for
        ** register variables.
        */
        struct {
            int                 RegOffs;
            int                 SaveOffs;
        } R;

        /* Segment name for tentantive global definitions */
        const char*             BssName;

        /* Value for integer constants (including enumerators) */
        long                    ConstVal;

        /* Data for functions */
        struct {
            struct SegContext*  Seg;      /* SegContext for this function */
            struct LiteralPool* LitPool;  /* Literal pool for this function */
            struct SymEntry*    WrappedCall;        /* Pointer to the WrappedCall */
            unsigned int        WrappedCallData;    /* The WrappedCall's user data */
        } F;

        /* Label name for static symbols */
        struct {
            unsigned            Label;
            Collection          *DefsOrRefs;
            struct CodeEntry    *IndJumpFrom;
        } L;

        /* Value of SP adjustment needed after forward 'goto' */
        unsigned short          SPAdjustment;

        /* Data for anonymous struct or union members */
        struct {
            int                 Offs;     /* Byte offset into struct */
            unsigned            ANumber;  /* Numeric ID */
            SymEntry*           Field;    /* The real field aliased */
        } A;

        /* Data for structs/unions tags */
        struct {
            struct SymTable*    SymTab;   /* Member symbol table */
            unsigned            Size;     /* Size of the union/struct */
            unsigned            ACount;   /* Count of anonymous fields */
        } S;

        /* Data for enums tags */
        struct {
            struct SymTable*    SymTab;   /* Member symbol table */
            const Type*         Type;     /* Underlying type */
        } E;

    } V;
    char                        Name[1]; /* Name, dynamically allocated */
};



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



SymEntry* NewSymEntry (const char* Name, unsigned Flags);
/* Create a new symbol table with the given name */

void FreeSymEntry (SymEntry* E);
/* Free a symbol entry */

void DumpSymEntry (FILE* F, const SymEntry* E);
/* Dump the given symbol table entry to the file in readable form */

int SymIsOutputFunc (const SymEntry* Sym);
/* Return true if this is a function that must be output */

#if defined(HAVE_INLINE)
INLINE int SymIsArray (const SymEntry* Sym)
/* Return true if the given entry is an array entry */
{
    return ((Sym->Flags & SC_TYPEMASK) == SC_ARRAY);
}
#else
#  define SymIsArray(Sym)       (((Sym)->Flags & SC_TYPEMASK) == SC_ARRAY)
#endif

#if defined(HAVE_INLINE)
INLINE int SymIsBitField (const SymEntry* Sym)
/* Return true if the given entry is a bit-field entry */
{
    return ((Sym->Flags & SC_TYPEMASK) == SC_BITFIELD);
}
#else
#  define SymIsBitField(Sym)    (((Sym)->Flags & SC_TYPEMASK) == SC_BITFIELD)
#endif

#if defined(HAVE_INLINE)
INLINE int SymIsLabel (const SymEntry* Sym)
/* Return true if the given entry is a label entry */
{
    return ((Sym)->Flags & SC_TYPEMASK) == SC_LABEL;
}
#else
#  define SymIsLabel(Sym)       (((Sym)->Flags & SC_TYPEMASK) == SC_LABEL)
#endif

#if defined(HAVE_INLINE)
INLINE int SymIsTypeDef (const SymEntry* Sym)
/* Return true if the given entry is a typedef entry */
{
    return ((Sym->Flags & SC_TYPEMASK) == SC_TYPEDEF);
}
#else
#  define SymIsTypeDef(Sym)     (((Sym)->Flags & SC_TYPEMASK) == SC_TYPEDEF)
#endif

#if defined(HAVE_INLINE)
INLINE int SymIsDef (const SymEntry* Sym)
/* Return true if the given entry is defined */
{
    return ((Sym->Flags & SC_DEF) == SC_DEF);
}
#else
#  define SymIsDef(Sym)     (((Sym)->Flags & SC_DEF) == SC_DEF)
#endif

#if defined(HAVE_INLINE)
INLINE int SymIsRef (const SymEntry* Sym)
/* Return true if the given entry is referenced */
{
    return ((Sym->Flags & SC_REF) == SC_REF);
}
#else
#  define SymIsRef(Sym)     (((Sym)->Flags & SC_REF) == SC_REF)
#endif

#if defined(HAVE_INLINE)
INLINE int SymIsRegVar (const SymEntry* Sym)
/* Return true if the given entry is a register variable */
{
    return ((Sym->Flags & (SC_STORAGEMASK | SC_TYPEMASK)) == (SC_REGISTER | SC_NONE));
}
#else
#  define SymIsRegVar(Sym)  (((Sym)->Flags & (SC_STORAGEMASK | SC_TYPEMASK)) == (SC_REGISTER | SC_NONE))
#endif

#if defined(HAVE_INLINE)
INLINE int SymHasFlexibleArrayMember (const SymEntry* Sym)
/* Return true if the given entry has a flexible array member */
{
    return ((Sym->Flags & SC_HAVEFAM) == SC_HAVEFAM);
}
#else
#  define SymHasFlexibleArrayMember(Sym)    (((Sym)->Flags & SC_HAVEFAM) == SC_HAVEFAM)
#endif

#if defined(HAVE_INLINE)
INLINE int SymHasConstMember (const SymEntry* Sym)
/* Return true if the given entry has a const member */
{
    return ((Sym->Flags & SC_HAVECONST) == SC_HAVECONST);
}
#else
#  define SymHasConstMember(Sym)    (((Sym)->Flags & SC_HAVECONST) == SC_HAVECONST)
#endif

#if defined(HAVE_INLINE)
INLINE const char* SymGetAsmName (const SymEntry* Sym)
/* Return the assembler label name for the symbol (beware: may be NULL!) */
{
    return Sym->AsmName;
}
#else
#  define SymGetAsmName(Sym)        ((Sym)->AsmName)
#endif

const DeclAttr* SymGetAttr (const SymEntry* Sym, DeclAttrType AttrType);
/* Return an attribute for this symbol or NULL if the attribute does not exist */

#if defined(HAVE_INLINE)
INLINE int SymHasAttr (const SymEntry* Sym, DeclAttrType A)
/* Return true if the symbol has the given attribute */
{
    return (SymGetAttr (Sym, A) != 0);
}
#else
#  define SymHasAttr(Sym, A)       (SymGetAttr (Sym, A) != 0)
#endif

void SymUseAttr (SymEntry* Sym, struct Declarator* D);
/* Use the attributes from the declarator for this symbol */

void SymSetAsmName (SymEntry* Sym);
/* Set the assembler name for an external symbol from the name of the symbol.
** The symbol must have no assembler name set yet.
*/

void SymCvtRegVarToAuto (SymEntry* Sym);
/* Convert a register variable to an auto variable */

void SymChangeType (SymEntry* Sym, const Type* T);
/* Change the type of the given symbol */

void SymChangeAsmName (SymEntry* Sym, const char* NewAsmName);
/* Change the assembler name of the symbol */

int SymHasAnonName (const SymEntry* Sym);
/* Return true if the symbol entry has an anonymous name */



/* End of symentry.h */

#endif
