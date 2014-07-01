/*****************************************************************************/
/*                                                                           */
/*                                  token.c                                  */
/*                                                                           */
/*                  Token list for the ca65 macro assembler                  */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2007-2011, Ullrich von Bassewitz                                      */
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



/* ca65 */
#include "token.h"



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



int TokHasSVal (token_t Tok)
/* Return true if the given token has an attached SVal */
{
    return (Tok == TOK_IDENT || Tok == TOK_LOCAL_IDENT || Tok == TOK_STRCON);
}



int TokHasIVal (token_t Tok)
/* Return true if the given token has an attached IVal */
{
    return (Tok == TOK_INTCON || Tok == TOK_CHARCON || Tok == TOK_REG);
}



void CopyToken (Token* Dst, const Token* Src)
/* Copy a token from Src to Dst. The current value of Dst.SVal is free'd,
** so Dst must be initialized.
*/
{
    /* Copy the fields */
    Dst->Tok  = Src->Tok;
    Dst->WS   = Src->WS;
    Dst->IVal = Src->IVal;
    SB_Copy (&Dst->SVal, &Src->SVal);
    Dst->Pos  = Src->Pos;
}
