#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <conio.h>
#include <em.h>

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

typedef struct emd_test_s {
    char key;
    char *displayname;
    char *drivername;
} emd_test_t;

static emd_test_t drivers[] = {

#if defined(__APPLE2__)
    { '0', "Apple II auxiliary memory", "a2.auxmem.emd" },
#endif

#if defined(__APPLE2ENH__)
    { '0', "Apple II auxiliary memory", "a2e.auxmem.emd" },
#endif

#if defined(__ATARI__)
    { '0', "Atari 130XE memory", "atr130.emd" },
#endif

#if defined(__ATARIXL__)
    { '0', "Atari 130XE memory", "atrx130.emd" },
#endif

#if defined(__C16__)
    { '0', "C16 RAM above $8000", "c16-ram.emd" },
#endif

#if defined(__C64__)
    { '0', "C64 RAM above $D000", "c64-ram.emd" },
    { '1', "C64 256K", "c64-c256k.emd" },
    { '2', "Double Quick Brown Box", "c64-dqbb.emd" },
    { '3', "GEORAM", "c64-georam.emd" },
    { '4', "Isepic", "c64-isepic.emd" },
    { '5', "RamCart", "c64-ramcart.emd" },
    { '6', "REU", "c64-reu.emd" },
    { '7', "C128 VDC (in C64 mode)", "c64-vdc.emd" },
    { '8', "C64DTV himem", "dtv-himem.emd" },
    { '9', "65816 extra banks", "c64-65816.emd" },
#endif

#if defined(__C128__)
    { '0', "C128 RAM in bank 1", "c128-ram.emd" },
    { '1', "C128 RAM in banks 1, 2 & 3", "c128-ram2.emd" },
    { '2', "GEORAM", "c128-georam.emd" },
    { '3', "RamCart", "c128-ramcart.emd" },
    { '4', "REU", "c128-reu.emd" },
    { '5', "VDC", "c128-vdc.emd" },
#endif

#if defined(__CBM510__)
    { '0', "CBM5x0 RAM in bank 2", "cbm510-ram.emd" },
#endif

#if defined(__CBM610__)
    { '0', "CBM6x0/7x0 RAM in bank 2", "cbm610-ram.emd" },
#endif

    { 0, NULL, NULL }
};

int main (void)
{
    unsigned char Res;
    unsigned I;
    unsigned Offs;
    unsigned PageCount;
    unsigned char X, Y;
    struct em_copy c;
    unsigned index;
    signed char valid_key = -1;
    char key;

    clrscr ();
    cputs ("Which RAM exp to test?\r\n\r\n");
    for (index = 0; drivers[index].key; ++index) {
        cprintf("%c: %s\r\n", drivers[index].key, drivers[index].displayname);
    }

    while (valid_key < 0) {
        key = cgetc();
        for (index = 0; drivers[index].key && valid_key < 0; ++index) {
            if (key == drivers[index].key) {
                valid_key = index;
            }
        }
    }

    clrscr ();
    Res = em_load_driver (drivers[valid_key].drivername);
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
