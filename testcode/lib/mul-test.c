/* mul-test.c -- Test the multiplication operator. */

#include <time.h>
#include <conio.h>
#include <ctype.h>


/* Number of elements in the progress bar. Use a power of 2, to avoid the
** multiplication (which is about to be tested).
*/
#define BAR_ELEMENTS    32U

#if defined(__CBM__)
static const unsigned char revers_bar[8] = {
    0, 0, 0, 0, 0, 1, 1, 1
};
static const unsigned char small_bar[8] = {
    ' ', 0xa5, 0xb4, 0xb5, 0xa1, 0xb6, 0xaa, 0xa7
};

#elif defined(__ATARI__)
#endif

/* Screen co-ordinates for the progress meter */
static unsigned char Width, Height;
static unsigned char X, Y;

static void ProgressMeter (unsigned Val)
/* Print the progress bar. */
{
    gotoxy (X, Y);
    cprintf (" %5lu/65536\r\n", (unsigned long) Val);
    revers (1);
    cclear (Val / (unsigned)(65536U / BAR_ELEMENTS));

/* Commodore and Atari computers can show eight times greater precision. */
#if defined(__CBM__)
    Val = (Val / (unsigned)(65536U / BAR_ELEMENTS / 8)) % 8;
    revers (revers_bar[Val]);
    cputc (small_bar[Val]);

#elif defined(__ATARI__)
#endif

    revers (0);
}



int main(void)
{
    char C;

    /* Clock variables */
    clock_t Ticks;
    clock_t Wait;
    unsigned Days;
    unsigned Hours;
    unsigned Minu;
    unsigned Sec;
    unsigned Milli;

    /* Actual test variables */
    register unsigned lhs = 0;
    register unsigned rhs = 0;
    register unsigned res;

    /* Clear the screen, and output an informational message. */
    clrscr ();
    screensize (&Width, &Height);
    cprintf ("This program does an exhaustive test of\r\n"
             "the multiplication routine. It runs for\r\n"
             "several days; so, please wait very\r\n"
             "patiently (or, speed up your emulator).\r\n"
             "\n"
             "Progress: ");

    /* Remember the current position for the progress bar */
    X = wherex ();
    Y = wherey ();

    /* Mark the maximum limit of the bar. */
    revers (1);
    cputcxy (BAR_ELEMENTS, Y, ' ');
    cputcxy (BAR_ELEMENTS, Y + 1, ' ');
    revers (0);

/* [Targets that have clock() will define CLOCKS_PER_SEC.] */
#ifdef CLOCKS_PER_SEC

    /* Start timing the test. */
    Ticks = clock();
#endif

    do {

        /* Update the progress bar */
        ProgressMeter (lhs);

/* Enable this to test the progress-meter code.
** (And, run emulators at their maximun speed.)
*/
#if 0
        continue;
#endif

        /* Do one row of tests */
        res = 0;
        do {
            if (lhs * rhs != res) {
#ifdef CLOCKS_PER_SEC
                Wait = clock ();
#endif
                gotoxy (0, Y+3);
                cprintf ("Error on %u * %u: %u != %u\r\n", lhs, rhs, lhs * rhs, res);
                cprintf ("Press a key -- 'Q' to quit. ");
                cursor (1);
                C = toupper (cgetc ());
                cclearxy (0, Y+3, Width);
                cclearxy (0, Y+4, Width);

#ifdef CLOCKS_PER_SEC

                /* Don't time the user's interaction. */
                Ticks += clock () - Wait;
#endif

                if (C == 'Q') {
                    goto Done;
                }
            }

            if (kbhit () && toupper (cgetc ()) == 'Q') {
                goto Done;
            }

            res += lhs;
        } while (++rhs != 0);

    } while (++lhs != 0);

Done:
#ifdef CLOCKS_PER_SEC

    /* Calculate the time used */
    Ticks = clock() - Ticks;
    Milli = ((Ticks % CLOCKS_PER_SEC) * 1000) / CLOCKS_PER_SEC;
    Sec = (unsigned) (Ticks / CLOCKS_PER_SEC);
    Minu = Sec / 60;
    Hours = Minu / 60;
    Days = Hours / 24;
    Hours %= 24;
    Minu %= 60;
    Sec %= 60;

    /* Print the time used */
    gotoxy (0, Y+3);
    cprintf ("Time used:\r\n"
             " %u days,\r\n"
             " %u hours,\r\n"
             " %u minutes,\r\n"
             " %u.%03u seconds.\n", Days, Hours, Minu, Sec, Milli);
#endif

    cprintf ("\rTap a key, to exit. ");
    cgetc();
    return 0;
}


