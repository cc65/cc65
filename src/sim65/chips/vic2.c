/*****************************************************************************/
/*                                                                           */
/*                                  vic2.c                                   */
/*                                                                           */
/*		   VIC II plugin for the sim65 6502 simulator		     */
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

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/cursorfont.h>

/* common */
#include "attrib.h"

/* sim65 */
#include "chipif.h"



/*****************************************************************************/
/*                                   Forwards                                */
/*****************************************************************************/



static int VicInitChip (const struct SimData* Data);
/* Initialize the chip, return an error code */

static void* VicCreateInstance (unsigned Addr, unsigned Range, void* CfgInfo);
/* Create a new chip instance */

static void VicDestroyInstance (void* Data);
/* Destroy a chip instance */

static void VicWrite (void* Data, unsigned Offs, unsigned char Val);
/* Write user data */

static unsigned char VicRead (void* Data, unsigned Offs);
/* Read user data */

static int VRamInitChip (const struct SimData* Data);
/* Initialize the chip, return an error code */

static void* VRamCreateInstance (unsigned Addr, unsigned Range, void* CfgInfo);
/* Create a new chip instance */

static void VRamDestroyInstance (void* Data);
/* Destroy a chip instance */

static void VRamWrite (void* Data, unsigned Offs, unsigned char Val);
/* Write user data */

static unsigned char VRamRead (void* Data, unsigned Offs);
/* Read user data */

static void VRamDrawBorder (void);
/* Draw the complete border */

static void VRamDrawChar (unsigned Offs);
/* Draw one character at the given position */

static void VRamDrawAllChars (void);
/* Redraw the complete interior screen */

static void VRamEventLoop (void);
/* Get all waiting events and handle them */

static int CRamInitChip (const struct SimData* Data);
/* Initialize the chip, return an error code */

static void* CRamCreateInstance (unsigned Addr, unsigned Range, void* CfgInfo);
/* Create a new chip instance */

static void CRamDestroyInstance (void* Data);
/* Destroy a chip instance */

static void CRamWrite (void* Data, unsigned Offs, unsigned char Val);
/* Write user data */

static unsigned char CRamRead (void* Data, unsigned Offs);
/* Read user data */



/*****************************************************************************/
/*                                Global data                                */
/*****************************************************************************/



/* The SimData pointer we get when InitChip is called */
static const SimData* Sim;

/* Control data passed to the main program */
static const struct ChipData CData[] = {
    {
        "VIC2",                 /* Name of the chip */
        CHIPDATA_TYPE_CHIP,     /* Type of the chip */
        CHIPDATA_VER_MAJOR,     /* Version information */
        CHIPDATA_VER_MINOR,

        /* -- Exported functions -- */
        VicInitChip,
        VicCreateInstance,
	VicDestroyInstance,
        VicWrite,
        VicWrite,
        VicRead,
        VicRead
    },
    {
        "VIC2-VIDEORAM",        /* Name of the chip */
        CHIPDATA_TYPE_CHIP,     /* Type of the chip */
        CHIPDATA_VER_MAJOR,     /* Version information */
        CHIPDATA_VER_MINOR,

        /* -- Exported functions -- */
        VRamInitChip,
        VRamCreateInstance,
	VRamDestroyInstance,
        VRamWrite,
        VRamWrite,
        VRamRead,
        VRamRead
    },
    {
        "VIC2-COLORRAM",        /* Name of the chip */
        CHIPDATA_TYPE_CHIP,     /* Type of the chip */
        CHIPDATA_VER_MAJOR,     /* Version information */
        CHIPDATA_VER_MINOR,

        /* -- Exported functions -- */
        CRamInitChip,
        CRamCreateInstance,
	CRamDestroyInstance,
        CRamWrite,
        CRamWrite,
        CRamRead,
        CRamRead
    }
};

/* Defines for the VIC chip */
#define VIC_COLOR_COUNT         16
#define VIC_BLACK	 	0
#define VIC_WHITE	 	1

/* The application color map. VIC II color values are taken from
 * http://www.pepto.de/projects/colorvic/ (Philip "Pepto" Timmermann)
 */
static XColor VicColors [VIC_COLOR_COUNT] = {
    { 0,   0*256,   0*256,   0*256, 0, 0 },     /* black */
    { 0, 255*256, 255*256, 255*256, 0, 0 },     /* white */
    { 0, 104*256,  55*256,  43*256, 0, 0 },     /* red */
    { 0, 112*256, 163*256, 178*256, 0, 0 },     /* cyan */
    { 0, 111*256,  61*256, 134*256, 0, 0 },     /* purple */
    { 0,  88*256, 141*256,  67*256, 0, 0 },     /* green */
    { 0,  53*256,  40*256, 121*256, 0, 0 },     /* blue */
    { 0, 184*256, 199*256, 111*256, 0, 0 },     /* yellow */
    { 0, 111*256,  79*256,  37*256, 0, 0 },     /* orange */
    { 0,  67*256,  57*256,   0*256, 0, 0 },     /* brown */
    { 0, 154*256, 103*256,  89*256, 0, 0 },     /* light red */
    { 0,  68*256,  68*256,  68*256, 0, 0 },     /* dark grey */
    { 0, 108*256, 108*256, 108*256, 0, 0 },     /* grey */
    { 0, 154*256, 210*256, 132*256, 0, 0 },     /* light green */
    { 0, 108*256,  94*256, 181*256, 0, 0 },     /* light blue */
    { 0, 149*256, 149*256, 149*256, 0, 0 }      /* light grey */
};


/*****************************************************************************/
/*                                     Data                                  */
/*****************************************************************************/



/* VIC II instance data */
typedef struct VicInstance VicInstance;
struct VicInstance {
    unsigned            Addr;           /* Address of the chip */
    unsigned            Range;          /* Memory range */
    unsigned char       Regs[47];       /* VIC registers */
};

/* Video RAM instance data */
typedef struct VRamInstance VRamInstance;
struct VRamInstance {

    /* Settings passed from the simulator */
    unsigned            Addr;           /* Address of the chip */
    unsigned            Range;          /* Memory range */

    /* X variables */
    Display*    VicDisplay;
    Window      VicWindow;
    int         VicScreen;
    GC          VicGC;

    /* Window dimensions, 384*288 (PAL) */
    unsigned XTotal;
    unsigned YTotal;

    /* Usable area within the window */
    unsigned XSize;
    unsigned YSize;

    /* Offset of the usable area */
    unsigned XOffs;
    unsigned YOffs;

    /* The window color map. */
    XColor Colors [VIC_COLOR_COUNT];

    /* A list of 4 rectangles used to draw the border */
    XRectangle Border[4];

    /* The virtual screen we are writing to. */
    unsigned char Mem[0x400];

    /* The character ROM data */
    unsigned char CharRom[0x1000];

};

typedef struct CRamInstance CRamInstance;
struct CRamInstance {

    /* Settings passed from the simulator */
    unsigned            Addr;           /* Address of the chip */
    unsigned            Range;          /* Memory range */

    /* The memory we are writing to. */
    unsigned char       Mem[0x400];
};

/* If we have a video ram window, place it's instance data here */
static VicInstance* Vic   = 0;
static VRamInstance* VRam = 0;
static CRamInstance* CRam = 0;



/*****************************************************************************/
/*                               Exported function                           */
/*****************************************************************************/



int GetChipData (const ChipData** Data, unsigned* Count)
{
    /* Pass the control structure to the caller */
    *Data  = CData;
    *Count = sizeof (CData) / sizeof (CData[0]);

    /* Call was successful */
    return 0;
}



/*****************************************************************************/
/*                                VIC II Chip                                */
/*****************************************************************************/



static int VicInitChip (const struct SimData* Data)
/* Initialize the chip, return an error code */
{
    /* Remember the pointer */
    Sim = Data;

    /* Always successful */
    return 0;
}



static void* VicCreateInstance (unsigned Addr, unsigned Range,
                                void* CfgInfo attribute ((unused)))
/* Initialize a new chip instance */
{
    /* Allocate a new instance structure */
    VicInstance* V = Vic = Sim->Malloc (sizeof (VicInstance));

    /* Initialize the structure, allocate RAM and attribute memory */
    V->Addr  = Addr;
    V->Range = Range;
    memset (V->Regs, 0, sizeof (V->Regs));

    /* Done, return the instance data */
    return V;
}



static void VicDestroyInstance (void* Data)
/* Destroy a chip instance */
{
    /* Cast the data pointer */
    VicInstance* V = Data;

    /* Free the instance data */
    Sim->Free (V);
}



static void VicWrite (void* Data, unsigned Offs, unsigned char Val)
/* Write user data */
{
    /* Cast the data pointer */
    VicInstance* V = Data;

    /* Check for a write outside our range */
    if (Offs >= sizeof (V->Regs)) {
        Sim->Break ("Writing to invalid VIC register at $%04X", V->Addr+Offs);
    } else {

        /* Do the write */
        V->Regs[Offs] = Val;

        /* Handle special registers */
        switch (Offs) {
            case 32:
                /* Exterior color */
                if (VRam) {
                    VRamDrawBorder ();
                }
                break;
            case 33:
                /* Background color #0 */
                if (VRam) {
                    VRamDrawAllChars ();
                }
                break;

        }

        /* Handle the event queue */
        if (VRam) {
            VRamEventLoop ();
        }
    }
}



static unsigned char VicRead (void* Data, unsigned Offs)
/* Read user data */
{
    /* Cast the data pointer */
    VicInstance* V = Data;

    /* Simulate the rasterline register */
    if (V->Regs[17] & 0x80) {
        if (++V->Regs[18] == (312 & 0xFF)) {
            V->Regs[17] &= 0x7F;
            V->Regs[18] = 0;
        }
    } else {
        if (++V->Regs[18] == 0) {
            V->Regs[17] |= 0x80;
        }
    }

    /* Check for a read outside our range */
    if (Offs >= sizeof (V->Regs)) {

        Sim->Break ("Reading invalid VIC register at $%04X", V->Addr+Offs);
        return 0xFF;

    } else {

        /* Do the read */
        return V->Regs[Offs];

    }
}



/*****************************************************************************/
/*                                 Video RAM                                 */
/*****************************************************************************/



static int VRamInitChip (const struct SimData* Data)
/* Initialize the chip, return an error code */
{
    /* Remember the pointer */
    Sim = Data;

    /* Always successful */
    return 0;
}



static void* VRamCreateInstance (unsigned Addr, unsigned Range, void* CfgInfo)
/* Create a new chip instance */
{
    char*       Name;
    FILE*       F;
    unsigned    ColorDepth;
    Colormap    CM;
    unsigned    CIdx;
    XSizeHints  SizeHints;
    XWMHints    WMHints;
    Cursor      C;

    /* Allocate the instance data */
    VRamInstance* V = VRam = Sim->Malloc (sizeof (VRamInstance));

    /* Remember a few settings */
    V->Addr  = Addr;
    V->Range = Range;

    /* Setup the window geometry */
    V->XTotal = 384;    /* PAL */
    V->YTotal = 288;
    V->XSize  = 320;
    V->YSize  = 200;
    V->XOffs  = (V->XTotal - V->XSize) / 2;
    V->YOffs  = (V->YTotal - V->YSize) / 2;

    /* Setup the rectanges used to draw the exterior */
    V->Border[0].x      = 0;
    V->Border[0].y      = 0;
    V->Border[0].width  = V->XTotal;
    V->Border[0].height = V->YOffs;
    V->Border[1].x      = 0;
    V->Border[1].y      = V->YOffs + V->YSize;
    V->Border[1].width  = V->XTotal;
    V->Border[1].height = V->YOffs;
    V->Border[2].x      = 0;
    V->Border[2].y      = V->YOffs;
    V->Border[2].width  = V->XOffs;
    V->Border[2].height = V->YSize;
    V->Border[3].x      = V->XOffs + V->XSize;
    V->Border[3].y      = V->YOffs;
    V->Border[3].width  = V->XOffs;
    V->Border[3].height = V->YSize;

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
    if (fread (V->CharRom, 1, sizeof (V->CharRom), F) != sizeof (V->CharRom)) {
        Sim->Warning ("Char ROM `%s' seems to be corrupt", Name);
    }

    /* Close the file */
    fclose (F);

    /* Free the file name */
    Sim->Free (Name);

    /* Open the X display. */
    V->VicDisplay = XOpenDisplay ("");
    if (V->VicDisplay == NULL) {
        Sim->Error ("VRAM: Cannot open X display");
    }

    /* Get a screen */
    V->VicScreen = DefaultScreen (V->VicDisplay);

    /* Check the available colors. For now, we expect direct colors, so we
     * will check for a color depth of at least 16.
     */
    ColorDepth = XDefaultDepth (V->VicDisplay, V->VicScreen);
    if (ColorDepth < 16) {
        /* OOPS */
        Sim->Error ("VRAM: Need color display");
    }

    /* Get all needed colors */
    memcpy (V->Colors, VicColors, sizeof (V->Colors));
    CM = DefaultColormap (V->VicDisplay, V->VicScreen);
    for (CIdx = 0; CIdx < VIC_COLOR_COUNT; CIdx++) {
        if (XAllocColor (V->VicDisplay, CM, &V->Colors [CIdx]) == 0) {
            Sim->Error ("VRAM: Cannot allocate color");
        }
    }

    /* Set up the size hints structure */
    SizeHints.x          = 0;
    SizeHints.y          = 0;
    SizeHints.flags      = PPosition | PSize | PMinSize | PMaxSize | PResizeInc;
    SizeHints.width      = V->XTotal;
    SizeHints.height     = V->YTotal;
    SizeHints.min_width  = V->XTotal;
    SizeHints.min_height = V->YTotal;
    SizeHints.max_width  = V->XTotal;
    SizeHints.max_height = V->YTotal;
    SizeHints.width_inc  = 0;
    SizeHints.height_inc = 0;
    WMHints.flags        = InputHint;
    WMHints.input        = True;

    /* Create the window */
    V->VicWindow = XCreateSimpleWindow (V->VicDisplay,
                                        DefaultRootWindow (V->VicDisplay),
                                        SizeHints.x,
                                        SizeHints.y,
                                        SizeHints.width,
                                        SizeHints.height,
                                        5,
                                        V->Colors [VIC_WHITE].pixel,
                                        V->Colors [VIC_BLACK].pixel);

    /* Set the standard window properties */
    XSetStandardProperties (V->VicDisplay,              /* Display */
                            V->VicWindow,               /* Window */
                            "sim65 VIC screen",         /* Window name */
                            "sim65 VIC screen",         /* Icon name */
                            None,                       /* Icon Pixmap */
                            0,                          /* argv */
                            0,                          /* argc */
                            &SizeHints);                /* Hints */
    XSetWMHints (V->VicDisplay, V->VicWindow, &WMHints);

    /* GC creation and initialization */
    V->VicGC = XCreateGC (V->VicDisplay, V->VicWindow, 0, 0);

    /* Set the cursor to show over the Vic window */
    C = XCreateFontCursor (V->VicDisplay, XC_pirate);
    XDefineCursor (V->VicDisplay, V->VicWindow, C);

    /* Select input events */
    XSelectInput (V->VicDisplay, V->VicWindow, ExposureMask | StructureNotifyMask);

    /* Show the window */
    XMapRaised (V->VicDisplay, V->VicWindow);

    /* Handle events */
    VRamEventLoop ();

    /* Return the instance data */
    return V;
}



static void VRamDestroyInstance (void* Data)
/* Destroy a chip instance */
{
    /* Cast the data pointer */
    VRamInstance* V = Data;

    /* Free X resources */
    XUndefineCursor (V->VicDisplay, V->VicWindow);
    XFreeGC (V->VicDisplay, V->VicGC);
    XDestroyWindow (V->VicDisplay, V->VicWindow);
    XCloseDisplay (V->VicDisplay);

    /* Clear the global pointer */
    VRam = 0;

    /* Free the instance data */
    Sim->Free (V);
}



static void VRamWrite (void* Data, unsigned Offs, unsigned char Val)
/* Write user data */
{
    /* Cast the data pointer */
    VRamInstance* V = Data;

    /* Check the offset */
    if (Offs >= sizeof (V->Mem)) {
        Sim->Break ("VRAM: Accessing invalid memory at $%06X", V->Addr + Offs);
        return;
    }

    /* Write the value */
    V->Mem[Offs] = Val;

    /* If this changes the visible part of the screen, schedule a redraw */
    if (Offs < 40*25) {

        /* Schedule a redraw */
        VRamDrawChar (Offs);

        /* Call the event loop */
        VRamEventLoop ();
    }
}



static unsigned char VRamRead (void* Data, unsigned Offs)
/* Read user data */
{
    /* Cast the data pointer */
    VRamInstance* V = Data;

    /* Check the offset */
    if (Offs >= sizeof (V->Mem)) {
        Sim->Break ("VRAM: Accessing invalid memory at $%06X", V->Addr + Offs);
        return 0xFF;
    } else {
        return V->Mem[Offs];
    }
}



static void VRamDrawBorder (void)
/* Draw the complete border */
{
    if (Vic) {
        /* Set the border color */
        XSetForeground (VRam->VicDisplay, VRam->VicGC, VRam->Colors[Vic->Regs[32]].pixel);

        /* Fill all rectangles that make the border */
        XFillRectangles (VRam->VicDisplay, VRam->VicWindow, VRam->VicGC,
                         VRam->Border, sizeof (VRam->Border) / sizeof (VRam->Border[0]));
    }
}



static void VRamDrawChar (unsigned Offs)
/* Draw one character at the given position */
{
    unsigned    Row, Col;
    XPoint      Points[64];
    unsigned    PCount;
    unsigned    Color;

    /* Get the character from the video RAM */
    unsigned char C = VRam->Mem[Offs];

    /* Calculate the offset for the character data in the character ROM */
    unsigned char* D = VRam->CharRom + (C * 8);

    /* Calculate the coords for the output */
    unsigned X = VRam->XOffs + (Offs % 40) * 8;
    unsigned Y = VRam->YOffs + (Offs / 40) * 8;

    /* Clear the character area with the background color */
    XSetForeground (VRam->VicDisplay, VRam->VicGC, VRam->Colors[Vic->Regs[33]].pixel);
    XFillRectangle (VRam->VicDisplay, VRam->VicWindow, VRam->VicGC, X, Y, 8, 8);

    /* Set the character color */
    Color = CRam? CRam->Mem[Offs] & 0x0F : VIC_WHITE;
    XSetForeground (VRam->VicDisplay, VRam->VicGC, VRam->Colors[Color].pixel);

    /* Draw the foreground pixels */
    PCount = 0;
    for (Row = 0; Row < 8; ++Row) {

        /* Get next byte from char rom */
        unsigned Data = *D++;

        /* Make pixels from this byte */
        for (Col = 0; Col < 8; ++Col) {
            if (Data & 0x80) {
                /* Foreground pixel */
                Points[PCount].x = X + Col;
                Points[PCount].y = Y + Row;
                ++PCount;
            }
            Data <<= 1;
        }
    }
    if (PCount) {
        XDrawPoints (VRam->VicDisplay, VRam->VicWindow, VRam->VicGC,
                     Points, PCount, CoordModeOrigin);
    }
}



static void VRamDrawArea (unsigned X1, unsigned Y1, unsigned X2, unsigned Y2)
/* Update an area of the interior screen */
{
    unsigned X, Y;

    /* Check if we have to draw anything */
    if (X2 < VRam->XOffs || Y2 < VRam->YOffs ||
        X1 >= VRam->XOffs + VRam->XSize ||
        Y1 >= VRam->YOffs + VRam->YSize) {
        /* Completely outside */
        return;
    }

    /* Make the coordinates relative to the interior */
    X1 -= VRam->XOffs;
    Y1 -= VRam->YOffs;
    X2 -= VRam->XOffs;
    Y2 -= VRam->YOffs;

    /* Loop updating characters */
    for (Y = Y1; Y <= Y2; Y += 8) {
        for (X = X1; X <= X2; X += 8) {
            VRamDrawChar ((Y / 8) * 40 + (X / 8));
        }
    }
}



static void VRamDrawAllChars (void)
/* Redraw the complete interior screen */
{
    unsigned I;
    for (I = 0; I < 25*40; ++I) {
        VRamDrawChar (I);
    }
}



static void VRamEventLoop (void)
/* Get all waiting events and handle them */
{
    unsigned X1, Y1, X2, Y2;

    /* Read input events */
    while (XEventsQueued (VRam->VicDisplay, QueuedAfterFlush) != 0) {

        /* Read an event */
        XEvent Event;
        XNextEvent (VRam->VicDisplay, &Event);

        switch (Event.type) {

            case Expose:
		/* Calculate the area to redraw, then update the screen */
                X1 = Event.xexpose.x;
                Y1 = Event.xexpose.y;
                X2 = Event.xexpose.x + Event.xexpose.width - 1;
                Y2 = Event.xexpose.y + Event.xexpose.height - 1;
                if (X1 < VRam->XOffs || X2 > VRam->XOffs + VRam->XSize ||
                    Y1 < VRam->YOffs || Y2 > VRam->YOffs + VRam->YSize) {
                    /* Update the border */
                    VRamDrawBorder ();
                }
                VRamDrawArea (X1, Y1, X2, Y2);
                break;

            case MappingNotify:
                XRefreshKeyboardMapping (&Event.xmapping);
                break;

        }
    }

    /* Flush the outgoing event queue */
    XFlush (VRam->VicDisplay);
}



/*****************************************************************************/
/*                                 Color RAM                                 */
/*****************************************************************************/



static int CRamInitChip (const struct SimData* Data)
/* Initialize the chip, return an error code */
{
    /* Remember the pointer */
    Sim = Data;

    /* Always successful */
    return 0;
}



static void* CRamCreateInstance (unsigned Addr, unsigned Range,
                                 void* CfgInfo attribute ((unused)))
/* Create a new chip instance */
{
    /* Allocate the instance data */
    CRamInstance* C = CRam = Sim->Malloc (sizeof (CRamInstance));

    /* Remember a few settings */
    C->Addr  = Addr;
    C->Range = Range;

    /* Clear the color RAM memory */
    memset (C->Mem, 0x00, sizeof (C->Mem));

    /* Return the instance data */
    return C;
}



static void CRamDestroyInstance (void* Data)
/* Destroy a chip instance */
{
    /* Clear the global pointer */
    CRam = 0;

    /* Free the instance data */
    Sim->Free (Data);
}



static void CRamWrite (void* Data, unsigned Offs, unsigned char Val)
/* Write user data */
{
    /* Cast the data pointer */
    CRamInstance* C = Data;

    /* Check the offset */
    if (Offs >= sizeof (C->Mem)) {
        Sim->Break ("CRAM: Accessing invalid memory at $%06X", C->Addr + Offs);
        return;
    }

    /* Write the value */
    C->Mem[Offs] = Val & 0x0F;

    /* If this changes the visible part of the screen, schedule a redraw */
    if (Offs < 40*25) {

        /* Schedule a redraw */
        VRamDrawChar (Offs);

        /* Call the event loop */
        VRamEventLoop ();
    }
}



static unsigned char CRamRead (void* Data, unsigned Offs)
/* Read user data */
{
    /* Cast the data pointer */
    CRamInstance* C = Data;

    /* Check the offset */
    if (Offs >= sizeof (C->Mem)) {
        Sim->Break ("CRAM: Accessing invalid memory at $%06X", C->Addr + Offs);
        return 0xFF;
    } else {
        return C->Mem[Offs] | 0xF0;
    }
}



