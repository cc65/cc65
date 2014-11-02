#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <conio.h>
#include <em.h>


#if defined(__C64__)
#define DRIVERNAME      "c64-ram.emd"
#elif defined(__C128__)
#define DRIVERNAME      "c128-ram.emd"
#elif defined(__C16__)
#define DRIVERNAME      "c16-ram.emd"
#elif defined(__CBM510__)
#define DRIVERNAME      "cbm510-ram.emd"
#elif defined(__CBM610__)
#define DRIVERNAME      "cbm610-ram.emd"
#elif defined(__APPLE2ENH__)
#define DRIVERNAME      "a2e.auxmem.emd"
#elif defined(__APPLE2__)
#define DRIVERNAME      "a2.auxmem.emd"
#elif defined(__ATARIXL__)
#define DRIVERNAME      "atrx130.emd"
#elif defined(__ATARI__)
#define DRIVERNAME      "atr130.emd"
#else
#define DRIVERNAME      "unknown"
#error "Unknown target system"
#endif


#define FORCE_ERROR1 0
#define FORCE_ERROR2 0


#define PAGE_SIZE       128                     /* Size in words */
#define BUF_SIZE        (PAGE_SIZE + PAGE_SIZE/2)
static unsigned buf[BUF_SIZE];



static void cleanup (void)
/* Remove the driver on exit */
{
    em_unload ();
}



static void fill (register unsigned* page, register unsigned char count, register unsigned num)
{
    register unsigned char i;
    for (i = 0; i < count; ++i, ++page) {
        *page = num;
    }
}



static void cmp (unsigned page, register const unsigned* buf,
                 register unsigned char count, register unsigned num)
{
    register unsigned char i;
    for (i = 0; i < count; ++i, ++buf) {
        if (*buf != num) {
            cprintf ("\r\nData mismatch in page $%04X at $%04X\r\n"
                     "Data is $%04X (should be $%04X)\r\n",
                     page, buf, *buf, num);
#ifdef __ATARI__
            cgetc ();
#endif
            exit (EXIT_FAILURE);
        }
    }
}



int main (void)
{
    unsigned char Res;
    unsigned I;
    unsigned Offs;
    unsigned PageCount;
    unsigned char X, Y;
    struct em_copy c;

    clrscr ();
    Res = em_load_driver (DRIVERNAME);
    if (Res != EM_ERR_OK) {
        cprintf ("Error in em_load_driver: %u\r\n", Res);
        cprintf ("os: %u, %s\r\n", _oserror, _stroserror (_oserror));
#ifdef __ATARI__
        cgetc ();
#endif
        exit (EXIT_FAILURE);
    }
    atexit (cleanup);

    /* Get the number of available pages */
    PageCount = em_pagecount ();
    cprintf ("Loaded ok, page count = $%04X\r\n", PageCount);

    /* TEST #1: em_map/em_use/em_commit */
    cputs ("Testing em_map/em_use/em_commit");

    /* Fill all pages */
    cputs ("\r\n  Filling   ");
    X = wherex ();
    Y = wherey ();
    for (I = 0; I < PageCount; ++I) {

        /* Fill the buffer and copy it to em */
        fill (em_use (I), PAGE_SIZE, I);
        em_commit ();

        /* Keep the user happy */
        gotoxy (X, Y);
        cputhex16 (I);
    }

#if FORCE_ERROR1
    ((unsigned*) em_map (0x03))[0x73] = 0xFFFF;
    em_commit ();
#endif

    /* Check all pages */
    cputs ("\r\n  Comparing ");
    X = wherex ();
    Y = wherey ();
    for (I = 0; I < PageCount; ++I) {

        /* Get the buffer and compare it */
        cmp (I, em_map (I), PAGE_SIZE, I);

        /* Keep the user happy */
        gotoxy (X, Y);
        cputhex16 (I);
    }

    /* TEST #2: em_copyfrom/em_copyto. */
    cputs ("\r\nTesting em_copyfrom/em_copyto");

    /* We're filling now 384 bytes per run to test the copy routines with
    ** other sizes.
    */
    PageCount = (PageCount * 2) / 3;

    /* Setup the copy structure */
    c.buf   = buf;
    c.count = sizeof (buf);

    /* Fill again all pages */
    cputs ("\r\n  Filling   ");
    X = wherex ();
    Y = wherey ();
    c.page = 0;
    c.offs = 0;
    for (I = 0; I < PageCount; ++I) {

        /* Fill the buffer and copy it to em */
        fill (buf, BUF_SIZE, I ^ 0xFFFF);
        em_copyto (&c);

        /* Adjust the em offset */
        Offs = c.offs + sizeof (buf);
        c.offs = (unsigned char) Offs;
        c.page += (Offs >> 8);

        /* Keep the user happy */
        gotoxy (X, Y);
        cputhex16 (I);
    }

#if FORCE_ERROR2
    c.page = 0x03;
    em_copyfrom (&c);
    buf[0x73] = 0xFFFF;
    em_copyto (&c);
#endif

    /* Check all pages */
    cputs ("\r\n  Comparing ");
    X = wherex ();
    Y = wherey ();
    c.page = 0;
    c.offs = 0;
    for (I = 0; I < PageCount; ++I) {

        /* Get the buffer and compare it */
        em_copyfrom (&c);
        cmp (I, buf, BUF_SIZE, I ^ 0xFFFF);

        /* Adjust the em offset */
        Offs = c.offs + sizeof (buf);
        c.offs = (unsigned char) Offs;
        c.page += (Offs >> 8);

        /* Keep the user happy */
        gotoxy (X, Y);
        cputhex16 (I);
    }

    /* Success */
    cprintf ("\r\nPassed!\r\n");

#ifdef __ATARI__
    cgetc ();
#endif

    return 0;

}
