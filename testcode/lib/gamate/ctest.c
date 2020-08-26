
#include <gamate.h>
#include <time.h>
#include <conio.h>

unsigned char y = 0;
unsigned char x = 0;
unsigned short n;

int main(int argc, char *argv[])
{
    clrscr();
    gotoxy(0,0);cputs("Gamate C-Test");

    textcolor(0);gotoxy(0,5);cputs("abcdABCD 0");
    textcolor(1);gotoxy(0,6);cputs("abcdABCD  1");
    textcolor(2);gotoxy(0,7);cputs("abcdABCD   2");
    textcolor(3);gotoxy(0,8);cputs("abcdABCD    3");

    while(1) {
        textcolor(COLOR_BLACK);

        n = clock();

        gotoxy(0,2);cprintf("%04x %02x %02x %02x", n, x, y, *((unsigned char*)JOY_DATA));

        switch((*((unsigned char*)JOY_DATA))) {
            case 0xff ^ JOY_UP_MASK:
                ++y; if (y == 0xc8) y = 0;
                break;
            case 0xff ^ JOY_DOWN_MASK:
                --y; if (y == 0xff) y = 0xc7;
                break;
            case 0xff ^ JOY_LEFT_MASK:
                ++x;
                break;
            case 0xff ^ JOY_RIGHT_MASK:
                --x;
                break;
            case 0xff ^ JOY_BTN_A_MASK:
                break;
        }

        waitvsync();

        (*((unsigned char*)LCD_XPOS)) = x;
        (*((unsigned char*)LCD_YPOS)) = y;

    }

    return 0;
}
