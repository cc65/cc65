/*****************************************************************************/
/*                                                                           */
/*                                 spawn-amiga.c                             */
/*                                                                           */
/*                Execute other external programs (Amiga version)            */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2002     Wolfgang Hosemann                                            */
/* EMail:       whose@t-online.de                                            */
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



#include <stdio.h>
#include <clib/dos_protos.h>

/* common */
#include "attrib.h"
#include "strbuf.h"



/*****************************************************************************/
/*  		    		     Code 				     */
/*****************************************************************************/



int spawnvp (int Mode attribute ((unused)),
             const char* File attribute ((unused)),
             char* const argv [])
/* Execute the given program searching and wait til it terminates. The Mode
 * argument is ignored (compatibility only). The result of the function is
 * the return code of the program. The function will terminate the program
 * on errors.
 */
{
    int Status;
    StrBuf Command = AUTO_STRBUF_INITIALIZER;

    /* Build the command line */
    while (*argv) {
        SB_AppendStr (&Command, *argv++);
        SB_AppendChar (&Command, ' ');
    }

    /* Terminate the command line */
    SB_Terminate (&Command);

    /* Invoke the shell to execute the command */
    Status = System (SB->GetConstBuf (&Command), TAG_END)

    /* Free the string buf data */
    DoneStrBuf (&Command);

    /* Return the result */
    return Status;
}



