/*****************************************************************************/
/*                                                                           */
/*                                  goto.c                                   */
/*                                                                           */
/*              Goto and label handling for the cc65 C compiler              */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000     Ullrich von Bassewitz                                        */
/*              Wacholderweg 14                                              */
/*              D-70597 Stuttgart                                            */
/* EMail:       uz@musoftware.de                                             */
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



#include "codegen.h"
#include "error.h"
#include "scanner.h"
#include "symtab.h"
#include "goto.h"



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



void GotoStatement (void)
/* Process a goto statement. */
{
    /* Eat the "goto" */
    NextToken ();

    /* Label name must follow */
    if (CurTok.Tok != TOK_IDENT) {

        Error ("Label name expected");

    } else {

        /* Add a new label symbol if we don't have one until now */
        SymEntry* Entry = AddLabelSym (CurTok.Ident, SC_REF);

        /* Jump to the label */
        g_jump (Entry->V.Label);
    }

    /* Eat the label name */
    NextToken ();
}



void DoLabel (void)
/* Define a label. */
{
    /* Add a label symbol */
    SymEntry* Entry = AddLabelSym (CurTok.Ident, SC_DEF);

    /* Emit the jump label */
    g_defcodelabel (Entry->V.Label);

    /* Eat the ident and colon */
    NextToken ();
    NextToken ();
}
