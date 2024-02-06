/*
    GEOSLib example

    using DlgBoxFileSelect

    Maciej 'YTM/Elysium' Witkowiak
    <ytm@elysium.pl>

    26.12.1999
*/


#include <geos.h>

char fName[17] = "";

void main (void)
{
    r0=(int)fName;

    DlgBoxOk(CBOLDON "You now will be presented", "with an apps list" CPLAINTEXT);
    DlgBoxFileSelect("", APPLICATION, fName);
    DlgBoxOk("You've chosen:" CBOLDON, fName);
}
