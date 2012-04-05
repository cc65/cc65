/*****************************************************************************/
/*                                                                           */
/*				    stdio.c				     */
/*                                                                           */
/*		   STDIO plugin for the sim65 6502 simulator		     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2002-2012, Ullrich von Bassewitz                                      */
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



static int InitChip (const struct SimData* Data);
/* Initialize the chip, return an error code */

static void* CreateInstance (unsigned Addr, unsigned Range, void* CfgInfo);
/* Create a new chip instance */

static void DestroyInstance (void* Data);
/* Destroy a chip instance */

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



/* The SimData pointer we get when InitChip is called */
static const SimData* Sim;

/* Control data passed to the main program */
static const struct ChipData CData[1] = {
    {
        "STDIO",                /* Name of the chip */
        CHIPDATA_TYPE_CHIP,     /* Type of the chip */
        CHIPDATA_VER_MAJOR,     /* Version information */
        CHIPDATA_VER_MINOR,

        /* -- Exported functions -- */
        InitChip,
        CreateInstance,
	DestroyInstance,
        WriteCtrl,
        Write,
        ReadCtrl,
        Read
    }
};

/* Screen instance data */
typedef struct InstanceData InstanceData;
struct InstanceData {
    /* The memory area used for data */
    unsigned char*      Mem[32];
};

/* Function opcodes */
enum {
    F_open,
    F_close,
    F_write,
    F_read,
    F_lseek,
    F_unlink,
    F_chdir,
    F_getcwd,
    F_mkdir,
    F_rmdir,
};



/*****************************************************************************/
/*                               Exported function                           */
/*****************************************************************************/



int GetChipData (const ChipData** Data, unsigned* Count)
{
    /* Pass the control structure to the caller */
    *Data = CData;
    *Count = sizeof (CData) / sizeof (CData[0]);

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



static void* CreateInstance (unsigned Addr, unsigned Range, void* CfgInfo)
/* Initialize a new chip instance */
{
    /* Allocate the instance data */
    InstanceData* D = Sim->Malloc (sizeof (InstanceData));

    /* Initialize the instance data */
    memset (D->Mem, 0x00, sizeof (D->Mem));

    /* Return the instance data */
    return D;
}



static void DestroyInstance (void* Data)
/* Destroy a chip instance */
{
    /* Cast the instance data pointer */
    InstanceData* D = Data;

    /* Free the instance data */
    Sim->Free (D);
}



static void WriteCtrl (void* Data attribute, unsigned Offs, unsigned char Val)
/* Write user data */
{
    /* Cast the instance data pointer */
    InstanceData* D = Data;

    /* Write to the memory */
    D->Mem[Offs] = Val;
}



static void Write (void* Data, unsigned Offs, unsigned char Val)
/* Write user data */
{
    /* Cast the instance data pointer */
    InstanceData* D = Data;

    /* Write to the memory */
    D->Mem[Offs] = Val;

    /* Now check the offset. Zero is special because it will trigger the
     * action
     */
    if (Offs == 0) {

        /* The action is determined by the value that is written */
        switch (Val) {

            case F_open:
                Sim->Break ("Unsupported function $%02X", Val);
                break;

            case F_close:
                Sim->Break ("Unsupported function $%02X", Val);
                break;

            case F_write:
                Sim->Break ("Unsupported function $%02X", Val);
                break;

            case F_read:
                Sim->Break ("Unsupported function $%02X", Val);
                break;

            case F_lseek:
                Sim->Break ("Unsupported function $%02X", Val);
                break;

            case F_unlink:
                Sim->Break ("Unsupported function $%02X", Val);
                break;

            case F_chdir:
                Sim->Break ("Unsupported function $%02X", Val);
                break;

            case F_getcwd:
                Sim->Break ("Unsupported function $%02X", Val);
                break;

            case F_mkdir:
                Sim->Break ("Unsupported function $%02X", Val);
                break;

            case F_rmdir:
                Sim->Break ("Unsupported function $%02X", Val);
                break;

        }
    }
}



static unsigned char ReadCtrl (void* Data, unsigned Offs)
/* Read user data */
{
    /* Cast the instance data pointer */
    InstanceData* D = Data;

    /* Read the cell and return the value */
    return D->Mem[Offs];
}



static unsigned char Read (void* Data, unsigned Offs)
/* Read user data */
{
    /* Cast the instance data pointer */
    InstanceData* D = Data;

    /* Read the cell and return the value */
    return D->Mem[Offs];
}



