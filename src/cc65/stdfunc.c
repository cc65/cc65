/*****************************************************************************/
/*                                                                           */
/*				   stdfunc.c				     */
/*                                                                           */
/*	   Handle inlining of known functions for the cc65 compiler	     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2003 Ullrich von Bassewitz                                       */
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
#include <string.h>

/* common */
#include "attrib.h"
#include "check.h"

/* cc65 */
#include "codegen.h"
#include "error.h"
#include "funcdesc.h"
#include "global.h"
#include "litpool.h"
#include "scanner.h"
#include "stdfunc.h"
#include "typeconv.h"



/*****************************************************************************/
/*		     	       Function forwards	    		     */
/*****************************************************************************/



static void StdFunc_memset (FuncDesc*, ExprDesc*);
static void StdFunc_strlen (FuncDesc*, ExprDesc*);



/*****************************************************************************/
/*  		    	  	     Data   	    	    		     */
/*****************************************************************************/



/* Table with all known functions and their handlers. Must be sorted
 * alphabetically!
 */
static struct StdFuncDesc {
    const char*	 	Name;
    void 	 	(*Handler) (FuncDesc*, ExprDesc*);
} StdFuncs [] = {
    {  	"memset",      	StdFunc_memset	  	},
    {  	"strlen",	StdFunc_strlen	  	},

};
#define FUNC_COUNT	(sizeof (StdFuncs) / sizeof (StdFuncs [0]))


/*****************************************************************************/
/*			       Helper functions	     	    		     */
/*****************************************************************************/



static int CmpFunc (const void* Key, const void* Elem)
/* Compare function for bsearch */
{
    return strcmp ((const char*) Key, ((const struct StdFuncDesc*) Elem)->Name);
}



static struct StdFuncDesc* FindFunc (const char* Name)
/* Find a function with the given name. Return a pointer to the descriptor if
 * found, return NULL otherwise.
 */
{
    return bsearch (Name, StdFuncs, FUNC_COUNT, sizeof (StdFuncs [0]), CmpFunc);
}



static unsigned ParseArg (type* Type, ExprDesc* Arg)
/* Parse one argument but do not push it onto the stack. Return the code
 * generator flags needed to do the actual push.
 */
{
    /* We have a prototype, so chars may be pushed as chars */
    unsigned Flags = CF_FORCECHAR;

    /* Read the expression we're going to pass to the function */
    int k = hie1 (InitExprDesc (Arg));

    /* Convert this expression to the expected type */
    k = TypeConversion (Arg, k, Type);

    /* If the value is not a constant, load it into the primary */
    if (k != 0 || Arg->Flags != E_MCONST) {

        /* Load into the primary */
        ExprLoad (CF_NONE, k, Arg);
        k = 0;

    } else {

        /* Remember that we have a constant value */
        Flags |= CF_CONST;

    }

    /* Use the type of the argument for the push */
    return (Flags | TypeOf (Arg->Type));
}



/*****************************************************************************/
/*		 	    Handle known functions  			     */
/*****************************************************************************/



static void StdFunc_memset (FuncDesc* F attribute ((unused)),
                            ExprDesc* lval attribute ((unused)))
/* Handle the memset function */
{
    /* Argument types */
    static type Arg1Type[] = { T_PTR, T_VOID, T_END };  /* void* */
    static type Arg2Type[] = { T_INT, T_END };          /* int */
    static type Arg3Type[] = { T_UINT, T_END };         /* size_t */

    unsigned Flags;
    ExprDesc Arg;
    int      MemSet    = 1;             /* Use real memset if true */
    unsigned ParamSize = 0;


    /* Check the prototype of the function against what we know about it, so
     * we can detect errors.
     */
    /* ### */

    /* Argument #1 */
    Flags = ParseArg (Arg1Type, &Arg);
    g_push (Flags, Arg.ConstVal);
    ParamSize += SizeOf (Arg1Type);
    ConsumeComma ();

    /* Argument #2. This argument is special in that we will call another
     * function if it is a constant zero.
     */
    Flags = ParseArg (Arg2Type, &Arg);
    if ((Flags & CF_CONST) != 0 && Arg.ConstVal == 0) {
        /* Don't call memset, call bzero instead */
        MemSet = 0;
    } else {
        /* Push the argument */
        g_push (Flags, Arg.ConstVal);
        ParamSize += SizeOf (Arg2Type);
    }
    ConsumeComma ();

    /* Argument #3. Since memset is a fastcall function, we must load the
     * arg into the primary if it is not already there. This parameter is
     * also ignored for the calculation of the parameter size, since it is
     * not passed via the stack.
     */
    Flags = ParseArg (Arg3Type, &Arg);
    if (Flags & CF_CONST) {
        ExprLoad (CF_FORCECHAR, 0, &Arg);
    }

    /* Emit the actual function call */
    g_call (CF_NONE, MemSet? "memset" : "_bzero", ParamSize);

    /* We expect the closing brace */
    ConsumeRParen ();
}



static void StdFunc_strlen (FuncDesc* F attribute ((unused)),
                            ExprDesc* lval attribute ((unused)))
/* Handle the strlen function */
{
    static type   ParamType[] = { T_PTR, T_SCHAR, T_END };
    int           k;
    ExprDesc      Param;
    unsigned      CodeFlags;
    unsigned long ParamName;

    /* Setup the argument type string */
    ParamType[1] = GetDefaultChar () | T_QUAL_CONST;

    /* Fetch the parameter and convert it to the type needed */
    k = TypeConversion (&Param, hie1 (InitExprDesc (&Param)), ParamType);

    /* Check if the parameter is a constant array of some type, or a numeric
     * address cast to a pointer.
     */
    CodeFlags = 0;
    ParamName = Param.Name;
    if ((IsTypeArray (Param.Type) && (Param.Flags & E_MCONST) != 0) ||
        (IsTypePtr (Param.Type) && Param.Flags == (E_MCONST | E_TCONST))) {

        /* Check which type of constant it is */
        switch (Param.Flags & E_MCTYPE) {

            case E_TCONST:
                /* Numerical address */
                CodeFlags |= CF_CONST | CF_ABSOLUTE;
                break;

            case E_TREGISTER:
                /* Register variable */
                CodeFlags |= CF_CONST | CF_REGVAR;
                break;

            case E_TGLAB:
                /* Global label */
                CodeFlags |= CF_CONST | CF_EXTERNAL;
                break;

            case E_TLLAB:
                /* Local symbol */
                CodeFlags |= CF_CONST | CF_STATIC;
                break;

            case E_TLIT:
                /* A literal of some kind. If string literals are read only,
                 * we can calculate the length of the string and remove it
                 * from the literal pool. Otherwise we have to calculate the
                 * length at runtime.
                 */
                if (!WriteableStrings) {
                    /* String literals are const */
                    ExprDesc Length;
                    MakeConstIntExpr (&Length, strlen (GetLiteral (Param.ConstVal)));
                    ResetLiteralPoolOffs (Param.ConstVal);
                    ExprLoad (CF_NONE, 0, &Length);
                    goto ExitPoint;
                } else {
                    CodeFlags |= CF_CONST | CF_STATIC;
                    ParamName = LiteralPoolLabel;
                }
                break;

            default:
                Internal ("Unknown constant type: %04X", Param.Flags);
        }

    } else {

     	/* Not an array with a constant address. Load parameter into primary */
     	ExprLoad (CF_NONE, k, &Param);

    }

    /* Generate the strlen code */
    g_strlen (CodeFlags, ParamName, Param.ConstVal);

ExitPoint:
    /* We expect the closing brace */
    ConsumeRParen ();
}



/*****************************************************************************/
/*  		    	 	     Code	       	 		     */
/*****************************************************************************/



int IsStdFunc (const char* Name)
/* Determine if the given function is a known standard function that may be
 * called in a special way.
 */
{
    /* Look into the table for known names */
    return FindFunc (Name) != 0;
}



void HandleStdFunc (FuncDesc* F, ExprDesc* lval)
/* Generate code for a known standard function. */
{
    /* Get a pointer to the table entry */
    struct StdFuncDesc* D = FindFunc ((const char*) lval->Name);
    CHECK (D != 0);

    /* Call the handler function */
    D->Handler (F, lval);
}



