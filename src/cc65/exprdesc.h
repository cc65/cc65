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
    /* Location: Where is the value we're talking about? */
    E_MASK_LOC          = 0x00FF,
    E_LOC_ABS           = 0x0001,       /* Absolute: numeric address or const */
    E_LOC_GLOBAL        = 0x0002,       /* Global variable */
    E_LOC_STATIC        = 0x0004,       /* Static variable */
    E_LOC_REGISTER      = 0x0008,       /* Register variable */
    E_LOC_STACK         = 0x0010,       /* Value on the stack */
    E_LOC_PRIMARY       = 0x0020,       /* The primary register */
    E_LOC_EXPR          = 0x0040,       /* An expression in the primary register */
    E_LOC_LITERAL       = 0x0080,       /* Literal in the literal pool */

    /* Constant location of some sort (only if rval) */
    E_LOC_CONST         = E_LOC_ABS | E_LOC_GLOBAL | E_LOC_STATIC |
                          E_LOC_REGISTER | E_LOC_LITERAL,

    /* Reference? */
    E_MASK_RTYPE        = 0x0100,
    E_RTYPE_RVAL        = 0x0000,
    E_RTYPE_LVAL        = 0x0100,

    /* Bit-field? */
    E_BITFIELD          = 0x0200,

    /* Test */
    E_NEED_TEST         = 0x0400,       /* Expression needs a test to set cc */
    E_CC_SET            = 0x0800,       /* Condition codes are set */

    E_HAVE_MARKS        = 0x1000,       /* Code marks are valid */

};

/* Forward */
struct Literal;

/* Describe the result of an expression */
typedef struct ExprDesc ExprDesc;
struct ExprDesc {
    struct SymEntry*    Sym;            /* Symbol table entry if known */
    Type*               Type;           /* Type array of expression */
    unsigned            Flags;
    unsigned long       Name;           /* Name or label number */
    long                IVal;           /* Integer value if expression constant */
    Double              FVal;           /* Floating point value */
    struct Literal*     LVal;           /* Literal value */

    /* Bit field stuff */
    unsigned            BitOffs;        /* Bit offset for bit fields */
    unsigned            BitWidth;       /* Bit width for bit fields */

    /* Start and end of generated code */
    CodeMark            Start;
    CodeMark            End;
};



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



ExprDesc* ED_Init (ExprDesc* Expr);
/* Initialize an ExprDesc */

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
INLINE int ED_IsLocAbs (const ExprDesc* Expr)
/* Return true if the expression is an absolute value */
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
#  define ED_IsLocRegister(Expr)    (((Expr)->Flags & E_MASK_LOC) == E_LOC_REGISTER)
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
/* Return true if the expression is an expression in the register pseudo variable */
{
    return (Expr->Flags & E_MASK_LOC) == E_LOC_PRIMARY;
}
#else
#  define ED_IsLocPrimary(Expr)  (((Expr)->Flags & E_MASK_LOC) == E_LOC_PRIMARY)
#endif

#if defined(HAVE_INLINE)
INLINE int ED_IsLocExpr (const ExprDesc* Expr)
/* Return true if the expression is an expression in the primary */
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
    return (Expr->Flags & E_LOC_CONST) != 0;
}
#else
#  define ED_IsLocConst(Expr)  (((Expr)->Flags & E_LOC_CONST) != 0)
#endif

#if defined(HAVE_INLINE)
INLINE int ED_IsLVal (const ExprDesc* Expr)
/* Return true if the expression is a reference */
{
    return (Expr->Flags & E_MASK_RTYPE) == E_RTYPE_LVAL;
}
#else
#  define ED_IsLVal(Expr)       (((Expr)->Flags & E_MASK_RTYPE) == E_RTYPE_LVAL)
#endif

#if defined(HAVE_INLINE)
INLINE int ED_IsRVal (const ExprDesc* Expr)
/* Return true if the expression is a rvalue */
{
    return (Expr->Flags & E_MASK_RTYPE) == E_RTYPE_RVAL;
}
#else
#  define ED_IsRVal(Expr)       (((Expr)->Flags & E_MASK_RTYPE) == E_RTYPE_RVAL)
#endif

#if defined(HAVE_INLINE)
INLINE void ED_MakeLVal (ExprDesc* Expr)
/* Make the expression a lvalue. */
{
    Expr->Flags |= E_RTYPE_LVAL;
}
#else
#  define ED_MakeLVal(Expr)     do { (Expr)->Flags |= E_RTYPE_LVAL; } while (0)
#endif

#if defined(HAVE_INLINE)
INLINE void ED_MakeRVal (ExprDesc* Expr)
/* Make the expression a rvalue. */
{
    Expr->Flags &= ~E_RTYPE_LVAL;
}
#else
#  define ED_MakeRVal(Expr)     do { (Expr)->Flags &= ~E_RTYPE_LVAL; } while (0)
#endif

#if defined(HAVE_INLINE)
INLINE int ED_IsBitField (const ExprDesc* Expr)
/* Return true if the expression is a bit field */
{
    return (Expr->Flags & E_BITFIELD) != 0;
}
#else
#  define ED_IsBitField(Expr)   (((Expr)->Flags & E_BITFIELD) != 0)
#endif

void ED_MakeBitField (ExprDesc* Expr, unsigned BitOffs, unsigned BitWidth);
/* Make this expression a bit field expression */

#if defined(HAVE_INLINE)
INLINE void ED_MarkForTest (ExprDesc* Expr)
/* Mark the expression for a test. */
{
    Expr->Flags |= E_NEED_TEST;
}
#else
#  define ED_MarkForTest(Expr)  do { (Expr)->Flags |= E_NEED_TEST; } while (0)
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
INLINE void ED_TestDone (ExprDesc* Expr)
/* Mark the expression as tested and condition codes set. */
{
    Expr->Flags = (Expr->Flags & ~E_NEED_TEST) | E_CC_SET;
}
#else
#  define ED_TestDone(Expr)     \
    do { (Expr)->Flags = ((Expr)->Flags & ~E_NEED_TEST) | E_CC_SET; } while (0)
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
INLINE void ED_MarkAsUntested (ExprDesc* Expr)
/* Mark the expression as not tested (condition codes not set). */
{
    Expr->Flags &= ~E_CC_SET;
}
#else
#  define ED_MarkAsUntested(Expr)   do { (Expr)->Flags &= ~E_CC_SET; } while (0)
#endif

void ED_SetCodeRange (ExprDesc* Expr, const CodeMark* Start, const CodeMark* End);
/* Set the code range for this expression */

int ED_CodeRangeIsEmpty (const ExprDesc* Expr);
/* Return true if no code was output for this expression */

const char* ED_GetLabelName (const ExprDesc* Expr, long Offs);
/* Return the assembler label name of the given expression. Beware: This
** function may use a static buffer, so the name may get "lost" on the second
** call to the function.
*/

int ED_GetStackOffs (const ExprDesc* Expr, int Offs);
/* Get the stack offset of an address on the stack in Expr taking into account
** an additional offset in Offs.
*/

ExprDesc* ED_MakeConstAbs (ExprDesc* Expr, long Value, Type* Type);
/* Make Expr an absolute const with the given value and type. */

ExprDesc* ED_MakeConstAbsInt (ExprDesc* Expr, long Value);
/* Make Expr a constant integer expression with the given value */

ExprDesc* ED_MakeRValExpr (ExprDesc* Expr);
/* Convert Expr into a rvalue which is in the primary register without an
** offset.
*/

ExprDesc* ED_MakeLValExpr (ExprDesc* Expr);
/* Convert Expr into a lvalue which is in the primary register without an
** offset.
*/

int ED_IsConst (const ExprDesc* Expr);
/* Return true if the expression denotes a constant of some sort. This can be a
** numeric constant, the address of a global variable (maybe with offset) or
** similar.
*/

#if defined(HAVE_INLINE)
INLINE int ED_IsConstAbs (const ExprDesc* Expr)
/* Return true if the expression denotes a constant absolute value. This can be
** a numeric constant, cast to any type.
*/
{
    return (Expr->Flags & (E_MASK_LOC|E_MASK_RTYPE)) == (E_LOC_ABS|E_RTYPE_RVAL);
}
#else
#  define ED_IsConstAbs(E)      \
        (((E)->Flags & (E_MASK_LOC|E_MASK_RTYPE)) == (E_LOC_ABS|E_RTYPE_RVAL))
#endif

int ED_IsConstAbsInt (const ExprDesc* Expr);
/* Return true if the expression is a constant (numeric) integer. */

int ED_IsNullPtr (const ExprDesc* Expr);
/* Return true if the given expression is a NULL pointer constant */

int ED_IsBool (const ExprDesc* Expr);
/* Return true of the expression can be treated as a boolean, that is, it can
** be an operand to a compare operation.
*/

void PrintExprDesc (FILE* F, ExprDesc* Expr);
/* Print an ExprDesc */

Type* ReplaceType (ExprDesc* Expr, const Type* NewType);
/* Replace the type of Expr by a copy of Newtype and return the old type string */



/* End of exprdesc.h */

#endif
