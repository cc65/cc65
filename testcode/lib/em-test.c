#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <conio.h>
#include <em.h>


#define FORCE_ERROR1 0
#define FORCE_ERROR2 0


static unsigned buf[128];



static void fill (register unsigned* page, register unsigned num)
{
    unsigned char i;
    for (i = 0; i < 128; ++i, ++page) {
        *page = num;
    }
}



static void cmp (unsigned page, register const unsigned* buf, register unsigned num)
{
    unsigned char i;
    for (i = 0; i < 128; ++i, ++buf) {
        if (*buf != num) {
            cprintf ("\r\nData mismatch in page $%04X at $%04X\r\n"
                     "Data is $%04X (should be $%04X)\r\n",
                     page, buf, *buf, num);
            exit (EXIT_FAILURE);
        }
    }
}



int main (void)
{
    unsigned char Res;
    unsigned I;
    unsigned PageCount;
    unsigned char X, Y;
    struct em_copy c;

    clrscr ();
    Res = em_load_driver ("c128-reu.emd");
    if (Res != EM_ERR_OK) {
       	cprintf ("Error in em_load_driver: %u\r\n", Res);
        cprintf ("os: %u, %s\r\n", _oserror, _stroserror (_oserror));
       	exit (EXIT_FAILURE);
    }

    /* Get the number of available pages */
    PageCount = em_pagecount ();
    cprintf ("Loaded ok, page count = $%04X\r\n", PageCount);

    /* Fill all pages */
    cputs ("Filling   ");
    X = wherex ();
    Y = wherey ();
    for (I = 0; I < PageCount; ++I) {
        fill (em_use (I), I);
        em_commit ();
        gotoxy (X, Y);
        cputhex16 (I);
    }

#if FORCE_ERROR1
    ((unsigned*) em_map (0x03))[0x73] = 0xFFFF;
    em_commit ();
#endif

    /* Check all pages */
    cputs ("\r\nComparing ");
    X = wherex ();
    Y = wherey ();
    for (I = 0; I < PageCount; ++I) {
        cmp (I, em_map (I), I);
        gotoxy (X, Y);
        cputhex16 (I);
    }

    /* Setup the copy structure */
    c.offs  = 0;
    c.buf   = buf;
    c.count = sizeof (buf);

    /* Fill again all pages */
    cputs ("\r\nFilling   ");
    X = wherex ();
    Y = wherey ();
    for (I = 0; I < PageCount; ++I) {
        fill (buf, I ^ 0xFFFF);
        c.page  = I;
        em_copyto (&c);
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
    cputs ("\r\nComparing ");
    X = wherex ();
    Y = wherey ();
    for (I = 0; I < PageCount; ++I) {
        c.page = I;
        em_copyfrom (&c);
        cmp (I, buf, I ^ 0xFFFF);
        gotoxy (X, Y);
        cputhex16 (I);
    }
    cprintf ("\r\n");

    cprintf ("Passed!\r\n");

    return 0;

}
