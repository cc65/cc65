/*****************************************************************************/
/*                                                                           */
/*                                 ppexpr.h                                  */
/*                                                                           */
/*                      Expressions for C preprocessor                       */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2022  The cc65 Authors                                                */
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



/* cc65 */
#include "error.h"
#include "scanner.h"
#include "ppexpr.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* PP expression parser status */
static int PPEvaluationEnabled = 0;
static int PPEvaluationFailed  = 0;



/*****************************************************************************/
/*                                 Forwards                                  */
/*****************************************************************************/



static void PPhie0 (PPExpr* Expr);
static void PPhie1 (PPExpr* Expr);



/*****************************************************************************/
/*                             Helper functions                              */
/*****************************************************************************/



static token_t PPFindTok (token_t Tok, const token_t* Table)
/* Find a token in a generator table */
{
    while (*Table != TOK_INVALID) {
        if (*Table == Tok) {
            return Tok;
        }
        ++Table;
    }
    return TOK_INVALID;
}



static void PPExprInit (PPExpr* Expr)
/* Initialize the expression */
{
    Expr->IVal  = 0;
    Expr->Flags = PPEXPR_NONE;
}



static void PPErrorSkipLine (void)
/* Set the expression parser error flag, skip the remain tokens till the end
** of the line, clear the current and the next tokens.
*/
{
    PPEvaluationFailed = 1;
    SkipTokens (0, 0);
    CurTok.Tok  = TOK_CEOF;
    NextTok.Tok = TOK_CEOF;
}



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



static void PPhiePrimary (PPExpr* Expr)
/* This is the lowest level of the PP expression parser */
{
    switch (CurTok.Tok) {
        case TOK_ICONST:
        case TOK_CCONST:
            /* Character and integer constants */
            Expr->IVal = CurTok.IVal;
            /* According to the C standard, all signed types act as intmax_t
            ** and all unsigned types act as uintmax_t.
            */
            if (IsSignUnsigned (CurTok.Type)) {
                Expr->Flags |= PPEXPR_UNSIGNED;
            }
            NextToken ();
            break;

        case TOK_FCONST:
            /* Floating point constant */
            PPError ("Floating constant in preprocessor expression");
            Expr->IVal = 0;
            NextToken ();
            break;

        case TOK_LPAREN:
            /* Parse parenthesized subexpression by calling the whole parser
            ** recursively.
            */
            NextToken ();
            PPhie0 (Expr);
            ConsumeRParen ();
            break;

        case TOK_IDENT:
            /* Assume that this identifier is an undefined macro and replace
            ** it by a constant value of zero.
            */
            NextToken ();
            Expr->Flags |= PPEXPR_UNDEFINED;
            Expr->IVal = 0;
            break;

        case TOK_CEOF:
            /* Error recovery */
            break;

        default:
            /* Illegal expression in PP mode */
            PPError ("Preprocessor expression expected");
            PPErrorSkipLine ();
            break;
    }
}



static void PPhie11 (PPExpr* Expr)
/* Handle compound types (structs and arrays) etc which are invalid in PP */
{
    /* Evaluate the lhs */
    PPhiePrimary (Expr);

    /* Check for a rhs */
    while (CurTok.Tok == TOK_INC    || CurTok.Tok == TOK_DEC    ||
           CurTok.Tok == TOK_LBRACK || CurTok.Tok == TOK_LPAREN ||
           CurTok.Tok == TOK_DOT    || CurTok.Tok == TOK_PTR_REF) {

        switch (CurTok.Tok) {

            case TOK_LBRACK:
                PPError ("Token \".\" is not valid in preprocessor expressions");
                PPErrorSkipLine ();
                break;

            case TOK_LPAREN:
                /* Function call syntax is not recognized in preprocessor
                ** expressions.
                */
                PPError ("Missing binary operator before token \"(\"");
                PPErrorSkipLine ();
                break;

            case TOK_DOT:
                PPError ("Token \".\" is not valid in preprocessor expressions");
                PPErrorSkipLine ();
                break;

            case TOK_PTR_REF:
                PPError ("Token \"->\" is not valid in preprocessor expressions");
                PPErrorSkipLine ();
                break;

            case TOK_INC:
                PPError ("Token \"++\" is not valid in preprocessor expressions");
                PPErrorSkipLine ();
                break;

            case TOK_DEC:
                PPError ("Token \"--\" is not valid in preprocessor expressions");
                PPErrorSkipLine ();
                break;

            default:
                Internal ("Invalid token in PPhie11: %d", CurTok.Tok);

        }
    }
}



void PPhie10 (PPExpr* Expr)
/* Handle prefixing unary operators */
{
    switch (CurTok.Tok) {

        case TOK_INC:
            PPError ("Token \"++\" is not valid in preprocessor expressions");
            PPErrorSkipLine ();
            break;

        case TOK_DEC:
            PPError ("Token \"--\" is not valid in preprocessor expressions");
            PPErrorSkipLine ();
            break;

        case TOK_PLUS:
            NextToken ();
            PPhie10 (Expr);
            Expr->IVal = +Expr->IVal;
            break;

        case TOK_MINUS:
            NextToken ();
            PPhie10 (Expr);
            Expr->IVal = -Expr->IVal;
            break;

        case TOK_COMP:
            NextToken ();
            PPhie10 (Expr);
            Expr->IVal = ~Expr->IVal;
            break;

        case TOK_BOOL_NOT:
            NextToken ();
            PPhie10 (Expr);
            Expr->IVal = !Expr->IVal;
            break;

        case TOK_CEOF:
            /* Error recovery */
            break;

        case TOK_STAR:
        case TOK_AND:
        case TOK_SIZEOF:
        default:
            /* Type cast, sizeof, *, &, are not recognized in preprocessor
            ** expressions. So everything is treated as as expression here.
            */
            PPhie11 (Expr);
            break;
    }
}



static void PPhie_internal (const token_t* Ops,   /* List of generators */
                            PPExpr* Expr,
                            void (*hienext) (PPExpr*))
/* Helper function */
{
    token_t Tok;

    hienext (Expr);

    while ((Tok = PPFindTok (CurTok.Tok, Ops)) != 0) {

        PPExpr Rhs;
        PPExprInit (&Rhs);

        /* Remember the operator token, then skip it */
        NextToken ();

        /* Get the right hand side */
        hienext (&Rhs);

        if (PPEvaluationEnabled && !PPEvaluationFailed) {

            /* If either side is unsigned, the result is unsigned */
            Expr->Flags |= Rhs.Flags & PPEXPR_UNSIGNED;

            /* Handle the op differently for signed and unsigned integers */
            if ((Expr->Flags & PPEXPR_UNSIGNED) == 0) {

                /* Evaluate the result for signed operands */
                signed long Val1 = Expr->IVal;
                signed long Val2 = Rhs.IVal;
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
                    case TOK_PLUS:
                        Expr->IVal = (Val1 + Val2);
                        break;
                    case TOK_MINUS:
                        Expr->IVal = (Val1 - Val2);
                        break;
                    case TOK_MUL:
                        Expr->IVal = (Val1 * Val2);
                        break;
                    case TOK_DIV:
                        if (Val2 == 0) {
                            PPError ("Division by zero");
                            Expr->IVal = 0;
                        } else {
                            Expr->IVal = (Val1 / Val2);
                        }
                        break;
                    case TOK_MOD:
                        if (Val2 == 0) {
                            PPError ("Modulo operation with zero");
                            Expr->IVal = 0;
                        } else {
                            Expr->IVal = (Val1 % Val2);
                        }
                        break;
                    default:
                        Internal ("PPhie_internal: got token 0x%X\n", Tok);
                }

            } else {

                /* Evaluate the result for unsigned operands */
                unsigned long Val1 = Expr->IVal;
                unsigned long Val2 = Rhs.IVal;
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
                    case TOK_PLUS:
                        Expr->IVal = (Val1 + Val2);
                        break;
                    case TOK_MINUS:
                        Expr->IVal = (Val1 - Val2);
                        break;
                    case TOK_MUL:
                        Expr->IVal = (Val1 * Val2);
                        break;
                    case TOK_DIV:
                        if (Val2 == 0) {
                            PPError ("Division by zero");
                            Expr->IVal = 0;
                        } else {
                            Expr->IVal = (Val1 / Val2);
                        }
                        break;
                    case TOK_MOD:
                        if (Val2 == 0) {
                            PPError ("Modulo operation with zero");
                            Expr->IVal = 0;
                        } else {
                            Expr->IVal = (Val1 % Val2);
                        }
                        break;
                    default:
                        Internal ("PPhie_internal: got token 0x%X\n", Tok);
                }
            }
        }
    }
}



static void PPhie_compare (const token_t* Ops,    /* List of generators */
                           PPExpr* Expr,
                           void (*hienext) (PPExpr*))
/* Helper function for the compare operators */
{
    token_t Tok;

    hienext (Expr);

    while ((Tok = PPFindTok (CurTok.Tok, Ops)) != 0) {

        PPExpr Rhs;

        PPExprInit (&Rhs);

        /* Skip the operator token */
        NextToken ();

        /* Get the right hand side */
        hienext (&Rhs);

        if (PPEvaluationEnabled && !PPEvaluationFailed) {

            /* If either side is unsigned, the comparison is unsigned */
            Expr->Flags |= Rhs.Flags & PPEXPR_UNSIGNED;

            /* Determine if this is a signed or unsigned compare */
            if ((Expr->Flags & PPEXPR_UNSIGNED) == 0) {

                /* Evaluate the result for signed operands */
                signed long Val1 = Expr->IVal;
                signed long Val2 = Rhs.IVal;
                switch (Tok) {
                    case TOK_EQ: Expr->IVal = (Val1 == Val2);   break;
                    case TOK_NE: Expr->IVal = (Val1 != Val2);   break;
                    case TOK_LT: Expr->IVal = (Val1 < Val2);    break;
                    case TOK_LE: Expr->IVal = (Val1 <= Val2);   break;
                    case TOK_GE: Expr->IVal = (Val1 >= Val2);   break;
                    case TOK_GT: Expr->IVal = (Val1 > Val2);    break;
                    default:     Internal ("PPhie_compare: got token 0x%X\n", Tok);
                }

            } else {

                /* Evaluate the result for unsigned operands */
                unsigned long Val1 = Expr->IVal;
                unsigned long Val2 = Rhs.IVal;
                switch (Tok) {
                    case TOK_EQ: Expr->IVal = (Val1 == Val2);   break;
                    case TOK_NE: Expr->IVal = (Val1 != Val2);   break;
                    case TOK_LT: Expr->IVal = (Val1 < Val2);    break;
                    case TOK_LE: Expr->IVal = (Val1 <= Val2);   break;
                    case TOK_GE: Expr->IVal = (Val1 >= Val2);   break;
                    case TOK_GT: Expr->IVal = (Val1 > Val2);    break;
                    default:     Internal ("PPhie_compare: got token 0x%X\n", Tok);
                }
            }
        }
    }

    /* The result is signed */
    Expr->Flags &= ~PPEXPR_UNSIGNED;
}



static void PPhie9 (PPExpr* Expr)
/* Handle "*", "/" and "%" operators */
{
    static const token_t PPhie9_ops[] = {
        TOK_STAR,
        TOK_DIV,
        TOK_MOD,
        TOK_INVALID
    };

    PPhie_internal (PPhie9_ops, Expr, PPhie10);
}



static void PPhie8 (PPExpr* Expr)
/* Handle "+" and "-" binary operators */
{
    static const token_t PPhie8_ops[] = {
        TOK_PLUS,
        TOK_MINUS,
        TOK_INVALID
    };

    PPhie_internal (PPhie8_ops, Expr, PPhie9);
}



static void PPhie7 (PPExpr* Expr)
/* Handle the "<<" and ">>" shift operators */
{
    /* Evaluate the lhs */
    PPhie8 (Expr);

    while (CurTok.Tok == TOK_SHL || CurTok.Tok == TOK_SHR) {

        token_t Op;             /* The operator token */
        PPExpr Rhs;
        PPExprInit (&Rhs);

        /* Remember the operator, then skip its token */
        Op = CurTok.Tok;
        NextToken ();

        /* Get the right hand side */
        PPhie8 (&Rhs);

        /* Evaluate */
        if (PPEvaluationEnabled && !PPEvaluationFailed) {
            /* To shift by a negative value is equivalent to shift to the
            ** opposite direction.
            */
            if ((Rhs.Flags & PPEXPR_UNSIGNED) != 0 && Rhs.IVal > (long)LONG_BITS) {
                Rhs.IVal = (long)LONG_BITS;
            }
            if (Op == TOK_SHR) {
                Rhs.IVal = -Rhs.IVal;
            }

            /* Evaluate the result */
            if ((Expr->Flags & PPEXPR_UNSIGNED) != 0) {
                if (Rhs.IVal >= (long)LONG_BITS) {
                    /* For now we use (unsigned) long types for integer constants */
                    PPWarning ("Integer overflow in preprocessor expression");
                    Expr->IVal = 0;
                } else if (Rhs.IVal > 0) {
                    Expr->IVal <<= Rhs.IVal;
                } else if (Rhs.IVal < -(long)LONG_BITS) {
                    Expr->IVal = 0;
                } else if (Rhs.IVal < 0) {
                    Expr->IVal = (unsigned long)Expr->IVal >> -Rhs.IVal;
                }
            } else {
                if (Rhs.IVal >= (long)(LONG_BITS - 1)) {
                    /* For now we use (unsigned) long types for integer constants */
                    PPWarning ("Integer overflow in preprocessor expression");
                    Expr->IVal = 0;
                } else if (Rhs.IVal > 0) {
                    Expr->IVal <<= Rhs.IVal;
                } else if (Rhs.IVal < -(long)LONG_BITS) {
                    Expr->IVal = -1;
                } else if (Rhs.IVal < 0) {
                    Expr->IVal >>= Expr->IVal >> -Rhs.IVal;
                }
            }
        }
    }
}



static void PPhie6 (PPExpr* Expr)
/* Handle greater-than type relational operators */
{
    static const token_t PPhie6_ops [] = {
        TOK_LT,
        TOK_LE,
        TOK_GE,
        TOK_GT,
        TOK_INVALID
    };

    PPhie_compare (PPhie6_ops, Expr, PPhie7);
}



static void PPhie5 (PPExpr* Expr)
/* Handle "==" and "!=" relational operators */
{
    static const token_t PPhie5_ops[] = {
        TOK_EQ,
        TOK_NE,
        TOK_INVALID
    };

    PPhie_compare (PPhie5_ops, Expr, PPhie6);
}



static void PPhie4 (PPExpr* Expr)
/* Handle the bitwise AND "&" operator */
{
    static const token_t PPhie4_ops[] = {
        TOK_AND,
        TOK_INVALID
    };

    PPhie_internal (PPhie4_ops, Expr, PPhie5);
}



static void PPhie3 (PPExpr* Expr)
/* Handle the bitwise exclusive OR "^" operator */
{
    static const token_t PPhie3_ops[] = {
        TOK_XOR,
        TOK_INVALID
    };

    PPhie_internal (PPhie3_ops, Expr, PPhie4);
}



static void PPhie2 (PPExpr* Expr)
/* Handle the bitwise OR "|" operator */
{
    static const token_t PPhie2_ops[] = {
        TOK_OR,
        TOK_INVALID
    };

    PPhie_internal (PPhie2_ops, Expr, PPhie3);
}



static void PPhieAnd (PPExpr* Expr)
/* Handle the logical AND "expr1 && expr2" operator */
{
    /* Get one operand */
    PPhie2 (Expr);

    if (CurTok.Tok == TOK_BOOL_AND) {

        int PPEvaluationEnabledPrev = PPEvaluationEnabled;
        PPExpr One;

        /* Do logical and */
        Expr->IVal = (Expr->IVal != 0);
        if (Expr->IVal == 0) {
            PPEvaluationEnabled = 0;
        }

        /* While there are more expressions */
        while (CurTok.Tok == TOK_BOOL_AND) {
            /* Skip the && */
            NextToken ();

            /* Get one operand */
            PPExprInit (&One);
            PPhie2 (&One);

            /* Evaluate */
            if (PPEvaluationEnabled) {
                if (One.IVal == 0) {
                    /* Skip evaluating remaining */
                    PPEvaluationEnabled = 0;
                    /* The value of the result will be false */
                    Expr->IVal = 0;
                }
            }
        }

        /* Restore evaluation as before */
        PPEvaluationEnabled = PPEvaluationEnabledPrev;
    }
}



static void PPhieOr (PPExpr* Expr)
/* Handle the logical OR "||" operator */
{
    /* Call the next level parser */
    PPhieAnd (Expr);

    if (CurTok.Tok == TOK_BOOL_OR) {

        int PPEvaluationEnabledPrev = PPEvaluationEnabled;
        PPExpr One;

        /* Do logical or */
        Expr->IVal = (Expr->IVal != 0);
        if (Expr->IVal != 0) {
            PPEvaluationEnabled = 0;
        }

        /* While there are more expressions */
        while (CurTok.Tok == TOK_BOOL_OR) {
            /* Skip the || */
            NextToken ();

            /* Get rhs subexpression */
            PPExprInit (&One);
            PPhieAnd (&One);

            /* Evaluate */
            if (PPEvaluationEnabled) {
                if (One.IVal != 0) {
                    /* Skip evaluating remaining */
                    PPEvaluationEnabled = 0;
                    /* The value of the result will be true */
                    Expr->IVal = 1;
                }
            }
        }

        /* Restore evaluation as before */
        PPEvaluationEnabled = PPEvaluationEnabledPrev;
    }
}



static void PPhieQuest (PPExpr* Expr)
/* Handle the ternary "expr1 ? expr2 : expr3 " operator */
{
    /* Call the lower level eval routine */
    PPhieOr (Expr);

    /* Check if it's a ternary expression */
    if (CurTok.Tok == TOK_QUEST) {
        int PPEvaluationEnabledPrev = PPEvaluationEnabled;
        PPExpr Expr2;       /* Expression 2 */
        PPExpr Expr3;       /* Expression 3 */

        /* Skip the question mark */
        NextToken ();

        /* Disable evaluation for Expr2 if the condition is false */
        if (Expr->IVal == 0) {
            PPEvaluationEnabled = 0;
        }

        /* Parse second expression */
        PPExprInit (&Expr2);
        PPhie0 (&Expr2);

        /* Skip the colon */
        ConsumeColon ();

        /* Disable evaluation for Expr3 if the condition is true */
        if (Expr->IVal != 0) {
            PPEvaluationEnabled = 0;
        }

        /* Parse third expression */
        PPExprInit (&Expr3);
        PPhie1 (&Expr3);

        /* Set the result */
        Expr->IVal = Expr->IVal ? Expr2.IVal != 0 : Expr3.IVal != 0;

        /* Restore evaluation as before */
        PPEvaluationEnabled = PPEvaluationEnabledPrev;
    }
}



static void PPhie1 (PPExpr* Expr)
/* Handle first level of expression hierarchy */
{
    PPhieQuest (Expr);

    if (!PPEvaluationEnabled) {
        /* Skip evaluation */
        return;
    }

    switch (CurTok.Tok) {

        case TOK_ASSIGN:
            PPError ("Token \"=\" is not valid in preprocessor expressions");
            PPErrorSkipLine ();
            break;

        case TOK_PLUS_ASSIGN:
            PPError ("Token \"+=\" is not valid in preprocessor expressions");
            PPErrorSkipLine ();
            break;

        case TOK_MINUS_ASSIGN:
            PPError ("Token \"-=\" is not valid in preprocessor expressions");
            PPErrorSkipLine ();
            break;

        case TOK_MUL_ASSIGN:
            PPError ("Token \"*=\" is not valid in preprocessor expressions");
            PPErrorSkipLine ();
            break;

        case TOK_DIV_ASSIGN:
            PPError ("Token \"/=\" is not valid in preprocessor expressions");
            PPErrorSkipLine ();
            break;

        case TOK_MOD_ASSIGN:
            PPError ("Token \"%%=\" is not valid in preprocessor expressions");
            PPErrorSkipLine ();
            break;

        case TOK_SHL_ASSIGN:
            PPError ("Token \"<<=\" is not valid in preprocessor expressions");
            PPErrorSkipLine ();
            break;

        case TOK_SHR_ASSIGN:
            PPError ("Token \">>=\" is not valid in preprocessor expressions");
            PPErrorSkipLine ();
            break;

        case TOK_AND_ASSIGN:
            PPError ("Token \"&=\" is not valid in preprocessor expressions");
            PPErrorSkipLine ();
            break;

        case TOK_OR_ASSIGN:
            PPError ("Token \"|=\" is not valid in preprocessor expressions");
            PPErrorSkipLine ();
            break;

        case TOK_XOR_ASSIGN:
            PPError ("Token \"^=\" is not valid in preprocessor expressions");
            PPErrorSkipLine ();
            break;

        default:
            break;
    }
}



static void PPhie0 (PPExpr* Expr)
/* Handle the comma "," operator */
{
    PPhie1 (Expr);

    while (CurTok.Tok == TOK_COMMA) {
        /* Skip the comma */
        NextToken ();
        /* Reset the expression */
        PPExprInit (Expr);
        /* Use the next operand as the value instead */
        PPhie1 (Expr);
    }
}



void ParsePPExprInLine (PPExpr* Expr)
/* Parse a line for PP expression */
{
    /* Initialize the parser status */
    PPEvaluationFailed = 0;
    PPEvaluationEnabled = 1;
    NextLineDisabled = 1;

    /* Parse */
    PPExprInit (Expr);
    PPhie0 (Expr);

    /* If the evaluation fails, the result is always zero */
    if (PPEvaluationFailed) {
        Expr->IVal = 0;
        PPEvaluationFailed = 0;
    }

    /* Restore parser status */
    NextLineDisabled = 0;
}
