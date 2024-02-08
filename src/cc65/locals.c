/*****************************************************************************/
/*                                                                           */
/*                                 locals.c                                  */
/*                                                                           */
/*              Local variable handling for the cc65 C compiler              */
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



/* common */
#include "xmalloc.h"
#include "xsprintf.h"

/* cc65 */
#include "anonname.h"
#include "asmlabel.h"
#include "codegen.h"
#include "declare.h"
#include "error.h"
#include "expr.h"
#include "function.h"
#include "global.h"
#include "initdata.h"
#include "loadexpr.h"
#include "locals.h"
#include "seqpoint.h"
#include "stackptr.h"
#include "standard.h"
#include "staticassert.h"
#include "symtab.h"
#include "typeconv.h"
#include "input.h"



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



static unsigned AllocLabel (void (*UseSeg) ())
/* Switch to a segment, define a local data label and return it */
{
    unsigned DataLabel;

    /* Switch to the segment */
    UseSeg ();

    /* Define the variable label */
    DataLabel = GetLocalDataLabel ();
    g_defdatalabel (DataLabel);

    /* Return the label */
    return DataLabel;
}



static void AllocStorage (unsigned DataLabel, void (*UseSeg) (), unsigned Size)
/* Reserve Size bytes of BSS storage prefixed by a local data label. */
{
    /* Switch to the segment */
    UseSeg ();

    /* Define the variable label */
    g_defdatalabel (DataLabel);

    /* Reserve space for the data */
    g_res (Size);
}



static void ParseRegisterDecl (Declarator* Decl, int Reg)
/* Parse the declarator of a register variable. Reg is the offset of the
** variable in the register bank.
*/
{
    SymEntry* Sym;

    /* Determine if this is a compound variable */
    int IsCompound = IsClassStruct (Decl->Type) || IsTypeArray (Decl->Type);

    /* Get the size of the variable */
    unsigned Size = SizeOf (Decl->Type);

    /* Save the current contents of the register variable on stack */
    F_AllocLocalSpace (CurrentFunc);
    g_save_regvars (Reg, Size);

    /* Add the symbol to the symbol table. We do that now, because for register
    ** variables the current stack pointer is implicitly used as location for
    ** the save area.
    */
    Sym = AddLocalSym (Decl->Ident, Decl->Type, Decl->StorageClass, Reg);

    /* Check for an optional initialization */
    if (CurTok.Tok == TOK_ASSIGN) {

        /* Skip the '=' */
        NextToken ();

        /* Special handling for compound types */
        if (IsCompound) {

            /* Switch to read only data and define a label for the
            ** initialization data.
            */
            unsigned InitLabel = AllocLabel (g_userodata);

            /* Parse the initialization generating a memory image of the
            ** data in the RODATA segment. The function does return the size
            ** of the initialization data, which may be greater than the
            ** actual size of the type, if the type is a structure with a
            ** flexible array member that has been initialized. Since we must
            ** know the size of the data in advance for register variables,
            ** we cannot allow that here.
            */
            if (ParseInit (Sym->Type) != Size) {
                Error ("Cannot initialize flexible array members of storage class 'register'");
            }

            /* Generate code to copy this data into the variable space */
            g_initregister (InitLabel, Reg, Size);

        } else {

            ExprDesc Expr;
            ED_Init (&Expr);

            /* Parse the expression */
            hie1 (&Expr);

            /* Convert it to the target type */
            TypeConversion (&Expr, Sym->Type);

            /* Load the value into the primary */
            LoadExpr (CF_NONE, &Expr);

            /* Store the value into the variable */
            g_putstatic (CF_REGVAR | CG_TypeOf (Sym->Type), Reg, 0);

            /* This has to be done at sequence point */
            DoDeferred (SQP_KEEP_NONE, &Expr);
        }

        /* Mark the variable as referenced */
        Sym->Flags |= SC_REF;
    }

    /* Cannot allocate a variable of unknown size */
    if (HasUnknownSize (Sym->Type)) {
        if (IsTypeArray (Decl->Type)) {
            Error ("Array '%s' has unknown size", Decl->Ident);
        } else {
            Error ("Variable '%s' has unknown size", Decl->Ident);
        }
    }
}



static void ParseAutoDecl (Declarator* Decl)
/* Parse the declarator of an auto variable. */
{
    unsigned  Flags;
    SymEntry* Sym;

    /* Determine if this is a compound variable */
    int IsCompound = IsClassStruct (Decl->Type) || IsTypeArray (Decl->Type);

    /* Get the size of the variable */
    unsigned Size = SizeOf (Decl->Type);

    /* Check if this is a variable on the stack or in static memory */
    if (IS_Get (&StaticLocals) == 0) {

        /* Add the symbol to the symbol table. The stack offset we use here
        ** may get corrected later.
        */
        Sym = AddLocalSym (Decl->Ident, Decl->Type,
                           Decl->StorageClass,
                           F_GetStackPtr (CurrentFunc) - (int) Size);

        /* Check for an optional initialization */
        if (CurTok.Tok == TOK_ASSIGN) {

            /* Skip the '=' */
            NextToken ();

            /* Special handling for compound types */
            if (IsCompound) {

                /* Switch to read only data and define a label for the
                ** initialization data.
                */
                unsigned InitLabel = AllocLabel (g_userodata);

                /* Parse the initialization generating a memory image of the
                ** data in the RODATA segment. The function will return the
                ** actual size of the initialization data, which may be
                ** greater than the size of the variable if it is a struct
                ** that contains a flexible array member and we're not in
                ** ANSI mode.
                */
                Size = ParseInit (Sym->Type);

                /* Now reserve space for the variable on the stack and correct
                ** the offset in the symbol table entry.
                */
                Sym->V.Offs = F_ReserveLocalSpace (CurrentFunc, Size);

                /* Next, allocate the space on the stack. This means that the
                ** variable is now located at offset 0 from the current sp.
                */
                F_AllocLocalSpace (CurrentFunc);

                /* Generate code to copy the initialization data into the
                ** variable space
                */
                g_initauto (InitLabel, Size);

            } else {

                ExprDesc Expr;
                ED_Init (&Expr);

                /* Allocate previously reserved local space */
                F_AllocLocalSpace (CurrentFunc);

                /* Setup the type flags for the assignment */
                Flags = (Size == SIZEOF_CHAR)? CF_FORCECHAR : CF_NONE;

                /* Parse the expression */
                hie1 (&Expr);

                /* Convert it to the target type */
                TypeConversion (&Expr, Sym->Type);

                /* If the value is not const, load it into the primary.
                ** Otherwise pass the information to the code generator.
                */
                if (ED_IsConstAbsInt (&Expr)) {
                    Flags |= CF_CONST;
                } else {
                    LoadExpr (CF_NONE, &Expr);
                    ED_MarkExprAsRVal (&Expr);
                }

                /* Push the value */
                g_push (Flags | CG_TypeOf (Sym->Type), Expr.IVal);

                /* This has to be done at sequence point */
                DoDeferred (SQP_KEEP_NONE, &Expr);
            }

            /* Mark the variable as referenced */
            Sym->Flags |= SC_REF;

            /* Make note of auto variables initialized in current block.
            ** We abuse the Collection somewhat by using it to store line
            ** numbers.
            */
            CollReplace (&CurrentFunc->LocalsBlockStack, (void *)(size_t)GetCurrentLineNum (),
                CollCount (&CurrentFunc->LocalsBlockStack) - 1);

        } else {
            /* Non-initialized local variable. Just keep track of
            ** the space needed.
            */
            F_ReserveLocalSpace (CurrentFunc, Size);
        }

    } else {

        unsigned DataLabel;


        /* Static local variables. */
        Decl->StorageClass = (Decl->StorageClass & ~SC_STORAGEMASK) | SC_STATIC;

        /* Generate a label, but don't define it */
        DataLabel = GetLocalDataLabel ();

        /* Add the symbol to the symbol table. */
        Sym = AddLocalSym (Decl->Ident, Decl->Type, Decl->StorageClass, DataLabel);

        /* Allow assignments */
        if (CurTok.Tok == TOK_ASSIGN) {

            /* Skip the '=' */
            NextToken ();

            if (IsCompound) {

                /* Switch to read only data and define a label for the
                ** initialization data.
                */
                unsigned InitLabel = AllocLabel (g_userodata);

                /* Parse the initialization generating a memory image of the
                ** data in the RODATA segment.
                */
                Size = ParseInit (Sym->Type);

                /* Allocate space for the variable */
                AllocStorage (DataLabel, g_usebss, Size);

                /* Generate code to copy this data into the variable space */
                g_initstatic (InitLabel, DataLabel, Size);

            } else {

                ExprDesc Expr;
                ED_Init (&Expr);

                /* Allocate space for the variable */
                AllocStorage (DataLabel, g_usebss, Size);

                /* Parse the expression */
                hie1 (&Expr);

                /* Convert it to the target type */
                TypeConversion (&Expr, Sym->Type);

                /* Load the value into the primary */
                LoadExpr (CF_NONE, &Expr);

                /* Store the value into the variable */
                g_putstatic (CF_STATIC | CG_TypeOf (Sym->Type), DataLabel, 0);

                /* This has to be done at sequence point */
                DoDeferred (SQP_KEEP_NONE, &Expr);
            }

            /* Mark the variable as referenced */
            Sym->Flags |= SC_REF;

        } else {

            /* No assignment - allocate a label and space for the variable */
            AllocStorage (DataLabel, g_usebss, Size);

        }
    }

    /* Cannot allocate an incomplete variable */
    if (HasUnknownSize (Sym->Type)) {
        if (IsTypeArray (Decl->Type)) {
            Error ("Array '%s' has unknown size", Decl->Ident);
        } else {
            Error ("Variable '%s' has unknown size", Decl->Ident);
        }
    }
}



static void ParseStaticDecl (Declarator* Decl)
/* Parse the declarator of a static variable. */
{
    unsigned Size;

    /* Generate a label, but don't define it */
    unsigned DataLabel = GetLocalDataLabel ();

    /* Add the symbol to the symbol table. */
    SymEntry* Sym = AddLocalSym (Decl->Ident, Decl->Type,
                                 Decl->StorageClass,
                                 DataLabel);

    /* Static data */
    if (CurTok.Tok == TOK_ASSIGN) {

        /* Initialization ahead, switch to data segment and define the label.
        ** For arrays, we need to check the elements of the array for
        ** constness, not the array itself.
        */
        if (IsQualConst (GetBaseElementType (Sym->Type))) {
            g_userodata ();
        } else {
            g_usedata ();
        }
        g_defdatalabel (DataLabel);

        /* Skip the '=' */
        NextToken ();

        /* Allow initialization of static vars */
        Size = ParseInit (Sym->Type);

        /* Mark the variable as referenced */
        Sym->Flags |= SC_REF;

    } else {

        /* Get the size of the variable */
        Size = SizeOf (Decl->Type);

        /* Allocate a label and space for the variable in the BSS segment */
        AllocStorage (DataLabel, g_usebss, Size);

    }

    /* Cannot allocate an incomplete variable */
    if (HasUnknownSize (Sym->Type)) {
        if (IsTypeArray (Decl->Type)) {
            Error ("Array '%s' has unknown size", Decl->Ident);
        } else {
            Error ("Variable '%s' has unknown size", Decl->Ident);
        }
    }
}



static int ParseOneDecl (DeclSpec* Spec)
/* Parse one variable declarator. */
{
    Declarator  Decl;           /* Declarator data structure */
    int         NeedClean;


    /* Read the declarator */
    NeedClean = ParseDecl (Spec, &Decl, DM_IDENT_OR_EMPTY);

    /* Check if there are explicitly specified non-external storage classes
    ** for function declarations.
    */
    if ((Decl.StorageClass & SC_TYPEMASK) == SC_FUNC) {
        /* Function can only be declared inside functions with the 'extern'
        ** storage class specifier or no storage class specifier at all.
        ** Note: this declaration is always checked for compatibility with
        ** other declarations of the same symbol, but does not necessarily
        ** make the symbol globally visible. This is tricky.
        */
        if ((Spec->Flags & DS_DEF_STORAGE) != DS_DEF_STORAGE    &&
            (Decl.StorageClass & SC_STORAGEMASK) != SC_EXTERN   &&
            (Decl.StorageClass & SC_STORAGEMASK) != 0) {
            Error ("Illegal storage class on function");
        }

        /* The default storage class could be wrong. Just clear them */
        Decl.StorageClass &= ~SC_STORAGEMASK;
    } else if ((Decl.StorageClass & SC_STORAGEMASK) != SC_EXTERN) {
        /* If the symbol is not marked as external, it will be defined now */
        Decl.StorageClass |= SC_DEF;
    }

    /* If we don't have a name, this was flagged as an error earlier.
    ** To avoid problems later, use an anonymous name here.
    */
    if (Decl.Ident[0] == '\0') {
        AnonName (Decl.Ident, "param");
    }

    /* Handle anything that needs storage (no functions, no typdefs) */
    if ((Decl.StorageClass & SC_DEF) == SC_DEF &&
        (Decl.StorageClass & SC_TYPEMASK) != SC_TYPEDEF) {

        /* If we have a register variable, try to allocate a register and
        ** convert the declaration to "auto" if this is not possible.
        */
        int Reg = 0;    /* Initialize to avoid gcc complains */
        if ((Decl.StorageClass & SC_STORAGEMASK) == SC_REGISTER &&
            (Reg = F_AllocRegVar (CurrentFunc, Decl.Type)) < 0) {
            /* No space for this register variable, convert to auto */
            Decl.StorageClass = (Decl.StorageClass & ~SC_STORAGEMASK) | SC_AUTO;
        }

        /* Check the variable type */
        if ((Decl.StorageClass & SC_STORAGEMASK) == SC_REGISTER) {
            /* Register variable */
            ParseRegisterDecl (&Decl, Reg);
        } else if ((Decl.StorageClass & SC_STORAGEMASK) == SC_AUTO) {
            /* Auto variable */
            ParseAutoDecl (&Decl);
        } else if ((Decl.StorageClass & SC_STORAGEMASK) == SC_STATIC) {
            /* Static variable */
            ParseStaticDecl (&Decl);
        } else {
            Internal ("Invalid storage class in ParseOneDecl: %04X", Decl.StorageClass);
        }

    } else {

        if ((Decl.StorageClass & SC_STORAGEMASK) == SC_EXTERN) {
            /* External identifier - may not get initialized */
            if (CurTok.Tok == TOK_ASSIGN) {
                Error ("Cannot initialize extern variable '%s'", Decl.Ident);
                /* Avoid excess errors */
                NextToken ();
                ParseInit (Decl.Type);
            }
        }

        if ((Decl.StorageClass & SC_STORAGEMASK) == SC_EXTERN ||
            (Decl.StorageClass & SC_TYPEMASK) == SC_FUNC) {
            /* Add the global symbol to both of the global and local symbol
            ** tables.
            */
            AddGlobalSym (Decl.Ident, Decl.Type, Decl.StorageClass);
        } else {
            /* Add the local symbol to the local symbol table */
            AddLocalSym (Decl.Ident, Decl.Type, Decl.StorageClass, 0);
        }

    }

    /* Make sure we aren't missing some work */
    CheckDeferredOpAllDone ();

    return NeedClean;
}



void DeclareLocals (void)
/* Declare local variables and types. */
{
    /* Remember the current stack pointer */
    int InitialStack = StackPtr;

    /* A place to store info about potential initializations of auto variables */
    CollAppend (&CurrentFunc->LocalsBlockStack, 0);

    /* Loop until we don't find any more variables. EOF is handled in the loop
    ** as well.
    */
    while (1) {
        DeclSpec Spec;
        int      NeedClean;

        /* Check for a _Static_assert */
        if (CurTok.Tok == TOK_STATIC_ASSERT) {
            ParseStaticAssert ();
            continue;
        }

        /* Read the declaration specifier */
        ParseDeclSpec (&Spec, TS_DEFAULT_TYPE_INT | TS_FUNCTION_SPEC, SC_AUTO);

        /* Check variable declarations. We need distinguish between a default
        ** int type and the end of variable declarations. So we will do the
        ** following: If there is no explicit storage class specifier *and* no
        ** explicit type given, *and* no type qualifiers have been read, it is
        ** assumed that we have reached the end of declarations.
        */
        if ((Spec.Flags & DS_DEF_STORAGE) != 0          &&  /* No storage spec */
            (Spec.Flags & DS_TYPE_MASK) == DS_DEF_TYPE  &&  /* No type given */
            GetQualifier (Spec.Type) == T_QUAL_NONE) {      /* No type qualifier */
            break;
        }

        /* Accept type only declarations */
        if (CurTok.Tok == TOK_SEMI) {
            /* Type declaration only */
            CheckEmptyDecl (&Spec);
            NextToken ();
            continue;
        }

        /* If we haven't got a type specifier yet, something must be wrong */
        if ((Spec.Flags & DS_TYPE_MASK) == DS_NONE) {
            /* Avoid extra errors if it was a failed type specifier */
            if ((Spec.Flags & DS_EXTRA_TYPE) == 0) {
                Error ("Declaration specifier expected");
            }
            NeedClean = -1;
            goto EndOfDecl;
        }

        /* Parse a comma separated variable list */
        while (1) {

            /* Parse one declarator */
            NeedClean = ParseOneDecl (&Spec);
            if (NeedClean <= 0) {
                break;
            }

            /* Check if there is more */
            if (CurTok.Tok == TOK_COMMA) {
                /* More to come */
                NextToken ();
            } else {
                /* Done */
                break;
            }
        }

        if (NeedClean > 0) {
            /* Must be followed by a semicolon */
            if (ConsumeSemi ()) {
                NeedClean = 0;
            } else {
                NeedClean = -1;
            }
        }

EndOfDecl:
        /* Try some smart error recovery */
        if (NeedClean < 0) {
            SmartErrorSkip (1);
        }
    }

    /* Be sure to allocate any reserved space for locals */
    F_AllocLocalSpace (CurrentFunc);

    /* No auto variables were inited. No new block on the stack then. */
    if (CollLast (&CurrentFunc->LocalsBlockStack) == NULL) {
        CollPop (&CurrentFunc->LocalsBlockStack);
    }

    /* In case we've allocated local variables in this block, emit a call to
    ** the stack checking routine if stack checks are enabled.
    */
    if (IS_Get (&CheckStack) && InitialStack != StackPtr) {
        g_cstackcheck ();
    }
}
