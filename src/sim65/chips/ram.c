/*****************************************************************************/
/*                                                                           */
/*				     ram.c				     */
/*                                                                           */
/*		    RAM plugin for the sim65 6502 simulator		     */
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



#include <stdlib.h>
#include <string.h>

/* sim65 */
#include "chipif.h"



/*****************************************************************************/
/*                                   Forwards                                */
/*****************************************************************************/



int InitChip (const struct SimData* Data);
/* Initialize the chip, return an error code */

static void* InitInstance (unsigned Addr, unsigned Range);
/* Initialize a new chip instance */

static void WriteCtrl (void* Data, unsigned Offs, unsigned char Val);
/* Write control data */

static void Write (void* Data, unsigned Offs, unsigned char Val);
/* Write user data */

static unsigned char ReadCtrl (void* Data, unsigned Offs);
/* Read control data */

static unsigned char Read (void* Data, unsigned Offs);
/* Read user data */



/*****************************************************************************/
/*                                     Data                                  */
/*****************************************************************************/



/* Control data passed to the main program */
static const struct ChipData RAMData[1] = {
    {
        "RAM",                  /* Name of the chip */
        CHIPDATA_VER_MAJOR,     /* Version information */
        CHIPDATA_VER_MINOR,

        /* -- Exported functions -- */
        InitChip,
        InitInstance,
        WriteCtrl,
        Write,
        ReadCtrl,
        Read
    }
};

/* The SimData pointer we get when InitChip is called */
static const SimData* Sim;

/* Possible RAM attributes */
#define ATTR_INITIALIZED        0x01    /* RAM cell is intialized */
#define ATTR_WPROT		0x02	/* RAM cell is write protected */

/* Data for one RAM instance */
typedef struct InstanceData InstanceData;
struct InstanceData {
    unsigned            BaseAddr;       /* Base address */
    unsigned            Range;          /* Memory range */
    unsigned char*      MemAttr;        /* Memory attributes */
    unsigned char*      Mem;            /* The memory itself */
};



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



static void* InitInstance (unsigned Addr, unsigned Range)
/* Initialize a new chip instance */
{
    /* Allocate a new instance structure */
    InstanceData* D = Sim->Malloc (sizeof (InstanceData));

    /* Initialize the structure, allocate RAM and attribute memory */
    D->BaseAddr = Addr;
    D->Range    = Range;
    D->MemAttr  = Sim->Malloc (Range * sizeof (D->MemAttr[0]));
    D->Mem      = Sim->Malloc (Range * sizeof (D->Mem[0]));

    /* Clear the RAM and attribute memory */
    memset (D->MemAttr, 0, Range * sizeof (D->MemAttr[0]));
    memset (D->Mem, 0, Range * sizeof (D->Mem[0]));

    /* Done, return the instance data */
    return D;
}



static void WriteCtrl (void* Data, unsigned Offs, unsigned char Val)
/* Write control data */
{
    /* Cast the data pointer */
    InstanceData* D = (InstanceData*) Data;

    /* Do the write and remember the cell as initialized */
    D->Mem[Offs] = Val;
    D->MemAttr[Offs] |= ATTR_INITIALIZED;
}



static void Write (void* Data, unsigned Offs, unsigned char Val)
/* Write user data */
{
    /* Cast the data pointer */
    InstanceData* D = (InstanceData*) Data;

    /* Check for a write to a write protected cell */
    if (D->MemAttr[Offs] & ATTR_WPROT) {
        Sim->Warning ("Writing to write protected memory at $%04X", D->BaseAddr+Offs);
    }

    /* Do the write and remember the cell as initialized */
    D->Mem[Offs] = Val;
    D->MemAttr[Offs] |= ATTR_INITIALIZED;
}



static unsigned char ReadCtrl (void* Data, unsigned Offs)
/* Read control data */
{
    /* Cast the data pointer */
    InstanceData* D = (InstanceData*) Data;

    /* Read the cell and return the value */
    return D->Mem[Offs];
}



static unsigned char Read (void* Data, unsigned Offs)
/* Read user data */
{
    /* Cast the data pointer */
    InstanceData* D = (InstanceData*) Data;

    /* Check for a read from an uninitialized cell */
    if ((D->MemAttr[Offs] & ATTR_INITIALIZED) == 0) {
        /* We're reading a memory cell that was never written to */
        Sim->Warning ("Reading from uninitialized memory at $%04X", D->BaseAddr+Offs);
    }

    /* Read the cell and return the value */
    return D->Mem[Offs];
}




