/*****************************************************************************/
/*                                                                           */
/*                                 declare.c                                 */
/*                                                                           */
/*                 Parse variable and function declarations                  */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2015, Ullrich von Bassewitz                                      */
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
#include <string.h>
#include <errno.h>

/* common */
#include "addrsize.h"
#include "mmodel.h"
#include "xmalloc.h"

/* cc65 */
#include "anonname.h"
#include "codegen.h"
#include "datatype.h"
#include "declare.h"
#include "declattr.h"
#include "error.h"
#include "expr.h"
#include "funcdesc.h"
#include "function.h"
#include "global.h"
#include "litpool.h"
#include "pragma.h"
#include "scanner.h"
#include "standard.h"
#include "symtab.h"
#include "typeconv.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



typedef struct StructInitData StructInitData;
struct StructInitData {
    unsigned    Size;                   /* Size of struct */
    unsigned    Offs;                   /* Current offset in struct */
    unsigned    BitVal;                 /* Summed up bit-field value */
    unsigned    ValBits;                /* Valid bits in Val */
};



/*****************************************************************************/
/*                                 Forwards                                  */
/*****************************************************************************/



static void ParseTypeSpec (DeclSpec* D, long Default, TypeCode Qualifiers);
/* Parse a type specifier */

static unsigned ParseInitInternal (Type* T, int AllowFlexibleMembers);
/* Parse initialization of variables. Return the number of data bytes. */



/*****************************************************************************/
/*                            Internal functions                             */
/*****************************************************************************/



static void DuplicateQualifier (const char* Name)
/* Print an error message */
{
    Warning ("Duplicate qualifier: `%s'", Name);
}



static TypeCode OptionalQualifiers (TypeCode Allowed)
/* Read type qualifiers if we have any. Allowed specifies the allowed
** qualifiers.
*/
{
    /* We start without any qualifiers */
    TypeCode Q = T_QUAL_NONE;

    /* Check for more qualifiers */
    while (1) {

        switch (CurTok.Tok) {

            case TOK_CONST:
                if (Allowed & T_QUAL_CONST) {
                    if (Q & T_QUAL_CONST) {
                        DuplicateQualifier ("const");
                    }
                    Q |= T_QUAL_CONST;
                } else {
                    goto Done;
                }
                break;

            case TOK_VOLATILE:
                if (Allowed & T_QUAL_VOLATILE) {
                    if (Q & T_QUAL_VOLATILE) {
                        DuplicateQualifier ("volatile");
                    }
                    Q |= T_QUAL_VOLATILE;
                } else {
                    goto Done;
                }
                break;

            case TOK_RESTRICT:
                if (Allowed & T_QUAL_RESTRICT) {
                    if (Q & T_QUAL_RESTRICT) {
                        DuplicateQualifier ("restrict");
                    }
                    Q |= T_QUAL_RESTRICT;
                } else {
                    goto Done;
                }
                break;

            case TOK_NEAR:
                if (Allowed & T_QUAL_NEAR) {
                    if (Q & T_QUAL_NEAR) {
                        DuplicateQualifier ("near");
                    }
                    Q |= T_QUAL_NEAR;
                } else {
                    goto Done;
                }
                break;

            case TOK_FAR:
                if (Allowed & T_QUAL_FAR) {
                    if (Q & T_QUAL_FAR) {
                        DuplicateQualifier ("far");
                    }
                    Q |= T_QUAL_FAR;
                } else {
                    goto Done;
                }
                break;

            case TOK_FASTCALL:
                if (Allowed & T_QUAL_FASTCALL) {
                    if (Q & T_QUAL_FASTCALL) {
                        DuplicateQualifier ("fastcall");
                    }
                    Q |= T_QUAL_FASTCALL;
                } else {
                    goto Done;
                }
                break;

            case TOK_CDECL:
                if (Allowed & T_QUAL_CDECL) {
                    if (Q & T_QUAL_CDECL) {
                        DuplicateQualifier ("cdecl");
                    }
                    Q |= T_QUAL_CDECL;
                } else {
                    goto Done;
                }
                break;

            default:
                goto Done;

        }

        /* Skip the token */
        NextToken ();
    }

Done:
    /* We cannot have more than one address size far qualifier */
    switch (Q & T_QUAL_ADDRSIZE) {

        case T_QUAL_NONE:
        case T_QUAL_NEAR:
        case T_QUAL_FAR:
            break;

        default:
            Error ("Cannot specify more than one address size qualifier");
            Q &= ~T_QUAL_ADDRSIZE;
    }

    /* We cannot have more than one calling convention specifier */
    switch (Q & T_QUAL_CCONV) {

        case T_QUAL_NONE:
        case T_QUAL_FASTCALL:
        case T_QUAL_CDECL:
            break;

        default:
            Error ("Cannot specify more than one calling convention qualifier");
            Q &= ~T_QUAL_CCONV;
    }

    /* Return the qualifiers read */
    return Q;
}



static void OptionalInt (void)
/* Eat an optional "int" token */
{
    if (CurTok.Tok == TOK_INT) {
        /* Skip it */
        NextToken ();
    }
}



static void OptionalSigned (void)
/* Eat an optional "signed" token */
{
    if (CurTok.Tok == TOK_SIGNED) {
        /* Skip it */
        NextToken ();
    }
}



static void InitDeclSpec (DeclSpec* D)
/* Initialize the DeclSpec struct for use */
{
    D->StorageClass     = 0;
    D->Type[0].C        = T_END;
    D->Flags            = 0;
}



static void InitDeclaration (Declaration* D)
/* Initialize the Declaration struct for use */
{
    D->Ident[0]   = '\0';
    D->Type[0].C  = T_END;
    D->Index      = 0;
    D->Attributes = 0;
}



static void NeedTypeSpace (Declaration* D, unsigned Count)
/* Check if there is enough space for Count type specifiers within D */
{
    if (D->Index + Count >= MAXTYPELEN) {
        /* We must call Fatal() here, since calling Error() will try to
        ** continue, and the declaration type is not correctly terminated
        ** in case we come here.
        */
        Fatal ("Too many type specifiers");
    }
}



static void AddTypeToDeclaration (Declaration* D, TypeCode T)
/* Add a type specifier to the type of a declaration */
{
    NeedTypeSpace (D, 1);
    D->Type[D->Index++].C = T;
}



static void FixQualifiers (Type* DataType)
/* Apply several fixes to qualifiers */
{
    Type*    T;
    TypeCode Q;

    /* Using typedefs, it is possible to generate declarations that have
    ** type qualifiers attached to an array, not the element type. Go and
    ** fix these here.
    */
    T = DataType;
    Q = T_QUAL_NONE;
    while (T->C != T_END) {
        if (IsTypeArray (T)) {
            /* Extract any type qualifiers */
            Q |= GetQualifier (T);
            T->C = UnqualifiedType (T->C);
        } else {
            /* Add extracted type qualifiers here */
            T->C |= Q;
            Q = T_QUAL_NONE;
        }
        ++T;
    }
    /* Q must be empty now */
    CHECK (Q == T_QUAL_NONE);

    /* Do some fixes on pointers and functions. */
    T = DataType;
    while (T->C != T_END) {
        if (IsTypePtr (T)) {
            /* Calling convention qualifier on the pointer? */
            if (IsQualCConv (T)) {
                /* Pull the convention off of the pointer */
                Q = T[0].C & T_QUAL_CCONV;
                T[0].C &= ~T_QUAL_CCONV;

                /* Pointer to a function which doesn't have an explicit convention? */
                if (IsTypeFunc (T + 1)) {
                    if (IsQualCConv (T + 1)) {
                        if ((T[1].C & T_QUAL_CCONV) == Q) {
                            Warning ("Pointer duplicates function's calling convention");
                        } else {
                            Error ("Function's and pointer's calling conventions are different");
                        }
                    } else {
                        if (Q == T_QUAL_FASTCALL && IsVariadicFunc (T + 1)) {
                            Error ("Variadic-function pointers cannot be __fastcall__");
                        } else {
                            /* Move the qualifier from the pointer to the function. */
                            T[1].C |= Q;
                        }
                    }
                } else {
                    Error ("Not pointer to a function; can't use a calling convention");
                }
            }

            /* Apply the default far and near qualifiers if none are given */
            Q = (T[0].C & T_QUAL_ADDRSIZE);
            if (Q == T_QUAL_NONE) {
                /* No address size qualifiers specified */
                if (IsTypeFunc (T+1)) {
                    /* Pointer to function. Use the qualifier from the function,
                    ** or the default if the function doesn't have one.
                    */
                    Q = (T[1].C & T_QUAL_ADDRSIZE);
                    if (Q == T_QUAL_NONE) {
                        Q = CodeAddrSizeQualifier ();
                    }
                } else {
                    Q = DataAddrSizeQualifier ();
                }
                T[0].C |= Q;
            } else {
                /* We have address size qualifiers. If followed by a function,
                ** apply them to the function also.
                */
                if (IsTypeFunc (T+1)) {
                    TypeCode FQ = (T[1].C & T_QUAL_ADDRSIZE);
                    if (FQ == T_QUAL_NONE) {
                        T[1].C |= Q;
                    } else if (FQ != Q) {
                        Error ("Address size qualifier mismatch");
                        T[1].C = (T[1].C & ~T_QUAL_ADDRSIZE) | Q;
                    }
                }
            }

        } else if (IsTypeFunc (T)) {

            /* Apply the default far and near qualifiers if none are given */
            if ((T[0].C & T_QUAL_ADDRSIZE) == 0) {
                T[0].C |= CodeAddrSizeQualifier ();
            }

        }
        ++T;
    }
}



static void ParseStorageClass (DeclSpec* D, unsigned DefStorage)
/* Parse a storage class */
{
    /* Assume we're using an explicit storage class */
    D->Flags &= ~DS_DEF_STORAGE;

    /* Check the storage class given */
    switch (CurTok.Tok) {

        case TOK_EXTERN:
            D->StorageClass = SC_EXTERN | SC_STATIC;
            NextToken ();
            break;

        case TOK_STATIC:
            D->StorageClass = SC_STATIC;
            NextToken ();
            break;

        case TOK_REGISTER:
            D->StorageClass = SC_REGISTER | SC_STATIC;
            NextToken ();
            break;

        case TOK_AUTO:
            D->StorageClass = SC_AUTO;
            NextToken ();
            break;

        case TOK_TYPEDEF:
            D->StorageClass = SC_TYPEDEF;
            NextToken ();
            break;

        default:
            /* No storage class given, use default */
            D->Flags |= DS_DEF_STORAGE;
            D->StorageClass = DefStorage;
            break;
    }
}



static void ParseEnumDecl (void)
/* Process an enum declaration . */
{
    int EnumVal;
    ident Ident;

    /* Accept forward definitions */
    if (CurTok.Tok != TOK_LCURLY) {
        return;
    }

    /* Skip the opening curly brace */
    NextToken ();

    /* Read the enum tags */
    EnumVal = 0;
    while (CurTok.Tok != TOK_RCURLY) {

        /* We expect an identifier */
        if (CurTok.Tok != TOK_IDENT) {
            Error ("Identifier expected");
            continue;
        }

        /* Remember the identifier and skip it */
        strcpy (Ident, CurTok.Ident);
        NextToken ();

        /* Check for an assigned value */
        if (CurTok.Tok == TOK_ASSIGN) {
            ExprDesc Expr;
            NextToken ();
            ConstAbsIntExpr (hie1, &Expr);
            EnumVal = Expr.IVal;
        }

        /* Add an entry to the symbol table */
        AddConstSym (Ident, type_int, SC_ENUM, EnumVal++);

        /* Check for end of definition */
        if (CurTok.Tok != TOK_COMMA)
            break;
        NextToken ();
    }
    ConsumeRCurly ();
}



static int ParseFieldWidth (Declaration* Decl)
/* Parse an optional field width. Returns -1 if no field width is specified,
** otherwise the width of the field.
*/
{
    ExprDesc Expr;

    if (CurTok.Tok != TOK_COLON) {
        /* No bit-field declaration */
        return -1;
    }

    /* Read the width */
    NextToken ();
    ConstAbsIntExpr (hie1, &Expr);
    if (Expr.IVal < 0) {
        Error ("Negative width in bit-field");
        return -1;
    }
    if (Expr.IVal > (int) INT_BITS) {
        Error ("Width of bit-field exceeds its type");
        return -1;
    }
    if (Expr.IVal == 0 && Decl->Ident[0] != '\0') {
        Error ("Zero width for named bit-field");
        return -1;
    }
    if (!IsTypeInt (Decl->Type)) {
        /* Only integer types may be used for bit-fields */
        Error ("Bit-field has invalid type");
        return -1;
    }

    /* Return the field width */
    return (int) Expr.IVal;
}



static SymEntry* StructOrUnionForwardDecl (const char* Name, unsigned Type)
/* Handle a struct or union forward decl */
{
    /* Try to find a struct/union with the given name. If there is none,
    ** insert a forward declaration into the current lexical level.
    */
    SymEntry* Entry = FindTagSym (Name);
    if (Entry == 0) {
        Entry = AddStructSym (Name, Type, 0, 0);
    } else if ((Entry->Flags & SC_TYPEMASK) != Type) {
        /* Already defined, but no struct */
        Error ("Symbol `%s' is already different kind", Name);
    }
    return Entry;
}



static unsigned CopyAnonStructFields (const Declaration* Decl, int Offs)
/* Copy fields from an anon union/struct into the current lexical level. The
** function returns the size of the embedded struct/union.
*/
{
    /* Get the pointer to the symbol table entry of the anon struct */
    SymEntry* Entry = GetSymEntry (Decl->Type);

    /* Get the size of the anon struct */
    unsigned Size = Entry->V.S.Size;

    /* Get the symbol table containing the fields. If it is empty, there has
    ** been an error before, so bail out.
    */
    SymTable* Tab = Entry->V.S.SymTab;
    if (Tab == 0) {
        /* Incomplete definition - has been flagged before */
        return Size;
    }

    /* Get a pointer to the list of symbols. Then walk the list adding copies
    ** of the embedded struct to the current level.
    */
    Entry = Tab->SymHead;
    while (Entry) {

        /* Enter a copy of this symbol adjusting the offset. We will just
        ** reuse the type string here.
        */
        AddLocalSym (Entry->Name, Entry->Type, SC_STRUCTFIELD, Offs + Entry->V.Offs);

        /* Currently, there can not be any attributes, but if there will be
        ** some in the future, we want to know this.
        */
        CHECK (Entry->Attr == 0);

        /* Next entry */
        Entry = Entry->NextSym;
    }

    /* Return the size of the embedded struct */
    return Size;
}



static SymEntry* ParseUnionDecl (const char* Name)
/* Parse a union declaration. */
{

    unsigned  UnionSize;
    unsigned  FieldSize;
    int       FieldWidth;       /* Width in bits, -1 if not a bit-field */
    SymTable* FieldTab;


    if (CurTok.Tok != TOK_LCURLY) {
        /* Just a forward declaration. */
        return StructOrUnionForwardDecl (Name, SC_UNION);
    }

    /* Add a forward declaration for the struct in the current lexical level */
    AddStructSym (Name, SC_UNION, 0, 0);

    /* Skip the curly brace */
    NextToken ();

    /* Enter a new lexical level for the struct */
    EnterStructLevel ();

    /* Parse union fields */
    UnionSize      = 0;
    while (CurTok.Tok != TOK_RCURLY) {

        /* Get the type of the entry */
        DeclSpec Spec;
        InitDeclSpec (&Spec);
        ParseTypeSpec (&Spec, -1, T_QUAL_NONE);

        /* Read fields with this type */
        while (1) {

            Declaration Decl;

            /* Get type and name of the struct field */
            ParseDecl (&Spec, &Decl, DM_ACCEPT_IDENT);

            /* Check for a bit-field declaration */
            FieldWidth = ParseFieldWidth (&Decl);

            /* Ignore zero sized bit fields in a union */
            if (FieldWidth == 0) {
                goto NextMember;
            }

            /* Check for fields without a name */
            if (Decl.Ident[0] == '\0') {
                /* In cc65 mode, we allow anonymous structs/unions within
                ** a struct.
                */
                if (IS_Get (&Standard) >= STD_CC65 && IsClassStruct (Decl.Type)) {
                    /* This is an anonymous struct or union. Copy the fields
                    ** into the current level.
                    */
                    CopyAnonStructFields (&Decl, 0);

                } else {
                    /* A non bit-field without a name is legal but useless */
                    Warning ("Declaration does not declare anything");
                }
                goto NextMember;
            }

            /* Handle sizes */
            FieldSize = CheckedSizeOf (Decl.Type);
            if (FieldSize > UnionSize) {
                UnionSize = FieldSize;
            }

            /* Add a field entry to the table. */
            if (FieldWidth > 0) {
                AddBitField (Decl.Ident, 0, 0, FieldWidth);
            } else {
                AddLocalSym (Decl.Ident, Decl.Type, SC_STRUCTFIELD, 0);
            }

NextMember: if (CurTok.Tok != TOK_COMMA) {
                break;
            }
            NextToken ();
        }
        ConsumeSemi ();
    }

    /* Skip the closing brace */
    NextToken ();

    /* Remember the symbol table and leave the struct level */
    FieldTab = GetSymTab ();
    LeaveStructLevel ();

    /* Make a real entry from the forward decl and return it */
    return AddStructSym (Name, SC_UNION, UnionSize, FieldTab);
}



static SymEntry* ParseStructDecl (const char* Name)
/* Parse a struct declaration. */
{

    unsigned  StructSize;
    int       FlexibleMember;
    int       BitOffs;          /* Bit offset for bit-fields */
    int       FieldWidth;       /* Width in bits, -1 if not a bit-field */
    SymTable* FieldTab;


    if (CurTok.Tok != TOK_LCURLY) {
        /* Just a forward declaration. */
        return StructOrUnionForwardDecl (Name, SC_STRUCT);
    }

    /* Add a forward declaration for the struct in the current lexical level */
    AddStructSym (Name, SC_STRUCT, 0, 0);

    /* Skip the curly brace */
    NextToken ();

    /* Enter a new lexical level for the struct */
    EnterStructLevel ();

    /* Parse struct fields */
    FlexibleMember = 0;
    StructSize     = 0;
    BitOffs        = 0;
    while (CurTok.Tok != TOK_RCURLY) {

        /* Get the type of the entry */
        DeclSpec Spec;
        InitDeclSpec (&Spec);
        ParseTypeSpec (&Spec, -1, T_QUAL_NONE);

        /* Read fields with this type */
        while (1) {

            Declaration Decl;
            ident       Ident;

            /* If we had a flexible array member before, no other fields can
            ** follow.
            */
            if (FlexibleMember) {
                Error ("Flexible array member must be last field");
                FlexibleMember = 0;     /* Avoid further errors */
            }

            /* Get type and name of the struct field */
            ParseDecl (&Spec, &Decl, DM_ACCEPT_IDENT);

            /* Check for a bit-field declaration */
            FieldWidth = ParseFieldWidth (&Decl);

            /* If this is not a bit field, or the bit field is too large for
            ** the remainder of the current member, or we have a bit field
            ** with width zero, align the struct to the next member by adding
            ** a member with an anonymous name.
            */
            if (BitOffs > 0) {
                if (FieldWidth <= 0 || (BitOffs + FieldWidth) > (int) INT_BITS) {

                    /* We need an anonymous name */
                    AnonName (Ident, "bit-field");

                    /* Add an anonymous bit-field that aligns to the next
                    ** storage unit.
                    */
                    AddBitField (Ident, StructSize, BitOffs, INT_BITS - BitOffs);

                    /* No bits left */
                    StructSize += SIZEOF_INT;
                    BitOffs = 0;
                }
            }

            /* Apart from the above, a bit field with width 0 is not processed
            ** further.
            */
            if (FieldWidth == 0) {
                goto NextMember;
            }

            /* Check if this field is a flexible array member, and
            ** calculate the size of the field.
            */
            if (IsTypeArray (Decl.Type) && GetElementCount (Decl.Type) == UNSPECIFIED) {
                /* Array with unspecified size */
                if (StructSize == 0) {
                    Error ("Flexible array member cannot be first struct field");
                }
                FlexibleMember = 1;
                /* Assume zero for size calculations */
                SetElementCount (Decl.Type, FLEXIBLE);
            }

            /* Check for fields without names */
            if (Decl.Ident[0] == '\0') {
                if (FieldWidth < 0) {
                    /* In cc65 mode, we allow anonymous structs/unions within
                    ** a struct.
                    */
                    if (IS_Get (&Standard) >= STD_CC65 && IsClassStruct (Decl.Type)) {

                        /* This is an anonymous struct or union. Copy the
                        ** fields into the current level.
                        */
                        StructSize += CopyAnonStructFields (&Decl, StructSize);

                    } else {
                        /* A non bit-field without a name is legal but useless */
                        Warning ("Declaration does not declare anything");
                    }
                    goto NextMember;
                } else {
                    /* A bit-field without a name will get an anonymous one */
                    AnonName (Decl.Ident, "bit-field");
                }
            }

            /* Add a field entry to the table */
            if (FieldWidth > 0) {
                /* Add full byte from the bit offset to the variable offset.
                ** This simplifies handling he bit-field as a char type
                ** in expressions.
                */
                unsigned Offs = StructSize + (BitOffs / CHAR_BITS);
                AddBitField (Decl.Ident, Offs, BitOffs % CHAR_BITS, FieldWidth);
                BitOffs += FieldWidth;
                CHECK (BitOffs <= (int) INT_BITS);
                if (BitOffs == INT_BITS) {
                    StructSize += SIZEOF_INT;
                    BitOffs = 0;
                }
            } else {
                AddLocalSym (Decl.Ident, Decl.Type, SC_STRUCTFIELD, StructSize);
                if (!FlexibleMember) {
                    StructSize += CheckedSizeOf (Decl.Type);
                }
            }

NextMember: if (CurTok.Tok != TOK_COMMA) {
                break;
            }
            NextToken ();
        }
        ConsumeSemi ();
    }

    /* If we have bits from bit-fields left, add them to the size. */
    if (BitOffs > 0) {
        StructSize += ((BitOffs + CHAR_BITS - 1) / CHAR_BITS);
    }

    /* Skip the closing brace */
    NextToken ();

    /* Remember the symbol table and leave the struct level */
    FieldTab = GetSymTab ();
    LeaveStructLevel ();

    /* Make a real entry from the forward decl and return it */
    return AddStructSym (Name, SC_STRUCT, StructSize, FieldTab);
}



static void ParseTypeSpec (DeclSpec* D, long Default, TypeCode Qualifiers)
/* Parse a type specifier */
{
    ident       Ident;
    SymEntry*   Entry;

    /* Assume we have an explicit type */
    D->Flags &= ~DS_DEF_TYPE;

    /* Read type qualifiers if we have any */
    Qualifiers |= OptionalQualifiers (T_QUAL_CONST | T_QUAL_VOLATILE);

    /* Look at the data type */
    switch (CurTok.Tok) {

        case TOK_VOID:
            NextToken ();
            D->Type[0].C = T_VOID;
            D->Type[1].C = T_END;
            break;

        case TOK_CHAR:
            NextToken ();
            D->Type[0].C = GetDefaultChar();
            D->Type[1].C = T_END;
            break;

        case TOK_LONG:
            NextToken ();
            if (CurTok.Tok == TOK_UNSIGNED) {
                NextToken ();
                OptionalInt ();
                D->Type[0].C = T_ULONG;
                D->Type[1].C = T_END;
            } else {
                OptionalSigned ();
                OptionalInt ();
                D->Type[0].C = T_LONG;
                D->Type[1].C = T_END;
            }
            break;

        case TOK_SHORT:
            NextToken ();
            if (CurTok.Tok == TOK_UNSIGNED) {
                NextToken ();
                OptionalInt ();
                D->Type[0].C = T_USHORT;
                D->Type[1].C = T_END;
            } else {
                OptionalSigned ();
                OptionalInt ();
                D->Type[0].C = T_SHORT;
                D->Type[1].C = T_END;
            }
            break;

        case TOK_INT:
            NextToken ();
            D->Type[0].C = T_INT;
            D->Type[1].C = T_END;
            break;

       case TOK_SIGNED:
            NextToken ();
            switch (CurTok.Tok) {

                case TOK_CHAR:
                    NextToken ();
                    D->Type[0].C = T_SCHAR;
                    D->Type[1].C = T_END;
                    break;

                case TOK_SHORT:
                    NextToken ();
                    OptionalInt ();
                    D->Type[0].C = T_SHORT;
                    D->Type[1].C = T_END;
                    break;

                case TOK_LONG:
                    NextToken ();
                    OptionalInt ();
                    D->Type[0].C = T_LONG;
                    D->Type[1].C = T_END;
                    break;

                case TOK_INT:
                    NextToken ();
                    /* FALL THROUGH */

                default:
                    D->Type[0].C = T_INT;
                    D->Type[1].C = T_END;
                    break;
            }
            break;

        case TOK_UNSIGNED:
            NextToken ();
            switch (CurTok.Tok) {

                case TOK_CHAR:
                    NextToken ();
                    D->Type[0].C = T_UCHAR;
                    D->Type[1].C = T_END;
                    break;

                case TOK_SHORT:
                    NextToken ();
                    OptionalInt ();
                    D->Type[0].C = T_USHORT;
                    D->Type[1].C = T_END;
                    break;

                case TOK_LONG:
                    NextToken ();
                    OptionalInt ();
                    D->Type[0].C = T_ULONG;
                    D->Type[1].C = T_END;
                    break;

                case TOK_INT:
                    NextToken ();
                    /* FALL THROUGH */

                default:
                    D->Type[0].C = T_UINT;
                    D->Type[1].C = T_END;
                    break;
            }
            break;

        case TOK_FLOAT:
            NextToken ();
            D->Type[0].C = T_FLOAT;
            D->Type[1].C = T_END;
            break;

        case TOK_DOUBLE:
            NextToken ();
            D->Type[0].C = T_DOUBLE;
            D->Type[1].C = T_END;
            break;

        case TOK_UNION:
            NextToken ();
            /* */
            if (CurTok.Tok == TOK_IDENT) {
                strcpy (Ident, CurTok.Ident);
                NextToken ();
            } else {
                AnonName (Ident, "union");
            }
            /* Remember we have an extra type decl */
            D->Flags |= DS_EXTRA_TYPE;
            /* Declare the union in the current scope */
            Entry = ParseUnionDecl (Ident);
            /* Encode the union entry into the type */
            D->Type[0].C = T_UNION;
            SetSymEntry (D->Type, Entry);
            D->Type[1].C = T_END;
            break;

        case TOK_STRUCT:
            NextToken ();
            /* */
            if (CurTok.Tok == TOK_IDENT) {
                strcpy (Ident, CurTok.Ident);
                NextToken ();
            } else {
                AnonName (Ident, "struct");
            }
            /* Remember we have an extra type decl */
            D->Flags |= DS_EXTRA_TYPE;
            /* Declare the struct in the current scope */
            Entry = ParseStructDecl (Ident);
            /* Encode the struct entry into the type */
            D->Type[0].C = T_STRUCT;
            SetSymEntry (D->Type, Entry);
            D->Type[1].C = T_END;
            break;

        case TOK_ENUM:
            NextToken ();
            if (CurTok.Tok != TOK_LCURLY) {
                /* Named enum */
                if (CurTok.Tok == TOK_IDENT) {
                    /* Find an entry with this name */
                    Entry = FindTagSym (CurTok.Ident);
                    if (Entry) {
                        if (SymIsLocal (Entry) && (Entry->Flags & SC_ENUM) == 0) {
                            Error ("Symbol `%s' is already different kind", Entry->Name);
                        }
                    } else {
                        /* Insert entry into table ### */
                    }
                    /* Skip the identifier */
                    NextToken ();
                } else {
                    Error ("Identifier expected");
                }
            }
            /* Remember we have an extra type decl */
            D->Flags |= DS_EXTRA_TYPE;
            /* Parse the enum decl */
            ParseEnumDecl ();
            D->Type[0].C = T_INT;
            D->Type[1].C = T_END;
            break;

        case TOK_IDENT:
            Entry = FindSym (CurTok.Ident);
            if (Entry && SymIsTypeDef (Entry)) {
                /* It's a typedef */
                NextToken ();
                TypeCopy (D->Type, Entry->Type);
                break;
            }
            /* FALL THROUGH */

        default:
            if (Default < 0) {
                Error ("Type expected");
                D->Type[0].C = T_INT;
                D->Type[1].C = T_END;
            } else {
                D->Flags |= DS_DEF_TYPE;
                D->Type[0].C = (TypeCode) Default;
                D->Type[1].C = T_END;
            }
            break;
    }

    /* There may also be qualifiers *after* the initial type */
    D->Type[0].C |= (Qualifiers | OptionalQualifiers (T_QUAL_CONST | T_QUAL_VOLATILE));
}



static Type* ParamTypeCvt (Type* T)
/* If T is an array, convert it to a pointer else do nothing. Return the
** resulting type.
*/
{
    if (IsTypeArray (T)) {
        T->C = T_PTR;
    }
    return T;
}



static void ParseOldStyleParamList (FuncDesc* F)
/* Parse an old style (K&R) parameter list */
{
    /* Some fix point tokens that are used for error recovery */
    static const token_t TokenList[] = { TOK_COMMA, TOK_RPAREN, TOK_SEMI };

    /* Parse params */
    while (CurTok.Tok != TOK_RPAREN) {

        /* List of identifiers expected */
        if (CurTok.Tok == TOK_IDENT) {

            /* Create a symbol table entry with type int */
            AddLocalSym (CurTok.Ident, type_int, SC_AUTO | SC_PARAM | SC_DEF | SC_DEFTYPE, 0);

            /* Count arguments */
            ++F->ParamCount;

            /* Skip the identifier */
            NextToken ();

        } else {
            /* Not a parameter name */
            Error ("Identifier expected");

            /* Try some smart error recovery */
            SkipTokens (TokenList, sizeof(TokenList) / sizeof(TokenList[0]));
        }

        /* Check for more parameters */
        if (CurTok.Tok == TOK_COMMA) {
            NextToken ();
        } else {
            break;
        }
    }

    /* Skip right paren. We must explicitly check for one here, since some of
    ** the breaks above bail out without checking.
    */
    ConsumeRParen ();

    /* An optional list of type specifications follows */
    while (CurTok.Tok != TOK_LCURLY) {

        DeclSpec        Spec;

        /* Read the declaration specifier */
        ParseDeclSpec (&Spec, SC_AUTO, T_INT);

        /* We accept only auto and register as storage class specifiers, but
        ** we ignore all this, since we use auto anyway.
        */
        if ((Spec.StorageClass & SC_AUTO) == 0 &&
            (Spec.StorageClass & SC_REGISTER) == 0) {
            Error ("Illegal storage class");
        }

        /* Parse a comma separated variable list */
        while (1) {

            Declaration         Decl;

            /* Read the parameter */
            ParseDecl (&Spec, &Decl, DM_NEED_IDENT);
            if (Decl.Ident[0] != '\0') {

                /* We have a name given. Search for the symbol */
                SymEntry* Sym = FindLocalSym (Decl.Ident);
                if (Sym) {
                    /* Check if we already changed the type for this
                    ** parameter
                    */
                    if (Sym->Flags & SC_DEFTYPE) {
                        /* Found it, change the default type to the one given */
                        ChangeSymType (Sym, ParamTypeCvt (Decl.Type));
                        /* Reset the "default type" flag */
                        Sym->Flags &= ~SC_DEFTYPE;
                    } else {
                        /* Type has already been changed */
                        Error ("Redefinition for parameter `%s'", Sym->Name);
                    }
                } else {
                    Error ("Unknown identifier: `%s'", Decl.Ident);
                }
            }

            if (CurTok.Tok == TOK_COMMA) {
                NextToken ();
            } else {
                break;
            }

        }

        /* Variable list must be semicolon terminated */
        ConsumeSemi ();
    }
}



static void ParseAnsiParamList (FuncDesc* F)
/* Parse a new style (ANSI) parameter list */
{
    /* Parse params */
    while (CurTok.Tok != TOK_RPAREN) {

        DeclSpec        Spec;
        Declaration     Decl;
        SymEntry*       Sym;

        /* Allow an ellipsis as last parameter */
        if (CurTok.Tok == TOK_ELLIPSIS) {
            NextToken ();
            F->Flags |= FD_VARIADIC;
            break;
        }

        /* Read the declaration specifier */
        ParseDeclSpec (&Spec, SC_AUTO, T_INT);

        /* We accept only auto and register as storage class specifiers */
        if ((Spec.StorageClass & SC_AUTO) == SC_AUTO) {
            Spec.StorageClass = SC_AUTO | SC_PARAM | SC_DEF;
        } else if ((Spec.StorageClass & SC_REGISTER) == SC_REGISTER) {
            Spec.StorageClass = SC_REGISTER | SC_STATIC | SC_PARAM | SC_DEF;
        } else {
            Error ("Illegal storage class");
            Spec.StorageClass = SC_AUTO | SC_PARAM | SC_DEF;
        }

        /* Allow parameters without a name, but remember if we had some to
        ** eventually print an error message later.
        */
        ParseDecl (&Spec, &Decl, DM_ACCEPT_IDENT);
        if (Decl.Ident[0] == '\0') {

            /* Unnamed symbol. Generate a name that is not user accessible,
            ** then handle the symbol normal.
            */
            AnonName (Decl.Ident, "param");
            F->Flags |= FD_UNNAMED_PARAMS;

            /* Clear defined bit on nonames */
            Decl.StorageClass &= ~SC_DEF;
        }

        /* Parse attributes for this parameter */
        ParseAttribute (&Decl);

        /* Create a symbol table entry */
        Sym = AddLocalSym (Decl.Ident, ParamTypeCvt (Decl.Type), Decl.StorageClass, 0);

        /* Add attributes if we have any */
        SymUseAttr (Sym, &Decl);

        /* If the parameter is a struct or union, emit a warning */
        if (IsClassStruct (Decl.Type)) {
            if (IS_Get (&WarnStructParam)) {
                Warning ("Passing struct by value for parameter `%s'", Decl.Ident);
            }
        }

        /* Count arguments */
        ++F->ParamCount;

        /* Check for more parameters */
        if (CurTok.Tok == TOK_COMMA) {
            NextToken ();
        } else {
            break;
        }
    }

    /* Skip right paren. We must explicitly check for one here, since some of
    ** the breaks above bail out without checking.
    */
    ConsumeRParen ();
}



static FuncDesc* ParseFuncDecl (void)
/* Parse the argument list of a function. */
{
    unsigned Offs;
    SymEntry* Sym;

    /* Create a new function descriptor */
    FuncDesc* F = NewFuncDesc ();

    /* Enter a new lexical level */
    EnterFunctionLevel ();

    /* Check for several special parameter lists */
    if (CurTok.Tok == TOK_RPAREN) {
        /* Parameter list is empty */
        F->Flags |= (FD_EMPTY | FD_VARIADIC);
    } else if (CurTok.Tok == TOK_VOID && NextTok.Tok == TOK_RPAREN) {
        /* Parameter list declared as void */
        NextToken ();
        F->Flags |= FD_VOID_PARAM;
    } else if (CurTok.Tok == TOK_IDENT &&
               (NextTok.Tok == TOK_COMMA || NextTok.Tok == TOK_RPAREN)) {
        /* If the identifier is a typedef, we have a new style parameter list,
        ** if it's some other identifier, it's an old style parameter list.
        */
        Sym = FindSym (CurTok.Ident);
        if (Sym == 0 || !SymIsTypeDef (Sym)) {
            /* Old style (K&R) function. */
            F->Flags |= FD_OLDSTYLE;
        }
    }

    /* Parse params */
    if ((F->Flags & FD_OLDSTYLE) == 0) {

        /* New style function */
        ParseAnsiParamList (F);

    } else {
        /* Old style function */
        ParseOldStyleParamList (F);
    }

    /* Remember the last function parameter. We need it later for several
    ** purposes, for example when passing stuff to fastcall functions. Since
    ** more symbols are added to the table, it is easier if we remember it
    ** now, since it is currently the last entry in the symbol table.
    */
    F->LastParam = GetSymTab()->SymTail;

    /* Assign offsets. If the function has a variable parameter list,
    ** there's one additional byte (the arg size).
    */
    Offs = (F->Flags & FD_VARIADIC)? 1 : 0;
    Sym = F->LastParam;
    while (Sym) {
        unsigned Size = CheckedSizeOf (Sym->Type);
        if (SymIsRegVar (Sym)) {
            Sym->V.R.SaveOffs = Offs;
        } else {
            Sym->V.Offs = Offs;
        }
        Offs += Size;
        F->ParamSize += Size;
        Sym = Sym->PrevSym;
    }

    /* Leave the lexical level remembering the symbol tables */
    RememberFunctionLevel (F);

    /* Return the function descriptor */
    return F;
}



static void Declarator (const DeclSpec* Spec, Declaration* D, declmode_t Mode)
/* Recursively process declarators. Build a type array in reverse order. */
{
    /* Read optional function or pointer qualifiers. They modify the
    ** identifier or token to the right. For convenience, we allow a calling
    ** convention also for pointers here. If it's a pointer-to-function, the
    ** qualifier later will be transfered to the function itself. If it's a
    ** pointer to something else, it will be flagged as an error.
    */
    TypeCode Qualifiers = OptionalQualifiers (T_QUAL_ADDRSIZE | T_QUAL_CCONV);

    /* Pointer to something */
    if (CurTok.Tok == TOK_STAR) {

        /* Skip the star */
        NextToken ();

        /* Allow const, restrict, and volatile qualifiers */
        Qualifiers |= OptionalQualifiers (T_QUAL_CONST | T_QUAL_VOLATILE | T_QUAL_RESTRICT);

        /* Parse the type that the pointer points to */
        Declarator (Spec, D, Mode);

        /* Add the type */
        AddTypeToDeclaration (D, T_PTR | Qualifiers);
        return;
    }

    if (CurTok.Tok == TOK_LPAREN) {
        NextToken ();
        Declarator (Spec, D, Mode);
        ConsumeRParen ();
    } else {
        /* Things depend on Mode now:
        **  - Mode == DM_NEED_IDENT means:
        **      we *must* have a type and a variable identifer.
        **  - Mode == DM_NO_IDENT means:
        **      we must have a type but no variable identifer
        **      (if there is one, it's not read).
        **  - Mode == DM_ACCEPT_IDENT means:
        **      we *may* have an identifier. If there is an identifier,
        **      it is read, but it is no error, if there is none.
        */
        if (Mode == DM_NO_IDENT) {
            D->Ident[0] = '\0';
        } else if (CurTok.Tok == TOK_IDENT) {
            strcpy (D->Ident, CurTok.Ident);
            NextToken ();
        } else {
            if (Mode == DM_NEED_IDENT) {
                Error ("Identifier expected");
            }
            D->Ident[0] = '\0';
        }
    }

    while (CurTok.Tok == TOK_LBRACK || CurTok.Tok == TOK_LPAREN) {
        if (CurTok.Tok == TOK_LPAREN) {

            /* Function declaration */
            FuncDesc* F;

            /* Skip the opening paren */
            NextToken ();

            /* Parse the function declaration */
            F = ParseFuncDecl ();

            /* We cannot specify fastcall for variadic functions */
            if ((F->Flags & FD_VARIADIC) && (Qualifiers & T_QUAL_FASTCALL)) {
                Error ("Variadic functions cannot be __fastcall__");
                Qualifiers &= ~T_QUAL_FASTCALL;
            }

            /* Add the function type. Be sure to bounds check the type buffer */
            NeedTypeSpace (D, 1);
            D->Type[D->Index].C = T_FUNC | Qualifiers;
            D->Type[D->Index].A.P = F;
            ++D->Index;

            /* Qualifiers now used */
            Qualifiers = T_QUAL_NONE;

        } else {
            /* Array declaration. */
            long Size = UNSPECIFIED;

            /* We cannot have any qualifiers for an array */
            if (Qualifiers != T_QUAL_NONE) {
                Error ("Invalid qualifiers for array");
                Qualifiers = T_QUAL_NONE;
            }

            /* Skip the left bracket */
            NextToken ();

            /* Read the size if it is given */
            if (CurTok.Tok != TOK_RBRACK) {
                ExprDesc Expr;
                ConstAbsIntExpr (hie1, &Expr);
                if (Expr.IVal <= 0) {
                    if (D->Ident[0] != '\0') {
                        Error ("Size of array `%s' is invalid", D->Ident);
                    } else {
                        Error ("Size of array is invalid");
                    }
                    Expr.IVal = 1;
                }
                Size = Expr.IVal;
            }

            /* Skip the right bracket */
            ConsumeRBrack ();

            /* Add the array type with the size to the type */
            NeedTypeSpace (D, 1);
            D->Type[D->Index].C = T_ARRAY;
            D->Type[D->Index].A.L = Size;
            ++D->Index;
        }
    }

    /* If we have remaining qualifiers, flag them as invalid */
    if (Qualifiers & T_QUAL_NEAR) {
        Error ("Invalid `__near__' qualifier");
    }
    if (Qualifiers & T_QUAL_FAR) {
        Error ("Invalid `__far__' qualifier");
    }
    if (Qualifiers & T_QUAL_FASTCALL) {
        Error ("Invalid `__fastcall__' qualifier");
    }
    if (Qualifiers & T_QUAL_CDECL) {
        Error ("Invalid `__cdecl__' qualifier");
    }
}



/*****************************************************************************/
/*                                   code                                    */
/*****************************************************************************/



Type* ParseType (Type* T)
/* Parse a complete type specification */
{
    DeclSpec Spec;
    Declaration Decl;

    /* Get a type without a default */
    InitDeclSpec (&Spec);
    ParseTypeSpec (&Spec, -1, T_QUAL_NONE);

    /* Parse additional declarators */
    ParseDecl (&Spec, &Decl, DM_NO_IDENT);

    /* Copy the type to the target buffer */
    TypeCopy (T, Decl.Type);

    /* Return a pointer to the target buffer */
    return T;
}



void ParseDecl (const DeclSpec* Spec, Declaration* D, declmode_t Mode)
/* Parse a variable, type or function declaration */
{
    /* Initialize the Declaration struct */
    InitDeclaration (D);

    /* Get additional declarators and the identifier */
    Declarator (Spec, D, Mode);

    /* Add the base type. */
    NeedTypeSpace (D, TypeLen (Spec->Type) + 1);        /* Bounds check */
    TypeCopy (D->Type + D->Index, Spec->Type);

    /* Use the storage class from the declspec */
    D->StorageClass = Spec->StorageClass;

    /* Do several fixes on qualifiers */
    FixQualifiers (D->Type);

    /* If we have a function, add a special storage class */
    if (IsTypeFunc (D->Type)) {
        D->StorageClass |= SC_FUNC;
    }

    /* Parse attributes for this declaration */
    ParseAttribute (D);

    /* Check several things for function or function pointer types */
    if (IsTypeFunc (D->Type) || IsTypeFuncPtr (D->Type)) {

        /* A function. Check the return type */
        Type* RetType = GetFuncReturn (D->Type);

        /* Functions may not return functions or arrays */
        if (IsTypeFunc (RetType)) {
            Error ("Functions are not allowed to return functions");
        } else if (IsTypeArray (RetType)) {
            Error ("Functions are not allowed to return arrays");
        }

        /* The return type must not be qualified */
        if (GetQualifier (RetType) != T_QUAL_NONE && RetType[1].C == T_END) {

            if (GetType (RetType) == T_TYPE_VOID) {
                /* A qualified void type is always an error */
                Error ("function definition has qualified void return type");
            } else {
                /* For others, qualifiers are ignored */
                Warning ("type qualifiers ignored on function return type");
                RetType[0].C = UnqualifiedType (RetType[0].C);
            }
        }

        /* Warn about an implicit int return in the function */
        if ((Spec->Flags & DS_DEF_TYPE) != 0 &&
            RetType[0].C == T_INT && RetType[1].C == T_END) {
            /* Function has an implicit int return. Output a warning if we don't
            ** have the C89 standard enabled explicitly.
            */
            if (IS_Get (&Standard) >= STD_C99) {
                Warning ("Implicit `int' return type is an obsolete feature");
            }
            GetFuncDesc (D->Type)->Flags |= FD_OLDSTYLE_INTRET;
        }

    }

    /* For anthing that is not a function or typedef, check for an implicit
    ** int declaration.
    */
    if ((D->StorageClass & SC_FUNC) != SC_FUNC &&
        (D->StorageClass & SC_TYPEMASK) != SC_TYPEDEF) {
        /* If the standard was not set explicitly to C89, print a warning
        ** for variables with implicit int type.
        */
        if ((Spec->Flags & DS_DEF_TYPE) != 0 && IS_Get (&Standard) >= STD_C99) {
            Warning ("Implicit `int' is an obsolete feature");
        }
    }

    /* Check the size of the generated type */
    if (!IsTypeFunc (D->Type) && !IsTypeVoid (D->Type)) {
        unsigned Size = SizeOf (D->Type);
        if (Size >= 0x10000) {
            if (D->Ident[0] != '\0') {
                Error ("Size of `%s' is invalid (0x%06X)", D->Ident, Size);
            } else {
                Error ("Invalid size in declaration (0x%06X)", Size);
            }
        }
    }

}



void ParseDeclSpec (DeclSpec* D, unsigned DefStorage, long DefType)
/* Parse a declaration specification */
{
    TypeCode Qualifiers;

    /* Initialize the DeclSpec struct */
    InitDeclSpec (D);

    /* There may be qualifiers *before* the storage class specifier */
    Qualifiers = OptionalQualifiers (T_QUAL_CONST | T_QUAL_VOLATILE);

    /* Now get the storage class specifier for this declaration */
    ParseStorageClass (D, DefStorage);

    /* Parse the type specifiers passing any initial type qualifiers */
    ParseTypeSpec (D, DefType, Qualifiers);
}



void CheckEmptyDecl (const DeclSpec* D)
/* Called after an empty type declaration (that is, a type declaration without
** a variable). Checks if the declaration does really make sense and issues a
** warning if not.
*/
{
    if ((D->Flags & DS_EXTRA_TYPE) == 0) {
        Warning ("Useless declaration");
    }
}



static void SkipInitializer (unsigned BracesExpected)
/* Skip the remainder of an initializer in case of errors. Try to be somewhat
** smart so we don't have too many following errors.
*/
{
    while (CurTok.Tok != TOK_CEOF && CurTok.Tok != TOK_SEMI && BracesExpected > 0) {
        switch (CurTok.Tok) {
            case TOK_RCURLY:    --BracesExpected;   break;
            case TOK_LCURLY:    ++BracesExpected;   break;
            default:                                break;
        }
        NextToken ();
    }
}



static unsigned OpeningCurlyBraces (unsigned BracesNeeded)
/* Accept any number of opening curly braces around an initialization, skip
** them and return the number. If the number of curly braces is less than
** BracesNeeded, issue a warning.
*/
{
    unsigned BraceCount = 0;
    while (CurTok.Tok == TOK_LCURLY) {
        ++BraceCount;
        NextToken ();
    }
    if (BraceCount < BracesNeeded) {
        Error ("`{' expected");
    }
    return BraceCount;
}



static void ClosingCurlyBraces (unsigned BracesExpected)
/* Accept and skip the given number of closing curly braces together with
** an optional comma. Output an error messages, if the input does not contain
** the expected number of braces.
*/
{
    while (BracesExpected) {
        if (CurTok.Tok == TOK_RCURLY) {
            NextToken ();
        } else if (CurTok.Tok == TOK_COMMA && NextTok.Tok == TOK_RCURLY) {
            NextToken ();
            NextToken ();
        } else {
            Error ("`}' expected");
            return;
        }
        --BracesExpected;
    }
}



static void DefineData (ExprDesc* Expr)
/* Output a data definition for the given expression */
{
    switch (ED_GetLoc (Expr)) {

        case E_LOC_ABS:
            /* Absolute: numeric address or const */
            g_defdata (TypeOf (Expr->Type) | CF_CONST, Expr->IVal, 0);
            break;

        case E_LOC_GLOBAL:
            /* Global variable */
            g_defdata (CF_EXTERNAL, Expr->Name, Expr->IVal);
            break;

        case E_LOC_STATIC:
        case E_LOC_LITERAL:
            /* Static variable or literal in the literal pool */
            g_defdata (CF_STATIC, Expr->Name, Expr->IVal);
            break;

        case E_LOC_REGISTER:
            /* Register variable. Taking the address is usually not
            ** allowed.
            */
            if (IS_Get (&AllowRegVarAddr) == 0) {
                Error ("Cannot take the address of a register variable");
            }
            g_defdata (CF_REGVAR, Expr->Name, Expr->IVal);
            break;

        case E_LOC_STACK:
        case E_LOC_PRIMARY:
        case E_LOC_EXPR:
            Error ("Non constant initializer");
            break;

        default:
            Internal ("Unknown constant type: 0x%04X", ED_GetLoc (Expr));
    }
}



static void OutputBitFieldData (StructInitData* SI)
/* Output bit field data */
{
    /* Ignore if we have no data */
    if (SI->ValBits > 0) {

        /* Output the data */
        g_defdata (CF_INT | CF_UNSIGNED | CF_CONST, SI->BitVal, 0);

        /* Clear the data from SI and account for the size */
        SI->BitVal  = 0;
        SI->ValBits = 0;
        SI->Offs   += SIZEOF_INT;
    }
}



static void ParseScalarInitInternal (Type* T, ExprDesc* ED)
/* Parse initializaton for scalar data types. This function will not output the
** data but return it in ED.
*/
{
    /* Optional opening brace */
    unsigned BraceCount = OpeningCurlyBraces (0);

    /* We warn if an initializer for a scalar contains braces, because this is
    ** quite unusual and often a sign for some problem in the input.
    */
    if (BraceCount > 0) {
        Warning ("Braces around scalar initializer");
    }

    /* Get the expression and convert it to the target type */
    ConstExpr (hie1, ED);
    TypeConversion (ED, T);

    /* Close eventually opening braces */
    ClosingCurlyBraces (BraceCount);
}



static unsigned ParseScalarInit (Type* T)
/* Parse initializaton for scalar data types. Return the number of data bytes. */
{
    ExprDesc ED;

    /* Parse initialization */
    ParseScalarInitInternal (T, &ED);

    /* Output the data */
    DefineData (&ED);

    /* Done */
    return SizeOf (T);
}



static unsigned ParsePointerInit (Type* T)
/* Parse initializaton for pointer data types. Return the number of data bytes. */
{
    /* Optional opening brace */
    unsigned BraceCount = OpeningCurlyBraces (0);

    /* Expression */
    ExprDesc ED;
    ConstExpr (hie1, &ED);
    TypeConversion (&ED, T);

    /* Output the data */
    DefineData (&ED);

    /* Close eventually opening braces */
    ClosingCurlyBraces (BraceCount);

    /* Done */
    return SIZEOF_PTR;
}



static unsigned ParseArrayInit (Type* T, int AllowFlexibleMembers)
/* Parse initializaton for arrays. Return the number of data bytes. */
{
    int Count;

    /* Get the array data */
    Type* ElementType    = GetElementType (T);
    unsigned ElementSize = CheckedSizeOf (ElementType);
    long ElementCount    = GetElementCount (T);

    /* Special handling for a character array initialized by a literal */
    if (IsTypeChar (ElementType) &&
        (CurTok.Tok == TOK_SCONST || CurTok.Tok == TOK_WCSCONST ||
        (CurTok.Tok == TOK_LCURLY &&
         (NextTok.Tok == TOK_SCONST || NextTok.Tok == TOK_WCSCONST)))) {

        /* Char array initialized by string constant */
        int NeedParen;

        /* If we initializer is enclosed in brackets, remember this fact and
        ** skip the opening bracket.
        */
        NeedParen = (CurTok.Tok == TOK_LCURLY);
        if (NeedParen) {
            NextToken ();
        }

        /* Translate into target charset */
        TranslateLiteral (CurTok.SVal);

        /* If the array is one too small for the string literal, omit the
        ** trailing zero.
        */
        Count = GetLiteralSize (CurTok.SVal);
        if (ElementCount != UNSPECIFIED &&
            ElementCount != FLEXIBLE    &&
            Count        == ElementCount + 1) {
            /* Omit the trailing zero */
            --Count;
        }

        /* Output the data */
        g_defbytes (GetLiteralStr (CurTok.SVal), Count);

        /* Skip the string */
        NextToken ();

        /* If the initializer was enclosed in curly braces, we need a closing
        ** one.
        */
        if (NeedParen) {
            ConsumeRCurly ();
        }

    } else {

        /* Curly brace */
        ConsumeLCurly ();

        /* Initialize the array members */
        Count = 0;
        while (CurTok.Tok != TOK_RCURLY) {
            /* Flexible array members may not be initialized within
            ** an array (because the size of each element may differ
            ** otherwise).
            */
            ParseInitInternal (ElementType, 0);
            ++Count;
            if (CurTok.Tok != TOK_COMMA)
                break;
            NextToken ();
        }

        /* Closing curly braces */
        ConsumeRCurly ();
    }

    if (ElementCount == UNSPECIFIED) {
        /* Number of elements determined by initializer */
        SetElementCount (T, Count);
        ElementCount = Count;
    } else if (ElementCount == FLEXIBLE && AllowFlexibleMembers) {
        /* In non ANSI mode, allow initialization of flexible array
        ** members.
        */
        ElementCount = Count;
    } else if (Count < ElementCount) {
        g_zerobytes ((ElementCount - Count) * ElementSize);
    } else if (Count > ElementCount) {
        Error ("Too many initializers");
    }
    return ElementCount * ElementSize;
}



static unsigned ParseStructInit (Type* T, int AllowFlexibleMembers)
/* Parse initialization of a struct or union. Return the number of data bytes. */
{
    SymEntry*       Entry;
    SymTable*       Tab;
    StructInitData  SI;


    /* Consume the opening curly brace */
    ConsumeLCurly ();

    /* Get a pointer to the struct entry from the type */
    Entry = GetSymEntry (T);

    /* Get the size of the struct from the symbol table entry */
    SI.Size = Entry->V.S.Size;

    /* Check if this struct definition has a field table. If it doesn't, it
    ** is an incomplete definition.
    */
    Tab = Entry->V.S.SymTab;
    if (Tab == 0) {
        Error ("Cannot initialize variables with incomplete type");
        /* Try error recovery */
        SkipInitializer (1);
        /* Nothing initialized */
        return 0;
    }

    /* Get a pointer to the list of symbols */
    Entry = Tab->SymHead;

    /* Initialize fields */
    SI.Offs    = 0;
    SI.BitVal  = 0;
    SI.ValBits = 0;
    while (CurTok.Tok != TOK_RCURLY) {

        /* */
        if (Entry == 0) {
            Error ("Too many initializers");
            SkipInitializer (1);
            return SI.Offs;
        }

        /* Parse initialization of one field. Bit-fields need a special
        ** handling.
        */
        if (SymIsBitField (Entry)) {

            ExprDesc ED;
            unsigned Val;
            unsigned Shift;

            /* Calculate the bitmask from the bit-field data */
            unsigned Mask = (1U << Entry->V.B.BitWidth) - 1U;

            /* Safety ... */
            CHECK (Entry->V.B.Offs * CHAR_BITS + Entry->V.B.BitOffs ==
                   SI.Offs         * CHAR_BITS + SI.ValBits);

            /* This may be an anonymous bit-field, in which case it doesn't
            ** have an initializer.
            */
            if (IsAnonName (Entry->Name)) {
                /* Account for the data and output it if we have a full word */
                SI.ValBits += Entry->V.B.BitWidth;
                CHECK (SI.ValBits <= INT_BITS);
                if (SI.ValBits == INT_BITS) {
                    OutputBitFieldData (&SI);
                }
                goto NextMember;
            } else {
                /* Read the data, check for a constant integer, do a range
                ** check.
                */
                ParseScalarInitInternal (type_uint, &ED);
                if (!ED_IsConstAbsInt (&ED)) {
                    Error ("Constant initializer expected");
                    ED_MakeConstAbsInt (&ED, 1);
                }
                if (ED.IVal > (long) Mask) {
                    Warning ("Truncating value in bit-field initializer");
                    ED.IVal &= (long) Mask;
                }
                Val = (unsigned) ED.IVal;
            }

            /* Add the value to the currently stored bit-field value */
            Shift = (Entry->V.B.Offs - SI.Offs) * CHAR_BITS + Entry->V.B.BitOffs;
            SI.BitVal |= (Val << Shift);

            /* Account for the data and output it if we have a full word */
            SI.ValBits += Entry->V.B.BitWidth;
            CHECK (SI.ValBits <= INT_BITS);
            if (SI.ValBits == INT_BITS) {
                OutputBitFieldData (&SI);
            }

        } else {

            /* Standard member. We should never have stuff from a
            ** bit-field left
            */
            CHECK (SI.ValBits == 0);

            /* Flexible array members may only be initialized if they are
            ** the last field (or part of the last struct field).
            */
            SI.Offs += ParseInitInternal (Entry->Type, AllowFlexibleMembers && Entry->NextSym == 0);
        }

        /* More initializers? */
        if (CurTok.Tok != TOK_COMMA) {
            break;
        }

        /* Skip the comma */
        NextToken ();

NextMember:
        /* Next member. For unions, only the first one can be initialized */
        if (IsTypeUnion (T)) {
            /* Union */
            Entry = 0;
        } else {
            /* Struct */
            Entry = Entry->NextSym;
        }
    }

    /* Consume the closing curly brace */
    ConsumeRCurly ();

    /* If we have data from a bit-field left, output it now */
    OutputBitFieldData (&SI);

    /* If there are struct fields left, reserve additional storage */
    if (SI.Offs < SI.Size) {
        g_zerobytes (SI.Size - SI.Offs);
        SI.Offs = SI.Size;
    }

    /* Return the actual number of bytes initialized. This number may be
    ** larger than sizeof (Struct) if flexible array members are present and
    ** were initialized (possible in non ANSI mode).
    */
    return SI.Offs;
}



static unsigned ParseVoidInit (void)
/* Parse an initialization of a void variable (special cc65 extension).
** Return the number of bytes initialized.
*/
{
    ExprDesc Expr;
    unsigned Size;

    /* Opening brace */
    ConsumeLCurly ();

    /* Allow an arbitrary list of values */
    Size = 0;
    do {
        ConstExpr (hie1, &Expr);
        switch (UnqualifiedType (Expr.Type[0].C)) {

            case T_SCHAR:
            case T_UCHAR:
                if (ED_IsConstAbsInt (&Expr)) {
                    /* Make it byte sized */
                    Expr.IVal &= 0xFF;
                }
                DefineData (&Expr);
                Size += SIZEOF_CHAR;
                break;

            case T_SHORT:
            case T_USHORT:
            case T_INT:
            case T_UINT:
            case T_PTR:
            case T_ARRAY:
                if (ED_IsConstAbsInt (&Expr)) {
                    /* Make it word sized */
                    Expr.IVal &= 0xFFFF;
                }
                DefineData (&Expr);
                Size += SIZEOF_INT;
                break;

            case T_LONG:
            case T_ULONG:
                if (ED_IsConstAbsInt (&Expr)) {
                    /* Make it dword sized */
                    Expr.IVal &= 0xFFFFFFFF;
                }
                DefineData (&Expr);
                Size += SIZEOF_LONG;
                break;

            default:
                Error ("Illegal type in initialization");
                break;

        }

        if (CurTok.Tok != TOK_COMMA) {
            break;
        }
        NextToken ();

    } while (CurTok.Tok != TOK_RCURLY);

    /* Closing brace */
    ConsumeRCurly ();

    /* Return the number of bytes initialized */
    return Size;
}



static unsigned ParseInitInternal (Type* T, int AllowFlexibleMembers)
/* Parse initialization of variables. Return the number of data bytes. */
{
    switch (UnqualifiedType (T->C)) {

        case T_SCHAR:
        case T_UCHAR:
        case T_SHORT:
        case T_USHORT:
        case T_INT:
        case T_UINT:
        case T_LONG:
        case T_ULONG:
        case T_FLOAT:
        case T_DOUBLE:
            return ParseScalarInit (T);

        case T_PTR:
            return ParsePointerInit (T);

        case T_ARRAY:
            return ParseArrayInit (T, AllowFlexibleMembers);

        case T_STRUCT:
        case T_UNION:
            return ParseStructInit (T, AllowFlexibleMembers);

        case T_VOID:
            if (IS_Get (&Standard) == STD_CC65) {
                /* Special cc65 extension in non ANSI mode */
                return ParseVoidInit ();
            }
            /* FALLTHROUGH */

        default:
            Error ("Illegal type");
            return SIZEOF_CHAR;

    }
}



unsigned ParseInit (Type* T)
/* Parse initialization of variables. Return the number of data bytes. */
{
    /* Parse the initialization. Flexible array members can only be initialized
    ** in cc65 mode.
    */
    unsigned Size = ParseInitInternal (T, IS_Get (&Standard) == STD_CC65);

    /* The initialization may not generate code on global level, because code
    ** outside function scope will never get executed.
    */
    if (HaveGlobalCode ()) {
        Error ("Non constant initializers");
        RemoveGlobalCode ();
    }

    /* Return the size needed for the initialization */
    return Size;
}
