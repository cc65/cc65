/*****************************************************************************/
/*                                                                           */
/*				   compile.c				     */
/*                                                                           */
/*			 Top level compiler subroutine			     */
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



#include <stdlib.h>

/* common */
#include "version.h"

/* cc65 */
#include "asmlabel.h"
#include "codegen.h"
#include "declare.h"
#include "error.h"
#include "expr.h"
#include "function.h"
#include "global.h"
#include "incpath.h"
#include "litpool.h"
#include "macrotab.h"
#include "pragma.h"
#include "symtab.h"
#include "compile.h"



/*****************************************************************************/
/*		    		     Code				     */
/*****************************************************************************/



static void Parse (void)
/* Top level parser routine. */
{
    int comma;
    SymEntry* Entry;

    NextToken ();		 	/* "prime" the pump */
    NextToken ();
    while (curtok != TOK_CEOF) {

	DeclSpec 	Spec;
	Declaration 	Decl;
	int		NeedStorage;

	/* Check for empty statements */
	if (curtok == TOK_SEMI) {
	    NextToken ();
	    continue;
	}

	/* Check for an ASM statement (which is allowed also on global level) */
	if (curtok == TOK_ASM) {
	    doasm ();
	    ConsumeSemi ();
	    continue;
	}

	/* Check for a #pragma */
	if (curtok == TOK_PRAGMA) {
	    DoPragma ();
	    continue;
	}

       	/* Read variable defs and functions */
	ParseDeclSpec (&Spec, SC_EXTERN | SC_STATIC, T_INT);

	/* Don't accept illegal storage classes */
	if (Spec.StorageClass == SC_AUTO || Spec.StorageClass == SC_REGISTER) {
	    Error ("Illegal storage class");
	    Spec.StorageClass = SC_EXTERN | SC_STATIC;
	}

	/* Check if this is only a type declaration */
	if (curtok == TOK_SEMI) {
	    CheckEmptyDecl (&Spec);
	    NextToken ();
	    continue;
	}

       	/* Check if we must reserve storage for the variable. We do
	 * this if we don't had a storage class given ("int i") or
	 * if the storage class is explicitly specified as static.
	 * This means that "extern int i" will not get storage
	 * allocated.
	 */
	NeedStorage = (Spec.StorageClass & SC_TYPEDEF) == 0 &&
		      ((Spec.Flags & DS_DEF_STORAGE) != 0  ||
	   	      (Spec.StorageClass & (SC_STATIC | SC_EXTERN)) == SC_STATIC);

	/* Read declarations for this type */
	Entry = 0;
	comma = 0;
       	while (1) {

	    unsigned SymFlags;

	    /* Read the next declaration */
	    ParseDecl (&Spec, &Decl, DM_NEED_IDENT);
	    if (Decl.Ident[0] == '\0') {
	    	NextToken ();
	    	break;
	    }

	    /* Get the symbol flags */
	    SymFlags = Spec.StorageClass;
	    if (IsTypeFunc (Decl.Type)) {
		SymFlags |= SC_FUNC;
	    } else {
	    	if (NeedStorage) {
		    /* We will allocate storage, variable is defined */
		    SymFlags |= SC_STORAGE | SC_DEF;
		}
	    }

	    /* Add an entry to the symbol table */
	    Entry = AddGlobalSym (Decl.Ident, Decl.Type, SymFlags);

	    /* Reserve storage for the variable if we need to */
       	    if (SymFlags & SC_STORAGE) {

	     	/* Get the size of the variable */
	     	unsigned Size = SizeOf (Decl.Type);

	     	/* Allow initialization */
	     	if (curtok == TOK_ASSIGN) {

	     	    /* We cannot initialize types of unknown size, or
	     	     * void types in non ANSI mode.
	     	     */
       	       	    if (Size == 0) {
	     	    	if (!IsTypeVoid (Decl.Type)) {
	     	    	    if (!IsTypeArray (Decl.Type)) {
	     	    	      	/* Size is unknown and not an array */
	     	    	      	Error ("Variable `%s' has unknown size", Decl.Ident);
	     	    	    }
	     	    	} else if (ANSI) {
	     	    	    /* We cannot declare variables of type void */
	     	    	    Error ("Illegal type for variable `%s'", Decl.Ident);
	     	    	}
	     	    }

	     	    /* Switch to the data or rodata segment */
		    if (IsQualConst (Decl.Type)) {
			g_userodata ();
		    } else {
			g_usedata ();
		    }

	     	    /* Define a label */
	     	    g_defgloblabel (Entry->Name);

	     	    /* Skip the '=' */
	     	    NextToken ();

	     	    /* Parse the initialization */
	     	    ParseInit (Entry->Type);
	     	} else {

	     	    if (IsTypeVoid (Decl.Type)) {
	     	    	/* We cannot declare variables of type void */
			Error ("Illegal type for variable `%s'", Decl.Ident);
	     	    } else if (Size == 0) {
	     		/* Size is unknown */
			Error ("Variable `%s' has unknown size", Decl.Ident);
	     	    }

	     	    /* Switch to the BSS segment */
	     	    g_usebss ();

	     	    /* Define a label */
	     	    g_defgloblabel (Entry->Name);

	     	    /* Allocate space for uninitialized variable */
	     	    g_res (SizeOf (Entry->Type));
	     	}

	    }

	    /* Check for end of declaration list */
	    if (curtok == TOK_COMMA) {
	    	NextToken ();
	    	comma = 1;
	    } else {
	     	break;
	    }
	}

	/* Function declaration? */
	if (Entry && IsTypeFunc (Entry->Type)) {

	    /* Function */
	    if (!comma) {

	     	if (curtok == TOK_SEMI) {

	     	    /* Prototype only */
	     	    NextToken ();

	     	} else {
	     	    if (Entry) {
	     	        NewFunc (Entry);
	     	    }
	     	}
	    }

	} else {

	    /* Must be followed by a semicolon */
	    ConsumeSemi ();

	}
    }
}



void Compile (void)
/* Top level compile routine. Will setup things and call the parser. */
{
    char* Path;


    /* Add some standard paths to the include search path */
    AddIncludePath ("", INC_USER);		/* Current directory */
    AddIncludePath ("include", INC_SYS);
#ifdef CC65_INC
    AddIncludePath (CC65_INC, INC_SYS);
#else
    AddIncludePath ("/usr/lib/cc65/include", INC_SYS);
#endif
    Path = getenv ("CC65_INC");
    if (Path) {
	AddIncludePath (Path, INC_SYS | INC_USER);
    }

    /* Add macros that are always defined */
    AddNumericMacro ("__CC65__", (VER_MAJOR * 0x100) + (VER_MINOR * 0x10) + VER_PATCH);

    /* Strict ANSI macro */
    if (ANSI) {
	AddNumericMacro ("__STRICT_ANSI__", 1);
    }

    /* Optimization macros */
    if (Optimize) {
	AddNumericMacro ("__OPT__", 1);
	if (FavourSize == 0) {
	    AddNumericMacro ("__OPT_i__", 1);
	}
	if (EnableRegVars) {
	    AddNumericMacro ("__OPT_r__", 1);
	}
	if (InlineStdFuncs) {
	    AddNumericMacro ("__OPT_s__", 1);
	}
    }

    /* Initialize the literal pool */
    InitLiteralPool ();

    /* Create the base lexical level */
    EnterGlobalLevel ();

    /* Generate the code generator preamble */
    g_preamble ();

    /* Ok, start the ball rolling... */
    Parse ();

    /* Dump the literal pool. */
    DumpLiteralPool ();

    /* Write imported/exported symbols */
    EmitExternals ();

    if (Debug) {
	PrintLiteralPoolStats (stdout);
	PrintMacroStats (stdout);
    }

    /* Leave the main lexical level */
    LeaveGlobalLevel ();

    /* Print an error report */
    ErrorReport ();
}



