/*
 * simple joystick test program
 * CPG 2000
 */

#include <stdio.h>
#include <conio.h>
#include <joystick.h>

/*#define printf_to_use xxprintf*/
#define printf_to_use cprintf

void xxprintf(char *string)
{
    char i=0;

    while (*(string + i)) cputc(*(string + i++));
}

int main(void)
{
    char c,j;
    int s;

    clrscr();
    printf_to_use("Enter joystick # (0-3): ");
    do {
        c = cgetc();
    } while (c < '0' && c > '3');
    printf("%c\n",c);
    j = c - '0';
    printf("using joystick #%d, 'q' to quit\n",j);

    while(1) {
        if (kbhit()) {
            c = cgetc();
            if (c == 'q' || c == 'Q')
                return(0);
        }
        s = readjoy(j);
        gotoxy(1,5);
        if (s & JOY_UP) {
            printf_to_use("UP ");
        }
        else {
            printf_to_use("   ");
        }
        if (s & JOY_DOWN) {
            printf_to_use("DOWN ");
        }
        else {
            printf_to_use("     ");
        }
        if (s & JOY_LEFT) {
            printf_to_use("LEFT ");
        }
        else {
            printf_to_use("     ");
        }
        if (s & JOY_RIGHT) {
            printf_to_use("RIGHT ");
        }
        else {
            printf_to_use("      ");
        }
        if (s & JOY_FIRE) {
            revers(1);
            printf_to_use(" FIRE ");
            revers(0);
        }
        else {
            printf_to_use("      ");
        }
    }
}
