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



#include "asmcode.h"
#include "asmlabel.h"
#include "codegen.h"
#include "error.h"
#include "funcdesc.h"
#include "litpool.h"
#include "locals.h"
#include "mem.h"
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
    unsigned	  	LocalMax; 	/* Total space for locals */
    unsigned   	       	LocalSize;	/* Current space for locals */
    unsigned	  	RetLab;		/* Return code label */
};

/* Pointer to current function */
Function* CurrentFunc = 0;



/*****************************************************************************/
/*     	      	 	    	     code	      		 	     */
/*****************************************************************************/



static Function* NewFunction (struct SymEntry* Sym)
/* Create a new function activation structure and return it */
{
    /* Allocate a new structure */
    Function* F = xmalloc (sizeof (Function));

    /* Initialize the fields */
    F->FuncEntry  = Sym;
    F->ReturnType = Sym->Type + 1 + DECODE_SIZE;
    F->Desc   	  = DecodePtr (Sym->Type + 1);
    F->EntryCode  = GetCodePos ();
    F->LocalMax	  = 0;
    F->LocalSize  = 0;
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
    return IsVoid (F->ReturnType);
}



unsigned GetRetLab (const Function* F)
/* Return the return jump label */
{
    return F->RetLab;
}



unsigned AllocLocalSpace (Function* F, unsigned Size)
/* Allocate space for the function locals, return stack offset */
{
    /* Remember the current offset */
    unsigned Offs = F->LocalSize;

    /* Add the size */
    F->LocalSize += Size;
    if (F->LocalSize > F->LocalMax) {
      	F->LocalMax = F->LocalSize;
    }

    /* Return the offset */
    return Offs;
}



void FreeLocalSpace (Function* F, unsigned Size)
/* Free space allocated for function locals */
{
    F->LocalSize -= Size;
}



void NewFunc (SymEntry* Func)
/* Parse argument declarations and function body. */
{
    int isbrk;

    /* Get the function descriptor from the function entry */
    FuncDesc* D = DecodePtr (Func->Type+1);

    /* Allocate the function activation record for the function */
    CurrentFunc = NewFunction (Func);

    /* Reenter the lexical level */
    ReenterFunctionLevel (D);

    /* Function body now defined */
    Func->Flags |= SC_DEF;

    /* C functions cannot currently have __fastcall__ calling conventions */
    if (IsFastCallFunc (Func->Type)) {
	Error (ERR_FASTCALL);
    }

    /* Need a starting curly brace */
    if (curtok != LCURLY) {
    	Error (ERR_LCURLY_EXPECTED);
    }

    /* Setup register variables */
    InitRegVars ();

    /* Switch to the code segment and generate function entry code */
    g_usecode ();
    g_enter (TypeOf (Func->Type), Func->Name, GetParamSize (CurrentFunc));

    /* Parse the function body */
    oursp = 0;
    isbrk = compound ();

    /* If the function did not end with an return statement, create exit code */
    if (!isbrk) {
#if 0
     	/* If the function has a return type, flag an error */
     	if (!voidfunc) {
     	    Error (ERR_MUST_RETURN_VALUE);
     	}
#endif
	RestoreRegVars (0);
        g_leave (CF_NONE, 0);
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



