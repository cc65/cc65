/*****************************************************************************/
/*                                                                           */
/*                                function.c                                 */
/*                                                                           */
/*                      Parse function entry/body/exit                       */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000-2015, Ullrich von Bassewitz                                      */
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
#include "check.h"
#include "xmalloc.h"

/* cc65 */
#include "asmcode.h"
#include "asmlabel.h"
#include "codegen.h"
#include "error.h"
#include "expr.h"
#include "funcdesc.h"
#include "global.h"
#include "litpool.h"
#include "locals.h"
#include "scanner.h"
#include "stackptr.h"
#include "standard.h"
#include "stmt.h"
#include "symtab.h"
#include "function.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Pointer to current function */
Function* CurrentFunc = 0;



/*****************************************************************************/
/*                 Subroutines working with struct Function                  */
/*****************************************************************************/



static Function* NewFunction (struct SymEntry* Sym, FuncDesc* D)
/* Create a new function activation structure and return it */
{
    /* Allocate a new structure */
    Function* F = (Function*) xmalloc (sizeof (Function));

    /* Initialize the fields */
    F->FuncEntry  = Sym;
    F->ReturnType = GetFuncReturnType (Sym->Type);
    F->Desc       = D;
    F->Reserved   = 0;
    F->RetLab     = 0;
    F->TopLevelSP = 0;
    F->RegOffs    = RegisterSpace;
    F->Flags      = IsTypeVoid (F->ReturnType) ? FF_VOID_RETURN : FF_NONE;

    InitCollection (&F->LocalsBlockStack);

    /* Return the new structure */
    return F;
}



static void FreeFunction (Function* F)
/* Free a function activation structure */
{
    DoneCollection (&F->LocalsBlockStack);
    xfree (F);
}



int F_CheckParamList (FuncDesc* D, int RequireAll)
/* Check and set the parameter sizes.
** If RequireAll is true, emit errors on parameters of incomplete types.
** Return true if all parameters have complete types.
*/
{
    unsigned    I = 0;
    unsigned    Offs;
    SymEntry*   Param;
    unsigned    ParamSize = 0;
    unsigned    IncompleteCount = 0;

    /* Don't bother to check unnecessarily */
    if ((D->Flags & FD_INCOMPLETE_PARAM) == 0) {
        return 1;
    }

    /* Assign offsets. If the function has a variable parameter list,
    ** there's one additional byte (the arg size).
    */
    Offs  = (D->Flags & FD_VARIADIC) ? 1 : 0;
    Param = D->LastParam;
    while (Param) {
        unsigned Size = SizeOf (Param->Type);
        if (RequireAll && IsIncompleteESUType (Param->Type)) {
            if (D->Flags & FD_UNNAMED_PARAMS) {
                Error ("Parameter %u has incomplete type '%s'",
                       D->ParamCount - I,
                       GetFullTypeName (Param->Type));
            } else {
                Error ("Parameter '%s' has incomplete type '%s'",
                       Param->Name,
                       GetFullTypeName (Param->Type));
            }
            ++IncompleteCount;
        }
        if (SymIsRegVar (Param)) {
            Param->V.R.SaveOffs = Offs;
        } else {
            Param->V.Offs = Offs;
        }
        Offs += Size;
        ParamSize += Size;
        Param = Param->PrevSym;
        ++I;
    }

    /* If all parameters have complete types, set the total size description,
    ** clear the FD_INCOMPLETE_PARAM flag and return true.
    */
    if (IncompleteCount == 0) {
        D->ParamSize = ParamSize;
        D->Flags &= ~FD_INCOMPLETE_PARAM;
        return 1;
    }

    /* Otherwise return false */
    return 0;
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



const Type* F_GetReturnType (Function* F)
/* Get the return type for the function */
{
    return F->ReturnType;
}



int F_HasVoidReturn (const Function* F)
/* Return true if the function does not have a return value */
{
    return (F->Flags & FF_VOID_RETURN) != 0;
}



void F_ReturnFound (Function* F)
/* Mark the function as having a return statement */
{
    F->Flags |= FF_HAS_RETURN;
}



int F_HasReturn (const Function* F)
/* Return true if the function contains a return statement*/
{
    return (F->Flags & FF_HAS_RETURN) != 0;
}



int F_IsMainFunc (const Function* F)
/* Return true if this is the main function */
{
    return (F->Flags & FF_IS_MAIN) != 0;
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



void F_SetRetLab (Function* F, unsigned NewRetLab)
/* Change the return jump label */
{
    F->RetLab = NewRetLab;
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
** offset.
*/
{
    F->Reserved += Size;
    return StackPtr - F->Reserved;
}



int F_GetStackPtr (const Function* F)
/* Return the current stack pointer including reserved (but not allocated)
** space on the stack.
*/
{
    return StackPtr - F->Reserved;
}



void F_AllocLocalSpace (Function* F)
/* Allocate any local space previously reserved. The function will do
** nothing if there is no reserved local space.
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



int F_AllocRegVar (Function* F, const Type* Type)
/* Allocate a register variable for the given variable type. If the allocation
** was successful, return the offset of the register variable in the register
** bank (zero page storage). If there is no register space left, return -1.
*/
{
    /* Allow register variables only on top level and if enabled */
    if (IS_Get (&EnableRegVars) && GetLexicalLevel () == LEX_LEVEL_FUNCTION) {

        /* Get the size of the variable */
        unsigned Size = CheckedSizeOf (Type);

        /* Do we have space left? */
        if (F->RegOffs >= Size) {
            /* Space left. We allocate the variables from high to low addresses,
            ** so the addressing is compatible with the saved values on stack.
            ** This allows shorter code when saving/restoring the variables.
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
    Sym = F->Desc->SymTab->SymHead;

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



static void F_EmitDebugInfo (void)
/* Emit debug infos for the current function */
{
    if (DebugInfo) {
        /* Get the current function */
        const SymEntry* Sym = CurrentFunc->FuncEntry;

        /* Output info for the function itself */
        AddTextLine ("\t.dbg\tfunc, \"%s\", \"00\", %s, \"%s\"",
                     Sym->Name,
                     (Sym->Flags & SC_EXTERN)? "extern" : "static",
                     Sym->AsmName);
    }
}



/*****************************************************************************/
/*                                   code                                    */
/*****************************************************************************/



void NewFunc (SymEntry* Func, FuncDesc* D)
/* Parse argument declarations and function body. */
{
    int         ParamComplete;  /* If all paramemters have complete types */
    int         C99MainFunc = 0;/* Flag for C99 main function returning int */
    SymEntry*   Param;
    const Type* RType;          /* Real type used for struct parameters */
    const Type* ReturnType;     /* Return type */

    /* Remember this function descriptor used for definition */
    GetFuncDesc (Func->Type)->FuncDef = D;

    /* Allocate the function activation record for the function */
    CurrentFunc = NewFunction (Func, D);

    /* Reenter the lexical level */
    ReenterFunctionLevel (D);

    /* Check return type */
    ReturnType = F_GetReturnType (CurrentFunc);
    if (IsIncompleteESUType (ReturnType)) {
        /* There are already diagnostics on returning arrays or functions */
        if (!IsTypeArray (ReturnType) && !IsTypeFunc (ReturnType)) {
            Error ("Function has incomplete return type '%s'",
                    GetFullTypeName (ReturnType));
        }
    }

    /* Check and set the parameter sizes. All parameter must have complete
    ** types now.
    */
    ParamComplete = F_CheckParamList (D, 1);

    /* Check if the function header contains unnamed parameters. These are
    ** only allowed in cc65 mode.
    */
    if ((D->Flags & FD_UNNAMED_PARAMS) != 0 && (IS_Get (&Standard) != STD_CC65)) {
        Error ("Parameter name omitted");
    }

    /* Declare two special functions symbols: __fixargs__ and __argsize__.
    ** The latter is different depending on the type of the function (variadic
    ** or not).
    */
    AddConstSym ("__fixargs__", type_uint, SC_DEF | SC_CONST, D->ParamSize);
    if (D->Flags & FD_VARIADIC) {
        /* Variadic function. The variable must be const. */
        static const Type T[] = { TYPE(T_UCHAR | T_QUAL_CONST), TYPE(T_END) };
        AddLocalSym ("__argsize__", T, SC_DEF | SC_REF | SC_AUTO, 0);
    } else {
        /* Non variadic */
        AddConstSym ("__argsize__", type_uchar, SC_DEF | SC_CONST, D->ParamSize);
    }

    /* Function body now defined */
    Func->Flags |= SC_DEF;

    /* Special handling for main() */
    if (strcmp (Func->Name, "main") == 0) {

        /* Mark this as the main function */
        CurrentFunc->Flags |= FF_IS_MAIN;

        /* Main cannot be a fastcall function */
        if (IsQualFastcall (Func->Type)) {
            Error ("'main' cannot be declared as __fastcall__");
        }

        /* If cc65 extensions aren't enabled, don't allow a main function that
        ** doesn't return an int.
        */
        if (IS_Get (&Standard) != STD_CC65 && ReturnType[0].C != T_INT) {
            Error ("'main' must always return an int");
        }

        /* Add a forced import of a symbol that is contained in the startup
        ** code. This will force the startup code to be linked in.
        */
        g_importstartup ();

        /* If main() takes parameters, generate a forced import to a function
        ** that will setup these parameters. This way, programs that do not
        ** need the additional code will not get it.
        */
        if (D->ParamCount > 0 || (D->Flags & FD_VARIADIC) != 0) {
            g_importmainargs ();

            /* The start-up code doesn't fast-call main(). */
            Func->Type->C |= T_QUAL_CDECL;
        }

        /* Determine if this is a main function in a C99 environment that
        ** returns an int.
        */
        if (GetUnqualRawTypeCode (ReturnType) == T_INT &&
            IS_Get (&Standard) == STD_C99) {
            C99MainFunc = 1;
        }
    }

    /* Allocate code and data segments for this function */
    Func->V.F.Seg = PushSegments (Func);

    /* Use the info in the segments for generating new local labels */
    UseLabelPoolFromSegments (Func->V.F.Seg);

    /* Set return label. This has to be done after the segments are pushed */
    F_SetRetLab (CurrentFunc, GetLocalLabel ());

    /* Allocate a new literal pool */
    PushLiteralPool (Func);

    /* If this is a fastcall function, push the last parameter onto the stack */
    if (D->ParamCount > 0 && IsFastcallFunc (Func->Type)) {
        unsigned Flags;

        /* Generate the push */
        /* Handle struct/union specially */
        if (IsClassStruct (D->LastParam->Type)) {
            Flags = TypeOf (GetStructReplacementType (D->LastParam->Type)) | CF_FORCECHAR;
        } else {
            Flags = TypeOf (D->LastParam->Type) | CF_FORCECHAR;
        }
        g_push (Flags, 0);
    }

    /* Generate function entry code if needed */
    g_enter (FuncTypeOf (Func->Type), F_GetParamSize (CurrentFunc));

    /* If stack checking code is requested, emit a call to the helper routine */
    if (IS_Get (&CheckStack)) {
        g_stackcheck ();
    }

    /* Setup the stack */
    StackPtr = 0;

    /* Emit code to handle the parameters if all of them have complete types */
    if (ParamComplete) {
        /* Walk through the parameter list and allocate register variable space
        ** for parameters declared as register. Generate code to swap the contents
        ** of the register bank with the save area on the stack.
        */
        Param = D->SymTab->SymHead;
        while (Param && (Param->Flags & SC_PARAM) != 0) {

            /* Check if we need copy for struct/union type */
            RType = Param->Type;
            if (IsClassStruct (RType)) {
                RType = GetStructReplacementType (RType);

                /* If there is no replacement type, then it is just the address.
                ** We don't currently support this case.
                */
                if (RType == Param->Type) {
                    Error ("Passing '%s' of this size (%d) by value is not supported", GetFullTypeName (Param->Type), SizeOf (RType));
                }
            }

            /* Check for a register variable */
            if (SymIsRegVar (Param)) {

                /* Allocate space */
                int Reg = F_AllocRegVar (CurrentFunc, RType);

                /* Could we allocate a register? */
                if (Reg < 0) {
                    /* No register available: Convert parameter to auto */
                    SymCvtRegVarToAuto (Param);
                } else {
                    /* Remember the register offset */
                    Param->V.R.RegOffs = Reg;

                    /* Generate swap code */
                    g_swap_regvars (Param->V.R.SaveOffs, Reg, CheckedSizeOf (RType));
                }
            }

            /* Next parameter */
            Param = Param->NextSym;
        }
    }

    /* Need a starting curly brace */
    ConsumeLCurly ();

    /* Make sure there is always something on the stack of local variable blocks */
    CollAppend (&CurrentFunc->LocalsBlockStack, 0);

    /* Parse local variable declarations if any */
    DeclareLocals ();

    /* Remember the current stack pointer. All variables allocated elsewhere
    ** must be dropped when doing a return from an inner block.
    */
    CurrentFunc->TopLevelSP = StackPtr;

    /* Now process statements in this block */
    while (CurTok.Tok != TOK_RCURLY && CurTok.Tok != TOK_CEOF) {
        AnyStatement (0);
    }

    /* If this is not a void function, and not the main function in a C99
    ** environment returning int, output a warning if we didn't see a return
    ** statement.
    */
    if (!F_HasVoidReturn (CurrentFunc) && !F_HasReturn (CurrentFunc) && !C99MainFunc && IS_Get (&WarnReturnType)) {
        Warning ("Control reaches end of non-void function [-Wreturn-type]");
    }

    /* If this is the main function in a C99 environment returning an int, let
    ** it always return zero. Note: Actual return statements jump to the return
    ** label defined below.
    ** The code is removed by the optimizer if unused.
    */
    if (C99MainFunc) {
        g_getimmed (CF_INT | CF_CONST, 0, 0);
    }

    /* Output the function exit code label */
    g_defcodelabel (F_GetRetLab (CurrentFunc));

    /* Restore the register variables */
    F_RestoreRegVars (CurrentFunc);

    /* Generate the exit code */
    g_leave ();

    /* Emit references to imports/exports */
    EmitExternals ();

    /* Emit function debug info */
    F_EmitDebugInfo ();
    EmitDebugInfo ();

    /* Leave the lexical level */
    LeaveFunctionLevel ();

    /* Eat the closing brace */
    ConsumeRCurly ();

    /* Restore the old literal pool, remembering the one for the function */
    Func->V.F.LitPool = PopLiteralPool ();

    /* If --local-strings was given, output the literals now */
    if (IS_Get (&LocalStrings)) {
        OutputLocalLiteralPool (Func->V.F.LitPool);
    }

    /* Switch back to the old segments */
    PopSegments ();

    /* Reset the current function pointer */
    FreeFunction (CurrentFunc);
    CurrentFunc = 0;
}
