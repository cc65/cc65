/*****************************************************************************/
/*                                                                           */
/*				   stdfunc.c				     */
/*                                                                           */
/*	   Handle inlining of known functions for the cc65 compiler	     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2004 Ullrich von Bassewitz                                       */
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
#include "asmlabel.h"
#include "codegen.h"
#include "error.h"
#include "funcdesc.h"
#include "global.h"
#include "litpool.h"
#include "scanner.h"
#include "stdfunc.h"
#include "stdnames.h"
#include "typeconv.h"



/*****************************************************************************/
/*	  	     	       Function forwards	    		     */
/*****************************************************************************/



static void StdFunc_memset (FuncDesc*, ExprDesc*);
static void StdFunc_strlen (FuncDesc*, ExprDesc*);



/*****************************************************************************/
/*  	  	    	  	     Data   	    	    		     */
/*****************************************************************************/



/* Table with all known functions and their handlers. Must be sorted
 * alphabetically!
 */
static struct StdFuncDesc {
    const char*	 	Name;
    void  	 	(*Handler) (FuncDesc*, ExprDesc*);
} StdFuncs[] = {
    {  	"memset",      	StdFunc_memset	  	},
    {  	"strlen",	StdFunc_strlen	  	},

};
#define FUNC_COUNT	(sizeof (StdFuncs) / sizeof (StdFuncs[0]))



/*****************************************************************************/
/*			       Helper functions	     	    		     */
/*****************************************************************************/



static int CmpFunc (const void* Key, const void* Elem)
/* Compare function for bsearch */
{
    return strcmp ((const char*) Key, ((const struct StdFuncDesc*) Elem)->Name);
}



static unsigned ParseArg (type* Type, ExprDesc* Arg)
/* Parse one argument but do not push it onto the stack. Return the code
 * generator flags needed to do the actual push.
 */
{
    /* We have a prototype, so chars may be pushed as chars */
    unsigned Flags = CF_FORCECHAR;

    /* Read the expression we're going to pass to the function */
    hie1 (Arg);

    /* Convert this expression to the expected type */
    TypeConversion (Arg, Type);

    /* If the value is a constant, set the flag, otherwise load it into the
     * primary register.
     */
    if (ED_IsConstAbsInt (Arg)) {
        /* Remember that we have a constant value */
        Flags |= CF_CONST;
    } else {
        /* Load into the primary */
        ExprLoad (CF_NONE, Arg);
        ED_MakeRVal (Arg);
    }

    /* Use the type of the argument for the push */
    return (Flags | TypeOf (Arg->Type));
}



/*****************************************************************************/
/*		 	    Handle known functions  			     */
/*****************************************************************************/



static void StdFunc_memset (FuncDesc* F attribute ((unused)), ExprDesc* Expr)
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

    /* Argument #1 */
    Flags = ParseArg (Arg1Type, &Arg);
    g_push (Flags, Arg.Val);
    ParamSize += SizeOf (Arg1Type);
    ConsumeComma ();

    /* Argument #2. This argument is special in that we will call another
     * function if it is a constant zero.
     */
    Flags = ParseArg (Arg2Type, &Arg);
    if ((Flags & CF_CONST) != 0 && Arg.Val == 0) {
        /* Don't call memset, call bzero instead */
        MemSet = 0;
    } else {
        /* Push the argument */
        g_push (Flags, Arg.Val);
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
	if (Arg.Val == 0) {
	    Warning ("Call to memset has no effect");
	}
        ExprLoad (CF_FORCECHAR, &Arg);
    }

    /* Emit the actual function call */
    g_call (CF_NONE, MemSet? Func_memset : Func__bzero, ParamSize);

    /* We expect the closing brace */
    ConsumeRParen ();

    /* The function result is an rvalue in the primary register */
    ED_MakeRValExpr (Expr);
    Expr->Type = GetFuncReturn (Expr->Type);
}



static void StdFunc_strlen (FuncDesc* F attribute ((unused)), ExprDesc* Expr)
/* Handle the strlen function */
{
    static type ArgType[] = { T_PTR, T_SCHAR, T_END };
    ExprDesc    Arg;
    unsigned    L;


    /* Setup the argument type string */
    ArgType[1] = GetDefaultChar () | T_QUAL_CONST;

    /* Evaluate the parameter */
    hie1 (&Arg);

    /* We can generate special code for several locations */
    if (ED_IsLocConst (&Arg) && IsTypeArray (Arg.Type)) {

        /* Do type conversion */
        TypeConversion (&Arg, ArgType);

        /* If the expression is a literal, and if string literals are read
         * only, we can calculate the length of the string and remove it
         * from the literal pool. Otherwise we have to calculate the length
         * at runtime.
         */
        if (ED_IsLocLiteral (&Arg) && IS_Get (&WritableStrings)) {

            /* Constant string literal */
            ED_MakeConstAbs (Expr, strlen (GetLiteral (Arg.Val)), type_size_t);
            ResetLiteralPoolOffs (Arg.Val);

        } else {

            /* Generate the strlen code */
            L = GetLocalLabel ();
            AddCodeLine ("ldy #$FF");
            g_defcodelabel (L);
            AddCodeLine ("iny");
            AddCodeLine ("lda %s,y", ED_GetLabelName (&Arg, 0));
            AddCodeLine ("bne %s", LocalLabelName (L));
            AddCodeLine ("tax");
            AddCodeLine ("tya");

            /* The function result is an rvalue in the primary register */
            ED_MakeRValExpr (Expr);
            Expr->Type = type_size_t;

        }

    } else if (ED_IsLocStack (&Arg) && StackPtr >= -255 && IsTypeArray (Arg.Type)) {

        /* Calculate the true stack offset */
        unsigned Offs = (unsigned) (Arg.Val - StackPtr);

        /* Do type conversion */
        TypeConversion (&Arg, ArgType);

        /* Generate the strlen code */
        L = GetLocalLabel ();
        AddCodeLine ("ldx #$FF");
        AddCodeLine ("ldy #$%02X", (unsigned char) (Offs-1));
        g_defcodelabel (L);
        AddCodeLine ("inx");
        AddCodeLine ("iny");
        AddCodeLine ("lda (sp),y");
        AddCodeLine ("bne %s", LocalLabelName (L));
        AddCodeLine ("txa");
        AddCodeLine ("ldx #$00");

        /* The function result is an rvalue in the primary register */
        ED_MakeRValExpr (Expr);
        Expr->Type = type_size_t;

    } else if (ED_IsLocRegister (&Arg) && ED_IsLVal (&Arg) && IsTypePtr (Arg.Type)) {

        /* Do type conversion */
        TypeConversion (&Arg, ArgType);

        /* Generate the strlen code */
        L = GetLocalLabel ();
        AddCodeLine ("ldy #$FF");
        g_defcodelabel (L);
        AddCodeLine ("iny");
        AddCodeLine ("lda (%s),y", ED_GetLabelName (&Arg, 0));
        AddCodeLine ("bne %s", LocalLabelName (L));
        AddCodeLine ("tax");
        AddCodeLine ("tya");

        /* The function result is an rvalue in the primary register */
        ED_MakeRValExpr (Expr);
        Expr->Type = type_size_t;

    } else {

        /* Do type conversion */
        TypeConversion (&Arg, ArgType);

        /* Load the expression into the primary */
        ExprLoad (CF_NONE, &Arg);

        /* Call the strlen function */
        AddCodeLine ("jsr _%s", Func_strlen);

        /* The function result is an rvalue in the primary register */
        ED_MakeRValExpr (Expr);
        Expr->Type = type_size_t;

    }

    /* We expect the closing brace */
    ConsumeRParen ();
}



/*****************************************************************************/
/*  		    	 	     Code	       	 		     */
/*****************************************************************************/



int FindStdFunc (const char* Name)
/* Determine if the given function is a known standard function that may be
 * called in a special way. If so, return the index, otherwise return -1.
 */
{
    /* Look into the table for known names */
    struct StdFuncDesc* D =
        bsearch (Name, StdFuncs, FUNC_COUNT, sizeof (StdFuncs[0]), CmpFunc);

    /* Return the function index or -1 */
    if (D == 0) {
        return -1;
    } else {
        return D - StdFuncs;
    }
}



void HandleStdFunc (int Index, FuncDesc* F, ExprDesc* lval)
/* Generate code for a known standard function. */
{
    struct StdFuncDesc* D;

    /* Get a pointer to the table entry */
    CHECK (Index >= 0 && Index < (int)FUNC_COUNT);
    D = StdFuncs + Index;

    /* Call the handler function */
    D->Handler (F, lval);
}



