/*****************************************************************************\
** fire test program for cc65.                                               **
**                                                                           **
** (w)2002 by groepaz/hitmen                                                 **
**                                                                           **
** Cleanup and porting by Ullrich von Bassewitz.                             **
** 2004-06-08, Greg King                                                     **
**                                                                           **
\*****************************************************************************/



/* sync page-flipping to vertical blank */
/* #define DOVSYNC */

#include <stdlib.h>
#include <string.h> /* for memset */
#include <time.h>
#include <conio.h>
#include <cbm.h>



#if defined(__C64__)
#  define BUFFER                0x0400
#  define SCREEN1               0xE000
#  define SCREEN2               0xE400
#  define CHARSET               0xE800
#  define COLORRAM              0xD800
#  define outb(addr,val)        (*(addr) = (val))
#  define inb(addr)             (*(addr))
#elif defined(__C128__)
#  define BUFFER                0x0400
#  define SCREEN1               0xE000
#  define SCREEN2               0xE400
#  define CHARSET               0xE800
#  define COLORRAM              0xD800
#  define outb(addr,val)        (*(addr) = (val))
#  define inb(addr)             (*(addr))
#elif defined(__CBM510__)
#  define BUFFER                0xF800
#  define SCREEN1               0xF000
#  define SCREEN2               0xF400
#  define CHARSET               0xE000
#  define COLORRAM              0xD400
#  define outb(addr,val)        pokebsys ((unsigned)(addr), val)
#  define inb(addr)             peekbsys ((unsigned)(addr))
#endif



/* Values for the VIC address register to switch between the two pages */
#define PAGE1                   ((SCREEN1 >> 6) & 0xF0) | ((CHARSET >> 10) & 0x0E)
#define PAGE2                   ((SCREEN2 >> 6) & 0xF0) | ((CHARSET >> 10) & 0x0E)



/* Use static local variables for speed */
#pragma static-locals (1)



#ifdef DOVSYNC
#  define WAITVSYNC() waitvsync()
#else
#  define WAITVSYNC()
#endif



static void makechar (void)
{
    static const unsigned char bittab[8] = {
        0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80
    };
    register char *font;
    register unsigned char i, ii, b, bc;
    unsigned char c;

    gotoxy (0, 1);

    for (font = (char*)CHARSET; font != (char*)(CHARSET+(1*8)); ++font) {
        *font = 0x00;
    }
    for (font = (char*)(CHARSET+(64*8)); font != (char*)(CHARSET+(256*8)); ++font) {
        *font = 0xff;
    }


    for (c = 0; c < 0x40; ++c) {
        bc = 0;
        for (i = 0; i < 8; i++){
            b = 0;
            for (ii = 0; ii < 8; ii++) {
                bc += c;
                if (bc > 0x3f) {
                    bc = bc - 0x40;
                    b += bittab[(ii + (i & 1)) & 0x7];
                }
            }
            ((unsigned char*)CHARSET + (1 * 8)) [(c * 8) + i] = b;
        }
        if ((c & 0x07) == 0) {
            cputc ('.');
        }
    }
}



static void fire (unsigned screenbase)
{
    register char* screen;
    register char* buffer;
    register char c;

    screen = (char*) screenbase;
    buffer = (char*) BUFFER;

    while (buffer != (char*) (BUFFER + (24 * 40))) {
        c = (buffer[40-1] + buffer[40-1] + buffer[40] + buffer[41]) / 4;
        if (c > 2) {
            c -= 3;
        }
        *screen = *buffer = c;
        ++screen;
        ++buffer;
    }

    screen = (char*) (screenbase + (23 * 40));
    buffer = (char*) (BUFFER + (23 * 40));

    for(; buffer != (char*)(BUFFER+(25*40)); ++screen, ++buffer) {
        *screen = *buffer = 0x30 + (inb (&SID.noise) >> 4);
    }
}



int main (void)
{
    unsigned char border;
    unsigned char background;
    unsigned char text;
    unsigned char v;
    clock_t       t;
    unsigned long f = 0;
    unsigned long sec;
    unsigned      sec10;
    unsigned long fps;
    unsigned      fps10;
    int           i;

#if defined(__C64__)
    unsigned char block;
#endif
#if defined(__C128__)
    unsigned char block;
    unsigned char initflag;
    unsigned char graphflag;
#endif

    /* Noise on channel 3 for random numbers  */
    outb (&SID.v3.freq, 0xffff);
    outb (&SID.v3.ctrl, 0x80);

    clrscr ();
    cprintf ("Making charset, mompls");
    makechar ();

    /* Set the border and background colors */
    border     = bordercolor (COLOR_BLACK);
    background = bgcolor (COLOR_BLACK);
    text       = textcolor (COLOR_BLACK);
    clrscr ();

    for(i = 0; i != 0x400; i++) {
        *((char *)(i + BUFFER))  = 0;
        *((char *)(i + SCREEN1)) = 0;
        *((char *)(i + SCREEN2)) = 0;
        outb ((char*)(i + COLORRAM), COLOR_YELLOW);
    }

#if defined(__C64__) || defined(__C128__)
    /* Move the VIC 16K block */
    block = inb (&CIA2.pra);
    outb (&CIA2.pra, (block & 0xFC) | ((SCREEN1 >> 14) ^ 0x03));
#endif
#if defined(__C128__)
    /* Save and change some flags, so that kernal/basic interrupt handler will
    ** not interfere with our routine.
    */
    initflag = *(unsigned char*) 0xA04;
    *(unsigned char*) 0xA04 &= 0xFE;
    graphflag = *(unsigned char*) 0xD8;
    *(unsigned char*) 0xD8 = 0xFF;
#endif

    /* Remember the VIC address register */
    v = inb (&VIC.addr);

    /* Run the demo until a key was hit */
    t = clock ();
    while (!kbhit()) {
        /* Build page 1, then make it visible */
        fire (SCREEN1);
        WAITVSYNC ();
        outb (&VIC.addr, PAGE1);

        /* Build page 2, then make it visible */
        fire (SCREEN2);
        WAITVSYNC ();
        outb (&VIC.addr, PAGE2);

        /* Count frames */
        f += 2;
    }
    t = clock() - t;

    /* Switch back the VIC screen */
    outb (&VIC.addr, v);

#if defined(__C64__) || defined(__C128__)
    /* Move back the VIC 16K block */
    outb (&CIA2.pra, block);
#endif
#if defined(__C128__)
    /* Restore the flags */
    *(unsigned char*) 0xA04 = initflag;
    *(unsigned char*) 0xD8  = graphflag;
#endif

    /* Fetch the character from the keyboard buffer and discard it */
    (void) cgetc();

    /* Reset screen colors */
    bordercolor (border);
    bgcolor (background);
    textcolor (text);
    clrscr ();

    /* Calculate stats */
    sec   = (t * 10) / CLK_TCK;
    sec10 = sec % 10;
    sec  /= 10;
    fps   = (f * (CLK_TCK * 10)) / t;
    fps10 = fps % 10;
    fps  /= 10;

    /* Output stats */
    gotoxy (0, 0); cprintf ("time  : %lu.%us", sec, sec10);
    gotoxy (0, 1); cprintf ("frames: %lu", f);
    gotoxy (0, 2); cprintf ("fps   : %lu.%u", fps, fps10);

    /* Wait for a key, then end */
    cputsxy (0, 4, "Press any key when done...");
    (void) cgetc ();

    /* Done */
    return EXIT_SUCCESS;
}



