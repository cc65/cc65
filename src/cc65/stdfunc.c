/*****************************************************************************/
/*                                                                           */
/*                                 stdfunc.c                                 */
/*                                                                           */
/*         Handle inlining of known functions for the cc65 compiler          */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2010 Ullrich von Bassewitz                                       */
/*               Roemerstrasse 52                                            */
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
#include "asmcode.h"
#include "asmlabel.h"
#include "codegen.h"
#include "error.h"
#include "funcdesc.h"
#include "global.h"
#include "litpool.h"
#include "loadexpr.h"
#include "scanner.h"
#include "stackptr.h"
#include "stdfunc.h"
#include "stdnames.h"
#include "typeconv.h"



/*****************************************************************************/
/*                             Function forwards                             */
/*****************************************************************************/



static void StdFunc_memcpy (FuncDesc*, ExprDesc*);
static void StdFunc_memset (FuncDesc*, ExprDesc*);
static void StdFunc_strcmp (FuncDesc*, ExprDesc*);
static void StdFunc_strcpy (FuncDesc*, ExprDesc*);
static void StdFunc_strlen (FuncDesc*, ExprDesc*);



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Table with all known functions and their handlers. Must be sorted
** alphabetically!
*/
static struct StdFuncDesc {
    const char*         Name;
    void                (*Handler) (FuncDesc*, ExprDesc*);
} StdFuncs[] = {
    {   "memcpy",       StdFunc_memcpy          },
    {   "memset",       StdFunc_memset          },
    {   "strcmp",       StdFunc_strcmp          },
    {   "strcpy",       StdFunc_strcpy          },
    {   "strlen",       StdFunc_strlen          },

};
#define FUNC_COUNT      (sizeof (StdFuncs) / sizeof (StdFuncs[0]))

typedef struct ArgDesc ArgDesc;
struct ArgDesc {
    const Type* ArgType;        /* Required argument type */
    ExprDesc    Expr;           /* Argument expression */
    const Type* Type;           /* The original type before conversion */
    CodeMark    Load;           /* Start of argument load code */
    CodeMark    Push;           /* Start of argument push code */
    CodeMark    End;            /* End of the code for calculation+push */
    unsigned    Flags;          /* Code generation flags */
};



/*****************************************************************************/
/*                             Helper functions                              */
/*****************************************************************************/



static int CmpFunc (const void* Key, const void* Elem)
/* Compare function for bsearch */
{
    return strcmp ((const char*) Key, ((const struct StdFuncDesc*) Elem)->Name);
}



static long ArrayElementCount (const ArgDesc* Arg)
/* Check if the type of the given argument is an array. If so, and if the
** element count is known, return it. In all other cases, return UNSPECIFIED.
*/
{
    long Count;

    if (IsTypeArray (Arg->Type)) {
        Count = GetElementCount (Arg->Type);
        if (Count == FLEXIBLE) {
            /* Treat as unknown */
            Count = UNSPECIFIED;
        }
    } else {
        Count = UNSPECIFIED;
    }
    return Count;
}



static void ParseArg (ArgDesc* Arg, Type* Type)
/* Parse one argument but do not push it onto the stack. Make all fields in
** Arg valid.
*/
{
    /* We have a prototype, so chars may be pushed as chars */
    Arg->Flags = CF_FORCECHAR;

    /* Remember the required argument type */
    Arg->ArgType = Type;

    /* Read the expression we're going to pass to the function */
    MarkedExprWithCheck (hie1, &Arg->Expr);

    /* Remember the actual argument type */
    Arg->Type = Arg->Expr.Type;

    /* Convert this expression to the expected type */
    TypeConversion (&Arg->Expr, Type);

    /* Remember the following code position */
    GetCodePos (&Arg->Load);

    /* If the value is a constant, set the flag, otherwise load it into the
    ** primary register.
    */
    if (ED_IsConstAbsInt (&Arg->Expr) && ED_CodeRangeIsEmpty (&Arg->Expr)) {
        /* Remember that we have a constant value */
        Arg->Flags |= CF_CONST;
    } else {
        /* Load into the primary */
        LoadExpr (CF_NONE, &Arg->Expr);
    }

    /* Remember the following code position */
    GetCodePos (&Arg->Push);
    GetCodePos (&Arg->End);

    /* Use the type of the argument for the push */
    Arg->Flags |= TypeOf (Arg->Expr.Type);
}



/*****************************************************************************/
/*                                  memcpy                                   */
/*****************************************************************************/



static void StdFunc_memcpy (FuncDesc* F attribute ((unused)), ExprDesc* Expr)
/* Handle the memcpy function */
{
    /* Argument types: (void*, const void*, size_t) */
    static Type Arg1Type[] = { TYPE(T_PTR), TYPE(T_VOID), TYPE(T_END) };
    static Type Arg2Type[] = { TYPE(T_PTR), TYPE(T_VOID|T_QUAL_CONST), TYPE(T_END) };
    static Type Arg3Type[] = { TYPE(T_SIZE_T), TYPE(T_END) };

    ArgDesc  Arg1, Arg2, Arg3;
    unsigned ParamSize = 0;
    unsigned Label;
    int      Offs;

    /* Argument #1 */
    ParseArg (&Arg1, Arg1Type);
    g_push (Arg1.Flags, Arg1.Expr.IVal);
    GetCodePos (&Arg1.End);
    ParamSize += SizeOf (Arg1Type);
    ConsumeComma ();

    /* Argument #2 */
    ParseArg (&Arg2, Arg2Type);
    g_push (Arg2.Flags, Arg2.Expr.IVal);
    GetCodePos (&Arg2.End);
    ParamSize += SizeOf (Arg2Type);
    ConsumeComma ();

    /* Argument #3. Since memcpy is a fastcall function, we must load the
    ** arg into the primary if it is not already there. This parameter is
    ** also ignored for the calculation of the parameter size, since it is
    ** not passed via the stack.
    */
    ParseArg (&Arg3, Arg3Type);
    if (Arg3.Flags & CF_CONST) {
        LoadExpr (CF_NONE, &Arg3.Expr);
    }

    /* Emit the actual function call. This will also cleanup the stack. */
    g_call (CF_FIXARGC, Func_memcpy, ParamSize);

    if (ED_IsConstAbsInt (&Arg3.Expr) && Arg3.Expr.IVal == 0) {

        /* memcpy has been called with a count argument of zero */
        Warning ("Call to memcpy has no effect");

        /* Remove all of the generated code but the load of the first
        ** argument, which is what memcpy returns.
        */
        RemoveCode (&Arg1.Push);

        /* Set the function result to the first argument */
        *Expr = Arg1.Expr;

        /* Bail out, no need for further improvements */
        goto ExitPoint;
    }

    /* We've generated the complete code for the function now and know the
    ** types of all parameters. Check for situations where better code can
    ** be generated. If such a situation is detected, throw away the
    ** generated, and emit better code.
    */
    if (ED_IsConstAbsInt (&Arg3.Expr) && Arg3.Expr.IVal <= 256 &&
        ((ED_IsRVal (&Arg2.Expr) && ED_IsLocConst (&Arg2.Expr)) ||
         (ED_IsLVal (&Arg2.Expr) && ED_IsLocRegister (&Arg2.Expr))) &&
        ((ED_IsRVal (&Arg1.Expr) && ED_IsLocConst (&Arg1.Expr)) ||
         (ED_IsLVal (&Arg1.Expr) && ED_IsLocRegister (&Arg1.Expr)))) {

        int Reg1 = ED_IsLVal (&Arg1.Expr) && ED_IsLocRegister (&Arg1.Expr);
        int Reg2 = ED_IsLVal (&Arg2.Expr) && ED_IsLocRegister (&Arg2.Expr);

        /* Drop the generated code */
        RemoveCode (&Arg1.Expr.Start);

        /* We need a label */
        Label = GetLocalLabel ();

        /* Generate memcpy code */
        if (Arg3.Expr.IVal <= 127) {

            AddCodeLine ("ldy #$%02X", (unsigned char) (Arg3.Expr.IVal-1));
            AddCodeLine ("lda #$%02X", (unsigned char) Arg2.Expr.IVal);
            g_defcodelabel (Label);
            if (Reg2) {
                AddCodeLine ("lda (%s),y", ED_GetLabelName (&Arg2.Expr, 0));
            } else {
                AddCodeLine ("lda %s,y", ED_GetLabelName (&Arg2.Expr, 0));
            }
            if (Reg1) {
                AddCodeLine ("sta (%s),y", ED_GetLabelName (&Arg1.Expr, 0));
            } else {
                AddCodeLine ("sta %s,y", ED_GetLabelName (&Arg1.Expr, 0));
            }
            AddCodeLine ("dey");
            AddCodeLine ("bpl %s", LocalLabelName (Label));

        } else {

            AddCodeLine ("ldy #$00");
            AddCodeLine ("lda #$%02X", (unsigned char) Arg2.Expr.IVal);
            g_defcodelabel (Label);
            if (Reg2) {
                AddCodeLine ("lda (%s),y", ED_GetLabelName (&Arg2.Expr, 0));
            } else {
                AddCodeLine ("lda %s,y", ED_GetLabelName (&Arg2.Expr, 0));
            }
            if (Reg1) {
                AddCodeLine ("sta (%s),y", ED_GetLabelName (&Arg1.Expr, 0));
            } else {
                AddCodeLine ("sta %s,y", ED_GetLabelName (&Arg1.Expr, 0));
            }
            AddCodeLine ("iny");
            AddCodeLine ("cpy #$%02X", (unsigned char) Arg3.Expr.IVal);
            AddCodeLine ("bne %s", LocalLabelName (Label));

        }

        /* memcpy returns the address, so the result is actually identical
        ** to the first argument.
        */
        *Expr = Arg1.Expr;

    } else if (ED_IsConstAbsInt (&Arg3.Expr) && Arg3.Expr.IVal <= 256 &&
               ED_IsRVal (&Arg2.Expr) && ED_IsLocConst (&Arg2.Expr) &&
               ED_IsRVal (&Arg1.Expr) && ED_IsLocStack (&Arg1.Expr) &&
               (Arg1.Expr.IVal - StackPtr) + Arg3.Expr.IVal < 256) {

        /* It is possible to just use one index register even if the stack
        ** offset is not zero, by adjusting the offset to the constant
        ** address accordingly. But we cannot do this if the data in
        ** question is in the register space or at an absolute address less
        ** than 256. Register space is zero page, which means that the
        ** address calculation could overflow in the linker.
        */
        int AllowOneIndex = !ED_IsLocRegister (&Arg2.Expr) &&
                            !(ED_IsLocAbs (&Arg2.Expr) && Arg2.Expr.IVal < 256);

        /* Calculate the real stack offset */
        Offs = ED_GetStackOffs (&Arg1.Expr, 0);

        /* Drop the generated code */
        RemoveCode (&Arg1.Expr.Start);

        /* We need a label */
        Label = GetLocalLabel ();

        /* Generate memcpy code */
        if (Arg3.Expr.IVal <= 127 && !AllowOneIndex) {

            if (Offs == 0) {
                AddCodeLine ("ldy #$%02X", (unsigned char) (Offs + Arg3.Expr.IVal - 1));
                g_defcodelabel (Label);
                AddCodeLine ("lda %s,y", ED_GetLabelName (&Arg2.Expr, -Offs));
                AddCodeLine ("sta (sp),y");
                AddCodeLine ("dey");
                AddCodeLine ("bpl %s", LocalLabelName (Label));
            } else {
                AddCodeLine ("ldx #$%02X", (unsigned char) (Arg3.Expr.IVal-1));
                AddCodeLine ("ldy #$%02X", (unsigned char) (Offs + Arg3.Expr.IVal - 1));
                g_defcodelabel (Label);
                AddCodeLine ("lda %s,x", ED_GetLabelName (&Arg2.Expr, 0));
                AddCodeLine ("sta (sp),y");
                AddCodeLine ("dey");
                AddCodeLine ("dex");
                AddCodeLine ("bpl %s", LocalLabelName (Label));
            }

        } else {

            if (Offs == 0 || AllowOneIndex) {
                AddCodeLine ("ldy #$%02X", (unsigned char) Offs);
                g_defcodelabel (Label);
                AddCodeLine ("lda %s,y", ED_GetLabelName (&Arg2.Expr, -Offs));
                AddCodeLine ("sta (sp),y");
                AddCodeLine ("iny");
                AddCodeLine ("cpy #$%02X", (unsigned char) (Offs + Arg3.Expr.IVal));
                AddCodeLine ("bne %s", LocalLabelName (Label));
            } else {
                AddCodeLine ("ldx #$00");
                AddCodeLine ("ldy #$%02X", (unsigned char) Offs);
                g_defcodelabel (Label);
                AddCodeLine ("lda %s,x", ED_GetLabelName (&Arg2.Expr, 0));
                AddCodeLine ("sta (sp),y");
                AddCodeLine ("iny");
                AddCodeLine ("inx");
                AddCodeLine ("cpx #$%02X", (unsigned char) Arg3.Expr.IVal);
                AddCodeLine ("bne %s", LocalLabelName (Label));
            }

        }

        /* memcpy returns the address, so the result is actually identical
        ** to the first argument.
        */
        *Expr = Arg1.Expr;

    } else if (ED_IsConstAbsInt (&Arg3.Expr) && Arg3.Expr.IVal <= 256 &&
               ED_IsRVal (&Arg2.Expr) && ED_IsLocStack (&Arg2.Expr) &&
               (Arg2.Expr.IVal - StackPtr) + Arg3.Expr.IVal < 256 &&
               ED_IsRVal (&Arg1.Expr) && ED_IsLocConst (&Arg1.Expr)) {

        /* It is possible to just use one index register even if the stack
        ** offset is not zero, by adjusting the offset to the constant
        ** address accordingly. But we cannot do this if the data in
        ** question is in the register space or at an absolute address less
        ** than 256. Register space is zero page, which means that the
        ** address calculation could overflow in the linker.
        */
        int AllowOneIndex = !ED_IsLocRegister (&Arg1.Expr) &&
                            !(ED_IsLocAbs (&Arg1.Expr) && Arg1.Expr.IVal < 256);

        /* Calculate the real stack offset */
        Offs = ED_GetStackOffs (&Arg2.Expr, 0);

        /* Drop the generated code */
        RemoveCode (&Arg1.Expr.Start);

        /* We need a label */
        Label = GetLocalLabel ();

        /* Generate memcpy code */
        if (Arg3.Expr.IVal <= 127 && !AllowOneIndex) {

            if (Offs == 0) {
                AddCodeLine ("ldy #$%02X", (unsigned char) (Arg3.Expr.IVal - 1));
                g_defcodelabel (Label);
                AddCodeLine ("lda (sp),y");
                AddCodeLine ("sta %s,y", ED_GetLabelName (&Arg1.Expr, 0));
                AddCodeLine ("dey");
                AddCodeLine ("bpl %s", LocalLabelName (Label));
            } else {
                AddCodeLine ("ldx #$%02X", (unsigned char) (Arg3.Expr.IVal-1));
                AddCodeLine ("ldy #$%02X", (unsigned char) (Offs + Arg3.Expr.IVal - 1));
                g_defcodelabel (Label);
                AddCodeLine ("lda (sp),y");
                AddCodeLine ("sta %s,x", ED_GetLabelName (&Arg1.Expr, 0));
                AddCodeLine ("dey");
                AddCodeLine ("dex");
                AddCodeLine ("bpl %s", LocalLabelName (Label));
            }

        } else {

            if (Offs == 0 || AllowOneIndex) {
                AddCodeLine ("ldy #$%02X", (unsigned char) Offs);
                g_defcodelabel (Label);
                AddCodeLine ("lda (sp),y");
                AddCodeLine ("sta %s,y", ED_GetLabelName (&Arg1.Expr, -Offs));
                AddCodeLine ("iny");
                AddCodeLine ("cpy #$%02X", (unsigned char) (Offs + Arg3.Expr.IVal));
                AddCodeLine ("bne %s", LocalLabelName (Label));
            } else {
                AddCodeLine ("ldx #$00");
                AddCodeLine ("ldy #$%02X", (unsigned char) Offs);
                g_defcodelabel (Label);
                AddCodeLine ("lda (sp),y");
                AddCodeLine ("sta %s,x", ED_GetLabelName (&Arg1.Expr, 0));
                AddCodeLine ("iny");
                AddCodeLine ("inx");
                AddCodeLine ("cpx #$%02X", (unsigned char) Arg3.Expr.IVal);
                AddCodeLine ("bne %s", LocalLabelName (Label));
            }

        }

        /* memcpy returns the address, so the result is actually identical
        ** to the first argument.
        */
        *Expr = Arg1.Expr;

    } else if (ED_IsConstAbsInt (&Arg3.Expr) && Arg3.Expr.IVal <= 256   &&
               ED_IsRVal (&Arg2.Expr) && ED_IsLocStack (&Arg2.Expr)     &&
               (Offs = ED_GetStackOffs (&Arg2.Expr, 0)) == 0) {

        /* Drop the generated code but leave the load of the first argument*/
        RemoveCode (&Arg1.Push);

        /* We need a label */
        Label = GetLocalLabel ();

        /* Generate memcpy code */
        AddCodeLine ("sta ptr1");
        AddCodeLine ("stx ptr1+1");
        if (Arg3.Expr.IVal <= 127) {
            AddCodeLine ("ldy #$%02X", (unsigned char) (Arg3.Expr.IVal - 1));
            g_defcodelabel (Label);
            AddCodeLine ("lda (sp),y");
            AddCodeLine ("sta (ptr1),y");
            AddCodeLine ("dey");
            AddCodeLine ("bpl %s", LocalLabelName (Label));
        } else {
            AddCodeLine ("ldy #$00");
            g_defcodelabel (Label);
            AddCodeLine ("lda (sp),y");
            AddCodeLine ("sta (ptr1),y");
            AddCodeLine ("iny");
            AddCodeLine ("cpy #$%02X", (unsigned char) Arg3.Expr.IVal);
            AddCodeLine ("bne %s", LocalLabelName (Label));
        }

        /* Reload result - X hasn't changed by the code above */
        AddCodeLine ("lda ptr1");

        /* The function result is an rvalue in the primary register */
        ED_MakeRValExpr (Expr);
        Expr->Type = GetFuncReturn (Expr->Type);

    } else {

        /* The function result is an rvalue in the primary register */
        ED_MakeRValExpr (Expr);
        Expr->Type = GetFuncReturn (Expr->Type);

    }

ExitPoint:
    /* We expect the closing brace */
    ConsumeRParen ();
}



/*****************************************************************************/
/*                                  memset                                   */
/*****************************************************************************/



static void StdFunc_memset (FuncDesc* F attribute ((unused)), ExprDesc* Expr)
/* Handle the memset function */
{
    /* Argument types: (void*, int, size_t) */
    static Type Arg1Type[] = { TYPE(T_PTR), TYPE(T_VOID), TYPE(T_END) };
    static Type Arg2Type[] = { TYPE(T_INT), TYPE(T_END) };
    static Type Arg3Type[] = { TYPE(T_SIZE_T), TYPE(T_END) };

    ArgDesc  Arg1, Arg2, Arg3;
    int      MemSet    = 1;             /* Use real memset if true */
    unsigned ParamSize = 0;
    unsigned Label;

    /* Argument #1 */
    ParseArg (&Arg1, Arg1Type);
    g_push (Arg1.Flags, Arg1.Expr.IVal);
    GetCodePos (&Arg1.End);
    ParamSize += SizeOf (Arg1Type);
    ConsumeComma ();

    /* Argument #2. This argument is special in that we will call another
    ** function if it is a constant zero.
    */
    ParseArg (&Arg2, Arg2Type);
    if ((Arg2.Flags & CF_CONST) != 0 && Arg2.Expr.IVal == 0) {
        /* Don't call memset, call bzero instead */
        MemSet = 0;
    } else {
        /* Push the argument */
        g_push (Arg2.Flags, Arg2.Expr.IVal);
        GetCodePos (&Arg2.End);
        ParamSize += SizeOf (Arg2Type);
    }
    ConsumeComma ();

    /* Argument #3. Since memset is a fastcall function, we must load the
    ** arg into the primary if it is not already there. This parameter is
    ** also ignored for the calculation of the parameter size, since it is
    ** not passed via the stack.
    */
    ParseArg (&Arg3, Arg3Type);
    if (Arg3.Flags & CF_CONST) {
        LoadExpr (CF_NONE, &Arg3.Expr);
    }

    /* Emit the actual function call. This will also cleanup the stack. */
    g_call (CF_FIXARGC, MemSet? Func_memset : Func__bzero, ParamSize);

    if (ED_IsConstAbsInt (&Arg3.Expr) && Arg3.Expr.IVal == 0) {

        /* memset has been called with a count argument of zero */
        Warning ("Call to memset has no effect");

        /* Remove all of the generated code but the load of the first
        ** argument, which is what memset returns.
        */
        RemoveCode (&Arg1.Push);

        /* Set the function result to the first argument */
        *Expr = Arg1.Expr;

        /* Bail out, no need for further improvements */
        goto ExitPoint;
    }

    /* We've generated the complete code for the function now and know the
    ** types of all parameters. Check for situations where better code can
    ** be generated. If such a situation is detected, throw away the
    ** generated, and emit better code.
    ** Note: Lots of improvements would be possible here, but I will
    ** concentrate on the most common case: memset with arguments 2 and 3
    ** being constant numerical values. Some checks have shown that this
    ** covers nearly 90% of all memset calls.
    */
    if (ED_IsConstAbsInt (&Arg3.Expr) && Arg3.Expr.IVal <= 256 &&
        ED_IsConstAbsInt (&Arg2.Expr) &&
        ((ED_IsRVal (&Arg1.Expr) && ED_IsLocConst (&Arg1.Expr)) ||
         (ED_IsLVal (&Arg1.Expr) && ED_IsLocRegister (&Arg1.Expr)))) {

        int Reg = ED_IsLVal (&Arg1.Expr) && ED_IsLocRegister (&Arg1.Expr);

        /* Drop the generated code */
        RemoveCode (&Arg1.Expr.Start);

        /* We need a label */
        Label = GetLocalLabel ();

        /* Generate memset code */
        if (Arg3.Expr.IVal <= 127) {

            AddCodeLine ("ldy #$%02X", (unsigned char) (Arg3.Expr.IVal-1));
            AddCodeLine ("lda #$%02X", (unsigned char) Arg2.Expr.IVal);
            g_defcodelabel (Label);
            if (Reg) {
                AddCodeLine ("sta (%s),y", ED_GetLabelName (&Arg1.Expr, 0));
            } else {
                AddCodeLine ("sta %s,y", ED_GetLabelName (&Arg1.Expr, 0));
            }
            AddCodeLine ("dey");
            AddCodeLine ("bpl %s", LocalLabelName (Label));

        } else {

            AddCodeLine ("ldy #$00");
            AddCodeLine ("lda #$%02X", (unsigned char) Arg2.Expr.IVal);
            g_defcodelabel (Label);
            if (Reg) {
                AddCodeLine ("sta (%s),y", ED_GetLabelName (&Arg1.Expr, 0));
            } else {
                AddCodeLine ("sta %s,y", ED_GetLabelName (&Arg1.Expr, 0));
            }
            AddCodeLine ("iny");
            AddCodeLine ("cpy #$%02X", (unsigned char) Arg3.Expr.IVal);
            AddCodeLine ("bne %s", LocalLabelName (Label));

        }

        /* memset returns the address, so the result is actually identical
        ** to the first argument.
        */
        *Expr = Arg1.Expr;

    } else if (ED_IsConstAbsInt (&Arg3.Expr) && Arg3.Expr.IVal <= 256 &&
               ED_IsConstAbsInt (&Arg2.Expr) &&
               ED_IsRVal (&Arg1.Expr) && ED_IsLocStack (&Arg1.Expr) &&
               (Arg1.Expr.IVal - StackPtr) + Arg3.Expr.IVal < 256) {

        /* Calculate the real stack offset */
        int Offs = ED_GetStackOffs (&Arg1.Expr, 0);

        /* Drop the generated code */
        RemoveCode (&Arg1.Expr.Start);

        /* We need a label */
        Label = GetLocalLabel ();

        /* Generate memset code */
        AddCodeLine ("ldy #$%02X", (unsigned char) Offs);
        AddCodeLine ("lda #$%02X", (unsigned char) Arg2.Expr.IVal);
        g_defcodelabel (Label);
        AddCodeLine ("sta (sp),y");
        AddCodeLine ("iny");
        AddCodeLine ("cpy #$%02X", (unsigned char) (Offs + Arg3.Expr.IVal));
        AddCodeLine ("bne %s", LocalLabelName (Label));

        /* memset returns the address, so the result is actually identical
        ** to the first argument.
        */
        *Expr = Arg1.Expr;

    } else if (ED_IsConstAbsInt (&Arg3.Expr) && Arg3.Expr.IVal <= 256 &&
               ED_IsConstAbsInt (&Arg2.Expr) &&
               (Arg2.Expr.IVal != 0 || IS_Get (&CodeSizeFactor) > 200)) {

        /* Remove all of the generated code but the load of the first
        ** argument.
        */
        RemoveCode (&Arg1.Push);

        /* We need a label */
        Label = GetLocalLabel ();

        /* Generate code */
        AddCodeLine ("sta ptr1");
        AddCodeLine ("stx ptr1+1");
        if (Arg3.Expr.IVal <= 127) {
            AddCodeLine ("ldy #$%02X", (unsigned char) (Arg3.Expr.IVal-1));
            AddCodeLine ("lda #$%02X", (unsigned char) Arg2.Expr.IVal);
            g_defcodelabel (Label);
            AddCodeLine ("sta (ptr1),y");
            AddCodeLine ("dey");
            AddCodeLine ("bpl %s", LocalLabelName (Label));
        } else {
            AddCodeLine ("ldy #$00");
            AddCodeLine ("lda #$%02X", (unsigned char) Arg2.Expr.IVal);
            g_defcodelabel (Label);
            AddCodeLine ("sta (ptr1),y");
            AddCodeLine ("iny");
            AddCodeLine ("cpy #$%02X", (unsigned char) Arg3.Expr.IVal);
            AddCodeLine ("bne %s", LocalLabelName (Label));
        }

        /* Load the function result pointer into a/x (x is still valid). This
        ** code will get removed by the optimizer if it is not used later.
        */
        AddCodeLine ("lda ptr1");

        /* The function result is an rvalue in the primary register */
        ED_MakeRValExpr (Expr);
        Expr->Type = GetFuncReturn (Expr->Type);

    } else {

        /* The function result is an rvalue in the primary register */
        ED_MakeRValExpr (Expr);
        Expr->Type = GetFuncReturn (Expr->Type);

    }

ExitPoint:
    /* We expect the closing brace */
    ConsumeRParen ();
}



/*****************************************************************************/
/*                                  strcmp                                   */
/*****************************************************************************/



static void StdFunc_strcmp (FuncDesc* F attribute ((unused)), ExprDesc* Expr)
/* Handle the strcmp function */
{
    /* Argument types: (const char*, const char*) */
    static Type Arg1Type[] = { TYPE(T_PTR), TYPE(T_CHAR|T_QUAL_CONST), TYPE(T_END) };
    static Type Arg2Type[] = { TYPE(T_PTR), TYPE(T_CHAR|T_QUAL_CONST), TYPE(T_END) };

    ArgDesc  Arg1, Arg2;
    unsigned ParamSize = 0;
    long     ECount1;
    long     ECount2;
    int      IsArray;
    int      Offs;

    /* Setup the argument type string */
    Arg1Type[1].C = GetDefaultChar () | T_QUAL_CONST;
    Arg2Type[1].C = GetDefaultChar () | T_QUAL_CONST;

    /* Argument #1 */
    ParseArg (&Arg1, Arg1Type);
    g_push (Arg1.Flags, Arg1.Expr.IVal);
    ParamSize += SizeOf (Arg1Type);
    ConsumeComma ();

    /* Argument #2. */
    ParseArg (&Arg2, Arg2Type);

    /* Since strcmp is a fastcall function, we must load the
    ** arg into the primary if it is not already there. This parameter is
    ** also ignored for the calculation of the parameter size, since it is
    ** not passed via the stack.
    */
    if (Arg2.Flags & CF_CONST) {
        LoadExpr (CF_NONE, &Arg2.Expr);
    }

    /* Emit the actual function call. This will also cleanup the stack. */
    g_call (CF_FIXARGC, Func_strcmp, ParamSize);

    /* Get the element counts of the arguments. Then get the larger of the
    ** two into ECount1. This removes FLEXIBLE and UNSPECIFIED automatically
    */
    ECount1 = ArrayElementCount (&Arg1);
    ECount2 = ArrayElementCount (&Arg2);
    if (ECount2 > ECount1) {
        ECount1 = ECount2;
    }

    /* If the second argument is the empty string literal, we can generate
    ** more efficient code.
    */
    if (ED_IsLocLiteral (&Arg2.Expr) &&
        IS_Get (&WritableStrings) == 0 &&
        GetLiteralSize (Arg2.Expr.LVal) == 1 &&
        GetLiteralStr (Arg2.Expr.LVal)[0] == '\0') {

        /* Drop the generated code so we have the first argument in the
        ** primary
        */
        RemoveCode (&Arg1.Push);

        /* We don't need the literal any longer */
        ReleaseLiteral (Arg2.Expr.LVal);

        /* We do now have Arg1 in the primary. Load the first character from
        ** this string and cast to int. This is the function result.
        */
        IsArray = IsTypeArray (Arg1.Type) && ED_IsRVal (&Arg1.Expr);
        if (IsArray && ED_IsLocStack (&Arg1.Expr) &&
            (Offs = ED_GetStackOffs (&Arg1.Expr, 0) < 256)) {
            /* Drop the generated code */
            RemoveCode (&Arg1.Load);

            /* Generate code */
            AddCodeLine ("ldy #$%02X", Offs);
            AddCodeLine ("ldx #$00");
            AddCodeLine ("lda (sp),y");
        } else if (IsArray && ED_IsLocConst (&Arg1.Expr)) {
            /* Drop the generated code */
            RemoveCode (&Arg1.Load);

            /* Generate code */
            AddCodeLine ("ldx #$00");
            AddCodeLine ("lda %s", ED_GetLabelName (&Arg1.Expr, 0));
        } else {
            /* Drop part of the generated code so we have the first argument
            ** in the primary
            */
            RemoveCode (&Arg1.Push);

            /* Fetch the first char */
            g_getind (CF_CHAR | CF_UNSIGNED, 0);
        }

    } else if ((IS_Get (&CodeSizeFactor) >= 165) &&
               ((ED_IsRVal (&Arg2.Expr) && ED_IsLocConst (&Arg2.Expr)) ||
                (ED_IsLVal (&Arg2.Expr) && ED_IsLocRegister (&Arg2.Expr))) &&
               ((ED_IsRVal (&Arg1.Expr) && ED_IsLocConst (&Arg1.Expr)) ||
                (ED_IsLVal (&Arg1.Expr) && ED_IsLocRegister (&Arg1.Expr))) &&
               (IS_Get (&InlineStdFuncs) || (ECount1 > 0 && ECount1 < 256))) {


        unsigned    Entry, Loop, Fin;   /* Labels */
        const char* Load;
        const char* Compare;

        if (ED_IsLVal (&Arg1.Expr) && ED_IsLocRegister (&Arg1.Expr)) {
            Load = "lda (%s),y";
        } else {
            Load = "lda %s,y";
        }
        if (ED_IsLVal (&Arg2.Expr) && ED_IsLocRegister (&Arg2.Expr)) {
            Compare = "cmp (%s),y";
        } else {
            Compare = "cmp %s,y";
        }

        /* Drop the generated code */
        RemoveCode (&Arg1.Expr.Start);

        /* We need labels */
        Entry = GetLocalLabel ();
        Loop  = GetLocalLabel ();
        Fin   = GetLocalLabel ();

        /* Generate strcmp code */
        AddCodeLine ("ldy #$00");
        AddCodeLine ("beq %s", LocalLabelName (Entry));
        g_defcodelabel (Loop);
        AddCodeLine ("tax");
        AddCodeLine ("beq %s", LocalLabelName (Fin));
        AddCodeLine ("iny");
        g_defcodelabel (Entry);
        AddCodeLine (Load, ED_GetLabelName (&Arg1.Expr, 0));
        AddCodeLine (Compare, ED_GetLabelName (&Arg2.Expr, 0));
        AddCodeLine ("beq %s", LocalLabelName (Loop));
        AddCodeLine ("ldx #$01");
        AddCodeLine ("bcs %s", LocalLabelName (Fin));
        AddCodeLine ("ldx #$FF");
        g_defcodelabel (Fin);

    } else if ((IS_Get (&CodeSizeFactor) > 190) &&
               ((ED_IsRVal (&Arg2.Expr) && ED_IsLocConst (&Arg2.Expr)) ||
                (ED_IsLVal (&Arg2.Expr) && ED_IsLocRegister (&Arg2.Expr))) &&
               (IS_Get (&InlineStdFuncs) || (ECount1 > 0 && ECount1 < 256))) {


        unsigned    Entry, Loop, Fin;   /* Labels */
        const char* Compare;

        if (ED_IsLVal (&Arg2.Expr) && ED_IsLocRegister (&Arg2.Expr)) {
            Compare = "cmp (%s),y";
        } else {
            Compare = "cmp %s,y";
        }

        /* Drop the generated code */
        RemoveCode (&Arg1.Push);

        /* We need labels */
        Entry = GetLocalLabel ();
        Loop  = GetLocalLabel ();
        Fin   = GetLocalLabel ();

        /* Store Arg1 into ptr1 */
        AddCodeLine ("sta ptr1");
        AddCodeLine ("stx ptr1+1");

        /* Generate strcmp code */
        AddCodeLine ("ldy #$00");
        AddCodeLine ("beq %s", LocalLabelName (Entry));
        g_defcodelabel (Loop);
        AddCodeLine ("tax");
        AddCodeLine ("beq %s", LocalLabelName (Fin));
        AddCodeLine ("iny");
        g_defcodelabel (Entry);
        AddCodeLine ("lda (ptr1),y");
        AddCodeLine (Compare, ED_GetLabelName (&Arg2.Expr, 0));
        AddCodeLine ("beq %s", LocalLabelName (Loop));
        AddCodeLine ("ldx #$01");
        AddCodeLine ("bcs %s", LocalLabelName (Fin));
        AddCodeLine ("ldx #$FF");
        g_defcodelabel (Fin);

    }

    /* The function result is an rvalue in the primary register */
    ED_MakeRValExpr (Expr);
    Expr->Type = GetFuncReturn (Expr->Type);

    /* We expect the closing brace */
    ConsumeRParen ();
}



/*****************************************************************************/
/*                                  strcpy                                   */
/*****************************************************************************/



static void StdFunc_strcpy (FuncDesc* F attribute ((unused)), ExprDesc* Expr)
/* Handle the strcpy function */
{
    /* Argument types: (char*, const char*) */
    static Type Arg1Type[] = { TYPE(T_PTR), TYPE(T_CHAR), TYPE(T_END) };
    static Type Arg2Type[] = { TYPE(T_PTR), TYPE(T_CHAR|T_QUAL_CONST), TYPE(T_END) };

    ArgDesc  Arg1, Arg2;
    unsigned ParamSize = 0;
    long     ECount;
    unsigned L1;

    /* Setup the argument type string */
    Arg1Type[1].C = GetDefaultChar ();
    Arg2Type[1].C = GetDefaultChar () | T_QUAL_CONST;

    /* Argument #1 */
    ParseArg (&Arg1, Arg1Type);
    g_push (Arg1.Flags, Arg1.Expr.IVal);
    GetCodePos (&Arg1.End);
    ParamSize += SizeOf (Arg1Type);
    ConsumeComma ();

    /* Argument #2. Since strcpy is a fastcall function, we must load the
    ** arg into the primary if it is not already there. This parameter is
    ** also ignored for the calculation of the parameter size, since it is
    ** not passed via the stack.
    */
    ParseArg (&Arg2, Arg2Type);
    if (Arg2.Flags & CF_CONST) {
        LoadExpr (CF_NONE, &Arg2.Expr);
    }

    /* Emit the actual function call. This will also cleanup the stack. */
    g_call (CF_FIXARGC, Func_strcpy, ParamSize);

    /* Get the element count of argument 1 if it is an array */
    ECount = ArrayElementCount (&Arg1);

    /* We've generated the complete code for the function now and know the
    ** types of all parameters. Check for situations where better code can
    ** be generated. If such a situation is detected, throw away the
    ** generated, and emit better code.
    */
    if (((ED_IsRVal (&Arg2.Expr) && ED_IsLocConst (&Arg2.Expr)) ||
         (ED_IsLVal (&Arg2.Expr) && ED_IsLocRegister (&Arg2.Expr))) &&
        ((ED_IsRVal (&Arg1.Expr) && ED_IsLocConst (&Arg1.Expr)) ||
         (ED_IsLVal (&Arg1.Expr) && ED_IsLocRegister (&Arg1.Expr))) &&
        (IS_Get (&InlineStdFuncs) ||
        (ECount != UNSPECIFIED && ECount < 256))) {

        const char* Load;
        const char* Store;
        if (ED_IsLVal (&Arg2.Expr) && ED_IsLocRegister (&Arg2.Expr)) {
            Load = "lda (%s),y";
        } else {
            Load = "lda %s,y";
        }
        if (ED_IsLVal (&Arg1.Expr) && ED_IsLocRegister (&Arg1.Expr)) {
            Store = "sta (%s),y";
        } else {
            Store = "sta %s,y";
        }

        /* Drop the generated code */
        RemoveCode (&Arg1.Expr.Start);

        /* We need labels */
        L1 = GetLocalLabel ();

        /* Generate strcpy code */
        AddCodeLine ("ldy #$FF");
        g_defcodelabel (L1);
        AddCodeLine ("iny");
        AddCodeLine (Load, ED_GetLabelName (&Arg2.Expr, 0));
        AddCodeLine (Store, ED_GetLabelName (&Arg1.Expr, 0));
        AddCodeLine ("bne %s", LocalLabelName (L1));

        /* strcpy returns argument #1 */
        *Expr = Arg1.Expr;

    } else if (ED_IsRVal (&Arg2.Expr) && ED_IsLocStack (&Arg2.Expr) &&
               StackPtr >= -255 &&
               ED_IsRVal (&Arg1.Expr) && ED_IsLocConst (&Arg1.Expr)) {

        /* It is possible to just use one index register even if the stack
        ** offset is not zero, by adjusting the offset to the constant
        ** address accordingly. But we cannot do this if the data in
        ** question is in the register space or at an absolute address less
        ** than 256. Register space is zero page, which means that the
        ** address calculation could overflow in the linker.
        */
        int AllowOneIndex = !ED_IsLocRegister (&Arg1.Expr) &&
                            !(ED_IsLocAbs (&Arg1.Expr) && Arg1.Expr.IVal < 256);

        /* Calculate the real stack offset */
        int Offs = ED_GetStackOffs (&Arg2.Expr, 0);

        /* Drop the generated code */
        RemoveCode (&Arg1.Expr.Start);

        /* We need labels */
        L1 = GetLocalLabel ();

        /* Generate strcpy code */
        AddCodeLine ("ldy #$%02X", (unsigned char) (Offs - 1));
        if (Offs == 0 || AllowOneIndex) {
            g_defcodelabel (L1);
            AddCodeLine ("iny");
            AddCodeLine ("lda (sp),y");
            AddCodeLine ("sta %s,y", ED_GetLabelName (&Arg1.Expr, -Offs));
        } else {
            AddCodeLine ("ldx #$FF");
            g_defcodelabel (L1);
            AddCodeLine ("iny");
            AddCodeLine ("inx");
            AddCodeLine ("lda (sp),y");
            AddCodeLine ("sta %s,x", ED_GetLabelName (&Arg1.Expr, 0));
        }
        AddCodeLine ("bne %s", LocalLabelName (L1));

        /* strcpy returns argument #1 */
        *Expr = Arg1.Expr;

    } else if (ED_IsRVal (&Arg2.Expr) && ED_IsLocConst (&Arg2.Expr) &&
               ED_IsRVal (&Arg1.Expr) && ED_IsLocStack (&Arg1.Expr) &&
               StackPtr >= -255) {

        /* It is possible to just use one index register even if the stack
        ** offset is not zero, by adjusting the offset to the constant
        ** address accordingly. But we cannot do this if the data in
        ** question is in the register space or at an absolute address less
        ** than 256. Register space is zero page, which means that the
        ** address calculation could overflow in the linker.
        */
        int AllowOneIndex = !ED_IsLocRegister (&Arg2.Expr) &&
                            !(ED_IsLocAbs (&Arg2.Expr) && Arg2.Expr.IVal < 256);

        /* Calculate the real stack offset */
        int Offs = ED_GetStackOffs (&Arg1.Expr, 0);

        /* Drop the generated code */
        RemoveCode (&Arg1.Expr.Start);

        /* We need labels */
        L1 = GetLocalLabel ();

        /* Generate strcpy code */
        AddCodeLine ("ldy #$%02X", (unsigned char) (Offs - 1));
        if (Offs == 0 || AllowOneIndex) {
            g_defcodelabel (L1);
            AddCodeLine ("iny");
            AddCodeLine ("lda %s,y", ED_GetLabelName (&Arg2.Expr, -Offs));
            AddCodeLine ("sta (sp),y");
        } else {
            AddCodeLine ("ldx #$FF");
            g_defcodelabel (L1);
            AddCodeLine ("iny");
            AddCodeLine ("inx");
            AddCodeLine ("lda %s,x", ED_GetLabelName (&Arg2.Expr, 0));
            AddCodeLine ("sta (sp),y");
        }
        AddCodeLine ("bne %s", LocalLabelName (L1));

        /* strcpy returns argument #1 */
        *Expr = Arg1.Expr;

    } else {

        /* The function result is an rvalue in the primary register */
        ED_MakeRValExpr (Expr);
        Expr->Type = GetFuncReturn (Expr->Type);

    }

    /* We expect the closing brace */
    ConsumeRParen ();
}



/*****************************************************************************/
/*                                  strlen                                   */
/*****************************************************************************/



static void StdFunc_strlen (FuncDesc* F attribute ((unused)), ExprDesc* Expr)
/* Handle the strlen function */
{
    static Type ArgType[] = { TYPE(T_PTR), TYPE(T_CHAR|T_QUAL_CONST), TYPE(T_END) };
    ExprDesc    Arg;
    int         IsArray;
    int         IsPtr;
    int         IsByteIndex;
    long        ECount;
    unsigned    L;



    /* Setup the argument type string */
    ArgType[1].C = GetDefaultChar () | T_QUAL_CONST;

    /* Evaluate the parameter */
    hie1 (&Arg);

    /* Check if the argument is an array. If so, remember the element count.
    ** Otherwise set the element count to undefined.
    */
    IsArray = IsTypeArray (Arg.Type);
    if (IsArray) {
        ECount = GetElementCount (Arg.Type);
        if (ECount == FLEXIBLE) {
            /* Treat as unknown */
            ECount = UNSPECIFIED;
        }
        IsPtr = 0;
    } else {
        ECount = UNSPECIFIED;
        IsPtr  = IsTypePtr (Arg.Type);
    }

    /* Check if the elements of an array can be addressed by a byte sized
    ** index. This is true if the size of the array is known and less than
    ** 256.
    */
    IsByteIndex = (ECount != UNSPECIFIED && ECount < 256);

    /* Do type conversion */
    TypeConversion (&Arg, ArgType);

    /* If the expression is a literal, and if string literals are read
    ** only, we can calculate the length of the string and remove it
    ** from the literal pool. Otherwise we have to calculate the length
    ** at runtime.
    */
    if (ED_IsLocLiteral (&Arg) && IS_Get (&WritableStrings) == 0) {

        /* Constant string literal */
        ED_MakeConstAbs (Expr, GetLiteralSize (Arg.LVal) - 1, type_size_t);

        /* We don't need the literal any longer */
        ReleaseLiteral (Arg.LVal);

    /* We will inline strlen for arrays with constant addresses, if either the
    ** inlining was forced on the command line, or the array is smaller than
    ** 256, so the inlining is considered safe.
    */
    } else if (ED_IsLocConst (&Arg) && IsArray &&
               (IS_Get (&InlineStdFuncs) || IsByteIndex)) {

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

    /* We will inline strlen for arrays on the stack, if the array is
    ** completely within the reach of a byte sized index register.
    */
    } else if (ED_IsLocStack (&Arg) && IsArray && IsByteIndex &&
               (Arg.IVal - StackPtr) + ECount < 256) {

        /* Calculate the true stack offset */
        int Offs = ED_GetStackOffs (&Arg, 0);

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

    /* strlen for a string that is pointed to by a register variable will only
    ** get inlined if requested on the command line, since we cannot know how
    ** big the buffer actually is, so inlining is not always safe.
    */
    } else if (ED_IsLocRegister (&Arg) && ED_IsLVal (&Arg) && IsPtr &&
               IS_Get (&InlineStdFuncs)) {

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

    /* Last check: We will inline a generic strlen routine if inlining was
    ** requested on the command line, and the code size factor is more than
    ** 400 (code is 13 bytes vs. 3 for a jsr call).
    */
    } else if (IS_Get (&CodeSizeFactor) > 400 && IS_Get (&InlineStdFuncs)) {

        /* Load the expression into the primary */
        LoadExpr (CF_NONE, &Arg);

        /* Inline the function */
        L = GetLocalLabel ();
        AddCodeLine ("sta ptr1");
        AddCodeLine ("stx ptr1+1");
        AddCodeLine ("ldy #$FF");
        g_defcodelabel (L);
        AddCodeLine ("iny");
        AddCodeLine ("lda (ptr1),y");
        AddCodeLine ("bne %s", LocalLabelName (L));
        AddCodeLine ("tax");
        AddCodeLine ("tya");

        /* The function result is an rvalue in the primary register */
        ED_MakeRValExpr (Expr);
        Expr->Type = type_size_t;

    } else {

        /* Load the expression into the primary */
        LoadExpr (CF_NONE, &Arg);

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
/*                                   Code                                    */
/*****************************************************************************/



int FindStdFunc (const char* Name)
/* Determine if the given function is a known standard function that may be
** called in a special way. If so, return the index, otherwise return -1.
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
