/*****************************************************************************/
/*                                                                           */
/*				   declare.c				     */
/*                                                                           */
/*		   Parse variable and function declarations		     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2005 Ullrich von Bassewitz                                       */
/*               Römerstraße 52                                              */
/*               D-70794 Filderstadt                                         */
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
/*				   Forwards				     */
/*****************************************************************************/



static void ParseTypeSpec (DeclSpec* D, long Default, TypeCode Qualifiers);
/* Parse a type specificier */

static unsigned ParseInitInternal (Type* T, int AllowFlexibleMembers);
/* Parse initialization of variables. Return the number of data bytes. */



/*****************************************************************************/
/*			      internal functions			     */
/*****************************************************************************/



static TypeCode OptionalQualifiers (TypeCode Q)
/* Read type qualifiers if we have any */
{
    while (TokIsTypeQual (&CurTok)) {

	switch (CurTok.Tok) {

	    case TOK_CONST:
	    	if (Q & T_QUAL_CONST) {
		    Error ("Duplicate qualifier: `const'");
		}
		Q |= T_QUAL_CONST;
		break;

	    case TOK_VOLATILE:
		if (Q & T_QUAL_VOLATILE) {
		    Error ("Duplicate qualifier: `volatile'");
		}
		Q |= T_QUAL_VOLATILE;
		break;

            case TOK_RESTRICT:
                if (Q & T_QUAL_RESTRICT) {
                    Error ("Duplicate qualifier: `restrict'");
                }
                Q |= T_QUAL_RESTRICT;
                break;

	    default:
		Internal ("Unexpected type qualifier token: %d", CurTok.Tok);

	}

	/* Skip the token */
	NextToken ();
    }

    /* Return the qualifiers read */
    return Q;
}



static void optionalint (void)
/* Eat an optional "int" token */
{
    if (CurTok.Tok == TOK_INT) {
	/* Skip it */
 	NextToken ();
    }
}



static void optionalsigned (void)
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
    D->Ident[0]  = '\0';
    D->Type[0].C = T_END;
    D->Index     = 0;
}



static void NeedTypeSpace (Declaration* D, unsigned Count)
/* Check if there is enough space for Count type specifiers within D */
{
    if (D->Index + Count >= MAXTYPELEN) {
   	/* We must call Fatal() here, since calling Error() will try to
   	 * continue, and the declaration type is not correctly terminated
   	 * in case we come here.
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



static void AddFuncTypeToDeclaration (Declaration* D, FuncDesc* F)
/* Add a function type plus function descriptor to the type of a declaration */
{
    NeedTypeSpace (D, 1);
    D->Type[D->Index].C = T_FUNC;
    SetFuncDesc (D->Type + D->Index, F);
    ++D->Index;
}



static void AddArrayToDeclaration (Declaration* D, long Size)
/* Add an array type plus size to the type of a declaration */
{
    NeedTypeSpace (D, 1);
    D->Type[D->Index].C = T_ARRAY;
    D->Type[D->Index].A.L = Size;
    ++D->Index;
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



static SymEntry* ParseStructDecl (const char* Name, TypeCode StructType)
/* Parse a struct/union declaration. */
{

    unsigned  StructSize;
    unsigned  FieldSize;
    unsigned  Offs;
    int       FlexibleMember;
    SymTable* FieldTab;
    SymEntry* Entry;


    if (CurTok.Tok != TOK_LCURLY) {
    	/* Just a forward declaration. Try to find a struct with the given
	 * name. If there is none, insert a forward declaration into the
	 * current lexical level.
	 */
	Entry = FindTagSym (Name);
       	if (Entry == 0) {
	    Entry = AddStructSym (Name, 0, 0);
	} else if (SymIsLocal (Entry) && (Entry->Flags & SC_STRUCT) == 0) {
	    /* Already defined in the level but no struct */
	    Error ("Symbol `%s' is already different kind", Name);
	}
    	return Entry;
    }

    /* Add a forward declaration for the struct in the current lexical level */
    Entry = AddStructSym (Name, 0, 0);

    /* Skip the curly brace */
    NextToken ();

    /* Enter a new lexical level for the struct */
    EnterStructLevel ();

    /* Parse struct fields */
    FlexibleMember = 0;
    StructSize     = 0;
    while (CurTok.Tok != TOK_RCURLY) {

	/* Get the type of the entry */
	DeclSpec Spec;
	InitDeclSpec (&Spec);
	ParseTypeSpec (&Spec, -1, T_QUAL_NONE);

	/* Read fields with this type */
	while (1) {

	    Declaration Decl;

            /* If we had a flexible array member before, no other fields can
             * follow.
             */
            if (FlexibleMember) {
                Error ("Flexible array member must be last field");
                FlexibleMember = 0;     /* Avoid further errors */
            }

	    /* Get type and name of the struct field */
	    ParseDecl (&Spec, &Decl, 0);

            /* Get the offset of this field */
            Offs = (StructType == T_STRUCT)? StructSize : 0;

            /* Calculate the sizes, handle flexible array members */
            if (StructType == T_STRUCT) {

                /* It's a struct. Check if this field is a flexible array
                 * member, and calculate the size of the field.
                 */
                if (IsTypeArray (Decl.Type) && GetElementCount (Decl.Type) == UNSPECIFIED) {
                    /* Array with unspecified size */
                    if (StructSize == 0) {
                        Error ("Flexible array member cannot be first struct field");
                    }
                    FlexibleMember = 1;
                    /* Assume zero for size calculations */
                    SetElementCount (Decl.Type, FLEXIBLE);
                } else {
                    StructSize += CheckedSizeOf (Decl.Type);
                }

	    } else {

                /* It's a union */
                FieldSize = CheckedSizeOf (Decl.Type);
	       	if (FieldSize > StructSize) {
	       	    StructSize = FieldSize;
	       	}
    	    }

	    /* Add a field entry to the table */
	    AddLocalSym (Decl.Ident, Decl.Type, SC_STRUCTFIELD, Offs);

	    if (CurTok.Tok != TOK_COMMA) {
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
    return AddStructSym (Name, StructSize, FieldTab);
}



static void ParseTypeSpec (DeclSpec* D, long Default, TypeCode Qualifiers)
/* Parse a type specificier */
{
    ident     	Ident;
    SymEntry* 	Entry;
    TypeCode    StructType;

    /* Assume we have an explicit type */
    D->Flags &= ~DS_DEF_TYPE;

    /* Read type qualifiers if we have any */
    Qualifiers = OptionalQualifiers (Qualifiers);

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
    	      	optionalint ();
    	      	D->Type[0].C = T_ULONG;
	      	D->Type[1].C = T_END;
    	    } else {
    	      	optionalsigned ();
    	      	optionalint ();
	      	D->Type[0].C = T_LONG;
	      	D->Type[1].C = T_END;
    	    }
	    break;

    	case TOK_SHORT:
    	    NextToken ();
    	    if (CurTok.Tok == TOK_UNSIGNED) {
    	      	NextToken ();
    	      	optionalint ();
	      	D->Type[0].C = T_USHORT;
	      	D->Type[1].C = T_END;
    	    } else {
    	      	optionalsigned ();
    	      	optionalint ();
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
    		    optionalint ();
		    D->Type[0].C = T_SHORT;
		    D->Type[1].C = T_END;
    		    break;

    		case TOK_LONG:
    		    NextToken ();
    	 	    optionalint ();
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
    		    optionalint ();
		    D->Type[0].C = T_USHORT;
		    D->Type[1].C = T_END;
    		    break;

    		case TOK_LONG:
    		    NextToken ();
    		    optionalint ();
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

    	case TOK_STRUCT:
    	case TOK_UNION:
    	    StructType = (CurTok.Tok == TOK_STRUCT)? T_STRUCT : T_UNION;
    	    NextToken ();
	    /* */
    	    if (CurTok.Tok == TOK_IDENT) {
	 	strcpy (Ident, CurTok.Ident);
    	 	NextToken ();
    	    } else {
	 	AnonName (Ident, (StructType == T_STRUCT)? "struct" : "union");
    	    }
	    /* Remember we have an extra type decl */
	    D->Flags |= DS_EXTRA_TYPE;
	    /* Declare the struct in the current scope */
    	    Entry = ParseStructDecl (Ident, StructType);
       	    /* Encode the struct entry into the type */
	    D->Type[0].C = StructType;
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
    		TypeCpy (D->Type, Entry->Type);
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
    D->Type[0].C |= OptionalQualifiers (Qualifiers);
}



static Type* ParamTypeCvt (Type* T)
/* If T is an array, convert it to a pointer else do nothing. Return the
 * resulting type.
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
    /* Parse params */
    while (CurTok.Tok != TOK_RPAREN) {

	/* List of identifiers expected */
	if (CurTok.Tok != TOK_IDENT) {
	    Error ("Identifier expected");
	}

	/* Create a symbol table entry with type int */
	AddLocalSym (CurTok.Ident, type_int, SC_AUTO | SC_PARAM | SC_DEF, 0);

	/* Count arguments */
       	++F->ParamCount;

	/* Skip the identifier */
	NextToken ();

	/* Check for more parameters */
	if (CurTok.Tok == TOK_COMMA) {
	    NextToken ();
	} else {
	    break;
	}
    }

    /* Skip right paren. We must explicitly check for one here, since some of
     * the breaks above bail out without checking.
     */
    ConsumeRParen ();

    /* An optional list of type specifications follows */
    while (CurTok.Tok != TOK_LCURLY) {

	DeclSpec    	Spec;

	/* Read the declaration specifier */
	ParseDeclSpec (&Spec, SC_AUTO, T_INT);

       	/* We accept only auto and register as storage class specifiers, but
	 * we ignore all this, since we use auto anyway.
	 */
	if ((Spec.StorageClass & SC_AUTO) == 0 &&
    	    (Spec.StorageClass & SC_REGISTER) == 0) {
	    Error ("Illegal storage class");
	}

	/* Parse a comma separated variable list */
	while (1) {

	    Declaration 	Decl;

	    /* Read the parameter */
	    ParseDecl (&Spec, &Decl, DM_NEED_IDENT);
	    if (Decl.Ident[0] != '\0') {

		/* We have a name given. Search for the symbol */
		SymEntry* Sym = FindLocalSym (Decl.Ident);
		if (Sym) {
		    /* Found it, change the default type to the one given */
		    ChangeSymType (Sym, ParamTypeCvt (Decl.Type));
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

	DeclSpec 	Spec;
	Declaration 	Decl;
	DeclAttr	Attr;

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
      	 * eventually print an error message later.
	 */
	ParseDecl (&Spec, &Decl, DM_ACCEPT_IDENT);
       	if (Decl.Ident[0] == '\0') {

	    /* Unnamed symbol. Generate a name that is not user accessible,
	     * then handle the symbol normal.
	     */
    	    AnonName (Decl.Ident, "param");
    	    F->Flags |= FD_UNNAMED_PARAMS;

    	    /* Clear defined bit on nonames */
    	    Spec.StorageClass &= ~SC_DEF;
    	}

	/* Parse an attribute ### */
	ParseAttribute (&Decl, &Attr);

	/* Create a symbol table entry */
	AddLocalSym (Decl.Ident, ParamTypeCvt (Decl.Type), Spec.StorageClass, 0);

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
     * the breaks above bail out without checking.
     */
    ConsumeRParen ();

    /* Check if this is a function definition */
    if (CurTok.Tok == TOK_LCURLY) {
     	/* Print an error if we have unnamed parameters and cc65 extensions
         * are disabled.
     	 */
       	if (IS_Get (&Standard) != STD_CC65 &&
            (F->Flags & FD_UNNAMED_PARAMS) != 0) {
     	    Error ("Parameter name omitted");
     	}
    }
}



static FuncDesc* ParseFuncDecl (const DeclSpec* Spec)
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
     	 * if it's some other identifier, it's an old style parameter list.
     	 */
     	Sym = FindSym (CurTok.Ident);
     	if (Sym == 0 || !SymIsTypeDef (Sym)) {
     	    /* Old style (K&R) function. */
     	    F->Flags |= FD_OLDSTYLE;
     	}
    }

    /* Check for an implicit int return in the function */
    if ((Spec->Flags & DS_DEF_TYPE) != 0 &&
        Spec->Type[0].C == T_INT         &&
        Spec->Type[1].C == T_END) {
        /* Function has an implicit int return. Output a warning if we don't
         * have the C89 standard enabled explicitly.
         */
        if (IS_Get (&Standard) >= STD_C99) {
            Warning ("Implicit `int' return type is an obsolete feature");
        }
        F->Flags |= FD_OLDSTYLE_INTRET;
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
     * purposes, for example when passing stuff to fastcall functions. Since
     * more symbols are added to the table, it is easier if we remember it
     * now, since it is currently the last entry in the symbol table.
     */
    F->LastParam = GetSymTab()->SymTail;

    /* Assign offsets. If the function has a variable parameter list,
     * there's one additional byte (the arg size).
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

    /* Add the default address size for the function */
    if (CodeAddrSize == ADDR_SIZE_FAR) {
        F->Flags |= FD_FAR;
    } else {
        F->Flags |= FD_NEAR;
    }

    /* Leave the lexical level remembering the symbol tables */
    RememberFunctionLevel (F);

    /* Return the function descriptor */
    return F;
}



static unsigned FunctionModifierFlags (void)
/* Parse __fastcall__, __near__ and __far__ and return the matching FD_ flags */
{
    /* Read the flags */
    unsigned Flags = FD_NONE;
    while (CurTok.Tok == TOK_FASTCALL || CurTok.Tok == TOK_NEAR || CurTok.Tok == TOK_FAR) {

        /* Get the flag bit for the next token */
        unsigned F = FD_NONE;
        switch (CurTok.Tok) {
            case TOK_FASTCALL:  F = FD_FASTCALL; break;
            case TOK_NEAR:     	F = FD_NEAR;     break;
            case TOK_FAR:	F = FD_FAR;	 break;
            default:            Internal ("Unexpected token: %d", CurTok.Tok);
        }

        /* Remember the flag for this modifier */
        if (Flags & F) {
            Error ("Duplicate modifier");
        }
        Flags |= F;

        /* Skip the token */
        NextToken ();
    }

    /* Sanity check */
    if ((Flags & (FD_NEAR | FD_FAR)) == (FD_NEAR | FD_FAR)) {
        Error ("Cannot specify both, `__near__' and `__far__' modifiers");
        Flags &= ~(FD_NEAR | FD_FAR);
    }

    /* Return the flags read */
    return Flags;
}



static void ApplyFunctionModifiers (Type* T, unsigned Flags)
/* Apply a set of function modifier flags to a function */
{
    /* Get the function descriptor */
    FuncDesc* F = GetFuncDesc (T);

    /* Special check for __fastcall__ */
    if ((Flags & FD_FASTCALL) != 0 && IsVariadicFunc (T)) {
        Error ("Cannot apply `__fastcall__' to functions with "
               "variable parameter list");
        Flags &= ~FD_FASTCALL;
    }

    /* Remove the default function address size modifiers */
    F->Flags &= ~(FD_NEAR | FD_FAR);

    /* Add the new modifers */
    F->Flags |= Flags;
}



static void Decl (const DeclSpec* Spec, Declaration* D, unsigned Mode)
/* Recursively process declarators. Build a type array in reverse order. */
{
    /* Pointer to something */
    if (CurTok.Tok == TOK_STAR) {

    	TypeCode C;

        /* Skip the star */
       	NextToken ();

    	/* Allow optional const or volatile qualifiers */
       	C = T_PTR | OptionalQualifiers (T_QUAL_NONE);

        /* Parse the type, the pointer points to */
       	Decl (Spec, D, Mode);

	/* Add the type */
	AddTypeToDeclaration (D, C);
       	return;
    }

    /* Function modifiers */
    if (CurTok.Tok == TOK_FASTCALL || CurTok.Tok == TOK_NEAR || CurTok.Tok == TOK_FAR) {

	/* Remember the current type pointer */
    	Type* T = D->Type + D->Index;

	/* Read the flags */
	unsigned Flags = FunctionModifierFlags ();

	/* Parse the function */
	Decl (Spec, D, Mode);

	/* Check that we have a function */
	if (!IsTypeFunc (T) && !IsTypeFuncPtr (T)) {
	    Error ("Function modifier applied to non function");
	} else {
            ApplyFunctionModifiers (T, Flags);
        }

	/* Done */
	return;
    }

    if (CurTok.Tok == TOK_LPAREN) {
       	NextToken ();
       	Decl (Spec, D, Mode);
       	ConsumeRParen ();
    } else {
     	/* Things depend on Mode now:
       	 *  - Mode == DM_NEED_IDENT means:
     	 *   	we *must* have a type and a variable identifer.
     	 *  - Mode == DM_NO_IDENT means:
     	 *   	we must have a type but no variable identifer
     	 *     	(if there is one, it's not read).
     	 *  - Mode == DM_ACCEPT_IDENT means:
     	 *   	we *may* have an identifier. If there is an identifier,
     	 *   	it is read, but it is no error, if there is none.
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
       	    NextToken ();

	    /* Parse the function declaration */
       	    F = ParseFuncDecl (Spec);

	    /* Add the function type. Be sure to bounds check the type buffer */
	    AddFuncTypeToDeclaration (D, F);
       	} else {
	    /* Array declaration */
       	    long Size = UNSPECIFIED;
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
       	    ConsumeRBrack ();

	    /* Add the array type with the size */
	    AddArrayToDeclaration (D, Size);
       	}
    }
}



/*****************************************************************************/
/*	       	      	       	     code	     			     */
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
    TypeCpy (T, Decl.Type);

    /* Return a pointer to the target buffer */
    return T;
}



void ParseDecl (const DeclSpec* Spec, Declaration* D, unsigned Mode)
/* Parse a variable, type or function declaration */
{
    /* Initialize the Declaration struct */
    InitDeclaration (D);

    /* Get additional declarators and the identifier */
    Decl (Spec, D, Mode);

    /* Add the base type. */
    NeedTypeSpace (D, TypeLen (Spec->Type) + 1);	/* Bounds check */
    TypeCpy (D->Type + D->Index, Spec->Type);

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
    Qualifiers = OptionalQualifiers (T_QUAL_NONE);

    /* Now get the storage class specifier for this declaration */
    ParseStorageClass (D, DefStorage);

    /* Parse the type specifiers passing any initial type qualifiers */
    ParseTypeSpec (D, DefType, Qualifiers);
}



void CheckEmptyDecl (const DeclSpec* D)
/* Called after an empty type declaration (that is, a type declaration without
 * a variable). Checks if the declaration does really make sense and issues a
 * warning if not.
 */
{
    if ((D->Flags & DS_EXTRA_TYPE) == 0) {
   	Warning ("Useless declaration");
    }
}



static void SkipInitializer (unsigned BracesExpected)
/* Skip the remainder of an initializer in case of errors. Try to be somewhat
 * smart so we don't have too many following errors.
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
 * them and return the number. If the number of curly braces is less than
 * BracesNeeded, issue a warning.
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
 * an optional comma. Output an error messages, if the input does not contain
 * the expected number of braces.
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
       	     * allowed.
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



static unsigned ParseScalarInit (Type* T)
/* Parse initializaton for scalar data types. Return the number of data bytes. */
{
    ExprDesc ED;

    /* Optional opening brace */
    unsigned BraceCount = OpeningCurlyBraces (0);

    /* We warn if an initializer for a scalar contains braces, because this is
     * quite unusual and often a sign for some problem in the input.
     */
    if (BraceCount > 0) {
        Warning ("Braces around scalar initializer");
    }

    /* Get the expression and convert it to the target type */
    ConstExpr (hie1, &ED);
    TypeConversion (&ED, T);

    /* Output the data */
    DefineData (&ED);

    /* Close eventually opening braces */
    ClosingCurlyBraces (BraceCount);

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
        (CurTok.Tok == TOK_SCONST ||
        (CurTok.Tok == TOK_LCURLY && NextTok.Tok == TOK_SCONST))) {

        /* Char array initialized by string constant */
        int NeedParen;
        const char* Str;

        /* If we initializer is enclosed in brackets, remember this fact and
         * skip the opening bracket.
         */
        NeedParen = (CurTok.Tok == TOK_LCURLY);
        if (NeedParen) {
            NextToken ();
        }

        /* Get the initializer string and its size */
        Str = GetLiteral (CurTok.IVal);
        Count = GetLiteralPoolOffs () - CurTok.IVal;

        /* Translate into target charset */
        TranslateLiteralPool (CurTok.IVal);

        /* If the array is one too small for the string literal, omit the
         * trailing zero.
         */
        if (ElementCount != UNSPECIFIED &&
            ElementCount != FLEXIBLE    &&
            Count        == ElementCount + 1) {
            /* Omit the trailing zero */
            --Count;
        }

        /* Output the data */
        g_defbytes (Str, Count);

        /* Remove string from pool */
        ResetLiteralPoolOffs (CurTok.IVal);
        NextToken ();

        /* If the initializer was enclosed in curly braces, we need a closing
         * one.
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
             * an array (because the size of each element may differ
             * otherwise).
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
         * members.
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
    SymEntry* Entry;
    SymTable* Tab;
    unsigned  StructSize;
    unsigned  Size;


    /* Consume the opening curly brace */
    ConsumeLCurly ();

    /* Get a pointer to the struct entry from the type */
    Entry = GetSymEntry (T);

    /* Get the size of the struct from the symbol table entry */
    StructSize = Entry->V.S.Size;

    /* Check if this struct definition has a field table. If it doesn't, it
     * is an incomplete definition.
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
    Size = 0;
    while (CurTok.Tok != TOK_RCURLY) {
    	if (Entry == 0) {
    	    Error ("Too many initializers");
            SkipInitializer (1);
    	    return Size;
    	}
        /* Parse initialization of one field. Flexible array members may
         * only be initialized if they are the last field (or part of the
         * last struct field).
         */
    	Size += ParseInitInternal (Entry->Type, AllowFlexibleMembers && Entry->NextSym == 0);
    	Entry = Entry->NextSym;
    	if (CurTok.Tok != TOK_COMMA)
    	    break;
    	NextToken ();
    }

    /* Consume the closing curly brace */
    ConsumeRCurly ();

    /* If there are struct fields left, reserve additional storage */
    if (Size < StructSize) {
    	g_zerobytes (StructSize - Size);
        Size = StructSize;
    }

    /* Return the actual number of bytes initialized. This number may be
     * larger than StructSize if flexible array members are present and were
     * initialized (possible in non ANSI mode).
     */
    return Size;
}



static unsigned ParseVoidInit (void)
/* Parse an initialization of a void variable (special cc65 extension).
 * Return the number of bytes initialized.
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
     * in cc65 mode.
     */
    unsigned Size = ParseInitInternal (T, IS_Get (&Standard) == STD_CC65);

    /* The initialization may not generate code on global level, because code
     * outside function scope will never get executed.
     */
    if (HaveGlobalCode ()) {
        Error ("Non constant initializers");
        RemoveGlobalCode ();
    }

    /* Return the size needed for the initialization */
    return Size;
}



