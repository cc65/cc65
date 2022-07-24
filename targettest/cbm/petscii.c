
/* this program prints all available "petscii" characters to screen, once
   using putchar (which wraps to kernal i/o) and once using conio (which
   will do direct videoram access). after that the produced screencodes
   are compared (they should match) (related to issue #988 */

#include <conio.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#if defined(__C64__)
#define VRAMPEEK(x) (*(char*)(0x0400 + (x)))
#define VRAMPOKE(x, y) *(char*)(0x0400 + (x)) = (y)
#define CRAMPEEK(x) ((*(char*)(0xd800 + (x))) & 15)
#define CRAMPOKE(x, y) *(char*)(0xd800 + (x)) = (y)
#else
#error "this target is not supported yet"
#endif

unsigned char x, y, c;
unsigned char c1, c2;
unsigned char *p1, *p2;

int err = 0;

int main(void)
{
    clrscr();
    bgcolor(COLOR_BLACK);
    bordercolor(COLOR_BLACK);

    /* output all characters using putchar() */
    c = 0;
    for (y = 0; y < 16; y++) {
        for (x = 0; x < 16; x++) {
            /* skip the codes that are unprintable control codes */
            if (!((c < 32) || ((c > 127) && (c < 160)))) {
                gotoxy(x, y); putchar(c);
            }
            c++;
        }
    }

    /* output all characters using conio */
    c = 0;
    for (y = 0; y < 16; y++) {
        for (x = 0; x < 16; x++) {
            /* skip the codes that are unprintable control codes */
            if (!((c < 32) || ((c > 127) && (c < 160)))) {
                gotoxy(x + 20, y); cputc(c);
            }
            c++;
        }
    }

    /* compare the two outputs */
    for (y = 0; y < 16; y++) {
        for (x = 0; x < 16; x++) {
            c1 = VRAMPEEK((y * 40) + x);
            c2 = VRAMPEEK((y * 40) + x + 0x14);
            if (c1 == c2) {
                c = COLOR_GREEN;
            } else {
                c = COLOR_RED;
                err = 1;
            }
            CRAMPOKE((y * 40) + x, c);
            CRAMPOKE((y * 40) + x + 0x14, c);
        }
    }

    /* show the result */
    textcolor(COLOR_WHITE);
    gotoxy(0, 17);
    if (err) {
        bordercolor(COLOR_RED);
        cputs("errors detected");
    } else {
        bordercolor(COLOR_GREEN);
        cputs("all fine");
    }
    cputs(" - press a key ");
    cursor(1);
    cgetc();
    cursor(0);

    clrscr();
    bordercolor(COLOR_BLACK);

    /* output all characters directly to the scree */
    c = 0;
    for (y = 0; y < 16; y++) {
        for (x = 0; x < 16; x++) {
            VRAMPOKE((y * 40) + x, c);
            CRAMPOKE((y * 40) + x, c & 15);
            c++;
        }
    }

    /* read the characters with conio peek functions and output with conio */
    for (y = 0; y < 16; y++) {
        for (x = 0; x < 16; x++) {
            gotoxy(x, y);
            c1 = cpeekc();
            c2 = cpeekrevers();
            c = cpeekcolor();

            gotoxy(x + 0x14, y);
            revers(c2);
            textcolor(c);
            cputc(c1);
        }
    }

    revers(0);
    textcolor(COLOR_WHITE);
    gotoxy(0, 17);
    cputs("press a key to compare ");
    cursor(1);
    cgetc();
    cursor(0);

    /* compare the two outputs */
    for (y = 0; y < 16; y++) {
        for (x = 0; x < 16; x++) {
            c = COLOR_GREEN;
            c1 = VRAMPEEK((y * 40) + x);
            c2 = VRAMPEEK((y * 40) + x + 0x14);
            if (c1 != c2) {
                c = COLOR_RED;
                err = 1;
            }
            c1 = CRAMPEEK((y * 40) + x);
            c2 = CRAMPEEK((y * 40) + x + 0x14);
            if (c1 != c2) {
                c = COLOR_RED;
                err = 1;
            }
            CRAMPOKE((y * 40) + x, c);
            CRAMPOKE((y * 40) + x + 0x14, c);
        }
    }

    /* show the result */
    revers(0);
    textcolor(COLOR_WHITE);
    gotoxy(0, 17);
    if (err) {
        bordercolor(COLOR_RED);
        cputs("errors detected");
    } else {
        bordercolor(COLOR_GREEN);
        cputs("all fine");
    }
    cputs(" - press a key ");
    cursor(1);
    cgetc();
    cursor(0);

    return 0;
}
