/*****************************************************************************/
/*                                                                           */
/*				    stdio.c				     */
/*                                                                           */
/*		   STDIO plugin for the sim65 6502 simulator		     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2002      Ullrich von Bassewitz                                       */
/*               Wacholderweg 14                                             */
/*               D-70597 Stuttgart                                           */
/* EMail:        uz@musoftware.de                                            */
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
#include <stdlib.h>
#include <string.h>

/* common */
#include "attrib.h"

/* sim65 */
#include "chipif.h"



/*****************************************************************************/
/*                                   Forwards                                */
/*****************************************************************************/



int InitChip (const struct SimData* Data);
/* Initialize the chip, return an error code */

static void* InitInstance (unsigned Addr, unsigned Range);
/* Initialize a new chip instance */

static void Write (void* Data, unsigned Offs, unsigned char Val);
/* Write user data */

static unsigned char Read (void* Data, unsigned Offs);
/* Read user data */



/*****************************************************************************/
/*                                     Data                                  */
/*****************************************************************************/



/* Control data passed to the main program */
static const struct ChipData RAMData[1] = {
    {
        "STDIO",                /* Name of the chip */
        CHIPDATA_VER_MAJOR,     /* Version information */
        CHIPDATA_VER_MINOR,

        /* -- Exported functions -- */
        InitChip,
        InitInstance,
        Write,
        Write,
        Read,
        Read
    }
};

/* The SimData pointer we get when InitChip is called */
static const SimData* Sim;



/*****************************************************************************/
/*                               Exported function                           */
/*****************************************************************************/



int GetChipData (const ChipData** Data, unsigned* Count)
{
    /* Pass the control structure to the caller */
    *Data = RAMData;
    *Count = sizeof (Data) / sizeof (Data[0]);

    /* Call was successful */
    return 0;
}



/*****************************************************************************/
/*                                     Code                                  */
/*****************************************************************************/



int InitChip (const struct SimData* Data)
/* Initialize the chip, return an error code */
{
    /* Remember the pointer */
    Sim = Data;

    /* Always successful */
    return 0;
}



static void* InitInstance (unsigned Addr attribute ((unused)),
			   unsigned Range attribute ((unused)))
/* Initialize a new chip instance */
{
    /* We don't need any instance data */
    return 0;
}



static void Write (void* Data attribute ((unused)),
		   unsigned Offs attribute ((unused)),
		   unsigned char Val)
/* Write user data */
{
    putchar (Val);
}



static unsigned char Read (void* Data attribute ((unused)),
			   unsigned Offs attribute ((unused)))
/* Read user data */
{
    /* Read a character and return the value */
    return getchar ();
}



					 
