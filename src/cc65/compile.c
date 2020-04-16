/*****************************************************************************/
/*                                                                           */
/*                                 compile.c                                 */
/*                                                                           */
/*                       Top level compiler subroutine                       */
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



#include <stdlib.h>
#include <time.h>

/* common */
#include "addrsize.h"
#include "debugflag.h"
#include "segnames.h"
#include "version.h"
#include "xmalloc.h"
#include "xsprintf.h"

/* cc65 */
#include "asmlabel.h"
#include "asmstmt.h"
#include "codegen.h"
#include "codeopt.h"
#include "compile.h"
#include "declare.h"
#include "error.h"
#include "expr.h"
#include "function.h"
#include "global.h"
#include "input.h"
#include "litpool.h"
#include "macrotab.h"
#include "output.h"
#include "pragma.h"
#include "preproc.h"
#include "standard.h"
#include "staticassert.h"
#include "symtab.h"



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



static void Parse (void)
/* Top level parser routine. */
{
    int comma;
    SymEntry* Entry;
    FuncDesc* FuncDef = 0;

    /* Go... */
    NextToken ();
    NextToken ();

    /* Parse until end of input */
    while (CurTok.Tok != TOK_CEOF) {

        DeclSpec        Spec;

        /* Check for empty statements */
        if (CurTok.Tok == TOK_SEMI) {
            NextToken ();
            continue;
        }

        /* Disallow ASM statements on global level */
        if (CurTok.Tok == TOK_ASM) {
            Error ("__asm__ is not allowed here");
            /* Parse and remove the statement for error recovery */
            AsmStatement ();
            ConsumeSemi ();
            RemoveGlobalCode ();
            continue;
        }

        /* Check for a #pragma */
        if (CurTok.Tok == TOK_PRAGMA) {
            DoPragma ();
            continue;
        }

        /* Check for a _Static_assert */
        if (CurTok.Tok == TOK_STATIC_ASSERT) {
            ParseStaticAssert ();
            continue;
        }

        /* Read variable defs and functions */
        ParseDeclSpec (&Spec, SC_EXTERN | SC_STATIC, T_INT);

        /* Don't accept illegal storage classes */
        if ((Spec.StorageClass & SC_TYPEMASK) == 0) {
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

        /* Read declarations for this type */
        Entry = 0;
        comma = 0;
        while (1) {

            Declaration Decl;

            /* Read the next declaration */
            ParseDecl (&Spec, &Decl, DM_NEED_IDENT);

            /* Check if we must reserve storage for the variable. We do this,
            **
            **   - if it is not a typedef or function,
            **   - if we don't had a storage class given ("int i")
            **   - if the storage class is explicitly specified as static,
            **   - or if there is an initialization.
            **
            ** This means that "extern int i;" will not get storage allocated.
            */
            if ((Decl.StorageClass & SC_FUNC) != SC_FUNC        &&
                (Decl.StorageClass & SC_TYPEMASK) != SC_TYPEDEF &&
                (Decl.StorageClass & SC_FICTITIOUS) != SC_FICTITIOUS) {
                if ((Spec.Flags & DS_DEF_STORAGE) != 0                       ||
                    (Decl.StorageClass & (SC_EXTERN|SC_STATIC)) == SC_STATIC ||
                    ((Decl.StorageClass & SC_EXTERN) != 0 &&
                     CurTok.Tok == TOK_ASSIGN)) {
                    /* We will allocate storage */
                    Decl.StorageClass |= SC_STORAGE;
                } else {
                    /* It's a declaration */
                    Decl.StorageClass |= SC_DECL;
                }
            }

            /* If this is a function declarator that is not followed by a comma
            ** or semicolon, it must be followed by a function body.
            */
            if ((Decl.StorageClass & SC_FUNC) != 0) {
                if (CurTok.Tok != TOK_COMMA && CurTok.Tok != TOK_SEMI) {
                    /* A definition */
                    Decl.StorageClass |= SC_DEF;

                    /* Convert an empty parameter list into one accepting no
                    ** parameters (same as void) as required by the standard.
                    */
                    FuncDef = GetFuncDesc (Decl.Type);
                    if (FuncDef->Flags & FD_EMPTY) {
                        FuncDef->Flags = (FuncDef->Flags & ~FD_EMPTY) | FD_VOID_PARAM;
                    }
                } else {
                    /* Just a declaration */
                    Decl.StorageClass |= SC_DECL;
                }
            }

            /* Add an entry to the symbol table */
            Entry = AddGlobalSym (Decl.Ident, Decl.Type, Decl.StorageClass);

            /* Add declaration attributes */
            SymUseAttr (Entry, &Decl);

            /* Reserve storage for the variable if we need to */
            if (Decl.StorageClass & SC_STORAGE) {

                /* Get the size of the variable */
                unsigned Size = SizeOf (Decl.Type);

                /* Allow initialization */
                if (CurTok.Tok == TOK_ASSIGN) {

                    /* This is a definition */
                    if (SymIsDef (Entry)) {
                        Error ("Global variable '%s' has already been defined",
                               Entry->Name);
                    }
                    Entry->Flags |= SC_DEF;

                    /* We cannot initialize types of unknown size, or
                    ** void types in ISO modes.
                    */
                    if (Size == 0) {
                        if (!IsTypeVoid (Decl.Type)) {
                            if (!IsTypeArray (Decl.Type)) {
                                /* Size is unknown and not an array */
                                Error ("Variable '%s' has unknown size", Decl.Ident);
                            }
                        } else if (IS_Get (&Standard) != STD_CC65) {
                            /* We cannot declare variables of type void */
                            Error ("Illegal type for variable '%s'", Decl.Ident);
                        }
                    }

                    /* Switch to the data or rodata segment. For arrays, check
                     ** the element qualifiers, since not the array but its
                     ** elements are const.
                     */
                    if (IsQualConst (GetBaseElementType (Decl.Type))) {
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
                        Error ("Illegal type for variable '%s'", Decl.Ident);
                        Entry->Flags &= ~(SC_STORAGE | SC_DEF);
                    } else if (Size == 0 && SymIsDef (Entry)) {
                        /* Size is unknown. Is it an array? */
                        if (!IsTypeArray (Decl.Type)) {
                            Error ("Variable '%s' has unknown size", Decl.Ident);
                        }
                    } else {
                        /* A global (including static) uninitialized variable is
                        ** only a tentative definition. For example, this is valid:
                        ** int i;
                        ** int i;
                        ** static int j;
                        ** static int j = 42;
                        ** Code for them will be generated by FinishCompile().
                        ** For now, just save the BSS segment name
                        ** (can be set by #pragma bss-name).
                        */
                        const char* bssName = GetSegName (SEG_BSS);

                        if (Entry->V.BssName && strcmp (Entry->V.BssName, bssName) != 0) {
                            Error ("Global variable '%s' already was defined in the '%s' segment.",
                                   Entry->Name, Entry->V.BssName);
                        }
                        Entry->V.BssName = xstrdup (bssName);

                        /* This is to make the automatical zeropage setting of the symbol
                        ** work right.
                        */
                        g_usebss ();
                    }
                }

                /* Make the symbol zeropage according to the segment address size */
                if ((Entry->Flags & SC_EXTERN) != 0) {
                    if (GetSegAddrSize (GetSegName (CS->CurDSeg)) == ADDR_SIZE_ZP) {
                        Entry->Flags |= SC_ZEROPAGE;
                        /* Check for enum forward declaration.
                        ** Warn about it when extensions are not allowed.
                        */
                        if (Size == 0 && IsTypeEnum (Decl.Type)) {
                            if (IS_Get (&Standard) != STD_CC65) {
                                Warning ("ISO C forbids forward references to 'enum' types");
                            }
                        }
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
                } else {
                    /* Parse the function body */
                    NewFunc (Entry, FuncDef);

                    /* Make sure we aren't omitting any work */
                    CheckDeferredOpAllDone ();
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
    ** in English.
    */
    static const char MonthNames[12][4] = {
        "Jan", "Feb", "Mar", "Apr", "May", "Jun",
        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
    };

    /* Add macros that are always defined */
    DefineNumericMacro ("__CC65__", GetVersionAsNumber ());

    /* Language standard that is supported */
    DefineNumericMacro ("__CC65_STD_C89__", STD_C89);
    DefineNumericMacro ("__CC65_STD_C99__", STD_C99);
    DefineNumericMacro ("__CC65_STD_CC65__", STD_CC65);
    DefineNumericMacro ("__CC65_STD__", IS_Get (&Standard));

    /* Optimization macros. Since no source code has been parsed for now, the
    ** IS_Get functions access the values in effect now, regardless of any
    ** changes using #pragma later.
    */
    if (IS_Get (&Optimize)) {
        DefineNumericMacro ("__OPT__", 1);
    }
    {
        long CodeSize = IS_Get (&CodeSizeFactor);
        if (CodeSize > 100) {
            DefineNumericMacro ("__OPT_i__", CodeSize);
        }
    }
    if (IS_Get (&EnableRegVars)) {
        DefineNumericMacro ("__OPT_r__", 1);
    }
    if (IS_Get (&InlineStdFuncs)) {
        DefineNumericMacro ("__OPT_s__", 1);
    }
    if (IS_Get (&EagerlyInlineFuncs)) {
        DefineNumericMacro ("__EAGERLY_INLINE_FUNCS__", 1);
    }

    /* __TIME__ and __DATE__ macros */
    Time = time (0);
    TM   = localtime (&Time);
    xsprintf (DateStr, sizeof (DateStr), "\"%s %2d %d\"",
              MonthNames[TM->tm_mon], TM->tm_mday, TM->tm_year + 1900);
    strftime (TimeStr, sizeof (TimeStr), "\"%H:%M:%S\"", TM);
    DefineTextMacro ("__DATE__", DateStr);
    DefineTextMacro ("__TIME__", TimeStr);

    /* Other standard macros */
    /* DefineNumericMacro ("__STDC__", 1);      <- not now */
    DefineNumericMacro ("__STDC_HOSTED__", 1);

    InitDeferredOps ();

    /* Create the base lexical level */
    EnterGlobalLevel ();

    /* Create the global code and data segments */
    CreateGlobalSegments ();

    /* There shouldn't be needs for local labels outside a function, but the
    ** current code generator still tries to get some at times even though the
    ** code were ill-formed. So just set it up with the global segment list.
    */
    UseLabelPoolFromSegments (GS);

    /* Initialize the literal pool */
    InitLiteralPool ();

    /* Generate the code generator preamble */
    g_preamble ();

    /* Open the input file */
    OpenMainFile (FileName);

    /* Are we supposed to compile or just preprocess the input? */
    if (PreprocessOnly) {

        /* Open the file */
        OpenOutputFile ();

        /* Preprocess each line and write it to the output file */
        while (NextLine ()) {
            Preprocess ();
            WriteOutput ("%.*s\n", (int) SB_GetLen (Line), SB_GetConstBuf (Line));
        }

        /* Close the output file */
        CloseOutputFile ();

    } else {

        /* Used for emitting externals */
        SymEntry* Entry;

        /* Ok, start the ball rolling... */
        Parse ();

        /* Reset the BSS segment name to its default; so that the below strcmp()
        ** will work as expected, at the beginning of the list of variables
        */
        SetSegName (SEG_BSS, SEGNAME_BSS);

        /* Walk over all global symbols and generate code for uninitialized
        ** global variables.
        */
        for (Entry = GetGlobalSymTab ()->SymHead; Entry; Entry = Entry->NextSym) {
            if ((Entry->Flags & (SC_STORAGE | SC_DEF | SC_STATIC)) == (SC_STORAGE | SC_STATIC)) {
                /* Assembly definition of uninitialized global variable */
                SymEntry* Sym = GetSymType (Entry->Type);
                unsigned Size = SizeOf (Entry->Type);
                if (Size == 0 && IsTypeArray (Entry->Type)) {
                    if (GetElementCount (Entry->Type) == UNSPECIFIED) {
                        /* Assume array size of 1 */
                        SetElementCount (Entry->Type, 1);
                        Size = SizeOf (Entry->Type);
                        Warning ("Incomplete array '%s[]' assumed to have one element", Entry->Name);
                    }

                    Sym = GetSymType (GetElementType (Entry->Type));
                }

                /* For non-ESU types, Size != 0 */
                if (Size != 0 || (Sym != 0 && SymIsDef (Sym))) {
                    /* Set the segment name only when it changes */
                    if (strcmp (GetSegName (SEG_BSS), Entry->V.BssName) != 0) {
                        SetSegName (SEG_BSS, Entry->V.BssName);
                        g_segname (SEG_BSS);
                    }
                    g_usebss ();
                    g_defgloblabel (Entry->Name);
                    g_res (Size);

                    /* Mark as defined; so that it will be exported, not imported */
                    Entry->Flags |= SC_DEF;
                } else {
                    /* Tentative declared variable is still of incomplete type */
                    Error ("Definition of '%s' has type '%s' that is never completed",
                           Entry->Name,
                           GetFullTypeName (Entry->Type));
                }
            }
        }
    }

    DoneDeferredOps ();

    if (Debug) {
        PrintMacroStats (stdout);
    }

    /* Print an error report */
    ErrorReport ();
}



void FinishCompile (void)
/* Emit literals, debug info, do cleanup and optimizations */
{
    SymEntry* Entry;

    /* Walk over all global symbols and do clean-up and optimizations for
    ** functions.
    */
    for (Entry = GetGlobalSymTab ()->SymHead; Entry; Entry = Entry->NextSym) {
        if (SymIsOutputFunc (Entry)) {
            /* Continue with previous label numbers */
            UseLabelPoolFromSegments (Entry->V.F.Seg);

            /* Function which is defined and referenced or extern */
            MoveLiteralPool (Entry->V.F.LitPool);
            CS_MergeLabels (Entry->V.F.Seg->Code);
            RunOpt (Entry->V.F.Seg->Code);
        }
    }

    /* Output the literal pool */
    OutputGlobalLiteralPool ();

    /* Emit debug infos if enabled */
    EmitDebugInfo ();

    /* Write imported/exported symbols */
    EmitExternals ();

    /* Leave the main lexical level */
    LeaveGlobalLevel ();
}
