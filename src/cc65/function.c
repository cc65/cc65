/*****************************************************************************/
/*                                                                           */
/*				  function.c				     */
/*                                                                           */
/*			Parse function entry/body/exit			     */
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
#include "stmt.h"
#include "symtab.h"
#include "function.h"



/*****************************************************************************/
/*	     	    		     Data			 	     */
/*****************************************************************************/



/* Structure that holds all data needed for function activation */
struct Function {
    struct SymEntry*   	FuncEntry;	/* Symbol table entry */
    type*   	  	ReturnType;	/* Function return type */
    struct FuncDesc*	Desc;	  	/* Function descriptor */
    CodeMark 	       	EntryCode;     	/* Backpatch addr for entry code */
    int			Reserved;	/* Reserved local space */
    unsigned	  	RetLab;	    	/* Return code label */
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
    F->ReturnType = Sym->Type + 1 + DECODE_SIZE;
    F->Desc   	  = (FuncDesc*) DecodePtr (Sym->Type + 1);
    F->EntryCode  = 0;
    F->Reserved	  = 0;
    F->RetLab	  = GetLabel ();

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
    return (F->Desc->Flags & FD_ELLIPSIS) != 0;
}



void RememberEntry (Function* F)
/* Remember the current output position for local space creation later */
{
    F->EntryCode = GetCodePos ();
}



unsigned GetRetLab (const Function* F)
/* Return the return jump label */
{
    return F->RetLab;
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

	/* Switch to the code segment */
	g_usecode ();

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
    int isbrk;
    unsigned Flags;

    /* Get the function descriptor from the function entry */
    FuncDesc* D = (FuncDesc*) DecodePtr (Func->Type+1);

    /* Allocate the function activation record for the function */
    CurrentFunc = NewFunction (Func);

    /* Reenter the lexical level */
    ReenterFunctionLevel (D);

    /* Declare two special functions symbols: __fixargs__ and __argsize__.
     * The latter is different depending on the type of the function (variadic
     * or not).
     */
    AddLocalSym ("__fixargs__", type_uint, SC_DEF | SC_CONST, D->ParamSize);
    if (D->Flags & FD_ELLIPSIS) {
	/* Variadic function. The variable must be const. */
	static const type T [] = { T_UCHAR | T_QUAL_CONST, T_END };
	AddLocalSym ("__argsize__", T, SC_DEF | SC_REF | SC_AUTO, 0);
    } else {
	/* Non variadic */
	AddLocalSym ("__argsize__", type_uchar, SC_DEF | SC_CONST, D->ParamSize);
    }

    /* Function body now defined */
    Func->Flags |= SC_DEF;

    /* Need a starting curly brace */
    if (curtok != TOK_LCURLY) {
    	Error ("`{' expected");
    }

    /* Setup register variables */
    InitRegVars ();

    /* Switch to the code segment and define the function name label */
    g_usecode ();
    g_defgloblabel (Func->Name);

    /* If this is a fastcall function, push the last parameter onto the stack */
    if (IsFastCallFunc (Func->Type) && D->ParamCount > 0) {

	SymEntry* LastParam;
	unsigned Flags;

	/* Fastcall functions may never have an ellipsis or the compiler is buggy */
	CHECK ((D->Flags & FD_ELLIPSIS) == 0);

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

    /* Remember the current code position. This may be used later to create
     * local variable space once we have created the function body itself.
     * Currently this is not possible because the stack offsets of all locals
     * have to be known in advance.
     */
    RememberEntry (CurrentFunc);

    /* Parse the function body */
    oursp = 0;
    isbrk = compound ();

    /* If the function did not end with an return statement, create exit code */
    if (!isbrk) {
#if 0
     	/* If the function has a return type, flag an error */
     	if (!voidfunc) {
     	    Error ("Function `%s' must return a value",	Func->Name);
     	}
#endif
	RestoreRegVars (0);

	Flags = HasVoidReturn (CurrentFunc)? CF_NONE : CF_REG;
        g_leave (Flags, 0);
    }

    /* Dump literal data created by the function */
    DumpLiteralPool ();

    /* Cleanup register variables */
    DoneRegVars ();

    /* Leave the lexical level */
    LeaveFunctionLevel ();

    /* Reset the current function pointer */
    FreeFunction (CurrentFunc);
    CurrentFunc = 0;
}




