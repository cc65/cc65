
/*

    GEOSLib example
    
    using DlgBoxFileSelect
    
    Maciej 'YTM/Alliance' Witkowiak
    <ytm@friko.onet.pl>
    
    26.12.1999
*/

#include <geos.h>

char fName[17]="";

void main (void)
{

    r0=(int)fName;

    DlgBoxOk(CBOLDON "You now will be presented","with apps list" CPLAINTEXT);
    DlgBoxFileSelect("",APPLICATION,fName);
    DlgBoxOk("You've chosen:" CBOLDON, fName);

    EnterDeskTop();
}
