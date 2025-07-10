/*****************************************************************************\
** plasma test program for cc65.                                             **
**                                                                           **
** (w)2001 by groepaz                                                        **
**                                                                           **
** Cleanup and porting by Ullrich von Bassewitz.                             **
**                                                                           **
\*****************************************************************************/



#include <stdlib.h>
#include <time.h>
#include <conio.h>
#include <cc65.h>



#if defined(__C64__) || defined(__C128__)
#  define SCREEN1               0xE000
#  define SCREEN2               0xE400
#  define CHARSET               0xE800
#  define outb(addr,val)        (*(addr)) = (val)
#  define inb(addr)             (*(addr))
#elif defined(__CBM510__)
#  define SCREEN1               0xF000
#  define SCREEN2               0xF400
#  define CHARSET               0xE000
#  define outb(addr,val)        pokebsys ((unsigned)(addr), val)
#  define inb(addr)             peekbsys ((unsigned)(addr))
#elif defined(__PLUS4__)
#  define SCREEN1               0x6400
#  define SCREEN2               0x6C00
#  define CHARSET               0x7000
#  define outb(addr,val)        (*(addr)) = (val)
#  define inb(addr)             (*(addr))
#endif



/* Values for the VIC address register to switch between the two pages */
#if defined(__PLUS4__)
#define PAGE1                   ((SCREEN1 >> 8) & 0xF8)
#define PAGE2                   ((SCREEN2 >> 8) & 0xF8)
#define CHARADR                 ((CHARSET >> 8) & 0xFC)
#else
#define PAGE1                   ((SCREEN1 >> 6) & 0xF0) | ((CHARSET >> 10) & 0x0E)
#define PAGE2                   ((SCREEN2 >> 6) & 0xF0) | ((CHARSET >> 10) & 0x0E)
#endif



/* Use static local variables for speed */
#pragma static-locals (1)


static const unsigned char sinustable[0x100] = {
    0x80, 0x7d, 0x7a, 0x77, 0x74, 0x70, 0x6d, 0x6a,
    0x67, 0x64, 0x61, 0x5e, 0x5b, 0x58, 0x55, 0x52,
    0x4f, 0x4d, 0x4a, 0x47, 0x44, 0x41, 0x3f, 0x3c,
    0x39, 0x37, 0x34, 0x32, 0x2f, 0x2d, 0x2b, 0x28,
    0x26, 0x24, 0x22, 0x20, 0x1e, 0x1c, 0x1a, 0x18,
    0x16, 0x15, 0x13, 0x11, 0x10, 0x0f, 0x0d, 0x0c,
    0x0b, 0x0a, 0x08, 0x07, 0x06, 0x06, 0x05, 0x04,
    0x03, 0x03, 0x02, 0x02, 0x02, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x02, 0x02, 0x02, 0x03,
    0x03, 0x04, 0x05, 0x06, 0x06, 0x07, 0x08, 0x0a,
    0x0b, 0x0c, 0x0d, 0x0f, 0x10, 0x11, 0x13, 0x15,
    0x16, 0x18, 0x1a, 0x1c, 0x1e, 0x20, 0x22, 0x24,
    0x26, 0x28, 0x2b, 0x2d, 0x2f, 0x32, 0x34, 0x37,
    0x39, 0x3c, 0x3f, 0x41, 0x44, 0x47, 0x4a, 0x4d,
    0x4f, 0x52, 0x55, 0x58, 0x5b, 0x5e, 0x61, 0x64,
    0x67, 0x6a, 0x6d, 0x70, 0x74, 0x77, 0x7a, 0x7d,
    0x80, 0x83, 0x86, 0x89, 0x8c, 0x90, 0x93, 0x96,
    0x99, 0x9c, 0x9f, 0xa2, 0xa5, 0xa8, 0xab, 0xae,
    0xb1, 0xb3, 0xb6, 0xb9, 0xbc, 0xbf, 0xc1, 0xc4,
    0xc7, 0xc9, 0xcc, 0xce, 0xd1, 0xd3, 0xd5, 0xd8,
    0xda, 0xdc, 0xde, 0xe0, 0xe2, 0xe4, 0xe6, 0xe8,
    0xea, 0xeb, 0xed, 0xef, 0xf0, 0xf1, 0xf3, 0xf4,
    0xf5, 0xf6, 0xf8, 0xf9, 0xfa, 0xfa, 0xfb, 0xfc,
    0xfd, 0xfd, 0xfe, 0xfe, 0xfe, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xfe, 0xfe, 0xfe, 0xfd,
    0xfd, 0xfc, 0xfb, 0xfa, 0xfa, 0xf9, 0xf8, 0xf6,
    0xf5, 0xf4, 0xf3, 0xf1, 0xf0, 0xef, 0xed, 0xeb,
    0xea, 0xe8, 0xe6, 0xe4, 0xe2, 0xe0, 0xde, 0xdc,
    0xda, 0xd8, 0xd5, 0xd3, 0xd1, 0xce, 0xcc, 0xc9,
    0xc7, 0xc4, 0xc1, 0xbf, 0xbc, 0xb9, 0xb6, 0xb3,
    0xb1, 0xae, 0xab, 0xa8, 0xa5, 0xa2, 0x9f, 0x9c,
    0x99, 0x96, 0x93, 0x90, 0x8c, 0x89, 0x86, 0x83
};



static void doplasma (register unsigned char* scrn)
{
    unsigned char xbuf[40];
    unsigned char ybuf[25];
    unsigned char c1a,c1b;
    unsigned char c2a,c2b;
    unsigned char c1A,c1B;
    unsigned char c2A,c2B;
    register unsigned char i, ii;

    c1a = c1A;
    c1b = c1B;
    for (ii = 0; ii < 25; ++ii) {
        ybuf[ii] = (sinustable[c1a] + sinustable[c1b]);
        c1a += 4;
        c1b += 9;
    }
    c1A += 3;
    c1B -= 5;
    c2a = c2A;
    c2b = c2B;
    for (i = 0; i < 40; ++i) {
        xbuf[i] = (sinustable[c2a] + sinustable[c2b]);
        c2a += 3;
        c2b += 7;
    }
    c2A += 2;
    c2B -= 3;
    for (ii = 0; ii < 25; ++ii) {
        /* Unrolling the following loop will give a speed increase of
        ** nearly 100% (~24fps), but it will also increase the code
        ** size a lot.
        */
        for (i = 0; i < 40; ++i, ++scrn) {
            *scrn = (xbuf[i] + ybuf[ii]);
        }
    }
}

static void makechar (void)
{
    static const unsigned char bittab[8] = {
        0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80
    };
    unsigned char i, ii, b, s;
    unsigned c;

    gotoxy (0, 1);
    for (c = 0; c < 0x100; ++c) {
        s = sinustable[c];
        for (i = 0; i < 8; ++i){
            b = 0;
            for (ii = 0; ii < 8; ++ii) {
                if ((rand() & 0xFFu) > s) {
                    b |= bittab[ii];
                }
            }
            ((unsigned char*)CHARSET) [(c*8) + i] = b;
        }
        if ((c & 0x07) == 0) {
            cputc ('.');
        }
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


#if defined(__C64__)
    unsigned char block;
#endif
#if defined(__C128__)
    unsigned char block;
    unsigned char initflag;
    unsigned char graphflag;
#endif
#if defined(__PLUS4__)
    unsigned int i;
    unsigned char v2;
#endif

    clrscr ();
    cprintf ("Making charset, mompls");
    makechar();

    /* Set the border and background colors */
    border     = bordercolor (COLOR_BLUE);
    background = bgcolor (COLOR_BLUE);
    text       = textcolor (COLOR_BLACK);
    clrscr ();

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
#if defined(__PLUS4__)
    v = inb (&TED.char_addr);
    v2 = inb (&TED.video_addr);
#else
    v = inb (&VIC.addr);
#endif

#if defined(__PLUS4__)
    for (i=0;i<1000;i++) {
        ((unsigned char *) (SCREEN1-0x0400))[i] = 0;
        ((unsigned char *) (SCREEN2-0x0400))[i] = 0;
    }
    outb (&TED.char_addr, CHARADR);
#endif

    /* Run the demo until a key was hit */
    t = clock ();
    while (!kbhit()) {
        /* Build page 1, then make it visible */
        doplasma ((unsigned char*)SCREEN1);
#if defined(__PLUS4__)
        outb (&TED.video_addr, PAGE1);
#else
        outb (&VIC.addr, PAGE1);
#endif

        /* Build page 2, then make it visible */
        doplasma ((unsigned char*)SCREEN2);
#if defined(__PLUS4__)
        outb (&TED.video_addr, PAGE2);
#else
        outb (&VIC.addr, PAGE2);
#endif

        /* Count frames */
        f += 2;
    }
    t = clock() - t;

    /* Switch back the VIC screen */
#if defined(__PLUS4__)
    outb (&TED.video_addr, v2);
    outb (&TED.char_addr, v);
#else
    outb (&VIC.addr, v);
#endif

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

    if (doesclrscrafterexit ()) {
        cputsxy (0, 4, "Press any key when done...");
        (void) cgetc ();
    }

    /* Done */
    return EXIT_SUCCESS;
}
