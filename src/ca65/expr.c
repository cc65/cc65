/*****************************************************************************/
/*                                                                           */
/*                                  expr.c                                   */
/*                                                                           */
/*             Expression evaluation for the ca65 macroassembler             */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2012, Ullrich von Bassewitz                                      */
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



#include <string.h>
#include <time.h>

/* common */
#include "check.h"
#include "cpu.h"
#include "exprdefs.h"
#include "print.h"
#include "shift.h"
#include "segdefs.h"
#include "strbuf.h"
#include "tgttrans.h"
#include "version.h"
#include "xmalloc.h"

/* ca65 */
#include "error.h"
#include "expr.h"
#include "global.h"
#include "instr.h"
#include "nexttok.h"
#include "objfile.h"
#include "segment.h"
#include "sizeof.h"
#include "studyexpr.h"
#include "symbol.h"
#include "symtab.h"
#include "toklist.h"
#include "ulabel.h"
#include "macro.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Since all expressions are first packed into expression trees, and each
** expression tree node is allocated on the heap, we add some type of special
** purpose memory allocation here: Instead of freeing the nodes, we save some
** number of freed nodes for later and remember them in a single linked list
** using the Left link.
*/
#define MAX_FREE_NODES  64
static ExprNode*        FreeExprNodes = 0;
static unsigned         FreeNodeCount = 0;



/*****************************************************************************/
/*                                  Helpers                                  */
/*****************************************************************************/



static ExprNode* NewExprNode (unsigned Op)
/* Create a new expression node */
{
    ExprNode* N;

    /* Do we have some nodes in the list already? */
    if (FreeNodeCount) {
        /* Use first node from list */
        N = FreeExprNodes;
        FreeExprNodes = N->Left;
        --FreeNodeCount;
    } else {
        /* Allocate fresh memory */
        N = xmalloc (sizeof (ExprNode));
    }
    N->Op = Op;
    N->Left = N->Right = 0;
    N->Obj = 0;

    return N;
}



static void FreeExprNode (ExprNode* E)
/* Free a node */
{
    if (E) {
        if (E->Op == EXPR_SYMBOL) {
            /* Remove the symbol reference */
            SymDelExprRef (E->V.Sym, E);
        }
        /* Place the symbol into the free nodes list if possible */
        if (FreeNodeCount < MAX_FREE_NODES) {
            /* Remember this node for later */
            E->Left = FreeExprNodes;
            FreeExprNodes = E;
            ++FreeNodeCount;
        } else {
            /* Free the memory */
            xfree (E);
        }
    }
}



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



static ExprNode* Expr0 (void);



int IsByteRange (long Val)
/* Return true if this is a byte value */
{
    return (Val & ~0xFFL) == 0;
}



int IsWordRange (long Val)
/* Return true if this is a word value */
{
    return (Val & ~0xFFFFL) == 0;
}



int IsFarRange (long Val)
/* Return true if this is a far (24 bit) value */
{
    return (Val & ~0xFFFFFFL) == 0;
}



int IsEasyConst (const ExprNode* E, long* Val)
/* Do some light checking if the given node is a constant. Don't care if E is
** a complex expression. If E is a constant, return true and place its value
** into Val, provided that Val is not NULL.
*/
{
    /* Resolve symbols, follow symbol chains */
    while (E->Op == EXPR_SYMBOL) {
        E = SymResolve (E->V.Sym);
        if (E == 0) {
            /* Could not resolve */
            return 0;
        }
    }

    /* Symbols resolved, check for a literal */
    if (E->Op == EXPR_LITERAL) {
        if (Val) {
            *Val = E->V.IVal;
        }
        return 1;
    }

    /* Not found to be a const according to our tests */
    return 0;
}



static ExprNode* LoByte (ExprNode* Operand)
/* Return the low byte of the given expression */
{
    ExprNode* Expr;
    long      Val;

    /* Special handling for const expressions */
    if (IsEasyConst (Operand, &Val)) {
        FreeExpr (Operand);
        Expr = GenLiteralExpr (Val & 0xFF);
    } else {
        /* Extract byte #0 */
        Expr = NewExprNode (EXPR_BYTE0);
        Expr->Left = Operand;
    }
    return Expr;
}



static ExprNode* HiByte (ExprNode* Operand)
/* Return the high byte of the given expression */
{
    ExprNode* Expr;
    long      Val;

    /* Special handling for const expressions */
    if (IsEasyConst (Operand, &Val)) {
        FreeExpr (Operand);
        Expr = GenLiteralExpr ((Val >> 8) & 0xFF);
    } else {
        /* Extract byte #1 */
        Expr = NewExprNode (EXPR_BYTE1);
        Expr->Left = Operand;
    }
    return Expr;
}



static ExprNode* Bank (ExprNode* Operand)
/* Return the bank of the given segmented expression */
{
    /* Generate the bank expression */
    ExprNode* Expr = NewExprNode (EXPR_BANK);
    Expr->Left = Operand;

    /* Return the result */
    return Expr;
}



static ExprNode* BankByte (ExprNode* Operand)
/* Return the bank byte of the given expression */
{
    ExprNode* Expr;
    long      Val;

    /* Special handling for const expressions */
    if (IsEasyConst (Operand, &Val)) {
        FreeExpr (Operand);
        Expr = GenLiteralExpr ((Val >> 16) & 0xFF);
    } else {
        /* Extract byte #2 */
        Expr = NewExprNode (EXPR_BYTE2);
        Expr->Left = Operand;
    }
    return Expr;
}



static ExprNode* LoWord (ExprNode* Operand)
/* Return the low word of the given expression */
{
    ExprNode* Expr;
    long      Val;

    /* Special handling for const expressions */
    if (IsEasyConst (Operand, &Val)) {
        FreeExpr (Operand);
        Expr = GenLiteralExpr (Val & 0xFFFF);
    } else {
        /* Extract word #0 */
        Expr = NewExprNode (EXPR_WORD0);
        Expr->Left = Operand;
    }
    return Expr;
}



static ExprNode* HiWord (ExprNode* Operand)
/* Return the high word of the given expression */
{
    ExprNode* Expr;
    long      Val;

    /* Special handling for const expressions */
    if (IsEasyConst (Operand, &Val)) {
        FreeExpr (Operand);
        Expr = GenLiteralExpr ((Val >> 16) & 0xFFFF);
    } else {
        /* Extract word #1 */
        Expr = NewExprNode (EXPR_WORD1);
        Expr->Left = Operand;
    }
    return Expr;
}



static ExprNode* Symbol (SymEntry* S)
/* Reference a symbol and return an expression for it */
{
    if (S == 0) {
        /* Some weird error happened before */
        return GenLiteralExpr (0);
    } else {
        /* Mark the symbol as referenced */
        SymRef (S);
        /* If the symbol is a variable, return just its value, otherwise
        ** return a reference to the symbol.
        */
        if (SymIsVar (S)) {
            return CloneExpr (GetSymExpr (S));
        } else {
            /* Create symbol node */
            return GenSymExpr (S);
        }
    }
}



ExprNode* FuncBank (void)
/* Handle the .BANK builtin function */
{
    return Bank (Expression ());
}



ExprNode* FuncBankByte (void)
/* Handle the .BANKBYTE builtin function */
{
    return BankByte (Expression ());
}



static ExprNode* FuncBlank (void)
/* Handle the .BLANK builtin function */
{
    /* We have a list of tokens that ends with the closing paren. Skip
    ** the tokens, and count them. Allow optionally curly braces.
    */
    token_t Term = GetTokListTerm (TOK_RPAREN);
    unsigned Count = 0;
    while (CurTok.Tok != Term) {

        /* Check for end of line or end of input. Since the calling function
        ** will check for the closing paren, we don't need to print an error
        ** here, just bail out.
        */
        if (TokIsSep (CurTok.Tok)) {
            break;
        }

        /* One more token */
        ++Count;

        /* Skip the token */
        NextTok ();
    }

    /* If the list was enclosed in curly braces, skip the closing brace */
    if (Term == TOK_RCURLY && CurTok.Tok == TOK_RCURLY) {
        NextTok ();
    }

    /* Return true if the list was empty */
    return GenLiteralExpr (Count == 0);
}



static ExprNode* FuncConst (void)
/* Handle the .CONST builtin function */
{
    /* Read an expression */
    ExprNode* Expr = Expression ();

    /* Check the constness of the expression */
    ExprNode* Result = GenLiteralExpr (IsConstExpr (Expr, 0));

    /* Free the expression */
    FreeExpr (Expr);

    /* Done */
    return Result;
}



static ExprNode* FuncDefined (void)
/* Handle the .DEFINED builtin function */
{
    /* Parse the symbol name and search for the symbol */
    SymEntry* Sym = ParseAnySymName (SYM_FIND_EXISTING);

    /* Check if the symbol is defined */
    return GenLiteralExpr (Sym != 0 && SymIsDef (Sym));
}



static ExprNode* FuncDefinedMacro (void)
/* Handle the .DEFINEDMACRO builtin function */
{
    Macro* Mac = 0;

    /* Check if the identifier is a macro */

    if (CurTok.Tok == TOK_IDENT) {
        Mac = FindMacro (&CurTok.SVal);
    } else {
        Error ("Identifier expected.");
    }
    /* Skip the name */
    NextTok ();

    return GenLiteralExpr (Mac != 0);
}



ExprNode* FuncHiByte (void)
/* Handle the .HIBYTE builtin function */
{
    return HiByte (Expression ());
}



static ExprNode* FuncHiWord (void)
/* Handle the .HIWORD builtin function */
{
    return HiWord (Expression ());
}



static ExprNode* FuncIsMnemonic (void)
/* Handle the .ISMNEMONIC, .ISMNEM builtin function */
{
    int Instr = -1;

    /* Check for a macro or an instruction depending on UbiquitousIdents */

    if (CurTok.Tok == TOK_IDENT) {
        if (UbiquitousIdents) {
            /* Macros CAN be instructions, so check for them first */
            if (FindMacro (&CurTok.SVal) == 0) {
                Instr = FindInstruction (&CurTok.SVal);
            }
        }
        else {
            /* Macros and symbols may NOT use the names of instructions, so just check for the instruction */
            Instr = FindInstruction (&CurTok.SVal);
        }
    }
    else {
        Error ("Identifier expected.");
    }
    /* Skip the name */
    NextTok ();

    return GenLiteralExpr (Instr > 0);
}



ExprNode* FuncLoByte (void)
/* Handle the .LOBYTE builtin function */
{
    return LoByte (Expression ());
}



static ExprNode* FuncLoWord (void)
/* Handle the .LOWORD builtin function */
{
    return LoWord (Expression ());
}



static ExprNode* DoMatch (enum TC EqualityLevel)
/* Handle the .MATCH and .XMATCH builtin functions */
{
    int Result;
    TokNode* Root = 0;
    TokNode* Last = 0;
    TokNode* Node;

    /* A list of tokens follows. Read this list and remember it building a
    ** single linked list of tokens including attributes. The list is
    ** either enclosed in curly braces, or terminated by a comma.
    */
    token_t Term = GetTokListTerm (TOK_COMMA);
    while (CurTok.Tok != Term) {

        /* We may not end-of-line of end-of-file here */
        if (TokIsSep (CurTok.Tok)) {
            Error ("Unexpected end of line");
            return GenLiteral0 ();
        }

        /* Get a node with this token */
        Node = NewTokNode ();

        /* Insert the node into the list */
        if (Last == 0) {
            Root = Node;
        } else {
            Last->Next = Node;
        }
        Last = Node;

        /* Skip the token */
        NextTok ();
    }

    /* Skip the terminator token*/
    NextTok ();

    /* If the token list was enclosed in curly braces, we expect a comma */
    if (Term == TOK_RCURLY) {
        ConsumeComma ();
    }

    /* Read the second list which is optionally enclosed in curly braces and
    ** terminated by the right parenthesis. Compare each token against the
    ** one in the first list.
    */
    Term = GetTokListTerm (TOK_RPAREN);
    Result = 1;
    Node = Root;
    while (CurTok.Tok != Term) {

        /* We may not end-of-line of end-of-file here */
        if (TokIsSep (CurTok.Tok)) {
            Error ("Unexpected end of line");
            return GenLiteral0 ();
        }

        /* Compare the tokens if the result is not already known */
        if (Result != 0) {
            if (Node == 0) {
                /* The second list is larger than the first one */
                Result = 0;
            } else if (TokCmp (Node) < EqualityLevel) {
                /* Tokens do not match */
                Result = 0;
            }
        }

        /* Next token in first list */
        if (Node) {
            Node = Node->Next;
        }

        /* Next token in current list */
        NextTok ();
    }

    /* If the token list was enclosed in curly braces, eat the closing brace */
    if (Term == TOK_RCURLY) {
        NextTok ();
    }

    /* Check if there are remaining tokens in the first list */
    if (Node != 0) {
        Result = 0;
    }

    /* Free the token list */
    while (Root) {
        Node = Root;
        Root = Root->Next;
        FreeTokNode (Node);
    }

    /* Done, return the result */
    return GenLiteralExpr (Result);
}



static ExprNode* FuncMatch (void)
/* Handle the .MATCH function */
{
    return DoMatch (tcSameToken);
}



static ExprNode* FuncMax (void)
/* Handle the .MAX function */
{
    ExprNode* Left;
    ExprNode* Right;
    ExprNode* Expr;
    long LeftVal, RightVal;

    /* Two arguments to the pseudo function */
    Left = Expression ();
    ConsumeComma ();
    Right = Expression ();

    /* Check if we can evaluate the value immediately */
    if (IsEasyConst (Left, &LeftVal) && IsEasyConst (Right, &RightVal)) {
        FreeExpr (Left);
        FreeExpr (Right);
        Expr = GenLiteralExpr ((LeftVal > RightVal)? LeftVal : RightVal);
    } else {
        /* Make an expression node */
        Expr = NewExprNode (EXPR_MAX);
        Expr->Left = Left;
        Expr->Right = Right;
    }
    return Expr;
}



static ExprNode* FuncMin (void)
/* Handle the .MIN function */
{
    ExprNode* Left;
    ExprNode* Right;
    ExprNode* Expr;
    long LeftVal, RightVal;

    /* Two arguments to the pseudo function */
    Left = Expression ();
    ConsumeComma ();
    Right = Expression ();

    /* Check if we can evaluate the value immediately */
    if (IsEasyConst (Left, &LeftVal) && IsEasyConst (Right, &RightVal)) {
        FreeExpr (Left);
        FreeExpr (Right);
        Expr = GenLiteralExpr ((LeftVal < RightVal)? LeftVal : RightVal);
    } else {
        /* Make an expression node */
        Expr = NewExprNode (EXPR_MIN);
        Expr->Left = Left;
        Expr->Right = Right;
    }
    return Expr;
}



static ExprNode* FuncReferenced (void)
/* Handle the .REFERENCED builtin function */
{
    /* Parse the symbol name and search for the symbol */
    SymEntry* Sym = ParseAnySymName (SYM_FIND_EXISTING);

    /* Check if the symbol is referenced */
    return GenLiteralExpr (Sym != 0 && SymIsRef (Sym));
}



static ExprNode* FuncAddrSize (void)
/* Handle the .ADDRSIZE function */
{
    StrBuf    ScopeName = STATIC_STRBUF_INITIALIZER;
    StrBuf    Name = STATIC_STRBUF_INITIALIZER;
    SymEntry* Sym;
    int       AddrSize;
    int       NoScope;


    /* Assume we don't know the size */
    AddrSize = 0;

    /* Check for a cheap local which needs special handling */
    if (CurTok.Tok == TOK_LOCAL_IDENT) {

        /* Cheap local symbol */
        Sym = SymFindLocal (SymLast, &CurTok.SVal, SYM_FIND_EXISTING);
        if (Sym == 0) {
            Error ("Unknown symbol or scope: `%m%p'", &CurTok.SVal);
        } else {
            AddrSize = Sym->AddrSize;
        }

        /* Remember and skip SVal, terminate ScopeName so it is empty */
        SB_Copy (&Name, &CurTok.SVal);
        NextTok ();
        SB_Terminate (&ScopeName);

    } else {

        /* Parse the scope and the name */
        SymTable* ParentScope = ParseScopedIdent (&Name, &ScopeName);

        /* Check if the parent scope is valid */
        if (ParentScope == 0) {
            /* No such scope */
            SB_Done (&ScopeName);
            SB_Done (&Name);
            return GenLiteral0 ();
        }

        /* If ScopeName is empty, no explicit scope was specified. We have to
        ** search upper scope levels in this case.
        */
        NoScope = SB_IsEmpty (&ScopeName);

        /* If we did find a scope with the name, read the symbol defining the
        ** size, otherwise search for a symbol entry with the name and scope.
        */
        if (NoScope) {
            Sym = SymFindAny (ParentScope, &Name);
        } else {
            Sym = SymFind (ParentScope, &Name, SYM_FIND_EXISTING);
        }
        /* If we found the symbol retrieve the size, otherwise complain */
        if (Sym) {
            AddrSize = Sym->AddrSize;
        } else {
            Error ("Unknown symbol or scope: `%m%p%m%p'", &ScopeName, &Name);
        }

    }

    if (AddrSize == 0) {
        Warning (1, "Unknown address size: `%m%p%m%p'", &ScopeName, &Name);
    }

    /* Free the string buffers */
    SB_Done (&ScopeName);
    SB_Done (&Name);

    /* Return the size. */

    return GenLiteralExpr (AddrSize);
}



static ExprNode* FuncSizeOf (void)
/* Handle the .SIZEOF function */
{
    StrBuf    ScopeName = STATIC_STRBUF_INITIALIZER;
    StrBuf    Name = STATIC_STRBUF_INITIALIZER;
    SymTable* Scope;
    SymEntry* Sym;
    SymEntry* SizeSym;
    long      Size;
    int       NoScope;


    /* Assume an error */
    SizeSym = 0;

    /* Check for a cheap local which needs special handling */
    if (CurTok.Tok == TOK_LOCAL_IDENT) {

        /* Cheap local symbol */
        Sym = SymFindLocal (SymLast, &CurTok.SVal, SYM_FIND_EXISTING);
        if (Sym == 0) {
            Error ("Unknown symbol or scope: `%m%p'", &CurTok.SVal);
        } else {
            SizeSym = GetSizeOfSymbol (Sym);
        }

        /* Remember and skip SVal, terminate ScopeName so it is empty */
        SB_Copy (&Name, &CurTok.SVal);
        NextTok ();
        SB_Terminate (&ScopeName);

    } else {

        /* Parse the scope and the name */
        SymTable* ParentScope = ParseScopedIdent (&Name, &ScopeName);

        /* Check if the parent scope is valid */
        if (ParentScope == 0) {
            /* No such scope */
            SB_Done (&ScopeName);
            SB_Done (&Name);
            return GenLiteral0 ();
        }

        /* If ScopeName is empty, no explicit scope was specified. We have to
        ** search upper scope levels in this case.
        */
        NoScope = SB_IsEmpty (&ScopeName);

        /* First search for a scope with the given name */
        if (NoScope) {
            Scope = SymFindAnyScope (ParentScope, &Name);
        } else {
            Scope = SymFindScope (ParentScope, &Name, SYM_FIND_EXISTING);
        }

        /* If we did find a scope with the name, read the symbol defining the
        ** size, otherwise search for a symbol entry with the name and scope.
        */
        if (Scope) {
            /* Yep, it's a scope */
            SizeSym = GetSizeOfScope (Scope);
        } else {
            if (NoScope) {
                Sym = SymFindAny (ParentScope, &Name);
            } else {
                Sym = SymFind (ParentScope, &Name, SYM_FIND_EXISTING);
            }

            /* If we found the symbol retrieve the size, otherwise complain */
            if (Sym) {
                SizeSym = GetSizeOfSymbol (Sym);
            } else {
                Error ("Unknown symbol or scope: `%m%p%m%p'",
                       &ScopeName, &Name);
            }
        }
    }

    /* Check if we have a size */
    if (SizeSym == 0 || !SymIsConst (SizeSym, &Size)) {
        Error ("Size of `%m%p%m%p' is unknown", &ScopeName, &Name);
        Size = 0;
    }

    /* Free the string buffers */
    SB_Done (&ScopeName);
    SB_Done (&Name);

    /* Return the size */
    return GenLiteralExpr (Size);
}



static ExprNode* FuncStrAt (void)
/* Handle the .STRAT function */
{
    StrBuf Str = STATIC_STRBUF_INITIALIZER;
    long Index;
    unsigned char C = 0;

    /* String constant expected */
    if (CurTok.Tok != TOK_STRCON) {
        Error ("String constant expected");
        NextTok ();
        goto ExitPoint;
    }

    /* Remember the string and skip it */
    SB_Copy (&Str, &CurTok.SVal);
    NextTok ();

    /* Comma must follow */
    ConsumeComma ();

    /* Expression expected */
    Index = ConstExpression ();

    /* Must be a valid index */
    if (Index >= (long) SB_GetLen (&Str)) {
        Error ("Range error");
        goto ExitPoint;
    }

    /* Get the char, handle as unsigned. Be sure to translate it into
    ** the target character set.
    */
    C = TgtTranslateChar (SB_At (&Str, (unsigned)Index));

ExitPoint:
    /* Free string buffer memory */
    SB_Done (&Str);

    /* Return the char expression */
    return GenLiteralExpr (C);
}



static ExprNode* FuncStrLen (void)
/* Handle the .STRLEN function */
{
    int Len;

    /* String constant expected */
    if (CurTok.Tok != TOK_STRCON) {

        Error ("String constant expected");
        /* Smart error recovery */
        if (CurTok.Tok != TOK_RPAREN) {
            NextTok ();
        }
        Len = 0;

    } else {

        /* Get the length of the string */
        Len = SB_GetLen (&CurTok.SVal);

        /* Skip the string */
        NextTok ();
    }

    /* Return the length */
    return GenLiteralExpr (Len);
}



static ExprNode* FuncTCount (void)
/* Handle the .TCOUNT function */
{
    /* We have a list of tokens that ends with the closing paren. Skip
    ** the tokens, and count them. Allow optionally curly braces.
    */
    token_t Term = GetTokListTerm (TOK_RPAREN);
    int Count = 0;
    while (CurTok.Tok != Term) {

        /* Check for end of line or end of input. Since the calling function
        ** will check for the closing paren, we don't need to print an error
        ** here, just bail out.
        */
        if (TokIsSep (CurTok.Tok)) {
            break;
        }

        /* One more token */
        ++Count;

        /* Skip the token */
        NextTok ();
    }

    /* If the list was enclosed in curly braces, skip the closing brace */
    if (Term == TOK_RCURLY && CurTok.Tok == TOK_RCURLY) {
        NextTok ();
    }

    /* Return the number of tokens */
    return GenLiteralExpr (Count);
}



static ExprNode* FuncXMatch (void)
/* Handle the .XMATCH function */
{
    return DoMatch (tcIdentical);
}



static ExprNode* Function (ExprNode* (*F) (void))
/* Handle builtin functions */
{
    ExprNode* E;

    /* Skip the keyword */
    NextTok ();

    /* Expression must be enclosed in braces */
    if (CurTok.Tok != TOK_LPAREN) {
        Error ("'(' expected");
        SkipUntilSep ();
        return GenLiteral0 ();
    }
    NextTok ();

    /* Call the function itself */
    E = F ();

    /* Closing brace must follow */
    ConsumeRParen ();

    /* Return the result of the actual function */
    return E;
}



static ExprNode* Factor (void)
{
    ExprNode* L;
    ExprNode* N;
    long      Val;

    switch (CurTok.Tok) {

        case TOK_INTCON:
            N = GenLiteralExpr (CurTok.IVal);
            NextTok ();
            break;

        case TOK_CHARCON:
            N = GenLiteralExpr (TgtTranslateChar (CurTok.IVal));
            NextTok ();
            break;

        case TOK_NAMESPACE:
        case TOK_IDENT:
        case TOK_LOCAL_IDENT:
            N = Symbol (ParseAnySymName (SYM_ALLOC_NEW));
            break;

        case TOK_ULABEL:
            N = ULabRef (CurTok.IVal);
            NextTok ();
            break;

        case TOK_PLUS:
            NextTok ();
            N = Factor ();
            break;

        case TOK_MINUS:
            NextTok ();
            L = Factor ();
            if (IsEasyConst (L, &Val)) {
                FreeExpr (L);
                N = GenLiteralExpr (-Val);
            } else {
                N = NewExprNode (EXPR_UNARY_MINUS);
                N->Left = L;
            }
            break;

        case TOK_NOT:
            NextTok ();
            L = Factor ();
            if (IsEasyConst (L, &Val)) {
                FreeExpr (L);
                N = GenLiteralExpr (~Val);
            } else {
                N = NewExprNode (EXPR_NOT);
                N->Left = L;
            }
            break;

        case TOK_STAR:
        case TOK_PC:
            NextTok ();
            N = GenCurrentPC ();
            break;

        case TOK_LT:
            NextTok ();
            N = LoByte (Factor ());
            break;

        case TOK_GT:
            NextTok ();
            N = HiByte (Factor ());
            break;

        case TOK_XOR:
            /* ^ means the bank byte of an expression */
            NextTok ();
            N = BankByte (Factor ());
            break;

        case TOK_LPAREN:
            NextTok ();
            N = Expr0 ();
            ConsumeRParen ();
            break;

        case TOK_BANK:
            N = Function (FuncBank);
            break;

        case TOK_BANKBYTE:
            N = Function (FuncBankByte);
            break;

        case TOK_ADDRSIZE:
            N = Function (FuncAddrSize);
            break;

        case TOK_ASIZE:
            if (GetCPU () != CPU_65816) {
                N = GenLiteralExpr (8);
            } else {
                N = GenLiteralExpr (ExtBytes[AM65I_IMM_ACCU] * 8);
            }
            NextTok ();
            break;

        case TOK_BLANK:
            N = Function (FuncBlank);
            break;

        case TOK_CONST:
            N = Function (FuncConst);
            break;

        case TOK_CPU:
            N = GenLiteralExpr (CPUIsets[CPU]);
            NextTok ();
            break;

        case TOK_DEFINED:
            N = Function (FuncDefined);
            break;

        case TOK_DEFINEDMACRO:
            N = Function (FuncDefinedMacro);
            break;

        case TOK_HIBYTE:
            N = Function (FuncHiByte);
            break;

        case TOK_HIWORD:
            N = Function (FuncHiWord);
            break;

        case TOK_ISMNEMONIC:
            N = Function (FuncIsMnemonic);
            break;

        case TOK_ISIZE:
            if (GetCPU () != CPU_65816) {
                N = GenLiteralExpr (8);
            } else {
                N = GenLiteralExpr (ExtBytes[AM65I_IMM_INDEX] * 8);
            }
            NextTok ();
            break;

        case TOK_LOBYTE:
            N = Function (FuncLoByte);
            break;

        case TOK_LOWORD:
            N = Function (FuncLoWord);
            break;

        case TOK_MATCH:
            N = Function (FuncMatch);
            break;

        case TOK_MAX:
            N = Function (FuncMax);
            break;

        case TOK_MIN:
            N = Function (FuncMin);
            break;

        case TOK_REFERENCED:
            N = Function (FuncReferenced);
            break;

        case TOK_SIZEOF:
            N = Function (FuncSizeOf);
            break;

        case TOK_STRAT:
            N = Function (FuncStrAt);
            break;

        case TOK_STRLEN:
            N = Function (FuncStrLen);
            break;

        case TOK_TCOUNT:
            N = Function (FuncTCount);
            break;

        case TOK_TIME:
            N = GenLiteralExpr ((long) time (0));
            NextTok ();
            break;

        case TOK_VERSION:
            N = GenLiteralExpr (GetVersionAsNumber ());
            NextTok ();
            break;

        case TOK_XMATCH:
            N = Function (FuncXMatch);
            break;

        default:
            if (LooseCharTerm && CurTok.Tok == TOK_STRCON &&
                SB_GetLen (&CurTok.SVal) == 1) {
                /* A character constant */
                N = GenLiteralExpr (TgtTranslateChar (SB_At (&CurTok.SVal, 0)));
            } else {
                N = GenLiteral0 ();     /* Dummy */
                Error ("Syntax error");
            }
            NextTok ();
            break;
    }
    return N;
}



static ExprNode* Term (void)
{
    /* Read left hand side */
    ExprNode* Root = Factor ();

    /* Handle multiplicative operations */
    while (CurTok.Tok == TOK_MUL || CurTok.Tok == TOK_DIV ||
           CurTok.Tok == TOK_MOD || CurTok.Tok == TOK_AND ||
           CurTok.Tok == TOK_XOR || CurTok.Tok == TOK_SHL ||
           CurTok.Tok == TOK_SHR) {

        long LVal, RVal, Val;
        ExprNode* Left;
        ExprNode* Right;

        /* Remember the token and skip it */
        token_t T = CurTok.Tok;
        NextTok ();

        /* Move root to left side and read the right side */
        Left  = Root;
        Right = Factor ();

        /* If both expressions are constant, we can evaluate the term */
        if (IsEasyConst (Left, &LVal) && IsEasyConst (Right, &RVal)) {

            switch (T) {
                case TOK_MUL:
                    Val = LVal * RVal;
                    break;

                case TOK_DIV:
                    if (RVal == 0) {
                        Error ("Division by zero");
                        Val = 1;
                    } else {
                        Val = LVal / RVal;
                    }
                    break;

                case TOK_MOD:
                    if (RVal == 0) {
                        Error ("Modulo operation with zero");
                        Val = 1;
                    } else {
                        Val = LVal % RVal;
                    }
                    break;

                case TOK_AND:
                    Val = LVal & RVal;
                    break;

                case TOK_XOR:
                    Val = LVal ^ RVal;
                    break;

                case TOK_SHL:
                    Val = shl_l (LVal, RVal);
                    break;

                case TOK_SHR:
                    Val = shr_l (LVal, RVal);
                    break;

                default:
                    Internal ("Invalid token");
            }

            /* Generate a literal expression and delete the old left and
            ** right sides.
            */
            FreeExpr (Left);
            FreeExpr (Right);
            Root = GenLiteralExpr (Val);

        } else {

            /* Generate an expression tree */
            unsigned char Op;
            switch (T) {
                case TOK_MUL:   Op = EXPR_MUL;  break;
                case TOK_DIV:   Op = EXPR_DIV;  break;
                case TOK_MOD:   Op = EXPR_MOD;  break;
                case TOK_AND:   Op = EXPR_AND;  break;
                case TOK_XOR:   Op = EXPR_XOR;  break;
                case TOK_SHL:   Op = EXPR_SHL;  break;
                case TOK_SHR:   Op = EXPR_SHR;  break;
                default:        Internal ("Invalid token");
            }
            Root        = NewExprNode (Op);
            Root->Left  = Left;
            Root->Right = Right;

        }

    }

    /* Return the expression tree we've created */
    return Root;
}



static ExprNode* SimpleExpr (void)
{
    /* Read left hand side */
    ExprNode* Root = Term ();

    /* Handle additive operations */
    while (CurTok.Tok == TOK_PLUS  ||
           CurTok.Tok == TOK_MINUS ||
           CurTok.Tok == TOK_OR) {

        long LVal, RVal, Val;
        ExprNode* Left;
        ExprNode* Right;

        /* Remember the token and skip it */
        token_t T = CurTok.Tok;
        NextTok ();

        /* Move root to left side and read the right side */
        Left  = Root;
        Right = Term ();

        /* If both expressions are constant, we can evaluate the term */
        if (IsEasyConst (Left, &LVal) && IsEasyConst (Right, &RVal)) {

            switch (T) {
                case TOK_PLUS:  Val = LVal + RVal;      break;
                case TOK_MINUS: Val = LVal - RVal;      break;
                case TOK_OR:    Val = LVal | RVal;      break;
                default:        Internal ("Invalid token");
            }

            /* Generate a literal expression and delete the old left and
            ** right sides.
            */
            FreeExpr (Left);
            FreeExpr (Right);
            Root = GenLiteralExpr (Val);

        } else {

            /* Generate an expression tree */
            unsigned char Op;
            switch (T) {
                case TOK_PLUS:  Op = EXPR_PLUS;  break;
                case TOK_MINUS: Op = EXPR_MINUS; break;
                case TOK_OR:    Op = EXPR_OR;    break;
                default:        Internal ("Invalid token");
            }
            Root        = NewExprNode (Op);
            Root->Left  = Left;
            Root->Right = Right;

        }
    }

    /* Return the expression tree we've created */
    return Root;
}



static ExprNode* BoolExpr (void)
/* Evaluate a boolean expression */
{
    /* Read left hand side */
    ExprNode* Root = SimpleExpr ();

    /* Handle booleans */
    while (CurTok.Tok == TOK_EQ || CurTok.Tok == TOK_NE ||
           CurTok.Tok == TOK_LT || CurTok.Tok == TOK_GT ||
           CurTok.Tok == TOK_LE || CurTok.Tok == TOK_GE) {

        long LVal, RVal, Val;
        ExprNode* Left;
        ExprNode* Right;

        /* Remember the token and skip it */
        token_t T = CurTok.Tok;
        NextTok ();

        /* Move root to left side and read the right side */
        Left  = Root;
        Right = SimpleExpr ();

        /* If both expressions are constant, we can evaluate the term */
        if (IsEasyConst (Left, &LVal) && IsEasyConst (Right, &RVal)) {

            switch (T) {
                case TOK_EQ:    Val = (LVal == RVal);   break;
                case TOK_NE:    Val = (LVal != RVal);   break;
                case TOK_LT:    Val = (LVal < RVal);    break;
                case TOK_GT:    Val = (LVal > RVal);    break;
                case TOK_LE:    Val = (LVal <= RVal);   break;
                case TOK_GE:    Val = (LVal >= RVal);   break;
                default:        Internal ("Invalid token");
            }

            /* Generate a literal expression and delete the old left and
            ** right sides.
            */
            FreeExpr (Left);
            FreeExpr (Right);
            Root = GenLiteralExpr (Val);

        } else {

            /* Generate an expression tree */
            unsigned char Op;
            switch (T) {
                case TOK_EQ:    Op = EXPR_EQ;   break;
                case TOK_NE:    Op = EXPR_NE;   break;
                case TOK_LT:    Op = EXPR_LT;   break;
                case TOK_GT:    Op = EXPR_GT;   break;
                case TOK_LE:    Op = EXPR_LE;   break;
                case TOK_GE:    Op = EXPR_GE;   break;
                default:        Internal ("Invalid token");
            }
            Root        = NewExprNode (Op);
            Root->Left  = Left;
            Root->Right = Right;

        }
    }

    /* Return the expression tree we've created */
    return Root;
}



static ExprNode* Expr2 (void)
/* Boolean operators: AND and XOR */
{
    /* Read left hand side */
    ExprNode* Root = BoolExpr ();

    /* Handle booleans */
    while (CurTok.Tok == TOK_BOOLAND || CurTok.Tok == TOK_BOOLXOR) {

        long LVal, RVal, Val;
        ExprNode* Left;
        ExprNode* Right;

        /* Remember the token and skip it */
        token_t T = CurTok.Tok;
        NextTok ();

        /* Move root to left side and read the right side */
        Left  = Root;
        Right = BoolExpr ();

        /* If both expressions are constant, we can evaluate the term */
        if (IsEasyConst (Left, &LVal) && IsEasyConst (Right, &RVal)) {

            switch (T) {
                case TOK_BOOLAND:   Val = ((LVal != 0) && (RVal != 0)); break;
                case TOK_BOOLXOR:   Val = ((LVal != 0) ^  (RVal != 0)); break;
                default:        Internal ("Invalid token");
            }

            /* Generate a literal expression and delete the old left and
            ** right sides.
            */
            FreeExpr (Left);
            FreeExpr (Right);
            Root = GenLiteralExpr (Val);

        } else {

            /* Generate an expression tree */
            unsigned char Op;
            switch (T) {
                case TOK_BOOLAND:   Op = EXPR_BOOLAND; break;
                case TOK_BOOLXOR:   Op = EXPR_BOOLXOR; break;
                default:            Internal ("Invalid token");
            }
            Root        = NewExprNode (Op);
            Root->Left  = Left;
            Root->Right = Right;

        }
    }

    /* Return the expression tree we've created */
    return Root;
}



static ExprNode* Expr1 (void)
/* Boolean operators: OR */
{
    /* Read left hand side */
    ExprNode* Root = Expr2 ();

    /* Handle booleans */
    while (CurTok.Tok == TOK_BOOLOR) {

        long LVal, RVal, Val;
        ExprNode* Left;
        ExprNode* Right;

        /* Remember the token and skip it */
        token_t T = CurTok.Tok;
        NextTok ();

        /* Move root to left side and read the right side */
        Left  = Root;
        Right = Expr2 ();

        /* If both expressions are constant, we can evaluate the term */
        if (IsEasyConst (Left, &LVal) && IsEasyConst (Right, &RVal)) {

            switch (T) {
                case TOK_BOOLOR:    Val = ((LVal != 0) || (RVal != 0)); break;
                default:        Internal ("Invalid token");
            }

            /* Generate a literal expression and delete the old left and
            ** right sides.
            */
            FreeExpr (Left);
            FreeExpr (Right);
            Root = GenLiteralExpr (Val);

        } else {

            /* Generate an expression tree */
            unsigned char Op;
            switch (T) {
                case TOK_BOOLOR:    Op = EXPR_BOOLOR;  break;
                default:            Internal ("Invalid token");
            }
            Root        = NewExprNode (Op);
            Root->Left  = Left;
            Root->Right = Right;

        }
    }

    /* Return the expression tree we've created */
    return Root;
}



static ExprNode* Expr0 (void)
/* Boolean operators: NOT */
{
    ExprNode* Root;

    /* Handle booleans */
    if (CurTok.Tok == TOK_BOOLNOT) {

        long Val;
        ExprNode* Left;

        /* Skip the operator token */
        NextTok ();

        /* Read the argument */
        Left = Expr0 ();

        /* If the argument is const, evaluate it directly */
        if (IsEasyConst (Left, &Val)) {
            FreeExpr (Left);
            Root = GenLiteralExpr (!Val);
        } else {
            Root = NewExprNode (EXPR_BOOLNOT);
            Root->Left = Left;
        }

    } else {

        /* Read left hand side */
        Root = Expr1 ();

    }

    /* Return the expression tree we've created */
    return Root;
}



ExprNode* Expression (void)
/* Evaluate an expression, build the expression tree on the heap and return
** a pointer to the root of the tree.
*/
{
    return Expr0 ();
}



long ConstExpression (void)
/* Parse an expression. Check if the expression is const, and print an error
** message if not. Return the value of the expression, or a dummy, if it is
** not constant.
*/
{
    long Val;

    /* Read the expression */
    ExprNode* Expr = Expression ();

    /* Study the expression */
    ExprDesc D;
    ED_Init (&D);
    StudyExpr (Expr, &D);

    /* Check if the expression is constant */
    if (ED_IsConst (&D)) {
        Val = D.Val;
    } else {
        Error ("Constant expression expected");
        Val = 0;
    }

    /* Free the expression tree and allocated memory for D */
    FreeExpr (Expr);
    ED_Done (&D);

    /* Return the value */
    return Val;
}



void FreeExpr (ExprNode* Root)
/* Free the expression, Root is pointing to. */
{
    if (Root) {
        FreeExpr (Root->Left);
        FreeExpr (Root->Right);
        FreeExprNode (Root);
    }
}



ExprNode* SimplifyExpr (ExprNode* Expr, const ExprDesc* D)
/* Try to simplify the given expression tree */
{
    if (Expr->Op != EXPR_LITERAL && ED_IsConst (D)) {
        /* No external references */
        FreeExpr (Expr);
        Expr = GenLiteralExpr (D->Val);
    }
    return Expr;
}



ExprNode* GenLiteralExpr (long Val)
/* Return an expression tree that encodes the given literal value */
{
    ExprNode* Expr = NewExprNode (EXPR_LITERAL);
    Expr->V.IVal = Val;
    return Expr;
}



ExprNode* GenLiteral0 (void)
/* Return an expression tree that encodes the the number zero */
{
    return GenLiteralExpr (0);
}



ExprNode* GenSymExpr (SymEntry* Sym)
/* Return an expression node that encodes the given symbol */
{
    ExprNode* Expr = NewExprNode (EXPR_SYMBOL);
    Expr->V.Sym = Sym;
    SymAddExprRef (Sym, Expr);
    return Expr;
}



static ExprNode* GenSectionExpr (unsigned SecNum)
/* Return an expression node for the given section */
{
    ExprNode* Expr = NewExprNode (EXPR_SECTION);
    Expr->V.SecNum = SecNum;
    return Expr;
}



static ExprNode* GenBankExpr (unsigned SecNum)
/* Return an expression node for the given bank */
{
    ExprNode* Expr = NewExprNode (EXPR_BANK);
    Expr->V.SecNum = SecNum;
    return Expr;
}



ExprNode* GenAddExpr (ExprNode* Left, ExprNode* Right)
/* Generate an addition from the two operands */
{
    long Val;
    if (IsEasyConst (Left, &Val) && Val == 0) {
        FreeExpr (Left);
        return Right;
    } else if (IsEasyConst (Right, &Val) && Val == 0) {
        FreeExpr (Right);
        return Left;
    } else {
        ExprNode* Root = NewExprNode (EXPR_PLUS);
        Root->Left = Left;
        Root->Right = Right;
        return Root;
    }
}



ExprNode* GenCurrentPC (void)
/* Return the current program counter as expression */
{
    ExprNode* Root;

    if (GetRelocMode ()) {
        /* Create SegmentBase + Offset */
        Root = GenAddExpr (GenSectionExpr (GetCurrentSegNum ()),
                           GenLiteralExpr (GetPC ()));
    } else {
        /* Absolute mode, just return PC value */
        Root = GenLiteralExpr (GetPC ());
    }

    return Root;
}



ExprNode* GenSwapExpr (ExprNode* Expr)
/* Return an extended expression with lo and hi bytes swapped */
{
    ExprNode* N = NewExprNode (EXPR_SWAP);
    N->Left = Expr;
    return N;
}



ExprNode* GenBranchExpr (unsigned Offs)
/* Return an expression that encodes the difference between current PC plus
** offset and the target expression (that is, Expression() - (*+Offs) ).
*/
{
    ExprNode* N;
    ExprNode* Root;
    long      Val;

    /* Read Expression() */
    N = Expression ();

    /* If the expression is a cheap constant, generate a simpler tree */
    if (IsEasyConst (N, &Val)) {

        /* Free the constant expression tree */
        FreeExpr (N);

        /* Generate the final expression:
        ** Val - (* + Offs)
        ** Val - ((Seg + PC) + Offs)
        ** Val - Seg - PC - Offs
        ** (Val - PC - Offs) - Seg
        */
        Root = GenLiteralExpr (Val - GetPC () - Offs);
        if (GetRelocMode ()) {
            N = Root;
            Root = NewExprNode (EXPR_MINUS);
            Root->Left  = N;
            Root->Right = GenSectionExpr (GetCurrentSegNum ());
        }

    } else {

        /* Generate the expression:
        ** N - (* + Offs)
        ** N - ((Seg + PC) + Offs)
        ** N - Seg - PC - Offs
        ** N - (PC + Offs) - Seg
        */
        Root = NewExprNode (EXPR_MINUS);
        Root->Left  = N;
        Root->Right = GenLiteralExpr (GetPC () + Offs);
        if (GetRelocMode ()) {
            N = Root;
            Root = NewExprNode (EXPR_MINUS);
            Root->Left  = N;
            Root->Right = GenSectionExpr (GetCurrentSegNum ());
        }
    }

    /* Return the result */
    return Root;
}



ExprNode* GenULabelExpr (unsigned Num)
/* Return an expression for an unnamed label with the given index */
{
    ExprNode* Node = NewExprNode (EXPR_ULABEL);
    Node->V.IVal        = Num;

    /* Return the new node */
    return Node;
}



ExprNode* GenByteExpr (ExprNode* Expr)
/* Force the given expression into a byte and return the result */
{
    /* Use the low byte operator to force the expression into byte size */
    return LoByte (Expr);
}



ExprNode* GenWordExpr (ExprNode* Expr)
/* Force the given expression into a word and return the result. */
{
    /* Use the low byte operator to force the expression into word size */
    return LoWord (Expr);
}



ExprNode* GenFarAddrExpr (ExprNode* Expr)
/* Force the given expression into a far address and return the result. */
{
    long      Val;

    /* Special handling for const expressions */
    if (IsEasyConst (Expr, &Val)) {
        FreeExpr (Expr);
        Expr = GenLiteralExpr (Val & 0xFFFFFF);
    } else {
        ExprNode* Operand = Expr;
        Expr = NewExprNode (EXPR_FARADDR);
        Expr->Left = Operand;
    }
    return Expr;
}



ExprNode* GenDWordExpr (ExprNode* Expr)
/* Force the given expression into a dword and return the result. */
{
    long      Val;

    /* Special handling for const expressions */
    if (IsEasyConst (Expr, &Val)) {
        FreeExpr (Expr);
        Expr = GenLiteralExpr (Val & 0xFFFFFFFF);
    } else {
        ExprNode* Operand = Expr;
        Expr = NewExprNode (EXPR_DWORD);
        Expr->Left = Operand;
    }
    return Expr;
}



ExprNode* GenNE (ExprNode* Expr, long Val)
/* Generate an expression that compares Expr and Val for inequality */
{
    /* Generate a compare node */
    ExprNode* Root = NewExprNode (EXPR_NE);
    Root->Left  = Expr;
    Root->Right = GenLiteralExpr (Val);

    /* Return the result */
    return Root;
}



int IsConstExpr (ExprNode* Expr, long* Val)
/* Return true if the given expression is a constant expression, that is, one
** with no references to external symbols. If Val is not NULL and the
** expression is constant, the constant value is stored here.
*/
{
    int IsConst;

    /* Study the expression */
    ExprDesc D;
    ED_Init (&D);
    StudyExpr (Expr, &D);

    /* Check if the expression is constant */
    IsConst = ED_IsConst (&D);
    if (IsConst && Val != 0) {
        *Val = D.Val;
    }

    /* Delete allocated memory and return the result */
    ED_Done (&D);
    return IsConst;
}



ExprNode* CloneExpr (ExprNode* Expr)
/* Clone the given expression tree. The function will simply clone symbol
** nodes, it will not resolve them.
*/
{
    ExprNode* Clone;

    /* Accept NULL pointers */
    if (Expr == 0) {
        return 0;
    }

    /* Clone the node */
    switch (Expr->Op) {

        case EXPR_LITERAL:
            Clone = GenLiteralExpr (Expr->V.IVal);
            break;

        case EXPR_ULABEL:
            Clone = GenULabelExpr (Expr->V.IVal);
            break;

        case EXPR_SYMBOL:
            Clone = GenSymExpr (Expr->V.Sym);
            break;

        case EXPR_SECTION:
            Clone = GenSectionExpr (Expr->V.SecNum);
            break;

        case EXPR_BANK:
            Clone = GenBankExpr (Expr->V.SecNum);
            break;

        default:
            /* Generate a new node */
            Clone = NewExprNode (Expr->Op);
            /* Clone the tree nodes */
            Clone->Left = CloneExpr (Expr->Left);
            Clone->Right = CloneExpr (Expr->Right);
            break;
    }

    /* Done */
    return Clone;
}



void WriteExpr (ExprNode* Expr)
/* Write the given expression to the object file */
{
    /* Null expressions are encoded by a type byte of zero */
    if (Expr == 0) {
        ObjWrite8 (EXPR_NULL);
        return;
    }

    /* If the is a leafnode, write the expression attribute, otherwise
    ** write the expression operands.
    */
    switch (Expr->Op) {

        case EXPR_LITERAL:
            ObjWrite8 (EXPR_LITERAL);
            ObjWrite32 (Expr->V.IVal);
            break;

        case EXPR_SYMBOL:
            if (SymIsImport (Expr->V.Sym)) {
                ObjWrite8 (EXPR_SYMBOL);
                ObjWriteVar (GetSymImportId (Expr->V.Sym));
            } else {
                WriteExpr (GetSymExpr (Expr->V.Sym));
            }
            break;

        case EXPR_SECTION:
            ObjWrite8 (EXPR_SECTION);
            ObjWriteVar (Expr->V.SecNum);
            break;

        case EXPR_ULABEL:
            WriteExpr (ULabResolve (Expr->V.IVal));
            break;

        default:
            /* Not a leaf node */
            ObjWrite8 (Expr->Op);
            WriteExpr (Expr->Left);
            WriteExpr (Expr->Right);
            break;

    }
}



void ExprGuessedAddrSize (const ExprNode* Expr, unsigned char AddrSize)
/* Mark the address size of the given expression tree as guessed. The address
** size passed as argument is the one NOT used, because the actual address
** size wasn't known. Example: Zero page addressing was not used because symbol
** is undefined, and absolute addressing was available.
** This function will actually parse the expression tree for undefined symbols,
** and mark these symbols accordingly.
*/
{
    /* Accept NULL expressions */
    if (Expr == 0) {
        return;
    }

    /* Check the type code */
    switch (EXPR_NODETYPE (Expr->Op)) {

        case EXPR_LEAFNODE:
            if (Expr->Op == EXPR_SYMBOL) {
                if (!SymIsDef (Expr->V.Sym)) {
                    /* Symbol is undefined, mark it */
                    SymGuessedAddrSize (Expr->V.Sym, AddrSize);
                }
            }
            return;

        case EXPR_BINARYNODE:
            ExprGuessedAddrSize (Expr->Right, AddrSize);
            /* FALLTHROUGH */

        case EXPR_UNARYNODE:
            ExprGuessedAddrSize (Expr->Left, AddrSize);
            break;
    }
}



ExprNode* MakeBoundedExpr (ExprNode* Expr, unsigned Size)
/* Force the given expression into a specific size of ForceRange is true */
{
    if (ForceRange) {
        switch (Size) {
            case 1:     Expr = GenByteExpr (Expr);      break;
            case 2:     Expr = GenWordExpr (Expr);      break;
            case 3:     Expr = GenFarAddrExpr (Expr);   break;
            case 4:     Expr = GenDWordExpr (Expr);     break;
            default:    Internal ("Invalid size in BoundedExpr: %u", Size);
        }
    }
    return Expr;
}



ExprNode* BoundedExpr (ExprNode* (*ExprFunc) (void), unsigned Size)
/* Parse an expression and force it within a given size if ForceRange is true */
{
    return MakeBoundedExpr (ExprFunc (), Size);
}
