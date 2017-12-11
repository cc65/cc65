/* Test that the cpeek...() functions are the inverses of cputc(),
** revers(), and textcolor() for the full range of character codes.
**
** 2017-07-15, Greg King
*/

#include <conio.h>
#include <cc65.h>

/* Standard location of the screen */

#if defined(__C128__) || defined(__C64__)
/* only 40-column screen */
#  define SCREEN_RAM ((unsigned char*)0x0400)
#elif defined(__C16__)                  /* Plus4 also */
#  define SCREEN_RAM ((unsigned char*)0x0C00)
#elif defined(__CBM510__)
#  define SCREEN_RAM ((unsigned char*)0xF000)
#elif defined(__CBM610__)
#  define SCREEN_RAM ((unsigned char*)0xD000)
#elif defined(__PET__)
#  define SCREEN_RAM ((unsigned char*)0x0800)
#elif defined(__VIC20__)
#  define SCREEN_RAM ((unsigned char*)0x1000)
#else
#  error This program cannot test that target.
#  define SCREEN_RAM ((unsigned char*)0)
#endif

static unsigned char width;


/* Move the cursor backward one char with
** the recognition of a row change.
*/
static void chBack (void)
{
    unsigned char y = wherey ();
    unsigned char x = wherex ();

    if (x == 0) {
        x = width;
        --y;
    }
    --x;

    gotoxy (x, y);
}


/* Move the cursor forward one char with
** the recognition of a row change.
*/
static void chForth (void)
{
    unsigned char y = wherey ();
    unsigned char x = wherex ();

    if (++x >= width) {
        x = 0;
        ++y;
    }

    gotoxy (x, y);
}


/* A hack to get an unmodified byte from the
** screen memory at the current cursor position.
*/
static unsigned char peekChWithoutTranslation (void)
{
#if defined(__CBM610__)
    return peekbsys ((unsigned)&SCREEN_RAM[wherey () * width + wherex ()]);
#else
    return SCREEN_RAM[wherey () * width + wherex ()];
#endif
}


/* A test which outputs the given char, reads it back from
** screen memory, outputs the returned char at the next position,
** then compares the two screen memory bytes for identity.
**
** Note:  cpeekc() must be tested indirectly because some platforms "fold" their
** character code-set into a smaller screen code-set.  Therefore, cpeekc() might
** return an equivalent, but not equal, character to the one that was cputc().
*/
static unsigned char testCPeekC (char ch)
{
    unsigned char ch2_a, ch2_b, ch2_c;

    /* Test the given char-code, but not the
    ** special characters NEWLINE and RETURN
    ** (they don't put anything on the screen).
    */
    if (('\n' == ch) || ('\r' == ch)
        ) {
        return 1;
    }

    /* Output the char to the screen. */
    cputc (ch);

    /* Move the cursor pos. to the previous output. */
    chBack ();

    /* Get back the written char without any translation. */
    ch2_b = peekChWithoutTranslation ();

    /* Get back the written char,
    ** including the translation, screen-code -> text.
    */
    ch2_a = cpeekc ();

    /* Move the cursor to the following writing position. */
    chForth ();

    /* Output again the char which was read back by cpeekc(). */
    cputc (ch2_a);

    /* Move the cursor pos. to the second output. */
    chBack ();

    /* Get back the second written char without any translation;
    ** and, compare it to the first untranslated char.
    */
    ch2_c = peekChWithoutTranslation ();
    if (ch2_c != ch2_b) {
        /* The test was NOT succesful.
        ** Output a diagnostic; and, return FAILURE.
        */
        revers(0);
        cprintf ("\r\nError on char: %#x was %#x instead.", ch, ch2_a);
        cprintf ("\r\nRaw screen codes: %#x, %#x.", ch2_b, ch2_c);
        return 0;
    }

    /* The test was succesful.
    ** Move the cursor to the following writing position.
    */
    chForth ();
    return 1;
}


/* The main code initiates the screen for the tests, and sets the reverse flag.
** Then, it calls testCPeekC() for every char within 0..255.
** Returns zero for success, one for failure.
*/
int main (void)
{
    unsigned char i;
    int ret = 0;

    clrscr ();
    revers (1);
    screensize (&width, &i);

#if defined(__VIC20__)
    /* The VIC-20's screen is too small to hold the full test. */
    i = 2;
#else
    i = 0;
#endif
    do {
        if (!testCPeekC (i)) {
            ret = 1;
            break;
        }
    } while (++i != 0);         /* will wrap around when finished */

    if (doesclrscrafterexit()) {
        cgetc();
    }
    return ret;
}
