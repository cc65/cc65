/*
 * expr.c
 *
 * Ullrich von Bassewitz, 21.06.1998
 */



#include <stdio.h>
#include <stdlib.h>

/* common */
#include "check.h"
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
#include "typecast.h"
#include "typecmp.h"
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
static GenDesc GenMUL    = { TOK_STAR,   	GEN_NOPUSH,	g_mul };
static GenDesc GenDIV    = { TOK_DIV,    	GEN_NOPUSH,     g_div };
static GenDesc GenMOD    = { TOK_MOD,    	GEN_NOPUSH,     g_mod };
static GenDesc GenASL    = { TOK_SHL,    	GEN_NOPUSH,     g_asl };
static GenDesc GenASR    = { TOK_SHR,    	GEN_NOPUSH,     g_asr };
static GenDesc GenLT     = { TOK_LT,     	GEN_NOPUSH,	g_lt  };
static GenDesc GenLE     = { TOK_LE,     	GEN_NOPUSH,	g_le  };
static GenDesc GenGE     = { TOK_GE,     	GEN_NOPUSH,	g_ge  };
static GenDesc GenGT     = { TOK_GT,     	GEN_NOPUSH,	g_gt  };
static GenDesc GenEQ     = { TOK_EQ,     	GEN_NOPUSH,	g_eq  };
static GenDesc GenNE     = { TOK_NE,     	GEN_NOPUSH,	g_ne  };
static GenDesc GenAND    = { TOK_AND,    	GEN_NOPUSH,     g_and };
static GenDesc GenXOR    = { TOK_XOR,    	GEN_NOPUSH,     g_xor };
static GenDesc GenOR     = { TOK_OR,    	GEN_NOPUSH,     g_or  };
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



static int hie0 (ExprDesc *lval);
/* Parse comma operator. */

static int expr (int (*func) (ExprDesc*), ExprDesc *lval);
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



unsigned assignadjust (type* lhst, ExprDesc* rhs)
/* Adjust the type of the right hand expression so that it can be assigned to
 * the type on the left hand side. This function is used for assignment and
 * for converting parameters in a function call. It returns the code generator
 * flags for the operation. The type string of the right hand side will be
 * set to the type of the left hand side.
 */
{
    /* Get the type of the right hand side. Treat function types as
     * pointer-to-function
     */
    type* rhst = rhs->Type;
    if (IsTypeFunc (rhst)) {
	rhst = PointerTo (rhst);
    }

    /* After calling this function, rhs will have the type of the lhs */
    rhs->Type = lhst;

    /* First, do some type checking */
    if (IsTypeVoid (lhst) || IsTypeVoid (rhst)) {
    	/* If one of the sides are of type void, output a more apropriate
    	 * error message.
    	 */
       	Error ("Illegal type");
    } else if (IsClassInt (lhst)) {
       	if (IsClassPtr (rhst)) {
     	    /* Pointer -> int conversion */
     	    Warning ("Converting pointer to integer without a cast");
       	} else if (!IsClassInt (rhst)) {
     	    Error ("Incompatible types");
     	} else {
   	    /* Convert the rhs to the type of the lhs. */
   	    unsigned flags = TypeOf (rhst);
       	    if (rhs->Flags == E_MCONST) {
   	 	flags |= CF_CONST;
   	    }
       	    return g_typecast (TypeOf (lhst), flags);
        }
    } else if (IsClassPtr (lhst)) {
     	if (IsClassPtr (rhst)) {
     	    /* Pointer to pointer assignment is valid, if:
     	     *   - both point to the same types, or
     	     *   - the rhs pointer is a void pointer, or
	     *   - the lhs pointer is a void pointer.
     	     */
	    if (!IsTypeVoid (Indirect (lhst)) && !IsTypeVoid (Indirect (rhst))) {
	 	/* Compare the types */
	 	switch (TypeCmp (lhst, rhst)) {

	 	    case TC_INCOMPATIBLE:
	 		Error ("Incompatible pointer types");
	 		break;

	 	    case TC_QUAL_DIFF:
	 		Error ("Pointer types differ in type qualifiers");
	 		break;

	 	    default:
	 		/* Ok */
	 		break;
	 	}
	    }
     	} else if (IsClassInt (rhst)) {
     	    /* Int to pointer assignment is valid only for constant zero */
     	    if (rhs->Flags != E_MCONST || rhs->ConstVal != 0) {
     	       	Warning ("Converting integer to pointer without a cast");
     	    }
	} else if (IsTypeFuncPtr (lhst) && IsTypeFunc(rhst)) {
	    /* Assignment of function to function pointer is allowed, provided
	     * that both functions have the same parameter list.
	     */
	    if (TypeCmp (Indirect (lhst), rhst) < TC_EQUAL) {
	 	Error ("Incompatible types");
	    }
     	} else {
	    Error ("Incompatible types");
	}
    } else {
	Error ("Incompatible types");
    }

    /* Return an int value in all cases where the operands are not both ints */
    return CF_INT;
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
	    if (!AllowRegVarAddr) {
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
	    if (!AllowRegVarAddr) {
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



static int kcalc (int tok, long val1, long val2)
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



static const GenDesc* FindGen (token_t Tok, const GenDesc** Table)
/* Find a token in a generator table */
{
    const GenDesc* G;
    while ((G = *Table) != 0) {
	if (G->Tok == Tok) {
	    return G;
	}
      	++Table;
    }
    return 0;
}



static int istypeexpr (void)
/* Return true if some sort of variable or type is waiting (helper for cast
 * and sizeof() in hie10).
 */
{
    SymEntry* Entry;

    return CurTok.Tok == TOK_LPAREN && (
       	   (NextTok.Tok >= TOK_FIRSTTYPE && NextTok.Tok <= TOK_LASTTYPE) ||
	   (NextTok.Tok == TOK_CONST)                           	 ||
       	   (NextTok.Tok  == TOK_IDENT 			      	         &&
	   (Entry = FindSym (NextTok.Ident)) != 0  		         &&
	   SymIsTypeDef (Entry)));
}



void PushAddr (ExprDesc* lval)
/* If the expression contains an address that was somehow evaluated,
 * push this address on the stack. This is a helper function for all
 * sorts of implicit or explicit assignment functions where the lvalue
 * must be saved if it's not constant, before evaluating the rhs.
 */
{
    /* Get the address on stack if needed */
    if (lval->Flags != E_MREG && (lval->Flags & E_MEXPR)) {
     	/* Push the address (always a pointer) */
     	g_push (CF_PTR, 0);
    }
}



void ConstSubExpr (int (*F) (ExprDesc*), ExprDesc* Expr)
/* Will evaluate an expression via the given function. If the result is not
 * a constant, a diagnostic will be printed, and the value is replaced by
 * a constant one to make sure there are no internal errors that result
 * from this input error.
 */
{
    InitExprDesc (Expr);
    if (F (Expr) != 0 || Expr->Flags != E_MCONST) {
       	Error ("Constant expression expected");
       	/* To avoid any compiler errors, make the expression a valid const */
     	MakeConstIntExpr (Expr, 1);
    }
}



void CheckBoolExpr (ExprDesc* lval)
/* Check if the given expression is a boolean expression, output a diagnostic
 * if not.
 */
{
    /* If it's an integer, it's ok. If it's not an integer, but a pointer,
     * the pointer used in a boolean context is also ok
     */
    if (!IsClassInt (lval->Type) && !IsClassPtr (lval->Type)) {
 	Error ("Boolean expression expected");
 	/* To avoid any compiler errors, make the expression a valid int */
	MakeConstIntExpr (lval, 1);
    }
}



/*****************************************************************************/
/*   	     	     		     code				     */
/*****************************************************************************/



void exprhs (unsigned flags, int k, ExprDesc *lval)
/* Put the result of an expression into the primary register */
{
    int f;

    f = lval->Flags;
    if (k) {
       	/* Dereferenced lvalue */
       	flags |= TypeOf (lval->Type);
     	if (lval->Test & E_FORCETEST) {
     	    flags |= CF_TEST;
     	    lval->Test &= ~E_FORCETEST;
     	}
       	if (f & E_MGLOBAL) {	/* ref to globalvar */
       	    /* Generate code */
     	    flags |= GlobalModeFlags (f);
       	    g_getstatic (flags, lval->Name, lval->ConstVal);
       	} else if (f & E_MLOCAL) {
     	    /* ref to localvar */
       	    g_getlocal (flags, lval->ConstVal);
     	} else if (f & E_MCONST) {
     	    /* ref to absolute address */
     	    g_getstatic (flags | CF_ABSOLUTE, lval->ConstVal, 0);
     	} else if (f == E_MEOFFS) {
     	    g_getind (flags, lval->ConstVal);
     	} else if (f != E_MREG) {
     	    g_getind (flags, 0);
     	}
    } else if (f == E_MEOFFS) {
     	/* reference not storable */
     	flags |= TypeOf (lval->Type);
       	g_inc (flags | CF_CONST, lval->ConstVal);
    } else if ((f & E_MEXPR) == 0) {
     	/* Constant of some sort, load it into the primary */
     	LoadConstant (flags, lval);
    }
    /* Are we testing this value? */
    if (lval->Test & E_FORCETEST) {
        /* Yes, force a test */
     	flags |= TypeOf (lval->Type);
       	g_test (flags);
       	lval->Test &= ~E_FORCETEST;
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
    ExprDesc lval;

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
     */
    if (CodeSizeFactor >= 200) {

	/* Calculate the number and size of the parameters */
	FrameParams = Func->ParamCount;
	FrameSize   = Func->ParamSize;
	if (FrameParams > 0 && (Func->Flags & FD_FASTCALL) != 0) {
	    /* Last parameter is not pushed */
	    const SymEntry* LastParam = Func->SymTab->SymTail;
	    FrameSize -= CheckedSizeOf (LastParam->Type);
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

    	unsigned CFlags;
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

     	/* Do some optimization: If we have a constant value to push,
     	 * use a special function that may optimize.
     	 */
       	CFlags = CF_NONE;
       	if (!Ellipsis && CheckedSizeOf (Param->Type) == 1) {
	    CFlags = CF_FORCECHAR;
      	}
	Flags = CF_NONE;
       	if (evalexpr (CFlags, hie1, &lval) == 0) {
       	    /* A constant value */
	    Flags |= CF_CONST;
     	}

      	/* If we don't have an argument spec, accept anything, otherwise
	 * convert the actual argument to the type needed.
	 */
       	if (!Ellipsis) {
	    /* Promote the argument if needed */
       	    assignadjust (Param->Type, &lval);

	    /* If we have a prototype, chars may be pushed as chars */
	    Flags |= CF_FORCECHAR;
       	}

	/* Use the type of the argument for the push */
       	Flags |= TypeOf (lval.Type);

	/* If this is a fastcall function, don't push the last argument */
       	if (ParamCount == Func->ParamCount && (Func->Flags & FD_FASTCALL) != 0) {
	    /* Just load the argument into the primary. This is only needed if
	     * we have a constant argument, otherwise the value is already in
	     * the primary.
	     */
	    if (Flags & CF_CONST) {
	    	exprhs (CF_FORCECHAR, 0, &lval);
	    }
      	} else {
	    unsigned ArgSize = sizeofarg (Flags);
	    if (FrameSize > 0) {
	    	/* We have the space already allocated, store in the frame */
	    	CHECK (FrameSize >= ArgSize);
	    	FrameSize -= ArgSize;
	    	FrameOffs -= ArgSize;
	    	/* Store */
	    	g_putlocal (Flags | CF_NOKEEP, FrameOffs, lval.ConstVal);
	    } else {
	    	/* Push the argument */
	    	g_push (Flags, lval.ConstVal);
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



static void FunctionCall (int k, ExprDesc* lval)
/* Perform a function call. */
{
    FuncDesc*	  Func;	       	  /* Function descriptor */
    int           IsFuncPtr;      /* Flag */
    unsigned 	  ParamSize;	  /* Number of parameter bytes */
    CodeMark 	  Mark = 0;       /* Initialize to keep gcc silent */
    int           PtrOffs = 0;    /* Offset of function pointer on stack */
    int           IsFastCall = 0; /* True if it's a fast call function */
    int           PtrOnStack = 0; /* True if a pointer copy is on stack */

    /* Get a pointer to the function descriptor from the type string */
    Func = GetFuncDesc (lval->Type);

    /* Handle function pointers transparently */
    IsFuncPtr = IsTypeFuncPtr (lval->Type);
    if (IsFuncPtr) {

	/* Check wether it's a fastcall function that has parameters */
	IsFastCall = IsFastCallFunc (lval->Type + 1) && (Func->ParamCount > 0);

	/* Things may be difficult, depending on where the function pointer
	 * resides. If the function pointer is an expression of some sort
	 * (not a local or global variable), we have to evaluate this
	 * expression now and save the result for later. Since calls to
	 * function pointers may be nested, we must save it onto the stack.
	 * For fastcall functions we do also need to place a copy of the
	 * pointer on stack, since we cannot use a/x.
	 */
	PtrOnStack = IsFastCall || ((lval->Flags & (E_MGLOBAL | E_MLOCAL)) == 0);
	if (PtrOnStack) {

	    /* Not a global or local variable, or a fastcall function. Load
	     * the pointer into the primary and mark it as an expression.
	     */
	    exprhs (CF_NONE, k, lval);
	    lval->Flags |= E_MEXPR;

	    /* Remember the code position */
	    Mark = GetCodePos ();

	    /* Push the pointer onto the stack and remember the offset */
	    g_push (CF_PTR, 0);
	    PtrOffs = oursp;
	}

    /* Check for known standard functions and inline them if requested */
    } else if (InlineStdFuncs && IsStdFunc ((const char*) lval->Name)) {

	/* Inline this function */
       	HandleStdFunc (Func, lval);
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
	     	exprhs (CF_NONE, k, lval);
	    }

	    /* Call the function */
	    g_callind (TypeOf (lval->Type+1), ParamSize, PtrOffs);

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
    	++lval->Type;

    } else {

	/* Normal function */
       	g_call (TypeOf (lval->Type), (const char*) lval->Name, ParamSize);

    }
}



static int primary (ExprDesc* lval)
/* This is the lowest level of the expression parser. */
{
    int k;

    /* Initialize fields in the expression stucture */
    lval->Test = 0;             /* No test */
    lval->Sym  = 0;             /* Symbol unknown */

    /* Character and integer constants. */
    if (CurTok.Tok == TOK_ICONST || CurTok.Tok == TOK_CCONST) {
    	lval->Flags = E_MCONST | E_TCONST;
    	lval->Type  = CurTok.Type;
    	lval->ConstVal = CurTok.IVal;
    	NextToken ();
    	return 0;
    }

    /* Process parenthesized subexpression by calling the whole parser
     * recursively.
     */
    if (CurTok.Tok == TOK_LPAREN) {
    	NextToken ();
    	InitExprDesc (lval);            /* Remove any attributes */
    	k = hie0 (lval);
       	ConsumeRParen ();
    	return k;
    }

    /* If we run into an identifier in preprocessing mode, we assume that this
     * is an undefined macro and replace it by a constant value of zero.
     */
    if (Preprocessing && CurTok.Tok == TOK_IDENT) {
        MakeConstIntExpr (lval, 0);
        return 0;
    }

    /* All others may only be used if the expression evaluation is not called
     * recursively by the preprocessor.
     */
    if (Preprocessing) {
       	/* Illegal expression in PP mode */
	Error ("Preprocessor expression expected");
	MakeConstIntExpr (lval, 1);
	return 0;
    }

    /* Identifier? */
    if (CurTok.Tok == TOK_IDENT) {

	SymEntry* Sym;
	ident Ident;

	/* Get a pointer to the symbol table entry */
       	Sym = lval->Sym = FindSym (CurTok.Ident);

	/* Is the symbol known? */
 	if (Sym) {

	    /* We found the symbol - skip the name token */
     	    NextToken ();

	    /* The expression type is the symbol type */
	    lval->Type = Sym->Type;

    	    /* Check for illegal symbol types */
	    CHECK ((Sym->Flags & SC_LABEL) != SC_LABEL);
       	    if (Sym->Flags & SC_TYPE) {
	       	/* Cannot use type symbols */
	       	Error ("Variable identifier expected");
	       	/* Assume an int type to make lval valid */
	       	lval->Flags = E_MLOCAL | E_TLOFFS;
	       	lval->Type = type_int;
	        lval->ConstVal = 0;
	       	return 0;
	    }

	    /* Check for legal symbol types */
       	    if ((Sym->Flags & SC_CONST) == SC_CONST) {
	   	/* Enum or some other numeric constant */
	       	lval->Flags = E_MCONST | E_TCONST;
	    	lval->ConstVal = Sym->V.ConstVal;
	       	return 0;
	    } else if ((Sym->Flags & SC_FUNC) == SC_FUNC) {
	   	/* Function */
	    	lval->Flags = E_MGLOBAL | E_MCONST | E_TGLAB;
	   	lval->Name = (unsigned long) Sym->Name;
	    	lval->ConstVal = 0;
	    } else if ((Sym->Flags & SC_AUTO) == SC_AUTO) {
	    	/* Local variable. If this is a parameter for a variadic
	   	 * function, we have to add some address calculations, and the
	   	 * address is not const.
	   	 */
       	       	if ((Sym->Flags & SC_PARAM) == SC_PARAM && F_IsVariadic (CurrentFunc)) {
	   	    /* Variadic parameter */
	   	    g_leavariadic (Sym->V.Offs - F_GetParamSize (CurrentFunc));
	   	    lval->Flags = E_MEXPR;
	   	    lval->ConstVal = 0;
	   	} else {
	   	    /* Normal parameter */
	   	    lval->Flags = E_MLOCAL | E_TLOFFS;
	   	    lval->ConstVal = Sym->V.Offs;
	   	}
	    } else if ((Sym->Flags & SC_REGISTER) == SC_REGISTER) {
	    	/* Register variable, zero page based */
	    	lval->Flags = E_MGLOBAL | E_MCONST | E_TREGISTER;
	    	lval->Name  = Sym->V.R.RegOffs;
	    	lval->ConstVal = 0;
	    } else if ((Sym->Flags & SC_STATIC) == SC_STATIC) {
	    	/* Static variable */
	   	if (Sym->Flags & (SC_EXTERN | SC_STORAGE)) {
	    	    lval->Flags = E_MGLOBAL | E_MCONST | E_TGLAB;
	    	    lval->Name = (unsigned long) Sym->Name;
	   	} else {
	    	    lval->Flags = E_MGLOBAL | E_MCONST | E_TLLAB;
	   	    lval->Name = Sym->V.Label;
	   	}
	    	lval->ConstVal = 0;
       	    } else {
	    	/* Local static variable */
	    	lval->Flags = E_MGLOBAL | E_MCONST | E_TLLAB;
	    	lval->Name  = Sym->V.Offs;
	    	lval->ConstVal = 0;
    	    }

	    /* The symbol is referenced now */
	    Sym->Flags |= SC_REF;
       	    if (IsTypeFunc (lval->Type) || IsTypeArray (lval->Type)) {
	    	return 0;
	    }
   	    return 1;
	}

     	/* We did not find the symbol. Remember the name, then skip it */
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
	    lval->Type  = Sym->Type;
	    lval->Flags = E_MGLOBAL | E_MCONST | E_TGLAB;
       	    lval->Name  = (unsigned long) Sym->Name;
	    lval->ConstVal = 0;
   	    return 0;

	} else {

	    /* Undeclared Variable */
	    Sym = AddLocalSym (Ident, type_int, SC_AUTO | SC_REF, 0);
	    lval->Flags = E_MLOCAL | E_TLOFFS;
	    lval->Type = type_int;
	    lval->ConstVal = 0;
	    Error ("Undefined symbol: `%s'", Ident);
	    return 1;

	}
    }

    /* String literal? */
    if (CurTok.Tok == TOK_SCONST) {
   	lval->Flags = E_MCONST | E_TLIT;
       	lval->ConstVal = CurTok.IVal;
	lval->Type  = GetCharArrayType (GetLiteralPoolOffs () - CurTok.IVal);
    	NextToken ();
	return 0;
    }

    /* ASM statement? */
    if (CurTok.Tok == TOK_ASM) {
	AsmStatement ();
	lval->Type  = type_void;
	lval->Flags = E_MEXPR;
	lval->ConstVal = 0;
	return 0;
    }

    /* __AX__ and __EAX__ pseudo values? */
    if (CurTok.Tok == TOK_AX || CurTok.Tok == TOK_EAX) {
       	lval->Type  = (CurTok.Tok == TOK_AX)? type_uint : type_ulong;
   	lval->Flags = E_MREG;
	lval->Test &= ~E_CC;
   	lval->ConstVal = 0;
	NextToken ();
    	return 1;    	  	/* May be used as lvalue */
    }

    /* Illegal primary. */
    Error ("Expression expected");
    MakeConstIntExpr (lval, 1);
    return 0;
}



static int arrayref (int k, ExprDesc* lval)
/* Handle an array reference */
{
    unsigned lflags;
    unsigned rflags;
    int ConstBaseAddr;
    int ConstSubAddr;
    int l;
    ExprDesc lval2;
    CodeMark Mark1;
    CodeMark Mark2;
    type* tptr1;
    type* tptr2;


    /* Skip the bracket */
    NextToken ();

    /* Get the type of left side */
    tptr1 = lval->Type;

    /* We can apply a special treatment for arrays that have a const base
     * address. This is true for most arrays and will produce a lot better
     * code. Check if this is a const base address.
     */
    lflags = lval->Flags & ~E_MCTYPE;
    ConstBaseAddr = (lflags == E_MCONST)       || /* Constant numeric address */
       	       	     (lflags & E_MGLOBAL) != 0 || /* Static array, or ... */
       	       	     lflags == E_MLOCAL;     	  /* Local array */

    /* If we have a constant base, we delay the address fetch */
    Mark1 = GetCodePos ();
    Mark2 = 0;	       	/* Silence gcc */
    if (!ConstBaseAddr) {
    	/* Get a pointer to the array into the primary */
    	exprhs (CF_NONE, k, lval);

    	/* Get the array pointer on stack. Do not push more than 16
    	 * bit, even if this value is greater, since we cannot handle
    	 * other than 16bit stuff when doing indexing.
    	 */
    	Mark2 = GetCodePos ();
    	g_push (CF_PTR, 0);
    }

    /* TOS now contains ptr to array elements. Get the subscript. */
    l = hie0 (&lval2);
    if (l == 0 && lval2.Flags == E_MCONST) {

     	/* The array subscript is a constant - remove value from stack */
    	if (!ConstBaseAddr) {
     	    RemoveCode (Mark2);
     	    pop (CF_PTR);
      	} else {
    	    /* Get an array pointer into the primary */
    	    exprhs (CF_NONE, k, lval);
    	}

     	if (IsClassPtr (tptr1)) {

     	    /* Scale the subscript value according to element size */
     	    lval2.ConstVal *= CheckedPSizeOf (tptr1);

	    /* Remove code for lhs load */
	    RemoveCode (Mark1);

    	    /* Handle constant base array on stack. Be sure NOT to
    	     * handle pointers the same way, this won't work.
    	     */
    	    if (IsTypeArray (tptr1) &&
    	       	((lval->Flags & ~E_MCTYPE) == E_MCONST ||
    	   	(lval->Flags & ~E_MCTYPE) == E_MLOCAL ||
    	   	(lval->Flags & E_MGLOBAL) != 0 ||
    	   	(lval->Flags == E_MEOFFS))) {
    	   	lval->ConstVal += lval2.ConstVal;

    	    } else {
	   	/* Pointer - load into primary and remember offset */
	   	if ((lval->Flags & E_MEXPR) == 0 || k != 0) {
	   	    exprhs (CF_NONE, k, lval);
	   	}
	   	lval->ConstVal = lval2.ConstVal;
	   	lval->Flags = E_MEOFFS;
	    }

       	    /* Result is of element type */
	    lval->Type = Indirect (tptr1);

	    /* Done */
    	    goto end_array;

       	} else if (IsClassPtr (tptr2 = lval2.Type)) {
    	    /* Subscript is pointer, get element type */
    	    lval2.Type = Indirect (tptr2);

    	    /* Scale the rhs value in the primary register */
    	    g_scale (TypeOf (tptr1), CheckedSizeOf (lval2.Type));
    	    /* */
    	    lval->Type = lval2.Type;
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
        exprhs (CF_NONE, l, &lval2);

	tptr2 = lval2.Type;
	if (IsClassPtr (tptr1)) {

	    /* Get the element type */
	    lval->Type = Indirect (tptr1);

       	    /* Indexing is based on int's, so we will just use the integer
	     * portion of the index (which is in (e)ax, so there's no further
	     * action required).
	     */
	    g_scale (CF_INT, CheckedSizeOf (lval->Type));

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
	    	exprhs (CF_NONE, k, lval);
	    	ConstBaseAddr = 0;
	    } else {
	        g_swap (CF_INT);
	    }

	    /* Scale it */
	    g_scale (TypeOf (tptr1), CheckedSizeOf (lval2.Type));
	    lval->Type = lval2.Type;
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

       	    if (ConstSubAddr && CheckedSizeOf (lval->Type) == SIZEOF_CHAR) {

	    	type* SavedType;

	    	/* Reverse the order of evaluation */
	    	unsigned flags = (CheckedSizeOf (lval2.Type) == SIZEOF_CHAR)? CF_CHAR : CF_INT;
    	     	RemoveCode (Mark2);

	    	/* Get a pointer to the array into the primary. We have changed
	    	 * Type above but we need the original type to load the
	    	 * address, so restore it temporarily.
	    	 */
	    	SavedType = lval->Type;
	     	lval->Type = tptr1;
	    	exprhs (CF_NONE, k, lval);
	    	lval->Type = SavedType;

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
	     	    g_inc (CF_INT | CF_UNSIGNED, lval->ConstVal);
	     	} else if (lflags == E_MLOCAL) {
	       	    /* Base address is a local variable address */
	   	    if (IsTypeArray (tptr1)) {
	     	        g_addaddr_local (CF_INT, lval->ConstVal);
	   	    } else {
	   		g_addlocal (CF_PTR, lval->ConstVal);
	   	    }
	     	} else {
	     	    /* Base address is a static variable address */
	     	    unsigned flags = CF_INT;
	   	    flags |= GlobalModeFlags (lval->Flags);
	   	    if (IsTypeArray (tptr1)) {
	     	        g_addaddr_static (flags, lval->Name, lval->ConstVal);
	   	    } else {
	   		g_addstatic (flags, lval->Name, lval->ConstVal);
	   	    }
	     	}
	    }
	}
    }
    lval->Flags = E_MEXPR;
end_array:
    ConsumeRBrack ();
    return !IsTypeArray (lval->Type);

}



static int structref (int k, ExprDesc* lval)
/* Process struct field after . or ->. */
{
    ident Ident;
    SymEntry* Field;
    int flags;

    /* Skip the token and check for an identifier */
    NextToken ();
    if (CurTok.Tok != TOK_IDENT) {
    	Error ("Identifier expected");
    	lval->Type = type_int;
    	return 0;
    }

    /* Get the symbol table entry and check for a struct field */
    strcpy (Ident, CurTok.Ident);
    NextToken ();
    Field = FindStructField (lval->Type, Ident);
    if (Field == 0) {
     	Error ("Struct/union has no field named `%s'", Ident);
       	lval->Type = type_int;
     	return 0;
    }

    /* If we have constant input data, the result is also constant */
    flags = lval->Flags & ~E_MCTYPE;
    if (flags == E_MCONST ||
       	(k == 0 && (flags == E_MLOCAL ||
	       	    (flags & E_MGLOBAL) != 0 ||
	       	    lval->Flags  == E_MEOFFS))) {
	lval->ConstVal += Field->V.Offs;
    } else {
	if ((flags & E_MEXPR) == 0 || k != 0) {
	    exprhs (CF_NONE, k, lval);
	}
	lval->ConstVal = Field->V.Offs;
	lval->Flags = E_MEOFFS;
    }
    lval->Type = Field->Type;
    return !IsTypeArray (Field->Type);
}



static int hie11 (ExprDesc *lval)
/* Handle compound types (structs and arrays) */
{
    int k;
    type* tptr;


    k = primary (lval);
    if (CurTok.Tok < TOK_LBRACK || CurTok.Tok > TOK_PTR_REF) {
	/* Not for us */
       	return k;
    }

    while (1) {

	if (CurTok.Tok == TOK_LBRACK) {

	    /* Array reference */
	    k = arrayref (k, lval);

	} else if (CurTok.Tok == TOK_LPAREN) {

	    /* Function call. Skip the opening parenthesis */
	    NextToken ();
	    tptr = lval->Type;
	    if (IsTypeFunc (lval->Type) || IsTypeFuncPtr (lval->Type)) {

		/* Call the function */
		FunctionCall (k, lval);

		/* Result is in the primary register */
     	    	lval->Flags = E_MEXPR;

		/* Set to result */
     	    	lval->Type = GetFuncReturn (lval->Type);

     	    } else {
     	    	Error ("Illegal function call");
     	    }
     	    k = 0;

     	} else if (CurTok.Tok == TOK_DOT) {

     	    if (!IsClassStruct (lval->Type)) {
     	   	Error ("Struct expected");
     	    }
     	    k = structref (0, lval);

     	} else if (CurTok.Tok == TOK_PTR_REF) {

     	    tptr = lval->Type;
     	    if (tptr[0] != T_PTR || (tptr[1] & T_STRUCT) == 0) {
     	   	Error ("Struct pointer expected");
     	    }
     	    k = structref (k, lval);

     	} else {
	    return k;
	}
    }
}



void Store (ExprDesc* lval, const type* StoreType)
/* Store the primary register into the location denoted by lval. If StoreType
 * is given, use this type when storing instead of lval->Type. If StoreType
 * is NULL, use lval->Type instead.
 */
{
    unsigned Flags;

    unsigned f = lval->Flags;

    /* If StoreType was not given, use lval->Type instead */
    if (StoreType == 0) {
        StoreType = lval->Type;
    }

    /* Get the code generator flags */
    Flags = TypeOf (StoreType);
    if (f & E_MGLOBAL) {
     	Flags |= GlobalModeFlags (f);
     	if (lval->Test) {
   	    /* Just testing */
       	    Flags |= CF_TEST;
   	}

    	/* Generate code */
       	g_putstatic (Flags, lval->Name, lval->ConstVal);

    } else if (f & E_MLOCAL) {
        /* Store an auto variable */
       	g_putlocal (Flags, lval->ConstVal, 0);
    } else if (f == E_MEOFFS) {
        /* Store indirect with offset */
    	g_putind (Flags, lval->ConstVal);
    } else if (f != E_MREG) {
    	if (f & E_MEXPR) {
            /* Indirect without offset */
    	    g_putind (Flags, 0);
    	} else {
    	    /* Store into absolute address */
    	    g_putstatic (Flags | CF_ABSOLUTE, lval->ConstVal, 0);
    	}
    }

    /* Assume that each one of the stores will invalidate CC */
    lval->Test &= ~E_CC;
}



static void pre_incdec (ExprDesc* lval, void (*inc) (unsigned, unsigned long))
/* Handle --i and ++i */
{
    int k;
    unsigned flags;
    unsigned long val;

    NextToken ();
    if ((k = hie10 (lval)) == 0) {
    	Error ("Invalid lvalue");
    	return;
    }

    /* Get the data type */
    flags = TypeOf (lval->Type) | CF_FORCECHAR | CF_CONST;

    /* Get the increment value in bytes */
    val = (lval->Type [0] == T_PTR)? CheckedPSizeOf (lval->Type) : 1;

    /* We're currently only able to handle some adressing modes */
    if ((lval->Flags & E_MGLOBAL) == 0 && 	/* Global address? */
	(lval->Flags & E_MLOCAL) == 0  && 	/* Local address? */
	(lval->Flags & E_MCONST) == 0  &&  	/* Constant address? */
	(lval->Flags & E_MEXPR) == 0) { 	/* Address in a/x? */

	/* Use generic code. Push the address if needed */
	PushAddr (lval);

	/* Fetch the value */
	exprhs (CF_NONE, k, lval);

	/* Increment value in primary */
       	inc (flags, val);

	/* Store the result back */
	Store (lval, 0);

    } else {

	/* Special code for some addressing modes - use the special += ops */
	if (lval->Flags & E_MGLOBAL) {
	    flags |= GlobalModeFlags (lval->Flags);
	    if (inc == g_inc) {
	     	g_addeqstatic (flags, lval->Name, lval->ConstVal, val);
	    } else {
	     	g_subeqstatic (flags, lval->Name, lval->ConstVal, val);
	    }
	} else if (lval->Flags & E_MLOCAL) {
	    /* ref to localvar */
	    if (inc == g_inc) {
	     	g_addeqlocal (flags, lval->ConstVal, val);
	    } else {
	     	g_subeqlocal (flags, lval->ConstVal, val);
	    }
	} else if (lval->Flags & E_MCONST) {
	    /* ref to absolute address */
	    flags |= CF_ABSOLUTE;
	    if (inc == g_inc) {
	     	g_addeqstatic (flags, lval->ConstVal, 0, val);
	    } else {
	     	g_subeqstatic (flags, lval->ConstVal, 0, val);
	    }
	} else if (lval->Flags & E_MEXPR) {
	    /* Address in a/x, check if we have an offset */
	    unsigned Offs = (lval->Flags == E_MEOFFS)? lval->ConstVal : 0;
	    if (inc == g_inc) {
	     	g_addeqind (flags, Offs, val);
	    } else {
	       	g_subeqind (flags, Offs, val);
	    }
	} else {
	    Internal ("Invalid addressing mode");
	}

    }

    /* Result is an expression */
    lval->Flags = E_MEXPR;
}



static void post_incdec (ExprDesc* lval, int k, void (*inc) (unsigned, unsigned long))
/* Handle i-- and i++ */
{
    unsigned flags;

    NextToken ();
    if (k == 0) {
    	Error ("Invalid lvalue");
       	return;
    }

    /* Get the data type */
    flags = TypeOf (lval->Type);

    /* Push the address if needed */
    PushAddr (lval);

    /* Fetch the value and save it (since it's the result of the expression) */
    exprhs (CF_NONE, 1, lval);
    g_save (flags | CF_FORCECHAR);

    /* If we have a pointer expression, increment by the size of the type */
    if (lval->Type[0] == T_PTR) {
    	inc (flags | CF_CONST | CF_FORCECHAR, CheckedSizeOf (lval->Type + 1));
    } else {
     	inc (flags | CF_CONST | CF_FORCECHAR, 1);
    }

    /* Store the result back */
    Store (lval, 0);

    /* Restore the original value */
    g_restore (flags | CF_FORCECHAR);
    lval->Flags = E_MEXPR;
}



static void unaryop (int tok, ExprDesc* lval)
/* Handle unary -/+ and ~ */
{
    int k;
    unsigned flags;

    NextToken ();
    k = hie10 (lval);
    if (k == 0 && (lval->Flags & E_MCONST) != 0) {
    	/* Value is constant */
	switch (tok) {
	    case TOK_MINUS: lval->ConstVal = -lval->ConstVal;	break;
	    case TOK_PLUS:  	  				break;
	    case TOK_COMP:  lval->ConstVal = ~lval->ConstVal; 	break;
	    default:	    Internal ("Unexpected token: %d", tok);
	}
    } else {
    	/* Value is not constant */
     	exprhs (CF_NONE, k, lval);

    	/* Get the type of the expression */
    	flags = TypeOf (lval->Type);

    	/* Handle the operation */
	switch (tok) {
       	    case TOK_MINUS: g_neg (flags);  break;
	    case TOK_PLUS:  	       	    break;
	    case TOK_COMP:  g_com (flags);  break;
	    default:	Internal ("Unexpected token: %d", tok);
	}
     	lval->Flags = E_MEXPR;
    }
}



int hie10 (ExprDesc* lval)
/* Handle ++, --, !, unary - etc. */
{
    int k;
    type* t;

    switch (CurTok.Tok) {

     	case TOK_INC:
     	    pre_incdec (lval, g_inc);
     	    return 0;

     	case TOK_DEC:
     	    pre_incdec (lval, g_dec);
     	    return 0;

	case TOK_PLUS:
     	case TOK_MINUS:
     	case TOK_COMP:
     	    unaryop (CurTok.Tok, lval);
     	    return 0;

     	case TOK_BOOL_NOT:
     	    NextToken ();
    	    if (evalexpr (CF_NONE, hie10, lval) == 0) {
    	       	/* Constant expression */
    	       	lval->ConstVal = !lval->ConstVal;
    	    } else {
    	       	g_bneg (TypeOf (lval->Type));
    	       	lval->Test |= E_CC;			/* bneg will set cc */
    	       	lval->Flags = E_MEXPR;	 	/* say it's an expr */
    	    }
     	    return 0;  	      	     		/* expr not storable */

     	case TOK_STAR:
     	    NextToken ();
    	    if (evalexpr (CF_NONE, hie10, lval) != 0) {
    	       	/* Expression is not const, indirect value loaded into primary */
	       	lval->Flags = E_MEXPR;
	       	lval->ConstVal = 0;		/* Offset is zero now */
    	    }
            /* If the expression is already a pointer to function, the
             * additional dereferencing operator must be ignored.
             */
            if (IsTypeFuncPtr (lval->Type)) {               
                /* Expression not storable */
                return 0;
            } else {
                if (IsClassPtr (lval->Type)) {                
                    lval->Type = Indirect (lval->Type);
                } else {
                    Error ("Illegal indirection");
                }
                return 1;
            }
            break;

     	case TOK_AND:
     	    NextToken ();
     	    k = hie10 (lval);
	    /* The & operator may be applied to any lvalue, and it may be
	     * applied to functions, even if they're no lvalues.
	     */
     	    if (k == 0 && !IsTypeFunc (lval->Type)) {
	       	/* Allow the & operator with an array */
	       	if (!IsTypeArray (lval->Type)) {
     	       	    Error ("Illegal address");
	       	}
     	    } else {
	       	t = TypeAlloc (TypeLen (lval->Type) + 2);
	       	t [0] = T_PTR;
	       	TypeCpy (t + 1, lval->Type);
	       	lval->Type = t;
	    }
     	    return 0;

     	case TOK_SIZEOF:
     	    NextToken ();
       	    if (istypeexpr ()) {
    	       	type Type[MAXTYPELEN];
     	       	NextToken ();
	       	lval->ConstVal = CheckedSizeOf (ParseType (Type));
     	       	ConsumeRParen ();
     	    } else {
    	       	/* Remember the output queue pointer */
    	       	CodeMark Mark = GetCodePos ();
     	       	hie10 (lval);
     	       	lval->ConstVal = CheckedSizeOf (lval->Type);
    	       	/* Remove any generated code */
    	       	RemoveCode (Mark);
     	    }
     	    lval->Flags = E_MCONST | E_TCONST;
     	    lval->Type = type_uint;
	    lval->Test &= ~E_CC;
     	    return 0;

     	default:
       	    if (istypeexpr ()) {
     	       	/* A cast */
    	       	return TypeCast (lval);
     	    }
    }

    k = hie11 (lval);
    switch (CurTok.Tok) {
     	case TOK_INC:
       	    post_incdec (lval, k, g_inc);
     	    return 0;

     	case TOK_DEC:
     	    post_incdec (lval, k, g_dec);
     	    return 0;

      	default:
     	    return k;
    }
}



static int hie_internal (const GenDesc** ops,  	/* List of generators */
       	                 ExprDesc* lval,	/* parent expr's lval */
       	                 int (*hienext) (ExprDesc*),
	       	       	 int* UsedGen) 		/* next higher level */
/* Helper function */
{
    int k;
    ExprDesc lval2;
    CodeMark Mark1;
    CodeMark Mark2;
    const GenDesc* Gen;
    token_t tok;       			/* The operator token */
    unsigned ltype, type;
    int rconst;	       	       	       	/* Operand is a constant */


    k = hienext (lval);

    *UsedGen = 0;
    while ((Gen = FindGen (CurTok.Tok, ops)) != 0) {

	/* Tell the caller that we handled it's ops */
	*UsedGen = 1;

	/* All operators that call this function expect an int on the lhs */
	if (!IsClassInt (lval->Type)) {
	    Error ("Integer expression expected");
	}

	/* Remember the operator token, then skip it */
       	tok = CurTok.Tok;
	NextToken ();

	/* Get the lhs on stack */
       	Mark1 = GetCodePos ();
	ltype = TypeOf (lval->Type);
	if (k == 0 && lval->Flags == E_MCONST) {
	    /* Constant value */
	    Mark2 = GetCodePos ();
       	    g_push (ltype | CF_CONST, lval->ConstVal);
	} else {
	    /* Value not constant */
	    exprhs (CF_NONE, k, lval);
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
	if (k == 0 && lval->Flags == E_MCONST && rconst) {

	    /* Both operands are constant, remove the generated code */
    	    RemoveCode (Mark1);
	    pop (ltype);

	    /* Evaluate the result */
	    lval->ConstVal = kcalc (tok, lval->ConstVal, lval2.ConstVal);

	    /* Get the type of the result */
	    lval->Type = promoteint (lval->Type, lval2.Type);

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
	     	if (tok == TOK_DIV && lval2.ConstVal == 0) {
	      	    Error ("Division by zero");
	     	} else if (tok == TOK_MOD && lval2.ConstVal == 0) {
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
	    lval->Type = promoteint (lval->Type, lval2.Type);

	    /* Generate code */
	    Gen->Func (type, lval2.ConstVal);
	    lval->Flags = E_MEXPR;
	}

	/* We have a rvalue now */
	k = 0;
    }

    return k;
}



static int hie_compare (const GenDesc** ops,	/* List of generators */
       	                ExprDesc* lval,		/* parent expr's lval */
       	                int (*hienext) (ExprDesc*))
/* Helper function for the compare operators */
{
    int k;
    ExprDesc lval2;
    CodeMark Mark1;
    CodeMark Mark2;
    const GenDesc* Gen;
    token_t tok;			/* The operator token */
    unsigned ltype;
    int rconst;	       	       	       	/* Operand is a constant */


    k = hienext (lval);

    while ((Gen = FindGen (CurTok.Tok, ops)) != 0) {

	/* Remember the operator token, then skip it */
       	tok = CurTok.Tok;
	NextToken ();

	/* Get the lhs on stack */
	Mark1 = GetCodePos ();
	ltype = TypeOf (lval->Type);
	if (k == 0 && lval->Flags == E_MCONST) {
	    /* Constant value */
	    Mark2 = GetCodePos ();
       	    g_push (ltype | CF_CONST, lval->ConstVal);
	} else {
	    /* Value not constant */
	    exprhs (CF_NONE, k, lval);
	    Mark2 = GetCodePos ();
	    g_push (ltype, 0);
      	}

	/* Get the right hand side */
	rconst = (evalexpr (CF_NONE, hienext, &lval2) == 0);

	/* Make sure, the types are compatible */
	if (IsClassInt (lval->Type)) {
	    if (!IsClassInt (lval2.Type) && !(IsClassPtr(lval2.Type) && IsNullPtr(lval))) {
	   	Error ("Incompatible types");
	    }
	} else if (IsClassPtr (lval->Type)) {
	    if (IsClassPtr (lval2.Type)) {
	   	/* Both pointers are allowed in comparison if they point to
	   	 * the same type, or if one of them is a void pointer.
	         */
       	       	type* left  = Indirect (lval->Type);
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
	if (k == 0 && lval->Flags == E_MCONST && rconst) {

	    /* Both operands are constant, remove the generated code */
    	    RemoveCode (Mark1);
	    pop (ltype);

	    /* Evaluate the result */
	    lval->ConstVal = kcalc (tok, lval->ConstVal, lval2.ConstVal);

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
	    if (IsTypeChar (lval->Type) && (IsTypeChar (lval2.Type) || rconst)) {
	       	flags |= CF_CHAR;
	       	if (IsSignUnsigned (lval->Type) || IsSignUnsigned (lval2.Type)) {
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
	    lval->Flags = E_MEXPR;
	}

	/* Result type is always int */
       	lval->Type = type_int;

	/* We have a rvalue now, condition codes are set */
	k = 0;
	lval->Test |= E_CC;
    }

    return k;
}



static int hie9 (ExprDesc *lval)
/* Process * and / operators. */
{
    static const GenDesc* hie9_ops [] = {
	&GenMUL, &GenDIV, &GenMOD, 0
    };
    int UsedGen;

    return hie_internal (hie9_ops, lval, hie10, &UsedGen);
}



static void parseadd (int k, ExprDesc* lval)
/* Parse an expression with the binary plus operator. lval contains the
 * unprocessed left hand side of the expression and will contain the
 * result of the expression on return.
 */
{
    ExprDesc lval2;
    unsigned flags;         	/* Operation flags */
    CodeMark Mark;		/* Remember code position */
    type* lhst;	   	    	/* Type of left hand side */
    type* rhst;	     	    	/* Type of right hand side */


    /* Skip the PLUS token */
    NextToken ();

    /* Get the left hand side type, initialize operation flags */
    lhst = lval->Type;
    flags = 0;

    /* Check for constness on both sides */
    if (k == 0 && (lval->Flags & E_MCONST) != 0) {

    	/* The left hand side is a constant. Good. Get rhs */
	k = hie9 (&lval2);
       	if (k == 0 && lval2.Flags == E_MCONST) {

       	    /* Right hand side is also constant. Get the rhs type */
    	    rhst = lval2.Type;

    	    /* Both expressions are constants. Check for pointer arithmetic */
       	    if (IsClassPtr (lhst) && IsClassInt (rhst)) {
       	    	/* Left is pointer, right is int, must scale rhs */
       	       	lval->ConstVal += lval2.ConstVal * CheckedPSizeOf (lhst);
    	    	/* Result type is a pointer */
    	    } else if (IsClassInt (lhst) && IsClassPtr (rhst)) {
    	    	/* Left is int, right is pointer, must scale lhs */
       	       	lval->ConstVal = lval->ConstVal * CheckedPSizeOf (rhst) + lval2.ConstVal;
    	    	/* Result type is a pointer */
    	    	lval->Type = lval2.Type;
       	    } else if (IsClassInt (lhst) && IsClassInt (rhst)) {
    	    	/* Integer addition */
    	    	lval->ConstVal += lval2.ConstVal;
    	    	typeadjust (lval, &lval2, 1);
    	    } else {
       	       	/* OOPS */
    	    	Error ("Invalid operands for binary operator `+'");
    	    }

       	    /* Result is constant, condition codes not set */
       	    lval->Test &= ~E_CC;

    	} else {

	    /* lhs is a constant and rhs is not constant. Load rhs into
	     * the primary.
	     */
	    exprhs (CF_NONE, k, &lval2);

       	    /* Beware: The check above (for lhs) lets not only pass numeric
	     * constants, but also constant addresses (labels), maybe even
	     * with an offset. We have to check for that here.
	     */

    	    /* First, get the rhs type. */
    	    rhst = lval2.Type;

	    /* Setup flags */
	    if (lval->Flags == E_MCONST) {
	    	/* A numerical constant */
	    	flags |= CF_CONST;
	    } else {
	    	/* Constant address label */
	    	flags |= GlobalModeFlags (lval->Flags) | CF_CONSTADDR;
	    }

    	    /* Check for pointer arithmetic */
    	    if (IsClassPtr (lhst) && IsClassInt (rhst)) {
    	    	/* Left is pointer, right is int, must scale rhs */
    	    	g_scale (CF_INT, CheckedPSizeOf (lhst));
    	    	/* Operate on pointers, result type is a pointer */
      	    	flags |= CF_PTR;
		/* Generate the code for the add */
		if (lval->Flags == E_MCONST) {
		    /* Numeric constant */
		    g_inc (flags, lval->ConstVal);
		} else {
		    /* Constant address */
		    g_addaddr_static (flags, lval->Name, lval->ConstVal);
		}
    	    } else if (IsClassInt (lhst) && IsClassPtr (rhst)) {

    	      	/* Left is int, right is pointer, must scale lhs. */
		unsigned ScaleFactor = CheckedPSizeOf (rhst);

       	       	/* Operate on pointers, result type is a pointer */
		flags |= CF_PTR;
		lval->Type = lval2.Type;

		/* Since we do already have rhs in the primary, if lhs is
		 * not a numeric constant, and the scale factor is not one
		 * (no scaling), we must take the long way over the stack.
		 */
		if (lval->Flags == E_MCONST) {
		    /* Numeric constant, scale lhs */
		    lval->ConstVal *= ScaleFactor;
		    /* Generate the code for the add */
		    g_inc (flags, lval->ConstVal);
		} else if (ScaleFactor == 1) {
		    /* Constant address but no need to scale */
		    g_addaddr_static (flags, lval->Name, lval->ConstVal);
		} else {
		    /* Constant address that must be scaled */
       	       	    g_push (TypeOf (lval2.Type), 0);   	/* rhs --> stack */
		    g_getimmed (flags, lval->Name, lval->ConstVal);
		    g_scale (CF_PTR, ScaleFactor);
		    g_add (CF_PTR, 0);
		}
       	    } else if (IsClassInt (lhst) && IsClassInt (rhst)) {
    	      	/* Integer addition */
       	       	flags |= typeadjust (lval, &lval2, 1);
		/* Generate the code for the add */
		if (lval->Flags == E_MCONST) {
		    /* Numeric constant */
		    g_inc (flags, lval->ConstVal);
		} else {
		    /* Constant address */
		    g_addaddr_static (flags, lval->Name, lval->ConstVal);
		}
    	    } else {
       	       	/* OOPS */
    	    	Error ("Invalid operands for binary operator `+'");
       	    }

    	    /* Result is in primary register */
    	    lval->Flags = E_MEXPR;
    	    lval->Test &= ~E_CC;

       	}

    } else {

    	/* Left hand side is not constant. Get the value onto the stack. */
    	exprhs (CF_NONE, k, lval);		/* --> primary register */
       	Mark = GetCodePos ();
    	g_push (TypeOf (lval->Type), 0);	/* --> stack */

    	/* Evaluate the rhs */
       	if (evalexpr (CF_NONE, hie9, &lval2) == 0) {

       	    /* Right hand side is a constant. Get the rhs type */
    	    rhst = lval2.Type;

      	    /* Remove pushed value from stack */
    	    RemoveCode (Mark);
    	    pop (TypeOf (lval->Type));

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
    	      	lval->Type = lval2.Type;
       	    } else if (IsClassInt (lhst) && IsClassInt (rhst)) {
    	      	/* Integer addition */
    	      	flags = typeadjust (lval, &lval2, 1);
       	    } else {
       	       	/* OOPS */
    	    	Error ("Invalid operands for binary operator `+'");
    	    }

    	    /* Generate code for the add */
       	    g_inc (flags | CF_CONST, lval2.ConstVal);

    	    /* Result is in primary register */
    	    lval->Flags = E_MEXPR;
    	    lval->Test &= ~E_CC;

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
    	    	g_swap (CF_INT);  		/* Swap TOS and primary */
    	    	g_scale (CF_INT, CheckedPSizeOf (rhst));
    	      	/* Operate on pointers, result type is a pointer */
    	      	flags = CF_PTR;
    	      	lval->Type = lval2.Type;
       	    } else if (IsClassInt (lhst) && IsClassInt (rhst)) {
    	      	/* Integer addition */
       	       	flags = typeadjust (lval, &lval2, 0);
    	    } else {
       	       	/* OOPS */
    	    	Error ("Invalid operands for binary operator `+'");
    	    }

    	    /* Generate code for the add */
       	    g_add (flags, 0);

    	    /* Result is in primary register */
       	    lval->Flags = E_MEXPR;
    	    lval->Test &= ~E_CC;

       	}

    }
}



static void parsesub (int k, ExprDesc* lval)
/* Parse an expression with the binary minus operator. lval contains the
 * unprocessed left hand side of the expression and will contain the
 * result of the expression on return.
 */
{
    ExprDesc lval2;
    unsigned flags;         	/* Operation flags */
    type* lhst;	    	    	/* Type of left hand side */
    type* rhst;	    	    	/* Type of right hand side */
    CodeMark Mark1;		/* Save position of output queue */
    CodeMark Mark2;    		/* Another position in the queue */
    int rscale;     	    	/* Scale factor for the result */


    /* Skip the MINUS token */
    NextToken ();

    /* Get the left hand side type, initialize operation flags */
    lhst = lval->Type;
    flags = 0;
    rscale = 1;	     	    	/* Scale by 1, that is, don't scale */

    /* Remember the output queue position, then bring the value onto the stack */
    Mark1 = GetCodePos ();
    exprhs (CF_NONE, k, lval); 	/* --> primary register */
    Mark2 = GetCodePos ();
    g_push (TypeOf (lhst), 0);	/* --> stack */

    /* Parse the right hand side */
    if (evalexpr (CF_NONE, hie9, &lval2) == 0) {

    	/* The right hand side is constant. Get the rhs type. */
       	rhst = lval2.Type;

    	/* Check left hand side */
    	if (k == 0 && (lval->Flags & E_MCONST) != 0) {

    	    /* Both sides are constant, remove generated code */
      	    RemoveCode (Mark1);
    	    pop (TypeOf (lhst));	/* Clean up the stack */

    	    /* Check for pointer arithmetic */
    	    if (IsClassPtr (lhst) && IsClassInt (rhst)) {
    	    	/* Left is pointer, right is int, must scale rhs */
    	    	lval->ConstVal -= lval2.ConstVal * CheckedPSizeOf (lhst);
    	    	/* Operate on pointers, result type is a pointer */
    	    } else if (IsClassPtr (lhst) && IsClassPtr (rhst)) {
    	    	/* Left is pointer, right is pointer, must scale result */
    	    	if (TypeCmp (Indirect (lhst), Indirect (rhst)) < TC_QUAL_DIFF) {
    	    	    Error ("Incompatible pointer types");
    	    	} else {
    	    	    lval->ConstVal = (lval->ConstVal - lval2.ConstVal) /
                                      CheckedPSizeOf (lhst);
    	    	}
    	    	/* Operate on pointers, result type is an integer */
    	    	lval->Type = type_int;
    	    } else if (IsClassInt (lhst) && IsClassInt (rhst)) {
    	    	/* Integer subtraction */
       	       	typeadjust (lval, &lval2, 1);
    	    	lval->ConstVal -= lval2.ConstVal;
    	    } else {
    	    	/* OOPS */
    	    	Error ("Invalid operands for binary operator `-'");
    	    }

    	    /* Result is constant, condition codes not set */
    	    /* lval->Flags = E_MCONST; ### */
    	    lval->Test &= ~E_CC;

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
    	    	lval->Type = type_int;
    	    } else if (IsClassInt (lhst) && IsClassInt (rhst)) {
    	    	/* Integer subtraction */
       	       	flags = typeadjust (lval, &lval2, 1);
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

 	    /* Result is in primary register */
 	    lval->Flags = E_MEXPR;
 	    lval->Test &= ~E_CC;

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
 	    lval->Type = type_int;
 	} else if (IsClassInt (lhst) && IsClassInt (rhst)) {
 	    /* Integer subtraction. If the left hand side descriptor says that
	     * the lhs is const, we have to remove this mark, since this is no
	     * longer true, lhs is on stack instead.
	     */
	    if (lval->Flags == E_MCONST) {
		lval->Flags = E_MEXPR;
	    }
	    /* Adjust operand types */
 	    flags = typeadjust (lval, &lval2, 0);
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

	/* Result is in primary register */
	lval->Flags = E_MEXPR;
	lval->Test &= ~E_CC;
    }
}



static int hie8 (ExprDesc* lval)
/* Process + and - binary operators. */
{
    int k = hie9 (lval);
    while (CurTok.Tok == TOK_PLUS || CurTok.Tok == TOK_MINUS) {

       	if (CurTok.Tok == TOK_PLUS) {
       	    parseadd (k, lval);
       	} else {
       	    parsesub (k, lval);
       	}
       	k = 0;
    }
    return k;
}




static int hie7 (ExprDesc *lval)
/* Parse << and >>. */
{
    static const GenDesc* hie7_ops [] = {
       	&GenASL, &GenASR, 0
    };
    int UsedGen;

    return hie_internal (hie7_ops, lval, hie8, &UsedGen);
}



static int hie6 (ExprDesc *lval)
/* process greater-than type comparators */
{
    static const GenDesc* hie6_ops [] = {
	&GenLT, &GenLE, &GenGE, &GenGT, 0
    };
    return hie_compare (hie6_ops, lval, hie7);
}



static int hie5 (ExprDesc *lval)
{
    static const GenDesc* hie5_ops[] = {
       	&GenEQ, &GenNE, 0
    };
    return hie_compare (hie5_ops, lval, hie6);
}



static int hie4 (ExprDesc* lval)
/* Handle & (bitwise and) */
{
    static const GenDesc* hie4_ops [] = {
       	&GenAND, 0
    };
    int UsedGen;

    return hie_internal (hie4_ops, lval, hie5, &UsedGen);
}



static int hie3 (ExprDesc *lval)
/* Handle ^ (bitwise exclusive or) */
{
    static const GenDesc* hie3_ops [] = {
       	&GenXOR, 0
    };
    int UsedGen;

    return hie_internal (hie3_ops, lval, hie4, &UsedGen);
}



static int hie2 (ExprDesc *lval)
/* Handle | (bitwise or) */
{
    static const GenDesc* hie2_ops [] = {
       	&GenOR, 0
    };
    int UsedGen;

    return hie_internal (hie2_ops, lval, hie3, &UsedGen);
}



static int hieAndPP (ExprDesc* lval)
/* Process "exp && exp" in preprocessor mode (that is, when the parser is
 * called recursively from the preprocessor.
 */
{
    ExprDesc lval2;

    ConstSubExpr (hie2, lval);
    while (CurTok.Tok == TOK_BOOL_AND) {

	/* Left hand side must be an int */
	if (!IsClassInt (lval->Type)) {
	    Error ("Left hand side must be of integer type");
	    MakeConstIntExpr (lval, 1);
	}

	/* Skip the && */
	NextToken ();

	/* Get rhs */
	ConstSubExpr (hie2, &lval2);

	/* Since we are in PP mode, all we know about is integers */
	if (!IsClassInt (lval2.Type)) {
	    Error ("Right hand side must be of integer type");
	    MakeConstIntExpr (&lval2, 1);
	}

	/* Combine the two */
	lval->ConstVal = (lval->ConstVal && lval2.ConstVal);
    }

    /* Always a rvalue */
    return 0;
}



static int hieOrPP (ExprDesc *lval)
/* Process "exp || exp" in preprocessor mode (that is, when the parser is
 * called recursively from the preprocessor.
 */
{
    ExprDesc lval2;

    ConstSubExpr (hieAndPP, lval);
    while (CurTok.Tok == TOK_BOOL_OR) {

	/* Left hand side must be an int */
	if (!IsClassInt (lval->Type)) {
	    Error ("Left hand side must be of integer type");
	    MakeConstIntExpr (lval, 1);
	}

	/* Skip the && */
	NextToken ();

	/* Get rhs */
	ConstSubExpr (hieAndPP, &lval2);

	/* Since we are in PP mode, all we know about is integers */
	if (!IsClassInt (lval2.Type)) {
	    Error ("Right hand side must be of integer type");
	    MakeConstIntExpr (&lval2, 1);
	}

	/* Combine the two */
	lval->ConstVal = (lval->ConstVal || lval2.ConstVal);
    }

    /* Always a rvalue */
    return 0;
}



static int hieAnd (ExprDesc* lval, unsigned TrueLab, int* BoolOp)
/* Process "exp && exp" */
{
    int k;
    int lab;
    ExprDesc lval2;

    k = hie2 (lval);
    if (CurTok.Tok == TOK_BOOL_AND) {

       	/* Tell our caller that we're evaluating a boolean */
       	*BoolOp = 1;

       	/* Get a label that we will use for false expressions */
       	lab = GetLocalLabel ();

       	/* If the expr hasn't set condition codes, set the force-test flag */
       	if ((lval->Test & E_CC) == 0) {
       	    lval->Test |= E_FORCETEST;
       	}

       	/* Load the value */
       	exprhs (CF_FORCECHAR, k, lval);

       	/* Generate the jump */
       	g_falsejump (CF_NONE, lab);

       	/* Parse more boolean and's */
       	while (CurTok.Tok == TOK_BOOL_AND) {

       	    /* Skip the && */
    	    NextToken ();

    	    /* Get rhs */
    	    k = hie2 (&lval2);
    	    if ((lval2.Test & E_CC) == 0) {
    		lval2.Test |= E_FORCETEST;
    	    }
    	    exprhs (CF_FORCECHAR, k, &lval2);

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

       	/* Define the label */
       	lval->Flags = E_MEXPR;
       	lval->Test |= E_CC;	/* Condition codes are set */
       	k = 0;
    }
    return k;
}



static int hieOr (ExprDesc *lval)
/* Process "exp || exp". */
{
    int k;
    ExprDesc lval2;
    int BoolOp = 0;  		/* Did we have a boolean op? */
    int AndOp;			/* Did we have a && operation? */
    unsigned TrueLab;		/* Jump to this label if true */
    unsigned DoneLab;

    /* Get a label */
    TrueLab = GetLocalLabel ();

    /* Call the next level parser */
    k = hieAnd (lval, TrueLab, &BoolOp);

    /* Any boolean or's? */
    if (CurTok.Tok == TOK_BOOL_OR) {

    	/* If the expr hasn't set condition codes, set the force-test flag */
       	if ((lval->Test & E_CC) == 0) {
    	    lval->Test |= E_FORCETEST;
    	}

    	/* Get first expr */
      	exprhs (CF_FORCECHAR, k, lval);

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
    	    k = hieAnd (&lval2, TrueLab, &AndOp);
       	    if ((lval2.Test & E_CC) == 0) {
    		lval2.Test |= E_FORCETEST;
    	    }
    	    exprhs (CF_FORCECHAR, k, &lval2);

       	    /* If there is more to come, add shortcut boolean eval. */
    	    g_truejump (CF_NONE, TrueLab);

	}
    	lval->Flags = E_MEXPR;
    	lval->Test |= E_CC;		       	/* Condition codes are set */
    	k = 0;
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
    return k;
}



static int hieQuest (ExprDesc *lval)
/* Parse "lvalue ? exp : exp" */
{
    int k;
    int labf;
    int labt;
    ExprDesc lval2;       	/* Expression 2 */
    ExprDesc lval3;       	/* Expression 3 */
    type* type2; 	       	/* Type of expression 2 */
    type* type3;	       	/* Type of expression 3 */
    type* rtype;	       	/* Type of result */


    k = Preprocessing? hieOrPP (lval) : hieOr (lval);
    if (CurTok.Tok == TOK_QUEST) {
    	NextToken ();
    	if ((lval->Test & E_CC) == 0) {
    	    /* Condition codes not set, force a test */
    	    lval->Test |= E_FORCETEST;
    	}
    	exprhs (CF_NONE, k, lval);
    	labf = GetLocalLabel ();
    	g_falsejump (CF_NONE, labf);

    	/* Parse second expression */
        k = expr (hie1, &lval2);
 	type2 = lval2.Type;
        if (!IsTypeVoid (lval2.Type)) {
            /* Load it into the primary */
            exprhs (CF_NONE, k, &lval2);
        }
    	labt = GetLocalLabel ();
    	ConsumeColon ();
    	g_jump (labt);

        /* Parse the third expression */
    	g_defcodelabel (labf);
        k = expr (hie1, &lval3);
	type3 = lval3.Type;
        if (!IsTypeVoid (lval3.Type)) {
            /* Load it into the primary */
            exprhs (CF_NONE, k, &lval3);
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
	if (IsClassInt (type2) && IsClassInt (type3)) {

	    /* Get common type */
	    rtype = promoteint (type2, type3);

	    /* Convert the third expression to this type if needed */
	    g_typecast (TypeOf (rtype), TypeOf (type3));

	    /* Setup a new label so that the expr3 code will jump around
	     * the type cast code for expr2.
	     */
       	    labf = GetLocalLabel (); 	/* Get new label */
	    g_jump (labf);	    	/* Jump around code */

	    /* The jump for expr2 goes here */
    	    g_defcodelabel (labt);

	    /* Create the typecast code for expr2 */
	    g_typecast (TypeOf (rtype), TypeOf (type2));

	    /* Jump here around the typecase code. */
	    g_defcodelabel (labf);
	    labt = 0;		/* Mark other label as invalid */

	} else if (IsClassPtr (type2) && IsClassPtr (type3)) {
	    /* Must point to same type */
	    if (TypeCmp (Indirect (type2), Indirect (type3)) < TC_EQUAL) {
	    	Error ("Incompatible pointer types");
	    }
	    /* Result has the common type */
	    rtype = lval2.Type;
	} else if (IsClassPtr (type2) && IsNullPtr (&lval3)) {
	    /* Result type is pointer, no cast needed */
	    rtype = lval2.Type;
	} else if (IsNullPtr (&lval2) && IsClassPtr (type3)) {
	    /* Result type is pointer, no cast needed */
	    rtype = lval3.Type;
        } else if (IsTypeVoid (type2) && IsTypeVoid (type3)) {
            /* Result type is void */
            rtype = lval3.Type;
	} else {
	    Error ("Incompatible types");
	    rtype = lval2.Type;	 	/* Doesn't matter here */
	}

	/* If we don't have the label defined until now, do it */
	if (labt) {
	    g_defcodelabel (labt);
	}

	/* Setup the target expression */
       	lval->Flags = E_MEXPR;
    	lval->Type = rtype;
    	k = 0;
    }
    return k;
}



static void opeq (const GenDesc* Gen, ExprDesc *lval, int k)
/* Process "op=" operators. */
{
    ExprDesc lval2;
    unsigned flags;
    CodeMark Mark;
    int MustScale;

    NextToken ();
    if (k == 0) {
     	Error ("Invalid lvalue in assignment");
     	return;
    }

    /* Determine the type of the lhs */
    flags = TypeOf (lval->Type);
    MustScale = (Gen->Func == g_add || Gen->Func == g_sub) &&
	    	lval->Type [0] == T_PTR;

    /* Get the lhs address on stack (if needed) */
    PushAddr (lval);

    /* Fetch the lhs into the primary register if needed */
    exprhs (CF_NONE, k, lval);

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
	    lval2.ConstVal *= CheckedSizeOf (lval->Type+1);
	}

	/* If the lhs is character sized, the operation may be later done
	 * with characters.
	 */
	if (CheckedSizeOf (lval->Type) == SIZEOF_CHAR) {
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
       	    g_scale (TypeOf (lval2.Type), CheckedSizeOf (lval->Type+1));
	}

	/* If the lhs is character sized, the operation may be later done
	 * with characters.
	 */
	if (CheckedSizeOf (lval->Type) == SIZEOF_CHAR) {
	    flags |= CF_FORCECHAR;
	}

	/* Adjust the types of the operands if needed */
       	Gen->Func (g_typeadjust (flags, TypeOf (lval2.Type)), 0);
    }
    Store (lval, 0);
    lval->Flags = E_MEXPR;
}



static void addsubeq (const GenDesc* Gen, ExprDesc *lval, int k)
/* Process the += and -= operators */
{
    ExprDesc lval2;
    unsigned lflags;
    unsigned rflags;
    int MustScale;


    /* We must have an lvalue */
    if (k == 0) {
     	Error ("Invalid lvalue in assignment");
     	return;
    }

    /* We're currently only able to handle some adressing modes */
    if ((lval->Flags & E_MGLOBAL) == 0 && 	/* Global address? */
	(lval->Flags & E_MLOCAL) == 0  && 	/* Local address? */
       	(lval->Flags & E_MCONST) == 0) {	/* Constant address? */
	/* Use generic routine */
       	opeq (Gen, lval, k);
	return;
    }

    /* Skip the operator */
    NextToken ();

    /* Check if we have a pointer expression and must scale rhs */
    MustScale = (lval->Type [0] == T_PTR);

    /* Initialize the code generator flags */
    lflags = 0;
    rflags = 0;

    /* Evaluate the rhs */
    if (evalexpr (CF_NONE, hie1, &lval2) == 0) {
    	/* The resulting value is a constant. */
       	if (MustScale) {
    	    /* lhs is a pointer, scale rhs */
    	    lval2.ConstVal *= CheckedSizeOf (lval->Type+1);
    	}
     	rflags |= CF_CONST;
	lflags |= CF_CONST;
    } else {
     	/* rhs is not constant and already in the primary register */
       	if (MustScale) {
     	    /* lhs is a pointer, scale rhs */
       	    g_scale (TypeOf (lval2.Type), CheckedSizeOf (lval->Type+1));
     	}
    }

    /* Setup the code generator flags */
    lflags |= TypeOf (lval->Type) | CF_FORCECHAR;
    rflags |= TypeOf (lval2.Type);

    /* Cast the rhs to the type of the lhs */
    g_typecast (lflags, rflags);

    /* Output apropriate code */
    if (lval->Flags & E_MGLOBAL) {
	/* Static variable */
	lflags |= GlobalModeFlags (lval->Flags);
	if (Gen->Tok == TOK_PLUS_ASSIGN) {
	    g_addeqstatic (lflags, lval->Name, lval->ConstVal, lval2.ConstVal);
	} else {
       	    g_subeqstatic (lflags, lval->Name, lval->ConstVal, lval2.ConstVal);
	}
    } else if (lval->Flags & E_MLOCAL) {
	/* ref to localvar */
	if (Gen->Tok == TOK_PLUS_ASSIGN) {
    	    g_addeqlocal (lflags, lval->ConstVal, lval2.ConstVal);
	} else {
	    g_subeqlocal (lflags, lval->ConstVal, lval2.ConstVal);
	}
    } else if (lval->Flags & E_MCONST) {
	/* ref to absolute address */
	lflags |= CF_ABSOLUTE;
	if (Gen->Tok == TOK_PLUS_ASSIGN) {
	    g_addeqstatic (lflags, lval->ConstVal, 0, lval2.ConstVal);
	} else {
       	    g_subeqstatic (lflags, lval->ConstVal, 0, lval2.ConstVal);
	}
    } else if (lval->Flags & E_MEXPR) {
       	/* Address in a/x. */
	if (Gen->Tok == TOK_PLUS_ASSIGN) {
       	    g_addeqind (lflags, lval->ConstVal, lval2.ConstVal);
	} else {
       	    g_subeqind (lflags, lval->ConstVal, lval2.ConstVal);
	}
    } else {
       	Internal ("Invalid addressing mode");
    }

    /* Expression is in the primary now */
    lval->Flags = E_MEXPR;
}



int hie1 (ExprDesc* lval)
/* Parse first level of expression hierarchy. */
{
    int k;

    k = hieQuest (lval);
    switch (CurTok.Tok) {

    	case TOK_RPAREN:
    	case TOK_SEMI:
    	    return k;

    	case TOK_ASSIGN:
    	    NextToken ();
    	    if (k == 0) {
    	      	Error ("Invalid lvalue in assignment");
    	    } else {
    	      	Assignment (lval);
    	    }
    	    break;

    	case TOK_PLUS_ASSIGN:
       	    addsubeq (&GenPASGN, lval, k);
    	    break;

    	case TOK_MINUS_ASSIGN:
       	    addsubeq (&GenSASGN, lval, k);
    	    break;

    	case TOK_MUL_ASSIGN:
       	    opeq (&GenMASGN, lval, k);
     	    break;

     	case TOK_DIV_ASSIGN:
       	    opeq (&GenDASGN, lval, k);
     	    break;

     	case TOK_MOD_ASSIGN:
       	    opeq (&GenMOASGN, lval, k);
     	    break;

     	case TOK_SHL_ASSIGN:
       	    opeq (&GenSLASGN, lval, k);
     	    break;

     	case TOK_SHR_ASSIGN:
       	    opeq (&GenSRASGN, lval, k);
     	    break;

     	case TOK_AND_ASSIGN:
       	    opeq (&GenAASGN, lval, k);
     	    break;

     	case TOK_XOR_ASSIGN:
       	    opeq (&GenXOASGN, lval, k);
     	    break;

     	case TOK_OR_ASSIGN:
       	    opeq (&GenOASGN, lval, k);
     	    break;

     	default:
     	    return k;
    }
    return 0;
}



static int hie0 (ExprDesc *lval)
/* Parse comma operator. */
{
    int k;

    k = hie1 (lval);
    while (CurTok.Tok == TOK_COMMA) {
    	NextToken ();
     	k = hie1 (lval);
    }
    return k;
}



int evalexpr (unsigned flags, int (*f) (ExprDesc*), ExprDesc* lval)
/* Will evaluate an expression via the given function. If the result is a
 * constant, 0 is returned and the value is put in the lval struct. If the
 * result is not constant, exprhs is called to bring the value into the
 * primary register and 1 is returned.
 */
{
    int k;

    /* Evaluate */
    k = f (lval);
    if (k == 0 && lval->Flags == E_MCONST) {
     	/* Constant expression */
     	return 0;
    } else {
     	/* Not constant, load into the primary */
        exprhs (flags, k, lval);
     	return 1;
    }
}



static int expr (int (*func) (ExprDesc*), ExprDesc *lval)
/* Expression parser; func is either hie0 or hie1. */
{
    int k;
    int savsp;

    savsp = oursp;

    k = (*func) (lval);

    /* Do some checks if code generation is still constistent */
    if (savsp != oursp) {
     	if (Debug) {
     	    fprintf (stderr, "oursp != savesp (%d != %d)\n", oursp, savsp);
     	} else {
     	    Internal ("oursp != savsp (%d != %d)", oursp, savsp);
     	}
    }
    return k;
}



void expression1 (ExprDesc* lval)
/* Evaluate an expression on level 1 (no comma operator) and put it into
 * the primary register
 */
{
    InitExprDesc (lval);
    exprhs (CF_NONE, expr (hie1, lval), lval);
}



void expression (ExprDesc* lval)
/* Evaluate an expression and put it into the primary register */
{
    InitExprDesc (lval);
    exprhs (CF_NONE, expr (hie0, lval), lval);
}



void ConstExpr (ExprDesc* lval)
/* Get a constant value */
{
    InitExprDesc (lval);
    if (expr (hie1, lval) != 0 || (lval->Flags & E_MCONST) == 0) {
     	Error ("Constant expression expected");
     	/* To avoid any compiler errors, make the expression a valid const */
     	MakeConstIntExpr (lval, 1);
    }
}



void ConstIntExpr (ExprDesc* Val)
/* Get a constant int value */
{
    InitExprDesc (Val);
    if (expr (hie1, Val) != 0        ||
	(Val->Flags & E_MCONST) == 0 ||
	!IsClassInt (Val->Type)) {
     	Error ("Constant integer expression expected");
     	/* To avoid any compiler errors, make the expression a valid const */
     	MakeConstIntExpr (Val, 1);
    }
}



void intexpr (ExprDesc* lval)
/* Get an integer expression */
{
    expression (lval);
    if (!IsClassInt (lval->Type)) {
     	Error ("Integer expression expected");
     	/* To avoid any compiler errors, make the expression a valid int */
     	MakeConstIntExpr (lval, 1);
    }
}



void Test (unsigned Label, int Invert)
/* Evaluate a boolean test expression and jump depending on the result of
 * the test and on Invert.
 */
{
    int k;
    ExprDesc lval;

    /* Evaluate the expression */
    k = expr (hie0, InitExprDesc (&lval));

    /* Check for a boolean expression */
    CheckBoolExpr (&lval);

    /* Check for a constant expression */
    if (k == 0 && lval.Flags == E_MCONST) {

      	/* Constant rvalue */
       	if (!Invert && lval.ConstVal == 0) {
      	    g_jump (Label);
     	    Warning ("Unreachable code");
     	} else if (Invert && lval.ConstVal != 0) {
 	    g_jump (Label);
      	}

    } else {

        /* If the expr hasn't set condition codes, set the force-test flag */
        if ((lval.Test & E_CC) == 0) {
            lval.Test |= E_FORCETEST;
        }

        /* Load the value into the primary register */
        exprhs (CF_FORCECHAR, k, &lval);

        /* Generate the jump */
        if (Invert) {
            g_truejump (CF_NONE, Label);
        } else {
            g_falsejump (CF_NONE, Label);
        }
    }
}



void TestInParens (unsigned Label, int Invert)
/* Evaluate a boolean test expression in parenthesis and jump depending on
 * the result of the test * and on Invert.
 */
{
    /* Eat the parenthesis */
    ConsumeLParen ();

    /* Do the test */
    Test (Label, Invert);

    /* Check for the closing brace */
    ConsumeRParen ();
}



