/*
 * cputc.c
 *
 * void __fastcall__ cputcxy (unsigned char x, unsigned char y, char c);
 * void __fastcall__ cputc (char c);
 */

#include <conio.h>

/* Implements a 25 by 25 screen in the 1024 bytes video ram (32 by 32) */
#define LINEWIDTH 0x20
#define SCREENBASE ((char *) 0xd000)
#define TOP_OFFSET 4
#define LEFT_OFFSET 3
#define SCREENVISBASE (SCREENBASE + 4 * LINEWIDTH + LEFT_OFFSET)
#define WIDTH 25
#define HEIGHT 25

static unsigned char xpos = 0;
static unsigned char ypos = 0;

void __fastcall__ cputc(char c)
{
    char * const cp = SCREENVISBASE + ypos * LINEWIDTH + xpos;

    *cp = c;

    xpos += 1;
    if (xpos > WIDTH - 1) {
        xpos = 0;
        ypos += 1;

        if (ypos > HEIGHT - 1) {
            ypos = 0;
        }
    }
}

void __fastcall__ cputcxy (unsigned char x, unsigned char y, char c)
{
    xpos = x > WIDTH - 1 ? WIDTH - 1 : x;
    ypos = y > HEIGHT - 1 ? HEIGHT - 1 : y;

    cputc(c);
}

unsigned char wherex (void)
{
    return xpos;
}

unsigned char wherey (void)
{
    return ypos;
}

void __fastcall__ gotox (unsigned char x)
{
    xpos = x;
}

void __fastcall__ gotoy (unsigned char y)
{
    ypos = y;
}

void __fastcall__ gotoxy (unsigned char x, unsigned char y)
{
    xpos = x;
    ypos = y;
}
