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



struct Segments;
struct LiteralPool;
struct CodeEntry;



/*****************************************************************************/
/*                              struct SymEntry                              */
/*****************************************************************************/



/* Storage classes and flags */
#define SC_NONE         0x0000U         /* Nothing */
#define SC_STRUCT       0x0001U         /* Struct */
#define SC_UNION        0x0002U         /* Union */
#define SC_ENUM         0x0003U         /* Enum */
#define SC_TYPEDEF      0x0004U         /* Typedef */
#define SC_ESUTYPEMASK  0x0007U         /* Mask for above types */
#define SC_ENUMERATOR   0x0008U         /* An enumerator */
#define SC_BITFIELD     0x0010U         /* A bit-field inside a struct or union */
#define SC_TYPEMASK     0x001FU         /* Mask for above types */

#define SC_FUNC         0x0020U         /* A function */
#define SC_LABEL        0x0040U         /* A goto code label */
#define SC_CONST        0x0080U         /* A numeric constant with a type */
#define SC_PARAM        0x0100U         /* A function parameter */
#define SC_DEFTYPE      0x0200U         /* Parameter has default type (=int, old style) */
#define SC_STRUCTFIELD  0x0400U         /* Struct or union field */

#define SC_ZEROPAGE     0x0800U         /* Symbol marked as zeropage */

#define SC_DEF          0x1000U         /* Symbol is defined */
#define SC_REF          0x2000U         /* Symbol is referenced */
#define SC_DECL         0x4000U         /* Symbol is declared in global scope */
#define SC_STORAGE      0x8000U         /* Symbol with associated storage */

#define SC_AUTO         0x010000U       /* Auto variable */
#define SC_REGISTER     0x020000U       /* Register variable */
#define SC_STATIC       0x040000U       /* Static - not to be confused with other *_STATIC */
#define SC_EXTERN       0x080000U       /* Extern linkage */
#define SC_STORAGEMASK  0x0F0000U       /* Storage type mask */

#define SC_HAVEATTR     0x100000U       /* Symbol has attributes */

#define SC_GOTO         0x200000U
#define SC_SPADJUSTMENT 0x400000U
#define SC_GOTO_IND     0x800000U       /* Indirect goto */

#define SC_ALIAS        0x01000000U     /* Alias of anonymous field */
#define SC_FICTITIOUS   0x02000000U     /* Symbol is fictitious */
#define SC_HAVEFAM      0x04000000U     /* Type has a Flexible Array Member */



/* Label definition or reference */
typedef struct DefOrRef DefOrRef;
struct DefOrRef {
    unsigned            Line;
    long                LocalsBlockId;
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
    SymEntry*                   Link;     /* General purpose single linked list */
    struct SymTable*            Owner;    /* Symbol table the symbol is in */
    unsigned                    Flags;    /* Symbol flags */
    Type*                       Type;     /* Symbol type */
    Collection*                 Attr;     /* Attribute list if any */
    char*                       AsmName;  /* Assembler name if any */

    /* Data that differs for the different symbol types */
    union {

        /* Offset for locals or struct members */
        int                     Offs;

        /* Data for anonymous struct or union members */
        struct {
            int                 Offs;     /* Byte offset into struct */
            unsigned            ANumber;  /* Numeric ID */
            SymEntry*           Field;    /* The real field aliased */
        } A;


        /* Label name for static symbols */
        struct {
            unsigned            Label;
            Collection          *DefsOrRefs;
            struct CodeEntry    *IndJumpFrom;
        } L;

        /* Value of SP adjustment needed after forward 'goto' */
        unsigned short      SPAdjustment;

        /* Register bank offset and offset of the saved copy on stack for
        ** register variables.
        */
        struct {
            int                 RegOffs;
            int                 SaveOffs;
        } R;

        /* Value for constants (including enums) */
        long                    ConstVal;

        /* Data for structs/unions */
        struct {
            struct SymTable*    SymTab;   /* Member symbol table */
            unsigned            Size;     /* Size of the union/struct */
            unsigned            ACount;   /* Count of anonymous fields */
        } S;

        /* Data for enums */
        struct {
            struct SymTable*    SymTab;   /* Member symbol table */
            const Type*         Type;     /* Underlying type */
        } E;

        /* Data for bit fields */
        struct {
            unsigned            Offs;     /* Byte offset into struct */
            unsigned            BitOffs;  /* Bit offset into storage unit */
            unsigned            BitWidth; /* Width in bits */
        } B;

        /* Data for functions */
        struct {
            struct Segments*    Seg;      /* Segments for this function */
            struct LiteralPool* LitPool;  /* Literal pool for this function */
        } F;

        /* Segment name for tentantive global definitions */
        const char*             BssName;
    } V;
    char                       Name[1]; /* Name, dynamically allocated */
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

#if defined(HAVE_INLINE)
INLINE int SymIsBitField (const SymEntry* Sym)
/* Return true if the given entry is a bit-field entry */
{
    return ((Sym->Flags & SC_BITFIELD) == SC_BITFIELD);
}
#else
#  define SymIsBitField(Sym)    (((Sym)->Flags & SC_BITFIELD) == SC_BITFIELD)
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
/* ### HACK! Fix the ugly type flags! */
{
    return ((Sym->Flags & (SC_REGISTER | SC_TYPEMASK)) == SC_REGISTER);
}
#else
#  define SymIsRegVar(Sym)  (((Sym)->Flags & (SC_REGISTER | SC_TYPEMASK)) == SC_REGISTER)
#endif

int SymIsOutputFunc (const SymEntry* Sym);
/* Return true if this is a function that must be output */

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
INLINE const char* SymGetAsmName (const SymEntry* Sym)
/* Return the assembler label name for the symbol (beware: may be NULL!) */
{
    return Sym->AsmName;
}
#else
#  define SymGetAsmName(Sym)      ((Sym)->AsmName)
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

void SymUseAttr (SymEntry* Sym, struct Declaration* D);
/* Use the attributes from the declaration for this symbol */

void SymSetAsmName (SymEntry* Sym);
/* Set the assembler name for an external symbol from the name of the symbol */

void CvtRegVarToAuto (SymEntry* Sym);
/* Convert a register variable to an auto variable */

SymEntry* GetSymType (const Type* T);
/* Get the symbol entry of the enum/struct/union type
** Return 0 if it is not an enum/struct/union.
*/

const char* GetSymTypeName (const Type* T);
/* Return a name string of the type or the symbol name if it is an ESU type.
** Note: This may use a static buffer that could be overwritten by other calls.
*/

void ChangeSymType (SymEntry* Entry, Type* T);
/* Change the type of the given symbol */

void ChangeAsmName (SymEntry* Entry, const char* NewAsmName);
/* Change the assembler name of the symbol */

int HasAnonName (const SymEntry* Entry);
/* Return true if the symbol entry has an anonymous name */



/* End of symentry.h */

#endif
