/*****************************************************************************/
/*                                                                           */
/*				   declare.c				     */
/*                                                                           */
/*		   Parse variable and function declarations		     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2003 Ullrich von Bassewitz                                       */
/*               Römerstrasse 52                                             */
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
#include "symtab.h"
#include "typeconv.h"



/*****************************************************************************/
/*				   Forwards				     */
/*****************************************************************************/



static void ParseTypeSpec (DeclSpec* D, int Default);
/* Parse a type specificier */

static unsigned ParseInitInternal (type* T, int AllowFlexibleMembers);
/* Parse initialization of variables. Return the number of data bytes. */



/*****************************************************************************/
/*			      internal functions			     */
/*****************************************************************************/



static type OptionalQualifiers (type Q)
/* Read type qualifiers if we have any */
{
    while (CurTok.Tok == TOK_CONST || CurTok.Tok == TOK_VOLATILE) {

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

	    default:
		/* Keep gcc silent */
		break;

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
    D->StorageClass   	= 0;
    D->Type[0]	    	= T_END;
    D->Flags 	    	= 0;
}



static void InitDeclaration (Declaration* D)
/* Initialize the Declaration struct for use */
{
    D->Ident[0]		= '\0';
    D->Type[0]	 	= T_END;
    D->T		= D->Type;
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
    	    ExprDesc lval;
   	    NextToken ();
   	    ConstExpr (&lval);
   	    EnumVal = lval.ConstVal;
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



static SymEntry* ParseStructDecl (const char* Name, type StructType)
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
	ParseTypeSpec (&Spec, -1);

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
                    Encode (Decl.Type + 1, FLEXIBLE);
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



static void ParseTypeSpec (DeclSpec* D, int Default)
/* Parse a type specificier */
{
    ident     	Ident;
    SymEntry* 	Entry;
    type 	StructType;
    type    	Qualifiers;	/* Type qualifiers */

    /* Assume we have an explicit type */
    D->Flags &= ~DS_DEF_TYPE;

    /* Read type qualifiers if we have any */
    Qualifiers = OptionalQualifiers (T_QUAL_NONE);

    /* Look at the data type */
    switch (CurTok.Tok) {

    	case TOK_VOID:
    	    NextToken ();
	    D->Type[0] = T_VOID;
	    D->Type[1] = T_END;
    	    break;

    	case TOK_CHAR:
    	    NextToken ();
	    D->Type[0] = GetDefaultChar();
	    D->Type[1] = T_END;
	    break;

    	case TOK_LONG:
    	    NextToken ();
    	    if (CurTok.Tok == TOK_UNSIGNED) {
    	      	NextToken ();
    	      	optionalint ();
	      	D->Type[0] = T_ULONG;
	      	D->Type[1] = T_END;
    	    } else {
    	      	optionalsigned ();
    	      	optionalint ();
	      	D->Type[0] = T_LONG;
	      	D->Type[1] = T_END;
    	    }
	    break;

    	case TOK_SHORT:
    	    NextToken ();
    	    if (CurTok.Tok == TOK_UNSIGNED) {
    	      	NextToken ();
    	      	optionalint ();
	      	D->Type[0] = T_USHORT;
	      	D->Type[1] = T_END;
    	    } else {
    	      	optionalsigned ();
    	      	optionalint ();
	      	D->Type[0] = T_SHORT;
	    	D->Type[1] = T_END;
    	    }
	    break;

    	case TOK_INT:
    	    NextToken ();
	    D->Type[0] = T_INT;
	    D->Type[1] = T_END;
    	    break;

       case TOK_SIGNED:
    	    NextToken ();
    	    switch (CurTok.Tok) {

       		case TOK_CHAR:
    		    NextToken ();
		    D->Type[0] = T_SCHAR;
		    D->Type[1] = T_END;
    		    break;

    		case TOK_SHORT:
    		    NextToken ();
    		    optionalint ();
		    D->Type[0] = T_SHORT;
		    D->Type[1] = T_END;
    		    break;

    		case TOK_LONG:
    		    NextToken ();
    	 	    optionalint ();
		    D->Type[0] = T_LONG;
		    D->Type[1] = T_END;
    	     	    break;

    		case TOK_INT:
    		    NextToken ();
    		    /* FALL THROUGH */

    	    	default:
	    	    D->Type[0] = T_INT;
		    D->Type[1] = T_END;
	  	    break;
    	    }
	    break;

    	case TOK_UNSIGNED:
    	    NextToken ();
    	    switch (CurTok.Tok) {

       	 	case TOK_CHAR:
    		    NextToken ();
		    D->Type[0] = T_UCHAR;
		    D->Type[1] = T_END;
    	     	    break;

    	    	case TOK_SHORT:
    		    NextToken ();
    		    optionalint ();
		    D->Type[0] = T_USHORT;
		    D->Type[1] = T_END;
    		    break;

    		case TOK_LONG:
    		    NextToken ();
    		    optionalint ();
		    D->Type[0] = T_ULONG;
		    D->Type[1] = T_END;
    		    break;

    	     	case TOK_INT:
    		    NextToken ();
    		    /* FALL THROUGH */

    		default:
		    D->Type[0] = T_UINT;
	     	    D->Type[1] = T_END;
		    break;
    	    }
	    break;

        case TOK_FLOAT:
    	    NextToken ();
	    D->Type[0] = T_FLOAT;
	    D->Type[1] = T_END;
    	    break;

        case TOK_DOUBLE:
    	    NextToken ();
	    D->Type[0] = T_DOUBLE;
	    D->Type[1] = T_END;
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
	    D->Type[0] = StructType;
	    EncodePtr (D->Type+1, Entry);
	    D->Type[DECODE_SIZE+1] = T_END;
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
	    D->Type[0] = T_INT;
	    D->Type[1] = T_END;
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
		D->Type[0] = T_INT;
		D->Type[1] = T_END;
    	    } else {
		D->Flags  |= DS_DEF_TYPE;
		D->Type[0] = (type) Default;
		D->Type[1] = T_END;
	    }
	    break;
    }

    /* There may also be qualifiers *after* the initial type */
    D->Type[0] |= OptionalQualifiers (Qualifiers);
}



static type* ParamTypeCvt (type* T)
/* If T is an array, convert it to a pointer else do nothing. Return the
 * resulting type.
 */
{
    if (IsTypeArray (T)) {
       	T += DECODE_SIZE;
	T[0] = T_PTR;
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
     	/* Print an error if in strict ANSI mode and we have unnamed
     	 * parameters.
     	 */
       	if (ANSI && (F->Flags & FD_UNNAMED_PARAMS) != 0) {
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
     	    /* Old style (K&R) function. Assume variable param list. */
     	    F->Flags |= (FD_OLDSTYLE | FD_VARIADIC);
     	}
    }

    /* Check for an implicit int return in the function */
    if ((Spec->Flags & DS_DEF_TYPE) != 0 &&
        Spec->Type[0] == T_INT 	         &&
        Spec->Type[1] == T_END) {
        /* Function has an implicit int return */
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

    /* Leave the lexical level remembering the symbol tables */
    RememberFunctionLevel (F);

    /* Return the function descriptor */
    return F;
}



static void Decl (const DeclSpec* Spec, Declaration* D, unsigned Mode)
/* Recursively process declarators. Build a type array in reverse order. */
{

    if (CurTok.Tok == TOK_STAR) {
    	type T = T_PTR;
       	NextToken ();
    	/* Allow optional const or volatile qualifiers */
    	T |= OptionalQualifiers (T_QUAL_NONE);
       	Decl (Spec, D, Mode);
       	*D->T++ = T;
       	return;
    } else if (CurTok.Tok == TOK_LPAREN) {
       	NextToken ();
       	Decl (Spec, D, Mode);
       	ConsumeRParen ();
    } else if (CurTok.Tok == TOK_FASTCALL) {
	/* Remember the current type pointer */
	type* T = D->T;
	/* Skip the fastcall token */
      	NextToken ();
	/* Parse the function */
	Decl (Spec, D, Mode);
	/* Set the fastcall flag */
	if (!IsTypeFunc (T) && !IsTypeFuncPtr (T)) {
	    Error ("__fastcall__ modifier applied to non function");
	} else if (IsVariadicFunc (T)) {
	    Error ("Cannot apply __fastcall__ to functions with variable parameter list");
	} else {
	    FuncDesc* F = GetFuncDesc (T);
       	    F->Flags |= FD_FASTCALL;
	}
	return;
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
	    *D->T++ = T_FUNC;
	    EncodePtr (D->T, F);
	    D->T += DECODE_SIZE;
       	} else {
	    /* Array declaration */
       	    long Size = UNSPECIFIED;
       	    NextToken ();
	    /* Read the size if it is given */
       	    if (CurTok.Tok != TOK_RBRACK) {
    	     	ExprDesc lval;
       	       	ConstExpr (&lval);
                if (lval.ConstVal <= 0) {
                    if (D->Ident[0] != '\0') {
                        Error ("Size of array `%s' is invalid", D->Ident);
                    } else {
                        Error ("Size of array is invalid");
                    }
                    lval.ConstVal = 1;
                }
       	       	Size = lval.ConstVal;
       	    }
       	    ConsumeRBrack ();
       	    *D->T++ = T_ARRAY;
       	    Encode (D->T, Size);
       	    D->T += DECODE_SIZE;
       	}
    }
}



/*****************************************************************************/
/*	       	      	       	     code	     			     */
/*****************************************************************************/



type* ParseType (type* Type)
/* Parse a complete type specification */
{
    DeclSpec Spec;
    Declaration Decl;

    /* Get a type without a default */
    InitDeclSpec (&Spec);
    ParseTypeSpec (&Spec, -1);

    /* Parse additional declarators */
    InitDeclaration (&Decl);
    ParseDecl (&Spec, &Decl, DM_NO_IDENT);

    /* Copy the type to the target buffer */
    TypeCpy (Type, Decl.Type);

    /* Return a pointer to the target buffer */
    return Type;
}



void ParseDecl (const DeclSpec* Spec, Declaration* D, unsigned Mode)
/* Parse a variable, type or function declaration */
{
    /* Initialize the Declaration struct */
    InitDeclaration (D);

    /* Get additional declarators and the identifier */
    Decl (Spec, D, Mode);

    /* Add the base type. */
    TypeCpy (D->T, Spec->Type);

    /* Check the size of the generated type */
    if (!IsTypeFunc (D->Type) && !IsTypeVoid (D->Type) && SizeOf (D->Type) >= 0x10000) {
    	if (D->Ident[0] != '\0') {
    	    Error ("Size of `%s' is invalid", D->Ident);
    	} else {
    	    Error ("Invalid size");
    	}
    }
}



void ParseDeclSpec (DeclSpec* D, unsigned DefStorage, int DefType)
/* Parse a declaration specification */
{
    /* Initialize the DeclSpec struct */
    InitDeclSpec (D);

    /* First, get the storage class specifier for this declaration */
    ParseStorageClass (D, DefStorage);

    /* Parse the type specifiers */
    ParseTypeSpec (D, DefType);
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



static unsigned ParseScalarInit (type* T)
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
    ConstExpr (&ED);
    TypeConversion (&ED, 0, T);

    /* Output the data */
    DefineData (&ED);

    /* Close eventually opening braces */
    ClosingCurlyBraces (BraceCount);

    /* Done */
    return SizeOf (T);
}



static unsigned ParsePointerInit (type* T)
/* Parse initializaton for pointer data types. Return the number of data bytes. */
{
    /* Optional opening brace */
    unsigned BraceCount = OpeningCurlyBraces (0);

    /* Expression */
    ExprDesc ED;
    ConstExpr (&ED);
    if ((ED.Flags & E_MCTYPE) == E_TCONST) {
        /* Make the const value the correct size */
        ED.ConstVal &= 0xFFFF;
    }
    TypeConversion (&ED, 0, T);

    /* Output the data */
    DefineData (&ED);

    /* Close eventually opening braces */
    ClosingCurlyBraces (BraceCount);

    /* Done */
    return SIZEOF_PTR;
}



static unsigned ParseArrayInit (type* T, int AllowFlexibleMembers)
/* Parse initializaton for arrays. Return the number of data bytes. */
{
    int Count;

    /* Get the array data */
    type* ElementType    = GetElementType (T);
    unsigned ElementSize = CheckedSizeOf (ElementType);
    long ElementCount    = GetElementCount (T);

    /* Special handling for a character array initialized by a literal */
    if (IsTypeChar (ElementType) && CurTok.Tok == TOK_SCONST) {

        /* Char array initialized by string constant */
        const char* Str = GetLiteral (CurTok.IVal);
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
        Encode (T + 1, Count);
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



static unsigned ParseStructInit (type* Type, int AllowFlexibleMembers)
/* Parse initialization of a struct or union. Return the number of data bytes. */
{
    SymEntry* Entry;
    SymTable* Tab;
    unsigned  StructSize;
    unsigned  Size;


    /* Consume the opening curly brace */
    ConsumeLCurly ();

    /* Get a pointer to the struct entry from the type */
    Entry = DecodePtr (Type + 1);

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
    ExprDesc lval;
    unsigned Size;

    /* Opening brace */
    ConsumeLCurly ();

    /* Allow an arbitrary list of values */
    Size = 0;
    do {
	ConstExpr (&lval);
	switch (UnqualifiedType (lval.Type[0])) {

	    case T_SCHAR:
	    case T_UCHAR:
		if ((lval.Flags & E_MCTYPE) == E_TCONST) {
		    /* Make it byte sized */
		    lval.ConstVal &= 0xFF;
		}
		DefineData (&lval);
                Size += SIZEOF_CHAR;
                break;

	    case T_SHORT:
	    case T_USHORT:
	    case T_INT:
	    case T_UINT:
	    case T_PTR:
	    case T_ARRAY:
	   	if ((lval.Flags & E_MCTYPE) == E_TCONST) {
    	      	    /* Make it word sized */
	   	    lval.ConstVal &= 0xFFFF;
	   	}
	   	DefineData (&lval);
	   	Size += SIZEOF_INT;
                break;

	    case T_LONG:
	    case T_ULONG:
	   	DefineData (&lval);
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



static unsigned ParseInitInternal (type* T, int AllowFlexibleMembers)
/* Parse initialization of variables. Return the number of data bytes. */
{
    switch (UnqualifiedType (*T)) {

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
	    if (!ANSI) {
	    	/* Special cc65 extension in non ANSI mode */
	      	return ParseVoidInit ();
	    }
	    /* FALLTHROUGH */

     	default:
	    Error ("Illegal type");
	    return SIZEOF_CHAR;

    }
}



unsigned ParseInit (type* T)
/* Parse initialization of variables. Return the number of data bytes. */
{
    /* Parse the initialization */
    unsigned Size = ParseInitInternal (T, !ANSI);

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



