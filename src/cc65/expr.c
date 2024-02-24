/* expr.c
**
** 1998-06-21, Ullrich von Bassewitz
** 2020-11-20, Greg King
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
#include "initdata.h"
#include "litpool.h"
#include "loadexpr.h"
#include "macrotab.h"
#include "preproc.h"
#include "scanner.h"
#include "seqpoint.h"
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
/*                           Forward declarations                            */
/*****************************************************************************/



static void parseadd (ExprDesc* Expr, int DoArrayRef);
static void PostInc (ExprDesc* Expr);
static void PostDec (ExprDesc* Expr);



/*****************************************************************************/
/*                             Helper functions                              */
/*****************************************************************************/



unsigned CG_AddrModeFlags (const ExprDesc* Expr)
/* Return the addressing mode flags for the given expression */
{
    switch (ED_GetLoc (Expr)) {
        case E_LOC_NONE:        return CF_IMM;
        case E_LOC_ABS:         return CF_ABSOLUTE;
        case E_LOC_GLOBAL:      return CF_EXTERNAL;
        case E_LOC_STATIC:      return CF_STATIC;
        case E_LOC_REGISTER:    return CF_REGVAR;
        case E_LOC_STACK:       return CF_STACK;
        case E_LOC_PRIMARY:     return CF_PRIMARY;
        case E_LOC_EXPR:        return CF_EXPR;
        case E_LOC_LITERAL:     return CF_LITERAL;
        case E_LOC_CODE:        return CF_CODE;
        default:
            Internal ("CG_AddrModeFlags: Invalid location flags value: 0x%04X", Expr->Flags);
            /* NOTREACHED */
            return 0;
    }
}



static unsigned CG_TypeOfBySize (unsigned Size)
/* Get the code generator replacement type of the object by its size */
{
    unsigned CG_Type;
    /* If the size is less than or equal to that of a a long, we will copy
    ** the struct using the primary register, otherwise we use memcpy.
    */
    switch (Size) {
        case 1:     CG_Type = CF_CHAR;  break;
        case 2:     CG_Type = CF_INT;   break;
        case 3:     /* FALLTHROUGH */
        case 4:     CG_Type = CF_LONG;  break;
        default:    CG_Type = CF_NONE;  break;
    }

    return CG_Type;
}



unsigned CG_TypeOf (const Type* T)
/* Get the code generator base type of the object */
{
    unsigned CG_Type;

    switch (GetUnderlyingTypeCode (T)) {

        case T_SCHAR:
            return CF_CHAR;

        case T_UCHAR:
            return CF_CHAR | CF_UNSIGNED;

        case T_SHORT:
        case T_INT:
            return CF_INT;

        case T_USHORT:
        case T_UINT:
        case T_PTR:
        case T_ARRAY:
            return CF_INT | CF_UNSIGNED;

        case T_LONG:
            return CF_LONG;

        case T_ULONG:
            return CF_LONG | CF_UNSIGNED;

        case T_FLOAT:
        case T_DOUBLE:
            /* These two are identical in the backend */
            return CF_FLOAT;

        case T_FUNC:
            /* Treat this as a function pointer */
            return CF_INT | CF_UNSIGNED;

        case T_STRUCT:
        case T_UNION:
            CG_Type = CG_TypeOfBySize (SizeOf (T));
            if (CG_Type != CF_NONE) {
                return CG_Type;
            }
            /* Address of ... */
            return CF_INT | CF_UNSIGNED;

        case T_VOID:
        case T_ENUM:
            /* Incomplete enum type */
            Error ("Incomplete type '%s'", GetFullTypeName (T));
            return CF_INT;

        default:
            Error ("Illegal type %04lX", T->C);
            return CF_INT;
    }
}



unsigned CG_CallFlags (const Type* T)
/* Get the code generator flags for calling the function */
{
    if (GetUnderlyingTypeCode (T) == T_FUNC) {
        return (T->A.F->Flags & FD_VARIADIC) ? 0 : CF_FIXARGC;
    } else {
        Error ("Illegal function type %04lX", T->C);
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



static unsigned typeadjust (ExprDesc* lhs, const ExprDesc* rhs, int NoPush)
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
    const Type* lhst = lhs->Type;
    const Type* rhst = rhs->Type;

    /* Generate type adjustment code if needed */
    ltype = CG_TypeOf (lhst);
    if (ED_IsConstAbsInt (lhs) && ltype == CF_INT && lhs->IVal >= 0 && lhs->IVal < 256) {
        /* If the lhs is a int constant that fits in an unsigned char, use unsigned char.
        ** g_typeadjust will either promote this to int or unsigned int as appropriate
        ** based on the other operand.  See comment in hie_internal.
        */
        ltype = CF_CHAR | CF_UNSIGNED;
    }
    if (ED_IsLocNone (lhs)) {
        ltype |= CF_CONST;
    }
    if (NoPush) {
        /* Value is in primary register*/
        ltype |= CF_PRIMARY;
    }
    rtype = CG_TypeOf (rhst);
    if (ED_IsConstAbsInt (rhs) && rtype == CF_INT && rhs->IVal >= 0 && rhs->IVal < 256) {
        rtype = CF_CHAR | CF_UNSIGNED;
    }
    if (ED_IsLocNone (rhs)) {
        rtype |= CF_CONST;
    }
    flags = g_typeadjust (ltype, rtype);

    /* Set the type of the result */
    lhs->Type = ArithmeticConvert (lhst, rhst);

    /* Return the code generator flags */
    return flags;
}



void LimitExprValue (ExprDesc* Expr, int WarnOverflow)
/* Limit the constant value of the expression to the range of its type */
{
    switch (GetUnderlyingTypeCode (Expr->Type)) {
        case T_INT:
        case T_SHORT:
            if (WarnOverflow && ((Expr->IVal < -0x8000) || (Expr->IVal > 0x7FFF))) {
                Warning ("Signed integer constant overflow");
            }
            Expr->IVal = (int16_t)Expr->IVal;
            break;

        case T_UINT:
        case T_USHORT:
        case T_PTR:
        case T_ARRAY:
            Expr->IVal = (uint16_t)Expr->IVal;
            break;

        case T_LONG:
            Expr->IVal = (int32_t)Expr->IVal;
            break;

        case T_ULONG:
            Expr->IVal = (uint32_t)Expr->IVal;
            break;

        case T_SCHAR:
            if (WarnOverflow && ((Expr->IVal < -0x80) || (Expr->IVal > 0x7F))) {
                Warning ("Signed character constant overflow");
            }
            Expr->IVal = (int8_t)Expr->IVal;
            break;

        case T_UCHAR:
            Expr->IVal = (uint8_t)Expr->IVal;
            break;

        default:
            Internal ("hie_internal: constant result type %s\n", GetFullTypeName (Expr->Type));
    }
}



static const GenDesc* FindGen (token_t Tok, const GenDesc* Table)
/* Find a token in a generator table */
{
    while (Table->Tok != TOK_INVALID) {
        if ((token_t)Table->Tok == Tok) {
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



static unsigned ExprCheckedSizeOf (const Type* T)
/* Specially checked SizeOf() used in 'sizeof' expressions */
{
    unsigned Size = SizeOf (T);

    if (Size == 0) {
        SymEntry* TagSym = GetESUTagSym (T);
        if (TagSym == 0 || !SymIsDef (TagSym)) {
            Error ("Cannot apply 'sizeof' to incomplete type '%s'", GetFullTypeName (T));
        }
    }
    return Size;
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



static void WarnConstCompareResult (const ExprDesc* Expr)
/* If the result of a comparison is constant, this is suspicious */
{
    if (!ED_NeedsConst (Expr) && IS_Get (&WarnConstComparison) != 0) {
        Warning ("Result of comparison is always %s", Expr->IVal != 0 ? "true" : "false");
    }
}



/*****************************************************************************/
/*                                   code                                    */
/*****************************************************************************/



typedef enum {
    DOT_INC,
    DOT_DEC,
} DeferredOpType;


typedef struct {
    ExprDesc        Expr;
    DeferredOpType  OpType;
} DeferredOp;

Collection DeferredOps;



void InitDeferredOps (void)
/* Init the collection for storing deferred ops */
{
    InitCollection (&DeferredOps);
}



void DoneDeferredOps (void)
/* Deinit the collection for storing deferred ops */
{
    DoneCollection (&DeferredOps);
}



static void DeferInc (const ExprDesc* Expr)
/* Defer the post-inc and put it in a queue */
{
    if (ED_IsUneval (Expr)) {
        return;
    }
    DeferredOp* Op = xmalloc (sizeof (DeferredOp));
    memcpy (&Op->Expr, Expr, sizeof (ExprDesc));
    Op->OpType = DOT_INC;
    CollAppend (&DeferredOps, Op);
}



static void DeferDec (const ExprDesc* Expr)
/* Defer the post-dec and put it in a queue */
{
    if (ED_IsUneval (Expr)) {
        return;
    }
    DeferredOp* Op = xmalloc (sizeof (DeferredOp));
    memcpy (&Op->Expr, Expr, sizeof (ExprDesc));
    Op->OpType = DOT_DEC;
    CollAppend (&DeferredOps, Op);
}



static void DoInc (ExprDesc* Expr, unsigned KeepResult)
/* Do increment */
{
    unsigned Flags;
    long Val;

    /* Get the increment value in bytes */
    Val = IsTypePtr (Expr->Type) ? CheckedSizeOf (Expr->Type + 1) : 1;

    /* Special treatment is needed for bit-fields */
    if (IsTypeFragBitField (Expr->Type)) {
        DoIncDecBitField (Expr, Val, KeepResult);
        return;
    }

    /* Get the flags */
    Flags = CG_TypeOf (Expr->Type) | CG_AddrModeFlags (Expr) | CF_FORCECHAR | CF_CONST;
    if (KeepResult != OA_NEED_NEW) {
        /* No need to get the result */
        Flags |= CF_NOKEEP;
    }

    if (KeepResult == OA_NEED_OLD) {

        Flags |= CF_FORCECHAR;

        /* Push the address if needed */
        PushAddr (Expr);

        /* Save the original value */
        LoadExpr (CF_NONE, Expr);
        g_save (Flags);

        /* Do the increment */
        g_inc (Flags | CF_CONST, Val);

        /* Store the result back */
        Store (Expr, 0);

        /* Restore the original value */
        g_restore (Flags);

    } else {

        /* Check the location of the data */
        switch (ED_GetLoc (Expr)) {

            case E_LOC_ABS:
                /* Absolute numeric addressed variable */
                g_addeqstatic (Flags, Expr->IVal, 0, Val);
                break;

            case E_LOC_GLOBAL:
            case E_LOC_STATIC:
            case E_LOC_REGISTER:
            case E_LOC_LITERAL:
            case E_LOC_CODE:
                /* Global variabl, static variable, register variable, pooled
                ** literal or code label location.
                */
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
                /* An expression referenced in the primary register */
                g_addeqind (Flags, Expr->IVal, Val);
                break;

            default:
                Internal ("Invalid location in DoInc(): 0x%04X", ED_GetLoc (Expr));
        }

    }
}



static void DoDec (ExprDesc* Expr, unsigned KeepResult)
/* Do decrement */
{
    unsigned Flags;
    long Val;

    /* Get the decrement value in bytes */
    Val = IsTypePtr (Expr->Type) ? CheckedSizeOf (Expr->Type + 1) : 1;

    /* Special treatment is needed for bit-fields */
    if (IsTypeFragBitField (Expr->Type)) {
        DoIncDecBitField (Expr, -Val, KeepResult);
        return;
    }

    /* Get the flags */
    Flags = CG_TypeOf (Expr->Type) | CG_AddrModeFlags (Expr) | CF_FORCECHAR | CF_CONST;
    if (KeepResult != OA_NEED_NEW) {
        /* No need to get the result */
        Flags |= CF_NOKEEP;
    }

    if (KeepResult == OA_NEED_OLD) {

        Flags |= CF_FORCECHAR;

        /* Push the address if needed */
        PushAddr (Expr);

        /* Save the original value */
        LoadExpr (CF_NONE, Expr);
        g_save (Flags);

        /* Do the decrement */
        g_dec (Flags | CF_CONST, Val);

        /* Store the result back */
        Store (Expr, 0);

        /* Restore the original value */
        g_restore (Flags);

    } else {

        /* Check the location of the data */
        switch (ED_GetLoc (Expr)) {

            case E_LOC_ABS:
                /* Absolute numeric addressed variable */
                g_subeqstatic (Flags, Expr->IVal, 0, Val);
                break;

            case E_LOC_GLOBAL:
            case E_LOC_STATIC:
            case E_LOC_REGISTER:
            case E_LOC_LITERAL:
            case E_LOC_CODE:
                /* Global variabl, static variable, register variable, pooled
                ** literal or code label location.
                */
                g_subeqstatic (Flags, Expr->Name, Expr->IVal, Val);
                break;

            case E_LOC_STACK:
                /* Value on the stack */
                g_subeqlocal (Flags, Expr->IVal, Val);
                break;

            case E_LOC_PRIMARY:
                /* The primary register */
                g_dec (Flags, Val);
                break;

            case E_LOC_EXPR:
                /* An expression referenced in the primary register */
                g_subeqind (Flags, Expr->IVal, Val);
                break;

            default:
                Internal ("Invalid location in DoDec(): 0x%04X", ED_GetLoc (Expr));
        }

    }
}



int GetDeferredOpCount (void)
/* Return how many deferred operations are still waiting in the queque */
{
    return (int)CollCount (&DeferredOps);
}



void CheckDeferredOpAllDone (void)
/* Check if all deferred operations are done at sequence points.
** Die off if check fails.
*/
{
    if (GetDeferredOpCount () > 0) {
        Internal ("Code generation messed up: missing operations past sequence points.");
    }
}



void DoDeferred (unsigned Flags, ExprDesc* Expr)
/* Do deferred operations such as post-inc/dec at sequence points */
{
    int         I;
    unsigned    Size = 0;
    int         Count = GetDeferredOpCount ();
    unsigned    StmtFlags = GetSQPFlags ();

    /* Nothing to be done */
    if (Count <= 0) {
        return;
    }

    /* Backup some regs/processor flags around the inc/dec */
    if ((StmtFlags & SQP_KEEP_TEST) != 0 ||
        ((Flags & SQP_KEEP_TEST) != 0 && ED_NeedsTest (Expr))) {
        /* Sufficient to add a pair of PHP/PLP for all cases */
        AddCodeLine ("php");
    }

    if ((Flags & SQP_MASK_EAX) != 0 && ED_NeedsPrimary (Expr)) {
        Size = SizeOf (Expr->Type);
    }

    /* Get the size of the backup */
    if ((StmtFlags & SQP_MASK_EAX) != 0) {
        switch (StmtFlags & SQP_MASK_EAX) {
            case SQP_KEEP_A:    if (Size < 1) Size = 1; break;
            case SQP_KEEP_AX:   if (Size < 2) Size = 2; break;
            case SQP_KEEP_EAX:  if (Size < 4) Size = 4; break;
            default:            ;
        }
    }

    /* Backup the content of EAX around the inc/dec */
    if (Size == 1) {
        AddCodeLine ("pha");
    } else if (Size == 2) {
        AddCodeLine ("sta regsave");
        AddCodeLine ("stx regsave+1");
    } else if (Size == 3 || Size == 4) {
        AddCodeLine("jsr saveeax");
    } else if (Size > 4) {
        Error ("Unsupported deferred operand size: %u", Size);
    }

    for (I = 0; I < Count; ++I) {
        DeferredOp* Op = CollAtUnchecked (&DeferredOps, I);
        switch (Op->OpType) {

            case DOT_INC:
                DoInc (&Op->Expr, OA_NEED_NONE);
                break;

            case DOT_DEC:
                DoDec (&Op->Expr, OA_NEED_NONE);
                break;
        }
        xfree (&Op->Expr);
    }
    CollDeleteAll (&DeferredOps);

    /* Restore the content of EAX around the inc/dec */
    if (Size == 1) {
        AddCodeLine ("pla");
    } else if (Size == 2) {
        AddCodeLine ("lda regsave");
        AddCodeLine ("ldx regsave+1");
    } else if (Size == 3 || Size == 4) {
        AddCodeLine ("jsr resteax");
    }

    /* Restore the regs/processor flags around the inc/dec */
    if ((StmtFlags & SQP_KEEP_TEST) != 0 ||
        ((Flags & SQP_KEEP_TEST) != 0 && ED_NeedsTest (Expr))) {
        /* Sufficient to pop the processor flags */
        AddCodeLine ("plp");
    }

    /* Expression has had side effects */
    Expr->Flags |= E_SIDE_EFFECTS;
}



static unsigned FunctionArgList (FuncDesc* Func, int IsFastcall, ExprDesc* ED)
/* Parse the argument list of the called function and pass the arguments to it.
** Depending on several criteria, this may be done by just pushing into each
** parameter separately, or creating the parameter frame once and then storing
** arguments into this frame one by one.
** The function returns the size of the arguments pushed in bytes.
*/
{
    ExprDesc  Expr;

    /* Initialize variables */
    SymEntry* Param       = 0;  /* Keep gcc silent */
    unsigned  PushedSize  = 0;  /* Size of arguments pushed */
    unsigned  PushedCount = 0;  /* Number of arguments pushed */
    unsigned  FrameSize   = 0;  /* Size of parameter frame */
    unsigned  FrameParams = 0;  /* Number of parameters in frame */
    int       FrameOffs   = 0;  /* Offset into parameter frame */
    int       Ellipsis    = 0;  /* Function is variadic */

    /* Make sure the size of all parameters are known */
    int ParamComplete = F_CheckParamList (Func, 1);

    /* As an optimization, we may allocate the complete parameter frame at
    ** once instead of pushing into each parameter as it comes. We may do that,
    ** if...
    **
    **  - optimizations that increase code size are enabled (allocating the
    **    stack frame at once gives usually larger code).
    **  - we have more than one parameter to push into (don't count the last
    **    parameter for __fastcall__ functions).
    **
    ** The FrameSize variable will contain a value > 0 if storing into a frame
    ** (instead of pushing) is enabled.
    **
    */
    if (ParamComplete && IS_Get (&CodeSizeFactor) >= 200) {
        /* Calculate the number and size of the parameters */
        FrameParams = Func->ParamCount;
        FrameSize   = Func->ParamSize;
        if (FrameParams > 0 && IsFastcall) {
            /* Last parameter is not pushed into */
            FrameSize -= CheckedSizeOf (Func->LastParam->Type);
            --FrameParams;
        }

        /* Do we have more than one parameter in the frame? */
        if (FrameParams > 1) {
            /* Okeydokey, set up the frame */
            FrameOffs = StackPtr;
            g_space (FrameSize);
            StackPtr -= FrameSize;
        } else {
            /* Don't use a preallocated frame */
            FrameSize = 0;
        }
    }

    /* Parse the actual argument list */
    while (CurTok.Tok != TOK_RPAREN) {
        unsigned Flags;     /* Code generator flags, not expression flags */

        ED_Init (&Expr);

        /* This way, the info of the last parameter won't be cleared */
        Expr.Flags |= ED->Flags & E_MASK_KEEP_SUBEXPR;

        /* Count arguments */
        ++PushedCount;

        /* Fetch the pointer to the next argument, check for too many args */
        if (PushedCount <= Func->ParamCount) {
            /* Beware: If there are parameters with identical names, they
            ** cannot go into the same symbol table, which means that, in this
            ** case of errorneous input, the number of nodes in the symbol
            ** table and PushedCount are NOT equal. We have to handle this case
            ** below to avoid segmentation violations. Since we know that this
            ** problem can only occur if there is more than one parameter,
            ** we will just use the last one.
            */
            if (PushedCount == 1) {
                /* First argument */
                Param = Func->SymTab->SymHead;
            } else if (Param->NextSym != 0) {
                /* Next argument */
                Param = Param->NextSym;
                CHECK ((Param->Flags & SC_PARAM) != 0);
            }
        } else if (!Ellipsis) {
            /* Too many arguments. Do we have an open or empty param. list? */
            if ((Func->Flags & (FD_VARIADIC | FD_EMPTY)) == 0) {
                /* End of param list reached, no ellipsis */
                Error ("Too many arguments in function call");
            }
            /* Assume an ellipsis even in case of errors, to avoid an error
            ** message for each other argument.
            */
            Ellipsis = 1;
        }

        /* Evaluate the argument expression */
        hie1 (&Expr);

        /* Skip to the next parameter if there are any incomplete types */
        if (ParamComplete) {
            /* If we don't have an argument spec., accept anything; otherwise,
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
                ** element", function to "pointer to function" and do integral
                ** promotion if necessary.
                */
                TypeConversion (&Expr, StdConversion (Expr.Type));

            }

            /* Handle struct/union specially */
            if (IsClassStruct (Expr.Type)) {
                /* Use the replacement type */
                Flags |= CG_TypeOf (GetStructReplacementType (Expr.Type));

                /* Load the value into the primary if it is not already there */
                LoadExpr (Flags, &Expr);
            } else {
                /* Load the value into the primary if it is not already there */
                LoadExpr (CF_NONE, &Expr);

                /* Use the type of the argument for the push */
                Flags |= CG_TypeOf (Expr.Type);
            }

            /* If this is a fastcall function, don't push the last argument */
            if ((CurTok.Tok == TOK_COMMA && NextTok.Tok != TOK_RPAREN) || !IsFastcall) {
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
                PushedSize += ArgSize;
            }
        }

        /* Propagate viral flags */
        ED_PropagateFrom (ED, &Expr);

        /* Check for end of argument list */
        if (CurTok.Tok != TOK_COMMA) {
            break;
        }
        NextToken ();

        /* Check for stray comma */
        if (CurTok.Tok == TOK_RPAREN) {
            Error ("Argument expected after comma");
            break;
        }

        DoDeferred (SQP_KEEP_NONE, &Expr);
    }

    /* Append last deferred inc/dec before the function is called.
    ** The last parameter needs to be preserved if it is passed in AX/EAX Regs.
    */
    DoDeferred (IsFastcall && PushedCount > 0 ? SQP_KEEP_EAX : SQP_KEEP_NONE, &Expr);

    /* Check if we had enough arguments */
    if (PushedCount < Func->ParamCount) {
        Error ("Too few arguments in function call");
    }

    /* The function returns the size of all arguments pushed onto the stack.
    ** However, if there are parameters missed (which is an error, and was
    ** flagged by the compiler), AND a stack frame was preallocated above,
    ** we would lose track of the stackpointer, and generate an internal error
    ** later. So we correct the value by the parameters that should have been
    ** pushed into, to avoid an internal compiler error. Since an error was
    ** generated before, no code will be output anyway.
    */
    return PushedSize + FrameSize;
}



static void FunctionCall (ExprDesc* Expr)
/* Perform a function call. */
{
    FuncDesc*     Func;           /* Function descriptor */
    int           IsFuncPtr;      /* Flag */
    unsigned      ArgSize;        /* Number of arguments bytes */
    CodeMark      Mark;
    int           PtrOffs = 0;    /* Offset of function pointer on stack */
    int           IsFastcall = 0; /* True if we are fast-calling the function */
    int           PtrOnStack = 0; /* True if a pointer copy is on stack */
    const Type*   ReturnType;

    /* Skip the left paren */
    NextToken ();

    /* Get a pointer to the function descriptor from the type string */
    Func = GetFuncDesc (Expr->Type);

    /* Handle function pointers transparently */
    IsFuncPtr = IsTypeFuncPtr (Expr->Type);
    if (IsFuncPtr) {
        /* Check whether it's a fastcall function that has parameters.
        ** Note: if a function is forward-declared in the old K & R style, then
        ** it may be called with any number of arguments, even though its
        ** parameter count is zero.  Handle K & R functions as though there are
        ** parameters.
        */
        IsFastcall = (Func->ParamCount > 0 || (Func->Flags & FD_EMPTY) != 0) &&
                     IsFastcallFunc (Expr->Type + 1);

        /* Things may be difficult, depending on where the function pointer
        ** resides. If the function pointer is an expression of some sort
        ** (not a local or global variable), we have to evaluate this
        ** expression now and save the result for later. Since calls to
        ** function pointers may be nested, we must save it onto the stack.
        ** For fastcall functions we do also need to place a copy of the
        ** pointer on stack, since we cannot use a/x.
        */
        PtrOnStack = IsFastcall || !ED_IsConstAddr (Expr);
        if (PtrOnStack) {

            /* Not a global or local variable, or a fastcall function. Load
            ** the pointer into the primary and mark it as an expression.
            */
            LoadExpr (CF_NONE, Expr);
            ED_FinalizeRValLoad (Expr);

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
        if (Expr->Name != 0 && !ED_IsUneval (Expr)) {
            int StdFunc = FindStdFunc ((const char*) Expr->Name);
            if (StdFunc >= 0) {
                /* Inline this function */
                HandleStdFunc (StdFunc, Func, Expr);
                return;
            }
        }

        /* If we didn't inline the function, get fastcall info */
        IsFastcall = (Func->ParamCount > 0 || (Func->Flags & FD_EMPTY) != 0) &&
                     IsFastcallFunc (Expr->Type);
    }

    /* Parse the argument list and pass them to the called function */
    ArgSize = FunctionArgList (Func, IsFastcall, Expr);

    if (ArgSize > 0xFF && (Func->Flags & FD_VARIADIC) != 0) {
        Error ("Total size of all arguments passed to a variadic function cannot exceed 255 bytes");
    }

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
                /* If we have no arguments, the pointer is still in the
                ** primary. Remove the code to push it and correct the
                ** stack pointer.
                */
                if (ArgSize == 0) {
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
            g_callind (CG_CallFlags (Expr->Type+1), ArgSize, PtrOffs);

        } else {

            /* Fastcall function. We cannot use the primary for the function
            ** pointer and must therefore use an offset to the stack location.
            ** Since fastcall functions may never be variadic, we can use the
            ** index register for this purpose.
            */
            g_callind (CF_STACK, ArgSize, PtrOffs);
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
        if (Expr->Sym && Expr->Sym->V.F.WrappedCall) {
            char tmp[64];
            StrBuf S = AUTO_STRBUF_INITIALIZER;

            if (Expr->Sym->V.F.WrappedCallData == WRAPPED_CALL_USE_BANK) {
                /* Store the bank attribute in tmp4 */
                SB_AppendStr (&S, "ldy #<.bank(_");
                SB_AppendStr (&S, (const char*) Expr->Name);
                SB_AppendChar (&S, ')');
            } else {
                /* Store the WrappedCall data in tmp4 */
                sprintf(tmp, "ldy #%u", Expr->Sym->V.F.WrappedCallData);
                SB_AppendStr (&S, tmp);
            }
            g_asmcode (&S);
            SB_Clear(&S);

            SB_AppendStr (&S, "sty tmp4");
            g_asmcode (&S);
            SB_Clear(&S);

            /* Store the original function address in ptr4 */
            SB_AppendStr (&S, "ldy #<(_");
            SB_AppendStr (&S, (const char*) Expr->Name);
            SB_AppendChar (&S, ')');
            g_asmcode (&S);
            SB_Clear(&S);

            SB_AppendStr (&S, "sty ptr4");
            g_asmcode (&S);
            SB_Clear(&S);

            SB_AppendStr (&S, "ldy #>(_");
            SB_AppendStr (&S, (const char*) Expr->Name);
            SB_AppendChar (&S, ')');
            g_asmcode (&S);
            SB_Clear(&S);

            SB_AppendStr (&S, "sty ptr4+1");
            g_asmcode (&S);
            SB_Clear(&S);

            SB_Done (&S);

            g_call (CG_CallFlags (Expr->Type), Expr->Sym->V.F.WrappedCall->Name, ArgSize);
        } else {
            g_call (CG_CallFlags (Expr->Type), (const char*) Expr->Name, ArgSize);
        }

    }

    /* The function result is an rvalue in the primary register */
    ED_FinalizeRValLoad (Expr);
    ReturnType = GetFuncReturnType (Expr->Type);

    /* Handle struct/union specially */
    if (IsClassStruct (ReturnType)) {
        /* If there is no replacement type, then it is just the address */
        if (ReturnType == GetStructReplacementType (ReturnType)) {
            /* Dereference it */
            ED_IndExpr (Expr);
            ED_MarkExprAsRVal (Expr);
        }
    }

    Expr->Type = ReturnType;

    /* We assume all function calls had side effects */
    Expr->Flags |= E_SIDE_EFFECTS;
}



static void Primary (ExprDesc* E)
/* This is the lowest level of the expression parser. */
{
    SymEntry* Sym;
    unsigned Flags = E->Flags & E_MASK_KEEP_MAKE;

    switch (CurTok.Tok) {

        case TOK_LPAREN:
            /* Process parenthesized subexpression by calling the whole parser
            ** recursively.
            */
            NextToken ();
            hie0 (E);
            ConsumeRParen ();
            break;

        case TOK_BOOL_AND:
            /* A computed goto label address */
            if (IS_Get (&Standard) >= STD_CC65) {
                SymEntry* Entry;
                NextToken ();
                Entry = AddLabelSym (CurTok.Ident, SC_REF | SC_GOTO_IND);
                /* output its label */
                E->Flags = E_RTYPE_RVAL | E_LOC_CODE | E_ADDRESS_OF;
                E->Name = Entry->V.L.Label;
                E->Type = type_void_p;
                NextToken ();
            } else {
                Error ("Computed gotos are a C extension, not supported with this --standard");
                ED_MakeConstAbsInt (E, 1);
            }
            break;

        case TOK_IDENT:
            /* Identifier. Get a pointer to the symbol table entry */
            Sym = E->Sym = FindSym (CurTok.Ident);

            /* Is the symbol known? */
            if (Sym) {

                /* We found the symbol - skip the name token */
                NextToken ();

                /* Check for illegal symbol types */
                CHECK ((Sym->Flags & SC_TYPEMASK) != SC_LABEL);
                if ((Sym->Flags & SC_TYPEMASK) == SC_TYPEDEF) {
                    /* Cannot use type symbols */
                    Error ("Variable identifier expected");
                    /* Assume an int type to make E valid */
                    E->Flags = E_LOC_STACK | E_RTYPE_LVAL;
                    E->Type  = type_int;
                    break;
                }

                /* Mark the symbol as referenced */
                Sym->Flags |= SC_REF;

                /* The expression type is the symbol type */
                E->Type = Sym->Type;

                /* Check for legal symbol types */
                if ((Sym->Flags & SC_CONST) == SC_CONST) {
                    /* Enum or some other numeric constant */
                    E->Flags = E_LOC_NONE | E_RTYPE_RVAL;
                    E->IVal  = Sym->V.ConstVal;
                } else if ((Sym->Flags & SC_STORAGEMASK) == SC_AUTO) {
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
                } else if ((Sym->Flags & SC_TYPEMASK) == SC_FUNC) {
                    /* Function */
                    E->Flags = E_LOC_GLOBAL | E_RTYPE_LVAL;
                    E->Name  = (uintptr_t) Sym->Name;
                } else if ((Sym->Flags & SC_STORAGEMASK) == SC_REGISTER) {
                    /* Register variable, zero page based */
                    E->Flags = E_LOC_REGISTER | E_RTYPE_LVAL;
                    E->Name  = Sym->V.R.RegOffs;
                } else if (SymIsGlobal (Sym)) {
                    /* Global variable */
                    E->Flags = E_LOC_GLOBAL | E_RTYPE_LVAL;
                    E->Name  = (uintptr_t) Sym->Name;
                } else if ((Sym->Flags & SC_STORAGEMASK) == SC_STATIC) {
                    /* Local static variable */
                    E->Flags = E_LOC_STATIC | E_RTYPE_LVAL;
                    E->Name  = Sym->V.L.Label;
                } else {
                    /* Other */
                    E->Flags = E_LOC_STATIC | E_RTYPE_LVAL;
                    E->Name  = Sym->V.Offs;
                }

                /* We've made all variables lvalues above. However, this is
                ** not always correct: An array is actually the address of its
                ** first element, which is an rvalue, and a function is an
                ** rvalue, too, because we cannot store anything in a function.
                ** So fix the flags depending on the type.
                */
                if (IsTypeArray (E->Type) || IsTypeFunc (E->Type)) {
                    ED_AddrExpr (E);
                }

            } else {

                /* We did not find the symbol. Remember the name, then skip it */
                ident Ident;
                strcpy (Ident, CurTok.Ident);
                NextToken ();

                /* IDENT is either an auto-declared function or an undefined variable. */
                if (CurTok.Tok == TOK_LPAREN) {
                    /* C99 doesn't allow calls to undeclared functions, so
                    ** generate an error and otherwise a warning. Declare a
                    ** function returning int. For that purpose, prepare a
                    ** function signature for a function having an empty param
                    ** list and returning int.
                    */
                    if (IS_Get (&Standard) >= STD_C99) {
                        Error ("Call to undeclared function '%s'", Ident);
                    } else {
                        Warning ("Call to undeclared function '%s'", Ident);
                    }
                    Sym = AddGlobalSym (Ident, GetImplicitFuncType(), SC_REF | SC_FUNC);
                    E->Type  = Sym->Type;
                    E->Flags = E_LOC_GLOBAL | E_RTYPE_RVAL;
                    E->Name  = (uintptr_t) Sym->Name;
                } else {
                    /* Undeclared Variable */
                    Error ("Undeclared identifier '%s'", Ident);
                    Sym = AddLocalSym (Ident, type_int, SC_AUTO | SC_REF, 0);
                    E->Flags = E_LOC_STACK | E_RTYPE_LVAL;
                    E->Type  = type_int;
                }

                E->Sym = Sym;
            }
            break;

        case TOK_SCONST:
        case TOK_WCSCONST:
            /* String literal */
            if ((Flags & E_EVAL_UNEVAL) != E_EVAL_UNEVAL) {
                E->V.LVal = UseLiteral (CurTok.SVal);
            } else {
                E->V.LVal = CurTok.SVal;
            }
            E->Type  = GetCharArrayType (GetLiteralSize (CurTok.SVal));
            E->Flags = E_LOC_LITERAL | E_RTYPE_RVAL | E_ADDRESS_OF;
            E->IVal  = 0;
            E->Name  = GetLiteralLabel (CurTok.SVal);
            NextToken ();
            break;

        case TOK_ICONST:
        case TOK_CCONST:
        case TOK_WCCONST:
            /* Character and integer constants */
            E->IVal  = CurTok.IVal;
            E->Flags = E_LOC_NONE | E_RTYPE_RVAL;
            E->Type  = CurTok.Type;
            NextToken ();
            break;

        case TOK_FCONST:
            /* Floating point constant */
            E->V.FVal = CurTok.FVal;
            E->Flags  = E_LOC_NONE | E_RTYPE_RVAL;
            E->Type   = CurTok.Type;
            NextToken ();
            break;

        case TOK_ASM:
            /* ASM statement */
            AsmStatement ();
            E->Flags = E_RTYPE_RVAL | E_EVAL_MAYBE_UNUSED | E_SIDE_EFFECTS;
            E->Type  = type_void;
            break;

        case TOK_A:
            /* Register pseudo variable */
            SetSQPFlags (SQP_KEEP_A);
            E->Type  = type_uchar;
            E->Flags = E_LOC_PRIMARY | E_RTYPE_LVAL;
            NextToken ();
            break;

        case TOK_AX:
            /* Register pseudo variable */
            SetSQPFlags (SQP_KEEP_AX);
            E->Type  = type_uint;
            E->Flags = E_LOC_PRIMARY | E_RTYPE_LVAL;
            NextToken ();
            break;

        case TOK_EAX:
            /* Register pseudo variable */
            SetSQPFlags (SQP_KEEP_EAX);
            E->Type  = type_ulong;
            E->Flags = E_LOC_PRIMARY | E_RTYPE_LVAL;
            NextToken ();
            break;

        default:
            /* Illegal primary. Be sure to skip the token to avoid endless
            ** error loops.
            */
            if (CurTok.Tok == TOK_LCURLY) {
                /* Statement block */
                NextToken ();
                Error ("Expression expected");
                E->Flags |= E_EVAL_MAYBE_UNUSED;
                hie0 (E);
                if (CurTok.Tok == TOK_RCURLY) {
                    NextToken ();
                }
                break;
            } else {
                /* Let's see if this is a C99-style declaration */
                DeclSpec Spec;
                ParseDeclSpec (&Spec, TS_DEFAULT_TYPE_NONE | TS_FUNCTION_SPEC, SC_AUTO);

                if ((Spec.Flags & DS_TYPE_MASK) != DS_NONE) {
                    Error ("Mixed declarations and code are not supported in cc65");
                    SmartErrorSkip (0);
                } else {
                    Error ("Expression expected");
                    E->Flags |= E_EVAL_MAYBE_UNUSED;
                    NextToken ();
                }
            }
            ED_MakeConstAbsInt (E, 1);
            break;
    }

    E->Flags |= Flags;
}



static void StructRef (ExprDesc* Expr)
/* Process struct/union field after . or ->. */
{
    Type* FinalType;
    TypeCode Q;

    /* Skip the token and check for an identifier */
    NextToken ();
    if (CurTok.Tok != TOK_IDENT) {
        Error ("Identifier expected for %s member", GetBasicTypeName (Expr->Type));
        /* Make the expression an integer at address zero */
        ED_MakeConstAbs (Expr, 0, type_int);
        return;
    }

    /* Get the symbol table entry and check for a struct/union field */
    NextToken ();
    const SymEntry Field = FindStructField (Expr->Type, CurTok.Ident);
    if (Field.Type == 0) {
        Error ("No field named '%s' found in '%s'", CurTok.Ident, GetFullTypeName (Expr->Type));
        /* Make the expression an integer at address zero */
        ED_MakeConstAbs (Expr, 0, type_int);
        return;
    }

    if (IsTypePtr (Expr->Type)) {

        /* pointer->field */
        if (!ED_IsQuasiConst (Expr) && !ED_IsLocPrimary (Expr)) {
            /* If we have a non-const struct/union pointer that is not in the
            ** primary yet, load its content now to get the base address.
            */
            LoadExpr (CF_NONE, Expr);
            ED_FinalizeRValLoad (Expr);
        }
        /* Dereference the address expression */
        ED_IndExpr (Expr);

    } else if (ED_IsRVal (Expr)       &&
               ED_IsLocPrimary (Expr) &&
               Expr->Type == GetStructReplacementType (Expr->Type)) {

        /* A struct/union is usually an lvalue. If not, it is a struct/union
        ** passed in the primary register, which is usually the result returned
        ** from a function. However, it is possible that this rvalue is the
        ** result of certain kind of operations on an lvalue such as assignment,
        ** and there are no reasons to disallow such use cases. So we just rely
        ** on the check upon function returns to catch the unsupported cases and
        ** dereference the rvalue address of the struct/union here all the time.
        */
        ED_IndExpr (Expr);

    } else if (!ED_IsLocQuasiConst (Expr) && !ED_IsLocPrimaryOrExpr (Expr)) {
        /* Load the base address into the primary (and use it as a reference
        ** later) if it's not quasi-const or in the primary already.
        */
        LoadExpr (CF_NONE, Expr);
    }

    /* Clear the tested flag set during loading */
    ED_MarkAsUntested (Expr);

    /* The type is the field type plus any qualifiers from the struct/union */
    if (IsClassStruct (Expr->Type)) {
        Q = GetQualifier (Expr->Type);
    } else {
        Q = GetQualifier (Indirect (Expr->Type));
    }
    if (GetQualifier (Field.Type) == (GetQualifier (Field.Type) | Q)) {
        FinalType = Field.Type;
    } else {
        FinalType = TypeDup (Field.Type);
        FinalType->C |= Q;
    }

    if (ED_IsRVal (Expr) && ED_IsLocPrimary (Expr) && !IsTypePtr (Expr->Type)) {

        unsigned Flags = 0;
        unsigned BitOffs;

        /* Get the size of the type */
        unsigned StructSize = SizeOf (Expr->Type);
        unsigned FieldSize  = SizeOf (Field.Type);

        /* Safety check */
        CHECK (Field.V.Offs + FieldSize <= StructSize);

        /* The type of the operation depends on the type of the struct/union */
        switch (StructSize) {
            case 1:
                Flags = CF_CHAR | CF_UNSIGNED | CF_CONST;
                break;
            case 2:
                Flags = CF_INT  | CF_UNSIGNED | CF_CONST;
                break;
            case 3:
                /* FALLTHROUGH */
            case 4:
                Flags = CF_LONG | CF_UNSIGNED | CF_CONST;
                break;
            default:
                Internal ("Invalid '%s' size: %u", GetFullTypeName (Expr->Type), StructSize);
                break;
        }

        /* Generate a shift to get the field in the proper position in the
        ** primary. For bit fields, mask the value.
        */
        BitOffs = Field.V.Offs * CHAR_BITS;
        if (SymIsBitField (&Field)) {
            BitOffs += Field.Type->A.B.Offs;
            g_asr (Flags, BitOffs);
            /* Mask the value. This is unnecessary if the shift executed above
            ** moved only zeroes into the value.
            */
            if (BitOffs + Field.Type->A.B.Width != FieldSize * CHAR_BITS) {
                g_and (CF_INT | CF_UNSIGNED | CF_CONST,
                       (0x0001U << Field.Type->A.B.Width) - 1U);
            }
        } else {
            g_asr (Flags, BitOffs);
        }

        /* Use the new type */
        Expr->Type = FinalType;

    } else {

        /* Set the struct/union field offset */
        Expr->IVal += Field.V.Offs;

        /* Use the new type */
        Expr->Type = FinalType;

        /* The usual rules for variables with respect to the reference types
        ** apply to struct/union fields as well: If a field is an array, it is
        ** virtually an rvalue address, otherwise it's an lvalue reference. (A
        ** function would also be an rvalue address, but a struct/union cannot
        ** contain functions).
        */
        if (IsTypeArray (Expr->Type)) {
            ED_AddrExpr (Expr);
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
    while (CurTok.Tok == TOK_INC    || CurTok.Tok == TOK_DEC    ||
           CurTok.Tok == TOK_LBRACK || CurTok.Tok == TOK_LPAREN ||
           CurTok.Tok == TOK_DOT    || CurTok.Tok == TOK_PTR_REF) {

        switch (CurTok.Tok) {

            case TOK_LBRACK:
                /* Array reference */
                parseadd (Expr, 1);
                break;

            case TOK_LPAREN:
                /* Function call */
                if (!IsTypeFuncLike (Expr->Type)) {
                    /* Not a function or function pointer */
                    Error ("Illegal function call");
                    /* Force the type to be a implicitly defined function, one
                    ** returning an int and taking any number of arguments.
                    ** Since we don't have a name, invent one.
                    */
                    ED_MakeConstAbs (Expr, 0, GetImplicitFuncType ());
                    Expr->Name = (uintptr_t) IllegalFunc;
                }
                /* Call the function */
                FunctionCall (Expr);
                break;

            case TOK_DOT:
                if (!IsClassStruct (Expr->Type)) {
                    Error ("Struct or union expected");
                }
                StructRef (Expr);
                break;

            case TOK_PTR_REF:
                /* If we have an array, convert it to pointer to first element */
                if (IsTypeArray (Expr->Type)) {
                    Expr->Type = ArrayToPtr (Expr->Type);
                }
                if (!IsClassPtr (Expr->Type) || !IsClassStruct (Indirect (Expr->Type))) {
                    Error ("Struct pointer or union pointer expected");
                }
                StructRef (Expr);
                break;

            case TOK_INC:
                PostInc (Expr);
                break;

            case TOK_DEC:
                PostDec (Expr);
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
    Flags = CG_TypeOf (StoreType) | CG_AddrModeFlags (Expr);

    /* Do the store depending on the location */
    switch (ED_GetLoc (Expr)) {

        case E_LOC_ABS:
            /* Absolute numeric addressed variable */
            g_putstatic (Flags, Expr->IVal, 0);
            break;

        case E_LOC_GLOBAL:
        case E_LOC_STATIC:
        case E_LOC_REGISTER:
        case E_LOC_LITERAL:
        case E_LOC_CODE:
            /* Global variabl, static variable, register variable, pooled
            ** literal or code label location.
            */
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
            /* An expression referenced in the primary register */
            g_putind (Flags, Expr->IVal);
            break;

        case E_LOC_NONE:
            /* We may get here as a result of previous compiler errors */
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

    /* Do the increment */
    DoInc (Expr, OA_NEED_NEW);

    /* Result is an expression, no reference */
    ED_FinalizeRValLoad (Expr);

    /* Expression has had side effects */
    Expr->Flags |= E_SIDE_EFFECTS;
}



static void PreDec (ExprDesc* Expr)
/* Handle the predecrement operators */
{
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

    /* Do the decrement */
    DoDec (Expr, OA_NEED_NEW);

    /* Result is an expression, no reference */
    ED_FinalizeRValLoad (Expr);

    /* Expression has had side effects */
    Expr->Flags |= E_SIDE_EFFECTS;
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
    Flags = CG_TypeOf (Expr->Type);

    /* We are allowed by the C standard to defer the inc operation until after
    ** the expression is used, so that we don't need to save and reload
    ** the original value.
    */

    /* Emit smaller code if a char variable is at a constant location */
    if ((Flags & CF_TYPEMASK) == CF_CHAR && ED_IsLocConst (Expr) && !IsTypeBitField (Expr->Type)) {

        LoadExpr (CF_NONE, Expr);
        AddCodeLine ("inc %s", ED_GetLabelName (Expr, 0));

        /* Expression has had side effects */
        Expr->Flags |= E_SIDE_EFFECTS;

    } else {

        if (ED_IsLocPrimaryOrExpr (Expr)) {

            /* Do the increment */
            DoInc (Expr, OA_NEED_OLD);

            /* Expression has had side effects */
            Expr->Flags |= E_SIDE_EFFECTS;

        } else {

            /* Defer the increment until after the value of this expression is used */
            DeferInc (Expr);

            /* Just return */
            return;
        }
    }

    /* The result is always an expression, no reference */
    ED_FinalizeRValLoad (Expr);
}



static void PostDec (ExprDesc* Expr)
/* Handle the postdecrement operator */
{
    unsigned Flags;

    NextToken ();

    /* The expression to decrement must be an lvalue */
    if (!ED_IsLVal (Expr)) {
        Error ("Invalid lvalue");
        return;
    }

    /* We cannot modify const values */
    if (IsQualConst (Expr->Type)) {
        Error ("Decrement of read-only variable");
    }

    /* Get the data type */
    Flags = CG_TypeOf (Expr->Type);

    /* Emit smaller code if a char variable is at a constant location */
    if ((Flags & CF_TYPEMASK) == CF_CHAR && ED_IsLocConst (Expr) && !IsTypeBitField (Expr->Type)) {

        LoadExpr (CF_NONE, Expr);
        AddCodeLine ("dec %s", ED_GetLabelName (Expr, 0));

        /* Expression has had side effects */
        Expr->Flags |= E_SIDE_EFFECTS;

    } else {

        if (ED_IsLocPrimaryOrExpr (Expr)) {

            /* Do the decrement */
            DoDec (Expr, OA_NEED_OLD);

            /* Expression has had side effects */
            Expr->Flags |= E_SIDE_EFFECTS;

        } else {

            /* Defer the decrement until after the value of this expression is used */
            DeferDec (Expr);

            /* Just return */
            return;
        }
    }

    /* The result is always an expression, no reference */
    ED_FinalizeRValLoad (Expr);
}



static void UnaryOp (ExprDesc* Expr)
/* Handle unary -/+ and ~ */
{
    /* Remember the operator token and skip it */
    token_t Tok = CurTok.Tok;
    NextToken ();

    /* Get the expression */
    hie10 (Expr);

    /* Check for a constant numeric expression */
    if (ED_IsConstAbs (Expr)) {

        if (IsClassFloat (Expr->Type)) {
            switch (Tok) {
                case TOK_MINUS: Expr->V.FVal = FP_D_Sub(FP_D_Make(0.0),Expr->V.FVal);               break;
                case TOK_PLUS:                                                                      break;
                case TOK_COMP:  Error ("Unary ~ operator not valid for floating point constant");   break;
                default:        Internal ("Unexpected token: %d", Tok);
            }
        } else {
            if (!IsClassInt (Expr->Type)) {
                Error ("Constant argument must have integer or float type");
                ED_MakeConstAbsInt (Expr, 1);
            }

            /* Value is numeric */
            switch (Tok) {
                case TOK_MINUS: Expr->IVal = -Expr->IVal;   break;
                case TOK_PLUS:                              break;
                case TOK_COMP:  Expr->IVal = ~Expr->IVal;   break;
                default:        Internal ("Unexpected token: %d", Tok);
            }

            /* Adjust the type of the expression */
            Expr->Type = IntPromotion (Expr->Type);

            /* Limit the calculated value to the range of its type */
            LimitExprValue (Expr, 1);
        }

    } else {
        unsigned Flags;

        /* If not constant, we can only handle integer types */
        if (!IsClassInt (Expr->Type)) {
            Error ("Non-constant argument must have integer type");
            ED_MakeConstAbsInt (Expr, 1);
        }

        /* Value is not constant */
        LoadExpr (CF_NONE, Expr);

        /* Adjust the type of the expression */
        Expr->Type = IntPromotion (Expr->Type);
        TypeConversion (Expr, Expr->Type);

        /* Get code generation flags */
        Flags = CG_TypeOf (Expr->Type);

        /* Handle the operation */
        switch (Tok) {
            case TOK_MINUS: g_neg (Flags);  break;
            case TOK_PLUS:                  break;
            case TOK_COMP:  g_com (Flags);  break;
            default:        Internal ("Unexpected token: %d", Tok);
        }

        /* The result is an rvalue in the primary */
        ED_FinalizeRValLoad (Expr);
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
            BoolExpr (hie10, Expr);
            if (ED_IsConstAbs (Expr)) {
                /* Constant numeric expression */
                Expr->IVal = !Expr->IVal;
            } else if (ED_IsEntityAddr (Expr)) {
                /* Address != NULL, so !Address == 0 */
                ED_MakeConstBool (Expr, 0);
            } else {
                /* Not constant, load into the primary */
                LoadExpr (CF_NONE, Expr);
                g_bneg (CG_TypeOf (Expr->Type));
                ED_FinalizeRValLoad (Expr);
                ED_TestDone (Expr);             /* bneg will set cc */
            }
            /* The result type is always boolean */
            Expr->Type = type_bool;
            break;

        case TOK_STAR:
            NextToken ();
            ExprWithCheck (hie10, Expr);

            /* If the expression is already a pointer to function, the
            ** additional dereferencing operator must be ignored. A function
            ** itself is represented as "pointer to function", so any number
            ** of dereference operators is legal, since the result will
            ** always be converted to "pointer to function".
            */
            if (IsTypeFuncLike (Expr->Type)) {
                /* Expression not storable */
                ED_MarkExprAsRVal (Expr);
            } else {
                if (!ED_IsQuasiConstAddr (Expr)) {
                    /* Not a constant address, load the pointer into the primary
                    ** and make it a calculated value.
                    */
                    LoadExpr (CF_NONE, Expr);
                    ED_FinalizeRValLoad (Expr);
                }

                if (IsClassPtr (Expr->Type)) {
                    Expr->Type = Indirect (Expr->Type);
                } else {
                    Error ("Illegal indirection");
                }
                /* If the expression points to an array, then don't convert the
                ** address -- it already is the location of the first element.
                */
                if (!IsTypeArray (Expr->Type)) {
                    /* The * operator yields an lvalue reference */
                    ED_IndExpr (Expr);
                }
            }
            break;

        case TOK_AND:
            NextToken ();
            ExprWithCheck (hie10, Expr);
            /* The & operator may be applied to any lvalue, and it may be
            ** applied to functions and arrays, even if they're not lvalues.
            */
            if (!IsTypeFunc (Expr->Type) && !IsTypeArray (Expr->Type)) {
                if (ED_IsRVal (Expr)) {
                    Error ("Illegal address");
                    /* Continue anyway, just to avoid further warnings */
                }

                if (IsTypeBitField (Expr->Type)) {
                    Error ("Cannot take address of bit-field");
                    /* Continue anyway, just to avoid further warnings */
                    Expr->Type = GetUnderlyingType (Expr->Type);
                }
                /* The & operator yields an rvalue address */
                ED_AddrExpr (Expr);
            }
            Expr->Type = AddressOf (Expr->Type);
            break;

        case TOK_SIZEOF:
            NextToken ();
            if (TypeSpecAhead ()) {
                Type T[MAXTYPELEN];
                Size = ExprCheckedSizeOf (ParseType (T));
            } else {
                /* Remember the output queue pointer */
                CodeMark Mark;
                GetCodePos (&Mark);

                /* The expression shall be unevaluated */
                ExprDesc Uneval;
                ED_Init (&Uneval);
                ED_MarkForUneval (&Uneval);
                hie10 (&Uneval);
                if (IsTypeBitField (Uneval.Type)) {
                    Error ("Cannot apply 'sizeof' to bit-field");
                    Size = 0;
                } else {
                    /* Calculate the size */
                    Size = ExprCheckedSizeOf (Uneval.Type);
                }
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

        ExprDesc Expr2;
        ED_Init (&Expr2);
        Expr2.Flags |= Expr->Flags & E_MASK_KEEP_SUBEXPR;

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
        ltype = CG_TypeOf (Expr->Type);
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

            /* Evaluate the result for operands */
            unsigned long Val1 = Expr->IVal;
            unsigned long Val2 = Expr2.IVal;

            /* Both operands are constant, remove the generated code */
            RemoveCode (&Mark1);

            /* Get the type of the result */
            Expr->Type = ArithmeticConvert (Expr->Type, Expr2.Type);

            /* Handle the op differently for signed and unsigned types */
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
                        if (!ED_IsUneval (Expr)) {
                            Warning ("Division by zero");
                        }
                        Expr->IVal = 0xFFFFFFFF;
                    } else {
                        /* Handle signed and unsigned operands differently */
                        if (IsSignSigned (Expr->Type)) {
                            Expr->IVal = ((long)Val1 / (long)Val2);
                        } else {
                            Expr->IVal = (Val1 / Val2);
                        }
                    }
                    break;
                case TOK_MOD:
                    if (Val2 == 0) {
                        if (!ED_IsUneval (Expr)) {
                            Warning ("Modulo operation with zero");
                        }
                        Expr->IVal = 0;
                    } else {
                        /* Handle signed and unsigned operands differently */
                        if (IsSignSigned (Expr->Type)) {
                            Expr->IVal = ((long)Val1 % (long)Val2);
                        } else {
                            Expr->IVal = (Val1 % Val2);
                        }
                    }
                    break;
                default:
                    Internal ("hie_internal: got token 0x%X\n", Tok);
            }

            /* Limit the calculated value to the range of its type */
            LimitExprValue (Expr, 1);

        } else if (lconst && (Gen->Flags & GEN_COMM) && !rconst) {
            /* If the LHS constant is an int that fits into an unsigned char, change the
            ** codegen type to unsigned char.  If the RHS is also an unsigned char, then
            ** g_typeadjust will return unsigned int (instead of int, which would be
            ** returned without this modification).  This allows more efficient operations,
            ** but does not affect correctness for the same reasons explained in g_typeadjust.
            */
            if (ltype == CF_INT && Expr->IVal >= 0 && Expr->IVal < 256) {
                ltype = CF_CHAR | CF_UNSIGNED;
            }

            /* The left side is constant, the right side is not, and the
            ** operator allows swapping the operands. We haven't pushed the
            ** left side onto the stack in this case, and will reverse the
            ** operation because this allows for better code.
            */
            unsigned rtype = ltype | CF_CONST;
            ltype = CG_TypeOf (Expr2.Type); /* Expr2 is now left */
            type = CF_CONST;
            if ((Gen->Flags & GEN_NOPUSH) == 0) {
                g_push (ltype, 0);
            } else {
                ltype |= CF_PRIMARY;        /* Value is in register */
            }

            /* Determine the type of the operation result. */
            type |= g_typeadjust (ltype, rtype);
            Expr->Type = ArithmeticConvert (Expr->Type, Expr2.Type);

            /* Generate code */
            Gen->Func (type, Expr->IVal);

            /* We have an rvalue in the primary now */
            ED_FinalizeRValLoad (Expr);

        } else {

            /* If the right hand side is constant, and the generator function
            ** expects the lhs in the primary, remove the push of the primary
            ** now.
            */
            unsigned rtype = CG_TypeOf (Expr2.Type);
            type = 0;
            if (rconst) {
                /* As above, but for the RHS. */
                if (rtype == CF_INT && Expr2.IVal >= 0 && Expr2.IVal < 256) {
                    rtype = CF_CHAR | CF_UNSIGNED;
                }
                /* Second value is constant - check for div */
                type |= CF_CONST;
                rtype |= CF_CONST;
                if (Expr2.IVal == 0 && !ED_IsUneval (Expr)) {
                    if (Tok == TOK_DIV) {
                        Warning ("Division by zero");
                    } else if (Tok == TOK_MOD) {
                        Warning ("Modulo operation with zero");
                    }
                }
                if ((Gen->Flags & GEN_NOPUSH) != 0) {
                    RemoveCode (&Mark2);
                    ltype |= CF_PRIMARY;    /* Value is in register */
                }
            }

            /* Determine the type of the operation result. */
            type |= g_typeadjust (ltype, rtype);
            Expr->Type = ArithmeticConvert (Expr->Type, Expr2.Type);

            /* Generate code */
            Gen->Func (type, Expr2.IVal);

            /* We have an rvalue in the primary now */
            ED_FinalizeRValLoad (Expr);
        }

        /* Propagate viral flags */
        ED_PropagateFrom (Expr, &Expr2);
    }
}



static void hie_compare (const GenDesc* Ops,    /* List of generators */
                         ExprDesc* Expr,
                         void (*hienext) (ExprDesc*))
/* Helper function for the compare operators */
{
    CodeMark Mark1;
    CodeMark Mark2;
    const GenDesc* Gen;
    token_t Tok;                        /* The operator token */
    unsigned ltype;
    int rconst;                         /* Operand is a constant */


    ExprWithCheck (hienext, Expr);

    while ((Gen = FindGen (CurTok.Tok, Ops)) != 0) {

        ExprDesc Expr2;
        ED_Init (&Expr2);
        Expr2.Flags |= Expr->Flags & E_MASK_KEEP_SUBEXPR;

        /* Remember the generator function */
        void (*GenFunc) (unsigned, unsigned long) = Gen->Func;

        /* Remember the operator token, then skip it */
        Tok = CurTok.Tok;
        NextToken ();

        /* If lhs is a function, convert it to the address of the function */
        if (IsTypeFunc (Expr->Type)) {
            Expr->Type = AddressOf (Expr->Type);
        }

        /* Get the lhs on stack */
        GetCodePos (&Mark1);
        ltype = CG_TypeOf (Expr->Type);
        if (ED_IsConstAbs (Expr)) {
            /* Numeric constant value */
            GetCodePos (&Mark2);
            g_push (ltype | CF_CONST, Expr->IVal);
        } else {
            /* Value not numeric constant */
            LoadExpr (CF_NONE, Expr);
            GetCodePos (&Mark2);
            g_push (ltype, 0);
        }

        /* Get the right hand side */
        MarkedExprWithCheck (hienext, &Expr2);

        /* If rhs is a function, convert it to the address of the function */
        if (IsTypeFunc (Expr2.Type)) {
            Expr2.Type = AddressOf (Expr2.Type);
        }

        /* Check for a numeric constant expression */
        rconst = (ED_IsConstAbs (&Expr2) && ED_CodeRangeIsEmpty (&Expr2));
        if (!rconst) {
            /* Not numeric constant, load into the primary */
            LoadExpr (CF_NONE, &Expr2);
        }

        /* Check if operands have allowed types for this operation */
        if (!IsRelationType (Expr->Type) || !IsRelationType (Expr2.Type)) {
            /* Output only one message even if both sides are wrong */
            TypeCompatibilityDiagnostic (Expr->Type, Expr2.Type, 1,
                "Comparing types '%s' with '%s' is invalid");
            /* Avoid further errors */
            ED_MakeConstAbsInt (Expr, 0);
            ED_MakeConstAbsInt (&Expr2, 0);
        }

        /* Some operations aren't allowed on function pointers */
        if ((Gen->Flags & GEN_NOFUNC) != 0) {
            if ((IsTypeFuncPtr (Expr->Type) || IsTypeFuncPtr (Expr2.Type))) {
                /* Output only one message even if both sides are wrong */
                Error ("Cannot use function pointers in this relation operation");
                /* Avoid further errors */
                ED_MakeConstAbsInt (Expr, 0);
                ED_MakeConstAbsInt (&Expr2, 0);
            }
        }

        /* Make sure, the types are compatible */
        if (IsClassInt (Expr->Type)) {
            if (!IsClassInt (Expr2.Type) && !ED_IsNullPtr (Expr)) {
                if (IsClassPtr (Expr2.Type)) {
                    TypeCompatibilityDiagnostic (Expr->Type, PtrConversion (Expr2.Type), 0,
                        "Comparing integer '%s' with pointer '%s'");
                } else {
                    TypeCompatibilityDiagnostic (Expr->Type, Expr2.Type, 1,
                        "Comparing types '%s' with '%s' is invalid");
                }
            }
        } else if (IsClassPtr (Expr->Type)) {
            if (IsClassPtr (Expr2.Type)) {
                /* Pointers are allowed in comparison */
                if (TypeCmp (Expr->Type, Expr2.Type).C < TC_VOID_PTR) {
                    /* Warn about distinct pointer types */
                    TypeCompatibilityDiagnostic (PtrConversion (Expr->Type), PtrConversion (Expr2.Type), 0,
                        "Distinct pointer types comparing '%s' with '%s'");
                }
            } else if (!ED_IsNullPtr (&Expr2)) {
                if (IsClassInt (Expr2.Type)) {
                    TypeCompatibilityDiagnostic (PtrConversion (Expr->Type), Expr2.Type, 0,
                        "Comparing pointer type '%s' with integer type '%s'");
                } else {
                    TypeCompatibilityDiagnostic (Expr->Type, Expr2.Type, 1,
                        "Comparing types '%s' with '%s' is invalid");
                }
            }
        }

        /* Check for numeric constant operands */
        if ((ED_IsEntityAddr (Expr) && ED_IsNullPtr (&Expr2)) ||
            (ED_IsNullPtr (Expr) && ED_IsEntityAddr (&Expr2))) {

            /* Object addresses are inequal to null pointer */
            Expr->IVal = (Tok != TOK_EQ);
            if (ED_IsNullPtr (&Expr2)) {
                if (Tok == TOK_LT || Tok == TOK_LE) {
                    Expr->IVal = 0;
                }
            } else {
                if (Tok == TOK_GT || Tok == TOK_GE) {
                    Expr->IVal = 0;
                }
            }

            /* Get rid of unwanted flags */
            ED_MakeConstBool (Expr, Expr->IVal);

            /* The result is constant, this is suspicious when not in
            ** preprocessor mode.
            */
            WarnConstCompareResult (Expr);

            if (ED_CodeRangeIsEmpty (&Expr2)) {
                /* Both operands are static, remove the load code */
                RemoveCode (&Mark1);
            } else {
                /* Drop pushed lhs */
                g_drop (sizeofarg (ltype));
                pop (ltype);
            }

        } else if (ED_IsEntityAddr (Expr)   &&
                   ED_IsEntityAddr (&Expr2) &&
                   Expr->Sym == Expr2.Sym) {

            /* Evaluate the result for static addresses */
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

            /* Get rid of unwanted flags */
            ED_MakeConstBool (Expr, Expr->IVal);

            /* If the result is constant, this is suspicious when not in
            ** preprocessor mode.
            */
            WarnConstCompareResult (Expr);

            if (ED_CodeRangeIsEmpty (&Expr2)) {
                /* Both operands are static, remove the load code */
                RemoveCode (&Mark1);
            } else {
                /* Drop pushed lhs */
                g_drop (sizeofarg (ltype));
                pop (ltype);
            }

        } else if (ED_IsConstAbs (Expr) && rconst) {

            /* Both operands are numeric constant, remove the generated code */
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

            /* Get rid of unwanted flags */
            ED_MakeConstBool (Expr, Expr->IVal);

            /* If the result is constant, this is suspicious when not in
            ** preprocessor mode.
            */
            WarnConstCompareResult (Expr);

        } else {

            /* Determine the signedness of the operands */
            int LeftSigned  = IsSignSigned (Expr->Type);
            int RightSigned = IsSignSigned (Expr2.Type);
            int CmpSigned   = IsClassInt (Expr->Type) && IsClassInt (Expr2.Type) &&
                              IsSignSigned (ArithmeticConvert (Expr->Type, Expr2.Type));

            /* If the right hand side is constant, and the generator function
            ** expects the lhs in the primary, remove the push of the primary
            ** now.
            */
            unsigned flags = 0;
            if (rconst) {
                flags |= CF_CONST;
                if ((Gen->Flags & GEN_NOPUSH) != 0) {
                    RemoveCode (&Mark2);
                    ltype |= CF_PRIMARY;    /* Value is in register */
                }
            }

            /* Determine the type of the operation. */
            if (IsRankChar (Expr->Type) && rconst && (!LeftSigned || RightSigned)) {

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

                /* Comparing a char against a constant may have a constant
                ** result. Please note: It is not possible to remove the code
                ** for the compare alltogether, because it may have side
                ** effects.
                */
                switch (Tok) {

                    case TOK_EQ:
                        if (Expr2.IVal < LeftMin || Expr2.IVal > LeftMax) {
                            ED_MakeConstBool (Expr, 0);
                            WarnConstCompareResult (Expr);
                            goto Done;
                        }
                        break;

                    case TOK_NE:
                        if (Expr2.IVal < LeftMin || Expr2.IVal > LeftMax) {
                            ED_MakeConstBool (Expr, 1);
                            WarnConstCompareResult (Expr);
                            goto Done;
                        }
                        break;

                    case TOK_LT:
                        if (Expr2.IVal <= LeftMin || Expr2.IVal > LeftMax) {
                            ED_MakeConstBool (Expr, Expr2.IVal > LeftMax);
                            WarnConstCompareResult (Expr);
                            goto Done;
                        }
                        break;

                    case TOK_LE:
                        if (Expr2.IVal < LeftMin || Expr2.IVal >= LeftMax) {
                            ED_MakeConstBool (Expr, Expr2.IVal >= LeftMax);
                            WarnConstCompareResult (Expr);
                            goto Done;
                        }
                        break;

                    case TOK_GE:
                        if (Expr2.IVal <= LeftMin || Expr2.IVal > LeftMax) {
                            ED_MakeConstBool (Expr, Expr2.IVal <= LeftMin);
                            WarnConstCompareResult (Expr);
                            goto Done;
                        }
                        break;

                    case TOK_GT:
                        if (Expr2.IVal < LeftMin || Expr2.IVal >= LeftMax) {
                            ED_MakeConstBool (Expr, Expr2.IVal < LeftMin);
                            WarnConstCompareResult (Expr);
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
                if (!LeftSigned || !RightSigned) {
                    CmpSigned = 0;
                    flags |= CF_UNSIGNED;
                }

            } else if (IsRankChar (Expr->Type) && IsRankChar (Expr2.Type) &&
                GetSignedness (Expr->Type) == GetSignedness (Expr2.Type)) {

                /* Both are chars with the same signedness. We can encode the
                ** operation as a char operation.
                */
                flags |= CF_CHAR;
                if (rconst) {
                    flags |= CF_FORCECHAR;
                }
                if (!LeftSigned || !RightSigned) {
                    CmpSigned = 0;
                    flags |= CF_UNSIGNED;
                }

            } else {
                unsigned rtype = CG_TypeOf (Expr2.Type) | (flags & CF_CONST);
                flags |= g_typeadjust (ltype, rtype);
            }

            /* If the comparison is made as unsigned types and the right is a
            ** constant, we may be able to change the compares to something more
            ** effective.
            */
            if (!CmpSigned && rconst) {

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
            ED_FinalizeRValLoad (Expr);

            /* Condition codes are set */
            ED_TestDone (Expr);
        }

        /* Result type is always boolean */
Done:   Expr->Type = type_bool;

        /* Propagate viral flags */
        ED_PropagateFrom (Expr, &Expr2);
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



static void parseadd (ExprDesc* Expr, int DoArrayRef)
/* Parse an expression with the binary plus or subscript operator. Expr contains
** the unprocessed left hand side of the expression and will contain the result
** of the expression on return. If DoArrayRef is zero, this evaluates the binary
** plus operation. Otherwise, this evaluates the subscript operation.
*/
{
    ExprDesc Expr2;
    unsigned flags;             /* Operation flags */
    CodeMark Mark;              /* Remember code position */
    const Type* lhst;           /* Type of left hand side */
    const Type* rhst;           /* Type of right hand side */
    int lscale;
    int rscale;
    int AddDone;                /* No need to generate runtime code */

    ED_Init (&Expr2);
    Expr2.Flags |= Expr->Flags & E_MASK_KEEP_SUBEXPR;

    /* Skip the PLUS or opening bracket token */
    NextToken ();

    /* Get the left hand side type, initialize operation flags */
    lhst = Expr->Type;
    flags = 0;
    lscale = rscale = 1;
    AddDone = 0;

    /* We can only do constant expressions for:
    ** - integer addition:
    **   - numeric + numeric
    **   - (integer)(base + offset) + numeric
    **   - numeric + (integer)(base + offset)
    ** - pointer offset:
    **   - (pointer)numeric + numeric * scale
    **   - (base + offset) + numeric * scale
    **   - (pointer)numeric + (integer)(base + offset) * 1
    **   - numeric * scale + (pointer)numeric
    **   - numeric * scale + (base + offset)
    **   - (integer)(base + offset) * 1 + (pointer)numeric
    */
    if (ED_IsQuasiConst (Expr)) {

        /* The left hand side is a constant of some sort. Good. Get rhs */
        ExprWithCheck (DoArrayRef ? hie0 : hie9, &Expr2);

        /* Right hand side is constant. Get the rhs type */
        rhst = Expr2.Type;
        if (ED_IsQuasiConst (&Expr2)) {

            /* Both expressions are constants. Check for pointer arithmetic */
            if (IsClassPtr (lhst) && IsClassInt (rhst)) {
                /* Left is pointer, right is int, must scale rhs */
                rscale = CheckedPSizeOf (lhst);
                /* Operate on pointers, result type is a pointer */
                flags = CF_PTR;
            } else if (IsClassInt (lhst) && IsClassPtr (rhst)) {
                /* Left is int, right is pointer, must scale lhs */
                lscale = CheckedPSizeOf (rhst);
                /* Operate on pointers, result type is a pointer */
                flags = CF_PTR;
            } else if (!DoArrayRef && IsClassInt (lhst) && IsClassInt (rhst)) {
                /* Integer addition */
                flags = CF_INT;
            } else {
                /* OOPS */
                AddDone = -1;
                /* Avoid further errors */
                ED_MakeConstAbsInt (Expr, 0);
            }

            if (!AddDone) {
                /* Do constant calculation if we can */
                if (ED_IsAbs (&Expr2) &&
                    (ED_IsAbs (Expr) || lscale == 1)) {
                    if (IsClassInt (lhst) && IsClassInt (rhst)) {
                        Expr->Type = ArithmeticConvert (Expr->Type, Expr2.Type);
                    }
                    Expr->IVal = Expr->IVal * lscale + Expr2.IVal * rscale;
                    AddDone = 1;
                } else if (ED_IsAbs (Expr) &&
                    (ED_IsAbs (&Expr2) || rscale == 1)) {
                    if (IsClassInt (lhst) && IsClassInt (rhst)) {
                        Expr2.Type = ArithmeticConvert (Expr2.Type, Expr->Type);
                    }
                    Expr2.IVal = Expr->IVal * lscale + Expr2.IVal * rscale;
                    /* Adjust the flags */
                    Expr2.Flags |= Expr->Flags & ~E_MASK_KEEP_SUBEXPR;
                    /* Get the symbol and the name */
                    *Expr = Expr2;
                    AddDone = 1;
                }
            }

            if (AddDone) {
                /* Adjust the result for addition */
                if (!DoArrayRef) {
                    if (IsClassPtr (lhst)) {
                        /* Result type is a pointer */
                        Expr->Type = lhst;
                    } else if (IsClassPtr (rhst)) {
                        /* Result type is a pointer */
                        Expr->Type = rhst;
                    } else {
                        /* Limit the calculated value to the range of its type */
                        LimitExprValue (Expr, 1);
                    }

                    /* The result is always an rvalue */
                    ED_MarkExprAsRVal (Expr);
                }
            } else {
                /* Decide the order */
                if (!ED_IsAbs (&Expr2) && rscale > 1) {
                    /* Rhs needs scaling but is not numeric. Load it. */
                    LoadExpr (CF_NONE, &Expr2);
                    /* Scale rhs */
                    g_scale (CF_INT, rscale);
                    /* Generate the code for the add */
                    if (ED_IsAbs (Expr)) {
                        /* Numeric constant */
                        g_inc (flags | CF_CONST, Expr->IVal);
                    } else if (ED_IsLocStack (Expr)) {
                        /* Local stack address */
                        g_addaddr_local (flags, Expr->IVal);
                    } else {
                        /* Static address */
                        g_addaddr_static (flags | CG_AddrModeFlags (Expr), Expr->Name, Expr->IVal);
                    }
                } else {
                    /* Lhs is not numeric. Load it. */
                    LoadExpr (CF_NONE, Expr);
                    /* Scale lhs if necessary */
                    if (lscale != 1) {
                        g_scale (CF_INT, lscale);
                    }
                    /* Generate the code for the add */
                    if (ED_IsAbs (&Expr2)) {
                        /* Numeric constant */
                        g_inc (flags | CF_CONST, Expr2.IVal);
                    } else if (ED_IsLocStack (&Expr2)) {
                        /* Local stack address */
                        g_addaddr_local (flags, Expr2.IVal);
                    } else {
                        /* Static address */
                        g_addaddr_static (flags | CG_AddrModeFlags (&Expr2), Expr2.Name, Expr2.IVal);
                    }
                }

                /* Result is an rvalue in primary register */
                ED_FinalizeRValLoad (Expr);
            }

        } else {

            /* lhs is constant and rhs is not constant. Load rhs into the
            ** primary.
            */
            GetCodePos (&Mark);
            LoadExpr (CF_NONE, &Expr2);

            /* Beware: The check above (for lhs) lets not only pass numeric
            ** constants, but also constant addresses (labels), maybe even
            ** with an offset. We have to check for that here.
            */

            /* Setup flags */
            if (ED_IsAbs (Expr)) {
                /* A numerical constant */
                flags |= CF_CONST;
            } else {
                /* Constant address label */
                flags |= CG_AddrModeFlags (Expr);
            }

            /* Check for pointer arithmetic */
            if (IsClassPtr (lhst) && IsClassInt (rhst)) {
                /* Left is pointer, right is int, must scale rhs */
                rscale = CheckedPSizeOf (lhst);
                g_scale (CF_INT, rscale);
                /* Operate on pointers, result type is a pointer */
                flags |= CF_PTR;
            } else if (IsClassInt (lhst) && IsClassPtr (rhst)) {
                /* Left is int, right is pointer, must scale lhs. */
                lscale = CheckedPSizeOf (rhst);
                /* Operate on pointers, result type is a pointer */
                flags |= CF_PTR;
                Expr->Type = Expr2.Type;
            } else if (!DoArrayRef && IsClassInt (lhst) && IsClassInt (rhst)) {
                /* Integer addition */
                flags |= typeadjust (Expr, &Expr2, 1);
            } else {
                /* OOPS */
                AddDone = -1;
            }

            /* Generate the code for the add */
            if (!AddDone) {
                if (ED_IsAbs (Expr) &&
                    Expr->IVal >= 0 &&
                    Expr->IVal * lscale < 256) {
                    /* Numeric constant */
                    g_inc (flags, Expr->IVal * lscale);
                    AddDone = 1;
                }
            }

            if (!AddDone) {
                if (ED_IsLocQuasiConst (&Expr2)  &&
                    !IsTypeBitField (Expr2.Type) &&
                    rscale == 1                  &&
                    CheckedSizeOf (rhst) == SIZEOF_CHAR) {
                    /* Change the order back */
                    RemoveCode (&Mark);
                    /* Load lhs */
                    LoadExpr (CF_NONE, Expr);
                    /* Use new flags */
                    flags = CF_CHAR | CG_AddrModeFlags (&Expr2);
                    /* Add the variable */
                    if (ED_IsLocStack (&Expr2)) {
                        g_addlocal (flags, Expr2.IVal);
                    } else {
                        g_addstatic (flags, Expr2.Name, Expr2.IVal);
                    }
                } else if (ED_IsAbs (Expr)) {
                    /* Numeric constant */
                    g_inc (flags, Expr->IVal * lscale);
                } else if (lscale == 1) {
                    if (ED_IsLocStack (Expr)) {
                        /* Constant address */
                        g_addaddr_local (flags, Expr->IVal);
                    } else {
                        g_addaddr_static (flags, Expr->Name, Expr->IVal);
                    }
                } else {
                    /* Since we do already have rhs in the primary, if lhs is
                    ** not a numeric constant, and the scale factor is not one
                    ** (no scaling), we must take the long way over the stack.
                    */
                    g_push (CG_TypeOf (Expr2.Type), 0); /* rhs --> stack */
                    LoadExpr (CF_NONE, Expr);
                    g_scale (CF_PTR, lscale);
                    g_add (CF_PTR, 0);
                }
            }

            /* Result is an rvalue in primary register */
            ED_FinalizeRValLoad (Expr);
        }

    } else {

        /* Left hand side is not constant. Get the value onto the stack. */
        LoadExpr (CF_NONE, Expr);               /* --> primary register */
        GetCodePos (&Mark);
        flags = CG_TypeOf (Expr->Type);         /* default codegen type */
        g_push (flags, 0);                      /* --> stack */

        /* Evaluate the rhs */
        MarkedExprWithCheck (DoArrayRef ? hie0 : hie9, &Expr2);

        /* Get the rhs type */
        rhst = Expr2.Type;

        /* Check for a constant rhs expression */
        if (ED_IsConstAbs (&Expr2) && ED_CodeRangeIsEmpty (&Expr2)) {

            /* Rhs is a numeric constant. Remove pushed lhs from stack. */
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
            } else if (!DoArrayRef && IsClassInt (lhst) && IsClassInt (rhst)) {
                /* Integer addition */
                flags = typeadjust (Expr, &Expr2, 1);
            } else {
                /* OOPS */
                AddDone = -1;
            }

            /* Generate code for the add */
            g_inc (flags | CF_CONST, Expr2.IVal);

        } else {

            /* Lhs and rhs are not so "numeric constant". Check for pointer arithmetic. */
            if (IsClassPtr (lhst) && IsClassInt (rhst)) {
                /* Left is pointer, right is int, must scale rhs */
                rscale = CheckedPSizeOf (lhst);
                if (ED_IsAbs (&Expr2)) {
                    Expr2.IVal *= rscale;
                    /* Load rhs into the primary */
                    LoadExpr (CF_NONE, &Expr2);
                } else {
                    /* Load rhs into the primary */
                    LoadExpr (CF_NONE, &Expr2);
                    g_scale (CF_INT, rscale);
                }
                /* Operate on pointers, result type is a pointer */
                flags = CF_PTR;
            } else if (IsClassInt (lhst) && IsClassPtr (rhst)) {
                /* Left is int, right is pointer, must scale lhs */
                lscale = CheckedPSizeOf (rhst);
                if (ED_CodeRangeIsEmpty (&Expr2)) {
                    RemoveCode (&Mark);             /* Remove pushed value from stack */
                    g_scale (CF_INT, lscale);
                    g_push (CF_PTR, 0);             /* --> stack */
                    LoadExpr (CF_NONE, &Expr2);     /* Load rhs into primary register */
                } else {
                    g_tosint (CG_TypeOf (lhst));    /* Make sure TOS is int */
                    LoadExpr (CF_NONE, &Expr2);     /* Load rhs into primary register */
                    if (lscale != 1) {
                        g_swap (CF_INT);            /* Swap TOS and primary */
                        g_scale (CF_INT, CheckedPSizeOf (rhst));
                    }
                }
                /* Operate on pointers, result type is a pointer */
                flags = CF_PTR;
                Expr->Type = Expr2.Type;
            } else if (!DoArrayRef && IsClassInt (lhst) && IsClassInt (rhst)) {
                /* Integer addition */
                /* Load rhs into the primary */
                LoadExpr (CF_NONE, &Expr2);
                /* Adjust rhs primary if needed  */
                flags = typeadjust (Expr, &Expr2, 0);
            } else {
                /* OOPS */
                AddDone = -1;
                /* We can't just goto End as that would leave the stack unbalanced */
            }

            /* Generate code for the add (the & is a hack here) */
            g_add (flags & ~CF_CONST, 0);

        }

        /* Result is an rvalue in primary register */
        ED_FinalizeRValLoad (Expr);
    }

    /* Deal with array ref */
    if (DoArrayRef) {
        /* Check the types of array and subscript */
        if (IsClassPtr (lhst)) {
            if (!IsClassInt (rhst))  {
                Error ("Array subscript is not an integer");
                ED_MakeConstAbs (Expr, 0, GetCharArrayType (1));
            }
        } else if (IsClassInt (lhst)) {
            if (!IsClassPtr (rhst)) {
                Error ("Subscripted value is neither array nor pointer");
                ED_MakeConstAbs (Expr, 0, GetCharArrayType (1));
            }
        } else {
            Error ("Cannot subscript");
            ED_MakeConstAbs (Expr, 0, GetCharArrayType (1));
        }

        /* The final result is usually an lvalue expression of element type
        ** referenced in the primary, unless it is once again an array. We can just
        ** assume the usual case first, and change it later if necessary.
        */
        ED_IndExpr (Expr);
        Expr->Type = Indirect (Expr->Type);

        /* An array element is actually a variable. So the rules for variables with
        ** respect to the reference type apply: If it's an array, it is virtually
        ** an rvalue address, otherwise it's an lvalue reference. (A function would
        ** also be an rvalue address, but an array cannot contain functions).
        */
        if (IsTypeArray (Expr->Type)) {
            ED_AddrExpr (Expr);
        }

        /* Consume the closing bracket */
        ConsumeRBrack ();
    } else {
        if (AddDone < 0) {
            Error ("Invalid operands for binary operator '+'");
        } else {
            /* Array and function types must be converted to pointer types */
            Expr->Type = StdConversion (Expr->Type);
        }
    }

    /* Condition code not set */
    ED_MarkAsUntested (Expr);

    /* Propagate viral flags */
    ED_PropagateFrom (Expr, &Expr2);
}



static void parsesub (ExprDesc* Expr)
/* Parse an expression with the binary minus operator. Expr contains the
** unprocessed left hand side of the expression and will contain the
** result of the expression on return.
*/
{
    ExprDesc Expr2;
    unsigned flags;             /* Operation flags */
    const Type* lhst;           /* Type of left hand side */
    const Type* rhst;           /* Type of right hand side */
    CodeMark Mark1;             /* Save position of output queue */
    CodeMark Mark2;             /* Another position in the queue */
    int rscale;                 /* Scale factor for pointer arithmetics */
    int SubDone;                /* No need to generate runtime code */

    ED_Init (&Expr2);
    Expr2.Flags |= Expr->Flags & E_MASK_KEEP_SUBEXPR;

    /* lhs cannot be function or pointer to function */
    if (IsTypeFuncLike (Expr->Type)) {
        Error ("Invalid left operand for binary operator '-'");
        /* Make it pointer to char to avoid further errors */
        Expr->Type = type_uchar;
    }

    /* Skip the MINUS token */
    NextToken ();

    /* Get the left hand side type, initialize operation flags */
    lhst = Expr->Type;
    flags = CF_INT;             /* Default result type */
    rscale = 1;                 /* Scale by 1, that is, don't scale */
    SubDone = 0;                /* Generate runtime code by default */

    /* Remember the output queue position, then bring the value onto the stack */
    GetCodePos (&Mark1);
    LoadExpr (CF_NONE, Expr);       /* --> primary register */
    GetCodePos (&Mark2);
    g_push (CG_TypeOf (lhst), 0);   /* --> stack */

    /* Parse the right hand side */
    MarkedExprWithCheck (hie9, &Expr2);

    /* rhs cannot be function or pointer to function */
    if (IsTypeFuncLike (Expr2.Type)) {
        Error ("Invalid right operand for binary operator '-'");
        /* Make it pointer to char to avoid further errors */
        Expr2.Type = type_uchar;
    }

    /* Get the rhs type */
    rhst = Expr2.Type;

    if (IsClassPtr (lhst)) {
        /* We'll have to scale the result */
        rscale = PSizeOf (lhst);
        /* We cannot scale by 0-size or unknown-size */
        if (rscale == 0 && (IsClassPtr (rhst) || IsClassInt (rhst))) {
            TypeCompatibilityDiagnostic (lhst, rhst,
                1, "Invalid pointer types in subtraction: '%s' and '%s'");
            /* Avoid further errors */
            rscale = 1;
        }
        /* Generate code for pointer subtraction */
        flags = CF_PTR;
    }

    /* We can only do constant expressions for:
    ** - integer subtraction:
    **   - numeric - numeric
    **   - (integer)(base + offset) - numeric
    **   - (integer)(same_base + offset) - (integer)(same_base + offset)
    ** - pointer offset:
    **   - (pointer)numeric - numeric * scale
    **   - (base + offset) - numeric * scale
    **   - (same_base + offset) - (integer)(same_base + offset) * 1
    ** - pointer diff:
    **   - (numeric - numeric) / scale
    **   - ((same_base + offset) - (same_base + offset)) / scale
    **   - ((base + offset) - (pointer)numeric) / 1
    */
    if (IsClassPtr (lhst) && IsClassPtr (rhst)) {

        /* Pointer Diff. We've got the scale factor and flags above */
        typecmp_t Cmp = TypeCmp (lhst, rhst);
        if (Cmp.C < TC_STRICT_COMPATIBLE) {
            TypeCompatibilityDiagnostic (lhst, rhst,
                1, "Incompatible pointer types in subtraction: '%s' and '%s'");
        }

        /* Operate on pointers, result type is an integer */
        Expr->Type = type_int;

        /* Check for a constant rhs expression */
        if (ED_IsQuasiConst (&Expr2) && ED_CodeRangeIsEmpty (&Expr2)) {
            /* The right hand side is constant. Check left hand side. */
            if (ED_IsQuasiConst (Expr)) {
                /* We can't do all 'ptr1 - ptr2' constantly at the moment */
                if (ED_GetLoc (Expr) == ED_GetLoc (&Expr2) && Expr->Sym == Expr2.Sym) {
                    Expr->IVal = (Expr->IVal - Expr2.IVal) / rscale;
                    /* Get rid of unneeded flags etc. */
                    ED_MakeConstAbsInt (Expr, Expr->IVal);
                    /* No runtime code */
                    SubDone = 1;
                } else if (rscale == 1 && ED_IsConstAbs (&Expr2)) {
                    Expr->IVal = (Expr->IVal - Expr2.IVal) / rscale;
                    /* No runtime code */
                    SubDone = 1;
                }
            }
        }

        if (!SubDone) {
            /* We'll do runtime code */
            if (ED_IsConstAbs (&Expr2) && ED_CodeRangeIsEmpty (&Expr2)) {
                /* Remove pushed value from stack */
                RemoveCode (&Mark2);
                /* Do the subtraction */
                g_dec (CF_INT | CF_CONST, Expr2.IVal);
            } else {
                /* load into the primary */
                LoadExpr (CF_NONE, &Expr2);
                /* Generate code for the sub */
                g_sub (CF_INT, 0);
            }
            /* We must scale the result */
            if (rscale != 1) {
                g_scale (CF_INT, -rscale);
            }
            /* Result is an rvalue in the primary register */
            ED_FinalizeRValLoad (Expr);
        } else {
            /* Remove pushed value from stack */
            RemoveCode (&Mark1);
            /* The result is always an rvalue */
            ED_MarkExprAsRVal (Expr);
        }

    } else if (ED_IsQuasiConst (&Expr2) && ED_CodeRangeIsEmpty (&Expr2)) {

        /* Right hand side is constant. Check left hand side. */
        if (ED_IsQuasiConst (Expr)) {

            /* Both sides are constant. Check for pointer arithmetic */
            if (IsClassPtr (lhst) && IsClassInt (rhst)) {
                /* Pointer subtraction. We've got the scale factor and flags above */
            } else if (IsClassInt (lhst) && IsClassInt (rhst)) {
                /* Integer subtraction. We'll adjust the types later */
            } else {
                /* OOPS */
                Error ("Invalid operands for binary operator '-'");
            }

            /* We can't make all subtraction expressions constant */
            if (ED_IsConstAbs (&Expr2)) {
                Expr->IVal -= Expr2.IVal * rscale;
                /* No runtime code */
                SubDone = 1;
            } else if (rscale == 1 && Expr->Sym == Expr2.Sym) {
                /* The result is the diff of the offsets */
                Expr->IVal -= Expr2.IVal;
                /* Get rid of unneeded bases and flags etc. */
                ED_MakeConstAbs (Expr, Expr->IVal, Expr->Type);
                /* No runtime code */
                SubDone = 1;
            }

            if (SubDone) {
                /* Remove loaded and pushed value from stack */
                RemoveCode (&Mark1);
                if (IsClassInt (lhst)) {
                    /* Just adjust the result type */
                    Expr->Type = ArithmeticConvert (Expr->Type, Expr2.Type);
                    /* And limit the calculated value to the range of it */
                    LimitExprValue (Expr, 1);
                }
                /* The result is always an rvalue */
                ED_MarkExprAsRVal (Expr);
            } else {
                if (ED_IsConstAbs (&Expr2)) {
                    /* Remove pushed value from stack */
                    RemoveCode (&Mark2);
                    if (IsClassInt (lhst)) {
                        /* Adjust the types */
                        flags = typeadjust (Expr, &Expr2, 1);
                    }
                    /* Do the subtraction */
                    g_dec (flags | CF_CONST, Expr2.IVal * rscale);
                } else {
                    if (IsClassInt (lhst)) {
                        /* Adjust the types */
                        flags = typeadjust (Expr, &Expr2, 0);
                    }
                    /* Load rhs into the primary */
                    LoadExpr (CF_NONE, &Expr2);
                    g_scale (CG_TypeOf (rhst), rscale);
                    /* Generate code for the sub (the & is a hack here) */
                    g_sub (flags & ~CF_CONST, 0);
                }
                /* Result is an rvalue in the primary register */
                ED_FinalizeRValLoad (Expr);
            }

        } else {

            /* Left hand side is not constant, right hand side is */
            if (IsClassPtr (lhst) && IsClassInt (rhst)) {
                /* Pointer subtraction. We've got the scale factor and flags above */
            } else if (IsClassInt (lhst) && IsClassInt (rhst)) {
                /* Integer subtraction. We'll adjust the types later */
            } else {
                /* OOPS */
                Error ("Invalid operands for binary operator '-'");
                flags = CF_INT;
            }

            if (ED_IsConstAbs (&Expr2)) {
                /* Remove pushed value from stack */
                RemoveCode (&Mark2);
                if (IsClassInt (lhst)) {
                    /* Adjust the types */
                    flags = typeadjust (Expr, &Expr2, 1);
                }
                /* Do the subtraction */
                g_dec (flags | CF_CONST, Expr2.IVal * rscale);
            } else {
                if (IsClassInt (lhst)) {
                    /* Adjust the types */
                    flags = typeadjust (Expr, &Expr2, 0);
                }
                /* Load rhs into the primary */
                LoadExpr (CF_NONE, &Expr2);
                g_scale (CG_TypeOf (rhst), rscale);
                /* Generate code for the sub (the & is a hack here) */
                g_sub (flags & ~CF_CONST, 0);
            }

            /* Result is an rvalue in the primary register */
            ED_FinalizeRValLoad (Expr);
        }

    } else {

        /* We'll use the pushed lhs on stack instead of the original source */
        ED_FinalizeRValLoad (Expr);

        /* Right hand side is not constant, load into the primary */
        LoadExpr (CF_NONE, &Expr2);

        /* Check for pointer arithmetic */
        if (IsClassPtr (lhst) && IsClassInt (rhst)) {
            /* Left is pointer, right is int, must scale rhs */
            g_scale (CF_INT, rscale);
        } else if (IsClassInt (lhst) && IsClassInt (rhst)) {
            /* Adjust operand types */
            flags = typeadjust (Expr, &Expr2, 0);
        } else {
            /* OOPS */
            Error ("Invalid operands for binary operator '-'");
        }

        /* Generate code for the sub (the & is a hack here) */
        g_sub (flags & ~CF_CONST, 0);

        /* Result is an rvalue in the primary register */
        ED_FinalizeRValLoad (Expr);
    }

    /* Result type is either a pointer or an integer */
    Expr->Type = StdConversion (Expr->Type);

    /* Condition code not set */
    ED_MarkAsUntested (Expr);

    /* Propagate viral flags */
    ED_PropagateFrom (Expr, &Expr2);
}



void hie8 (ExprDesc* Expr)
/* Process + and - binary operators. */
{
    ExprWithCheck (hie9, Expr);
    while (CurTok.Tok == TOK_PLUS || CurTok.Tok == TOK_MINUS) {
        if (CurTok.Tok == TOK_PLUS) {
            parseadd (Expr, 0);
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



static int hieAnd (ExprDesc* Expr, unsigned* TrueLab, int* TrueLabAllocated)
/* Process "exp && exp". This should only be called within hieOr.
** Return true if logic AND does occur.
*/
{
    unsigned Flags = Expr->Flags & E_MASK_KEEP_SUBEXPR;
    int HasFalseJump = 0, HasTrueJump = 0;
    CodeMark Start;

    /* The label that we will use for false expressions */
    int FalseLab = 0;

    /* Get lhs */
    GetCodePos (&Start);
    ExprWithCheck (hie2, Expr);
    if ((Flags & E_EVAL_UNEVAL) == E_EVAL_UNEVAL) {
        RemoveCode (&Start);
    }

    if (CurTok.Tok == TOK_BOOL_AND) {

        ExprDesc Expr2;

        /* Check type */
        if (!ED_IsBool (Expr)) {
            Error ("Scalar expression expected");
            ED_MakeConstBool (Expr, 0);
        } else if ((Flags & E_EVAL_UNEVAL) != E_EVAL_UNEVAL) {
            if (!ED_IsConstBool (Expr)) {
                /* Set the test flag */
                ED_RequireTest (Expr);

                /* Load the value */
                LoadExpr (CF_FORCECHAR, Expr);

                /* Append deferred inc/dec at sequence point */
                DoDeferred (SQP_KEEP_TEST, Expr);

                /* Clear the test flag */
                ED_RequireNoTest (Expr);

                if (HasFalseJump == 0) {
                    /* Remember that the jump is used */
                    HasFalseJump = 1;
                    /* Get a label for false expressions */
                    FalseLab = GetLocalLabel ();
                }

                /* Generate the jump */
                g_falsejump (CF_NONE, FalseLab);
            } else {
                /* Constant boolean subexpression could still have deferred inc/dec
                ** operations, so just flush their side-effects at this sequence
                ** point.
                */
                DoDeferred (SQP_KEEP_NONE, Expr);

                if (ED_IsConstFalse (Expr)) {
                    /* Skip remaining */
                    Flags |= E_EVAL_UNEVAL;
                }
            }
        }

        /* Parse more boolean and's */
        while (CurTok.Tok == TOK_BOOL_AND) {

            ED_Init (&Expr2);
            Expr2.Flags = Flags;

            /* Skip the && */
            NextToken ();

            /* Get rhs */
            GetCodePos (&Start);
            hie2 (&Expr2);
            if ((Flags & E_EVAL_UNEVAL) == E_EVAL_UNEVAL) {
                RemoveCode (&Start);
            }

            /* Check type */
            if (!ED_IsBool (&Expr2)) {
                Error ("Scalar expression expected");
                ED_MakeConstBool (&Expr2, 0);
            } else if ((Flags & E_EVAL_UNEVAL) != E_EVAL_UNEVAL) {
                if (!ED_IsConstBool (&Expr2)) {
                    ED_RequireTest (&Expr2);
                    LoadExpr (CF_FORCECHAR, &Expr2);

                    /* Append deferred inc/dec at sequence point */
                    DoDeferred (SQP_KEEP_TEST, &Expr2);

                    /* Do short circuit evaluation */
                    if (CurTok.Tok == TOK_BOOL_AND) {
                        if (HasFalseJump == 0) {
                            /* Remember that the jump is used */
                            HasFalseJump = 1;
                            /* Get a label for false expressions */
                            FalseLab = GetLocalLabel ();
                        }
                        g_falsejump (CF_NONE, FalseLab);
                    } else {
                        /* We need the true label for the last expression */
                        HasTrueJump = 1;
                    }
                } else {
                    /* Constant boolean subexpression could still have deferred inc/
                    ** dec operations, so just flush their side-effects at this
                    ** sequence point.
                    */
                    DoDeferred (SQP_KEEP_NONE, &Expr2);

                    if (ED_IsConstFalse (&Expr2)) {
                        /* Skip remaining */
                        Flags |= E_EVAL_UNEVAL;
                        /* The value of the expression will be false */
                        ED_MakeConstBool (Expr, 0);
                    }
                }
            }

            /* Propagate viral flags */
            if ((Expr->Flags & E_EVAL_UNEVAL) != E_EVAL_UNEVAL) {
                ED_PropagateFrom (Expr, &Expr2);
            }

        }

        /* Last expression */
        if ((Flags & E_EVAL_UNEVAL) != E_EVAL_UNEVAL) {
            if (HasFalseJump || HasTrueJump) {
                if (*TrueLabAllocated == 0) {
                    /* Get a label that we will use for true expressions */
                    *TrueLab = GetLocalLabel ();
                    *TrueLabAllocated = 1;
                }
                if (!ED_IsConstAbs (&Expr2)) {
                    /* Will branch to true and fall to false */
                    g_truejump (CF_NONE, *TrueLab);
                } else {
                    /* Will jump away */
                    g_jump (*TrueLab);
                }
                /* The result is an rvalue in primary */
                ED_FinalizeRValLoad (Expr);
                /* No need to test as the result will be jumped to */
                ED_TestDone (Expr);
            }
        }

        if (HasFalseJump) {
            /* Define the false jump label here */
            g_defcodelabel (FalseLab);
        }

        /* Convert to bool */
        if ((ED_IsConstAbs (Expr) && Expr->IVal != 0) ||
            ED_IsAddrExpr (Expr)) {
            ED_MakeConstBool (Expr, 1);
        } else {
            Expr->Type = type_bool;
        }

        /* Tell our caller that we're evaluating a boolean */
        return 1;
    }

    return 0;
}



static void hieOr (ExprDesc *Expr)
/* Process "exp || exp". */
{
    unsigned Flags = Expr->Flags & E_MASK_KEEP_SUBEXPR;
    int      AndOp;             /* Did we have a && operation? */
    unsigned TrueLab;           /* Jump to this label if true */
    int      HasTrueJump = 0;
    CodeMark Start;

    /* Call the next level parser */
    GetCodePos (&Start);
    AndOp = hieAnd (Expr, &TrueLab, &HasTrueJump);
    if ((Flags & E_EVAL_UNEVAL) == E_EVAL_UNEVAL) {
        RemoveCode (&Start);
    }

    /* Any boolean or's? */
    if (CurTok.Tok == TOK_BOOL_OR) {

        /* Check type */
        if (!ED_IsBool (Expr)) {
            Error ("Scalar expression expected");
            ED_MakeConstBool (Expr, 0);
        } else if ((Flags & E_EVAL_UNEVAL) != E_EVAL_UNEVAL) {

            if (!ED_IsConstBool (Expr)) {
                /* Test the lhs if we haven't had && operators. If we had them, the
                ** jump is already in place and there's no need to do the test.
                */
                if (!AndOp) {
                    /* Set the test flag */
                    ED_RequireTest (Expr);

                    /* Get first expr */
                    LoadExpr (CF_FORCECHAR, Expr);

                    /* Append deferred inc/dec at sequence point */
                    DoDeferred (SQP_KEEP_TEST, Expr);

                    /* Clear the test flag */
                    ED_RequireNoTest (Expr);

                    if (HasTrueJump == 0) {
                        /* Get a label that we will use for true expressions */
                        TrueLab = GetLocalLabel();
                        HasTrueJump = 1;
                    }

                    /* Jump to TrueLab if true */
                    g_truejump (CF_NONE, TrueLab);
                }
            } else {
                /* Constant boolean subexpression could still have deferred inc/dec
                ** operations, so just flush their side-effects at this sequence
                ** point.
                */
                DoDeferred (SQP_KEEP_NONE, Expr);

                if (ED_IsConstTrue (Expr)) {
                    /* Skip remaining */
                    Flags |= E_EVAL_UNEVAL;
                }
            }
        }

        /* while there's more expr */
        while (CurTok.Tok == TOK_BOOL_OR) {

            ExprDesc Expr2;
            ED_Init (&Expr2);
            Expr2.Flags = Flags;

            /* skip the || */
            NextToken ();

            /* Get rhs subexpression */
            GetCodePos (&Start);
            AndOp = hieAnd (&Expr2, &TrueLab, &HasTrueJump);
            if ((Flags & E_EVAL_UNEVAL) == E_EVAL_UNEVAL) {
                RemoveCode (&Start);
            }

            /* Check type */
            if (!ED_IsBool (&Expr2)) {
                Error ("Scalar expression expected");
                ED_MakeConstBool (&Expr2, 0);
            } else if ((Flags & E_EVAL_UNEVAL) != E_EVAL_UNEVAL) {

                if (!ED_IsConstBool (&Expr2)) {
                    /* If there is more to come, add shortcut boolean eval */
                    if (!AndOp) {
                        ED_RequireTest (&Expr2);
                        LoadExpr (CF_FORCECHAR, &Expr2);

                        /* Append deferred inc/dec at sequence point */
                        DoDeferred (SQP_KEEP_TEST, &Expr2);

                        if (HasTrueJump == 0) {
                            TrueLab = GetLocalLabel();
                            HasTrueJump = 1;
                        }
                        g_truejump (CF_NONE, TrueLab);
                    }
                } else {
                    /* Constant boolean subexpression could still have deferred inc/
                    ** dec operations, so just flush their side-effects at this
                    ** sequence point.
                    */
                    DoDeferred (SQP_KEEP_NONE, &Expr2);

                    if (ED_IsConstTrue (&Expr2)) {
                        /* Skip remaining */
                        Flags |= E_EVAL_UNEVAL;
                        /* The result is always true */
                        ED_MakeConstBool (Expr, 1);
                    }
                }
            }

            /* Propagate viral flags */
            if ((Expr->Flags & E_EVAL_UNEVAL) != E_EVAL_UNEVAL) {
                ED_PropagateFrom (Expr, &Expr2);
            }

        }

        /* Convert to bool */
        if ((ED_IsConstAbs (Expr) && Expr->IVal != 0) ||
            ED_IsAddrExpr (Expr)) {
            ED_MakeConstBool (Expr, 1);
        } else {
            Expr->Type = type_bool;
        }
    }

    /* If we really had boolean ops, generate the end sequence if necessary */
    if (HasTrueJump) {
        if ((Flags & E_EVAL_UNEVAL) != E_EVAL_UNEVAL) {
            /* False case needs to jump over true case */
            unsigned DoneLab = GetLocalLabel ();
            /* Load false only if the result is not true */
            g_getimmed (CF_INT | CF_CONST, 0, 0);   /* Load FALSE */
            g_falsejump (CF_NONE, DoneLab);

            /* Load the true value */
            g_defcodelabel (TrueLab);
            g_getimmed (CF_INT | CF_CONST, 1, 0);   /* Load TRUE */
            g_defcodelabel (DoneLab);
        } else {
            /* Load the true value */
            g_defcodelabel (TrueLab);
            g_getimmed (CF_INT | CF_CONST, 1, 0);   /* Load TRUE */
        }

        /* The result is an rvalue in primary */
        ED_FinalizeRValLoad (Expr);
        /* Condition codes are set */
        ED_TestDone (Expr);
    }
}



static void hieQuest (ExprDesc* Expr)
/* Parse the ternary operator */
{
    int         FalseLab = 0;
    int         TrueLab = 0;
    CodeMark    SkippedBranch;
    CodeMark    TrueCodeEnd;
    ExprDesc    Expr2;          /* Expression 2 */
    ExprDesc    Expr3;          /* Expression 3 */
    int         Expr2IsNULL;    /* Expression 2 is a NULL pointer */
    int         Expr3IsNULL;    /* Expression 3 is a NULL pointer */
    Type*       ResultType;     /* Type of result */

    /* Call the lower level eval routine */
    ExprWithCheck (hieOr, Expr);

    /* Check if it's a ternary expression */
    if (CurTok.Tok == TOK_QUEST) {

        /* The constant condition must be compile-time known as well */
        int ConstantCond = ED_IsConstBool (Expr);
        unsigned Flags   = Expr->Flags & E_MASK_KEEP_RESULT;

        ED_Init (&Expr2);
        Expr2.Flags = Flags;
        ED_Init (&Expr3);
        Expr3.Flags = Flags;

        if (!ConstantCond) {
            /* Condition codes not set, request a test */
            ED_RequireTest (Expr);
            LoadExpr (CF_NONE, Expr);

            /* Append deferred inc/dec at sequence point */
            DoDeferred (SQP_KEEP_TEST, Expr);

            FalseLab = GetLocalLabel ();
            g_falsejump (CF_NONE, FalseLab);
        } else {
            /* Convert non-integer constant to boolean constant, so that we
            ** may just check it in an easier way later.
            */
            if (ED_IsConstTrue (Expr)) {
                ED_MakeConstBool (Expr, 1);
            } else if (ED_IsConstFalse (Expr)) {
                ED_MakeConstBool (Expr, 0);
            }

            /* Constant boolean subexpression could still have deferred inc/dec
            ** operations, so just flush their side-effects at this sequence point.
            */
            DoDeferred (SQP_KEEP_NONE, Expr);

            if (Expr->IVal == 0) {
                /* Remember the current code position */
                GetCodePos (&SkippedBranch);

                /* Expr2 is unevaluated when the condition is false */
                Expr2.Flags |= E_EVAL_UNEVAL;
            } else {
                /* Expr3 is unevaluated when the condition is true */
                Expr3.Flags |= E_EVAL_UNEVAL;
            }
        }

        /* Skip the question mark */
        NextToken ();

        /* Parse second expression. Remember for later if it is a NULL pointer
        ** constant expression, then load it into the primary.
        */
        ExprWithCheck (hie0, &Expr2);
        Expr2IsNULL = ED_IsNullPtrConstant (&Expr2);
        if (!IsTypeVoid (Expr2.Type)    &&
            ED_YetToLoad (&Expr2)       &&
            (!ConstantCond || !ED_IsConst (&Expr2))) {
            /* Load it into the primary */
            LoadExpr (CF_NONE, &Expr2);

            /* Append deferred inc/dec at sequence point */
            DoDeferred (SQP_KEEP_EXPR, &Expr2);

            ED_FinalizeRValLoad (&Expr2);
        } else {
            /* Constant subexpression could still have deferred inc/dec
            ** operations, so just flush their side-effects at this sequence
            ** point.
            */
            DoDeferred (SQP_KEEP_NONE, &Expr2);
        }
        Expr2.Type = PtrConversion (Expr2.Type);

        if (!ConstantCond) {
            /* Remember the current code position */
            GetCodePos (&TrueCodeEnd);

            /* Jump around the evaluation of the third expression */
            TrueLab = GetLocalLabel ();

            g_jump (TrueLab);

            /* Jump here if the first expression was false */
            g_defcodelabel (FalseLab);
        } else {
            if (Expr->IVal == 0) {
                /* Remove the load code of Expr2 */
                RemoveCode (&SkippedBranch);
            } else {
                /* Remember the current code position */
                GetCodePos (&SkippedBranch);
            }
        }

        ConsumeColon ();

        /* Parse third expression. Remember for later if it is a NULL pointer
        ** constant expression, then load it into the primary.
        */
        ExprWithCheck (hieQuest, &Expr3);
        Expr3IsNULL = ED_IsNullPtrConstant (&Expr3);
        if (!IsTypeVoid (Expr3.Type)    &&
            ED_YetToLoad (&Expr3)       &&
            (!ConstantCond || !ED_IsConst (&Expr3))) {
            /* Load it into the primary */
            LoadExpr (CF_NONE, &Expr3);

            /* Append deferred inc/dec at sequence point */
            DoDeferred (SQP_KEEP_EXPR, &Expr3);

            ED_FinalizeRValLoad (&Expr3);
        } else {
            /* Constant subexpression could still have deferred inc/dec
            ** operations, so just flush their side-effects at this sequence
            ** point.
            */
            DoDeferred (SQP_KEEP_NONE, &Expr3);
        }
        Expr3.Type = PtrConversion (Expr3.Type);

        if (ConstantCond && Expr->IVal != 0) {
            /* Remove the load code of Expr3 */
            RemoveCode (&SkippedBranch);
        }

        /* Check if any conversions are needed, if so, do them.
        ** Conversion rules for ?: expression are:
        **   - if both expressions are int expressions, default promotion
        **     rules for ints apply.
        **   - if both expressions have the same structure, union or void type,
        **     the result has the same type.
        **   - if both expressions are pointers to compatible types (possibly
        **     qualified differently), the result of the expression is an
        **     appropriately qualified version of the composite type.
        **   - if one of the expressions is a pointer and the other is a
        **     pointer to (possibly qualified) void, the resulting type is a
        **     pointer to appropriately qualified void.
        **   - if one of the expressions is a pointer and the other is
        **     a null pointer constant, the resulting type is that of the
        **     pointer type.
        **   - all other cases are flagged by an error.
        */
        if (IsClassInt (Expr2.Type) && IsClassInt (Expr3.Type)) {

            CodeMark    CvtCodeStart;
            CodeMark    CvtCodeEnd;


            /* Get common type */
            ResultType = TypeDup (ArithmeticConvert (Expr2.Type, Expr3.Type));

            /* Convert the third expression to this type if needed */
            TypeConversion (&Expr3, ResultType);

            /* Emit conversion code for the second expression, but remember
            ** where it starts end ends.
            */
            GetCodePos (&CvtCodeStart);
            TypeConversion (&Expr2, ResultType);
            GetCodePos (&CvtCodeEnd);

            if (!ConstantCond) {
                /* If we had conversion code, move it to the right place */
                if (!CodeRangeIsEmpty (&CvtCodeStart, &CvtCodeEnd)) {
                    MoveCode (&CvtCodeStart, &CvtCodeEnd, &TrueCodeEnd);
                }
            }

        } else if (IsClassPtr (Expr2.Type) && IsClassPtr (Expr3.Type)) {
            /* If one of the two is 'void *', the result type is a pointer to
            ** appropriately qualified void.
            */
            if (IsTypeVoid (Indirect (Expr2.Type))) {
                ResultType = NewPointerTo (Indirect (Expr2.Type));
                ResultType[1].C |= GetQualifier (Indirect (Expr3.Type));
            } else if (IsTypeVoid (Indirect (Expr3.Type))) {
                ResultType = NewPointerTo (Indirect (Expr3.Type));
                ResultType[1].C |= GetQualifier (Indirect (Expr2.Type));
            } else {
                /* Must point to compatible types */
                if (TypeCmp (Expr2.Type, Expr3.Type).C < TC_VOID_PTR) {
                    TypeCompatibilityDiagnostic (Expr2.Type, Expr3.Type,
                        1, "Incompatible pointer types in ternary: '%s' and '%s'");
                    /* Avoid further errors */
                    ResultType = NewPointerTo (type_void);
                } else {
                    /* Result has the properly qualified composite type */
                    ResultType = TypeDup (Expr2.Type);
                    TypeComposition (ResultType, Expr3.Type);
                    ResultType[1].C |= GetQualifier (Indirect (Expr3.Type));
                }
            }
        } else if (IsClassPtr (Expr2.Type) && Expr3IsNULL) {
            /* Result type is pointer, no cast needed */
            ResultType = TypeDup (Expr2.Type);
        } else if (Expr2IsNULL && IsClassPtr (Expr3.Type)) {
            /* Result type is pointer, no cast needed */
            ResultType = TypeDup (Expr3.Type);
        } else if (IsTypeVoid (Expr2.Type) && IsTypeVoid (Expr3.Type)) {
            /* Result type is void */
            ResultType = TypeDup (type_void);
        } else {
            if (IsClassStruct (Expr2.Type) && IsClassStruct (Expr3.Type) &&
                TypeCmp (Expr2.Type, Expr3.Type).C == TC_IDENTICAL) {
                /* Result type is struct/union */
                ResultType = TypeDup (Expr2.Type);
            } else {
                TypeCompatibilityDiagnostic (Expr2.Type, Expr3.Type, 1,
                    "Incompatible types in ternary '%s' with '%s'");
                ResultType = TypeDup (Expr2.Type);      /* Doesn't matter here */
            }
        }

        if (!ConstantCond) {
            /* Define the final label */
            g_defcodelabel (TrueLab);
            /* Set up the result expression type */
            ED_FinalizeRValLoad (Expr);
            /* Restore the original evaluation flags */
            Expr->Flags = (Expr->Flags & ~E_MASK_KEEP_RESULT) | Flags;
        } else {
            if (Expr->IVal != 0) {
                *Expr = Expr2;
            } else {
                *Expr = Expr3;
            }
            /* The result expression is always an rvalue */
            ED_MarkExprAsRVal (Expr);
        }

        /* Setup the target expression */
        Expr->Type = ResultType;

        /* Propagate viral flags */
        if ((Expr2.Flags & E_EVAL_UNEVAL) != E_EVAL_UNEVAL) {
            ED_PropagateFrom (Expr, &Expr2);
        }
        if ((Expr3.Flags & E_EVAL_UNEVAL) != E_EVAL_UNEVAL) {
            ED_PropagateFrom (Expr, &Expr3);
        }
    }
}



void hie1 (ExprDesc* Expr)
/* Parse first level of expression hierarchy. */
{
    hieQuest (Expr);
    switch (CurTok.Tok) {

        case TOK_ASSIGN:
            OpAssign (0, Expr, "=");
            break;

        case TOK_PLUS_ASSIGN:
            OpAddSubAssign (&GenPASGN, Expr, "+=");
            break;

        case TOK_MINUS_ASSIGN:
            OpAddSubAssign (&GenSASGN, Expr, "-=");
            break;

        case TOK_MUL_ASSIGN:
            OpAssign (&GenMASGN, Expr, "*=");
            break;

        case TOK_DIV_ASSIGN:
            OpAssign (&GenDASGN, Expr, "/=");
            break;

        case TOK_MOD_ASSIGN:
            OpAssign (&GenMOASGN, Expr, "%=");
            break;

        case TOK_SHL_ASSIGN:
            OpAssign (&GenSLASGN, Expr, "<<=");
            break;

        case TOK_SHR_ASSIGN:
            OpAssign (&GenSRASGN, Expr, ">>=");
            break;

        case TOK_AND_ASSIGN:
            OpAssign (&GenAASGN, Expr, "&=");
            break;

        case TOK_XOR_ASSIGN:
            OpAssign (&GenXOASGN, Expr, "^=");
            break;

        case TOK_OR_ASSIGN:
            OpAssign (&GenOASGN, Expr, "|=");
            break;

        default:
            break;
    }
}



void hie0 (ExprDesc *Expr)
/* Parse comma operator. */
{
    unsigned Flags = Expr->Flags & E_MASK_KEEP_MAKE;
    unsigned PrevErrorCount = ErrorCount;
    CodeMark Start, End;

    /* Remember the current code position */
    GetCodePos (&Start);

    hie1 (Expr);
    while (CurTok.Tok == TOK_COMMA) {
        /* Append deferred inc/dec at sequence point */
        DoDeferred (SQP_KEEP_NONE, Expr);

        /* If the expression has no observable effect and isn't cast to type
        ** void, emit a warning and remove useless code if any.
        */
        GetCodePos (&End);
        if (CodeRangeIsEmpty (&Start, &End) ||
            (Expr->Flags & E_SIDE_EFFECTS) == 0) {

            if (!ED_MayHaveNoEffect (Expr)  &&
                IS_Get (&WarnNoEffect)      &&
                PrevErrorCount == ErrorCount) {
                Warning ("Left-hand operand of comma expression has no effect");
            }

            /* Remove code with no effect */
            RemoveCode (&Start);
        }

        PrevErrorCount = ErrorCount;

        /* Remember the current code position */
        GetCodePos (&Start);

        /* Keep viral flags propagated from subexpressions */
        Flags |= Expr->Flags & E_MASK_VIRAL;

        /* Reset the expression */
        ED_Init (Expr);
        Expr->Flags = Flags & ~E_MASK_VIRAL;
        NextToken ();
        hie1 (Expr);

        /* Propagate viral flags */
        Expr->Flags |= Flags & E_MASK_VIRAL;
    }
}



void Expression0 (ExprDesc* Expr)
/* Evaluate an expression via hie0 and put the result into the primary register.
** The expression is completely evaluated and all side effects complete.
*/
{
    unsigned Flags = Expr->Flags & E_MASK_KEEP_RESULT;

    /* Only check further after the expression is evaluated */
    ExprWithCheck (hie0, Expr);

    if ((Expr->Flags & Flags & E_MASK_EVAL) != (Flags & E_MASK_EVAL)) {
        Internal ("Expression flags tampered: %08X", Flags);
    }

    if (ED_YetToLoad (Expr)) {
        LoadExpr (CF_NONE, Expr);
    }

    /* Append deferred inc/dec at sequence point */
    DoDeferred (SQP_KEEP_EXPR, Expr);
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
        Error ("Scalar expression expected");
        /* To avoid any compiler errors, make the expression a valid int */
        ED_MakeConstBool (Expr, 1);
    }
}



ExprDesc NoCodeConstExpr (void (*Func) (ExprDesc*))
/* Get an expression evaluated via the given function. If the result is not a
** constant expression without runtime code generated, a diagnostic will be
** printed, and the value is replaced by a constant one to make sure there are
** no internal errors that result from this input error.
*/
{
    ExprDesc Expr;
    ED_Init (&Expr);

    Expr.Flags |= E_EVAL_C_CONST;
    MarkedExprWithCheck (Func, &Expr);
    if (!ED_IsConst (&Expr) || !ED_CodeRangeIsEmpty (&Expr)) {
        Error ("Constant expression expected");
        /* To avoid any compiler errors, make the expression a valid const */
        Expr.Flags &= E_MASK_RTYPE | E_MASK_KEEP_MAKE;
        Expr.Flags |= E_LOC_NONE;

        /* Remove any non-constant code generated */
        if (!ED_CodeRangeIsEmpty (&Expr)) {
            RemoveCodeRange (&Expr.Start, &Expr.End);
        }
    }

    /* Return by value */
    return Expr;
}



ExprDesc NoCodeConstAbsIntExpr (void (*Func) (ExprDesc*))
/* Get an expression evaluated via the given function. If the result is not a
** constant numeric integer value without runtime code generated, a diagnostic
** will be printed, and the value is replaced by a constant one to make sure
** there are no internal errors that result from this input error.
*/
{
    ExprDesc Expr;
    ED_Init (&Expr);

    Expr.Flags |= E_EVAL_C_CONST;
    MarkedExprWithCheck (Func, &Expr);
    if (!ED_IsConstAbsInt (&Expr) || !ED_CodeRangeIsEmpty (&Expr)) {
        Error ("Constant integer expression expected");
        /* To avoid any compiler errors, make the expression a valid const */
        ED_MakeConstAbsInt (&Expr, 1);

        /* Remove any non-constant code generated */
        if (!ED_CodeRangeIsEmpty (&Expr)) {
            RemoveCodeRange (&Expr.Start, &Expr.End);
        }
    }

    /* Return by value */
    return Expr;
}
