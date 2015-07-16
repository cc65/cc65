/* expr.c
**
** 1998-06-21, Ullrich von Bassewitz
** 2015-06-26, Greg King
*/



#include <stdio.h>
#include <stdlib.h>

/* common */
#include "check.h"
#include "debugflag.h"
#include "xmalloc.h"

/* cc65 */
#include "asmcode.h"
#include "asmlabel.h"
#include "asmstmt.h"
#include "assignment.h"
#include "codegen.h"
#include "declare.h"
#include "error.h"
#include "funcdesc.h"
#include "function.h"
#include "global.h"
#include "litpool.h"
#include "loadexpr.h"
#include "macrotab.h"
#include "preproc.h"
#include "scanner.h"
#include "shiftexpr.h"
#include "stackptr.h"
#include "standard.h"
#include "stdfunc.h"
#include "symtab.h"
#include "typecmp.h"
#include "typeconv.h"
#include "expr.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Generator attributes */
#define GEN_NOPUSH      0x01            /* Don't push lhs */
#define GEN_COMM        0x02            /* Operator is commutative */
#define GEN_NOFUNC      0x04            /* Not allowed for function pointers */

/* Map a generator function and its attributes to a token */
typedef struct {
    token_t       Tok;                  /* Token to map to */
    unsigned      Flags;                /* Flags for generator function */
    void          (*Func) (unsigned, unsigned long);    /* Generator func */
} GenDesc;

/* Descriptors for the operations */
static GenDesc GenPASGN  = { TOK_PLUS_ASSIGN,   GEN_NOPUSH,     g_add };
static GenDesc GenSASGN  = { TOK_MINUS_ASSIGN,  GEN_NOPUSH,     g_sub };
static GenDesc GenMASGN  = { TOK_MUL_ASSIGN,    GEN_NOPUSH,     g_mul };
static GenDesc GenDASGN  = { TOK_DIV_ASSIGN,    GEN_NOPUSH,     g_div };
static GenDesc GenMOASGN = { TOK_MOD_ASSIGN,    GEN_NOPUSH,     g_mod };
static GenDesc GenSLASGN = { TOK_SHL_ASSIGN,    GEN_NOPUSH,     g_asl };
static GenDesc GenSRASGN = { TOK_SHR_ASSIGN,    GEN_NOPUSH,     g_asr };
static GenDesc GenAASGN  = { TOK_AND_ASSIGN,    GEN_NOPUSH,     g_and };
static GenDesc GenXOASGN = { TOK_XOR_ASSIGN,    GEN_NOPUSH,     g_xor };
static GenDesc GenOASGN  = { TOK_OR_ASSIGN,     GEN_NOPUSH,     g_or  };



/*****************************************************************************/
/*                             Helper functions                              */
/*****************************************************************************/



static unsigned GlobalModeFlags (const ExprDesc* Expr)
/* Return the addressing mode flags for the given expression */
{
    switch (ED_GetLoc (Expr)) {
        case E_LOC_ABS:         return CF_ABSOLUTE;
        case E_LOC_GLOBAL:      return CF_EXTERNAL;
        case E_LOC_STATIC:      return CF_STATIC;
        case E_LOC_REGISTER:    return CF_REGVAR;
        case E_LOC_STACK:       return CF_NONE;
        case E_LOC_PRIMARY:     return CF_NONE;
        case E_LOC_EXPR:        return CF_NONE;
        case E_LOC_LITERAL:     return CF_STATIC;       /* Same as static */
        default:
            Internal ("GlobalModeFlags: Invalid location flags value: 0x%04X", Expr->Flags);
            /* NOTREACHED */
            return 0;
    }
}



void ExprWithCheck (void (*Func) (ExprDesc*), ExprDesc* Expr)
/* Call an expression function with checks. */
{
    /* Remember the stack pointer */
    int OldSP = StackPtr;

    /* Call the expression function */
    (*Func) (Expr);

    /* Do some checks to see if code generation is still consistent */
    if (StackPtr != OldSP) {
        if (Debug) {
            Error ("Code generation messed up: "
                   "StackPtr is %d, should be %d",
                   StackPtr, OldSP);
        } else {
            Internal ("Code generation messed up: "
                      "StackPtr is %d, should be %d",
                      StackPtr, OldSP);
        }
    }
}



void MarkedExprWithCheck (void (*Func) (ExprDesc*), ExprDesc* Expr)
/* Call an expression function with checks and record start and end of the
** generated code.
*/
{
    CodeMark Start, End;
    GetCodePos (&Start);
    ExprWithCheck (Func, Expr);
    GetCodePos (&End);
    ED_SetCodeRange (Expr, &Start, &End);
}



static Type* promoteint (Type* lhst, Type* rhst)
/* In an expression with two ints, return the type of the result */
{
    /* Rules for integer types:
    **   - If one of the values is a long, the result is long.
    **   - If one of the values is unsigned, the result is also unsigned.
    **   - Otherwise the result is an int.
    */
    if (IsTypeLong (lhst) || IsTypeLong (rhst)) {
        if (IsSignUnsigned (lhst) || IsSignUnsigned (rhst)) {
            return type_ulong;
        } else {
            return type_long;
        }
    } else {
        if (IsSignUnsigned (lhst) || IsSignUnsigned (rhst)) {
            return type_uint;
        } else {
            return type_int;
        }
    }
}



static unsigned typeadjust (ExprDesc* lhs, ExprDesc* rhs, int NoPush)
/* Adjust the two values for a binary operation. lhs is expected on stack or
** to be constant, rhs is expected to be in the primary register or constant.
** The function will put the type of the result into lhs and return the
** code generator flags for the operation.
** If NoPush is given, it is assumed that the operation does not expect the lhs
** to be on stack, and that lhs is in a register instead.
** Beware: The function does only accept int types.
*/
{
    unsigned ltype, rtype;
    unsigned flags;

    /* Get the type strings */
    Type* lhst = lhs->Type;
    Type* rhst = rhs->Type;

    /* Generate type adjustment code if needed */
    ltype = TypeOf (lhst);
    if (ED_IsLocAbs (lhs)) {
        ltype |= CF_CONST;
    }
    if (NoPush) {
        /* Value is in primary register*/
        ltype |= CF_REG;
    }
    rtype = TypeOf (rhst);
    if (ED_IsLocAbs (rhs)) {
        rtype |= CF_CONST;
    }
    flags = g_typeadjust (ltype, rtype);

    /* Set the type of the result */
    lhs->Type = promoteint (lhst, rhst);

    /* Return the code generator flags */
    return flags;
}



static const GenDesc* FindGen (token_t Tok, const GenDesc* Table)
/* Find a token in a generator table */
{
    while (Table->Tok != TOK_INVALID) {
        if (Table->Tok == Tok) {
            return Table;
        }
        ++Table;
    }
    return 0;
}



static int TypeSpecAhead (void)
/* Return true if some sort of type is waiting (helper for cast and sizeof()
** in hie10).
*/
{
    SymEntry* Entry;

    /* There's a type waiting if:
    **
    ** We have an opening paren, and
    **   a.  the next token is a type, or
    **   b.  the next token is a type qualifier, or
    **   c.  the next token is a typedef'd type
    */
    return CurTok.Tok == TOK_LPAREN && (
           TokIsType (&NextTok)                         ||
           TokIsTypeQual (&NextTok)                     ||
           (NextTok.Tok  == TOK_IDENT                   &&
           (Entry = FindSym (NextTok.Ident)) != 0       &&
           SymIsTypeDef (Entry)));
}



void PushAddr (const ExprDesc* Expr)
/* If the expression contains an address that was somehow evaluated,
** push this address on the stack. This is a helper function for all
** sorts of implicit or explicit assignment functions where the lvalue
** must be saved if it's not constant, before evaluating the rhs.
*/
{
    /* Get the address on stack if needed */
    if (ED_IsLocExpr (Expr)) {
        /* Push the address (always a pointer) */
        g_push (CF_PTR, 0);
    }
}



static void WarnConstCompareResult (void)
/* If the result of a comparison is constant, this is suspicious when not in
** preprocessor mode.
*/
{
    if (!Preprocessing && IS_Get (&WarnConstComparison) != 0) {
        Warning ("Result of comparison is constant");
    }
}



/*****************************************************************************/
/*                                   code                                    */
/*****************************************************************************/



static unsigned FunctionParamList (FuncDesc* Func, int IsFastcall)
/* Parse a function parameter list and pass the parameters to the called
** function. Depending on several criteria this may be done by just pushing
** each parameter separately, or creating the parameter frame once and then
** storing into this frame.
** The function returns the size of the parameters pushed.
*/
{
    ExprDesc Expr;

    /* Initialize variables */
    SymEntry* Param       = 0;  /* Keep gcc silent */
    unsigned  ParamSize   = 0;  /* Size of parameters pushed */
    unsigned  ParamCount  = 0;  /* Number of parameters pushed */
    unsigned  FrameSize   = 0;  /* Size of parameter frame */
    unsigned  FrameParams = 0;  /* Number of params in frame */
    int       FrameOffs   = 0;  /* Offset into parameter frame */
    int       Ellipsis    = 0;  /* Function is variadic */

    /* As an optimization, we may allocate the complete parameter frame at
    ** once instead of pushing each parameter as it comes. We may do that,
    ** if...
    **
    **  - optimizations that increase code size are enabled (allocating the
    **    stack frame at once gives usually larger code).
    **  - we have more than one parameter to push (don't count the last param
    **    for __fastcall__ functions).
    **
    ** The FrameSize variable will contain a value > 0 if storing into a frame
    ** (instead of pushing) is enabled.
    **
    */
    if (IS_Get (&CodeSizeFactor) >= 200) {

        /* Calculate the number and size of the parameters */
        FrameParams = Func->ParamCount;
        FrameSize   = Func->ParamSize;
        if (FrameParams > 0 && IsFastcall) {
            /* Last parameter is not pushed */
            FrameSize -= CheckedSizeOf (Func->LastParam->Type);
            --FrameParams;
        }

        /* Do we have more than one parameter in the frame? */
        if (FrameParams > 1) {
            /* Okeydokey, setup the frame */
            FrameOffs = StackPtr;
            g_space (FrameSize);
            StackPtr -= FrameSize;
        } else {
            /* Don't use a preallocated frame */
            FrameSize = 0;
        }
    }

    /* Parse the actual parameter list */
    while (CurTok.Tok != TOK_RPAREN) {

        unsigned Flags;

        /* Count arguments */
        ++ParamCount;

        /* Fetch the pointer to the next argument, check for too many args */
        if (ParamCount <= Func->ParamCount) {
            /* Beware: If there are parameters with identical names, they
            ** cannot go into the same symbol table, which means that in this
            ** case of errorneous input, the number of nodes in the symbol
            ** table and ParamCount are NOT equal. We have to handle this case
            ** below to avoid segmentation violations. Since we know that this
            ** problem can only occur if there is more than one parameter,
            ** we will just use the last one.
            */
            if (ParamCount == 1) {
                /* First argument */
                Param = Func->SymTab->SymHead;
            } else if (Param->NextSym != 0) {
                /* Next argument */
                Param = Param->NextSym;
                CHECK ((Param->Flags & SC_PARAM) != 0);
            }
        } else if (!Ellipsis) {
            /* Too many arguments. Do we have an open param list? */
            if ((Func->Flags & FD_VARIADIC) == 0) {
                /* End of param list reached, no ellipsis */
                Error ("Too many arguments in function call");
            }
            /* Assume an ellipsis even in case of errors to avoid an error
            ** message for each other argument.
            */
            Ellipsis = 1;
        }

        /* Evaluate the parameter expression */
        hie1 (&Expr);

        /* If we don't have an argument spec, accept anything, otherwise
        ** convert the actual argument to the type needed.
        */
        Flags = CF_NONE;
        if (!Ellipsis) {

            /* Convert the argument to the parameter type if needed */
            TypeConversion (&Expr, Param->Type);

            /* If we have a prototype, chars may be pushed as chars */
            Flags |= CF_FORCECHAR;

        } else {

            /* No prototype available. Convert array to "pointer to first
            ** element", and function to "pointer to function".
            */
            Expr.Type = PtrConversion (Expr.Type);

        }

        /* Load the value into the primary if it is not already there */
        LoadExpr (Flags, &Expr);

        /* Use the type of the argument for the push */
        Flags |= TypeOf (Expr.Type);

        /* If this is a fastcall function, don't push the last argument */
        if (ParamCount != Func->ParamCount || !IsFastcall) {
            unsigned ArgSize = sizeofarg (Flags);
            if (FrameSize > 0) {
                /* We have the space already allocated, store in the frame.
                ** Because of invalid type conversions (that have produced an
                ** error before), we can end up here with a non-aligned stack
                ** frame. Since no output will be generated anyway, handle
                ** these cases gracefully instead of doing a CHECK.
                */
                if (FrameSize >= ArgSize) {
                    FrameSize -= ArgSize;
                } else {
                    FrameSize = 0;
                }
                FrameOffs -= ArgSize;
                /* Store */
                g_putlocal (Flags | CF_NOKEEP, FrameOffs, Expr.IVal);
            } else {
                /* Push the argument */
                g_push (Flags, Expr.IVal);
            }

            /* Calculate total parameter size */
            ParamSize += ArgSize;
        }

        /* Check for end of argument list */
        if (CurTok.Tok != TOK_COMMA) {
            break;
        }
        NextToken ();
    }

    /* Check if we had enough parameters */
    if (ParamCount < Func->ParamCount) {
        Error ("Too few arguments in function call");
    }

    /* The function returns the size of all parameters pushed onto the stack.
    ** However, if there are parameters missing (which is an error and was
    ** flagged by the compiler) AND a stack frame was preallocated above,
    ** we would loose track of the stackpointer and generate an internal error
    ** later. So we correct the value by the parameters that should have been
    ** pushed to avoid an internal compiler error. Since an error was
    ** generated before, no code will be output anyway.
    */
    return ParamSize + FrameSize;
}



static void FunctionCall (ExprDesc* Expr)
/* Perform a function call. */
{
    FuncDesc*     Func;           /* Function descriptor */
    int           IsFuncPtr;      /* Flag */
    unsigned      ParamSize;      /* Number of parameter bytes */
    CodeMark      Mark;
    int           PtrOffs = 0;    /* Offset of function pointer on stack */
    int           IsFastcall = 0; /* True if it's a fast-call function */
    int           PtrOnStack = 0; /* True if a pointer copy is on stack */

    /* Skip the left paren */
    NextToken ();

    /* Get a pointer to the function descriptor from the type string */
    Func = GetFuncDesc (Expr->Type);

    /* Handle function pointers transparently */
    IsFuncPtr = IsTypeFuncPtr (Expr->Type);
    if (IsFuncPtr) {
        /* Check whether it's a fastcall function that has parameters */
        IsFastcall = (Func->Flags & FD_VARIADIC) == 0 && Func->ParamCount > 0 &&
            (AutoCDecl ?
             IsQualFastcall (Expr->Type + 1) :
             !IsQualCDecl (Expr->Type + 1));

        /* Things may be difficult, depending on where the function pointer
        ** resides. If the function pointer is an expression of some sort
        ** (not a local or global variable), we have to evaluate this
        ** expression now and save the result for later. Since calls to
        ** function pointers may be nested, we must save it onto the stack.
        ** For fastcall functions we do also need to place a copy of the
        ** pointer on stack, since we cannot use a/x.
        */
        PtrOnStack = IsFastcall || !ED_IsConst (Expr);
        if (PtrOnStack) {

            /* Not a global or local variable, or a fastcall function. Load
            ** the pointer into the primary and mark it as an expression.
            */
            LoadExpr (CF_NONE, Expr);
            ED_MakeRValExpr (Expr);

            /* Remember the code position */
            GetCodePos (&Mark);

            /* Push the pointer onto the stack and remember the offset */
            g_push (CF_PTR, 0);
            PtrOffs = StackPtr;
        }

    } else {
        /* Check function attributes */
        if (Expr->Sym && SymHasAttr (Expr->Sym, atNoReturn)) {
            /* For now, handle as if a return statement was encountered */
            F_ReturnFound (CurrentFunc);
        }

        /* Check for known standard functions and inline them */
        if (Expr->Name != 0) {
            int StdFunc = FindStdFunc ((const char*) Expr->Name);
            if (StdFunc >= 0) {
                /* Inline this function */
                HandleStdFunc (StdFunc, Func, Expr);
                return;
            }
        }

        /* If we didn't inline the function, get fastcall info */
        IsFastcall = (Func->Flags & FD_VARIADIC) == 0 &&
            (AutoCDecl ?
             IsQualFastcall (Expr->Type) :
             !IsQualCDecl (Expr->Type));
    }

    /* Parse the parameter list */
    ParamSize = FunctionParamList (Func, IsFastcall);

    /* We need the closing paren here */
    ConsumeRParen ();

    /* Special handling for function pointers */
    if (IsFuncPtr) {

        /* If the function is not a fastcall function, load the pointer to
        ** the function into the primary.
        */
        if (!IsFastcall) {

            /* Not a fastcall function - we may use the primary */
            if (PtrOnStack) {
                /* If we have no parameters, the pointer is still in the
                ** primary. Remove the code to push it and correct the
                ** stack pointer.
                */
                if (ParamSize == 0) {
                    RemoveCode (&Mark);
                    PtrOnStack = 0;
                } else {
                    /* Load from the saved copy */
                    g_getlocal (CF_PTR, PtrOffs);
                }
            } else {
                /* Load from original location */
                LoadExpr (CF_NONE, Expr);
            }

            /* Call the function */
            g_callind (TypeOf (Expr->Type+1), ParamSize, PtrOffs);

        } else {

            /* Fastcall function. We cannot use the primary for the function
            ** pointer and must therefore use an offset to the stack location.
            ** Since fastcall functions may never be variadic, we can use the
            ** index register for this purpose.
            */
            g_callind (CF_LOCAL, ParamSize, PtrOffs);
        }

        /* If we have a pointer on stack, remove it */
        if (PtrOnStack) {
            g_drop (SIZEOF_PTR);
            pop (CF_PTR);
        }

        /* Skip T_PTR */
        ++Expr->Type;

    } else {

        /* Normal function */
        g_call (TypeOf (Expr->Type), (const char*) Expr->Name, ParamSize);

    }

    /* The function result is an rvalue in the primary register */
    ED_MakeRValExpr (Expr);
    Expr->Type = GetFuncReturn (Expr->Type);
}



static void Primary (ExprDesc* E)
/* This is the lowest level of the expression parser. */
{
    SymEntry* Sym;

    /* Initialize fields in the expression stucture */
    ED_Init (E);

    /* Character and integer constants. */
    if (CurTok.Tok == TOK_ICONST || CurTok.Tok == TOK_CCONST) {
        E->IVal  = CurTok.IVal;
        E->Flags = E_LOC_ABS | E_RTYPE_RVAL;
        E->Type  = CurTok.Type;
        NextToken ();
        return;
    }

    /* Floating point constant */
    if (CurTok.Tok == TOK_FCONST) {
        E->FVal  = CurTok.FVal;
        E->Flags = E_LOC_ABS | E_RTYPE_RVAL;
        E->Type  = CurTok.Type;
        NextToken ();
        return;
    }

    /* Process parenthesized subexpression by calling the whole parser
    ** recursively.
    */
    if (CurTok.Tok == TOK_LPAREN) {
        NextToken ();
        hie0 (E);
        ConsumeRParen ();
        return;
    }

    /* If we run into an identifier in preprocessing mode, we assume that this
    ** is an undefined macro and replace it by a constant value of zero.
    */
    if (Preprocessing && CurTok.Tok == TOK_IDENT) {
        NextToken ();
        ED_MakeConstAbsInt (E, 0);
        return;
    }

    /* All others may only be used if the expression evaluation is not called
    ** recursively by the preprocessor.
    */
    if (Preprocessing) {
        /* Illegal expression in PP mode */
        Error ("Preprocessor expression expected");
        ED_MakeConstAbsInt (E, 1);
        return;
    }

    switch (CurTok.Tok) {

        case TOK_IDENT:
            /* Identifier. Get a pointer to the symbol table entry */
            Sym = E->Sym = FindSym (CurTok.Ident);

            /* Is the symbol known? */
            if (Sym) {

                /* We found the symbol - skip the name token */
                NextToken ();

                /* Check for illegal symbol types */
                CHECK ((Sym->Flags & SC_LABEL) != SC_LABEL);
                if (Sym->Flags & SC_TYPE) {
                    /* Cannot use type symbols */
                    Error ("Variable identifier expected");
                    /* Assume an int type to make E valid */
                    E->Flags = E_LOC_STACK | E_RTYPE_LVAL;
                    E->Type  = type_int;
                    return;
                }

                /* Mark the symbol as referenced */
                Sym->Flags |= SC_REF;

                /* The expression type is the symbol type */
                E->Type = Sym->Type;

                /* Check for legal symbol types */
                if ((Sym->Flags & SC_CONST) == SC_CONST) {
                    /* Enum or some other numeric constant */
                    E->Flags = E_LOC_ABS | E_RTYPE_RVAL;
                    E->IVal = Sym->V.ConstVal;
                } else if ((Sym->Flags & SC_FUNC) == SC_FUNC) {
                    /* Function */
                    E->Flags = E_LOC_GLOBAL | E_RTYPE_LVAL;
                    E->Name = (unsigned long) Sym->Name;
                } else if ((Sym->Flags & SC_AUTO) == SC_AUTO) {
                    /* Local variable. If this is a parameter for a variadic
                    ** function, we have to add some address calculations, and the
                    ** address is not const.
                    */
                    if ((Sym->Flags & SC_PARAM) == SC_PARAM && F_IsVariadic (CurrentFunc)) {
                        /* Variadic parameter */
                        g_leavariadic (Sym->V.Offs - F_GetParamSize (CurrentFunc));
                        E->Flags = E_LOC_EXPR | E_RTYPE_LVAL;
                    } else {
                        /* Normal parameter */
                        E->Flags = E_LOC_STACK | E_RTYPE_LVAL;
                        E->IVal  = Sym->V.Offs;
                    }
                } else if ((Sym->Flags & SC_REGISTER) == SC_REGISTER) {
                    /* Register variable, zero page based */
                    E->Flags = E_LOC_REGISTER | E_RTYPE_LVAL;
                    E->Name  = Sym->V.R.RegOffs;
                } else if ((Sym->Flags & SC_STATIC) == SC_STATIC) {
                    /* Static variable */
                    if (Sym->Flags & (SC_EXTERN | SC_STORAGE)) {
                        E->Flags = E_LOC_GLOBAL | E_RTYPE_LVAL;
                        E->Name = (unsigned long) Sym->Name;
                    } else {
                        E->Flags = E_LOC_STATIC | E_RTYPE_LVAL;
                        E->Name = Sym->V.Label;
                    }
                } else {
                    /* Local static variable */
                    E->Flags = E_LOC_STATIC | E_RTYPE_LVAL;
                    E->Name  = Sym->V.Offs;
                }

                /* We've made all variables lvalues above. However, this is
                ** not always correct: An array is actually the address of its
                ** first element, which is a rvalue, and a function is a
                ** rvalue, too, because we cannot store anything in a function.
                ** So fix the flags depending on the type.
                */
                if (IsTypeArray (E->Type) || IsTypeFunc (E->Type)) {
                    ED_MakeRVal (E);
                }

            } else {

                /* We did not find the symbol. Remember the name, then skip it */
                ident Ident;
                strcpy (Ident, CurTok.Ident);
                NextToken ();

                /* IDENT is either an auto-declared function or an undefined variable. */
                if (CurTok.Tok == TOK_LPAREN) {
                    /* C99 doesn't allow calls to undefined functions, so
                    ** generate an error and otherwise a warning. Declare a
                    ** function returning int. For that purpose, prepare a
                    ** function signature for a function having an empty param
                    ** list and returning int.
                    */
                    if (IS_Get (&Standard) >= STD_C99) {
                        Error ("Call to undefined function `%s'", Ident);
                    } else {
                        Warning ("Call to undefined function `%s'", Ident);
                    }
                    Sym = AddGlobalSym (Ident, GetImplicitFuncType(), SC_EXTERN | SC_REF | SC_FUNC);
                    E->Type  = Sym->Type;
                    E->Flags = E_LOC_GLOBAL | E_RTYPE_RVAL;
                    E->Name  = (unsigned long) Sym->Name;
                } else {
                    /* Undeclared Variable */
                    Sym = AddLocalSym (Ident, type_int, SC_AUTO | SC_REF, 0);
                    E->Flags = E_LOC_STACK | E_RTYPE_LVAL;
                    E->Type = type_int;
                    Error ("Undefined symbol: `%s'", Ident);
                }

            }
            break;

        case TOK_SCONST:
        case TOK_WCSCONST:
            /* String literal */
            E->LVal  = UseLiteral (CurTok.SVal);
            E->Type  = GetCharArrayType (GetLiteralSize (CurTok.SVal));
            E->Flags = E_LOC_LITERAL | E_RTYPE_RVAL;
            E->IVal  = 0;
            E->Name  = GetLiteralLabel (CurTok.SVal);
            NextToken ();
            break;

        case TOK_ASM:
            /* ASM statement */
            AsmStatement ();
            E->Flags = E_LOC_EXPR | E_RTYPE_RVAL;
            E->Type  = type_void;
            break;

        case TOK_A:
            /* Register pseudo variable */
            E->Type  = type_uchar;
            E->Flags = E_LOC_PRIMARY | E_RTYPE_LVAL;
            NextToken ();
            break;

        case TOK_AX:
            /* Register pseudo variable */
            E->Type  = type_uint;
            E->Flags = E_LOC_PRIMARY | E_RTYPE_LVAL;
            NextToken ();
            break;

        case TOK_EAX:
            /* Register pseudo variable */
            E->Type  = type_ulong;
            E->Flags = E_LOC_PRIMARY | E_RTYPE_LVAL;
            NextToken ();
            break;

        default:
            /* Illegal primary. Be sure to skip the token to avoid endless
            ** error loops.
            */
            Error ("Expression expected");
            NextToken ();
            ED_MakeConstAbsInt (E, 1);
            break;
    }
}



static void ArrayRef (ExprDesc* Expr)
/* Handle an array reference. This function needs a rewrite. */
{
    int         ConstBaseAddr;
    ExprDesc    Subscript;
    CodeMark    Mark1;
    CodeMark    Mark2;
    TypeCode    Qualifiers;
    Type*       ElementType;
    Type*       tptr1;


    /* Skip the bracket */
    NextToken ();

    /* Get the type of left side */
    tptr1 = Expr->Type;

    /* We can apply a special treatment for arrays that have a const base
    ** address. This is true for most arrays and will produce a lot better
    ** code. Check if this is a const base address.
    */
    ConstBaseAddr = ED_IsRVal (Expr) &&
                    (ED_IsLocConst (Expr) || ED_IsLocStack (Expr));

    /* If we have a constant base, we delay the address fetch */
    GetCodePos (&Mark1);
    if (!ConstBaseAddr) {
        /* Get a pointer to the array into the primary */
        LoadExpr (CF_NONE, Expr);

        /* Get the array pointer on stack. Do not push more than 16
        ** bit, even if this value is greater, since we cannot handle
        ** other than 16bit stuff when doing indexing.
        */
        GetCodePos (&Mark2);
        g_push (CF_PTR, 0);
    }

    /* TOS now contains ptr to array elements. Get the subscript. */
    MarkedExprWithCheck (hie0, &Subscript);

    /* Check the types of array and subscript. We can either have a
    ** pointer/array to the left, in which case the subscript must be of an
    ** integer type, or we have an integer to the left, in which case the
    ** subscript must be a pointer/array.
    ** Since we do the necessary checking here, we can rely later on the
    ** correct types.
    */
    Qualifiers = T_QUAL_NONE;
    if (IsClassPtr (Expr->Type)) {
        if (!IsClassInt (Subscript.Type))  {
            Error ("Array subscript is not an integer");
            /* To avoid any compiler errors, make the expression a valid int */
            ED_MakeConstAbsInt (&Subscript, 0);
        }
        if (IsTypeArray (Expr->Type)) {
            Qualifiers = GetQualifier (Expr->Type);
        }
        ElementType = Indirect (Expr->Type);
    } else if (IsClassInt (Expr->Type)) {
        if (!IsClassPtr (Subscript.Type)) {
            Error ("Subscripted value is neither array nor pointer");
            /* To avoid compiler errors, make the subscript a char[] at
            ** address 0.
            */
            ED_MakeConstAbs (&Subscript, 0, GetCharArrayType (1));
        } else if (IsTypeArray (Subscript.Type)) {
            Qualifiers = GetQualifier (Subscript.Type);
        }
        ElementType = Indirect (Subscript.Type);
    } else {
        Error ("Cannot subscript");
        /* To avoid compiler errors, fake both the array and the subscript, so
        ** we can just proceed.
        */
        ED_MakeConstAbs (Expr, 0, GetCharArrayType (1));
        ED_MakeConstAbsInt (&Subscript, 0);
        ElementType = Indirect (Expr->Type);
    }

    /* The element type has the combined qualifiers from itself and the array,
    ** it is a member of (if any).
    */
    if (GetQualifier (ElementType) != (GetQualifier (ElementType) | Qualifiers)) {
        ElementType = TypeDup (ElementType);
        ElementType->C |= Qualifiers;
    }

    /* If the subscript is a bit-field, load it and make it an rvalue */
    if (ED_IsBitField (&Subscript)) {
        LoadExpr (CF_NONE, &Subscript);
        ED_MakeRValExpr (&Subscript);
    }

    /* Check if the subscript is constant absolute value */
    if (ED_IsConstAbs (&Subscript) && ED_CodeRangeIsEmpty (&Subscript)) {

        /* The array subscript is a numeric constant. If we had pushed the
        ** array base address onto the stack before, we can remove this value,
        ** since we can generate expression+offset.
        */
        if (!ConstBaseAddr) {
            RemoveCode (&Mark2);
        } else {
            /* Get an array pointer into the primary */
            LoadExpr (CF_NONE, Expr);
        }

        if (IsClassPtr (Expr->Type)) {

            /* Lhs is pointer/array. Scale the subscript value according to
            ** the element size.
            */
            Subscript.IVal *= CheckedSizeOf (ElementType);

            /* Remove the address load code */
            RemoveCode (&Mark1);

            /* In case of an array, we can adjust the offset of the expression
            ** already in Expr. If the base address was a constant, we can even
            ** remove the code that loaded the address into the primary.
            */
            if (IsTypeArray (Expr->Type)) {

                /* Adjust the offset */
                Expr->IVal += Subscript.IVal;

            } else {

                /* It's a pointer, so we do have to load it into the primary
                ** first (if it's not already there).
                */
                if (ConstBaseAddr || ED_IsLVal (Expr)) {
                    LoadExpr (CF_NONE, Expr);
                    ED_MakeRValExpr (Expr);
                }

                /* Use the offset */
                Expr->IVal = Subscript.IVal;
            }

        } else {

            /* Scale the rhs value according to the element type */
            g_scale (TypeOf (tptr1), CheckedSizeOf (ElementType));

            /* Add the subscript. Since arrays are indexed by integers,
            ** we will ignore the true type of the subscript here and
            ** use always an int. #### Use offset but beware of LoadExpr!
            */
            g_inc (CF_INT | CF_CONST, Subscript.IVal);

        }

    } else {

        /* Array subscript is not constant. Load it into the primary */
        GetCodePos (&Mark2);
        LoadExpr (CF_NONE, &Subscript);

        /* Do scaling */
        if (IsClassPtr (Expr->Type)) {

            /* Indexing is based on unsigneds, so we will just use the integer
            ** portion of the index (which is in (e)ax, so there's no further
            ** action required).
            */
            g_scale (CF_INT, CheckedSizeOf (ElementType));

        } else {

            /* Get the int value on top. If we come here, we're sure, both
            ** values are 16 bit (the first one was truncated if necessary
            ** and the second one is a pointer). Note: If ConstBaseAddr is
            ** true, we don't have a value on stack, so to "swap" both, just
            ** push the subscript.
            */
            if (ConstBaseAddr) {
                g_push (CF_INT, 0);
                LoadExpr (CF_NONE, Expr);
                ConstBaseAddr = 0;
            } else {
                g_swap (CF_INT);
            }

            /* Scale it */
            g_scale (TypeOf (tptr1), CheckedSizeOf (ElementType));

        }

        /* The offset is now in the primary register. It we didn't have a
        ** constant base address for the lhs, the lhs address is already
        ** on stack, and we must add the offset. If the base address was
        ** constant, we call special functions to add the address to the
        ** offset value.
        */
        if (!ConstBaseAddr) {

            /* The array base address is on stack and the subscript is in the
            ** primary. Add both.
            */
            g_add (CF_INT, 0);

        } else {

            /* The subscript is in the primary, and the array base address is
            ** in Expr. If the subscript has itself a constant address, it is
            ** often a better idea to reverse again the order of the
            ** evaluation. This will generate better code if the subscript is
            ** a byte sized variable. But beware: This is only possible if the
            ** subscript was not scaled, that is, if this was a byte array
            ** or pointer.
            */
            if ((ED_IsLocConst (&Subscript) || ED_IsLocStack (&Subscript)) &&
                CheckedSizeOf (ElementType) == SIZEOF_CHAR) {

                unsigned Flags;

                /* Reverse the order of evaluation */
                if (CheckedSizeOf (Subscript.Type) == SIZEOF_CHAR) {
                    Flags = CF_CHAR;
                } else {
                    Flags = CF_INT;
                }
                RemoveCode (&Mark2);

                /* Get a pointer to the array into the primary. */
                LoadExpr (CF_NONE, Expr);

                /* Add the variable */
                if (ED_IsLocStack (&Subscript)) {
                    g_addlocal (Flags, Subscript.IVal);
                } else {
                    Flags |= GlobalModeFlags (&Subscript);
                    g_addstatic (Flags, Subscript.Name, Subscript.IVal);
                }
            } else {

                if (ED_IsLocAbs (Expr)) {
                    /* Constant numeric address. Just add it */
                    g_inc (CF_INT, Expr->IVal);
                } else if (ED_IsLocStack (Expr)) {
                    /* Base address is a local variable address */
                    if (IsTypeArray (Expr->Type)) {
                        g_addaddr_local (CF_INT, Expr->IVal);
                    } else {
                        g_addlocal (CF_PTR, Expr->IVal);
                    }
                } else {
                    /* Base address is a static variable address */
                    unsigned Flags = CF_INT | GlobalModeFlags (Expr);
                    if (ED_IsRVal (Expr)) {
                        /* Add the address of the location */
                        g_addaddr_static (Flags, Expr->Name, Expr->IVal);
                    } else {
                        /* Add the contents of the location */
                        g_addstatic (Flags, Expr->Name, Expr->IVal);
                    }
                }
            }


        }

        /* The result is an expression in the primary */
        ED_MakeRValExpr (Expr);

    }

    /* Result is of element type */
    Expr->Type = ElementType;

    /* An array element is actually a variable. So the rules for variables
    ** with respect to the reference type apply: If it's an array, it is
    ** a rvalue, otherwise it's an lvalue. (A function would also be a rvalue,
    ** but an array cannot contain functions).
    */
    if (IsTypeArray (Expr->Type)) {
        ED_MakeRVal (Expr);
    } else {
        ED_MakeLVal (Expr);
    }

    /* Consume the closing bracket */
    ConsumeRBrack ();
}



static void StructRef (ExprDesc* Expr)
/* Process struct field after . or ->. */
{
    ident Ident;
    SymEntry* Field;
    Type* FinalType;
    TypeCode Q;

    /* Skip the token and check for an identifier */
    NextToken ();
    if (CurTok.Tok != TOK_IDENT) {
        Error ("Identifier expected");
        /* Make the expression an integer at address zero */
        ED_MakeConstAbs (Expr, 0, type_int);
        return;
    }

    /* Get the symbol table entry and check for a struct field */
    strcpy (Ident, CurTok.Ident);
    NextToken ();
    Field = FindStructField (Expr->Type, Ident);
    if (Field == 0) {
        Error ("Struct/union has no field named `%s'", Ident);
        /* Make the expression an integer at address zero */
        ED_MakeConstAbs (Expr, 0, type_int);
        return;
    }

    /* If we have a struct pointer that is an lvalue and not already in the
    ** primary, load it now.
    */
    if (ED_IsLVal (Expr) && IsTypePtr (Expr->Type)) {

        /* Load into the primary */
        LoadExpr (CF_NONE, Expr);

        /* Make it an lvalue expression */
        ED_MakeLValExpr (Expr);
    }

    /* The type is the type of the field plus any qualifiers from the struct */
    if (IsClassStruct (Expr->Type)) {
        Q = GetQualifier (Expr->Type);
    } else {
        Q = GetQualifier (Indirect (Expr->Type));
    }
    if (GetQualifier (Field->Type) == (GetQualifier (Field->Type) | Q)) {
        FinalType = Field->Type;
    } else {
        FinalType = TypeDup (Field->Type);
        FinalType->C |= Q;
    }

    /* A struct is usually an lvalue. If not, it is a struct in the primary
    ** register.
    */
    if (ED_IsRVal (Expr) && ED_IsLocExpr (Expr) && !IsTypePtr (Expr->Type)) {

        unsigned Flags = 0;
        unsigned BitOffs;

        /* Get the size of the type */
        unsigned Size = SizeOf (Expr->Type);

        /* Safety check */
        CHECK (Field->V.Offs + Size <= SIZEOF_LONG);

        /* The type of the operation depends on the type of the struct */
        switch (Size) {
            case 1:     Flags = CF_CHAR | CF_UNSIGNED | CF_CONST;       break;
            case 2:     Flags = CF_INT  | CF_UNSIGNED | CF_CONST;       break;
            case 3:     /* FALLTHROUGH */
            case 4:     Flags = CF_LONG | CF_UNSIGNED | CF_CONST;       break;
            default:    Internal ("Invalid struct size: %u", Size);     break;
        }

        /* Generate a shift to get the field in the proper position in the
        ** primary. For bit fields, mask the value.
        */
        BitOffs = Field->V.Offs * CHAR_BITS;
        if (SymIsBitField (Field)) {
            BitOffs += Field->V.B.BitOffs;
            g_asr (Flags, BitOffs);
            /* Mask the value. This is unnecessary if the shift executed above
            ** moved only zeroes into the value.
            */
            if (BitOffs + Field->V.B.BitWidth != Size * CHAR_BITS) {
                g_and (CF_INT | CF_UNSIGNED | CF_CONST,
                       (0x0001U << Field->V.B.BitWidth) - 1U);
            }
        } else {
            g_asr (Flags, BitOffs);
        }

        /* Use the new type */
        Expr->Type = FinalType;

    } else {

        /* Set the struct field offset */
        Expr->IVal += Field->V.Offs;

        /* Use the new type */
        Expr->Type = FinalType;

        /* An struct member is actually a variable. So the rules for variables
        ** with respect to the reference type apply: If it's an array, it is
        ** a rvalue, otherwise it's an lvalue. (A function would also be a rvalue,
        ** but a struct field cannot be a function).
        */
        if (IsTypeArray (Expr->Type)) {
            ED_MakeRVal (Expr);
        } else {
            ED_MakeLVal (Expr);
        }

        /* Make the expression a bit field if necessary */
        if (SymIsBitField (Field)) {
            ED_MakeBitField (Expr, Field->V.B.BitOffs, Field->V.B.BitWidth);
        }
    }

}



static void hie11 (ExprDesc *Expr)
/* Handle compound types (structs and arrays) */
{
    /* Name value used in invalid function calls */
    static const char IllegalFunc[] = "illegal_function_call";

    /* Evaluate the lhs */
    Primary (Expr);

    /* Check for a rhs */
    while (CurTok.Tok == TOK_LBRACK || CurTok.Tok == TOK_LPAREN ||
           CurTok.Tok == TOK_DOT    || CurTok.Tok == TOK_PTR_REF) {

        switch (CurTok.Tok) {

            case TOK_LBRACK:
                /* Array reference */
                ArrayRef (Expr);
                break;

            case TOK_LPAREN:
                /* Function call. */
                if (!IsTypeFunc (Expr->Type) && !IsTypeFuncPtr (Expr->Type)) {
                    /* Not a function */
                    Error ("Illegal function call");
                    /* Force the type to be a implicitly defined function, one
                    ** returning an int and taking any number of arguments.
                    ** Since we don't have a name, invent one.
                    */
                    ED_MakeConstAbs (Expr, 0, GetImplicitFuncType ());
                    Expr->Name = (long) IllegalFunc;
                }
                /* Call the function */
                FunctionCall (Expr);
                break;

            case TOK_DOT:
                if (!IsClassStruct (Expr->Type)) {
                    Error ("Struct expected");
                }
                StructRef (Expr);
                break;

            case TOK_PTR_REF:
                /* If we have an array, convert it to pointer to first element */
                if (IsTypeArray (Expr->Type)) {
                    Expr->Type = ArrayToPtr (Expr->Type);
                }
                if (!IsClassPtr (Expr->Type) || !IsClassStruct (Indirect (Expr->Type))) {
                    Error ("Struct pointer expected");
                }
                StructRef (Expr);
                break;

            default:
                Internal ("Invalid token in hie11: %d", CurTok.Tok);

        }
    }
}



void Store (ExprDesc* Expr, const Type* StoreType)
/* Store the primary register into the location denoted by Expr. If StoreType
** is given, use this type when storing instead of Expr->Type. If StoreType
** is NULL, use Expr->Type instead.
*/
{
    unsigned Flags;

    /* If StoreType was not given, use Expr->Type instead */
    if (StoreType == 0) {
        StoreType = Expr->Type;
    }

    /* Prepare the code generator flags */
    Flags = TypeOf (StoreType) | GlobalModeFlags (Expr);

    /* Do the store depending on the location */
    switch (ED_GetLoc (Expr)) {

        case E_LOC_ABS:
            /* Absolute: numeric address or const */
            g_putstatic (Flags, Expr->IVal, 0);
            break;

        case E_LOC_GLOBAL:
            /* Global variable */
            g_putstatic (Flags, Expr->Name, Expr->IVal);
            break;

        case E_LOC_STATIC:
        case E_LOC_LITERAL:
            /* Static variable or literal in the literal pool */
            g_putstatic (Flags, Expr->Name, Expr->IVal);
            break;

        case E_LOC_REGISTER:
            /* Register variable */
            g_putstatic (Flags, Expr->Name, Expr->IVal);
            break;

        case E_LOC_STACK:
            /* Value on the stack */
            g_putlocal (Flags, Expr->IVal, 0);
            break;

        case E_LOC_PRIMARY:
            /* The primary register (value is already there) */
            break;

        case E_LOC_EXPR:
            /* An expression in the primary register */
            g_putind (Flags, Expr->IVal);
            break;

        default:
            Internal ("Invalid location in Store(): 0x%04X", ED_GetLoc (Expr));
    }

    /* Assume that each one of the stores will invalidate CC */
    ED_MarkAsUntested (Expr);
}



static void PreInc (ExprDesc* Expr)
/* Handle the preincrement operators */
{
    unsigned Flags;
    unsigned long Val;

    /* Skip the operator token */
    NextToken ();

    /* Evaluate the expression and check that it is an lvalue */
    hie10 (Expr);
    if (!ED_IsLVal (Expr)) {
        Error ("Invalid lvalue");
        return;
    }

    /* We cannot modify const values */
    if (IsQualConst (Expr->Type)) {
        Error ("Increment of read-only variable");
    }

    /* Get the data type */
    Flags = TypeOf (Expr->Type) | GlobalModeFlags (Expr) | CF_FORCECHAR | CF_CONST;

    /* Get the increment value in bytes */
    Val = IsTypePtr (Expr->Type)? CheckedPSizeOf (Expr->Type) : 1;

    /* Check the location of the data */
    switch (ED_GetLoc (Expr)) {

        case E_LOC_ABS:
            /* Absolute: numeric address or const */
            g_addeqstatic (Flags, Expr->IVal, 0, Val);
            break;

        case E_LOC_GLOBAL:
            /* Global variable */
            g_addeqstatic (Flags, Expr->Name, Expr->IVal, Val);
            break;

        case E_LOC_STATIC:
        case E_LOC_LITERAL:
            /* Static variable or literal in the literal pool */
            g_addeqstatic (Flags, Expr->Name, Expr->IVal, Val);
            break;

        case E_LOC_REGISTER:
            /* Register variable */
            g_addeqstatic (Flags, Expr->Name, Expr->IVal, Val);
            break;

        case E_LOC_STACK:
            /* Value on the stack */
            g_addeqlocal (Flags, Expr->IVal, Val);
            break;

        case E_LOC_PRIMARY:
            /* The primary register */
            g_inc (Flags, Val);
            break;

        case E_LOC_EXPR:
            /* An expression in the primary register */
            g_addeqind (Flags, Expr->IVal, Val);
            break;

        default:
            Internal ("Invalid location in PreInc(): 0x%04X", ED_GetLoc (Expr));
    }

    /* Result is an expression, no reference */
    ED_MakeRValExpr (Expr);
}



static void PreDec (ExprDesc* Expr)
/* Handle the predecrement operators */
{
    unsigned Flags;
    unsigned long Val;

    /* Skip the operator token */
    NextToken ();

    /* Evaluate the expression and check that it is an lvalue */
    hie10 (Expr);
    if (!ED_IsLVal (Expr)) {
        Error ("Invalid lvalue");
        return;
    }

    /* We cannot modify const values */
    if (IsQualConst (Expr->Type)) {
        Error ("Decrement of read-only variable");
    }

    /* Get the data type */
    Flags = TypeOf (Expr->Type) | GlobalModeFlags (Expr) | CF_FORCECHAR | CF_CONST;

    /* Get the increment value in bytes */
    Val = IsTypePtr (Expr->Type)? CheckedPSizeOf (Expr->Type) : 1;

    /* Check the location of the data */
    switch (ED_GetLoc (Expr)) {

        case E_LOC_ABS:
            /* Absolute: numeric address or const */
            g_subeqstatic (Flags, Expr->IVal, 0, Val);
            break;

        case E_LOC_GLOBAL:
            /* Global variable */
            g_subeqstatic (Flags, Expr->Name, Expr->IVal, Val);
            break;

        case E_LOC_STATIC:
        case E_LOC_LITERAL:
            /* Static variable or literal in the literal pool */
            g_subeqstatic (Flags, Expr->Name, Expr->IVal, Val);
            break;

        case E_LOC_REGISTER:
            /* Register variable */
            g_subeqstatic (Flags, Expr->Name, Expr->IVal, Val);
            break;

        case E_LOC_STACK:
            /* Value on the stack */
            g_subeqlocal (Flags, Expr->IVal, Val);
            break;

        case E_LOC_PRIMARY:
            /* The primary register */
            g_inc (Flags, Val);
            break;

        case E_LOC_EXPR:
            /* An expression in the primary register */
            g_subeqind (Flags, Expr->IVal, Val);
            break;

        default:
            Internal ("Invalid location in PreDec(): 0x%04X", ED_GetLoc (Expr));
    }

    /* Result is an expression, no reference */
    ED_MakeRValExpr (Expr);
}



static void PostInc (ExprDesc* Expr)
/* Handle the postincrement operator */
{
    unsigned Flags;

    NextToken ();

    /* The expression to increment must be an lvalue */
    if (!ED_IsLVal (Expr)) {
        Error ("Invalid lvalue");
        return;
    }

    /* We cannot modify const values */
    if (IsQualConst (Expr->Type)) {
        Error ("Increment of read-only variable");
    }

    /* Get the data type */
    Flags = TypeOf (Expr->Type);

    /* Push the address if needed */
    PushAddr (Expr);

    /* Fetch the value and save it (since it's the result of the expression) */
    LoadExpr (CF_NONE, Expr);
    g_save (Flags | CF_FORCECHAR);

    /* If we have a pointer expression, increment by the size of the type */
    if (IsTypePtr (Expr->Type)) {
        g_inc (Flags | CF_CONST | CF_FORCECHAR, CheckedSizeOf (Expr->Type + 1));
    } else {
        g_inc (Flags | CF_CONST | CF_FORCECHAR, 1);
    }

    /* Store the result back */
    Store (Expr, 0);

    /* Restore the original value in the primary register */
    g_restore (Flags | CF_FORCECHAR);

    /* The result is always an expression, no reference */
    ED_MakeRValExpr (Expr);
}



static void PostDec (ExprDesc* Expr)
/* Handle the postdecrement operator */
{
    unsigned Flags;

    NextToken ();

    /* The expression to increment must be an lvalue */
    if (!ED_IsLVal (Expr)) {
        Error ("Invalid lvalue");
        return;
    }

    /* We cannot modify const values */
    if (IsQualConst (Expr->Type)) {
        Error ("Decrement of read-only variable");
    }

    /* Get the data type */
    Flags = TypeOf (Expr->Type);

    /* Push the address if needed */
    PushAddr (Expr);

    /* Fetch the value and save it (since it's the result of the expression) */
    LoadExpr (CF_NONE, Expr);
    g_save (Flags | CF_FORCECHAR);

    /* If we have a pointer expression, increment by the size of the type */
    if (IsTypePtr (Expr->Type)) {
        g_dec (Flags | CF_CONST | CF_FORCECHAR, CheckedSizeOf (Expr->Type + 1));
    } else {
        g_dec (Flags | CF_CONST | CF_FORCECHAR, 1);
    }

    /* Store the result back */
    Store (Expr, 0);

    /* Restore the original value in the primary register */
    g_restore (Flags | CF_FORCECHAR);

    /* The result is always an expression, no reference */
    ED_MakeRValExpr (Expr);
}



static void UnaryOp (ExprDesc* Expr)
/* Handle unary -/+ and ~ */
{
    unsigned Flags;

    /* Remember the operator token and skip it */
    token_t Tok = CurTok.Tok;
    NextToken ();

    /* Get the expression */
    hie10 (Expr);

    /* We can only handle integer types */
    if (!IsClassInt (Expr->Type)) {
        Error ("Argument must have integer type");
        ED_MakeConstAbsInt (Expr, 1);
    }

    /* Check for a constant expression */
    if (ED_IsConstAbs (Expr)) {
        /* Value is constant */
        switch (Tok) {
            case TOK_MINUS: Expr->IVal = -Expr->IVal;   break;
            case TOK_PLUS:                              break;
            case TOK_COMP:  Expr->IVal = ~Expr->IVal;   break;
            default:        Internal ("Unexpected token: %d", Tok);
        }
    } else {
        /* Value is not constant */
        LoadExpr (CF_NONE, Expr);

        /* Get the type of the expression */
        Flags = TypeOf (Expr->Type);

        /* Handle the operation */
        switch (Tok) {
            case TOK_MINUS: g_neg (Flags);  break;
            case TOK_PLUS:                  break;
            case TOK_COMP:  g_com (Flags);  break;
            default:        Internal ("Unexpected token: %d", Tok);
        }

        /* The result is a rvalue in the primary */
        ED_MakeRValExpr (Expr);
    }
}



void hie10 (ExprDesc* Expr)
/* Handle ++, --, !, unary - etc. */
{
    unsigned long Size;

    switch (CurTok.Tok) {

        case TOK_INC:
            PreInc (Expr);
            break;

        case TOK_DEC:
            PreDec (Expr);
            break;

        case TOK_PLUS:
        case TOK_MINUS:
        case TOK_COMP:
            UnaryOp (Expr);
            break;

        case TOK_BOOL_NOT:
            NextToken ();
            if (evalexpr (CF_NONE, hie10, Expr) == 0) {
                /* Constant expression */
                Expr->IVal = !Expr->IVal;
            } else {
                g_bneg (TypeOf (Expr->Type));
                ED_MakeRValExpr (Expr);
                ED_TestDone (Expr);             /* bneg will set cc */
            }
            break;

        case TOK_STAR:
            NextToken ();
            ExprWithCheck (hie10, Expr);
            if (ED_IsLVal (Expr) || !(ED_IsLocConst (Expr) || ED_IsLocStack (Expr))) {
                /* Not a const, load it into the primary and make it a
                ** calculated value.
                */
                LoadExpr (CF_NONE, Expr);
                ED_MakeRValExpr (Expr);
            }
            /* If the expression is already a pointer to function, the
            ** additional dereferencing operator must be ignored. A function
            ** itself is represented as "pointer to function", so any number
            ** of dereference operators is legal, since the result will
            ** always be converted to "pointer to function".
            */
            if (IsTypeFuncPtr (Expr->Type) || IsTypeFunc (Expr->Type)) {
                /* Expression not storable */
                ED_MakeRVal (Expr);
            } else {
                if (IsClassPtr (Expr->Type)) {
                    Expr->Type = Indirect (Expr->Type);
                } else {
                    Error ("Illegal indirection");
                }
                /* If the expression points to an array, then don't convert the
                ** address -- it already is the location of the first element.
                */
                if (!IsTypeArray (Expr->Type)) {
                    /* The * operator yields an lvalue */
                    ED_MakeLVal (Expr);
                }
            }
            break;

        case TOK_AND:
            NextToken ();
            ExprWithCheck (hie10, Expr);
            /* The & operator may be applied to any lvalue, and it may be
            ** applied to functions, even if they're no lvalues.
            */
            if (ED_IsRVal (Expr) && !IsTypeFunc (Expr->Type) && !IsTypeArray (Expr->Type)) {
                Error ("Illegal address");
            } else {
                if (ED_IsBitField (Expr)) {
                    Error ("Cannot take address of bit-field");
                    /* Do it anyway, just to avoid further warnings */
                    Expr->Flags &= ~E_BITFIELD;
                }
                Expr->Type = PointerTo (Expr->Type);
                /* The & operator yields an rvalue */
                ED_MakeRVal (Expr);
            }
            break;

        case TOK_SIZEOF:
            NextToken ();
            if (TypeSpecAhead ()) {
                Type T[MAXTYPELEN];
                NextToken ();
                Size = CheckedSizeOf (ParseType (T));
                ConsumeRParen ();
            } else {
                /* Remember the output queue pointer */
                CodeMark Mark;
                GetCodePos (&Mark);
                hie10 (Expr);
                /* If the expression is a literal string, release it, so it
                ** won't be output as data if not used elsewhere.
                */
                if (ED_IsLocLiteral (Expr)) {
                    ReleaseLiteral (Expr->LVal);
                }
                /* Calculate the size */
                Size = CheckedSizeOf (Expr->Type);
                /* Remove any generated code */
                RemoveCode (&Mark);
            }
            ED_MakeConstAbs (Expr, Size, type_size_t);
            ED_MarkAsUntested (Expr);
            break;

        default:
            if (TypeSpecAhead ()) {

                /* A typecast */
                TypeCast (Expr);

            } else {

                /* An expression */
                hie11 (Expr);

                /* Handle post increment */
                switch (CurTok.Tok) {
                    case TOK_INC:   PostInc (Expr); break;
                    case TOK_DEC:   PostDec (Expr); break;
                    default:                        break;
                }

            }
            break;
    }
}



static void hie_internal (const GenDesc* Ops,   /* List of generators */
                          ExprDesc* Expr,
                          void (*hienext) (ExprDesc*),
                          int* UsedGen)
/* Helper function */
{
    ExprDesc Expr2;
    CodeMark Mark1;
    CodeMark Mark2;
    const GenDesc* Gen;
    token_t Tok;                        /* The operator token */
    unsigned ltype, type;
    int lconst;                         /* Left operand is a constant */
    int rconst;                         /* Right operand is a constant */


    ExprWithCheck (hienext, Expr);

    *UsedGen = 0;
    while ((Gen = FindGen (CurTok.Tok, Ops)) != 0) {

        /* Tell the caller that we handled it's ops */
        *UsedGen = 1;

        /* All operators that call this function expect an int on the lhs */
        if (!IsClassInt (Expr->Type)) {
            Error ("Integer expression expected");
            /* To avoid further errors, make Expr a valid int expression */
            ED_MakeConstAbsInt (Expr, 1);
        }

        /* Remember the operator token, then skip it */
        Tok = CurTok.Tok;
        NextToken ();

        /* Get the lhs on stack */
        GetCodePos (&Mark1);
        ltype = TypeOf (Expr->Type);
        lconst = ED_IsConstAbs (Expr);
        if (lconst) {
            /* Constant value */
            GetCodePos (&Mark2);
            /* If the operator is commutative, don't push the left side, if
            ** it's a constant, since we will exchange both operands.
            */
            if ((Gen->Flags & GEN_COMM) == 0) {
                g_push (ltype | CF_CONST, Expr->IVal);
            }
        } else {
            /* Value not constant */
            LoadExpr (CF_NONE, Expr);
            GetCodePos (&Mark2);
            g_push (ltype, 0);
        }

        /* Get the right hand side */
        MarkedExprWithCheck (hienext, &Expr2);

        /* Check for a constant expression */
        rconst = (ED_IsConstAbs (&Expr2) && ED_CodeRangeIsEmpty (&Expr2));
        if (!rconst) {
            /* Not constant, load into the primary */
            LoadExpr (CF_NONE, &Expr2);
        }

        /* Check the type of the rhs */
        if (!IsClassInt (Expr2.Type)) {
            Error ("Integer expression expected");
        }

        /* Check for const operands */
        if (lconst && rconst) {

            /* Both operands are constant, remove the generated code */
            RemoveCode (&Mark1);

            /* Get the type of the result */
            Expr->Type = promoteint (Expr->Type, Expr2.Type);

            /* Handle the op differently for signed and unsigned types */
            if (IsSignSigned (Expr->Type)) {

                /* Evaluate the result for signed operands */
                signed long Val1 = Expr->IVal;
                signed long Val2 = Expr2.IVal;
                switch (Tok) {
                    case TOK_OR:
                        Expr->IVal = (Val1 | Val2);
                        break;
                    case TOK_XOR:
                        Expr->IVal = (Val1 ^ Val2);
                        break;
                    case TOK_AND:
                        Expr->IVal = (Val1 & Val2);
                        break;
                    case TOK_STAR:
                        Expr->IVal = (Val1 * Val2);
                        break;
                    case TOK_DIV:
                        if (Val2 == 0) {
                            Error ("Division by zero");
                            Expr->IVal = 0x7FFFFFFF;
                        } else {
                            Expr->IVal = (Val1 / Val2);
                        }
                        break;
                    case TOK_MOD:
                        if (Val2 == 0) {
                            Error ("Modulo operation with zero");
                            Expr->IVal = 0;
                        } else {
                            Expr->IVal = (Val1 % Val2);
                        }
                        break;
                    default:
                        Internal ("hie_internal: got token 0x%X\n", Tok);
                }
            } else {

                /* Evaluate the result for unsigned operands */
                unsigned long Val1 = Expr->IVal;
                unsigned long Val2 = Expr2.IVal;
                switch (Tok) {
                    case TOK_OR:
                        Expr->IVal = (Val1 | Val2);
                        break;
                    case TOK_XOR:
                        Expr->IVal = (Val1 ^ Val2);
                        break;
                    case TOK_AND:
                        Expr->IVal = (Val1 & Val2);
                        break;
                    case TOK_STAR:
                        Expr->IVal = (Val1 * Val2);
                        break;
                    case TOK_DIV:
                        if (Val2 == 0) {
                            Error ("Division by zero");
                            Expr->IVal = 0xFFFFFFFF;
                        } else {
                            Expr->IVal = (Val1 / Val2);
                        }
                        break;
                    case TOK_MOD:
                        if (Val2 == 0) {
                            Error ("Modulo operation with zero");
                            Expr->IVal = 0;
                        } else {
                            Expr->IVal = (Val1 % Val2);
                        }
                        break;
                    default:
                        Internal ("hie_internal: got token 0x%X\n", Tok);
                }
            }

        } else if (lconst && (Gen->Flags & GEN_COMM) && !rconst) {

            /* The left side is constant, the right side is not, and the
            ** operator allows swapping the operands. We haven't pushed the
            ** left side onto the stack in this case, and will reverse the
            ** operation because this allows for better code.
            */
            unsigned rtype = ltype | CF_CONST;
            ltype = TypeOf (Expr2.Type);       /* Expr2 is now left */
            type = CF_CONST;
            if ((Gen->Flags & GEN_NOPUSH) == 0) {
                g_push (ltype, 0);
            } else {
                ltype |= CF_REG;        /* Value is in register */
            }

            /* Determine the type of the operation result. */
            type |= g_typeadjust (ltype, rtype);
            Expr->Type = promoteint (Expr->Type, Expr2.Type);

            /* Generate code */
            Gen->Func (type, Expr->IVal);

            /* We have a rvalue in the primary now */
            ED_MakeRValExpr (Expr);

        } else {

            /* If the right hand side is constant, and the generator function
            ** expects the lhs in the primary, remove the push of the primary
            ** now.
            */
            unsigned rtype = TypeOf (Expr2.Type);
            type = 0;
            if (rconst) {
                /* Second value is constant - check for div */
                type |= CF_CONST;
                rtype |= CF_CONST;
                if (Tok == TOK_DIV && Expr2.IVal == 0) {
                    Error ("Division by zero");
                } else if (Tok == TOK_MOD && Expr2.IVal == 0) {
                    Error ("Modulo operation with zero");
                }
                if ((Gen->Flags & GEN_NOPUSH) != 0) {
                    RemoveCode (&Mark2);
                    ltype |= CF_REG;    /* Value is in register */
                }
            }

            /* Determine the type of the operation result. */
            type |= g_typeadjust (ltype, rtype);
            Expr->Type = promoteint (Expr->Type, Expr2.Type);

            /* Generate code */
            Gen->Func (type, Expr2.IVal);

            /* We have a rvalue in the primary now */
            ED_MakeRValExpr (Expr);
        }
    }
}



static void hie_compare (const GenDesc* Ops,    /* List of generators */
                         ExprDesc* Expr,
                         void (*hienext) (ExprDesc*))
/* Helper function for the compare operators */
{
    ExprDesc Expr2;
    CodeMark Mark0;
    CodeMark Mark1;
    CodeMark Mark2;
    const GenDesc* Gen;
    token_t Tok;                        /* The operator token */
    unsigned ltype;
    int rconst;                         /* Operand is a constant */


    GetCodePos (&Mark0);
    ExprWithCheck (hienext, Expr);

    while ((Gen = FindGen (CurTok.Tok, Ops)) != 0) {

        /* Remember the generator function */
        void (*GenFunc) (unsigned, unsigned long) = Gen->Func;

        /* Remember the operator token, then skip it */
        Tok = CurTok.Tok;
        NextToken ();

        /* If lhs is a function, convert it to pointer to function */
        if (IsTypeFunc (Expr->Type)) {
            Expr->Type = PointerTo (Expr->Type);
        }

        /* Get the lhs on stack */
        GetCodePos (&Mark1);
        ltype = TypeOf (Expr->Type);
        if (ED_IsConstAbs (Expr)) {
            /* Constant value */
            GetCodePos (&Mark2);
            g_push (ltype | CF_CONST, Expr->IVal);
        } else {
            /* Value not constant */
            LoadExpr (CF_NONE, Expr);
            GetCodePos (&Mark2);
            g_push (ltype, 0);
        }

        /* Get the right hand side */
        MarkedExprWithCheck (hienext, &Expr2);

        /* If rhs is a function, convert it to pointer to function */
        if (IsTypeFunc (Expr2.Type)) {
            Expr2.Type = PointerTo (Expr2.Type);
        }

        /* Check for a constant expression */
        rconst = (ED_IsConstAbs (&Expr2) && ED_CodeRangeIsEmpty (&Expr2));
        if (!rconst) {
            /* Not constant, load into the primary */
            LoadExpr (CF_NONE, &Expr2);
        }

        /* Some operations aren't allowed on function pointers */
        if ((Gen->Flags & GEN_NOFUNC) != 0) {
            /* Output only one message even if both sides are wrong */
            if (IsTypeFuncPtr (Expr->Type)) {
                Error ("Invalid left operand for relational operator");
                /* Avoid further errors */
                ED_MakeConstAbsInt (Expr, 0);
                ED_MakeConstAbsInt (&Expr2, 0);
            } else if (IsTypeFuncPtr (Expr2.Type)) {
                Error ("Invalid right operand for relational operator");
                /* Avoid further errors */
                ED_MakeConstAbsInt (Expr, 0);
                ED_MakeConstAbsInt (&Expr2, 0);
            }
        }

        /* Make sure, the types are compatible */
        if (IsClassInt (Expr->Type)) {
            if (!IsClassInt (Expr2.Type) && !(IsClassPtr(Expr2.Type) && ED_IsNullPtr(Expr))) {
                Error ("Incompatible types");
            }
        } else if (IsClassPtr (Expr->Type)) {
            if (IsClassPtr (Expr2.Type)) {
                /* Both pointers are allowed in comparison if they point to
                ** the same type, or if one of them is a void pointer.
                */
                Type* left  = Indirect (Expr->Type);
                Type* right = Indirect (Expr2.Type);
                if (TypeCmp (left, right) < TC_EQUAL && left->C != T_VOID && right->C != T_VOID) {
                    /* Incomatible pointers */
                    Error ("Incompatible types");
                }
            } else if (!ED_IsNullPtr (&Expr2)) {
                Error ("Incompatible types");
            }
        }

        /* Check for const operands */
        if (ED_IsConstAbs (Expr) && rconst) {

            /* If the result is constant, this is suspicious when not in
            ** preprocessor mode.
            */
            WarnConstCompareResult ();

            /* Both operands are constant, remove the generated code */
            RemoveCode (&Mark1);

            /* Determine if this is a signed or unsigned compare */
            if (IsClassInt (Expr->Type) && IsSignSigned (Expr->Type) &&
                IsClassInt (Expr2.Type) && IsSignSigned (Expr2.Type)) {

                /* Evaluate the result for signed operands */
                signed long Val1 = Expr->IVal;
                signed long Val2 = Expr2.IVal;
                switch (Tok) {
                    case TOK_EQ: Expr->IVal = (Val1 == Val2);   break;
                    case TOK_NE: Expr->IVal = (Val1 != Val2);   break;
                    case TOK_LT: Expr->IVal = (Val1 < Val2);    break;
                    case TOK_LE: Expr->IVal = (Val1 <= Val2);   break;
                    case TOK_GE: Expr->IVal = (Val1 >= Val2);   break;
                    case TOK_GT: Expr->IVal = (Val1 > Val2);    break;
                    default:     Internal ("hie_compare: got token 0x%X\n", Tok);
                }

            } else {

                /* Evaluate the result for unsigned operands */
                unsigned long Val1 = Expr->IVal;
                unsigned long Val2 = Expr2.IVal;
                switch (Tok) {
                    case TOK_EQ: Expr->IVal = (Val1 == Val2);   break;
                    case TOK_NE: Expr->IVal = (Val1 != Val2);   break;
                    case TOK_LT: Expr->IVal = (Val1 < Val2);    break;
                    case TOK_LE: Expr->IVal = (Val1 <= Val2);   break;
                    case TOK_GE: Expr->IVal = (Val1 >= Val2);   break;
                    case TOK_GT: Expr->IVal = (Val1 > Val2);    break;
                    default:     Internal ("hie_compare: got token 0x%X\n", Tok);
                }
            }

        } else {

            /* Determine the signedness of the operands */
            int LeftSigned  = IsSignSigned (Expr->Type);
            int RightSigned = IsSignSigned (Expr2.Type);

            /* If the right hand side is constant, and the generator function
            ** expects the lhs in the primary, remove the push of the primary
            ** now.
            */
            unsigned flags = 0;
            if (rconst) {
                flags |= CF_CONST;
                if ((Gen->Flags & GEN_NOPUSH) != 0) {
                    RemoveCode (&Mark2);
                    ltype |= CF_REG;    /* Value is in register */
                }
            }

            /* Determine the type of the operation. */
            if (IsTypeChar (Expr->Type) && rconst) {

                /* Left side is unsigned char, right side is constant.
                ** Determine the minimum and maximum values
                */
                int LeftMin, LeftMax;
                if (LeftSigned) {
                    LeftMin = -128;
                    LeftMax = 127;
                } else {
                    LeftMin = 0;
                    LeftMax = 255;
                }
                /* An integer value is always represented as a signed in the
                ** ExprDesc structure. This may lead to false results below,
                ** if it is actually unsigned, but interpreted as signed
                ** because of the representation. Fortunately, in this case,
                ** the actual value doesn't matter, since it's always greater
                ** than what can be represented in a char. So correct the
                ** value accordingly.
                */
                if (!RightSigned && Expr2.IVal < 0) {
                    /* Correct the value so it is an unsigned. It will then
                    ** anyway match one of the cases below.
                    */
                    Expr2.IVal = LeftMax + 1;
                }

                /* Comparing a char against a constant may have a constant
                ** result. Please note: It is not possible to remove the code
                ** for the compare alltogether, because it may have side
                ** effects.
                */
                switch (Tok) {

                    case TOK_EQ:
                        if (Expr2.IVal < LeftMin || Expr2.IVal > LeftMax) {
                            ED_MakeConstAbsInt (Expr, 0);
                            WarnConstCompareResult ();
                            goto Done;
                        }
                        break;

                    case TOK_NE:
                        if (Expr2.IVal < LeftMin || Expr2.IVal > LeftMax) {
                            ED_MakeConstAbsInt (Expr, 1);
                            WarnConstCompareResult ();
                            goto Done;
                        }
                        break;

                    case TOK_LT:
                        if (Expr2.IVal <= LeftMin || Expr2.IVal > LeftMax) {
                            ED_MakeConstAbsInt (Expr, Expr2.IVal > LeftMax);
                            WarnConstCompareResult ();
                            goto Done;
                        }
                        break;

                    case TOK_LE:
                        if (Expr2.IVal < LeftMin || Expr2.IVal >= LeftMax) {
                            ED_MakeConstAbsInt (Expr, Expr2.IVal >= LeftMax);
                            WarnConstCompareResult ();
                            goto Done;
                        }
                        break;

                    case TOK_GE:
                        if (Expr2.IVal <= LeftMin || Expr2.IVal > LeftMax) {
                            ED_MakeConstAbsInt (Expr, Expr2.IVal <= LeftMin);
                            WarnConstCompareResult ();
                            goto Done;
                        }
                        break;

                    case TOK_GT:
                        if (Expr2.IVal < LeftMin || Expr2.IVal >= LeftMax) {
                            ED_MakeConstAbsInt (Expr, Expr2.IVal < LeftMin);
                            WarnConstCompareResult ();
                            goto Done;
                        }
                        break;

                    default:
                        Internal ("hie_compare: got token 0x%X\n", Tok);
                }

                /* If the result is not already constant (as evaluated in the
                ** switch above), we can execute the operation as a char op,
                ** since the right side constant is in a valid range.
                */
                flags |= (CF_CHAR | CF_FORCECHAR);
                if (!LeftSigned) {
                    flags |= CF_UNSIGNED;
                }

            } else if (IsTypeChar (Expr->Type) && IsTypeChar (Expr2.Type) &&
                GetSignedness (Expr->Type) == GetSignedness (Expr2.Type)) {

                /* Both are chars with the same signedness. We can encode the
                ** operation as a char operation.
                */
                flags |= CF_CHAR;
                if (rconst) {
                    flags |= CF_FORCECHAR;
                }
                if (!LeftSigned) {
                    flags |= CF_UNSIGNED;
                }
            } else {
                unsigned rtype = TypeOf (Expr2.Type) | (flags & CF_CONST);
                flags |= g_typeadjust (ltype, rtype);
            }

            /* If the left side is an unsigned and the right is a constant,
            ** we may be able to change the compares to something more
            ** effective.
            */
            if (!LeftSigned && rconst) {

                switch (Tok) {

                    case TOK_LT:
                        if (Expr2.IVal == 1) {
                            /* An unsigned compare to one means that the value
                            ** must be zero.
                            */
                            GenFunc = g_eq;
                            Expr2.IVal = 0;
                        }
                        break;

                    case TOK_LE:
                        if (Expr2.IVal == 0) {
                            /* An unsigned compare to zero means that the value
                            ** must be zero.
                            */
                            GenFunc = g_eq;
                        }
                        break;

                    case TOK_GE:
                        if (Expr2.IVal == 1) {
                            /* An unsigned compare to one means that the value
                            ** must not be zero.
                            */
                            GenFunc = g_ne;
                            Expr2.IVal = 0;
                        }
                        break;

                    case TOK_GT:
                        if (Expr2.IVal == 0) {
                            /* An unsigned compare to zero means that the value
                            ** must not be zero.
                            */
                            GenFunc = g_ne;
                        }
                        break;

                    default:
                        break;

                }

            }

            /* Generate code */
            GenFunc (flags, Expr2.IVal);

            /* The result is an rvalue in the primary */
            ED_MakeRValExpr (Expr);
        }

        /* Result type is always int */
        Expr->Type = type_int;

Done:   /* Condition codes are set */
        ED_TestDone (Expr);
    }
}



static void hie9 (ExprDesc *Expr)
/* Process * and / operators. */
{
    static const GenDesc hie9_ops[] = {
        { TOK_STAR,     GEN_NOPUSH | GEN_COMM,  g_mul   },
        { TOK_DIV,      GEN_NOPUSH,             g_div   },
        { TOK_MOD,      GEN_NOPUSH,             g_mod   },
        { TOK_INVALID,  0,                      0       }
    };
    int UsedGen;

    hie_internal (hie9_ops, Expr, hie10, &UsedGen);
}



static void parseadd (ExprDesc* Expr)
/* Parse an expression with the binary plus operator. Expr contains the
** unprocessed left hand side of the expression and will contain the
** result of the expression on return.
*/
{
    ExprDesc Expr2;
    unsigned flags;             /* Operation flags */
    CodeMark Mark;              /* Remember code position */
    Type* lhst;                 /* Type of left hand side */
    Type* rhst;                 /* Type of right hand side */


    /* Skip the PLUS token */
    NextToken ();

    /* Get the left hand side type, initialize operation flags */
    lhst = Expr->Type;
    flags = 0;

    /* Check for constness on both sides */
    if (ED_IsConst (Expr)) {

        /* The left hand side is a constant of some sort. Good. Get rhs */
        ExprWithCheck (hie9, &Expr2);
        if (ED_IsConstAbs (&Expr2)) {

            /* Right hand side is a constant numeric value. Get the rhs type */
            rhst = Expr2.Type;

            /* Both expressions are constants. Check for pointer arithmetic */
            if (IsClassPtr (lhst) && IsClassInt (rhst)) {
                /* Left is pointer, right is int, must scale rhs */
                Expr->IVal += Expr2.IVal * CheckedPSizeOf (lhst);
                /* Result type is a pointer */
            } else if (IsClassInt (lhst) && IsClassPtr (rhst)) {
                /* Left is int, right is pointer, must scale lhs */
                Expr->IVal = Expr->IVal * CheckedPSizeOf (rhst) + Expr2.IVal;
                /* Result type is a pointer */
                Expr->Type = Expr2.Type;
            } else if (IsClassInt (lhst) && IsClassInt (rhst)) {
                /* Integer addition */
                Expr->IVal += Expr2.IVal;
                typeadjust (Expr, &Expr2, 1);
            } else {
                /* OOPS */
                Error ("Invalid operands for binary operator `+'");
            }

        } else {

            /* lhs is a constant and rhs is not constant. Load rhs into
            ** the primary.
            */
            LoadExpr (CF_NONE, &Expr2);

            /* Beware: The check above (for lhs) lets not only pass numeric
            ** constants, but also constant addresses (labels), maybe even
            ** with an offset. We have to check for that here.
            */

            /* First, get the rhs type. */
            rhst = Expr2.Type;

            /* Setup flags */
            if (ED_IsLocAbs (Expr)) {
                /* A numerical constant */
                flags |= CF_CONST;
            } else {
                /* Constant address label */
                flags |= GlobalModeFlags (Expr) | CF_CONSTADDR;
            }

            /* Check for pointer arithmetic */
            if (IsClassPtr (lhst) && IsClassInt (rhst)) {
                /* Left is pointer, right is int, must scale rhs */
                g_scale (CF_INT, CheckedPSizeOf (lhst));
                /* Operate on pointers, result type is a pointer */
                flags |= CF_PTR;
                /* Generate the code for the add */
                if (ED_GetLoc (Expr) == E_LOC_ABS) {
                    /* Numeric constant */
                    g_inc (flags, Expr->IVal);
                } else {
                    /* Constant address */
                    g_addaddr_static (flags, Expr->Name, Expr->IVal);
                }
            } else if (IsClassInt (lhst) && IsClassPtr (rhst)) {

                /* Left is int, right is pointer, must scale lhs. */
                unsigned ScaleFactor = CheckedPSizeOf (rhst);

                /* Operate on pointers, result type is a pointer */
                flags |= CF_PTR;
                Expr->Type = Expr2.Type;

                /* Since we do already have rhs in the primary, if lhs is
                ** not a numeric constant, and the scale factor is not one
                ** (no scaling), we must take the long way over the stack.
                */
                if (ED_IsLocAbs (Expr)) {
                    /* Numeric constant, scale lhs */
                    Expr->IVal *= ScaleFactor;
                    /* Generate the code for the add */
                    g_inc (flags, Expr->IVal);
                } else if (ScaleFactor == 1) {
                    /* Constant address but no need to scale */
                    g_addaddr_static (flags, Expr->Name, Expr->IVal);
                } else {
                    /* Constant address that must be scaled */
                    g_push (TypeOf (Expr2.Type), 0);    /* rhs --> stack */
                    g_getimmed (flags, Expr->Name, Expr->IVal);
                    g_scale (CF_PTR, ScaleFactor);
                    g_add (CF_PTR, 0);
                }
            } else if (IsClassInt (lhst) && IsClassInt (rhst)) {
                /* Integer addition */
                flags |= typeadjust (Expr, &Expr2, 1);
                /* Generate the code for the add */
                if (ED_IsLocAbs (Expr)) {
                    /* Numeric constant */
                    g_inc (flags, Expr->IVal);
                } else {
                    /* Constant address */
                    g_addaddr_static (flags, Expr->Name, Expr->IVal);
                }
            } else {
                /* OOPS */
                Error ("Invalid operands for binary operator `+'");
                flags = CF_INT;
            }

            /* Result is a rvalue in primary register */
            ED_MakeRValExpr (Expr);
        }

    } else {

        /* Left hand side is not constant. Get the value onto the stack. */
        LoadExpr (CF_NONE, Expr);              /* --> primary register */
        GetCodePos (&Mark);
        g_push (TypeOf (Expr->Type), 0);        /* --> stack */

        /* Evaluate the rhs */
        MarkedExprWithCheck (hie9, &Expr2);

        /* Check for a constant rhs expression */
        if (ED_IsConstAbs (&Expr2) && ED_CodeRangeIsEmpty (&Expr2)) {

            /* Right hand side is a constant. Get the rhs type */
            rhst = Expr2.Type;

            /* Remove pushed value from stack */
            RemoveCode (&Mark);

            /* Check for pointer arithmetic */
            if (IsClassPtr (lhst) && IsClassInt (rhst)) {
                /* Left is pointer, right is int, must scale rhs */
                Expr2.IVal *= CheckedPSizeOf (lhst);
                /* Operate on pointers, result type is a pointer */
                flags = CF_PTR;
            } else if (IsClassInt (lhst) && IsClassPtr (rhst)) {
                /* Left is int, right is pointer, must scale lhs (ptr only) */
                g_scale (CF_INT | CF_CONST, CheckedPSizeOf (rhst));
                /* Operate on pointers, result type is a pointer */
                flags = CF_PTR;
                Expr->Type = Expr2.Type;
            } else if (IsClassInt (lhst) && IsClassInt (rhst)) {
                /* Integer addition */
                flags = typeadjust (Expr, &Expr2, 1);
            } else {
                /* OOPS */
                Error ("Invalid operands for binary operator `+'");
                flags = CF_INT;
            }

            /* Generate code for the add */
            g_inc (flags | CF_CONST, Expr2.IVal);

        } else {

            /* Not constant, load into the primary */
            LoadExpr (CF_NONE, &Expr2);

            /* lhs and rhs are not constant. Get the rhs type. */
            rhst = Expr2.Type;

            /* Check for pointer arithmetic */
            if (IsClassPtr (lhst) && IsClassInt (rhst)) {
                /* Left is pointer, right is int, must scale rhs */
                g_scale (CF_INT, CheckedPSizeOf (lhst));
                /* Operate on pointers, result type is a pointer */
                flags = CF_PTR;
            } else if (IsClassInt (lhst) && IsClassPtr (rhst)) {
                /* Left is int, right is pointer, must scale lhs */
                g_tosint (TypeOf (rhst));       /* Make sure, TOS is int */
                g_swap (CF_INT);                /* Swap TOS and primary */
                g_scale (CF_INT, CheckedPSizeOf (rhst));
                /* Operate on pointers, result type is a pointer */
                flags = CF_PTR;
                Expr->Type = Expr2.Type;
            } else if (IsClassInt (lhst) && IsClassInt (rhst)) {
                /* Integer addition. Note: Result is never constant.
                ** Problem here is that typeadjust does not know if the
                ** variable is an rvalue or lvalue, so if both operands
                ** are dereferenced constant numeric addresses, typeadjust
                ** thinks the operation works on constants. Removing
                ** CF_CONST here means handling the symptoms, however, the
                ** whole parser is such a mess that I fear to break anything
                ** when trying to apply another solution.
                */
                flags = typeadjust (Expr, &Expr2, 0) & ~CF_CONST;
            } else {
                /* OOPS */
                Error ("Invalid operands for binary operator `+'");
                flags = CF_INT;
            }

            /* Generate code for the add */
            g_add (flags, 0);

        }

        /* Result is a rvalue in primary register */
        ED_MakeRValExpr (Expr);
    }

    /* Condition codes not set */
    ED_MarkAsUntested (Expr);

}



static void parsesub (ExprDesc* Expr)
/* Parse an expression with the binary minus operator. Expr contains the
** unprocessed left hand side of the expression and will contain the
** result of the expression on return.
*/
{
    ExprDesc Expr2;
    unsigned flags;             /* Operation flags */
    Type* lhst;                 /* Type of left hand side */
    Type* rhst;                 /* Type of right hand side */
    CodeMark Mark1;             /* Save position of output queue */
    CodeMark Mark2;             /* Another position in the queue */
    int rscale;                 /* Scale factor for the result */


    /* lhs cannot be function or pointer to function */
    if (IsTypeFunc (Expr->Type) || IsTypeFuncPtr (Expr->Type)) {
        Error ("Invalid left operand for binary operator `-'");
        /* Make it pointer to char to avoid further errors */
        Expr->Type = type_uchar;
    }

    /* Skip the MINUS token */
    NextToken ();

    /* Get the left hand side type, initialize operation flags */
    lhst = Expr->Type;
    rscale = 1;                 /* Scale by 1, that is, don't scale */

    /* Remember the output queue position, then bring the value onto the stack */
    GetCodePos (&Mark1);
    LoadExpr (CF_NONE, Expr);  /* --> primary register */
    GetCodePos (&Mark2);
    g_push (TypeOf (lhst), 0);  /* --> stack */

    /* Parse the right hand side */
    MarkedExprWithCheck (hie9, &Expr2);

    /* rhs cannot be function or pointer to function */
    if (IsTypeFunc (Expr2.Type) || IsTypeFuncPtr (Expr2.Type)) {
        Error ("Invalid right operand for binary operator `-'");
        /* Make it pointer to char to avoid further errors */
        Expr2.Type = type_uchar;
    }

    /* Check for a constant rhs expression */
    if (ED_IsConstAbs (&Expr2) && ED_CodeRangeIsEmpty (&Expr2)) {

        /* The right hand side is constant. Get the rhs type. */
        rhst = Expr2.Type;

        /* Check left hand side */
        if (ED_IsConstAbs (Expr)) {

            /* Both sides are constant, remove generated code */
            RemoveCode (&Mark1);

            /* Check for pointer arithmetic */
            if (IsClassPtr (lhst) && IsClassInt (rhst)) {
                /* Left is pointer, right is int, must scale rhs */
                Expr->IVal -= Expr2.IVal * CheckedPSizeOf (lhst);
                /* Operate on pointers, result type is a pointer */
            } else if (IsClassPtr (lhst) && IsClassPtr (rhst)) {
                /* Left is pointer, right is pointer, must scale result */
                if (TypeCmp (Indirect (lhst), Indirect (rhst)) < TC_QUAL_DIFF) {
                    Error ("Incompatible pointer types");
                } else {
                    Expr->IVal = (Expr->IVal - Expr2.IVal) /
                                      CheckedPSizeOf (lhst);
                }
                /* Operate on pointers, result type is an integer */
                Expr->Type = type_int;
            } else if (IsClassInt (lhst) && IsClassInt (rhst)) {
                /* Integer subtraction */
                typeadjust (Expr, &Expr2, 1);
                Expr->IVal -= Expr2.IVal;
            } else {
                /* OOPS */
                Error ("Invalid operands for binary operator `-'");
            }

            /* Result is constant, condition codes not set */
            ED_MarkAsUntested (Expr);

        } else {

            /* Left hand side is not constant, right hand side is.
            ** Remove pushed value from stack.
            */
            RemoveCode (&Mark2);

            if (IsClassPtr (lhst) && IsClassInt (rhst)) {
                /* Left is pointer, right is int, must scale rhs */
                Expr2.IVal *= CheckedPSizeOf (lhst);
                /* Operate on pointers, result type is a pointer */
                flags = CF_PTR;
            } else if (IsClassPtr (lhst) && IsClassPtr (rhst)) {
                /* Left is pointer, right is pointer, must scale result */
                if (TypeCmp (Indirect (lhst), Indirect (rhst)) < TC_QUAL_DIFF) {
                    Error ("Incompatible pointer types");
                } else {
                    rscale = CheckedPSizeOf (lhst);
                }
                /* Operate on pointers, result type is an integer */
                flags = CF_PTR;
                Expr->Type = type_int;
            } else if (IsClassInt (lhst) && IsClassInt (rhst)) {
                /* Integer subtraction */
                flags = typeadjust (Expr, &Expr2, 1);
            } else {
                /* OOPS */
                Error ("Invalid operands for binary operator `-'");
                flags = CF_INT;
            }

            /* Do the subtraction */
            g_dec (flags | CF_CONST, Expr2.IVal);

            /* If this was a pointer subtraction, we must scale the result */
            if (rscale != 1) {
                g_scale (flags, -rscale);
            }

            /* Result is a rvalue in the primary register */
            ED_MakeRValExpr (Expr);
            ED_MarkAsUntested (Expr);

        }

    } else {

        /* Not constant, load into the primary */
        LoadExpr (CF_NONE, &Expr2);

        /* Right hand side is not constant. Get the rhs type. */
        rhst = Expr2.Type;

        /* Check for pointer arithmetic */
        if (IsClassPtr (lhst) && IsClassInt (rhst)) {
            /* Left is pointer, right is int, must scale rhs */
            g_scale (CF_INT, CheckedPSizeOf (lhst));
            /* Operate on pointers, result type is a pointer */
            flags = CF_PTR;
        } else if (IsClassPtr (lhst) && IsClassPtr (rhst)) {
            /* Left is pointer, right is pointer, must scale result */
            if (TypeCmp (Indirect (lhst), Indirect (rhst)) < TC_QUAL_DIFF) {
                Error ("Incompatible pointer types");
            } else {
                rscale = CheckedPSizeOf (lhst);
            }
            /* Operate on pointers, result type is an integer */
            flags = CF_PTR;
            Expr->Type = type_int;
        } else if (IsClassInt (lhst) && IsClassInt (rhst)) {
            /* Integer subtraction. If the left hand side descriptor says that
            ** the lhs is const, we have to remove this mark, since this is no
            ** longer true, lhs is on stack instead.
            */
            if (ED_IsLocAbs (Expr)) {
                ED_MakeRValExpr (Expr);
            }
            /* Adjust operand types */
            flags = typeadjust (Expr, &Expr2, 0);
        } else {
            /* OOPS */
            Error ("Invalid operands for binary operator `-'");
            flags = CF_INT;
        }

        /* Generate code for the sub (the & is a hack here) */
        g_sub (flags & ~CF_CONST, 0);

        /* If this was a pointer subtraction, we must scale the result */
        if (rscale != 1) {
            g_scale (flags, -rscale);
        }

        /* Result is a rvalue in the primary register */
        ED_MakeRValExpr (Expr);
        ED_MarkAsUntested (Expr);
    }
}



void hie8 (ExprDesc* Expr)
/* Process + and - binary operators. */
{
    ExprWithCheck (hie9, Expr);
    while (CurTok.Tok == TOK_PLUS || CurTok.Tok == TOK_MINUS) {
        if (CurTok.Tok == TOK_PLUS) {
            parseadd (Expr);
        } else {
            parsesub (Expr);
        }
    }
}



static void hie6 (ExprDesc* Expr)
/* Handle greater-than type comparators */
{
    static const GenDesc hie6_ops [] = {
        { TOK_LT,       GEN_NOPUSH | GEN_NOFUNC,     g_lt    },
        { TOK_LE,       GEN_NOPUSH | GEN_NOFUNC,     g_le    },
        { TOK_GE,       GEN_NOPUSH | GEN_NOFUNC,     g_ge    },
        { TOK_GT,       GEN_NOPUSH | GEN_NOFUNC,     g_gt    },
        { TOK_INVALID,  0,                           0       }
    };
    hie_compare (hie6_ops, Expr, ShiftExpr);
}



static void hie5 (ExprDesc* Expr)
/* Handle == and != */
{
    static const GenDesc hie5_ops[] = {
        { TOK_EQ,       GEN_NOPUSH,     g_eq    },
        { TOK_NE,       GEN_NOPUSH,     g_ne    },
        { TOK_INVALID,  0,              0       }
    };
    hie_compare (hie5_ops, Expr, hie6);
}



static void hie4 (ExprDesc* Expr)
/* Handle & (bitwise and) */
{
    static const GenDesc hie4_ops[] = {
        { TOK_AND,      GEN_NOPUSH | GEN_COMM,  g_and   },
        { TOK_INVALID,  0,                      0       }
    };
    int UsedGen;

    hie_internal (hie4_ops, Expr, hie5, &UsedGen);
}



static void hie3 (ExprDesc* Expr)
/* Handle ^ (bitwise exclusive or) */
{
    static const GenDesc hie3_ops[] = {
        { TOK_XOR,      GEN_NOPUSH | GEN_COMM,  g_xor   },
        { TOK_INVALID,  0,                      0       }
    };
    int UsedGen;

    hie_internal (hie3_ops, Expr, hie4, &UsedGen);
}



static void hie2 (ExprDesc* Expr)
/* Handle | (bitwise or) */
{
    static const GenDesc hie2_ops[] = {
        { TOK_OR,       GEN_NOPUSH | GEN_COMM,  g_or    },
        { TOK_INVALID,  0,                      0       }
    };
    int UsedGen;

    hie_internal (hie2_ops, Expr, hie3, &UsedGen);
}



static void hieAndPP (ExprDesc* Expr)
/* Process "exp && exp" in preprocessor mode (that is, when the parser is
** called recursively from the preprocessor.
*/
{
    ExprDesc Expr2;

    ConstAbsIntExpr (hie2, Expr);
    while (CurTok.Tok == TOK_BOOL_AND) {

        /* Skip the && */
        NextToken ();

        /* Get rhs */
        ConstAbsIntExpr (hie2, &Expr2);

        /* Combine the two */
        Expr->IVal = (Expr->IVal && Expr2.IVal);
    }
}



static void hieOrPP (ExprDesc *Expr)
/* Process "exp || exp" in preprocessor mode (that is, when the parser is
** called recursively from the preprocessor.
*/
{
    ExprDesc Expr2;

    ConstAbsIntExpr (hieAndPP, Expr);
    while (CurTok.Tok == TOK_BOOL_OR) {

        /* Skip the && */
        NextToken ();

        /* Get rhs */
        ConstAbsIntExpr (hieAndPP, &Expr2);

        /* Combine the two */
        Expr->IVal = (Expr->IVal || Expr2.IVal);
    }
}



static void hieAnd (ExprDesc* Expr, unsigned TrueLab, int* BoolOp)
/* Process "exp && exp" */
{
    int FalseLab;
    ExprDesc Expr2;

    ExprWithCheck (hie2, Expr);
    if (CurTok.Tok == TOK_BOOL_AND) {

        /* Tell our caller that we're evaluating a boolean */
        *BoolOp = 1;

        /* Get a label that we will use for false expressions */
        FalseLab = GetLocalLabel ();

        /* If the expr hasn't set condition codes, set the force-test flag */
        if (!ED_IsTested (Expr)) {
            ED_MarkForTest (Expr);
        }

        /* Load the value */
        LoadExpr (CF_FORCECHAR, Expr);

        /* Generate the jump */
        g_falsejump (CF_NONE, FalseLab);

        /* Parse more boolean and's */
        while (CurTok.Tok == TOK_BOOL_AND) {

            /* Skip the && */
            NextToken ();

            /* Get rhs */
            hie2 (&Expr2);
            if (!ED_IsTested (&Expr2)) {
                ED_MarkForTest (&Expr2);
            }
            LoadExpr (CF_FORCECHAR, &Expr2);

            /* Do short circuit evaluation */
            if (CurTok.Tok == TOK_BOOL_AND) {
                g_falsejump (CF_NONE, FalseLab);
            } else {
                /* Last expression - will evaluate to true */
                g_truejump (CF_NONE, TrueLab);
            }
        }

        /* Define the false jump label here */
        g_defcodelabel (FalseLab);

        /* The result is an rvalue in primary */
        ED_MakeRValExpr (Expr);
        ED_TestDone (Expr);     /* Condition codes are set */
    }
}



static void hieOr (ExprDesc *Expr)
/* Process "exp || exp". */
{
    ExprDesc Expr2;
    int BoolOp = 0;             /* Did we have a boolean op? */
    int AndOp;                  /* Did we have a && operation? */
    unsigned TrueLab;           /* Jump to this label if true */
    unsigned DoneLab;

    /* Get a label */
    TrueLab = GetLocalLabel ();

    /* Call the next level parser */
    hieAnd (Expr, TrueLab, &BoolOp);

    /* Any boolean or's? */
    if (CurTok.Tok == TOK_BOOL_OR) {

        /* If the expr hasn't set condition codes, set the force-test flag */
        if (!ED_IsTested (Expr)) {
            ED_MarkForTest (Expr);
        }

        /* Get first expr */
        LoadExpr (CF_FORCECHAR, Expr);

        /* For each expression jump to TrueLab if true. Beware: If we
        ** had && operators, the jump is already in place!
        */
        if (!BoolOp) {
            g_truejump (CF_NONE, TrueLab);
        }

        /* Remember that we had a boolean op */
        BoolOp = 1;

        /* while there's more expr */
        while (CurTok.Tok == TOK_BOOL_OR) {

            /* skip the || */
            NextToken ();

            /* Get a subexpr */
            AndOp = 0;
            hieAnd (&Expr2, TrueLab, &AndOp);
            if (!ED_IsTested (&Expr2)) {
                ED_MarkForTest (&Expr2);
            }
            LoadExpr (CF_FORCECHAR, &Expr2);

            /* If there is more to come, add shortcut boolean eval. */
            g_truejump (CF_NONE, TrueLab);

        }

        /* The result is an rvalue in primary */
        ED_MakeRValExpr (Expr);
        ED_TestDone (Expr);                     /* Condition codes are set */
    }

    /* If we really had boolean ops, generate the end sequence */
    if (BoolOp) {
        DoneLab = GetLocalLabel ();
        g_getimmed (CF_INT | CF_CONST, 0, 0);   /* Load FALSE */
        g_falsejump (CF_NONE, DoneLab);
        g_defcodelabel (TrueLab);
        g_getimmed (CF_INT | CF_CONST, 1, 0);   /* Load TRUE */
        g_defcodelabel (DoneLab);
    }
}



static void hieQuest (ExprDesc* Expr)
/* Parse the ternary operator */
{
    int         FalseLab;
    int         TrueLab;
    CodeMark    TrueCodeEnd;
    ExprDesc    Expr2;          /* Expression 2 */
    ExprDesc    Expr3;          /* Expression 3 */
    int         Expr2IsNULL;    /* Expression 2 is a NULL pointer */
    int         Expr3IsNULL;    /* Expression 3 is a NULL pointer */
    Type*       ResultType;     /* Type of result */


    /* Call the lower level eval routine */
    if (Preprocessing) {
        ExprWithCheck (hieOrPP, Expr);
    } else {
        ExprWithCheck (hieOr, Expr);
    }

    /* Check if it's a ternary expression */
    if (CurTok.Tok == TOK_QUEST) {
        NextToken ();
        if (!ED_IsTested (Expr)) {
            /* Condition codes not set, request a test */
            ED_MarkForTest (Expr);
        }
        LoadExpr (CF_NONE, Expr);
        FalseLab = GetLocalLabel ();
        g_falsejump (CF_NONE, FalseLab);

        /* Parse second expression. Remember for later if it is a NULL pointer
        ** expression, then load it into the primary.
        */
        ExprWithCheck (hie1, &Expr2);
        Expr2IsNULL = ED_IsNullPtr (&Expr2);
        if (!IsTypeVoid (Expr2.Type)) {
            /* Load it into the primary */
            LoadExpr (CF_NONE, &Expr2);
            ED_MakeRValExpr (&Expr2);
            Expr2.Type = PtrConversion (Expr2.Type);
        }

        /* Remember the current code position */
        GetCodePos (&TrueCodeEnd);

        /* Jump around the evaluation of the third expression */
        TrueLab = GetLocalLabel ();
        ConsumeColon ();
        g_jump (TrueLab);

        /* Jump here if the first expression was false */
        g_defcodelabel (FalseLab);

        /* Parse third expression. Remember for later if it is a NULL pointer
        ** expression, then load it into the primary.
        */
        ExprWithCheck (hie1, &Expr3);
        Expr3IsNULL = ED_IsNullPtr (&Expr3);
        if (!IsTypeVoid (Expr3.Type)) {
            /* Load it into the primary */
            LoadExpr (CF_NONE, &Expr3);
            ED_MakeRValExpr (&Expr3);
            Expr3.Type = PtrConversion (Expr3.Type);
        }

        /* Check if any conversions are needed, if so, do them.
        ** Conversion rules for ?: expression are:
        **   - if both expressions are int expressions, default promotion
        **     rules for ints apply.
        **   - if both expressions are pointers of the same type, the
        **     result of the expression is of this type.
        **   - if one of the expressions is a pointer and the other is
        **     a zero constant, the resulting type is that of the pointer
        **     type.
        **   - if both expressions are void expressions, the result is of
        **     type void.
        **   - all other cases are flagged by an error.
        */
        if (IsClassInt (Expr2.Type) && IsClassInt (Expr3.Type)) {

            CodeMark    CvtCodeStart;
            CodeMark    CvtCodeEnd;


            /* Get common type */
            ResultType = promoteint (Expr2.Type, Expr3.Type);

            /* Convert the third expression to this type if needed */
            TypeConversion (&Expr3, ResultType);

            /* Emit conversion code for the second expression, but remember
            ** where it starts end ends.
            */
            GetCodePos (&CvtCodeStart);
            TypeConversion (&Expr2, ResultType);
            GetCodePos (&CvtCodeEnd);

            /* If we had conversion code, move it to the right place */
            if (!CodeRangeIsEmpty (&CvtCodeStart, &CvtCodeEnd)) {
                MoveCode (&CvtCodeStart, &CvtCodeEnd, &TrueCodeEnd);
            }

        } else if (IsClassPtr (Expr2.Type) && IsClassPtr (Expr3.Type)) {
            /* Must point to same type */
            if (TypeCmp (Indirect (Expr2.Type), Indirect (Expr3.Type)) < TC_EQUAL) {
                Error ("Incompatible pointer types");
            }
            /* Result has the common type */
            ResultType = Expr2.Type;
        } else if (IsClassPtr (Expr2.Type) && Expr3IsNULL) {
            /* Result type is pointer, no cast needed */
            ResultType = Expr2.Type;
        } else if (Expr2IsNULL && IsClassPtr (Expr3.Type)) {
            /* Result type is pointer, no cast needed */
            ResultType = Expr3.Type;
        } else if (IsTypeVoid (Expr2.Type) && IsTypeVoid (Expr3.Type)) {
            /* Result type is void */
            ResultType = Expr3.Type;
        } else {
            Error ("Incompatible types");
            ResultType = Expr2.Type;            /* Doesn't matter here */
        }

        /* Define the final label */
        g_defcodelabel (TrueLab);

        /* Setup the target expression */
        ED_MakeRValExpr (Expr);
        Expr->Type  = ResultType;
    }
}



static void opeq (const GenDesc* Gen, ExprDesc* Expr, const char* Op)
/* Process "op=" operators. */
{
    ExprDesc Expr2;
    unsigned flags;
    CodeMark Mark;
    int MustScale;

    /* op= can only be used with lvalues */
    if (!ED_IsLVal (Expr)) {
        Error ("Invalid lvalue in assignment");
        return;
    }

    /* The left side must not be const qualified */
    if (IsQualConst (Expr->Type)) {
        Error ("Assignment to const");
    }

    /* There must be an integer or pointer on the left side */
    if (!IsClassInt (Expr->Type) && !IsTypePtr (Expr->Type)) {
        Error ("Invalid left operand type");
        /* Continue. Wrong code will be generated, but the compiler won't
        ** break, so this is the best error recovery.
        */
    }

    /* Skip the operator token */
    NextToken ();

    /* Determine the type of the lhs */
    flags = TypeOf (Expr->Type);
    MustScale = (Gen->Func == g_add || Gen->Func == g_sub) && IsTypePtr (Expr->Type);

    /* Get the lhs address on stack (if needed) */
    PushAddr (Expr);

    /* Fetch the lhs into the primary register if needed */
    LoadExpr (CF_NONE, Expr);

    /* Bring the lhs on stack */
    GetCodePos (&Mark);
    g_push (flags, 0);

    /* Evaluate the rhs */
    MarkedExprWithCheck (hie1, &Expr2);

    /* The rhs must be an integer (or a float, but we don't support that yet */
    if (!IsClassInt (Expr2.Type)) {
        Error ("Invalid right operand for binary operator `%s'", Op);
        /* Continue. Wrong code will be generated, but the compiler won't
        ** break, so this is the best error recovery.
        */
    }

    /* Check for a constant expression */
    if (ED_IsConstAbs (&Expr2) && ED_CodeRangeIsEmpty (&Expr2)) {
        /* The resulting value is a constant. If the generator has the NOPUSH
        ** flag set, don't push the lhs.
        */
        if (Gen->Flags & GEN_NOPUSH) {
            RemoveCode (&Mark);
        }
        if (MustScale) {
            /* lhs is a pointer, scale rhs */
            Expr2.IVal *= CheckedSizeOf (Expr->Type+1);
        }

        /* If the lhs is character sized, the operation may be later done
        ** with characters.
        */
        if (CheckedSizeOf (Expr->Type) == SIZEOF_CHAR) {
            flags |= CF_FORCECHAR;
        }

        /* Special handling for add and sub - some sort of a hack, but short code */
        if (Gen->Func == g_add) {
            g_inc (flags | CF_CONST, Expr2.IVal);
        } else if (Gen->Func == g_sub) {
            g_dec (flags | CF_CONST, Expr2.IVal);
        } else {
            if (Expr2.IVal == 0) {
                /* Check for div by zero/mod by zero */
                if (Gen->Func == g_div) {
                    Error ("Division by zero");
                } else if (Gen->Func == g_mod) {
                    Error ("Modulo operation with zero");
                }
            }
            Gen->Func (flags | CF_CONST, Expr2.IVal);
        }
    } else {

        /* rhs is not constant. Load into the primary */
        LoadExpr (CF_NONE, &Expr2);
        if (MustScale) {
            /* lhs is a pointer, scale rhs */
            g_scale (TypeOf (Expr2.Type), CheckedSizeOf (Expr->Type+1));
        }

        /* If the lhs is character sized, the operation may be later done
        ** with characters.
        */
        if (CheckedSizeOf (Expr->Type) == SIZEOF_CHAR) {
            flags |= CF_FORCECHAR;
        }

        /* Adjust the types of the operands if needed */
        Gen->Func (g_typeadjust (flags, TypeOf (Expr2.Type)), 0);
    }
    Store (Expr, 0);
    ED_MakeRValExpr (Expr);
}



static void addsubeq (const GenDesc* Gen, ExprDesc *Expr, const char* Op)
/* Process the += and -= operators */
{
    ExprDesc Expr2;
    unsigned lflags;
    unsigned rflags;
    int      MustScale;


    /* We're currently only able to handle some adressing modes */
    if (ED_GetLoc (Expr) == E_LOC_EXPR || ED_GetLoc (Expr) == E_LOC_PRIMARY) {
        /* Use generic routine */
        opeq (Gen, Expr, Op);
        return;
    }

    /* We must have an lvalue */
    if (ED_IsRVal (Expr)) {
        Error ("Invalid lvalue in assignment");
        return;
    }

    /* The left side must not be const qualified */
    if (IsQualConst (Expr->Type)) {
        Error ("Assignment to const");
    }

    /* There must be an integer or pointer on the left side */
    if (!IsClassInt (Expr->Type) && !IsTypePtr (Expr->Type)) {
        Error ("Invalid left operand type");
        /* Continue. Wrong code will be generated, but the compiler won't
        ** break, so this is the best error recovery.
        */
    }

    /* Skip the operator */
    NextToken ();

    /* Check if we have a pointer expression and must scale rhs */
    MustScale = IsTypePtr (Expr->Type);

    /* Initialize the code generator flags */
    lflags = 0;
    rflags = 0;

    /* Evaluate the rhs. We expect an integer here, since float is not
    ** supported
    */
    hie1 (&Expr2);
    if (!IsClassInt (Expr2.Type)) {
        Error ("Invalid right operand for binary operator `%s'", Op);
        /* Continue. Wrong code will be generated, but the compiler won't
        ** break, so this is the best error recovery.
        */
    }
    if (ED_IsConstAbs (&Expr2)) {
        /* The resulting value is a constant. Scale it. */
        if (MustScale) {
            Expr2.IVal *= CheckedSizeOf (Indirect (Expr->Type));
        }
        rflags |= CF_CONST;
        lflags |= CF_CONST;
    } else {
        /* Not constant, load into the primary */
        LoadExpr (CF_NONE, &Expr2);
        if (MustScale) {
            /* lhs is a pointer, scale rhs */
            g_scale (TypeOf (Expr2.Type), CheckedSizeOf (Indirect (Expr->Type)));
        }
    }

    /* Setup the code generator flags */
    lflags |= TypeOf (Expr->Type) | GlobalModeFlags (Expr) | CF_FORCECHAR;
    rflags |= TypeOf (Expr2.Type) | CF_FORCECHAR;

    /* Convert the type of the lhs to that of the rhs */
    g_typecast (lflags, rflags);

    /* Output apropriate code depending on the location */
    switch (ED_GetLoc (Expr)) {

        case E_LOC_ABS:
            /* Absolute: numeric address or const */
            if (Gen->Tok == TOK_PLUS_ASSIGN) {
                g_addeqstatic (lflags, Expr->Name, Expr->IVal, Expr2.IVal);
            } else {
                g_subeqstatic (lflags, Expr->Name, Expr->IVal, Expr2.IVal);
            }
            break;

        case E_LOC_GLOBAL:
            /* Global variable */
            if (Gen->Tok == TOK_PLUS_ASSIGN) {
                g_addeqstatic (lflags, Expr->Name, Expr->IVal, Expr2.IVal);
            } else {
                g_subeqstatic (lflags, Expr->Name, Expr->IVal, Expr2.IVal);
            }
            break;

        case E_LOC_STATIC:
        case E_LOC_LITERAL:
            /* Static variable or literal in the literal pool */
            if (Gen->Tok == TOK_PLUS_ASSIGN) {
                g_addeqstatic (lflags, Expr->Name, Expr->IVal, Expr2.IVal);
            } else {
                g_subeqstatic (lflags, Expr->Name, Expr->IVal, Expr2.IVal);
            }
            break;

        case E_LOC_REGISTER:
            /* Register variable */
            if (Gen->Tok == TOK_PLUS_ASSIGN) {
                g_addeqstatic (lflags, Expr->Name, Expr->IVal, Expr2.IVal);
            } else {
                g_subeqstatic (lflags, Expr->Name, Expr->IVal, Expr2.IVal);
            }
            break;

        case E_LOC_STACK:
            /* Value on the stack */
            if (Gen->Tok == TOK_PLUS_ASSIGN) {
                g_addeqlocal (lflags, Expr->IVal, Expr2.IVal);
            } else {
                g_subeqlocal (lflags, Expr->IVal, Expr2.IVal);
            }
            break;

        default:
            Internal ("Invalid location in Store(): 0x%04X", ED_GetLoc (Expr));
    }

    /* Expression is a rvalue in the primary now */
    ED_MakeRValExpr (Expr);
}



void hie1 (ExprDesc* Expr)
/* Parse first level of expression hierarchy. */
{
    hieQuest (Expr);
    switch (CurTok.Tok) {

        case TOK_ASSIGN:
            Assignment (Expr);
            break;

        case TOK_PLUS_ASSIGN:
            addsubeq (&GenPASGN, Expr, "+=");
            break;

        case TOK_MINUS_ASSIGN:
            addsubeq (&GenSASGN, Expr, "-=");
            break;

        case TOK_MUL_ASSIGN:
            opeq (&GenMASGN, Expr, "*=");
            break;

        case TOK_DIV_ASSIGN:
            opeq (&GenDASGN, Expr, "/=");
            break;

        case TOK_MOD_ASSIGN:
            opeq (&GenMOASGN, Expr, "%=");
            break;

        case TOK_SHL_ASSIGN:
            opeq (&GenSLASGN, Expr, "<<=");
            break;

        case TOK_SHR_ASSIGN:
            opeq (&GenSRASGN, Expr, ">>=");
            break;

        case TOK_AND_ASSIGN:
            opeq (&GenAASGN, Expr, "&=");
            break;

        case TOK_XOR_ASSIGN:
            opeq (&GenXOASGN, Expr, "^=");
            break;

        case TOK_OR_ASSIGN:
            opeq (&GenOASGN, Expr, "|=");
            break;

        default:
            break;
    }
}



void hie0 (ExprDesc *Expr)
/* Parse comma operator. */
{
    hie1 (Expr);
    while (CurTok.Tok == TOK_COMMA) {
        NextToken ();
        hie1 (Expr);
    }
}



int evalexpr (unsigned Flags, void (*Func) (ExprDesc*), ExprDesc* Expr)
/* Will evaluate an expression via the given function. If the result is a
** constant, 0 is returned and the value is put in the Expr struct. If the
** result is not constant, LoadExpr is called to bring the value into the
** primary register and 1 is returned.
*/
{
    /* Evaluate */
    ExprWithCheck (Func, Expr);

    /* Check for a constant expression */
    if (ED_IsConstAbs (Expr)) {
        /* Constant expression */
        return 0;
    } else {
        /* Not constant, load into the primary */
        LoadExpr (Flags, Expr);
        return 1;
    }
}



void Expression0 (ExprDesc* Expr)
/* Evaluate an expression via hie0 and put the result into the primary register */
{
    ExprWithCheck (hie0, Expr);
    LoadExpr (CF_NONE, Expr);
}



void ConstExpr (void (*Func) (ExprDesc*), ExprDesc* Expr)
/* Will evaluate an expression via the given function. If the result is not
** a constant of some sort, a diagnostic will be printed, and the value is
** replaced by a constant one to make sure there are no internal errors that
** result from this input error.
*/
{
    ExprWithCheck (Func, Expr);
    if (!ED_IsConst (Expr)) {
        Error ("Constant expression expected");
        /* To avoid any compiler errors, make the expression a valid const */
        ED_MakeConstAbsInt (Expr, 1);
    }
}



void BoolExpr (void (*Func) (ExprDesc*), ExprDesc* Expr)
/* Will evaluate an expression via the given function. If the result is not
** something that may be evaluated in a boolean context, a diagnostic will be
** printed, and the value is replaced by a constant one to make sure there
** are no internal errors that result from this input error.
*/
{
    ExprWithCheck (Func, Expr);
    if (!ED_IsBool (Expr)) {
        Error ("Boolean expression expected");
        /* To avoid any compiler errors, make the expression a valid int */
        ED_MakeConstAbsInt (Expr, 1);
    }
}



void ConstAbsIntExpr (void (*Func) (ExprDesc*), ExprDesc* Expr)
/* Will evaluate an expression via the given function. If the result is not
** a constant numeric integer value, a diagnostic will be printed, and the
** value is replaced by a constant one to make sure there are no internal
** errors that result from this input error.
*/
{
    ExprWithCheck (Func, Expr);
    if (!ED_IsConstAbsInt (Expr)) {
        Error ("Constant integer expression expected");
        /* To avoid any compiler errors, make the expression a valid const */
        ED_MakeConstAbsInt (Expr, 1);
    }
}
