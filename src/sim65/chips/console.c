/*****************************************************************************/
/*                                                                           */
/*                                 console.c                                 */
/*                                                                           */
/*                  Console plugin for the sim65 simulator                   */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2003-2009, Ullrich von Bassewitz                                      */
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



static int ScreenInitChip (const struct SimData* Data);
/* Initialize the chip, return an error code */

static void* ScreenCreateInstance (unsigned Addr, unsigned Range, void* CfgInfo);
/* Create a new chip instance */

static void ScreenDestroyInstance (void* Data);
/* Destroy a chip instance */

static void ScreenWrite (void* Data, unsigned Offs, unsigned char Val);
/* Write user data */

static unsigned char ScreenRead (void* Data, unsigned Offs);
/* Read user data */

static void ScreenDrawBorder (void);
/* Draw the complete border */

static void ScreenDrawChar (unsigned Offs);
/* Draw one character at the given position */

static void ScreenDrawAllChars (void);
/* Redraw the complete interior screen */

static void ScreenEventLoop (void);
/* Get all waiting events and handle them */



/*****************************************************************************/
/*                                Global data                                */
/*****************************************************************************/



/* The SimData pointer we get when InitChip is called */
static const SimData* Sim;

/* Control data passed to the main program */
static const struct ChipData CData[] = {
    {
        "VIDEOSCREEN",          /* Name of the chip */
        CHIPDATA_TYPE_CHIP,     /* Type of the chip */
        CHIPDATA_VER_MAJOR,     /* Version information */
        CHIPDATA_VER_MINOR,

        /* -- Exported functions -- */
        ScreenInitChip,
        ScreenCreateInstance,
	ScreenDestroyInstance,
        ScreenWrite,
        ScreenWrite,
        ScreenRead,
        ScreenRead
    },
};

/* Defines for console screen */
static const XColor FgColor = {
    0,  32*256, 141*256, 32*256, 0, 0          /* green */
};
static const XColor BgColor = {
    0,  0*256, 0*256, 0*256, 0, 0               /* black */
};


/*****************************************************************************/
/*                                     Data                                  */
/*****************************************************************************/



/* Screen instance data */
typedef struct ScreenInstance ScreenInstance;
struct ScreenInstance {

    /* Settings passed from the simulator */
    unsigned            Addr;           /* Address of the chip */
    unsigned            Range;          /* Memory range */

    /* X variables */
    Display*            ScreenDisplay;
    Window              ScreenWindow;
    int                 Screen;
    GC                  ScreenGC;

    /* Windows rows and columns */
    unsigned            Rows;
    unsigned            Cols;

    /* Window dimensions, 384*288 (PAL) */
    unsigned            XTotal;
    unsigned            YTotal;

    /* Usable area within the window */
    unsigned            XSize;
    unsigned            YSize;

    /* Offset of the usable area */
    unsigned            XOffs;
    unsigned            YOffs;

    /* Character height */
    unsigned            CharHeight;

    /* Fore- and background color */
    XColor              FgColor;
    XColor              BgColor;

    /* A list of 4 rectangles used to draw the border */
    XRectangle          Border[4];

    /* The virtual screen we are writing to. */
    unsigned            MemSize;
    unsigned char*      Mem;

    /* The font data */
    unsigned            FontDataSize;
    unsigned char*      FontData;

};

/* If we have a video ram window, place it's instance data here */
static ScreenInstance* VScreen = 0;



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
/*                             Helper functions                              */
/*****************************************************************************/



static long CfgGetNum (void* CfgInfo, const char* AttrName, long Min, long Max, long Def)
/* Read a number from the attributes. Check against Min/Max. Return the
 * number or Def if it doesn't exist.
 */
{
    long Val;

    /* Read the attribute if it does exist */
    if (Sim->GetCfgNum (CfgInfo, AttrName, &Val)) {
        /* Check it */
        if (Val < Min || Val > Max) {
            Sim->Error ("Range error for attribute `%s'", AttrName);
        }

        /* Return it */
        return Val;

    } else {

        /* Return the default */
        return Def;

    }
}



/*****************************************************************************/
/*                              Console screen                               */
/*****************************************************************************/



static int ScreenInitChip (const struct SimData* Data)
/* Initialize the chip, return an error code */
{
    /* Remember the pointer */
    Sim = Data;

    /* Always successful */
    return 0;
}



static void* ScreenCreateInstance (unsigned Addr, unsigned Range, void* CfgInfo)
/* Create a new chip instance */
{
    char*       Name;
    FILE*       F;
    unsigned    ColorDepth;
    Colormap    CM;
    XSizeHints  SizeHints;
    XWMHints    WMHints;
    Cursor      C;

    /* Allocate the instance data */
    ScreenInstance* V = VScreen = Sim->Malloc (sizeof (ScreenInstance));

    /* Remember a few settings */
    V->Addr  = Addr;
    V->Range = Range;

    /* Character height is 8 or given as attribute */
    V->CharHeight = (unsigned) CfgGetNum (CfgInfo, "charheight", 8, 16, 8);

    /* Allocate memory for the font */
    V->FontDataSize = V->CharHeight * 256;
    V->FontData = Sim->Malloc (V->FontDataSize);

    /* We must have a "fontdata" attribute. Get it. */
    if (Sim->GetCfgStr (CfgInfo, "fontdata", &Name) == 0) {
        /* Attribute not found */
        Sim->Error ("Attribute `fontdata' missing");        /* ### */
    }

    /* Open the file with the given name */
    F = fopen (Name, "rb");
    if (F == 0) {
        Sim->Error ("Cannot open `%s': %s", Name, strerror (errno));
    }

    /* Read the file into the memory */
    if (fread (V->FontData, 1, V->FontDataSize, F) != V->FontDataSize) {
        Sim->Warning ("Font data file `%s' seems to be corrupt", Name);
    }

    /* Close the file */
    fclose (F);

    /* Free the file name */
    Sim->Free (Name);

    /* Read screen rows and columns */
    V->Rows = (unsigned) CfgGetNum (CfgInfo, "rows", 15, 75, 25);
    V->Cols = (unsigned) CfgGetNum (CfgInfo, "cols", 32, 132, 80);

    /* Allocate screen memory and clear it */
    V->MemSize = V->Rows * V->Cols;
    V->Mem = Sim->Malloc (V->MemSize);
    memset (V->Mem, ' ', V->MemSize);

    /* Setup the window geometry */
    V->XSize  = V->Cols * 8;
    V->YSize  = V->Rows * V->CharHeight;
    V->XTotal = V->XSize + 20;
    V->YTotal = V->YSize + 20;
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

    /* Open the X display. */
    V->ScreenDisplay = XOpenDisplay ("");
    if (V->ScreenDisplay == NULL) {
        Sim->Error ("Screen: Cannot open X display");
    }

    /* Get a screen */
    V->Screen = DefaultScreen (V->ScreenDisplay);

    /* Check the available colors. For now, we expect direct colors, so we
     * will check for a color depth of at least 16.
     */
    ColorDepth = XDefaultDepth (V->ScreenDisplay, V->Screen);
    if (ColorDepth < 16) {
        /* OOPS */
        Sim->Error ("Screen: Need color display");
    }

    /* Get all needed colors */
    V->FgColor = FgColor;
    V->BgColor = BgColor;
    CM = DefaultColormap (V->ScreenDisplay, V->Screen);
    if (XAllocColor (V->ScreenDisplay, CM, &V->FgColor) == 0) {
        Sim->Error ("Screen: Cannot allocate foreground color");
    }
    if (XAllocColor (V->ScreenDisplay, CM, &V->BgColor) == 0) {
        Sim->Error ("Screen: Cannot allocate background color");
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
    V->ScreenWindow = XCreateSimpleWindow (V->ScreenDisplay,
                                           DefaultRootWindow (V->ScreenDisplay),
                                           SizeHints.x,
                                           SizeHints.y,
                                           SizeHints.width,
                                           SizeHints.height,
                                           5,
                                           V->FgColor.pixel,
                                           V->BgColor.pixel);

    /* Set the standard window properties */
    XSetStandardProperties (V->ScreenDisplay,           /* Display */
                            V->ScreenWindow,            /* Window */
                            "sim65 console screen",     /* Window name */
                            "sim65 console screen",     /* Icon name */
                            None,                       /* Icon Pixmap */
                            0,                          /* argv */
                            0,                          /* argc */
                            &SizeHints);                /* Hints */
    XSetWMHints (V->ScreenDisplay, V->ScreenWindow, &WMHints);

    /* GC creation and initialization */
    V->ScreenGC = XCreateGC (V->ScreenDisplay, V->ScreenWindow, 0, 0);

    /* Set the cursor to show over the console window */
    C = XCreateFontCursor (V->ScreenDisplay, XC_pirate);
    XDefineCursor (V->ScreenDisplay, V->ScreenWindow, C);

    /* Select input events */
    XSelectInput (V->ScreenDisplay, V->ScreenWindow, ExposureMask | StructureNotifyMask);

    /* Show the window */
    XMapRaised (V->ScreenDisplay, V->ScreenWindow);

    /* Handle events */
    ScreenEventLoop ();

    /* Return the instance data */
    return V;
}



static void ScreenDestroyInstance (void* Data)
/* Destroy a chip instance */
{
    /* Cast the data pointer */
    ScreenInstance* V = Data;

    /* Free X resources */
    XUndefineCursor (V->ScreenDisplay, V->ScreenWindow);
    XFreeGC (V->ScreenDisplay, V->ScreenGC);
    XDestroyWindow (V->ScreenDisplay, V->ScreenWindow);
    XCloseDisplay (V->ScreenDisplay);

    /* Clear the global pointer */
    VScreen = 0;

    /* Free the instance data */
    Sim->Free (V->FontData);
    Sim->Free (V->Mem);
    Sim->Free (V);
}



static void ScreenWrite (void* Data, unsigned Offs, unsigned char Val)
/* Write user data */
{
    /* Cast the data pointer */
    ScreenInstance* V = Data;

    /* Check the offset */
    if (Offs >= V->MemSize) {
        Sim->Break ("Screen: Accessing invalid memory at $%06X", V->Addr + Offs);
        return;
    }

    /* Write the value */
    V->Mem[Offs] = Val;

    /* Schedule a redraw */
    ScreenDrawChar (Offs);

    /* Call the event loop */
    ScreenEventLoop ();
}



static unsigned char ScreenRead (void* Data, unsigned Offs)
/* Read user data */
{
    /* Cast the data pointer */
    ScreenInstance* V = Data;

    /* Check the offset */
    if (Offs >= sizeof (V->Mem)) {
        Sim->Break ("Screen: Accessing invalid memory at $%06X", V->Addr + Offs);
        return 0xFF;
    } else {
        return V->Mem[Offs];
    }
}



static void ScreenDrawBorder (void)
/* Draw the complete border */
{
    if (VScreen) {
        /* Set the border color */
        XSetForeground (VScreen->ScreenDisplay, VScreen->ScreenGC, VScreen->BgColor.pixel);

        /* Fill all rectangles that make the border */
        XFillRectangles (VScreen->ScreenDisplay, VScreen->ScreenWindow, VScreen->ScreenGC,
                         VScreen->Border, sizeof (VScreen->Border) / sizeof (VScreen->Border[0]));
    }
}



static void ScreenDrawChar (unsigned Offs)
/* Draw one character at the given position */
{
    unsigned    Row, Col;
    XPoint      Points[128];
    unsigned    PCount;

    /* Get the character from the video RAM */
    unsigned char C = VScreen->Mem[Offs];

    /* Calculate the offset for the character data in the character ROM */
    unsigned char* D = VScreen->FontData + (C * VScreen->CharHeight);

    /* Calculate the coords for the output */
    unsigned X = VScreen->XOffs + (Offs % VScreen->Cols) * 8;
    unsigned Y = VScreen->YOffs + (Offs / VScreen->Cols) * VScreen->CharHeight;

    /* Clear the character area with the background color */
    XSetForeground (VScreen->ScreenDisplay, VScreen->ScreenGC, VScreen->BgColor.pixel);
    XFillRectangle (VScreen->ScreenDisplay, VScreen->ScreenWindow, VScreen->ScreenGC, X, Y, 8, VScreen->CharHeight);

    /* Prepare the foreground pixels */
    PCount = 0;
    for (Row = 0; Row < VScreen->CharHeight; ++Row) {

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
        /* Set the character color */
        XSetForeground (VScreen->ScreenDisplay, VScreen->ScreenGC, VScreen->FgColor.pixel);

        /* Draw the pixels */
        XDrawPoints (VScreen->ScreenDisplay, VScreen->ScreenWindow, VScreen->ScreenGC,
                     Points, PCount, CoordModeOrigin);
    }
}



static void ScreenDrawArea (unsigned X1, unsigned Y1, unsigned X2, unsigned Y2)
/* Update an area of the interior screen */
{
    unsigned X, Y;

    /* Check if we have to draw anything */
    if (X2 < VScreen->XOffs || Y2 < VScreen->YOffs ||
        X1 >= VScreen->XOffs + VScreen->XSize ||
        Y1 >= VScreen->YOffs + VScreen->YSize) {
        /* Completely outside */
        return;
    }

    /* Make the coordinates relative to the interior */
    X1 -= VScreen->XOffs;
    Y1 -= VScreen->YOffs;
    X2 -= VScreen->XOffs;
    Y2 -= VScreen->YOffs;

    /* Loop updating characters */
    for (Y = Y1; Y <= Y2; Y += 8) {
        for (X = X1; X <= X2; X += 8) {
            ScreenDrawChar ((Y / 8) * 40 + (X / 8));
        }
    }
}



static void ScreenDrawAllChars (void)
/* Redraw the complete interior screen */
{
    unsigned I;
    for (I = 0; I < 25*40; ++I) {
        ScreenDrawChar (I);
    }
}



static void ScreenEventLoop (void)
/* Get all waiting events and handle them */
{
    unsigned X1, Y1, X2, Y2;

    /* Read input events */
    while (XEventsQueued (VScreen->ScreenDisplay, QueuedAfterFlush) != 0) {

        /* Read an event */
        XEvent Event;
        XNextEvent (VScreen->ScreenDisplay, &Event);

        switch (Event.type) {

            case Expose:
		/* Calculate the area to redraw, then update the screen */
                X1 = Event.xexpose.x;
                Y1 = Event.xexpose.y;
                X2 = Event.xexpose.x + Event.xexpose.width - 1;
                Y2 = Event.xexpose.y + Event.xexpose.height - 1;
                if (X1 < VScreen->XOffs || X2 > VScreen->XOffs + VScreen->XSize ||
                    Y1 < VScreen->YOffs || Y2 > VScreen->YOffs + VScreen->YSize) {
                    /* Update the border */
                    ScreenDrawBorder ();
                }
                ScreenDrawArea (X1, Y1, X2, Y2);
                break;

            case MappingNotify:
                XRefreshKeyboardMapping (&Event.xmapping);
                break;

        }
    }

    /* Flush the outgoing event queue */
    XFlush (VScreen->ScreenDisplay);
}



