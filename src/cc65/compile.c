/*****************************************************************************/
/*                                                                           */
/*				   compile.c				     */
/*                                                                           */
/*			 Top level compiler subroutine			     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000-2004 Ullrich von Bassewitz                                       */
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



#include <stdlib.h>
#include <time.h>

/* common */
#include "debugflag.h"
#include "version.h"
#include "xmalloc.h"
#include "xsprintf.h"

/* cc65 */
#include "asmlabel.h"
#include "asmstmt.h"
#include "codegen.h"
#include "compile.h"
#include "declare.h"
#include "error.h"
#include "expr.h"
#include "function.h"
#include "global.h"
#include "input.h"
#include "litpool.h"
#include "macrotab.h"
#include "pragma.h"
#include "preproc.h"
#include "standard.h"
#include "symtab.h"



/*****************************************************************************/
/*		    	      	     Code				     */
/*****************************************************************************/



static void Parse (void)
/* Top level parser routine. */
{
    int comma;
    SymEntry* Entry;

    /* Go... */
    NextToken ();
    NextToken ();

    /* Parse until end of input */
    while (CurTok.Tok != TOK_CEOF) {

	DeclSpec  	Spec;
	Declaration 	Decl;
	int	  	NeedStorage;

	/* Check for empty statements */
	if (CurTok.Tok == TOK_SEMI) {
	    NextToken ();
	    continue;
	}

	/* Check for an ASM statement (which is allowed also on global level) */
	if (CurTok.Tok == TOK_ASM) {
	    AsmStatement ();
	    ConsumeSemi ();
	    continue;
	}

	/* Check for a #pragma */
	if (CurTok.Tok == TOK_PRAGMA) {
	    DoPragma ();
	    continue;
	}

       	/* Read variable defs and functions */
	ParseDeclSpec (&Spec, SC_EXTERN | SC_STATIC, T_INT);

	/* Don't accept illegal storage classes */
        if ((Spec.StorageClass & SC_TYPE) == 0) {
            if ((Spec.StorageClass & SC_AUTO) != 0 ||
                (Spec.StorageClass & SC_REGISTER) != 0) {
                Error ("Illegal storage class");
                Spec.StorageClass = SC_EXTERN | SC_STATIC;
            }
        }

	/* Check if this is only a type declaration */
	if (CurTok.Tok == TOK_SEMI) {
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
	    } else if ((SymFlags & SC_TYPEDEF) == 0) {
                if ((Spec.Flags & DS_DEF_TYPE) != 0 && IS_Get (&Standard) >= STD_C99) {
                    Warning ("Implicit `int' is an obsolete feature");
                }
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
	     	if (CurTok.Tok == TOK_ASSIGN) {

	     	    /* We cannot initialize types of unknown size, or
	     	     * void types in non ANSI mode.
	     	     */
       	       	    if (Size == 0) {
	     	    	if (!IsTypeVoid (Decl.Type)) {
	     	    	    if (!IsTypeArray (Decl.Type)) {
	     	      	      	/* Size is unknown and not an array */
	     	    	      	Error ("Variable `%s' has unknown size", Decl.Ident);
	     	    	    }
	     	    	} else if (IS_Get (&Standard) != STD_CC65) {
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
                        Entry->Flags &= ~(SC_STORAGE | SC_DEF);
                    } else if (Size == 0) {
	     		/* Size is unknown. Is it an array? */
                        if (!IsTypeArray (Decl.Type)) {
                            Error ("Variable `%s' has unknown size", Decl.Ident);
                        }
                        Entry->Flags &= ~(SC_STORAGE | SC_DEF);
	     	    }

                    /* Allocate storage if it is still needed */
                    if (Entry->Flags & SC_STORAGE) {

                        /* Switch to the BSS segment */
                        g_usebss ();

                        /* Define a label */
                        g_defgloblabel (Entry->Name);

                        /* Allocate space for uninitialized variable */
                        g_res (Size);
                    }
	     	}

	    }

	    /* Check for end of declaration list */
	    if (CurTok.Tok == TOK_COMMA) {
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
	     	if (CurTok.Tok == TOK_SEMI) {
	     	    /* Prototype only */
	     	    NextToken ();
	     	} else if (Entry) {
                    /* Function body definition */
                    if (SymIsDef (Entry)) {
                        Error ("Body for function `%s' has already been defined",
                               Entry->Name);
                    }
                    NewFunc (Entry);
	     	}
	    }

	} else {

	    /* Must be followed by a semicolon */
	    ConsumeSemi ();

	}
    }
}



void Compile (const char* FileName)
/* Top level compile routine. Will setup things and call the parser. */
{
    char        DateStr[32];
    char        TimeStr[32];
    time_t      Time;
    struct tm*  TM;

    /* Since strftime is locale dependent, we need the abbreviated month names
     * in english.
     */
    static const char MonthNames[12][4] = {
        "Jan", "Feb", "Mar", "Apr", "May", "Jun",
        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
    };

    /* Add macros that are always defined */
    DefineNumericMacro ("__CC65__", VERSION);

    /* Language standard that is supported */
    DefineNumericMacro ("__CC65_STD_C89__", STD_C89);
    DefineNumericMacro ("__CC65_STD_C99__", STD_C99);
    DefineNumericMacro ("__CC65_STD_CC65__", STD_CC65);
    DefineNumericMacro ("__CC65_STD__", IS_Get (&Standard));

    /* Optimization macros. Since no source code has been parsed for now, the
     * IS_Get functions access the values in effect now, regardless of any
     * changes using #pragma later.
     */
    if (IS_Get (&Optimize)) {
        long CodeSize = IS_Get (&CodeSizeFactor);
    	DefineNumericMacro ("__OPT__", 1);
       	if (CodeSize > 100) {
    	    DefineNumericMacro ("__OPT_i__", CodeSize);
    	}
    	if (IS_Get (&EnableRegVars)) {
    	    DefineNumericMacro ("__OPT_r__", 1);
    	}
       	if (IS_Get (&InlineStdFuncs)) {
    	    DefineNumericMacro ("__OPT_s__", 1);
    	}
    }

    /* __TIME__ and __DATE__ macros */
    Time = time (0);
    TM   = localtime (&Time);
    xsprintf (DateStr, sizeof (DateStr), "\"%s %2d %d\"",
              MonthNames[TM->tm_mon], TM->tm_mday, TM->tm_year + 1900);
    strftime (TimeStr, sizeof (TimeStr), "\"%H:%M:%S\"", TM);
    DefineTextMacro ("__DATE__", DateStr);
    DefineTextMacro ("__TIME__", TimeStr);

    /* Initialize the literal pool */
    InitLiteralPool ();

    /* Create the base lexical level */
    EnterGlobalLevel ();

    /* Generate the code generator preamble */
    g_preamble ();

    /* Open the input file */
    OpenMainFile (FileName);

    /* Are we supposed to compile or just preprocess the input? */
    if (PreprocessOnly) {

        while (NextLine ()) {
            Preprocess ();
            printf ("%.*s\n", SB_GetLen (Line), SB_GetConstBuf (Line));
        }

        if (Debug) {
            PrintMacroStats (stdout);
        }

    } else {

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

    }

    /* Leave the main lexical level */
    LeaveGlobalLevel ();

    /* Print an error report */
    ErrorReport ();
}




