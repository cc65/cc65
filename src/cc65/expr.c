/* expr.c
 *
 * Ullrich von Bassewitz, 21.06.1998
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
#include "macrotab.h"
#include "preproc.h"
#include "scanner.h"
#include "stdfunc.h"
#include "symtab.h"
#include "typecmp.h"
#include "typeconv.h"
#include "expr.h"



/*****************************************************************************/
/*				     Data				     */
/*****************************************************************************/



/* Generator attributes */
#define GEN_NOPUSH	0x01		/* Don't push lhs */

/* Map a generator function and its attributes to a token */
typedef struct {
    token_t       Tok;	     		/* Token to map to */
    unsigned      Flags;  		/* Flags for generator function */
    void       	  (*Func) (unsigned, unsigned long); 	/* Generator func */
} GenDesc;

/* Descriptors for the operations */
static GenDesc GenPASGN  = { TOK_PLUS_ASSIGN,	GEN_NOPUSH,     g_add };
static GenDesc GenSASGN  = { TOK_MINUS_ASSIGN,  GEN_NOPUSH,     g_sub };
static GenDesc GenMASGN  = { TOK_MUL_ASSIGN,	GEN_NOPUSH,     g_mul };
static GenDesc GenDASGN  = { TOK_DIV_ASSIGN,	GEN_NOPUSH,     g_div };
static GenDesc GenMOASGN = { TOK_MOD_ASSIGN,	GEN_NOPUSH,     g_mod };
static GenDesc GenSLASGN = { TOK_SHL_ASSIGN,	GEN_NOPUSH,     g_asl };
static GenDesc GenSRASGN = { TOK_SHR_ASSIGN,	GEN_NOPUSH,     g_asr };
static GenDesc GenAASGN  = { TOK_AND_ASSIGN,	GEN_NOPUSH,     g_and };
static GenDesc GenXOASGN = { TOK_XOR_ASSIGN,	GEN_NOPUSH,     g_xor };
static GenDesc GenOASGN  = { TOK_OR_ASSIGN,	GEN_NOPUSH,     g_or  };



/*****************************************************************************/
/*			       Function forwards			     */
/*****************************************************************************/



void hie0 (ExprDesc *lval);
/* Parse comma operator. */

void expr (void (*Func) (ExprDesc*), ExprDesc *Expr);
/* Expression parser; func is either hie0 or hie1. */



/*****************************************************************************/
/*			       Helper functions				     */
/*****************************************************************************/



static unsigned GlobalModeFlags (unsigned flags)
/* Return the addressing mode flags for the variable with the given flags */
{
    flags &= E_MCTYPE;
    if (flags == E_TGLAB) {
	/* External linkage */
	return CF_EXTERNAL;
    } else if (flags == E_TREGISTER) {
	/* Register variable */
	return CF_REGVAR;
    } else {
	/* Static */
	return CF_STATIC;
    }
}



static int IsNullPtr (ExprDesc* lval)
/* Return true if this is the NULL pointer constant */
{
    return (IsClassInt (lval->Type) && 	        /* Is it an int? */
       	    lval->Flags == E_MCONST && 	        /* Is it constant? */
	    lval->ConstVal == 0);		/* And is it's value zero? */
}



static type* promoteint (type* lhst, type* rhst)
/* In an expression with two ints, return the type of the result */
{
    /* Rules for integer types:
     *   - If one of the values is a long, the result is long.
     *   - If one of the values	is unsigned, the result is also unsigned.
     *   - Otherwise the result is an int.
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
 * to be constant, rhs is expected to be in the primary register or constant.
 * The function will put the type of the result into lhs and return the
 * code generator flags for the operation.
 * If NoPush is given, it is assumed that the operation does not expect the lhs
 * to be on stack, and that lhs is in a register instead.
 * Beware: The function does only accept int types.
 */
{
    unsigned ltype, rtype;
    unsigned flags;

    /* Get the type strings */
    type* lhst = lhs->Type;
    type* rhst = rhs->Type;

    /* Generate type adjustment code if needed */
    ltype = TypeOf (lhst);
    if (lhs->Flags == E_MCONST) {
	ltype |= CF_CONST;
    }
    if (NoPush) {
	/* Value is in primary register*/
     	ltype |= CF_REG;
    }
    rtype = TypeOf (rhst);
    if (rhs->Flags == E_MCONST) {
	rtype |= CF_CONST;
    }
    flags = g_typeadjust (ltype, rtype);

    /* Set the type of the result */
    lhs->Type = promoteint (lhst, rhst);

    /* Return the code generator flags */
    return flags;
}



void DefineData (ExprDesc* Expr)
/* Output a data definition for the given expression */
{
    unsigned Flags = Expr->Flags;

    switch (Flags & E_MCTYPE) {

       	case E_TCONST:
	    /* Number */
	    g_defdata (TypeOf (Expr->Type) | CF_CONST, Expr->ConstVal, 0);
       	    break;

	case E_TREGISTER:
	    /* Register variable. Taking the address is usually not
	     * allowed.
	     */
	    if (IS_Get (&AllowRegVarAddr) == 0) {
	     	Error ("Cannot take the address of a register variable");
	    }
	    /* FALLTHROUGH */

       	case E_TGLAB:
        case E_TLLAB:
       	    /* Local or global symbol */
	    g_defdata (GlobalModeFlags (Flags), Expr->Name, Expr->ConstVal);
	    break;

       	case E_TLIT:
	    /* a literal of some kind */
       	    g_defdata (CF_STATIC, LiteralPoolLabel, Expr->ConstVal);
       	    break;

       	default:
	    Internal ("Unknown constant type: %04X", Flags);
    }
}



static void LoadConstant (unsigned Flags, ExprDesc* Expr)
/* Load the primary register with some constant value. */
{
    switch (Expr->Flags & E_MCTYPE) {

    	case E_TLOFFS:
       	    g_leasp (Expr->ConstVal);
	    break;

       	case E_TCONST:
	    /* Number constant */
       	    g_getimmed (Flags | TypeOf (Expr->Type) | CF_CONST, Expr->ConstVal, 0);
       	    break;

	case E_TREGISTER:
	    /* Register variable. Taking the address is usually not
	     * allowed.
	     */
	    if (IS_Get (&AllowRegVarAddr) == 0) {
	     	Error ("Cannot take the address of a register variable");
	    }
	    /* FALLTHROUGH */

       	case E_TGLAB:
        case E_TLLAB:
       	    /* Local or global symbol, load address */
	    Flags |= GlobalModeFlags (Expr->Flags);
	    Flags &= ~CF_CONST;
	    g_getimmed (Flags, Expr->Name, Expr->ConstVal);
       	    break;

       	case E_TLIT:
	    /* Literal string */
	    g_getimmed (CF_STATIC, LiteralPoolLabel, Expr->ConstVal);
       	    break;

       	default:
	    Internal ("Unknown constant type: %04X", Expr->Flags);
    }
}



static int kcalc (token_t tok, long val1, long val2)
/* Calculate an operation with left and right operand constant. */
{
    switch (tok) {
       	case TOK_EQ:
  	    return (val1 == val2);
       	case TOK_NE:
	    return (val1 != val2);
       	case TOK_LT:
	    return (val1 < val2);
       	case TOK_LE:
	    return (val1 <= val2);
       	case TOK_GE:
	    return (val1 >= val2);
       	case TOK_GT:
	    return (val1 > val2);
       	case TOK_OR:
  	    return (val1 | val2);
       	case TOK_XOR:
  	    return (val1 ^ val2);
       	case TOK_AND:
  	    return (val1 & val2);
       	case TOK_SHR:
  	    return (val1 >> val2);
       	case TOK_SHL:
  	    return (val1 << val2);
       	case TOK_STAR:
  	    return (val1 * val2);
       	case TOK_DIV:
	    if (val2 == 0) {
	   	Error ("Division by zero");
	   	return 0x7FFFFFFF;
	    }
  	    return (val1 / val2);
       	case TOK_MOD:
	    if (val2 == 0) {
	   	Error ("Modulo operation with zero");
	       	return 0;
	    }
  	    return (val1 % val2);
  	default:
  	    Internal ("kcalc: got token 0x%X\n", tok);
  	    return 0;
    }
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
 * in hie10).
 */
{
    SymEntry* Entry;

    /* There's a type waiting if:
     *
     *   1.    We have an opening paren, and
     *     a.  the next token is a type, or
     *     b.  the next token is a type qualifier, or
     *     c.  the next token is a typedef'd type
     */
    return CurTok.Tok == TOK_LPAREN && (
       	   TokIsType (&NextTok)                         ||
           TokIsTypeQual (&NextTok)                     ||
       	   (NextTok.Tok  == TOK_IDENT 			&&
	   (Entry = FindSym (NextTok.Ident)) != 0  	&&
	   SymIsTypeDef (Entry)));
}



void PushAddr (ExprDesc* Expr)
/* If the expression contains an address that was somehow evaluated,
 * push this address on the stack. This is a helper function for all
 * sorts of implicit or explicit assignment functions where the lvalue
 * must be saved if it's not constant, before evaluating the rhs.
 */
{
    /* Get the address on stack if needed */
    if (Expr->Flags != E_MREG && (Expr->Flags & E_MEXPR)) {
     	/* Push the address (always a pointer) */
     	g_push (CF_PTR, 0);
    }
}



void ConstSubExpr (void (*Func) (ExprDesc*), ExprDesc* Expr)
/* Will evaluate an expression via the given function. If the result is not
 * a constant, a diagnostic will be printed, and the value is replaced by
 * a constant one to make sure there are no internal errors that result
 * from this input error.
 */
{
    Func (InitExprDesc (Expr));
    if (ED_IsLVal (Expr) != 0 || Expr->Flags != E_MCONST) {
       	Error ("Constant expression expected");
       	/* To avoid any compiler errors, make the expression a valid const */
     	ED_MakeConstInt (Expr, 1);
    }
}



void CheckBoolExpr (ExprDesc* Expr)
/* Check if the given expression is a boolean expression, output a diagnostic
 * if not.
 */
{
    /* If it's an integer, it's ok. If it's not an integer, but a pointer,
     * the pointer used in a boolean context is also ok
     */
    if (!IsClassInt (Expr->Type) && !IsClassPtr (Expr->Type)) {
 	Error ("Boolean expression expected");
 	/* To avoid any compiler errors, make the expression a valid int */
	ED_MakeConstInt (Expr, 1);
    }
}



/*****************************************************************************/
/*   	     	     		     code				     */
/*****************************************************************************/



void ExprLoad (unsigned Flags, ExprDesc* Expr)
/* Place the result of an expression into the primary register if it is not
 * already there.
 */
{
    int f;

    f = Expr->Flags;
    if (ED_IsLVal (Expr)) {
       	/* Dereferenced lvalue */
       	Flags |= TypeOf (Expr->Type);
     	if (Expr->Test & E_FORCETEST) {
     	    Flags |= CF_TEST;
     	    Expr->Test &= ~E_FORCETEST;
     	}
       	if (f & E_MGLOBAL) {
     	    /* Reference to a global variable */
     	    Flags |= GlobalModeFlags (f);
       	    g_getstatic (Flags, Expr->Name, Expr->ConstVal);
       	} else if (f & E_MLOCAL) {
     	    /* Reference to a local variable */
       	    g_getlocal (Flags, Expr->ConstVal);
     	} else if (f & E_MCONST) {
     	    /* Reference to an absolute address */
     	    g_getstatic (Flags | CF_ABSOLUTE, Expr->ConstVal, 0);
     	} else if (f == E_MEOFFS) {
     	    /* Reference to address in primary with offset in Expr */
     	    g_getind (Flags, Expr->ConstVal);
     	} else if (f != E_MREG) {
     	    /* Reference with address in primary */
     	    g_getind (Flags, 0);
     	} else if (Flags & CF_TEST) {
            /* The value is already in the primary but needs a test */
            g_test (Flags);
        }
    } else {
     	/* An rvalue */
     	if (f == E_MEOFFS) {
     	    /* reference not storable */
     	    Flags |= TypeOf (Expr->Type);
     	    g_inc (Flags | CF_CONST, Expr->ConstVal);
     	} else if ((f & E_MEXPR) == 0) {
     	    /* Constant of some sort, load it into the primary */
     	    LoadConstant (Flags, Expr);
     	}

        /* Are we testing this value? */
        if (Expr->Test & E_FORCETEST) {
            /* Yes, force a test */
            Flags |= TypeOf (Expr->Type);
            g_test (Flags);
            Expr->Test &= ~E_FORCETEST;
        }
    }
}



static unsigned FunctionParamList (FuncDesc* Func)
/* Parse a function parameter list and pass the parameters to the called
 * function. Depending on several criteria this may be done by just pushing
 * each parameter separately, or creating the parameter frame once and then
 * storing into this frame.
 * The function returns the size of the parameters pushed.
 */
{
    ExprDesc Expr;

    /* Initialize variables */
    SymEntry* Param    	  = 0;	/* Keep gcc silent */
    unsigned  ParamSize   = 0;	/* Size of parameters pushed */
    unsigned  ParamCount  = 0;	/* Number of parameters pushed */
    unsigned  FrameSize   = 0;	/* Size of parameter frame */
    unsigned  FrameParams = 0;	/* Number of params in frame */
    int       FrameOffs   = 0;	/* Offset into parameter frame */
    int	      Ellipsis    = 0;	/* Function is variadic */

    /* As an optimization, we may allocate the complete parameter frame at
     * once instead of pushing each parameter as it comes. We may do that,
     * if...
     *
     *  - optimizations that increase code size are enabled (allocating the
     *    stack frame at once gives usually larger code).
     *  - we have more than one parameter to push (don't count the last param
     *    for __fastcall__ functions).
     *
     * The FrameSize variable will contain a value > 0 if storing into a frame
     * (instead of pushing) is enabled.
     *
     */
    if (CodeSizeFactor >= 200) {

	/* Calculate the number and size of the parameters */
	FrameParams = Func->ParamCount;
	FrameSize   = Func->ParamSize;
	if (FrameParams > 0 && (Func->Flags & FD_FASTCALL) != 0) {
	    /* Last parameter is not pushed */
	    FrameSize -= CheckedSizeOf (Func->LastParam->Type);
	    --FrameParams;
	}

	/* Do we have more than one parameter in the frame? */
	if (FrameParams > 1) {
	    /* Okeydokey, setup the frame */
	    FrameOffs = oursp;
	    g_space (FrameSize);
	    oursp -= FrameSize;
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
	     * cannot go into the same symbol table, which means that in this
	     * case of errorneous input, the number of nodes in the symbol
	     * table and ParamCount are NOT equal. We have to handle this case
	     * below to avoid segmentation violations. Since we know that this
	     * problem can only occur if there is more than one parameter,
	     * we will just use the last one.
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
	     * message for each other argument.
	     */
	    Ellipsis = 1;
     	}

        /* Evaluate the parameter expression */
        hie1 (InitExprDesc (&Expr));

      	/* If we don't have an argument spec, accept anything, otherwise
	 * convert the actual argument to the type needed.
	 */
        Flags = CF_NONE;
       	if (!Ellipsis) {
	    /* Convert the argument to the parameter type if needed */
            TypeConversion (&Expr, Param->Type);

	    /* If we have a prototype, chars may be pushed as chars */
	    Flags |= CF_FORCECHAR;
       	}

        /* Load the value into the primary if it is not already there */
        ExprLoad (Flags, &Expr);

	/* Use the type of the argument for the push */
       	Flags |= TypeOf (Expr.Type);

	/* If this is a fastcall function, don't push the last argument */
       	if (ParamCount != Func->ParamCount || (Func->Flags & FD_FASTCALL) == 0) {
	    unsigned ArgSize = sizeofarg (Flags);
	    if (FrameSize > 0) {
	    	/* We have the space already allocated, store in the frame.
                 * Because of invalid type conversions (that have produced an
                 * error before), we can end up here with a non aligned stack
                 * frame. Since no output will be generated anyway, handle
                 * these cases gracefully instead of doing a CHECK.
                 */
                if (FrameSize >= ArgSize) {
                    FrameSize -= ArgSize;
                } else {
                    FrameSize = 0;
                }
	    	FrameOffs -= ArgSize;
	    	/* Store */
	      	g_putlocal (Flags | CF_NOKEEP, FrameOffs, Expr.ConstVal);
	    } else {
	    	/* Push the argument */
	    	g_push (Flags, Expr.ConstVal);
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
     * However, if there are parameters missing (which is an error and was
     * flagged by the compiler) AND a stack frame was preallocated above,
     * we would loose track of the stackpointer and generate an internal error
     * later. So we correct the value by the parameters that should have been
     * pushed to avoid an internal compiler error. Since an error was
     * generated before, no code will be output anyway.
     */
    return ParamSize + FrameSize;
}



static void FunctionCall (ExprDesc* Expr)
/* Perform a function call. */
{
    FuncDesc*	  Func;	       	  /* Function descriptor */
    int           IsFuncPtr;      /* Flag */
    unsigned 	  ParamSize;	  /* Number of parameter bytes */
    CodeMark 	  Mark = 0;       /* Initialize to keep gcc silent */
    int           PtrOffs = 0;    /* Offset of function pointer on stack */
    int           IsFastCall = 0; /* True if it's a fast call function */
    int           PtrOnStack = 0; /* True if a pointer copy is on stack */

    /* Skip the left paren */
    NextToken ();

    /* Get a pointer to the function descriptor from the type string */
    Func = GetFuncDesc (Expr->Type);

    /* Handle function pointers transparently */
    IsFuncPtr = IsTypeFuncPtr (Expr->Type);
    if (IsFuncPtr) {

	/* Check wether it's a fastcall function that has parameters */
	IsFastCall = IsFastCallFunc (Expr->Type + 1) && (Func->ParamCount > 0);

	/* Things may be difficult, depending on where the function pointer
	 * resides. If the function pointer is an expression of some sort
	 * (not a local or global variable), we have to evaluate this
	 * expression now and save the result for later. Since calls to
	 * function pointers may be nested, we must save it onto the stack.
	 * For fastcall functions we do also need to place a copy of the
	 * pointer on stack, since we cannot use a/x.
	 */
	PtrOnStack = IsFastCall || ((Expr->Flags & (E_MGLOBAL | E_MLOCAL)) == 0);
	if (PtrOnStack) {

	    /* Not a global or local variable, or a fastcall function. Load
	     * the pointer into the primary and mark it as an expression.
	     */
       	    ExprLoad (CF_NONE, Expr);
	    Expr->Flags |= E_MEXPR;

	    /* Remember the code position */
	    Mark = GetCodePos ();

	    /* Push the pointer onto the stack and remember the offset */
	    g_push (CF_PTR, 0);
	    PtrOffs = oursp;
	}

    /* Check for known standard functions and inline them if requested */
    } else if (IS_Get (&InlineStdFuncs) && IsStdFunc ((const char*) Expr->Name)) {

	/* Inline this function */
       	HandleStdFunc (Func, Expr);
       	return;

    }

    /* Parse the parameter list */
    ParamSize = FunctionParamList (Func);

    /* We need the closing paren here */
    ConsumeRParen ();

    /* Special handling for function pointers */
    if (IsFuncPtr) {

	/* If the function is not a fastcall function, load the pointer to
	 * the function into the primary.
	 */
	if (!IsFastCall) {

	    /* Not a fastcall function - we may use the primary */
       	    if (PtrOnStack) {
	    	/* If we have no parameters, the pointer is still in the
	    	 * primary. Remove the code to push it and correct the
	    	 * stack pointer.
	    	 */
	    	if (ParamSize == 0) {
	    	    RemoveCode (Mark);
	    	    pop (CF_PTR);
	    	    PtrOnStack = 0;
	    	} else {
	    	    /* Load from the saved copy */
	    	    g_getlocal (CF_PTR, PtrOffs);
	    	}
	    } else {
	     	/* Load from original location */
	     	ExprLoad (CF_NONE, Expr);
	    }

	    /* Call the function */
	    g_callind (TypeOf (Expr->Type+1), ParamSize, PtrOffs);

     	} else {

	    /* Fastcall function. We cannot use the primary for the function
	     * pointer and must therefore use an offset to the stack location.
	     * Since fastcall functions may never be variadic, we can use the
	     * index register for this purpose.
	     */
	    g_callind (CF_LOCAL, ParamSize, PtrOffs);
	}

	/* If we have a pointer on stack, remove it */
	if (PtrOnStack) {
	    g_space (- (int) sizeofarg (CF_PTR));
	    pop (CF_PTR);
	}

	/* Skip T_PTR */
    	++Expr->Type;

    } else {

	/* Normal function */
       	g_call (TypeOf (Expr->Type), (const char*) Expr->Name, ParamSize);

    }
}



static void Primary (ExprDesc* E)
/* This is the lowest level of the expression parser. */
{
    SymEntry* Sym;

    /* Initialize fields in the expression stucture */
    E->Test = 0;        /* No test */
    E->Sym  = 0;        /* Symbol unknown */

    /* Character and integer constants. */
    if (CurTok.Tok == TOK_ICONST || CurTok.Tok == TOK_CCONST) {
    	E->Flags = E_MCONST | E_TCONST | E_RVAL;
    	E->Type  = CurTok.Type;
    	E->ConstVal = CurTok.IVal;
    	NextToken ();
    	return;
    }

    /* Process parenthesized subexpression by calling the whole parser
     * recursively.
     */
    if (CurTok.Tok == TOK_LPAREN) {
    	NextToken ();
        hie0 (InitExprDesc (E));
       	ConsumeRParen ();
    	return;
    }

    /* If we run into an identifier in preprocessing mode, we assume that this
     * is an undefined macro and replace it by a constant value of zero.
     */
    if (Preprocessing && CurTok.Tok == TOK_IDENT) {
        ED_MakeConstInt (E, 0);
        return;
    }

    /* All others may only be used if the expression evaluation is not called
     * recursively by the preprocessor.
     */
    if (Preprocessing) {
       	/* Illegal expression in PP mode */
	Error ("Preprocessor expression expected");
	ED_MakeConstInt (E, 1);
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

                /* The expression type is the symbol type */
                E->Type = Sym->Type;

                /* Check for illegal symbol types */
                CHECK ((Sym->Flags & SC_LABEL) != SC_LABEL);
                if (Sym->Flags & SC_TYPE) {
                    /* Cannot use type symbols */
                    Error ("Variable identifier expected");
                    /* Assume an int type to make E valid */
                    E->Flags = E_MLOCAL | E_TLOFFS | E_LVAL;
                    E->Type  = type_int;
                    E->ConstVal = 0;
                    return;
                }

                /* Mark the symbol as referenced */
                Sym->Flags |= SC_REF;

                /* Check for legal symbol types */
                if ((Sym->Flags & SC_CONST) == SC_CONST) {
                    /* Enum or some other numeric constant */
                    E->Flags = E_MCONST | E_TCONST | E_RVAL;
                    E->ConstVal = Sym->V.ConstVal;
                } else if ((Sym->Flags & SC_FUNC) == SC_FUNC) {
                    /* Function */
                    E->Flags = E_MGLOBAL | E_MCONST | E_TGLAB | E_RVAL;
                    E->Name = (unsigned long) Sym->Name;
                    E->ConstVal = 0;
                } else if ((Sym->Flags & SC_AUTO) == SC_AUTO) {
                    /* Local variable. If this is a parameter for a variadic
                     * function, we have to add some address calculations, and the
                     * address is not const.
                     */
                    if ((Sym->Flags & SC_PARAM) == SC_PARAM && F_IsVariadic (CurrentFunc)) {
                        /* Variadic parameter */
                        g_leavariadic (Sym->V.Offs - F_GetParamSize (CurrentFunc));
                        E->Flags = E_MEXPR | E_LVAL;
                        E->ConstVal = 0;
                    } else {
                        /* Normal parameter */
                        E->Flags = E_MLOCAL | E_TLOFFS | E_LVAL;
                        E->ConstVal = Sym->V.Offs;
                    }
                } else if ((Sym->Flags & SC_REGISTER) == SC_REGISTER) {
                    /* Register variable, zero page based */
                    E->Flags = E_MGLOBAL | E_MCONST | E_TREGISTER | E_LVAL;
                    E->Name  = Sym->V.R.RegOffs;
                    E->ConstVal = 0;
                } else if ((Sym->Flags & SC_STATIC) == SC_STATIC) {
                    /* Static variable */
                    if (Sym->Flags & (SC_EXTERN | SC_STORAGE)) {
                        E->Flags = E_MGLOBAL | E_MCONST | E_TGLAB | E_LVAL;
                        E->Name = (unsigned long) Sym->Name;
                    } else {
                        E->Flags = E_MGLOBAL | E_MCONST | E_TLLAB | E_LVAL;
                        E->Name = Sym->V.Label;
                    }
                    E->ConstVal = 0;
                } else {
                    /* Local static variable */
                    E->Flags = E_MGLOBAL | E_MCONST | E_TLLAB | E_LVAL;
                    E->Name  = Sym->V.Offs;
                    E->ConstVal = 0;
                }

                /* The following should not be necessary if the reference flag is
                 * set right above, but currently I do not oversee if it's really
                 * needed and the old code did it.
                 * ###
                 */
                ED_SetValType (E, !IsTypeFunc (E->Type) && !IsTypeArray (E->Type));

            } else {

                /* We did not find the symbol. Remember the name, then skip it */
                ident Ident;
                strcpy (Ident, CurTok.Ident);
                NextToken ();

                /* IDENT is either an auto-declared function or an undefined variable. */
                if (CurTok.Tok == TOK_LPAREN) {
                    /* Declare a function returning int. For that purpose, prepare a
                     * function signature for a function having an empty param list
                     * and returning int.
                     */
                    Warning ("Function call without a prototype");
                    Sym = AddGlobalSym (Ident, GetImplicitFuncType(), SC_EXTERN | SC_REF | SC_FUNC);
                    E->Type  = Sym->Type;
                    E->Flags = E_MGLOBAL | E_MCONST | E_TGLAB | E_RVAL;
                    E->Name  = (unsigned long) Sym->Name;
                    E->ConstVal = 0;
                } else {
                    /* Undeclared Variable */
                    Sym = AddLocalSym (Ident, type_int, SC_AUTO | SC_REF, 0);
                    E->Flags = E_MLOCAL | E_TLOFFS | E_LVAL;
                    E->Type = type_int;
                    E->ConstVal = 0;
                    Error ("Undefined symbol: `%s'", Ident);
                }

            }
            break;

        case TOK_SCONST:
            /* String literal */
            E->Flags = E_MCONST | E_TLIT | E_RVAL;
            E->ConstVal = CurTok.IVal;
            E->Type  = GetCharArrayType (GetLiteralPoolOffs () - CurTok.IVal);
            NextToken ();
            break;

        case TOK_ASM:
            /* ASM statement */
            AsmStatement ();
            E->Flags = E_MEXPR | E_RVAL;
            E->ConstVal = 0;
            E->Type  = type_void;
            break;

        case TOK_AX:
        case TOK_EAX:
            /* __AX__ and __EAX__ pseudo values */
            E->Type  = (CurTok.Tok == TOK_AX)? type_uint : type_ulong;
            E->Flags = E_MREG | E_LVAL;      /* May be used as lvalue */
            E->Test &= ~E_CC;
            E->ConstVal = 0;
            NextToken ();
            break;

        default:
            /* Illegal primary. */
            Error ("Expression expected");
            ED_MakeConstInt (E, 1);
            break;
    }
}



static void ArrayRef (ExprDesc* Expr)
/* Handle an array reference */
{
    unsigned lflags;
    unsigned rflags;
    int ConstBaseAddr;
    int ConstSubAddr;
    ExprDesc lval2;
    CodeMark Mark1;
    CodeMark Mark2;
    type* tptr1;
    type* tptr2;


    /* Skip the bracket */
    NextToken ();

    /* Get the type of left side */
    tptr1 = Expr->Type;

    /* We can apply a special treatment for arrays that have a const base
     * address. This is true for most arrays and will produce a lot better
     * code. Check if this is a const base address.
     */
    lflags = Expr->Flags & ~E_MCTYPE;
    ConstBaseAddr = (lflags == E_MCONST)       || /* Constant numeric address */
       	       	     (lflags & E_MGLOBAL) != 0 || /* Static array, or ... */
       	       	     lflags == E_MLOCAL;     	  /* Local array */

    /* If we have a constant base, we delay the address fetch */
    Mark1 = GetCodePos ();
    Mark2 = 0;	       	/* Silence gcc */
    if (!ConstBaseAddr) {
    	/* Get a pointer to the array into the primary */
    	ExprLoad (CF_NONE, Expr);

    	/* Get the array pointer on stack. Do not push more than 16
    	 * bit, even if this value is greater, since we cannot handle
    	 * other than 16bit stuff when doing indexing.
    	 */
    	Mark2 = GetCodePos ();
    	g_push (CF_PTR, 0);
    }

    /* TOS now contains ptr to array elements. Get the subscript. */
    hie0 (&lval2);
    if (ED_IsRVal (&lval2) && lval2.Flags == E_MCONST) {

     	/* The array subscript is a constant - remove value from stack */
    	if (!ConstBaseAddr) {
     	    RemoveCode (Mark2);
     	    pop (CF_PTR);
      	} else {
    	    /* Get an array pointer into the primary */
    	    ExprLoad (CF_NONE, Expr);
    	}

     	if (IsClassPtr (tptr1)) {

     	    /* Scale the subscript value according to element size */
     	    lval2.ConstVal *= CheckedPSizeOf (tptr1);

	    /* Remove code for lhs load */
	    RemoveCode (Mark1);

    	    /* Handle constant base array on stack. Be sure NOT to
    	     * handle pointers the same way, and check for character literals
             * (both won't work).
    	     */
    	    if (IsTypeArray (tptr1) && Expr->Flags != (E_MCONST | E_TLIT) &&
    	       	((Expr->Flags & ~E_MCTYPE) == E_MCONST ||
    	   	(Expr->Flags & ~E_MCTYPE) == E_MLOCAL ||
    	   	(Expr->Flags & E_MGLOBAL) != 0 ||
    	   	(Expr->Flags == E_MEOFFS))) {
    	   	Expr->ConstVal += lval2.ConstVal;

    	    } else {
	   	/* Pointer - load into primary and remember offset */
	   	if ((Expr->Flags & E_MEXPR) == 0 || ED_IsLVal (Expr)) {
	   	    ExprLoad (CF_NONE, Expr);
	   	}
	   	Expr->ConstVal = lval2.ConstVal;
	   	Expr->Flags = E_MEOFFS;
	    }

       	    /* Result is of element type */
	    Expr->Type = Indirect (tptr1);

	    /* Done */
    	    goto end_array;

       	} else if (IsClassPtr (tptr2 = lval2.Type)) {
    	    /* Subscript is pointer, get element type */
    	    lval2.Type = Indirect (tptr2);

    	    /* Scale the rhs value in the primary register */
    	    g_scale (TypeOf (tptr1), CheckedSizeOf (lval2.Type));
    	    /* */
    	    Expr->Type = lval2.Type;
    	} else {
    	    Error ("Cannot subscript");
    	}

    	/* Add the subscript. Since arrays are indexed by integers,
    	 * we will ignore the true type of the subscript here and
    	 * use always an int.
    	 */
    	g_inc (CF_INT | CF_CONST, lval2.ConstVal);

    } else {

    	/* Array subscript is not constant. Load it into the primary */
	Mark2 = GetCodePos ();
        ExprLoad (CF_NONE, &lval2);

	tptr2 = lval2.Type;
	if (IsClassPtr (tptr1)) {

 	    /* Get the element type */
	    Expr->Type = Indirect (tptr1);

       	    /* Indexing is based on int's, so we will just use the integer
	     * portion of the index (which is in (e)ax, so there's no further
	     * action required).
	     */
	    g_scale (CF_INT, CheckedSizeOf (Expr->Type));

	} else if (IsClassPtr (tptr2)) {

	    /* Get the element type */
	    lval2.Type = Indirect (tptr2);

	    /* Get the int value on top. If we go here, we're sure,
	     * both values are 16 bit (the first one was truncated
       	     * if necessary and the second one is a pointer).
	     * Note: If ConstBaseAddr is true, we don't have a value on
	     * stack, so to "swap" both, just push the subscript.
    	     */
	    if (ConstBaseAddr) {
	    	g_push (CF_INT, 0);
	    	ExprLoad (CF_NONE, Expr);
	    	ConstBaseAddr = 0;
	    } else {
	        g_swap (CF_INT);
	    }

	    /* Scale it */
	    g_scale (TypeOf (tptr1), CheckedSizeOf (lval2.Type));
	    Expr->Type = lval2.Type;
	} else {
	    Error ("Cannot subscript");
	}

	/* The offset is now in the primary register. It didn't have a
	 * constant base address for the lhs, the lhs address is already
	 * on stack, and we must add the offset. If the base address was
	 * constant, we call special functions to add the address to the
	 * offset value.
	 */
	if (!ConstBaseAddr) {
	    /* Add the subscript. Both values are int sized. */
	    g_add (CF_INT, 0);
	} else {

	    /* If the subscript has itself a constant address, it is often
	     * a better idea to reverse again the order of the evaluation.
	     * This will generate better code if the subscript is a byte
	     * sized variable. But beware: This is only possible if the
	     * subscript was not scaled, that is, if this was a byte array
	     * or pointer.
	     */
 	    rflags = lval2.Flags & ~E_MCTYPE;
	    ConstSubAddr = (rflags == E_MCONST)       || /* Constant numeric address */
       	       	       	    (rflags & E_MGLOBAL) != 0 || /* Static array, or ... */
	    	    	    rflags == E_MLOCAL;      	 /* Local array */

       	    if (ConstSubAddr && CheckedSizeOf (Expr->Type) == SIZEOF_CHAR) {

	    	type* SavedType;

	    	/* Reverse the order of evaluation */
	    	unsigned flags = (CheckedSizeOf (lval2.Type) == SIZEOF_CHAR)? CF_CHAR : CF_INT;
    	     	RemoveCode (Mark2);

	    	/* Get a pointer to the array into the primary. We have changed
	    	 * Type above but we need the original type to load the
	    	 * address, so restore it temporarily.
	    	 */
	    	SavedType = Expr->Type;
	     	Expr->Type = tptr1;
	    	ExprLoad (CF_NONE, Expr);
	    	Expr->Type = SavedType;

	    	/* Add the variable */
	    	if (rflags == E_MLOCAL) {
	    	    g_addlocal (flags, lval2.ConstVal);
	    	} else {
	   	    flags |= GlobalModeFlags (lval2.Flags);
	    	    g_addstatic (flags, lval2.Name, lval2.ConstVal);
	    	}
	    } else {
	     	if (lflags == E_MCONST) {
	     	    /* Constant numeric address. Just add it */
	     	    g_inc (CF_INT | CF_UNSIGNED, Expr->ConstVal);
	     	} else if (lflags == E_MLOCAL) {
	       	    /* Base address is a local variable address */
	   	    if (IsTypeArray (tptr1)) {
	     	        g_addaddr_local (CF_INT, Expr->ConstVal);
	   	    } else {
	   		g_addlocal (CF_PTR, Expr->ConstVal);
	   	    }
	     	} else {
	     	    /* Base address is a static variable address */
	     	    unsigned flags = CF_INT;
	   	    flags |= GlobalModeFlags (Expr->Flags);
	   	    if (IsTypeArray (tptr1)) {
	     	        g_addaddr_static (flags, Expr->Name, Expr->ConstVal);
	   	    } else {
	   		g_addstatic (flags, Expr->Name, Expr->ConstVal);
	   	    }
	     	}
	    }
	}
    }
    Expr->Flags = E_MEXPR;
end_array:
    ConsumeRBrack ();
    ED_SetValType (Expr, !IsTypeArray (Expr->Type));
}



static void StructRef (ExprDesc* Expr)
/* Process struct field after . or ->. */
{
    ident Ident;
    SymEntry* Field;
    int Flags;

    /* Skip the token and check for an identifier */
    NextToken ();
    if (CurTok.Tok != TOK_IDENT) {
    	Error ("Identifier expected");
    	Expr->Type = type_int;
        return;
    }

    /* Get the symbol table entry and check for a struct field */
    strcpy (Ident, CurTok.Ident);
    NextToken ();
    Field = FindStructField (Expr->Type, Ident);
    if (Field == 0) {
     	Error ("Struct/union has no field named `%s'", Ident);
       	Expr->Type = type_int;
     	return;
    }

    /* If we have constant input data, the result is also constant */
    Flags = (Expr->Flags & ~E_MCTYPE);
    if (Flags == E_MCONST ||
       	(ED_IsRVal (Expr) && (Flags == E_MLOCAL ||
	       	             (Flags & E_MGLOBAL) != 0 ||
	       	              Expr->Flags  == E_MEOFFS))) {
	Expr->ConstVal += Field->V.Offs;
    } else {
	if ((Flags & E_MEXPR) == 0 || ED_IsLVal (Expr)) {
	    ExprLoad (CF_NONE, Expr);
	}
	Expr->ConstVal = Field->V.Offs;
	Expr->Flags = E_MEOFFS;
    }
    Expr->Type = Field->Type;
    ED_SetValType (Expr, !IsTypeArray (Field->Type));
}



static void hie11 (ExprDesc *Expr)
/* Handle compound types (structs and arrays) */
{
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
                if (IsTypeFunc (Expr->Type) || IsTypeFuncPtr (Expr->Type)) {

                    /* Call the function */
                    FunctionCall (Expr);

                    /* Result is in the primary register */
                    Expr->Flags = E_MEXPR | E_RVAL;

                    /* Set to result */
                    Expr->Type = GetFuncReturn (Expr->Type);

                } else {
                    Error ("Illegal function call");
                    ED_MakeRVal (Expr);
                }
                break;

            case TOK_DOT:
                if (!IsClassStruct (Expr->Type)) {
                    Error ("Struct expected");
                }
                ED_MakeRVal (Expr);         /* #### ? */
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



void Store (ExprDesc* Expr, const type* StoreType)
/* Store the primary register into the location denoted by Expr. If StoreType
 * is given, use this type when storing instead of Expr->Type. If StoreType
 * is NULL, use Expr->Type instead.
 */
{
    unsigned Flags;

    unsigned f = Expr->Flags;

    /* If StoreType was not given, use Expr->Type instead */
    if (StoreType == 0) {
        StoreType = Expr->Type;
    }

    /* Get the code generator flags */
    Flags = TypeOf (StoreType);
    if (f & E_MGLOBAL) {
     	Flags |= GlobalModeFlags (f);
     	if (Expr->Test) {
   	    /* Just testing */
       	    Flags |= CF_TEST;
   	}

    	/* Generate code */
       	g_putstatic (Flags, Expr->Name, Expr->ConstVal);

    } else if (f & E_MLOCAL) {
        /* Store an auto variable */
       	g_putlocal (Flags, Expr->ConstVal, 0);
    } else if (f == E_MEOFFS) {
        /* Store indirect with offset */
    	g_putind (Flags, Expr->ConstVal);
    } else if (f != E_MREG) {
    	if (f & E_MEXPR) {
            /* Indirect without offset */
    	    g_putind (Flags, 0);
    	} else {
    	    /* Store into absolute address */
    	    g_putstatic (Flags | CF_ABSOLUTE, Expr->ConstVal, 0);
    	}
    }

    /* Assume that each one of the stores will invalidate CC */
    Expr->Test &= ~E_CC;
}



static void PreIncDec (ExprDesc* Expr, void (*inc) (unsigned, unsigned long))
/* Handle --i and ++i */
{
    unsigned flags;
    unsigned long val;

    /* Skip the operator token */
    NextToken ();

    /* Evaluate the expression and check that it is an lvalue */
    hie10 (Expr);
    if (ED_IsRVal (Expr) == 0) {
    	Error ("Invalid lvalue");
    	return;
    }

    /* Get the data type */
    flags = TypeOf (Expr->Type) | CF_FORCECHAR | CF_CONST;

    /* Get the increment value in bytes */
    val = (Expr->Type[0] == T_PTR)? CheckedPSizeOf (Expr->Type) : 1;

    /* Check for special addressing modes */
    if (Expr->Flags & E_MGLOBAL) {
        /* Global address */
        flags |= GlobalModeFlags (Expr->Flags);
        if (inc == g_inc) {
            g_addeqstatic (flags, Expr->Name, Expr->ConstVal, val);
        } else {
            g_subeqstatic (flags, Expr->Name, Expr->ConstVal, val);
        }
    } else if (Expr->Flags & E_MLOCAL) {
        /* Local address */
        if (inc == g_inc) {
            g_addeqlocal (flags, Expr->ConstVal, val);
        } else {
            g_subeqlocal (flags, Expr->ConstVal, val);
        }
    } else if (Expr->Flags & E_MCONST) {
        /* Constant absolute address */
        flags |= CF_ABSOLUTE;
        if (inc == g_inc) {
            g_addeqstatic (flags, Expr->ConstVal, 0, val);
        } else {
            g_subeqstatic (flags, Expr->ConstVal, 0, val);
        }
    } else if (Expr->Flags & E_MEXPR) {
        /* Address in a/x, check if we have an offset */
        unsigned Offs = (Expr->Flags == E_MEOFFS)? Expr->ConstVal : 0;
        if (inc == g_inc) {
            g_addeqind (flags, Offs, val);
        } else {
            g_subeqind (flags, Offs, val);
        }
    } else {

        /* Use generic code. Push the address if needed */
        PushAddr (Expr);

        /* Fetch the value */
        ExprLoad (CF_NONE, Expr);

        /* Increment value in primary */
        inc (flags, val);

        /* Store the result back */
        Store (Expr, 0);

    }

    /* Result is an expression, no reference */
    Expr->Flags = E_MEXPR | E_RVAL;
}



static void PostIncDec (ExprDesc* Expr, void (*inc) (unsigned, unsigned long))
/* Handle i-- and i++ */
{
    unsigned flags;

    NextToken ();

    /* The expression to increment must be an lvalue */
    if (ED_IsRVal (Expr)) {
    	Error ("Invalid lvalue");
       	return;
    }

    /* Get the data type */
    flags = TypeOf (Expr->Type);

    /* Push the address if needed */
    PushAddr (Expr);

    /* Fetch the value and save it (since it's the result of the expression) */
    ExprLoad (CF_NONE, Expr);
    g_save (flags | CF_FORCECHAR);

    /* If we have a pointer expression, increment by the size of the type */
    if (Expr->Type[0] == T_PTR) {
    	inc (flags | CF_CONST | CF_FORCECHAR, CheckedSizeOf (Expr->Type + 1));
    } else {
     	inc (flags | CF_CONST | CF_FORCECHAR, 1);
    }

    /* Store the result back */
    Store (Expr, 0);

    /* Restore the original value in the primary register */
    g_restore (flags | CF_FORCECHAR);

    /* The result is always an expression, no reference */
    Expr->Flags = E_MEXPR | E_RVAL;
}



static void UnaryOp (ExprDesc* Expr)
/* Handle unary -/+ and ~ */
{
    unsigned flags;

    /* Remember the operator token and skip it */
    token_t Tok = CurTok.Tok;
    NextToken ();

    /* Get the expression */
    hie10 (Expr);

    /* Check for a constant expression */
    if (ED_IsRVal (Expr) && (Expr->Flags & E_MCONST) != 0) {
    	/* Value is constant */
	switch (Tok) {
	    case TOK_MINUS: Expr->ConstVal = -Expr->ConstVal;	break;
	    case TOK_PLUS:  	  			   	break;
	    case TOK_COMP:  Expr->ConstVal = ~Expr->ConstVal; 	break;
	    default: 	    Internal ("Unexpected token: %d", Tok);
	}
    } else {
    	/* Value is not constant */
     	ExprLoad (CF_NONE, Expr);

    	/* Get the type of the expression */
    	flags = TypeOf (Expr->Type);

    	/* Handle the operation */
	switch (Tok) {
       	    case TOK_MINUS: g_neg (flags);  break;
	    case TOK_PLUS:  	       	    break;
	    case TOK_COMP:  g_com (flags);  break;
	    default: 	Internal ("Unexpected token: %d", Tok);
	}

        /* The result is a rvalue in the primary */
     	Expr->Flags = E_MEXPR | E_RVAL;
    }
}



void hie10 (ExprDesc* Expr)
/* Handle ++, --, !, unary - etc. */
{
    switch (CurTok.Tok) {

     	case TOK_INC:
     	    PreIncDec (Expr, g_inc);
     	    break;

     	case TOK_DEC:
     	    PostIncDec (Expr, g_dec);
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
    	       	Expr->ConstVal = !Expr->ConstVal;
    	    } else {
    	       	g_bneg (TypeOf (Expr->Type));
    	       	Expr->Test |= E_CC;		/* bneg will set cc */
    	       	Expr->Flags = E_MEXPR | E_RVAL;  /* say it's an expr */
    	    }
     	    break;

     	case TOK_STAR:
     	    NextToken ();
    	    if (evalexpr (CF_NONE, hie10, Expr) != 0) {
    	       	/* Expression is not const, indirect value loaded into primary */
	       	Expr->Flags = E_MEXPR | E_RVAL;
	       	Expr->ConstVal = 0;		/* Offset is zero now */
    	    }
            /* If the expression is already a pointer to function, the
             * additional dereferencing operator must be ignored.
             */
            if (IsTypeFuncPtr (Expr->Type)) {
                /* Expression not storable */
                ED_MakeRVal (Expr);
            } else {
                if (IsClassPtr (Expr->Type)) {
                    Expr->Type = Indirect (Expr->Type);
                } else {
                    Error ("Illegal indirection");
                }
                ED_MakeLVal (Expr);
            }
            break;

     	case TOK_AND:
     	    NextToken ();
       	    hie10 (Expr);
	    /* The & operator may be applied to any lvalue, and it may be
	     * applied to functions, even if they're no lvalues.
	     */
     	    if (ED_IsRVal (Expr) && !IsTypeFunc (Expr->Type)) {
	       	/* Allow the & operator with an array */
	       	if (!IsTypeArray (Expr->Type)) {
     	       	    Error ("Illegal address");
	       	}
     	    } else {
                Expr->Type = PointerTo (Expr->Type);
                ED_MakeRVal (Expr);
	    }
     	    break;

     	case TOK_SIZEOF:
     	    NextToken ();
       	    if (TypeSpecAhead ()) {
    	       	type Type[MAXTYPELEN];
     	       	NextToken ();
	       	Expr->ConstVal = CheckedSizeOf (ParseType (Type));
     	       	ConsumeRParen ();
     	    } else {
    	       	/* Remember the output queue pointer */
    	       	CodeMark Mark = GetCodePos ();
     	       	hie10 (Expr);
     	       	Expr->ConstVal = CheckedSizeOf (Expr->Type);
    	       	/* Remove any generated code */
    	       	RemoveCode (Mark);
     	    }
     	    Expr->Flags = E_MCONST | E_TCONST | E_RVAL;
     	    Expr->Type = type_size_t;
	    Expr->Test &= ~E_CC;
       	    break;

     	default:
       	    if (TypeSpecAhead ()) {

     	       	/* A typecast */
    	       	TypeCast (Expr);

     	    } else {

                /* An expression */
                hie11 (Expr);

                /* Handle post increment */
                if (CurTok.Tok == TOK_INC) {
                    PostIncDec (Expr, g_inc);
                } else if (CurTok.Tok == TOK_DEC) {
                    PostIncDec (Expr, g_dec);
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
    ExprDesc lval2;
    CodeMark Mark1;
    CodeMark Mark2;
    const GenDesc* Gen;
    token_t Tok;       	     		/* The operator token */
    unsigned ltype, type;
    int rconst;	       	       	       	/* Operand is a constant */


    hienext (Expr);

    *UsedGen = 0;
    while ((Gen = FindGen (CurTok.Tok, Ops)) != 0) {

	/* Tell the caller that we handled it's ops */
       	*UsedGen = 1;

	/* All operators that call this function expect an int on the lhs */
	if (!IsClassInt (Expr->Type)) {
	    Error ("Integer expression expected");
	}

	/* Remember the operator token, then skip it */
       	Tok = CurTok.Tok;
	NextToken ();

	/* Get the lhs on stack */
       	Mark1 = GetCodePos ();
	ltype = TypeOf (Expr->Type);
	if (ED_IsRVal (Expr) && Expr->Flags == E_MCONST) {
	    /* Constant value */
	    Mark2 = GetCodePos ();
       	    g_push (ltype | CF_CONST, Expr->ConstVal);
	} else {
	    /* Value not constant */
	    ExprLoad (CF_NONE, Expr);
	    Mark2 = GetCodePos ();
	    g_push (ltype, 0);
	}

	/* Get the right hand side */
	rconst = (evalexpr (CF_NONE, hienext, &lval2) == 0);

	/* Check the type of the rhs */
	if (!IsClassInt (lval2.Type)) {
	    Error ("Integer expression expected");
	}

	/* Check for const operands */
	if (ED_IsRVal (Expr) && Expr->Flags == E_MCONST && rconst) {

	    /* Both operands are constant, remove the generated code */
    	    RemoveCode (Mark1);
	    pop (ltype);

	    /* Evaluate the result */
	    Expr->ConstVal = kcalc (Tok, Expr->ConstVal, lval2.ConstVal);

	    /* Get the type of the result */
	    Expr->Type = promoteint (Expr->Type, lval2.Type);

	} else {

	    /* If the right hand side is constant, and the generator function
	     * expects the lhs in the primary, remove the push of the primary
	     * now.
	     */
	    unsigned rtype = TypeOf (lval2.Type);
	    type = 0;
	    if (rconst) {
	     	/* Second value is constant - check for div */
	     	type |= CF_CONST;
	 	rtype |= CF_CONST;
	     	if (Tok == TOK_DIV && lval2.ConstVal == 0) {
	      	    Error ("Division by zero");
	     	} else if (Tok == TOK_MOD && lval2.ConstVal == 0) {
	     	    Error ("Modulo operation with zero");
	     	}
	 	if ((Gen->Flags & GEN_NOPUSH) != 0) {
	 	    RemoveCode (Mark2);
	       	    pop (ltype);
	 	    ltype |= CF_REG;   	/* Value is in register */
	 	}
	    }

 	    /* Determine the type of the operation result. */
       	    type |= g_typeadjust (ltype, rtype);
	    Expr->Type = promoteint (Expr->Type, lval2.Type);

	    /* Generate code */
	    Gen->Func (type, lval2.ConstVal);

            /* We have a rvalue in the primary now */
	    Expr->Flags = E_MEXPR | E_RVAL;
	}
    }
}



static void hie_compare (const GenDesc* Ops,    /* List of generators */
       	                 ExprDesc* Expr,
       	                 void (*hienext) (ExprDesc*))
/* Helper function for the compare operators */
{
    ExprDesc lval2;
    CodeMark Mark1;
    CodeMark Mark2;
    const GenDesc* Gen;
    token_t tok; 	  		/* The operator token */
    unsigned ltype;
    int rconst;	       	       	       	/* Operand is a constant */


    hienext (Expr);

    while ((Gen = FindGen (CurTok.Tok, Ops)) != 0) {

    	/* Remember the operator token, then skip it */
       	tok = CurTok.Tok;
    	NextToken ();

    	/* Get the lhs on stack */
    	Mark1 = GetCodePos ();
    	ltype = TypeOf (Expr->Type);
    	if (ED_IsRVal (Expr) && Expr->Flags == E_MCONST) {
    	    /* Constant value */
    	    Mark2 = GetCodePos ();
       	    g_push (ltype | CF_CONST, Expr->ConstVal);
    	} else {
    	    /* Value not constant */
    	    ExprLoad (CF_NONE, Expr);
    	    Mark2 = GetCodePos ();
    	    g_push (ltype, 0);
      	}

	/* Get the right hand side */
	rconst = (evalexpr (CF_NONE, hienext, &lval2) == 0);

	/* Make sure, the types are compatible */
	if (IsClassInt (Expr->Type)) {
	    if (!IsClassInt (lval2.Type) && !(IsClassPtr(lval2.Type) && IsNullPtr(Expr))) {
	   	Error ("Incompatible types");
	    }
	} else if (IsClassPtr (Expr->Type)) {
	    if (IsClassPtr (lval2.Type)) {
	   	/* Both pointers are allowed in comparison if they point to
	   	 * the same type, or if one of them is a void pointer.
	         */
       	       	type* left  = Indirect (Expr->Type);
	   	type* right = Indirect (lval2.Type);
	   	if (TypeCmp (left, right) < TC_EQUAL && *left != T_VOID && *right != T_VOID) {
	   	    /* Incomatible pointers */
	   	    Error ("Incompatible types");
	   	}
	    } else if (!IsNullPtr (&lval2)) {
	   	Error ("Incompatible types");
	    }
	}

	/* Check for const operands */
	if (ED_IsRVal (Expr) && Expr->Flags == E_MCONST && rconst) {

	    /* Both operands are constant, remove the generated code */
    	    RemoveCode (Mark1);
	    pop (ltype);

	    /* Evaluate the result */
	    Expr->ConstVal = kcalc (tok, Expr->ConstVal, lval2.ConstVal);

	} else {

	    /* If the right hand side is constant, and the generator function
	     * expects the lhs in the primary, remove the push of the primary
	     * now.
	     */
	    unsigned flags = 0;
	    if (rconst) {
	    	flags |= CF_CONST;
	    	if ((Gen->Flags & GEN_NOPUSH) != 0) {
	    	    RemoveCode (Mark2);
	       	    pop (ltype);
	    	    ltype |= CF_REG;	/* Value is in register */
	    	}
	    }

 	    /* Determine the type of the operation result. If the left
	     * operand is of type char and the right is a constant, or
	     * if both operands are of type char, we will encode the
	     * operation as char operation. Otherwise the default
	     * promotions are used.
	     */
	    if (IsTypeChar (Expr->Type) && (IsTypeChar (lval2.Type) || rconst)) {
	       	flags |= CF_CHAR;
	       	if (IsSignUnsigned (Expr->Type) || IsSignUnsigned (lval2.Type)) {
	       	    flags |= CF_UNSIGNED;
	       	}
	       	if (rconst) {
	       	    flags |= CF_FORCECHAR;
	       	}
	    } else {
		unsigned rtype = TypeOf (lval2.Type) | (flags & CF_CONST);
       	        flags |= g_typeadjust (ltype, rtype);
	    }

	    /* Generate code */
	    Gen->Func (flags, lval2.ConstVal);
	    Expr->Flags = E_MEXPR | E_RVAL;
	}

	/* Result type is always int */
       	Expr->Type = type_int;

	/* Condition codes are set */
	Expr->Test |= E_CC;
    }
}



static void hie9 (ExprDesc *Expr)
/* Process * and / operators. */
{
    static const GenDesc hie9_ops[] = {
        { TOK_STAR,   	GEN_NOPUSH,	g_mul   },
        { TOK_DIV,    	GEN_NOPUSH,     g_div   },
        { TOK_MOD,    	GEN_NOPUSH,     g_mod   },
        { TOK_INVALID,  0,              0       }
    };
    int UsedGen;

    hie_internal (hie9_ops, Expr, hie10, &UsedGen);
}



static void parseadd (ExprDesc* Expr)
/* Parse an expression with the binary plus operator. Expr contains the
 * unprocessed left hand side of the expression and will contain the
 * result of the expression on return.
 */
{
    ExprDesc lval2;
    unsigned flags;          	/* Operation flags */
    CodeMark Mark;    	     	/* Remember code position */
    type* lhst;	      	     	/* Type of left hand side */
    type* rhst;	      	     	/* Type of right hand side */


    /* Skip the PLUS token */
    NextToken ();

    /* Get the left hand side type, initialize operation flags */
    lhst = Expr->Type;
    flags = 0;

    /* Check for constness on both sides */
    if (ED_IsRVal (Expr) && (Expr->Flags & E_MCONST) != 0) {

    	/* The left hand side is a constant. Good. Get rhs */
	hie9 (&lval2);
       	if (ED_IsRVal (&lval2) && lval2.Flags == E_MCONST) {

       	    /* Right hand side is also constant. Get the rhs type */
    	    rhst = lval2.Type;

    	    /* Both expressions are constants. Check for pointer arithmetic */
       	    if (IsClassPtr (lhst) && IsClassInt (rhst)) {
       	    	/* Left is pointer, right is int, must scale rhs */
       	       	Expr->ConstVal += lval2.ConstVal * CheckedPSizeOf (lhst);
    	    	/* Result type is a pointer */
    	    } else if (IsClassInt (lhst) && IsClassPtr (rhst)) {
    	    	/* Left is int, right is pointer, must scale lhs */
       	       	Expr->ConstVal = Expr->ConstVal * CheckedPSizeOf (rhst) + lval2.ConstVal;
    	    	/* Result type is a pointer */
    	    	Expr->Type = lval2.Type;
       	    } else if (IsClassInt (lhst) && IsClassInt (rhst)) {
    	    	/* Integer addition */
    	    	Expr->ConstVal += lval2.ConstVal;
    	    	typeadjust (Expr, &lval2, 1);
    	    } else {
       	       	/* OOPS */
    	    	Error ("Invalid operands for binary operator `+'");
    	    }

    	} else {

	    /* lhs is a constant and rhs is not constant. Load rhs into
	     * the primary.
	     */
    	    ExprLoad (CF_NONE, &lval2);

       	    /* Beware: The check above (for lhs) lets not only pass numeric
	     * constants, but also constant addresses (labels), maybe even
	     * with an offset. We have to check for that here.
	     */

    	    /* First, get the rhs type. */
       	    rhst = lval2.Type;

	    /* Setup flags */
	    if (Expr->Flags == E_MCONST) {
	    	/* A numerical constant */
	    	flags |= CF_CONST;
	    } else {
	    	/* Constant address label */
	    	flags |= GlobalModeFlags (Expr->Flags) | CF_CONSTADDR;
	    }

    	    /* Check for pointer arithmetic */
    	    if (IsClassPtr (lhst) && IsClassInt (rhst)) {
    	    	/* Left is pointer, right is int, must scale rhs */
    	    	g_scale (CF_INT, CheckedPSizeOf (lhst));
    	    	/* Operate on pointers, result type is a pointer */
      	    	flags |= CF_PTR;
	    	/* Generate the code for the add */
	    	if (Expr->Flags == E_MCONST) {
	    	    /* Numeric constant */
	    	    g_inc (flags, Expr->ConstVal);
	    	} else {
	    	    /* Constant address */
	    	    g_addaddr_static (flags, Expr->Name, Expr->ConstVal);
	    	}
    	    } else if (IsClassInt (lhst) && IsClassPtr (rhst)) {

    	      	/* Left is int, right is pointer, must scale lhs. */
	    	unsigned ScaleFactor = CheckedPSizeOf (rhst);

       	       	/* Operate on pointers, result type is a pointer */
	    	flags |= CF_PTR;
	    	Expr->Type = lval2.Type;

	    	/* Since we do already have rhs in the primary, if lhs is
	    	 * not a numeric constant, and the scale factor is not one
	    	 * (no scaling), we must take the long way over the stack.
	    	 */
	    	if (Expr->Flags == E_MCONST) {
	    	    /* Numeric constant, scale lhs */
	    	    Expr->ConstVal *= ScaleFactor;
	    	    /* Generate the code for the add */
	    	    g_inc (flags, Expr->ConstVal);
	    	} else if (ScaleFactor == 1) {
	    	    /* Constant address but no need to scale */
    	    	    g_addaddr_static (flags, Expr->Name, Expr->ConstVal);
	    	} else {
	    	    /* Constant address that must be scaled */
       	       	    g_push (TypeOf (lval2.Type), 0);   	/* rhs --> stack */
	    	    g_getimmed (flags, Expr->Name, Expr->ConstVal);
	    	    g_scale (CF_PTR, ScaleFactor);
	    	    g_add (CF_PTR, 0);
	    	}
       	    } else if (IsClassInt (lhst) && IsClassInt (rhst)) {
    	      	/* Integer addition */
       	       	flags |= typeadjust (Expr, &lval2, 1);
	    	/* Generate the code for the add */
	    	if (Expr->Flags == E_MCONST) {
	    	    /* Numeric constant */
	    	    g_inc (flags, Expr->ConstVal);
	    	} else {
	    	    /* Constant address */
	    	    g_addaddr_static (flags, Expr->Name, Expr->ConstVal);
	    	}
    	    } else {
       	       	/* OOPS */
    	    	Error ("Invalid operands for binary operator `+'");
       	    }

    	    /* Result is a rvalue in primary register */
    	    Expr->Flags = E_MEXPR | E_RVAL;
       	}

    } else {

    	/* Left hand side is not constant. Get the value onto the stack. */
    	ExprLoad (CF_NONE, Expr);              /* --> primary register */
       	Mark = GetCodePos ();
    	g_push (TypeOf (Expr->Type), 0);	/* --> stack */

    	/* Evaluate the rhs */
       	if (evalexpr (CF_NONE, hie9, &lval2) == 0) {

       	    /* Right hand side is a constant. Get the rhs type */
    	    rhst = lval2.Type;

      	    /* Remove pushed value from stack */
    	    RemoveCode (Mark);
    	    pop (TypeOf (Expr->Type));

       	    /* Check for pointer arithmetic */
       	    if (IsClassPtr (lhst) && IsClassInt (rhst)) {
    	       	/* Left is pointer, right is int, must scale rhs */
    	       	lval2.ConstVal *= CheckedPSizeOf (lhst);
    	      	/* Operate on pointers, result type is a pointer */
    	      	flags = CF_PTR;
    	    } else if (IsClassInt (lhst) && IsClassPtr (rhst)) {
    	      	/* Left is int, right is pointer, must scale lhs (ptr only) */
       	       	g_scale (CF_INT | CF_CONST, CheckedPSizeOf (rhst));
       	       	/* Operate on pointers, result type is a pointer */
    	      	flags = CF_PTR;
    	      	Expr->Type = lval2.Type;
       	    } else if (IsClassInt (lhst) && IsClassInt (rhst)) {
    	      	/* Integer addition */
       	      	flags = typeadjust (Expr, &lval2, 1);
       	    } else {
       	       	/* OOPS */
    	    	Error ("Invalid operands for binary operator `+'");
    	    }

    	    /* Generate code for the add */
       	    g_inc (flags | CF_CONST, lval2.ConstVal);

    	} else {

    	    /* lhs and rhs are not constant. Get the rhs type. */
    	    rhst = lval2.Type;

    	    /* Check for pointer arithmetic */
      	    if (IsClassPtr (lhst) && IsClassInt (rhst)) {
    	      	/* Left is pointer, right is int, must scale rhs */
    	      	g_scale (CF_INT, CheckedPSizeOf (lhst));
    	      	/* Operate on pointers, result type is a pointer */
    	      	flags = CF_PTR;
    	    } else if (IsClassInt (lhst) && IsClassPtr (rhst)) {
    	      	/* Left is int, right is pointer, must scale lhs */
    	      	g_tosint (TypeOf (rhst));	/* Make sure, TOS is int */
    	    	g_swap (CF_INT);  	 	/* Swap TOS and primary */
    	    	g_scale (CF_INT, CheckedPSizeOf (rhst));
    	      	/* Operate on pointers, result type is a pointer */
    	      	flags = CF_PTR;
    	      	Expr->Type = lval2.Type;
       	    } else if (IsClassInt (lhst) && IsClassInt (rhst)) {
    	      	/* Integer addition. Note: Result is never constant.
                 * Problem here is that typeadjust does not know if the
                 * variable is an rvalue or lvalue, so if both operands
                 * are dereferenced constant numeric addresses, typeadjust
                 * thinks the operation works on constants. Removing
                 * CF_CONST here means handling the symptoms, however, the
                 * whole parser is such a mess that I fear to break anything
                 * when trying to apply another solution.
                 */
       	       	flags = typeadjust (Expr, &lval2, 0) & ~CF_CONST;
    	    } else {
       	       	/* OOPS */
    	    	Error ("Invalid operands for binary operator `+'");
    	    }

    	    /* Generate code for the add */
       	    g_add (flags, 0);

       	}

        /* Result is a rvalue in primary register */
        Expr->Flags = E_MEXPR | E_RVAL;
    }

    /* Condition codes not set */
    Expr->Test &= ~E_CC;

}



static void parsesub (ExprDesc* Expr)
/* Parse an expression with the binary minus operator. Expr contains the
 * unprocessed left hand side of the expression and will contain the
 * result of the expression on return.
 */
{
    ExprDesc lval2;
    unsigned flags;         	/* Operation flags */
    type* lhst;	    	    	/* Type of left hand side */
    type* rhst;	    	    	/* Type of right hand side */
    CodeMark Mark1;		/* Save position of output queue */
    CodeMark Mark2;    	       	/* Another position in the queue */
    int rscale;     	    	/* Scale factor for the result */


    /* Skip the MINUS token */
    NextToken ();

    /* Get the left hand side type, initialize operation flags */
    lhst = Expr->Type;
    flags = 0;
    rscale = 1;	     	    	/* Scale by 1, that is, don't scale */

    /* Remember the output queue position, then bring the value onto the stack */
    Mark1 = GetCodePos ();
    ExprLoad (CF_NONE, Expr);  /* --> primary register */
    Mark2 = GetCodePos ();
    g_push (TypeOf (lhst), 0);	/* --> stack */

    /* Parse the right hand side */
    if (evalexpr (CF_NONE, hie9, &lval2) == 0) {

    	/* The right hand side is constant. Get the rhs type. */
       	rhst = lval2.Type;

    	/* Check left hand side */
    	if (ED_IsRVal (Expr) && (Expr->Flags & E_MCONST) != 0) {

    	    /* Both sides are constant, remove generated code */
      	    RemoveCode (Mark1);
    	    pop (TypeOf (lhst));       	/* Clean up the stack */

    	    /* Check for pointer arithmetic */
    	    if (IsClassPtr (lhst) && IsClassInt (rhst)) {
    	    	/* Left is pointer, right is int, must scale rhs */
    	    	Expr->ConstVal -= lval2.ConstVal * CheckedPSizeOf (lhst);
    	    	/* Operate on pointers, result type is a pointer */
    	    } else if (IsClassPtr (lhst) && IsClassPtr (rhst)) {
    	    	/* Left is pointer, right is pointer, must scale result */
    	    	if (TypeCmp (Indirect (lhst), Indirect (rhst)) < TC_QUAL_DIFF) {
    	    	    Error ("Incompatible pointer types");
    	    	} else {
    	    	    Expr->ConstVal = (Expr->ConstVal - lval2.ConstVal) /
                                      CheckedPSizeOf (lhst);
    	    	}
    	    	/* Operate on pointers, result type is an integer */
    	    	Expr->Type = type_int;
    	    } else if (IsClassInt (lhst) && IsClassInt (rhst)) {
    	    	/* Integer subtraction */
       	       	typeadjust (Expr, &lval2, 1);
    	    	Expr->ConstVal -= lval2.ConstVal;
    	    } else {
    	    	/* OOPS */
    	    	Error ("Invalid operands for binary operator `-'");
    	    }

    	    /* Result is constant, condition codes not set */
    	    /* Expr->Flags = E_MCONST; ### */
    	    Expr->Test &= ~E_CC;

    	} else {

    	    /* Left hand side is not constant, right hand side is.
    	     * Remove pushed value from stack.
    	     */
    	    RemoveCode (Mark2);
    	    pop (TypeOf (lhst));

    	    if (IsClassPtr (lhst) && IsClassInt (rhst)) {
    	    	/* Left is pointer, right is int, must scale rhs */
       	       	lval2.ConstVal *= CheckedPSizeOf (lhst);
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
       	       	flags = typeadjust (Expr, &lval2, 1);
    	    } else {
    	    	/* OOPS */
    	    	Error ("Invalid operands for binary operator `-'");
    	    }

    	    /* Do the subtraction */
    	    g_dec (flags | CF_CONST, lval2.ConstVal);

 	    /* If this was a pointer subtraction, we must scale the result */
 	    if (rscale != 1) {
 	    	g_scale (flags, -rscale);
 	    }

 	    /* Result is a rvalue in the primary register */
 	    Expr->Flags = E_MEXPR | E_RVAL;
 	    Expr->Test &= ~E_CC;

 	}

    } else {

 	/* Right hand side is not constant. Get the rhs type. */
 	rhst = lval2.Type;

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
	     * the lhs is const, we have to remove this mark, since this is no
	     * longer true, lhs is on stack instead.
	     */
	    if (Expr->Flags == E_MCONST) {
		Expr->Flags = E_MEXPR | E_RVAL;
	    }
	    /* Adjust operand types */
 	    flags = typeadjust (Expr, &lval2, 0);
    	} else {
 	    /* OOPS */
	    Error ("Invalid operands for binary operator `-'");
 	}

 	/* Generate code for the sub (the & is a hack here) */
 	g_sub (flags & ~CF_CONST, 0);

 	/* If this was a pointer subtraction, we must scale the result */
 	if (rscale != 1) {
 	    g_scale (flags, -rscale);
      	}

	/* Result is a rvalue in the primary register */
	Expr->Flags = E_MEXPR | E_RVAL;
	Expr->Test &= ~E_CC;
    }
}



static void hie8 (ExprDesc* Expr)
/* Process + and - binary operators. */
{
    hie9 (Expr);
    while (CurTok.Tok == TOK_PLUS || CurTok.Tok == TOK_MINUS) {
       	if (CurTok.Tok == TOK_PLUS) {
       	    parseadd (Expr);
       	} else {
       	    parsesub (Expr);
       	}
    }
}




static void hie7 (ExprDesc* Expr)
/* Parse << and >>. */
{
    static const GenDesc hie7_ops [] = {
        { TOK_SHL,    	GEN_NOPUSH,     g_asl   },
        { TOK_SHR,    	GEN_NOPUSH,     g_asr   },
        { TOK_INVALID,  0,              0       }
    };
    int UsedGen;

    hie_internal (hie7_ops, Expr, hie8, &UsedGen);
}



static void hie6 (ExprDesc* Expr)
/* Handle greater-than type comparators */
{
    static const GenDesc hie6_ops [] = {
        { TOK_LT,     	GEN_NOPUSH,	g_lt    },
        { TOK_LE,     	GEN_NOPUSH,	g_le    },
        { TOK_GE,     	GEN_NOPUSH,	g_ge    },
        { TOK_GT,     	GEN_NOPUSH,	g_gt    },
        { TOK_INVALID,  0,              0       }
    };
    hie_compare (hie6_ops, Expr, hie7);
}



static void hie5 (ExprDesc* Expr)
/* Handle == and != */
{
    static const GenDesc hie5_ops[] = {
        { TOK_EQ,     	GEN_NOPUSH,	g_eq    },
        { TOK_NE,     	GEN_NOPUSH,	g_ne    },
        { TOK_INVALID,  0,              0       }
    };
    hie_compare (hie5_ops, Expr, hie6);
}



static void hie4 (ExprDesc* Expr)
/* Handle & (bitwise and) */
{
    static const GenDesc hie4_ops[] = {
        { TOK_AND,    	GEN_NOPUSH,     g_and   },
        { TOK_INVALID,  0,              0       }
    };
    int UsedGen;

    hie_internal (hie4_ops, Expr, hie5, &UsedGen);
}



static void hie3 (ExprDesc* Expr)
/* Handle ^ (bitwise exclusive or) */
{
    static const GenDesc hie3_ops[] = {
        { TOK_XOR,    	GEN_NOPUSH,     g_xor   },
        { TOK_INVALID,  0,              0       }
    };
    int UsedGen;

    hie_internal (hie3_ops, Expr, hie4, &UsedGen);
}



static void hie2 (ExprDesc* Expr)
/* Handle | (bitwise or) */
{
    static const GenDesc hie2_ops[] = {
        { TOK_OR,    	GEN_NOPUSH,     g_or    },
        { TOK_INVALID,  0,              0       }
    };
    int UsedGen;

    hie_internal (hie2_ops, Expr, hie3, &UsedGen);
}



static void hieAndPP (ExprDesc* Expr)
/* Process "exp && exp" in preprocessor mode (that is, when the parser is
 * called recursively from the preprocessor.
 */
{
    ExprDesc lval2;

    ConstSubExpr (hie2, Expr);
    while (CurTok.Tok == TOK_BOOL_AND) {

	/* Left hand side must be an int */
	if (!IsClassInt (Expr->Type)) {
	    Error ("Left hand side must be of integer type");
	    ED_MakeConstInt (Expr, 1);
	}

	/* Skip the && */
	NextToken ();

	/* Get rhs */
	ConstSubExpr (hie2, &lval2);

	/* Since we are in PP mode, all we know about is integers */
	if (!IsClassInt (lval2.Type)) {
	    Error ("Right hand side must be of integer type");
	    ED_MakeConstInt (&lval2, 1);
	}

	/* Combine the two */
	Expr->ConstVal = (Expr->ConstVal && lval2.ConstVal);
    }
}



static void hieOrPP (ExprDesc *Expr)
/* Process "exp || exp" in preprocessor mode (that is, when the parser is
 * called recursively from the preprocessor.
 */
{
    ExprDesc lval2;

    ConstSubExpr (hieAndPP, Expr);
    while (CurTok.Tok == TOK_BOOL_OR) {

	/* Left hand side must be an int */
	if (!IsClassInt (Expr->Type)) {
	    Error ("Left hand side must be of integer type");
	    ED_MakeConstInt (Expr, 1);
	}

	/* Skip the && */
	NextToken ();

	/* Get rhs */
	ConstSubExpr (hieAndPP, &lval2);

	/* Since we are in PP mode, all we know about is integers */
	if (!IsClassInt (lval2.Type)) {
	    Error ("Right hand side must be of integer type");
	    ED_MakeConstInt (&lval2, 1);
	}

	/* Combine the two */
	Expr->ConstVal = (Expr->ConstVal || lval2.ConstVal);
    }
}



static void hieAnd (ExprDesc* Expr, unsigned TrueLab, int* BoolOp)
/* Process "exp && exp" */
{
    int lab;
    ExprDesc lval2;

    hie2 (Expr);
    if (CurTok.Tok == TOK_BOOL_AND) {

       	/* Tell our caller that we're evaluating a boolean */
       	*BoolOp = 1;

       	/* Get a label that we will use for false expressions */
       	lab = GetLocalLabel ();

       	/* If the expr hasn't set condition codes, set the force-test flag */
       	if ((Expr->Test & E_CC) == 0) {
       	    Expr->Test |= E_FORCETEST;
       	}

       	/* Load the value */
       	ExprLoad (CF_FORCECHAR, Expr);

       	/* Generate the jump */
       	g_falsejump (CF_NONE, lab);

       	/* Parse more boolean and's */
       	while (CurTok.Tok == TOK_BOOL_AND) {

       	    /* Skip the && */
    	    NextToken ();

    	    /* Get rhs */
    	    hie2 (&lval2);
    	    if ((lval2.Test & E_CC) == 0) {
    		lval2.Test |= E_FORCETEST;
    	    }
    	    ExprLoad (CF_FORCECHAR, &lval2);

       	    /* Do short circuit evaluation */
    	    if (CurTok.Tok == TOK_BOOL_AND) {
    	        g_falsejump (CF_NONE, lab);
       	    } else {
       		/* Last expression - will evaluate to true */
       	     	g_truejump (CF_NONE, TrueLab);
       	    }
       	}

       	/* Define the false jump label here */
       	g_defcodelabel (lab);

       	/* The result is an rvalue in primary */
       	Expr->Flags = E_MEXPR | E_RVAL;
       	Expr->Test |= E_CC;	/* Condition codes are set */
    }
}



static void hieOr (ExprDesc *Expr)
/* Process "exp || exp". */
{
    ExprDesc lval2;
    int BoolOp = 0;  		/* Did we have a boolean op? */
    int AndOp;			/* Did we have a && operation? */
    unsigned TrueLab;		/* Jump to this label if true */
    unsigned DoneLab;

    /* Get a label */
    TrueLab = GetLocalLabel ();

    /* Call the next level parser */
    hieAnd (Expr, TrueLab, &BoolOp);

    /* Any boolean or's? */
    if (CurTok.Tok == TOK_BOOL_OR) {

    	/* If the expr hasn't set condition codes, set the force-test flag */
       	if ((Expr->Test & E_CC) == 0) {
    	    Expr->Test |= E_FORCETEST;
    	}

    	/* Get first expr */
      	ExprLoad (CF_FORCECHAR, Expr);

       	/* For each expression jump to TrueLab if true. Beware: If we
    	 * had && operators, the jump is already in place!
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
    	    hieAnd (&lval2, TrueLab, &AndOp);
       	    if ((lval2.Test & E_CC) == 0) {
    	    	lval2.Test |= E_FORCETEST;
    	    }
    	    ExprLoad (CF_FORCECHAR, &lval2);

       	    /* If there is more to come, add shortcut boolean eval. */
    	    g_truejump (CF_NONE, TrueLab);

	}

       	/* The result is an rvalue in primary */
    	Expr->Flags = E_MEXPR | E_RVAL;
    	Expr->Test |= E_CC;	     	       	/* Condition codes are set */
    }

    /* If we really had boolean ops, generate the end sequence */
    if (BoolOp) {
    	DoneLab = GetLocalLabel ();
    	g_getimmed (CF_INT | CF_CONST, 0, 0);	/* Load FALSE */
       	g_falsejump (CF_NONE, DoneLab);
    	g_defcodelabel (TrueLab);
    	g_getimmed (CF_INT | CF_CONST, 1, 0); 	/* Load TRUE */
    	g_defcodelabel (DoneLab);
    }
}



static void hieQuest (ExprDesc* Expr)
/* Parse the ternary operator */
{
    int         labf;
    int         labt;
    ExprDesc 	Expr2;          /* Expression 2 */
    ExprDesc 	Expr3;          /* Expression 3 */
    int         Expr2IsNULL;    /* Expression 2 is a NULL pointer */
    int         Expr3IsNULL;    /* Expression 3 is a NULL pointer */
    type* 	ResultType;     /* Type of result */


    /* Call the lower level eval routine */
    if (Preprocessing) {
        hieOrPP (Expr);
    } else {
        hieOr (Expr);
    }

    /* Check if it's a ternary expression */
    if (CurTok.Tok == TOK_QUEST) {
    	NextToken ();
    	if ((Expr->Test & E_CC) == 0) {
    	    /* Condition codes not set, force a test */
    	    Expr->Test |= E_FORCETEST;
    	}
    	ExprLoad (CF_NONE, Expr);
    	labf = GetLocalLabel ();
    	g_falsejump (CF_NONE, labf);

    	/* Parse second expression. Remember for later if it is a NULL pointer
         * expression, then load it into the primary.
         */
        expr (hie1, &Expr2);
        Expr2IsNULL = IsNullPtr (&Expr2);
        if (!IsTypeVoid (Expr2.Type)) {
            /* Load it into the primary */
            ExprLoad (CF_NONE, &Expr2);
	    Expr2.Flags = E_MEXPR | E_RVAL;
        }
    	labt = GetLocalLabel ();
    	ConsumeColon ();
    	g_jump (labt);

        /* Jump here if the first expression was false */
    	g_defcodelabel (labf);

    	/* Parse second expression. Remember for later if it is a NULL pointer
         * expression, then load it into the primary.
         */
        expr (hie1, &Expr3);
        Expr3IsNULL = IsNullPtr (&Expr3);
        if (!IsTypeVoid (Expr3.Type)) {
            /* Load it into the primary */
            ExprLoad (CF_NONE, &Expr3);
	    Expr3.Flags = E_MEXPR | E_RVAL;
        }

    	/* Check if any conversions are needed, if so, do them.
    	 * Conversion rules for ?: expression are:
    	 *   - if both expressions are int expressions, default promotion
    	 *     rules for ints apply.
    	 *   - if both expressions are pointers of the same type, the
    	 *     result of the expression is of this type.
    	 *   - if one of the expressions is a pointer and the other is
    	 *     a zero constant, the resulting type is that of the pointer
	 *     type.
         *   - if both expressions are void expressions, the result is of
         *     type void.
	 *   - all other cases are flagged by an error.
	 */
	if (IsClassInt (Expr2.Type) && IsClassInt (Expr3.Type)) {

	    /* Get common type */
	    ResultType = promoteint (Expr2.Type, Expr3.Type);

	    /* Convert the third expression to this type if needed */
	    TypeConversion (&Expr3, ResultType);

	    /* Setup a new label so that the expr3 code will jump around
	     * the type cast code for expr2.
	     */
       	    labf = GetLocalLabel (); 	/* Get new label */
	    g_jump (labf);     	    	/* Jump around code */

	    /* The jump for expr2 goes here */
    	    g_defcodelabel (labt);

	    /* Create the typecast code for expr2 */
    	    TypeConversion (&Expr2, ResultType);

	    /* Jump here around the typecase code. */
	    g_defcodelabel (labf);
	    labt = 0;	       	/* Mark other label as invalid */

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
      	    ResultType = Expr2.Type;		/* Doesn't matter here */
      	}

      	/* If we don't have the label defined until now, do it */
      	if (labt) {
      	    g_defcodelabel (labt);
      	}

      	/* Setup the target expression */
       	Expr->Flags = E_MEXPR | E_RVAL;
      	Expr->Type  = ResultType;
    }
}



static void opeq (const GenDesc* Gen, ExprDesc* Expr)
/* Process "op=" operators. */
{
    ExprDesc lval2;
    unsigned flags;
    CodeMark Mark;
    int MustScale;

    NextToken ();
    if (ED_IsRVal (Expr)) {
     	Error ("Invalid lvalue in assignment");
     	return;
    }

    /* Determine the type of the lhs */
    flags = TypeOf (Expr->Type);
    MustScale = (Gen->Func == g_add || Gen->Func == g_sub) &&
	    	Expr->Type [0] == T_PTR;

    /* Get the lhs address on stack (if needed) */
    PushAddr (Expr);

    /* Fetch the lhs into the primary register if needed */
    ExprLoad (CF_NONE, Expr);

    /* Bring the lhs on stack */
    Mark = GetCodePos ();
    g_push (flags, 0);

    /* Evaluate the rhs */
    if (evalexpr (CF_NONE, hie1, &lval2) == 0) {
	/* The resulting value is a constant. If the generator has the NOPUSH
	 * flag set, don't push the lhs.
	 */
	if (Gen->Flags & GEN_NOPUSH) {
	    RemoveCode (Mark);
	    pop (flags);
	}
       	if (MustScale) {
	    /* lhs is a pointer, scale rhs */
	    lval2.ConstVal *= CheckedSizeOf (Expr->Type+1);
	}

	/* If the lhs is character sized, the operation may be later done
	 * with characters.
	 */
	if (CheckedSizeOf (Expr->Type) == SIZEOF_CHAR) {
	    flags |= CF_FORCECHAR;
	}

     	/* Special handling for add and sub - some sort of a hack, but short code */
	if (Gen->Func == g_add) {
	    g_inc (flags | CF_CONST, lval2.ConstVal);
	} else if (Gen->Func == g_sub) {
	    g_dec (flags | CF_CONST, lval2.ConstVal);
	} else {
       	    Gen->Func (flags | CF_CONST, lval2.ConstVal);
	}
    } else {
	/* rhs is not constant and already in the primary register */
       	if (MustScale) {
 	    /* lhs is a pointer, scale rhs */
       	    g_scale (TypeOf (lval2.Type), CheckedSizeOf (Expr->Type+1));
	}

	/* If the lhs is character sized, the operation may be later done
	 * with characters.
	 */
	if (CheckedSizeOf (Expr->Type) == SIZEOF_CHAR) {
	    flags |= CF_FORCECHAR;
	}

	/* Adjust the types of the operands if needed */
       	Gen->Func (g_typeadjust (flags, TypeOf (lval2.Type)), 0);
    }
    Store (Expr, 0);
    Expr->Flags = E_MEXPR | E_RVAL;
}



static void addsubeq (const GenDesc* Gen, ExprDesc *Expr)
/* Process the += and -= operators */
{
    ExprDesc lval2;
    unsigned lflags;
    unsigned rflags;
    int      MustScale;


    /* We must have an lvalue */
    if (ED_IsRVal (Expr)) {
     	Error ("Invalid lvalue in assignment");
     	return;
    }

    /* We're currently only able to handle some adressing modes */
    if ((Expr->Flags & E_MGLOBAL) == 0 && 	/* Global address? */
	(Expr->Flags & E_MLOCAL) == 0  && 	/* Local address? */
       	(Expr->Flags & E_MCONST) == 0) {	/* Constant address? */
	/* Use generic routine */
       	opeq (Gen, Expr);
	return;
    }

    /* Skip the operator */
    NextToken ();

    /* Check if we have a pointer expression and must scale rhs */
    MustScale = (Expr->Type [0] == T_PTR);

    /* Initialize the code generator flags */
    lflags = 0;
    rflags = 0;

    /* Evaluate the rhs */
    hie1 (&lval2);
    if (ED_IsRVal (&lval2) && lval2.Flags == E_MCONST) {
    	/* The resulting value is a constant. */
       	if (MustScale) {
    	    /* lhs is a pointer, scale rhs */
    	    lval2.ConstVal *= CheckedSizeOf (Expr->Type+1);
    	}
     	rflags |= CF_CONST;
    	lflags |= CF_CONST;
    } else {
     	/* Not constant, load into the primary */
        ExprLoad (CF_NONE, &lval2);
       	if (MustScale) {
     	    /* lhs is a pointer, scale rhs */
       	    g_scale (TypeOf (lval2.Type), CheckedSizeOf (Expr->Type+1));
     	}
    }

    /* Setup the code generator flags */
    lflags |= TypeOf (Expr->Type) | CF_FORCECHAR;
    rflags |= TypeOf (lval2.Type);

    /* Convert the type of the lhs to that of the rhs */
    g_typecast (lflags, rflags);

    /* Output apropriate code */
    if (Expr->Flags & E_MGLOBAL) {
	/* Static variable */
	lflags |= GlobalModeFlags (Expr->Flags);
	if (Gen->Tok == TOK_PLUS_ASSIGN) {
	    g_addeqstatic (lflags, Expr->Name, Expr->ConstVal, lval2.ConstVal);
	} else {
       	    g_subeqstatic (lflags, Expr->Name, Expr->ConstVal, lval2.ConstVal);
	}
    } else if (Expr->Flags & E_MLOCAL) {
	/* ref to localvar */
	if (Gen->Tok == TOK_PLUS_ASSIGN) {
    	    g_addeqlocal (lflags, Expr->ConstVal, lval2.ConstVal);
	} else {
	    g_subeqlocal (lflags, Expr->ConstVal, lval2.ConstVal);
	}
    } else if (Expr->Flags & E_MCONST) {
	/* ref to absolute address */
	lflags |= CF_ABSOLUTE;
	if (Gen->Tok == TOK_PLUS_ASSIGN) {
	    g_addeqstatic (lflags, Expr->ConstVal, 0, lval2.ConstVal);
	} else {
       	    g_subeqstatic (lflags, Expr->ConstVal, 0, lval2.ConstVal);
	}
    } else if (Expr->Flags & E_MEXPR) {
       	/* Address in a/x. */
	if (Gen->Tok == TOK_PLUS_ASSIGN) {
       	    g_addeqind (lflags, Expr->ConstVal, lval2.ConstVal);
	} else {
       	    g_subeqind (lflags, Expr->ConstVal, lval2.ConstVal);
	}
    } else {
       	Internal ("Invalid addressing mode");
    }

    /* Expression is a rvalue in the primary now */
    Expr->Flags = E_MEXPR | E_RVAL;
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
       	    addsubeq (&GenPASGN, Expr);
    	    break;

    	case TOK_MINUS_ASSIGN:
       	    addsubeq (&GenSASGN, Expr);
    	    break;

    	case TOK_MUL_ASSIGN:
       	    opeq (&GenMASGN, Expr);
     	    break;

     	case TOK_DIV_ASSIGN:
       	    opeq (&GenDASGN, Expr);
     	    break;

     	case TOK_MOD_ASSIGN:
       	    opeq (&GenMOASGN, Expr);
     	    break;

     	case TOK_SHL_ASSIGN:
       	    opeq (&GenSLASGN, Expr);
     	    break;

     	case TOK_SHR_ASSIGN:
       	    opeq (&GenSRASGN, Expr);
     	    break;

     	case TOK_AND_ASSIGN:
       	    opeq (&GenAASGN, Expr);
     	    break;

     	case TOK_XOR_ASSIGN:
       	    opeq (&GenXOASGN, Expr);
     	    break;

     	case TOK_OR_ASSIGN:
       	    opeq (&GenOASGN, Expr);
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
 * constant, 0 is returned and the value is put in the Expr struct. If the
 * result is not constant, ExprLoad is called to bring the value into the
 * primary register and 1 is returned.
 */
{
    /* Evaluate */
    Func (Expr);

    /* Check for a constant expression */
    if (ED_IsRVal (Expr) && Expr->Flags == E_MCONST) {
     	/* Constant expression */
     	return 0;
    } else {
     	/* Not constant, load into the primary */
        ExprLoad (Flags, Expr);
     	return 1;
    }
}



void expr (void (*Func) (ExprDesc*), ExprDesc *Expr)
/* Expression parser; func is either hie0 or hie1. */
{
    /* Remember the stack pointer */
    int savsp = oursp;

    /* Call the expression function */
    (*Func) (Expr);

    /* Do some checks if code generation is still constistent */
    if (savsp != oursp) {
     	if (Debug) {
     	    fprintf (stderr, "oursp != savesp (%d != %d)\n", oursp, savsp);
     	} else {
     	    Internal ("oursp != savsp (%d != %d)", oursp, savsp);
     	}
    }
}



void expression1 (ExprDesc* Expr)
/* Evaluate an expression on level 1 (no comma operator) and put it into
 * the primary register
 */
{
    expr (hie1, InitExprDesc (Expr));
    ExprLoad (CF_NONE, Expr);
}



void expression0 (ExprDesc* Expr)
/* Evaluate an expression via hie0 and put it into the primary register */
{
    expr (hie0, InitExprDesc (Expr));
    ExprLoad (CF_NONE, Expr);
}



void ConstExpr (ExprDesc* Expr)
/* Get a constant value */
{
    expr (hie1, InitExprDesc (Expr));
    if (ED_IsLVal (Expr) || (Expr->Flags & E_MCONST) == 0) {
     	Error ("Constant expression expected");
     	/* To avoid any compiler errors, make the expression a valid const */
     	ED_MakeConstInt (Expr, 1);
    }
}



void ConstIntExpr (ExprDesc* Expr)
/* Get a constant int value */
{
    expr (hie1, InitExprDesc (Expr));
    if (ED_IsLVal (Expr) || (Expr->Flags & E_MCONST) == 0 || !IsClassInt (Expr->Type)) {
     	Error ("Constant integer expression expected");
     	/* To avoid any compiler errors, make the expression a valid const */
     	ED_MakeConstInt (Expr, 1);
    }
}



void intexpr (ExprDesc* lval)
/* Get an integer expression */
{
    expression0 (lval);
    if (!IsClassInt (lval->Type)) {
     	Error ("Integer expression expected");
     	/* To avoid any compiler errors, make the expression a valid int */
     	ED_MakeConstInt (lval, 1);
    }
}



