/*
 * declare.c
 *
 * Ullrich von Bassewitz, 20.06.1998
 */



#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

/* common */
#include "xmalloc.h"

/* cc65 */
#include "anonname.h"
#include "codegen.h"
#include "datatype.h"
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
#include "declare.h"



/*****************************************************************************/
/*				   Forwards				     */
/*****************************************************************************/



static void ParseTypeSpec (DeclSpec* D, int Default);
/* Parse a type specificier */



/*****************************************************************************/
/*			      internal functions			     */
/*****************************************************************************/



static type OptionalQualifiers (type Q)
/* Read type qualifiers if we have any */
{
    while (curtok == TOK_CONST || curtok == TOK_VOLATILE) {

	switch (curtok) {

	    case TOK_CONST:
		if (Q & T_QUAL_CONST) {
		    Error (ERR_DUPLICATE_QUALIFIER, "const");
		}
		Q |= T_QUAL_CONST;
		break;

	    case TOK_VOLATILE:
		if (Q & T_QUAL_VOLATILE) {
		    Error (ERR_DUPLICATE_QUALIFIER, "volatile");
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
    if (curtok == TOK_INT) {
	/* Skip it */
 	NextToken ();
    }
}



static void optionalsigned (void)
/* Eat an optional "signed" token */
{
    if (curtok == TOK_SIGNED) {
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
    switch (curtok) {

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
    if (curtok != TOK_LCURLY) {
   	return;
    }

    /* Skip the opening curly brace */
    NextToken ();

    /* Read the enum tags */
    EnumVal = 0;
    while (curtok != TOK_RCURLY) {

	/* We expect an identifier */
   	if (curtok != TOK_IDENT) {
   	    Error (ERR_IDENT_EXPECTED);
   	    continue;
   	}

	/* Remember the identifier and skip it */
	strcpy (Ident, CurTok.Ident);
   	NextToken ();

	/* Check for an assigned value */
   	if (curtok == TOK_ASSIGN) {
    	    struct expent lval;
   	    NextToken ();
   	    constexpr (&lval);
   	    EnumVal = lval.e_const;
    	}

	/* Add an entry to the symbol table */
	AddEnumSym (Ident, EnumVal++);

	/* Check for end of definition */
    	if (curtok != TOK_COMMA)
    	    break;
    	NextToken ();
    }
    ConsumeRCurly ();
}



static SymEntry* ParseStructDecl (const char* Name, type StructType)
/* Parse a struct/union declaration. */
{

    unsigned Size;
    unsigned Offs;
    SymTable* FieldTab;
    SymEntry* Entry;


    if (curtok != TOK_LCURLY) {
    	/* Just a forward declaration. Try to find a struct with the given
	 * name. If there is none, insert a forward declaration into the
	 * current lexical level.
	 */
	Entry = FindTagSym (Name);
       	if (Entry == 0) {
	    Entry = AddStructSym (Name, 0, 0);
	} else if (SymIsLocal (Entry) && (Entry->Flags & SC_STRUCT) == 0) {
	    /* Already defined in the level but no struct */
	    Error (ERR_SYMBOL_KIND);
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
    Size = 0;
    while (curtok != TOK_RCURLY) {

	/* Get the type of the entry */
	DeclSpec Spec;
	InitDeclSpec (&Spec);
	ParseTypeSpec (&Spec, -1);

	/* Read fields with this type */
	while (1) {

	    /* Get type and name of the struct field */
	    Declaration Decl;
	    ParseDecl (&Spec, &Decl, 0);

	    /* Add a field entry to the table */
	    AddLocalSym (Decl.Ident, Decl.Type, SC_SFLD, (StructType == T_STRUCT)? Size : 0);

	    /* Calculate offset of next field/size of the union */
    	    Offs = SizeOf (Decl.Type);
	    if (StructType == T_STRUCT) {
	       	Size += Offs;
	    } else {
	       	if (Offs > Size) {
	       	    Size = Offs;
	       	}
	    }

	    if (curtok != TOK_COMMA)
	       	break;
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
    return AddStructSym (Name, Size, FieldTab);
}



static void ParseTypeSpec (DeclSpec* D, int Default)
/* Parse a type specificier */
{
    ident     	Ident;
    SymEntry* 	Entry;
    type 	StructType;
    type	Qualifiers;	/* Type qualifiers */

    /* Assume we have an explicit type */
    D->Flags &= ~DS_DEF_TYPE;

    /* Read type qualifiers if we have any */
    Qualifiers = OptionalQualifiers (T_QUAL_NONE);

    /* Look at the data type */
    switch (curtok) {

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
    	    if (curtok == TOK_UNSIGNED) {
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
    	    if (curtok == TOK_UNSIGNED) {
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
    	    switch (curtok) {

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
    	    switch (curtok) {

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

    	case TOK_STRUCT:
    	case TOK_UNION:
    	    StructType = (curtok == TOK_STRUCT)? T_STRUCT : T_UNION;
    	    NextToken ();
	    /* */
    	    if (curtok == TOK_IDENT) {
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
	    if (curtok != TOK_LCURLY) {
	     	/* Named enum */
		if (curtok == TOK_IDENT) {
		    /* Find an entry with this name */
		    Entry = FindTagSym (CurTok.Ident);
		    if (Entry) {
			if (SymIsLocal (Entry) && (Entry->Flags & SC_ENUM) == 0) {
		    	    Error (ERR_SYMBOL_KIND);
			}
		    } else {
			/* Insert entry into table ### */
		    }
		    /* Skip the identifier */
		    NextToken ();
		} else {
    	            Error (ERR_IDENT_EXPECTED);
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
	    if (Entry && IsTypeDef (Entry)) {
       	       	/* It's a typedef */
    	      	NextToken ();
		TypeCpy (D->Type, Entry->Type);
    	      	break;
    	    }
    	    /* FALL THROUGH */

    	default:
    	    if (Default < 0) {
    		Error (ERR_TYPE_EXPECTED);
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
    while (curtok != TOK_RPAREN) {

	/* List of identifiers expected */
	if (curtok != TOK_IDENT) {
	    Error (ERR_IDENT_EXPECTED);
	}

	/* Create a symbol table entry with type int */
	AddLocalSym (CurTok.Ident, type_int, SC_AUTO | SC_PARAM | SC_DEF, 0);

	/* Count arguments */
       	++F->ParamCount;

	/* Skip the identifier */
	NextToken ();

	/* Check for more parameters */
	if (curtok == TOK_COMMA) {
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
    while (curtok != TOK_LCURLY) {

	DeclSpec 	Spec;

	/* Read the declaration specifier */
	ParseDeclSpec (&Spec, SC_AUTO, T_INT);

       	/* We accept only auto and register as storage class specifiers, but
	 * we ignore all this, since we use auto anyway.
	 */
	if ((Spec.StorageClass & SC_AUTO) == 0 &&
	    (Spec.StorageClass & SC_REGISTER) == 0) {
	    Error (ERR_ILLEGAL_STORAGE_CLASS);
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
		    Error (ERR_UNKNOWN_IDENT, Decl.Ident);
		}
	    }

     	    if (curtok == TOK_COMMA) {
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
    while (curtok != TOK_RPAREN) {

	DeclSpec 	Spec;
	Declaration 	Decl;
	DeclAttr	Attr;

      	/* Allow an ellipsis as last parameter */
	if (curtok == TOK_ELLIPSIS) {
	    NextToken ();
	    F->Flags |= FD_ELLIPSIS;
	    break;
	}

	/* Read the declaration specifier */
	ParseDeclSpec (&Spec, SC_AUTO, T_INT);

       	/* We accept only auto and register as storage class specifiers, but
	 * we ignore all this and use auto.
	 */
	if ((Spec.StorageClass & SC_AUTO) == 0 &&
	    (Spec.StorageClass & SC_REGISTER) == 0) {
	    Error (ERR_ILLEGAL_STORAGE_CLASS);
	}
	Spec.StorageClass = SC_AUTO | SC_PARAM | SC_DEF;

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
	if (curtok == TOK_COMMA) {
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
    if (curtok == TOK_LCURLY) {
     	/* Print an error if in strict ANSI mode and we have unnamed
     	 * parameters.
     	 */
       	if (ANSI && (F->Flags & FD_UNNAMED_PARAMS) != 0) {
     	    Error (ERR_MISSING_PARAM_NAME);
     	}
    }
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
    if (curtok == TOK_RPAREN) {
     	/* Parameter list is empty */
     	F->Flags |= (FD_EMPTY | FD_ELLIPSIS);
    } else if (curtok == TOK_VOID && nxttok == TOK_RPAREN) {
     	/* Parameter list declared as void */
     	NextToken ();
     	F->Flags |= FD_VOID_PARAM;
    } else if (curtok == TOK_IDENT && (nxttok == TOK_COMMA || nxttok == TOK_RPAREN)) {
	/* Old style (K&R) function. Assume variable param list. */
       	F->Flags |= (FD_OLDSTYLE | FD_ELLIPSIS);
    }

    /* Parse params */
    if ((F->Flags & FD_OLDSTYLE) == 0) {
	/* New style function */
     	ParseAnsiParamList (F);
    } else {
	/* Old style function */
	ParseOldStyleParamList (F);
    }

    /* Assign offsets. If the function has a variable parameter list,
     * there's one additional byte (the arg size).
     */
    Offs = (F->Flags & FD_ELLIPSIS)? 1 : 0;
    Sym = GetSymTab()->SymTail;
    while (Sym) {
	unsigned Size = SizeOf (Sym->Type);
     	Sym->V.Offs = Offs;
       	Offs += Size;
	F->ParamSize += Size;
     	Sym = Sym->PrevSym;
    }

    /* Leave the lexical level remembering the symbol tables */
    RememberFunctionLevel (F);

    /* Return the function descriptor */
    return F;
}



static void Decl (Declaration* D, unsigned Mode)
/* Recursively process declarators. Build a type array in reverse order. */
{

    if (curtok == TOK_STAR) {
	type T = T_PTR;
       	NextToken ();
	/* Allow optional const or volatile qualifiers */
	T |= OptionalQualifiers (T_QUAL_NONE);
       	Decl (D, Mode);
       	*D->T++ = T;
       	return;
    } else if (curtok == TOK_LPAREN) {
       	NextToken ();
       	Decl (D, Mode);
       	ConsumeRParen ();
    } else if (curtok == TOK_FASTCALL) {
	/* Remember the current type pointer */
	type* T = D->T;
	/* Skip the fastcall token */
      	NextToken ();
	/* Parse the function */
	Decl (D, Mode);
	/* Set the fastcall flag */
	if (!IsTypeFunc (T)) {
	    Error (ERR_ILLEGAL_MODIFIER);
	} else {
	    FuncDesc* F = DecodePtr (T+1);
       	    F->Flags |= FD_FASTCALL;
	}
	return;
    } else {
     	/* Things depend on Mode now:
       	 *  - Mode == DM_NEED_IDENT means:
     	 *   	we *must* have a type and a variable identifer.
     	 *  - Mode == DM_NO_IDENT means:
     	 *	we must have a type but no variable identifer
     	 *     	(if there is one, it's not read).
     	 *  - Mode == DM_ACCEPT_IDENT means:
     	 *	we *may* have an identifier. If there is an identifier,
     	 *	it is read, but it is no error, if there is none.
     	 */
     	if (Mode == DM_NO_IDENT) {
     	    D->Ident[0] = '\0';
     	} else if (curtok == TOK_IDENT) {
       	    strcpy (D->Ident, CurTok.Ident);
     	    NextToken ();
     	} else {
     	    if (Mode == DM_NEED_IDENT) {
     	       	Error (ERR_IDENT_EXPECTED);
     	    }
     	    D->Ident[0] = '\0';
     	    return;
     	}
    }

    while (curtok == TOK_LBRACK || curtok == TOK_LPAREN) {
       	if (curtok == TOK_LPAREN) {
       	    /* Function declaration */
	    FuncDesc* F;
       	    NextToken ();
	    /* Parse the function declaration */
       	    F = ParseFuncDecl ();
	    *D->T++ = T_FUNC;
	    EncodePtr (D->T, F);
	    D->T += DECODE_SIZE;
       	} else {
	    /* Array declaration */
       	    unsigned long Size = 0;
       	    NextToken ();
	    /* Read the size if it is given */
       	    if (curtok != TOK_RBRACK) {
    	     	struct expent lval;
       	       	constexpr (&lval);
       	       	Size = lval.e_const;
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
    Decl (D, Mode);

    /* Add the base type. */
    TypeCpy (D->T, Spec->Type);

    /* Check the size of the generated type */
    if (!IsTypeFunc (D->Type) && !IsTypeVoid (D->Type) && SizeOf (D->Type) >= 0x10000) {
     	Error (ERR_ILLEGAL_SIZE);
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



static void ParseVoidInit (void)
/* Parse an initialization of a void variable (special cc65 extension) */
{
    struct expent lval;

    /* Allow an arbitrary list of values */
    ConsumeLCurly ();
    do {
	constexpr (&lval);
	switch (lval.e_tptr[0]) {

	    case T_SCHAR:
	    case T_UCHAR:
		if ((lval.e_flags & E_MCTYPE) == E_TCONST) {
		    /* Make it byte sized */
		    lval.e_const &= 0xFF;
		}
		DefineData (&lval);
		break;

	    case T_SHORT:
	    case T_USHORT:
	    case T_INT:
	    case T_UINT:
	    case T_PTR:
	    case T_ARRAY:
		if ((lval.e_flags & E_MCTYPE) == E_TCONST) {
    		    /* Make it word sized */
		    lval.e_const &= 0xFFFF;
		}
		DefineData (&lval);
		break;

	    case T_LONG:
	    case T_ULONG:
		DefineData (&lval);
		break;

	    default:
		Error (ERR_ILLEGAL_TYPE);
	    	break;

	}

	if (curtok != TOK_COMMA) {
	    break;
	}
	NextToken ();

    } while (curtok != TOK_RCURLY);

    ConsumeRCurly ();
}



static void ParseStructInit (type* Type)
/* Parse initialization of a struct or union */
{
    SymEntry* Entry;
    SymTable* Tab;

    /* Consume the opening curly brace */
    ConsumeLCurly ();

    /* Get a pointer to the struct entry from the type */
    Entry = (SymEntry*) Decode (Type + 1);

    /* Check if this struct definition has a field table. If it doesn't, it
     * is an incomplete definition.
     */
    Tab = Entry->V.S.SymTab;
    if (Tab == 0) {
     	Error (ERR_INIT_INCOMPLETE_TYPE);
	/* Returning here will cause lots of errors, but recovery is difficult */
	return;
    }

    /* Get a pointer to the list of symbols */
    Entry = Tab->SymHead;
    while (curtok != TOK_RCURLY) {
 	if (Entry == 0) {
 	    Error (ERR_TOO_MANY_INITIALIZERS);
 	    return;
 	}
 	ParseInit (Entry->Type);
 	Entry = Entry->NextSym;
 	if (curtok != TOK_COMMA)
 	    break;
 	NextToken ();
    }

    /* Consume the closing curly brace */
    ConsumeRCurly ();

    /* If there are struct fields left, reserve additional storage */
    while (Entry) {
 	g_zerobytes (SizeOf (Entry->Type));
 	Entry = Entry->NextSym;
    }
}



void ParseInit (type* T)
/* Parse initialization of variables. */
{
    int count;
    struct expent lval;
    type* t;
    const char* str;
    int sz;

    switch (UnqualifiedType (*T)) {

     	case T_SCHAR:
     	case T_UCHAR:
     	    constexpr (&lval);
	    if ((lval.e_flags & E_MCTYPE) == E_TCONST) {
	    	/* Make it byte sized */
	    	lval.e_const &= 0xFF;
	    }
	    assignadjust (T, &lval);
	    DefineData (&lval);
     	    break;

    	case T_SHORT:
    	case T_USHORT:
     	case T_INT:
     	case T_UINT:
     	case T_PTR:
     	    constexpr (&lval);
	    if ((lval.e_flags & E_MCTYPE) == E_TCONST) {
	    	/* Make it word sized */
	    	lval.e_const &= 0xFFFF;
	    }
	    assignadjust (T, &lval);
	    DefineData (&lval);
     	    break;

    	case T_LONG:
    	case T_ULONG:
	    constexpr (&lval);
	    if ((lval.e_flags & E_MCTYPE) == E_TCONST) {
	    	/* Make it long sized */
	    	lval.e_const &= 0xFFFFFFFF;
	    }
	    assignadjust (T, &lval);
	    DefineData (&lval);
     	    break;

     	case T_ARRAY:
     	    sz = Decode (T + 1);
	    t = T + DECODE_SIZE + 1;
       	    if (IsTypeChar(t) && curtok == TOK_SCONST) {
     	     	str = GetLiteral (curval);
     	     	count = strlen (str) + 1;
	    	TranslateLiteralPool (curval);	/* Translate into target charset */
     	     	g_defbytes (str, count);
     	     	ResetLiteralOffs (curval);	/* Remove string from pool */
     	     	NextToken ();
     	    } else {
     	     	ConsumeLCurly ();
     	     	count = 0;
     	     	while (curtok != TOK_RCURLY) {
     	     	    ParseInit (T + DECODE_SIZE + 1);
     	     	    ++count;
     	     	    if (curtok != TOK_COMMA)
     	     	 	break;
     	     	    NextToken ();
     	     	}
     	     	ConsumeRCurly ();
     	    }
     	    if (sz == 0) {
     	     	Encode (T + 1, count);
     	    } else if (count < sz) {
     	     	g_zerobytes ((sz - count) * SizeOf (T + DECODE_SIZE + 1));
     	    } else if (count > sz) {
     	     	Error (ERR_TOO_MANY_INITIALIZERS);
     	    }
     	    break;

        case T_STRUCT:
        case T_UNION:
	    ParseStructInit (T);
     	    break;

	case T_VOID:
	    if (!ANSI) {
	    	/* Special cc65 extension in non ANSI mode */
	      	ParseVoidInit ();
		break;
	    }
	    /* FALLTHROUGH */

     	default:
	    Error (ERR_ILLEGAL_TYPE);
	    break;

    }
}



