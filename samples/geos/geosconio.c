
#include <geos.h>
#include <conio.h>

void main(void) {

char ch;

    DlgBoxOk("Now the screen will be", "cleared.");
    
    clrscr();
    
    DlgBoxOk("Now a character will be", "written at 20,20");
    
    gotoxy(20,20);
    cputc('A');

    DlgBoxOk("Now a string will be", "written at 0,1");
    
    cputsxy(0,1, CBOLDON "Just" COUTLINEON  "a " CITALICON "string." CPLAINTEXT );

    DlgBoxOk("Write text and finish it", "with a dot.");

    cursor(1);
    do {
        ch = cgetc();
	cputc(ch);
    } while (ch!='.');

    DlgBoxOk("Seems that it is all.", "Bye.");

    EnterDeskTop();

}
