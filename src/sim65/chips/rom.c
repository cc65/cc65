/*****************************************************************************/
/*                                                                           */
/*				     rom.c				     */
/*                                                                           */
/*		    ROM plugin for the sim65 6502 simulator		     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2003      Ullrich von Bassewitz                                       */
/*               Römerstrasse 52                                             */
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



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

/* sim65 */
#include "chipif.h"



/*****************************************************************************/
/*                                   Forwards                                */
/*****************************************************************************/



static int InitChip (const struct SimData* Data);
/* Initialize the chip, return an error code */

static void* InitInstance (unsigned Addr, unsigned Range, void* CfgInfo);
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
static const struct ChipData ROMData[1] = {
    {
        "ROM",                  /* Name of the chip */
        CHIPDATA_TYPE_CHIP,     /* Type of the chip */
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

/* Data for one ROM instance */
typedef struct InstanceData InstanceData;
struct InstanceData {
    unsigned            BaseAddr;       /* Base address */
    unsigned            Range;          /* Memory range */
    unsigned char*      Mem;            /* The memory itself */
};



/*****************************************************************************/
/*                               Exported function                           */
/*****************************************************************************/



int GetChipData (const ChipData** Data, unsigned* Count)
{
    /* Pass the control structure to the caller */
    *Data = ROMData;
    *Count = sizeof (Data) / sizeof (Data[0]);

    /* Call was successful */
    return 0;
}



/*****************************************************************************/
/*                                     Code                                  */
/*****************************************************************************/



static int InitChip (const struct SimData* Data)
/* Initialize the chip, return an error code */
{
    /* Remember the pointer */
    Sim = Data;

    /* Always successful */
    return 0;
}



static void* InitInstance (unsigned Addr, unsigned Range, void* CfgInfo)
/* Initialize a new chip instance */
{
    char* Name;
    FILE* F;

    /* Allocate a new instance structure */
    InstanceData* D = Sim->Malloc (sizeof (InstanceData));

    /* Initialize the structure, allocate RAM and attribute memory */
    D->BaseAddr = Addr;
    D->Range    = Range;
    D->Mem      = Sim->Malloc (Range);

    /* We must have a "file" attribute. Get it. */
    if (Sim->GetCfgStr (CfgInfo, "file", &Name) == 0) {
        /* Attribute not found */
        Sim->Error ("Attribute `file' missing");        /* ### */
    }

    /* Open the file with the given name */
    F = fopen (Name, "rb");
    if (F == 0) {
        Sim->Error ("Cannot open `%s': %s", Name, strerror (errno));
    }

    /* Read the file into the memory */
    if (fread (D->Mem, 1, D->Range, F) != D->Range) {
        Sim->Warning ("Cannot read %u bytes from file `%s'", D->Range, Name);
    }

    /* Close the file */
    fclose (F);

    /* Free the file name */
    Sim->Free (Name);

    /* Done, return the instance data */
    return D;
}



static void WriteCtrl (void* Data, unsigned Offs, unsigned char Val)
/* Write control data */
{
    /* Cast the data pointer */
    InstanceData* D = (InstanceData*) Data;

    /* Do the write */
    D->Mem[Offs] = Val;
}



static void Write (void* Data, unsigned Offs, unsigned char Val)
/* Write user data */
{
    /* Cast the data pointer */
    InstanceData* D = (InstanceData*) Data;

    /* Print a warning */
    Sim->Break ("Writing to write protected memory at $%04X (value = $%02X)",
		D->BaseAddr+Offs, Val);
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

    /* Read the cell and return the value */
    return D->Mem[Offs];
}



