/*****************************************************************************/
/*                                                                           */
/*				  function.c				     */
/*                                                                           */
/*			Parse function entry/body/exit			     */
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



/* common */
#include "check.h"
#include "xmalloc.h"

/* cc65 */
#include "asmcode.h"
#include "asmlabel.h"
#include "codegen.h"
#include "error.h"
#include "funcdesc.h"
#include "global.h"
#include "litpool.h"
#include "locals.h"
#include "scanner.h"
#include "segments.h"
#include "stmt.h"
#include "symtab.h"
#include "function.h"



/*****************************************************************************/
/*	     	    		     Data			 	     */
/*****************************************************************************/



/* Structure that holds all data needed for function activation */
struct Function {
    struct SymEntry*   	FuncEntry;  	/* Symbol table entry */
    type*		ReturnType; 	/* Function return type */
    struct FuncDesc*	Desc;	    	/* Function descriptor */
    int			Reserved;   	/* Reserved local space */
    unsigned	  	RetLab;	    	/* Return code label */
    int			TopLevelSP;	/* SP at function top level */
    unsigned            RegOffs;        /* Register variable space offset */
};

/* Pointer to current function */
Function* CurrentFunc = 0;



/*****************************************************************************/
/*		   Subroutines working with struct Function		     */
/*****************************************************************************/



static Function* NewFunction (struct SymEntry* Sym)
/* Create a new function activation structure and return it */
{
    /* Allocate a new structure */
    Function* F = (Function*) xmalloc (sizeof (Function));

    /* Initialize the fields */
    F->FuncEntry  = Sym;
    F->ReturnType = GetFuncReturn (Sym->Type);
    F->Desc   	  = (FuncDesc*) DecodePtr (Sym->Type + 1);
    F->Reserved	  = 0;
    F->RetLab	  = GetLocalLabel ();
    F->TopLevelSP = 0;
    F->RegOffs    = RegisterSpace;

    /* Return the new structure */
    return F;
}



static void FreeFunction (Function* F)
/* Free a function activation structure */
{
    xfree (F);
}



const char* F_GetFuncName (const Function* F)
/* Return the name of the current function */
{
    return F->FuncEntry->Name;
}



unsigned F_GetParamCount (const Function* F)
/* Return the parameter count for the current function */
{
    return F->Desc->ParamCount;
}



unsigned F_GetParamSize (const Function* F)
/* Return the parameter size for the current function */
{
    return F->Desc->ParamSize;
}



type* F_GetReturnType (Function* F)
/* Get the return type for the function */
{
    return F->ReturnType;
}



int F_HasVoidReturn (const Function* F)
/* Return true if the function does not have a return value */
{
    return IsTypeVoid (F->ReturnType);
}



int F_IsVariadic (const Function* F)
/* Return true if this is a variadic function */
{
    return (F->Desc->Flags & FD_VARIADIC) != 0;
}



int F_IsOldStyle (const Function* F)
/* Return true if this is an old style (K&R) function */
{
    return (F->Desc->Flags & FD_OLDSTYLE) != 0;
}



int F_HasOldStyleIntRet (const Function* F)
/* Return true if this is an old style (K&R) function with an implicit int return */
{
    return (F->Desc->Flags & FD_OLDSTYLE_INTRET) != 0;
}



unsigned F_GetRetLab (const Function* F)
/* Return the return jump label */
{
    return F->RetLab;
}



int F_GetTopLevelSP (const Function* F)
/* Get the value of the stack pointer on function top level */
{
    return F->TopLevelSP;
}



int F_ReserveLocalSpace (Function* F, unsigned Size)
/* Reserve (but don't allocate) the given local space and return the stack
 * offset.
 */
{
    F->Reserved += Size;
    return StackPtr - F->Reserved;
}



void F_AllocLocalSpace (Function* F)
/* Allocate any local space previously reserved. The function will do
 * nothing if there is no reserved local space.
 */
{
    if (F->Reserved > 0) {

       	/* Create space on the stack */
       	g_space (F->Reserved);

       	/* Correct the stack pointer */
       	StackPtr -= F->Reserved;

       	/* Nothing more reserved */
       	F->Reserved = 0;
    }
}



int F_AllocRegVar (Function* F, const type* Type)
/* Allocate a register variable for the given variable type. If the allocation
 * was successful, return the offset of the register variable in the register
 * bank (zero page storage). If there is no register space left, return -1.
 */
{
    /* Allow register variables only on top level and if enabled */
    if (IS_Get (&EnableRegVars) && GetLexicalLevel () == LEX_LEVEL_FUNCTION) {

      	/* Get the size of the variable */
      	unsigned Size = CheckedSizeOf (Type);

      	/* Do we have space left? */
      	if (F->RegOffs >= Size) {
      	    /* Space left. We allocate the variables from high to low addresses,
      	     * so the adressing is compatible with the saved values on stack.
      	     * This allows shorter code when saving/restoring the variables.
      	     */
      	    F->RegOffs -= Size;
      	    return F->RegOffs;
      	}
    }

    /* No space left or no allocation */
    return -1;
}



static void F_RestoreRegVars (Function* F)
/* Restore the register variables for the local function if there are any. */
{
    const SymEntry* Sym;

    /* If we don't have register variables in this function, bail out early */
    if (F->RegOffs == RegisterSpace) {
       	return;
    }

    /* Save the accumulator if needed */
    if (!F_HasVoidReturn (F)) {
       	g_save (CF_CHAR | CF_FORCECHAR);
    }

    /* Get the first symbol from the function symbol table */
    Sym = F->FuncEntry->V.F.Func->SymTab->SymHead;

    /* Walk through all symbols checking for register variables */
    while (Sym) {
        if (SymIsRegVar (Sym)) {

            /* Check for more than one variable */
            int Offs       = Sym->V.R.SaveOffs;
            unsigned Bytes = CheckedSizeOf (Sym->Type);

            while (1) {

                /* Find next register variable */
                const SymEntry* NextSym = Sym->NextSym;
                while (NextSym && !SymIsRegVar (NextSym)) {
                    NextSym = NextSym->NextSym;
                }

                /* If we have a next one, compare the stack offsets */
                if (NextSym) {

                    /* We have a following register variable. Get the size */
                    int Size = CheckedSizeOf (NextSym->Type);

                    /* Adjacent variable? */
                    if (NextSym->V.R.SaveOffs + Size != Offs) {
                        /* No */
                        break;
                    }

                    /* Adjacent variable */
                    Bytes += Size;
                    Offs  -= Size;
                    Sym   = NextSym;

                } else {
                    break;
                }
            }

            /* Restore the memory range */
            g_restore_regvars (Offs, Sym->V.R.RegOffs, Bytes);

        }

        /* Check next symbol */
        Sym = Sym->NextSym;
    }

    /* Restore the accumulator if needed */
    if (!F_HasVoidReturn (F)) {
     	g_restore (CF_CHAR | CF_FORCECHAR);
    }
}



/*****************************************************************************/
/*     	      	  	    	     code	      		 	     */
/*****************************************************************************/



void NewFunc (SymEntry* Func)
/* Parse argument declarations and function body. */
{
    int HadReturn;
    SymEntry* Param;

    /* Get the function descriptor from the function entry */
    FuncDesc* D = Func->V.F.Func;

    /* Allocate the function activation record for the function */
    CurrentFunc = NewFunction (Func);

    /* Reenter the lexical level */
    ReenterFunctionLevel (D);

    /* Declare two special functions symbols: __fixargs__ and __argsize__.
     * The latter is different depending on the type of the function (variadic
     * or not).
     */
    AddConstSym ("__fixargs__", type_uint, SC_DEF | SC_CONST, D->ParamSize);
    if (D->Flags & FD_VARIADIC) {
     	/* Variadic function. The variable must be const. */
     	static const type T [] = { T_UCHAR | T_QUAL_CONST, T_END };
     	AddLocalSym ("__argsize__", T, SC_DEF | SC_REF | SC_AUTO, 0);
    } else {
     	/* Non variadic */
       	AddConstSym ("__argsize__", type_uchar, SC_DEF | SC_CONST, D->ParamSize);
    }

    /* Function body now defined */
    Func->Flags |= SC_DEF;

    /* Allocate code and data segments for this function */
    Func->V.F.Seg = PushSegments (Func);

    /* Special handling for main() */
    if (strcmp (Func->Name, "main") == 0) {
        /* Main cannot be a fastcall function */
        if (IsFastCallFunc (Func->Type)) {
            Error ("`main' cannot be declared as __fastcall__");
        }

        /* If main() takes parameters, generate a forced import to a function
         * that will setup these parameters. This way, programs that do not
         * need the additional code will not get it.
         */
        if (D->ParamCount > 0 || (D->Flags & FD_VARIADIC) != 0) {
            g_importmainargs ();
        }
    }

    /* If this is a fastcall function, push the last parameter onto the stack */
    if (IsFastCallFunc (Func->Type) && D->ParamCount > 0) {

     	unsigned Flags;

     	/* Fastcall functions may never have an ellipsis or the compiler is buggy */
     	CHECK ((D->Flags & FD_VARIADIC) == 0);

     	/* Generate the push */
     	if (IsTypeFunc (D->LastParam->Type)) {
	    /* Pointer to function */
	    Flags = CF_PTR;
	} else {
    	    Flags = TypeOf (D->LastParam->Type) | CF_FORCECHAR;
	}
	g_push (Flags, 0);
    }

    /* Generate function entry code if needed */
    g_enter (TypeOf (Func->Type), F_GetParamSize (CurrentFunc));

    /* If stack checking code is requested, emit a call to the helper routine */
    if (IS_Get (&CheckStack)) {
    	g_stackcheck ();
    }

    /* Setup the stack */
    StackPtr = 0;

    /* Walk through the parameter list and allocate register variable space
     * for parameters declared as register. Generate code to swap the contents
     * of the register bank with the save area on the stack.
     */
    Param = D->SymTab->SymHead;
    while (Param && (Param->Flags & SC_PARAM) != 0) {

        /* Check for a register variable */
        if (SymIsRegVar (Param)) {

            /* Allocate space */
            int Reg = F_AllocRegVar (CurrentFunc, Param->Type);

            /* Could we allocate a register? */
            if (Reg < 0) {
                /* No register available: Convert parameter to auto */
                CvtRegVarToAuto (Param);
            } else {
                /* Remember the register offset */
                Param->V.R.RegOffs = Reg;

                /* Generate swap code */
                g_swap_regvars (Param->V.R.SaveOffs, Reg, CheckedSizeOf (Param->Type));

            }
        }

        /* Next parameter */
        Param = Param->NextSym;
    }

    /* Need a starting curly brace */
    ConsumeLCurly ();

    /* Parse local variable declarations if any */
    DeclareLocals ();

    /* Remember the current stack pointer. All variables allocated elsewhere
     * must be dropped when doing a return from an inner block.
     */
    CurrentFunc->TopLevelSP = StackPtr;

    /* Now process statements in this block */
    HadReturn = 0;
    while (CurTok.Tok != TOK_RCURLY) {
     	if (CurTok.Tok != TOK_CEOF) {
     	    HadReturn = Statement (0);
     	} else {
     	    break;
     	}
    }

    /* Output the function exit code label */
    g_defcodelabel (F_GetRetLab (CurrentFunc));

    /* Restore the register variables */
    F_RestoreRegVars (CurrentFunc);

    /* Generate the exit code */
    g_leave ();

    /* Emit references to imports/exports */
    EmitExternals ();

    /* Leave the lexical level */
    LeaveFunctionLevel ();

    /* Eat the closing brace */
    ConsumeRCurly ();

    /* Switch back to the old segments */
    PopSegments ();

    /* Reset the current function pointer */
    FreeFunction (CurrentFunc);
    CurrentFunc = 0;
}



