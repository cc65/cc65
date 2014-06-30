/*****************************************************************************/
/*                                                                           */
/*                                 reginfo.c                                 */
/*                                                                           */
/*                        6502 register tracking info                        */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2001-2003 Ullrich von Bassewitz                                       */
/*               Roemerstrasse 52                                            */
/*               D-70794 Filderstadt                                         */
/* EMail:        uz@cc65.org                                                 */
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



/* common */
#include "xmalloc.h"

/* cc65 */
#include "reginfo.h"



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



void RC_Invalidate (RegContents* C)
/* Invalidate all registers */
{
    C->RegA   = UNKNOWN_REGVAL;
    C->RegX   = UNKNOWN_REGVAL;
    C->RegY   = UNKNOWN_REGVAL;
    C->SRegLo = UNKNOWN_REGVAL;
    C->SRegHi = UNKNOWN_REGVAL;
    C->Ptr1Lo = UNKNOWN_REGVAL;
    C->Ptr1Hi = UNKNOWN_REGVAL;
    C->Tmp1   = UNKNOWN_REGVAL;
}



void RC_InvalidateZP (RegContents* C)
/* Invalidate all ZP registers */
{
    C->SRegLo = UNKNOWN_REGVAL;
    C->SRegHi = UNKNOWN_REGVAL;
    C->Ptr1Lo = UNKNOWN_REGVAL;
    C->Ptr1Hi = UNKNOWN_REGVAL;
    C->Tmp1   = UNKNOWN_REGVAL;
}



static void RC_Dump1 (FILE* F, const char* Desc, short Val)
/* Dump one register value */
{
    if (RegValIsKnown (Val)) {
        fprintf (F, "%s=$%02X ", Desc, Val);
    } else {
        fprintf (F, "%s=$XX ", Desc);
    }
}



void RC_Dump (FILE* F, const RegContents* RC)
/* Dump the contents of the given RegContents struct */
{
    RC_Dump1 (F, "A", RC->RegA);
    RC_Dump1 (F, "X", RC->RegX);
    RC_Dump1 (F, "Y", RC->RegY);
    RC_Dump1 (F, "SREG", RC->SRegLo);
    RC_Dump1 (F, "SREG+1", RC->SRegHi);
    RC_Dump1 (F, "PTR1", RC->Ptr1Lo);
    RC_Dump1 (F, "PTR1+1", RC->Ptr1Hi);
    RC_Dump1 (F, "TMP1", RC->Tmp1);
    fprintf (F, "\n");
}



RegInfo* NewRegInfo (const RegContents* RC)
/* Allocate a new register info, initialize and return it. If RC is not
** a NULL pointer, it is used to initialize both, the input and output
** registers. If the pointer is NULL, all registers are set to unknown.
*/
{
    /* Allocate memory */
    RegInfo* RI = xmalloc (sizeof (RegInfo));

    /* Initialize the registers */
    if (RC) {
        RI->In   = *RC;
        RI->Out  = *RC;
        RI->Out2 = *RC;
    } else {
        RC_Invalidate (&RI->In);
        RC_Invalidate (&RI->Out);
        RC_Invalidate (&RI->Out2);
    }

    /* Return the new struct */
    return RI;
}



void FreeRegInfo (RegInfo* RI)
/* Free a RegInfo struct */
{
    xfree (RI);
}



void DumpRegInfo (const char* Desc, const RegInfo* RI)
/* Dump the register info for debugging */
{
    fprintf (stdout, "%s:\n", Desc);
    fprintf (stdout, "In:  ");
    RC_Dump (stdout, &RI->In);
    fprintf (stdout, "Out: ");
    RC_Dump (stdout, &RI->Out);
}
