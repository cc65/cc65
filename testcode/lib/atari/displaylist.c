/*
** test program for ANTIC instructions as defined in "_antic.h"
**
** 23-Feb-2017, Christian Krueger
*/

#include <conio.h>
#include <atari.h>

// code is only for testing purposes, as screen and display list are not aligned,
// and jumps not set!

unsigned char DummyScreen[400];

void DisplayList = {
    DL_BLK1,
    DL_BLK2,
    DL_BLK3,
    DL_BLK4,
    DL_BLK5,
    DL_BLK6,
    DL_BLK7,
    DL_DLI(DL_BLK8),
    DL_LMS(DL_CHR40x8x1),
    DummyScreen,
    DL_HSCROL(DL_CHR40x10x1),
    DL_VSCROL(DL_CHR40x8x4),
    DL_CHR40x16x4,
    DL_LMS(DL_HSCROL(DL_VSCROL(DL_DLI(DL_CHR20x8x2)))),
    DummyScreen+120,
    DL_CHR20x16x2,
    DL_MAP40x8x4,
    DL_MAP80x4x2,
    DL_MAP80x4x4,
    DL_MAP160x2x2,
    DL_MAP160x1x2,
    DL_MAP160x2x4,
    DL_MAP160x1x4,
    DL_MAP320x1x1,
    DL_JVB,
    DL_JMP
};


/* We know that the sizeof expression is constant; don't tell us. */

#pragma warn (const-comparison, off)

int
main(void)
{
    int returnValue = (sizeof DisplayList != 28);       // assure only one byte per instruction!

    clrscr();
    if (returnValue)
        cputs("Test FAILED!");
    else
        cputs("Test passed.");

    cputs("\n\rHit any key to exit...");
    cgetc();

    return returnValue;
}
