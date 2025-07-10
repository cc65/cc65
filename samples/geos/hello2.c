/*
    GEOSLib example

    Hello, world example - using graphic functions

    Maciej 'YTM/Alliance' Witkowiak
    <ytm@friko.onet.pl>

    26.12.1999
*/


#include <geos.h>

// Let's define the window we're operating
struct window wholeScreen = {0, SC_PIX_HEIGHT-1, 0, SC_PIX_WIDTH-1};


void main (void)
{
    // Let's show what we've got...

    // Let's clear the screen - with different pattern, because apps have cleared screen upon
    // start
    SetPattern(0);
    InitDrawWindow(&wholeScreen);
    Rectangle();

    // Now some texts
    PutString(COUTLINEON "This is compiled using cc65!" CPLAINTEXT, 20, 10);
    PutString(CBOLDON "This is bold", 30, 10);
    PutString(CULINEON "and this is bold and underline!", 40, 10);
    PutString(CPLAINTEXT "This is plain text", 50, 10);

    // Wait for 5 secs...
    // Note that this is multitasking sleep, and if there are any icons/menus onscreen,
    // they would be usable, in this case you have only pointer usable
    Sleep(5*50);

    // Normal apps exit from main into system's mainloop, and app finish
    // when user selects it from icons or menu, but here we want to exit
    // immediately.
    // So instead:
    //  MainLoop();
    // we can do:
    //  (nothing as this is the end of main function)
    //  exit(EXIT_SUCCESS);
    //  return;

    return;
}
