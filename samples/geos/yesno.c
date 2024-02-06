/*
   GEOSLib example

   example of using DlgBoxYesNo, DlgBoxOkCancel and DlgBoxOk functions

   Maciej 'YTM/Elysium' Witkowiak
   <ytm@elysium.pl>

   26.12.1999
 */


#include <geos.h>

void main(void)
{
    do {
        if (DlgBoxYesNo("Are you female?", "(don't lie ;-)") == YES) {
            DlgBoxOk("You claim to be woman!", "You wanna dance?");
        } else {
            DlgBoxOk("Ergh, another man...", "Let's go for a beer.");
        };
    } while (DlgBoxOkCancel("Do you want to try again?", "") == OK);
}
