#include <time.h>
#include <conio.h>
#include <ctype.h>


/* Number of elements in the progress bar. Use a power of 2 to avoid the
 * multiplication (which is about to be tested).
 */
#define BAR_ELEMENTS    32U

/* Screen coordinates for the progress meter */
static unsigned char Width, Height;
static unsigned char X, Y;

static void ProgressMeter (unsigned Val)
/* Print the progress bar */
{
    cclearxy (X, Y, Width);
    gotoxy (X, Y);
    cprintf ("% 6lu/65536", (unsigned long) Val);
}



int main(void)
{
    char C;

    /* Clock variable */
    clock_t Ticks;
    unsigned Sec;
    unsigned Milli;

    /* Actual test variables */
    register unsigned lhs = 0;
    register unsigned rhs = 0;
    register unsigned res;

    /* Clear the screen and output an informational message */
    clrscr ();
    screensize (&Width, &Height);
    cprintf ("This program does an exhaustive test of\r\n"
             "the multiplication routine. It runs\r\n"
             "several days, so please wait very\r\n"
             "patiently (or speedup your emulator)\r\n"
             "\r\n"
             "Progress:\r\n");

    /* Remember the current position for the progress bar */
    X = wherex ();
    Y = wherey ();


    /* Read the clock */
    Ticks = clock();


    do {

        /* Update the progress bar */
        ProgressMeter (lhs);

        /* Do one row of tests */
        res = 0;
        do {
            if (lhs * rhs != res) {
                gotoxy (X, Y+1);
                cprintf ("Error on %u * %u: %u != %u\r\n", lhs, rhs, lhs * rhs, res);
                cprintf ("Press a key ..., 'Q' to quit");
                C = toupper (cgetc ());
                cclearxy (X, Y+1, Width);
                cclearxy (X, Y+2, Width);
                if (C == 'Q') {
                    goto Done;
                }
            }
            res += lhs;
        } while (++rhs != 0);

    } while (++lhs != 0);

Done:
    /* Calculate the time used */
    Ticks = clock() - Ticks;
    Sec = (unsigned) (Ticks / CLOCKS_PER_SEC);
    Milli = ((Ticks % CLOCKS_PER_SEC) * 1000) / CLOCKS_PER_SEC;

    /* Print the time used */
    gotoxy (X, Y+1);
    cprintf ("Time used: %u.%03u seconds\n", Sec, Milli);

    return 0;
}


