/* A small test for strtol. Assumes twos complement */
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



static void CheckStrToL (const char* Str, int Base, long Val, unsigned char Ok)
{
    char* EndPtr;
    long Res = strtol (Str, &EndPtr, Base);
    if (Ok) {
        if (Res != Val) {
            fprintf (outfile,
                     "strtol error in \"%s\":\n"
                     "  result = %ld, should be %ld, chars = %d\n",
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
                     "  result = %ld, should be %ld, chars = %d\n",
                     Str, Res, Val, EndPtr - Str);
            ++Failures;
        }
    }
}



int main (void)
{
    char Buf[80];

    /* Prefixed allowed if base = 0 */
    CheckStrToL ("\t 0x10G ", 0, 16L, OK);
    CheckStrToL ("\t 0X10G ", 0, 16L, OK);
    CheckStrToL (" \t0377\t", 0, 255L, OK);
    CheckStrToL (" 377", 0, 377L, OK);

    CheckStrToL ("\t -0x10G ", 0, -16L, OK);
    CheckStrToL ("\t -0X10G ", 0, -16L, OK);
    CheckStrToL (" \t-0377\t", 0, -255L, OK);
    CheckStrToL (" -377", 0, -377L, OK);

    /* No prefixes if base = 10 */
    CheckStrToL ("\t 1234 ", 10, 1234L, OK);
    CheckStrToL ("\t -1234 ", 10, -1234L, OK);
    CheckStrToL ("\t -0x10G ", 10, 0L, OK);
    CheckStrToL ("\t -0X10G ", 10, 0L, OK);
    CheckStrToL (" \t-0377\t", 10, -377L, OK);
    CheckStrToL (" 0377", 10, 377L, OK);

    /* 0x prefix is allowed if base = 16 */
    CheckStrToL ("\t 0x1234 ", 16, 0x1234L, OK);
    CheckStrToL ("\t -0x1234 ", 16, -0x1234L, OK);
    CheckStrToL ("\t -010G ", 16, -16L, OK);
    CheckStrToL ("\t 10G ", 16, 16L, OK);

    /* Check LONG_MIN and LONG_MAX */
    sprintf (Buf, "%ld", LONG_MIN);
    CheckStrToL (Buf, 0, LONG_MIN, OK);
    sprintf (Buf, "%ld", LONG_MAX);
    CheckStrToL (Buf, 0, LONG_MAX, OK);

    /* Check value one smaller */
    sprintf (Buf+1, "%ld", LONG_MIN);
    Buf[1] = '0';       /* Overwrite '-' */
    IncStr (Buf+1);
    if (Buf[1] == '0') {
        Buf[1] = '-';
        Buf[0] = ' ';
    } else {
        Buf[0] = '-';
    }
    CheckStrToL (Buf, 0, LONG_MIN, ERROR);

    /* Check value one larger */
    sprintf (Buf+1, "%ld", LONG_MAX);
    Buf[0] = '0';
    IncStr (Buf);
    if (Buf[0] == '0') {
        Buf[0] = ' ';
    }
    CheckStrToL (Buf, 0, LONG_MAX, ERROR);

    /* Check numbers that are much too large or small */
    CheckStrToL ("-999999999999999999999999999999999999999999999999999999999", 0, LONG_MIN, ERROR);
    CheckStrToL ("+999999999999999999999999999999999999999999999999999999999", 0, LONG_MAX, ERROR);
    CheckStrToL (" 999999999999999999999999999999999999999999999999999999999", 0, LONG_MAX, ERROR);

    /* Check a few other bases */
    CheckStrToL ("aBcD", 36, 481261L, OK);
    CheckStrToL ("zyaB", 35, 0L, ERROR);
    CheckStrToL ("zyaB", 36, 1677395L, ERROR);

    fprintf (outfile, "Failures: %u\n", Failures);
    return (Failures != 0);
}
