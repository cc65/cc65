/*
    GEOSLib example

    Hello, world example - with DBox

    Maciej 'YTM/Elysium' Witkowiak
    <ytm@elysium.pl>

    26.12.1999
*/


#include <geos.h>

void main (void)
{
    // Let's show what we've got...

    DlgBoxOk(CBOLDON "Hello, world" CPLAINTEXT,
             "This is written in C!");

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
