/*
    This is an example program for GEOS.
    It reads GEOS serial number and prints it on the screen.

    Maciej 'YTM/Elysium' Witkowiak <ytm@elysium.pl>
    05.03.2004
*/


#include <stdlib.h>
#include <geos.h>
#include <conio.h>

const graphicStr Table = {
    NEWPATTERN(0), MOVEPENTO(0, 0), RECTANGLETO(320, 199), GSTR_END };

void Exit(void)
{
    exit(EXIT_SUCCESS);
}

void Menu = {
    (char)0, (char)14, (int)0, (int)28, (char)(HORIZONTAL|1),
    CBOLDON "quit", (char)MENU_ACTION, &Exit };

int main(void)
{
    dispBufferOn = ST_WR_FORE;

    GraphicsString(&Table);
    cputsxy(0, 3, CBOLDON "Your Serial Number is:");
    cputhex16(GetSerialNumber());

    DoMenu(&Menu);
    MainLoop();

    // will never reach this point...
    return 0;
}
