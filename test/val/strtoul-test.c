/* A small test for strtuol. Assumes twos complement */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <errno.h>



#define outfile stderr



#define ERROR   0
#define OK      1



static unsigned int Failures = 0;



static void IncStr (char* Buf)
/* Increment a number represented as a string by one. The string MUST not
** start with a '9', we cannot handle overflow in this case.
*/
{
    int Len = strlen (Buf);

    while (--Len >= 0) {
        switch (Buf[Len]) {
            case '9':
                Buf[Len] = '0';
                break;

            default:
                ++(Buf[Len]);
                return;
        }
    }
}



static void CheckStrToUL (const char* Str, int Base, unsigned long Val, unsigned char Ok)
{
    char* EndPtr;
    unsigned long Res = strtoul (Str, &EndPtr, Base);
    if (Ok) {
        if (Res != Val) {
            fprintf (outfile,
                     "strtol error in \"%s\":\n"
                     "  result = %lu, should be %lu, chars = %d\n",
                     Str, Res, Val, EndPtr - Str);
            ++Failures;
        }
    } else {
        if (errno != ERANGE) {
            fprintf (outfile,
                     "strtol error in \"%s\":\n"
                     "  should not convert, but errno = %d\n",
                     Str, errno);
            ++Failures;
        }
        if (Res != Val) {
            fprintf (outfile,
                     "strtol error in \"%s\":\n"
                     "  result = %lu, should be %lu, chars = %d\n",
                     Str, Res, Val, EndPtr - Str);
            ++Failures;
        }
    }
}



int main (void)
{
    char Buf[80];

    /* Prefixed allowed if base = 0 */
    CheckStrToUL ("\t 0x10G ", 0, 16UL, OK);
    CheckStrToUL ("\t 0X10G ", 0, 16UL, OK);
    CheckStrToUL (" \t0377\t", 0, 255UL, OK);
    CheckStrToUL (" 377", 0, 377UL, OK);

    CheckStrToUL ("\t -0x10G ", 0, (unsigned long) -16L, OK);
    CheckStrToUL ("\t -0X10G ", 0, (unsigned long) -16L, OK);
    CheckStrToUL (" \t-0377\t", 0, (unsigned long) -255L, OK);
    CheckStrToUL (" -377", 0, (unsigned long) -377L, OK);

    /* No prefixes if base = 10 */
    CheckStrToUL ("\t 1234 ", 10, 1234UL, OK);
    CheckStrToUL ("\t -1234 ", 10, (unsigned long) -1234L, OK);
    CheckStrToUL ("\t -0x10G ", 10, 0UL, OK);
    CheckStrToUL ("\t -0X10G ", 10, 0UL, OK);
    CheckStrToUL (" \t-0377\t", 10, (unsigned long) -377L, OK);
    CheckStrToUL (" 0377", 10, 377UL, OK);

    /* 0x prefix is allowed if base = 16 */
    CheckStrToUL ("\t 0x1234 ", 16, 0x1234UL, OK);
    CheckStrToUL ("\t -0x1234 ", 16, (unsigned long) -0x1234L, OK);
    CheckStrToUL ("\t -010G ", 16, (unsigned long) -16L, OK);
    CheckStrToUL ("\t 10G ", 16, 16UL, OK);

    /* Check ULONG_MAX */
    sprintf (Buf, "%lu", ULONG_MAX);
    CheckStrToUL (Buf, 0, ULONG_MAX, OK);

    /* Check value one larger */
    sprintf (Buf+1, "%lu", ULONG_MAX);
    Buf[0] = '0';
    IncStr (Buf);
    if (Buf[0] == '0') {
        Buf[0] = ' ';
    }
    CheckStrToUL (Buf, 0, ULONG_MAX, ERROR);

    /* Check numbers that are much too large or small */
    CheckStrToUL ("-999999999999999999999999999999999999999999999999999999999", 0, ULONG_MAX, ERROR);
    CheckStrToUL ("+999999999999999999999999999999999999999999999999999999999", 0, ULONG_MAX, ERROR);
    CheckStrToUL (" 999999999999999999999999999999999999999999999999999999999", 0, ULONG_MAX, ERROR);

    /* Check a few other bases */
    CheckStrToUL ("aBcD", 36, 481261UL, OK);
    CheckStrToUL ("zyaB", 35, 0UL, ERROR);
    CheckStrToUL ("zyaB", 36, 1677395UL, ERROR);

    fprintf (outfile, "Failures: %u\n", Failures);
    return (Failures != 0);
}

