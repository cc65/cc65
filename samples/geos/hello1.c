
/*

    GEOSLib example
    
    Hello, world example - with DBox
    
    Maciej 'YTM/Alliance' Witkowiak
    <ytm@friko.onet.pl>
    
    26.12.1999
*/

#include <geos.h>

void main (void)
{

/* Let's show what we've got... */

    DlgBoxOk(CBOLDON "Hello, world" CPLAINTEXT,
	     "This is written in C!");
	     
/* Normal apps exit from main into system's mainloop, and app finish
   when user selects it from icons or menu, but here we want to exit
   immediately.
*/
    EnterDeskTop();
}
