/*****************************************************************************/
/*                                                                           */
/*                                exprdesc.h                                 */
/*                                                                           */
/*                      Expression descriptor structure                      */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2002-2010, Ullrich von Bassewitz                                      */
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



#ifndef EXPRDESC_H
#define EXPRDESC_H



#include <inttypes.h>
#include <string.h>

/* common */
#include "fp.h"
#include "inline.h"

/* cc65 */
#include "asmcode.h"
#include "datatype.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Defines for the flags field of the expression descriptor */
enum {
    /* Location: Where is the value we're talking about?
    **
    ** Remarks:
    ** - E_LOC_<else> refers to any other than E_LOC_NONE and E_LOC_PRIMARY.
    ** - E_LOC_EXPR can be regarded as a generalized E_LOC_<else>.
    ** - E_LOC_NONE can be regarded as E_LOC_PRIMARY + E_ADDRESS_OF unless
    **    remarked otherwise (see below).
    ** - An E_LOC_NONE value is not considered to be an "address".
    ** - ref-load doesn't change the location, while rval-load puts into the
    **    primary register a "temporary" that is the straight integer rvalue or
    **    a "delegate" to the real rvalue somewhere else.
    ** - ref-load doesn't change the rval/lval category of the expression,
    **    while rval-load converts it to an rvalue if it wasn't.
    ** - In practice, ref-load is unimplemented, and can be simulated with
    **    adding E_ADDRESS_OF temporaily through LoadExpr + FinalizeLoad,
    **    whilst val-load is done with LoadExpr + FinalizeRValLoad.
    **
    ** E_LOC_NONE     -- ref-load     -> + E_LOADED (int rvalue)
    ** E_LOC_PRIMARY  -- ref-load     -> + E_LOADED (unchanged)
    ** E_LOC_<else>   -- ref-load     -> + E_LOADED (reference lvalue)
    ** + E_ADDRESS_OF -- ref-load     -> + E_LOADED (address rvalue)
    ** E_LOC_NONE     -- val-load     -> E_LOC_PRIMARY (int rvalue)
    ** E_LOC_PRIMARY  -- val-load     -> E_LOC_PRIMARY (unchanged)
    ** E_LOC_<else>   -- val-load     -> E_LOC_PRIMARY (rvalue/delegate)
    ** + E_ADDRESS_OF -- val-load     -> E_LOC_PRIMARY (address rvalue)
    ** E_LOC_NONE     -- take address -> (error)
    ** E_LOC_PRIMARY  -- take address -> + E_ADDRESS_OF (or error)
    ** E_LOC_EXPR     -- take address -> E_LOC_PRIMARY (address)
    ** E_LOC_<else>   -- take address -> + E_ADDRESS_OF (address)
    ** + E_ADDRESS_OF -- take address -> (error)
    ** E_LOC_NONE     -- dereference  -> E_LOC_ABS (lvalue reference)
    ** E_LOC_PRIMARY  -- dereference  -> E_LOC_EXPR (lvalue reference)
    ** E_LOC_<else>   -- dereference  -> E_LOC_EXPR (pointed-to-value, must load)
    ** + E_ADDRESS_OF -- dereference  -> (lvalue reference)
    */
    E_MASK_LOC          = 0x01FF,
    E_LOC_NONE          = 0x0000,       /* Pure rvalue with no storage */
    E_LOC_ABS           = 0x0001,       /* Absolute numeric addressed variable */
    E_LOC_GLOBAL        = 0x0002,       /* Global variable */
    E_LOC_STATIC        = 0x0004,       /* Local static variable */
    E_LOC_REGISTER      = 0x0008,       /* Register variable */
    E_LOC_STACK         = 0x0010,       /* Value on the stack */
    E_LOC_PRIMARY       = 0x0020,       /* Temporary in primary register */
    E_LOC_EXPR          = 0x0040,       /* A location that the primary register points to */
    E_LOC_LITERAL       = 0x0080,       /* Literal in the literal pool */
    E_LOC_CODE          = 0x0100,       /* C code label location (&&Label) */

    /* Immutable location addresses (immutable bases and offsets) */
    E_LOC_CONST         = E_LOC_NONE | E_LOC_ABS | E_LOC_GLOBAL | E_LOC_STATIC |
                          E_LOC_REGISTER | E_LOC_LITERAL | E_LOC_CODE,

    /* Not-so-immutable location addresses (stack offsets may change dynamically) */
    E_LOC_QUASICONST    = E_LOC_CONST | E_LOC_STACK,

    /* Expression type modifiers */
    E_ADDRESS_OF        = 0x0400,       /* Expression is the address of the lvalue */

    /* lvalue/rvalue in C language's sense */
    E_MASK_RTYPE        = 0x0800,
    E_RTYPE_RVAL        = 0x0000,
    E_RTYPE_LVAL        = 0x0800,

    /* Expression status */
    E_LOADED            = 0x1000,       /* Expression is loaded in primary */
    E_CC_SET            = 0x2000,       /* Condition codes are set */
    E_HAVE_MARKS        = 0x4000,       /* Code marks are valid */
    E_SIDE_EFFECTS      = 0x8000,       /* Expression has had side effects */

    /* Optimization hints */
    E_MASK_NEED         = 0x030000,
    E_NEED_EAX          = 0x000000,     /* Expression result needs to be loaded in Primary */
    E_NEED_NONE         = 0x010000,     /* Expression result is unused */
    E_NEED_TEST         = 0x020000,     /* Expression needs a test to set cc */

    /* Expression evaluation requirements.
    ** Usage: (Flags & E_EVAL_<Flag>) == E_EVAL_<Flag>
    **
    ** Remark:
    ** - Expression result, that is the "final value" of the expression, is no
    **    more than one of the effects of the whole expression. Effects other
    **    than it are usually consided "side-effects" in this regard.
    ** - The compiler front end cannot know things determined by the linker,
    **    such as the actual address of an object with static storage. What it
    **    can know is categorized as "compiler-known" here.
    ** - The concept "immutable" here means that once something is determined
    **    (not necessarily by the compiler), it will never change. This is not
    **    the same meaning as the "constant" word in the C standard.
    ** - The concept "compile-time" ( not to be confued with "compiler-known"),
    **    or "static" (compared to "run-time" as in "_Static_assert" in C, not
    **    to be confused with the "static" storage) here means that something
    **    has no run-time behaviors, enforced by the fact that it generates no
    **    target code (hence "no-code"). It is closely related to the concepts
    **    above but not the same.
    ** - An "unevaluated" expression is special and different from the above:
    **    while it generates no code, cannot change its "value" (it actually has
    **    no value), and must be completely handled by the compiler front-end,
    **    it is unique in that it is not "evaluated" while the others are, and
    **    the codegen routine of such an expression is usually separated from
    **    the normally evaluated ones. Therefore it is treated differently from
    **    the above and uses a separate flag that implies none of the above.
    ** - The "maybe-unused" flag is to suppress the checking and warning on
    **    expressions with no effects. It doesn't have any special meanings
    **    beyond that, and is independent from the E_NEED_<flag>s. All
    **    "unevaluated" expressions are  flagged as "maybe-unused" just to
    **    avoid unnecessary warnings.
    **
    ** Relationship of some concepts:
    ** - "no-code" implies "no-side-effects"
    ** - "immutable" = "compiler-known" OR "no-code"
    ** - "constant expression" in C = "compiler-known" AND "no-code", with minor differences
    */
    E_MASK_EVAL             = 0xFC0000,
    E_EVAL_NONE             = 0x000000, /* No requirements */
    E_EVAL_IMMUTABLE_RESULT = 0x040000, /* Expression result must be immutable */
    E_EVAL_COMPILER_KNOWN   = 0x0C0000, /* Expression result must be known to the compiler */
    E_EVAL_NO_SIDE_EFFECTS  = 0x100000, /* Evaluation must have no side effects */
    E_EVAL_NO_CODE          = 0x340000, /* Evaluation must generate no code */
    E_EVAL_MAYBE_UNUSED     = 0x400000, /* Expression result may be unused */
    E_EVAL_UNEVAL           = 0xC00000, /* Expression is unevaluated */

    /* Expression result must be known to the compiler and generate no code to load */
    E_EVAL_C_CONST          = E_EVAL_COMPILER_KNOWN | E_EVAL_NO_CODE,

    /* Flags to combine from subexpressions */
    E_MASK_VIRAL            = E_SIDE_EFFECTS,

    /* Flags to keep in subexpressions of most operations other than ternary */
    E_MASK_KEEP_SUBEXPR     = E_MASK_EVAL,

    /* Flags to keep for the two result subexpressions of the ternary operation */
    E_MASK_KEEP_RESULT      = E_MASK_NEED | E_MASK_EVAL,

    /* Flags to keep when using the ED_Make functions */
    E_MASK_KEEP_MAKE        = E_HAVE_MARKS | E_MASK_KEEP_RESULT,
};

/* Forward */
struct Literal;

/* Describe the result of an expression */
typedef struct ExprDesc ExprDesc;
struct ExprDesc {
    const Type*         Type;           /* C type of the expression */
    unsigned            Flags;          /* Properties of the expression */
    uintptr_t           Name;           /* Name pointer or label number */
    struct SymEntry*    Sym;            /* Symbol table entry if any */
    long                IVal;           /* Integer value if expression constant */
    union {
        Double          FVal;           /* Floating point value */
        struct Literal* LVal;           /* Literal value */
    } V;

    /* Start and end of generated code */
    CodeMark            Start;
    CodeMark            End;
};



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



ExprDesc* ED_Init (ExprDesc* Expr);
/* Initialize an ExprDesc */



/*****************************************************************************/
/*                              Info Extraction                              */
/*****************************************************************************/



#if defined(HAVE_INLINE)
INLINE int ED_GetLoc (const ExprDesc* Expr)
/* Return the location flags from the expression */
{
    return (Expr->Flags & E_MASK_LOC);
}
#else
#  define ED_GetLoc(Expr)       ((Expr)->Flags & E_MASK_LOC)
#endif

#if defined(HAVE_INLINE)
INLINE int ED_GetNeeds (const ExprDesc* Expr)
/* Get flags about what the expression needs. */
{
    return (Expr->Flags & E_MASK_NEED);
}
#else
#  define ED_GetNeeds(Expr)     ((Expr)->Flags & E_MASK_NEED)
#endif

const char* ED_GetLabelName (const ExprDesc* Expr, long Offs);
/* Return the assembler label name of the given expression. Beware: This
** function may use a static buffer, so the name may get "lost" on the second
** call to the function.
*/

int ED_GetStackOffs (const ExprDesc* Expr, int Offs);
/* Get the stack offset of an address on the stack in Expr taking into account
** an additional offset in Offs.
*/



/*****************************************************************************/
/*                                Predicates                                 */
/*****************************************************************************/



#if defined(HAVE_INLINE)
INLINE int ED_IsLocNone (const ExprDesc* Expr)
/* Return true if the expression is an absolute value */
{
    return (Expr->Flags & E_MASK_LOC) == E_LOC_NONE;
}
#else
#  define ED_IsLocNone(Expr)    (((Expr)->Flags & E_MASK_LOC) == E_LOC_NONE)
#endif

#if defined(HAVE_INLINE)
INLINE int ED_IsLocAbs (const ExprDesc* Expr)
/* Return true if the expression is referenced with an absolute address */
{
    return (Expr->Flags & E_MASK_LOC) == E_LOC_ABS;
}
#else
#  define ED_IsLocAbs(Expr)     (((Expr)->Flags & E_MASK_LOC) == E_LOC_ABS)
#endif

#if defined(HAVE_INLINE)
INLINE int ED_IsLocRegister (const ExprDesc* Expr)
/* Return true if the expression is located in a register */
{
    return (Expr->Flags & E_MASK_LOC) == E_LOC_REGISTER;
}
#else
#  define ED_IsLocRegister(Expr)  (((Expr)->Flags & E_MASK_LOC) == E_LOC_REGISTER)
#endif

#if defined(HAVE_INLINE)
INLINE int ED_IsLocStack (const ExprDesc* Expr)
/* Return true if the expression is located on the stack */
{
    return (Expr->Flags & E_MASK_LOC) == E_LOC_STACK;
}
#else
#  define ED_IsLocStack(Expr)     (((Expr)->Flags & E_MASK_LOC) == E_LOC_STACK)
#endif

#if defined(HAVE_INLINE)
INLINE int ED_IsLocPrimary (const ExprDesc* Expr)
/* Return true if the expression is an expression in the primary */
{
    return (Expr->Flags & E_MASK_LOC) == E_LOC_PRIMARY;
}
#else
#  define ED_IsLocPrimary(Expr)  (((Expr)->Flags & E_MASK_LOC) == E_LOC_PRIMARY)
#endif

#if defined(HAVE_INLINE)
INLINE int ED_IsLocExpr (const ExprDesc* Expr)
/* Return true if the expression is an expression referenced in the primary */
{
    return (Expr->Flags & E_MASK_LOC) == E_LOC_EXPR;
}
#else
#  define ED_IsLocExpr(Expr)     (((Expr)->Flags & E_MASK_LOC) == E_LOC_EXPR)
#endif

#if defined(HAVE_INLINE)
INLINE int ED_IsLocLiteral (const ExprDesc* Expr)
/* Return true if the expression is a string from the literal pool */
{
    return (Expr->Flags & E_MASK_LOC) == E_LOC_LITERAL;
}
#else
#  define ED_IsLocLiteral(Expr)   (((Expr)->Flags & E_MASK_LOC) == E_LOC_LITERAL)
#endif

#if defined(HAVE_INLINE)
INLINE int ED_IsLocConst (const ExprDesc* Expr)
/* Return true if the expression is a constant location of some sort */
{
    return ((Expr)->Flags & E_MASK_LOC & ~E_LOC_CONST) == 0;
}
#else
#  define ED_IsLocConst(Expr)   (((Expr)->Flags & E_MASK_LOC & ~E_LOC_CONST) == 0)
#endif

#if defined(HAVE_INLINE)
INLINE int ED_IsLocQuasiConst (const ExprDesc* Expr)
/* Return true if the expression is a constant location of some sort or on the
** stack.
*/
{
    return ED_IsLocConst (Expr) || ED_IsLocStack (Expr);
}
#else
int ED_IsLocQuasiConst (const ExprDesc* Expr);
/* Return true if the expression denotes a constant address of some sort. This
** can be the address of a global variable (maybe with offset) or similar.
*/
#endif

int ED_IsLocZP (const ExprDesc* Expr);
/* Return true if the expression is in a location on a zeropage */

#if defined(HAVE_INLINE)
INLINE int ED_IsLocPrimaryOrExpr (const ExprDesc* Expr)
/* Return true if the expression is E_LOC_PRIMARY or E_LOC_EXPR */
{
    return ED_IsLocPrimary (Expr) || ED_IsLocExpr (Expr);
}
#else
int ED_IsLocPrimaryOrExpr (const ExprDesc* Expr);
/* Return true if the expression is E_LOC_PRIMARY or E_LOC_EXPR */
#endif

#if defined(HAVE_INLINE)
INLINE int ED_NeedsPrimary (const ExprDesc* Expr)
/* Check if the expression needs to be in Primary. */
{
    return (Expr->Flags & E_MASK_NEED) == E_NEED_EAX;
}
#else
#  define ED_NeedsPrimary(Expr) (((Expr)->Flags & E_MASK_NEED) == E_NEED_EAX)
#endif

#if defined(HAVE_INLINE)
INLINE int ED_NeedsTest (const ExprDesc* Expr)
/* Check if the expression needs a test. */
{
    return (Expr->Flags & E_NEED_TEST) != 0;
}
#else
#  define ED_NeedsTest(Expr)    (((Expr)->Flags & E_NEED_TEST) != 0)
#endif

#if defined(HAVE_INLINE)
INLINE int ED_IsTested (const ExprDesc* Expr)
/* Check if the expression has set the condition codes. */
{
    return (Expr->Flags & E_CC_SET) != 0;
}
#else
#  define ED_IsTested(Expr)   (((Expr)->Flags & E_CC_SET) != 0)
#endif

#if defined(HAVE_INLINE)
INLINE int ED_YetToTest (const ExprDesc* Expr)
/* Check if the expression needs to be tested but not yet. */
{
    return ((Expr)->Flags & (E_NEED_TEST | E_CC_SET)) == E_NEED_TEST;
}
#else
#  define ED_YetToTest(Expr)    (((Expr)->Flags & (E_NEED_TEST | E_CC_SET)) == E_NEED_TEST)
#endif

#if defined(HAVE_INLINE)
INLINE int ED_IsLoaded (const ExprDesc* Expr)
/* Check if the expression is loaded.
** NOTE: This is currently unused and not working due to code complexity.
*/
{
    return (Expr->Flags & E_LOADED) != 0;
}
#else
#  define ED_IsLoaded(Expr)   (((Expr)->Flags & E_LOADED) != 0)
#endif

int ED_YetToLoad (const ExprDesc* Expr);
/* Check if the expression is yet to be loaded somehow. */

#if defined(HAVE_INLINE)
INLINE int ED_NeedsConst (const ExprDesc* Expr)
/* Check if the expression need be immutable */
{
    return (Expr->Flags & E_EVAL_IMMUTABLE_RESULT) == E_EVAL_IMMUTABLE_RESULT;
}
#else
#  define ED_NeedsConst(Expr)   (((Expr)->Flags & E_EVAL_IMMUTABLE_RESULT) == E_EVAL_IMMUTABLE_RESULT)
#endif

#if defined(HAVE_INLINE)
INLINE int ED_IsUneval (const ExprDesc* Expr)
/* Check if the expression is not to be evaluated */
{
    return (Expr->Flags & E_EVAL_UNEVAL) == E_EVAL_UNEVAL;
}
#else
#  define ED_IsUneval(Expr)     (((Expr)->Flags & E_EVAL_UNEVAL) == E_EVAL_UNEVAL)
#endif

#if defined(HAVE_INLINE)
INLINE int ED_MayHaveNoEffect (const ExprDesc* Expr)
/* Check if the expression may be present without effects */
{
    return (Expr->Flags & E_EVAL_MAYBE_UNUSED) == E_EVAL_MAYBE_UNUSED;
}
#else
#  define ED_MayHaveNoEffect(Expr)  (((Expr)->Flags & E_EVAL_MAYBE_UNUSED) == E_EVAL_MAYBE_UNUSED)
#endif

#if defined(HAVE_INLINE)
INLINE int ED_IsAddrExpr (const ExprDesc* Expr)
/* Check if the expression is taken address of instead of its value.
*/
{
    return (Expr->Flags & E_ADDRESS_OF) != 0;
}
#else
#  define ED_IsAddrExpr(Expr) (((Expr)->Flags & E_ADDRESS_OF) != 0)
#endif

#if defined(HAVE_INLINE)
INLINE int ED_IsIndExpr (const ExprDesc* Expr)
/* Check if the expression is a reference to its value */
{
    return (Expr->Flags & E_ADDRESS_OF) == 0 &&
           !ED_IsLocNone (Expr) && !ED_IsLocPrimary (Expr);
}
#else
int ED_IsIndExpr (const ExprDesc* Expr);
/* Check if the expression is a reference to its value */
#endif

#if defined(HAVE_INLINE)
INLINE int ED_IsLVal (const ExprDesc* Expr)
/* Return true if the expression is a reference */
{
    return ((Expr)->Flags & E_MASK_RTYPE) == E_RTYPE_LVAL;
}
#else
#  define ED_IsLVal(Expr)       (((Expr)->Flags & E_MASK_RTYPE) == E_RTYPE_LVAL)
#endif

#if defined(HAVE_INLINE)
INLINE int ED_IsRVal (const ExprDesc* Expr)
/* Return true if the expression is an rvalue */
{
    return ((Expr)->Flags & E_MASK_RTYPE) == E_RTYPE_RVAL;
}
#else
#  define ED_IsRVal(Expr)       (((Expr)->Flags & E_MASK_RTYPE) == E_RTYPE_RVAL)
#endif

#if defined(HAVE_INLINE)
INLINE int ED_IsAbs (const ExprDesc* Expr)
/* Return true if the expression denotes a numeric value or address. */
{
    return (Expr->Flags & (E_MASK_LOC)) == (E_LOC_NONE) ||
           (Expr->Flags & (E_MASK_LOC|E_ADDRESS_OF)) == (E_LOC_ABS|E_ADDRESS_OF);
}
#else
int ED_IsAbs (const ExprDesc* Expr);
/* Return true if the expression denotes a numeric value or address. */
#endif

#if defined(HAVE_INLINE)
INLINE int ED_IsConstAbs (const ExprDesc* Expr)
/* Return true if the expression denotes a constant absolute value. This can be
** a numeric constant, cast to any type.
*/
{
    return ED_IsRVal (Expr) && ED_IsAbs (Expr);
}
#else
int ED_IsConstAbs (const ExprDesc* Expr);
/* Return true if the expression denotes a constant absolute value. This can be
** a numeric constant, cast to any type.
*/
#endif

int ED_IsConstAbsInt (const ExprDesc* Expr);
/* Return true if the expression is a constant (numeric) integer. */

int ED_IsConstBool (const ExprDesc* Expr);
/* Return true if the expression can be constantly evaluated as a boolean. */

int ED_IsConstTrue (const ExprDesc* Expr);
/* Return true if the constant expression can be evaluated as boolean true at
** compile time.
*/

int ED_IsConstFalse (const ExprDesc* Expr);
/* Return true if the constant expression can be evaluated as boolean false at
** compile time.
*/

int ED_IsConst (const ExprDesc* Expr);
/* Return true if the expression denotes a constant of some sort. This can be a
** numeric constant, the address of a global variable (maybe with offset) or
** similar.
*/

int ED_IsQuasiConst (const ExprDesc* Expr);
/* Return true if the expression denotes a quasi-constant of some sort. This
** can be a numeric constant, a constant address or a stack variable address.
*/

int ED_IsConstAddr (const ExprDesc* Expr);
/* Return true if the expression denotes a constant address of some sort. This
** can be the address of a global variable (maybe with offset) or similar.
*/

int ED_IsQuasiConstAddr (const ExprDesc* Expr);
/* Return true if the expression denotes a quasi-constant address of some sort.
** This can be a constant address or a stack variable address.
*/

int ED_IsStackAddr (const ExprDesc* Expr);
/* Return true if the expression denotes a fixed address on stack */

int ED_IsZPInd (const ExprDesc* Expr);
/* Return true if the expression is located on the zeropage */

int ED_IsNullPtr (const ExprDesc* Expr);
/* Return true if the given expression is a null pointer.
** Note: A null pointer constant converted to a pointer type is a null pointer.
*/

int ED_IsNullPtrConstant (const ExprDesc* Expr);
/* Return true if the given expression is a null pointer constant.
** Note: An integer constant expression with value 0, or such an
** expression cast to void* is a null pointer constant. However, a
** null pointer constant converted to a pointer type is just a null
** pointer, not necessarily a constant in ISO C.
*/

int ED_IsEntityAddr (const ExprDesc* Expr);
/* Return true if the expression denotes the address of an object or function.
*/

int ED_IsBool (const ExprDesc* Expr);
/* Return true if the expression can be treated as a boolean, that is, it can
** be an operand to a compare operation with 0/NULL.
*/



/*****************************************************************************/
/*                               Manipulation                                */
/*****************************************************************************/



ExprDesc* ED_MakeConstAbs (ExprDesc* Expr, long Value, const Type* Type);
/* Replace Expr with an absolute const with the given value and type */

ExprDesc* ED_MakeConstAbsInt (ExprDesc* Expr, long Value);
/* Replace Expr with an constant integer with the given value */

ExprDesc* ED_MakeConstBool (ExprDesc* Expr, long Value);
/* Replace Expr with a constant boolean expression with the given value */

ExprDesc* ED_FinalizeRValLoad (ExprDesc* Expr);
/* Finalize the result of LoadExpr to be an rvalue in the primary register */

#if defined(HAVE_INLINE)
INLINE void ED_MarkExprAsLVal (ExprDesc* Expr)
/* Mark the expression as an lvalue.
** HINT: Consider using ED_IndExpr instead of this, unless you know what
**       consequence there will be, as there are both a big part in the code
**       assuming rvalue = const and a big part assuming rvalue = address.
*/
{
    Expr->Flags |= E_RTYPE_LVAL;
}
#else
#  define ED_MarkExprAsLVal(Expr)   do { (Expr)->Flags |= E_RTYPE_LVAL; } while (0)
#endif

#if defined(HAVE_INLINE)
INLINE void ED_MarkExprAsRVal (ExprDesc* Expr)
/* Mark the expression as an rvalue.
** HINT: Consider using ED_AddrExpr instead of this, unless you know what
**       consequence there will be, as there are both a big part in the code
**       assuming rvalue = const and a big part assuming rvalue = address.
*/
{
    Expr->Flags &= ~E_RTYPE_LVAL;
}
#else
#  define ED_MarkExprAsRVal(Expr)   do { (Expr)->Flags &= ~E_RTYPE_LVAL; } while (0)
#endif

void ED_AddrExpr (ExprDesc* Expr);
/* Take address of Expr */

void ED_IndExpr (ExprDesc* Expr);
/* Dereference Expr */

#if defined(HAVE_INLINE)
INLINE void ED_RequireTest (ExprDesc* Expr)
/* Mark the expression for a test. */
{
    Expr->Flags |= E_NEED_TEST;
}
#else
#  define ED_RequireTest(Expr)  do { (Expr)->Flags |= E_NEED_TEST; } while (0)
#endif

#if defined(HAVE_INLINE)
INLINE void ED_RequireNoTest (ExprDesc* Expr)
/* Mark the expression not for a test. */
{
    Expr->Flags &= ~E_NEED_TEST;
}
#else
#  define ED_RequireNoTest(Expr)    do { (Expr)->Flags &= ~E_NEED_TEST; } while (0)
#endif

#if defined(HAVE_INLINE)
INLINE void ED_TestDone (ExprDesc* Expr)
/* Mark the expression as tested and condition codes set. */
{
    Expr->Flags |= E_CC_SET;
}
#else
#  define ED_TestDone(Expr)     \
    do { (Expr)->Flags |= E_CC_SET; } while (0)
#endif

#if defined(HAVE_INLINE)
INLINE void ED_MarkAsUntested (ExprDesc* Expr)
/* Mark the expression as not tested (condition codes not set). */
{
    Expr->Flags &= ~E_CC_SET;
}
#else
#  define ED_MarkAsUntested(Expr)   do { (Expr)->Flags &= ~E_CC_SET; } while (0)
#endif

void ED_MarkForUneval (ExprDesc* Expr);
/* Mark the expression as not to be evaluated */

#if defined(HAVE_INLINE)
INLINE void ED_PropagateFrom (ExprDesc* Expr, const ExprDesc* SubExpr)
/* Propagate viral flags from subexpression */
{
    Expr->Flags |= SubExpr->Flags & E_MASK_VIRAL;
}
#else
#  define ED_PropagateFrom(Expr, SubExpr)   (void)((Expr)->Flags |= (SubExpr)->Flags & E_MASK_VIRAL)
#endif

const Type* ReplaceType (ExprDesc* Expr, const Type* NewType);
/* Replace the type of Expr by a copy of Newtype and return the old type string */



/*****************************************************************************/
/*                               Other Helpers                               */
/*****************************************************************************/



void PrintExprDesc (FILE* F, ExprDesc* Expr);
/* Print an ExprDesc */

void ED_SetCodeRange (ExprDesc* Expr, const CodeMark* Start, const CodeMark* End);
/* Set the code range for this expression */

int ED_CodeRangeIsEmpty (const ExprDesc* Expr);
/* Return true if no code was output for this expression */



/* End of exprdesc.h */

#endif
