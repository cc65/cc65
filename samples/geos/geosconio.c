
#include <geos.h>
#include <conio.h>
#include <mouse.h>

void main(void)
{
    struct mouse_info info;
    char ch;

    DlgBoxOk("Now the screen will be", "cleared.");

    clrscr();

    DlgBoxOk("Now a character will be", "written at 20,20");

    gotoxy(20, 20);
    cputc('A');

    DlgBoxOk("Now a string will be", "written at 0,1");

    cputsxy(0, 1, CBOLDON "Just" COUTLINEON  "a " CITALICON "string." CPLAINTEXT );

    DlgBoxOk("Write text and finish it", "with a dot.");

    cursor(1);
    do {
        ch = cgetc();
        cputc(ch);
    } while (ch!='.');
    cursor(0);

    DlgBoxOk("Seems that it is all for conio.", "Let's test mouse routines.");

    mouse_init(1);
    cputsxy(0, 2, CBOLDON "Now you can't see mouse (press any key)" CPLAINTEXT);
    mouse_hide();
    while (!kbhit()) { };
    cputc(cgetc());

    cputsxy(0, 3, CBOLDON "Now you see the mouse (press any key)" CPLAINTEXT);
    mouse_show();
    while (!kbhit()) { };
    cputc(cgetc());

    // Get the current mouse coordinates and button states and print them
    mouse_info(&info);
    gotoxy(0, 4);
    cprintf("X  = %3d", info.pos.x);
    gotoxy(0, 5);
    cprintf("Y  = %3d", info.pos.y);
    gotoxy(0, 6);
    cprintf("LB = %c", (info.buttons & MOUSE_BTN_LEFT)? '1' : '0');
    gotoxy(0, 7);
    cprintf("RB = %c", (info.buttons & MOUSE_BTN_RIGHT)? '1' : '0');

    DlgBoxOk("Bye,", "Bye.");
}
