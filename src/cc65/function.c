/*****************************************************************************/
/*                                                                           */
/*				  function.c				     */
/*                                                                           */
/*			Parse function entry/body/exit			     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000-2001 Ullrich von Bassewitz                                       */
/*               Wacholderweg 14                                             */
/*               D-70597 Stuttgart                                           */
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
    struct SymEntry*   	FuncEntry;	/* Symbol table entry */
    type*		ReturnType;	/* Function return type */
    struct FuncDesc*	Desc;	  	/* Function descriptor */
    int			Reserved;	/* Reserved local space */
    unsigned	  	RetLab;	    	/* Return code label */
    int			TopLevelSP;	/* SP at function top level */
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

    /* Return the new structure */
    return F;
}



static void FreeFunction (Function* F)
/* Free a function activation structure */
{
    xfree (F);
}



const char* GetFuncName (const Function* F)
/* Return the name of the current function */
{
    return F->FuncEntry->Name;
}



unsigned GetParamCount (const Function* F)
/* Return the parameter count for the current function */
{
    return F->Desc->ParamCount;
}



unsigned GetParamSize (const Function* F)
/* Return the parameter size for the current function */
{
    return F->Desc->ParamSize;
}



type* GetReturnType (Function* F)
/* Get the return type for the function */
{
    return F->ReturnType;
}



int HasVoidReturn (const Function* F)
/* Return true if the function does not have a return value */
{
    return IsTypeVoid (F->ReturnType);
}



int IsVariadic (const Function* F)
/* Return true if this is a variadic function */
{
    return (F->Desc->Flags & FD_VARIADIC) != 0;
}



unsigned GetRetLab (const Function* F)
/* Return the return jump label */
{
    return F->RetLab;
}



int GetTopLevelSP (const Function* F)
/* Get the value of the stack pointer on function top level */
{
    return F->TopLevelSP;
}



int ReserveLocalSpace (Function* F, unsigned Size)
/* Reserve (but don't allocate) the given local space and return the stack
 * offset.
 */
{
    F->Reserved += Size;
    return oursp - F->Reserved;
}



void AllocLocalSpace (Function* F)
/* Allocate any local space previously reserved. The function will do
 * nothing if there is no reserved local space.
 */
{
    if (F->Reserved > 0) {

	/* Create space on the stack */
	g_space (F->Reserved);

	/* Correct the stack pointer */
	oursp -= F->Reserved;

	/* Nothing more reserved */
	F->Reserved = 0;
    }
}



/*****************************************************************************/
/*     	      	  	    	     code	      		 	     */
/*****************************************************************************/



void NewFunc (SymEntry* Func)
/* Parse argument declarations and function body. */
{
    int HadReturn;
    int IsVoidFunc;

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

    /* Setup register variables */
    InitRegVars ();

    /* Allocate code and data segments for this function */
    Func->V.F.Seg = PushSegments (Func);

    /* If this is a fastcall function, push the last parameter onto the stack */
    if (IsFastCallFunc (Func->Type) && D->ParamCount > 0) {

	SymEntry* LastParam;
	unsigned Flags;

	/* Fastcall functions may never have an ellipsis or the compiler is buggy */
	CHECK ((D->Flags & FD_VARIADIC) == 0);

	/* Get a pointer to the last parameter entry */
	LastParam = D->SymTab->SymTail;

	/* Generate the push */
	if (IsTypeFunc (LastParam->Type)) {
	    /* Pointer to function */
	    Flags = CF_PTR;
	} else {
	    Flags = TypeOf (LastParam->Type) | CF_FORCECHAR;
	}
	g_push (Flags, 0);
    }

    /* If stack checking code is requested, emit a call to the helper routine */
    if (CheckStack) {
	g_stackcheck ();
    }

    /* Generate function entry code if needed */
    g_enter (TypeOf (Func->Type), GetParamSize (CurrentFunc));

    /* Setup the stack */
    oursp = 0;

    /* Need a starting curly brace */
    ConsumeLCurly ();

    /* Parse local variable declarations if any */
    DeclareLocals ();

    /* Remember the current stack pointer. All variables allocated elsewhere
     * must be dropped when doing a return from an inner block.
     */
    CurrentFunc->TopLevelSP = oursp;

    /* Now process statements in this block */
    HadReturn = 0;
    while (CurTok.Tok != TOK_RCURLY) {
     	if (CurTok.Tok != TOK_CEOF) {
     	    HadReturn = Statement (0);
     	} else {
     	    break;
     	}
    }

    /* If the function has a return type but no return statement, flag
     * a warning
     */
    IsVoidFunc = HasVoidReturn (CurrentFunc);
#if 0
    /* Does not work reliably */
    if (!IsVoidFunc && !HadReturn) {
	Warning ("Function `%s' should return a value", Func->Name);
    }
#endif

    /* Output the function exit code label */
    g_defcodelabel (GetRetLab (CurrentFunc));

    /* Restore the register variables */
    RestoreRegVars (!IsVoidFunc);

    /* Generate the exit code */
    g_leave ();

    /* Eat the closing brace */
    ConsumeRCurly ();

    /* Emit references to imports/exports */
    EmitExternals ();

    /* Cleanup register variables */
    DoneRegVars ();

    /* Leave the lexical level */
    LeaveFunctionLevel ();

    /* Switch back to the old segments */
    PopSegments ();

    /* Reset the current function pointer */
    FreeFunction (CurrentFunc);
    CurrentFunc = 0;
}



