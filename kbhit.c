/*
 * Keyboard test program.
 */

#include <conio.h>

int main(void)
{
    unsigned char c;

    clrscr();
    cputs("   Keyboard Test\r\n");

    while (1) {
        c = cgetc();
        cputc(c);
    }
    return 0;
}
