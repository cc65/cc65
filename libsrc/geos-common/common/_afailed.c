/*
** _afailed.c
**
** Maciej 'YTM/Elysium' Witkowiak 28.10.2001
*/

#include <stdio.h>
#include <stdlib.h>
#include <geos.h>

void _afailed (char* file, unsigned line)
{
    ExitTurbo();

    drawWindow.top = 0;
    drawWindow.left = 0;
    drawWindow.bot = 15;
    drawWindow.right = 150;
    dispBufferOn = ST_WR_FORE|ST_WR_BACK;
    SetPattern(0);
    Rectangle();
    FrameRectangle(0xff);

    PutString(CBOLDON "file: ", 10, 10);
    PutString(file, 10, r11);
    PutString(CBOLDON "  line: ", 10, r11);
    PutDecimal(0, line, 10, r11);

    DlgBoxOk(CBOLDON "ASSERTION FAILED", "PROGRAM TERMINATED" CPLAINTEXT);

    exit (2);
}
