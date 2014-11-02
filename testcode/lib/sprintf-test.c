#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#if defined(__CC65__)
#include <conio.h>
#endif


/* Flag to #ifdef the tests out that crash the old implementation */
/*#define NOCRASH         1 */



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



/* Struct used to test the 'n' conversion specifier. It is machine dependent /
** not portable.
*/
typedef union WriteCount WriteCount;
union WriteCount {
    signed char C;
    int         I;
    long        L;
};

/* Count the number of tests and the number of failures */
static unsigned Tests = 0;
static unsigned Failures = 0;



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



static void OneTest (int Line, const char* RString, int RCount, const char* Format, ...)
/* Test one conversion. Line is the line number (to make the life of the
** tester easier), RString the expected result and RCount the expected return
** count. The other parameters are used for formatting.
*/
{
    int Count;
    char Buf[128];
    va_list ap;

    /* Count the number of tests */
    ++Tests;

    /* Format the string using the given arguments */
    va_start (ap, Format);
    Count = vsprintf (Buf, Format, ap);
    va_end (ap);

    /* Check the result */
    if (Count != RCount || strcmp (Buf, RString) != 0) {
        ++Failures;
        printf ("%3d: \"%s\" (%d)\n"
                "     \"%s\" (%d)\n",
                Line, Buf, Count, RString, RCount);
    }
}



static void WriteTest (int Line, const char* Format, WriteCount* W, long Count)
/* Test one write conversion. Line is the line number (to make the life of the
** tester easier), Format is the format specification. W is a WriteCount
** variable and Count is the expected result.
*/
{
    /* Clear the counter in full length */
    W->L = 0x5A5A5A5AL;

    /* Format the string using the given arguments */
    OneTest (Line, "4200", 4, Format, 4200, W);

    /* Check the counter */
    if (W->L != Count) {
        ++Failures;
        printf ("%3d: n = 0x%08lX\n"
                "     n = 0x%08lX\n",
                Line, W->L, Count);
    }
}



int main (void)
{
    WriteCount W;


    /* The one and only starter to begin with ... */
    OneTest (__LINE__, "hello, world",            12, "hello, world");

    /* Duplicate percent signs are used to output single ones */
    OneTest (__LINE__, "hello % %",                9, "hello %% %%");

    /* Simple conversions */
    OneTest (__LINE__, "u",                        1, "%c", 'u');
    OneTest (__LINE__, "4200",                     4, "%d", 4200);
    OneTest (__LINE__, "-4200",                    5, "%d", -4200);
    OneTest (__LINE__, "4200",                     4, "%i", 4200);
    OneTest (__LINE__, "-4200",                    5, "%i", -4200);
    OneTest (__LINE__, "10150",                    5, "%o", 4200U);
    OneTest (__LINE__, "167630",                   6, "%o", -4200U);
    OneTest (__LINE__, "hello, world",            12, "hello, %s", "world");
    OneTest (__LINE__, "4200",                     4, "%u", 4200U);
    OneTest (__LINE__, "61336",                    5, "%u", -4200U);
    OneTest (__LINE__, "1068",                     4, "%x", 4200U);
    OneTest (__LINE__, "ef98",                     4, "%x", -4200U);
    OneTest (__LINE__, "1068",                     4, "%X", 4200U);
    OneTest (__LINE__, "EF98",                     4, "%X", -4200U);

    /* Simple conversions with special values */
    OneTest (__LINE__, "\0",                       1, "%c", '\0');
    OneTest (__LINE__, "0",                        1, "%d", 0);
    OneTest (__LINE__, "0",                        1, "%o", 0U);
    OneTest (__LINE__, "hello, ",                  7, "hello, %s", "");
    OneTest (__LINE__, "0",                        1, "%u", 0U);
    OneTest (__LINE__, "0",                        1, "%x", 0U);

    /* 'h' modifier */
    OneTest (__LINE__, "4200",                     4, "%hd", 4200);
    OneTest (__LINE__, "-4200",                    5, "%hd", -4200);
    OneTest (__LINE__, "4200",                     4, "%hi", 4200);
    OneTest (__LINE__, "-4200",                    5, "%hi", -4200);
    OneTest (__LINE__, "10150",                    5, "%ho", 4200U);
    OneTest (__LINE__, "167630",                   6, "%ho", -4200U);
    OneTest (__LINE__, "4200",                     4, "%hu", 4200U);
    OneTest (__LINE__, "61336",                    5, "%hu", -4200U);
    OneTest (__LINE__, "1068",                     4, "%hx", 4200U);
    OneTest (__LINE__, "ef98",                     4, "%hx", -4200U);
    OneTest (__LINE__, "1068",                     4, "%hX", 4200U);
    OneTest (__LINE__, "EF98",                     4, "%hX", -4200U);

    /* 'hh' modifier */
    OneTest (__LINE__, "104",                      3, "%hhd", 4200);
    OneTest (__LINE__, "-104",                     4, "%hhd", -4200);
    OneTest (__LINE__, "104",                      3, "%hhi", 4200);
    OneTest (__LINE__, "-104",                     4, "%hhi", -4200);
    OneTest (__LINE__, "150",                      3, "%hho", 4200U);
    OneTest (__LINE__, "230",                      3, "%hho", -4200U);
    OneTest (__LINE__, "104",                      3, "%hhu", 4200U);
    OneTest (__LINE__, "152",                      3, "%hhu", -4200U);
    OneTest (__LINE__, "68",                       2, "%hhx", 4200U);
    OneTest (__LINE__, "98",                       2, "%hhx", -4200U);
    OneTest (__LINE__, "68",                       2, "%hhX", 4200U);
    OneTest (__LINE__, "98",                       2, "%hhX", -4200U);

    /* 'j' modifier */
    OneTest (__LINE__, "4200123",                  7, "%jd", 4200123L);
    OneTest (__LINE__, "-4200123",                 8, "%jd", -4200123L);
    OneTest (__LINE__, "4200123",                  7, "%ji", 4200123L);
    OneTest (__LINE__, "-4200123",                 8, "%ji", -4200123L);
    OneTest (__LINE__, "20013273",                 8, "%jo", 4200123UL);
    OneTest (__LINE__, "37757764505",             11, "%jo", -4200123UL);
    OneTest (__LINE__, "4200123",                  7, "%ju", 4200123UL);
    OneTest (__LINE__, "4290767173",              10, "%ju", -4200123UL);
    OneTest (__LINE__, "4016bb",                   6, "%jx", 4200123UL);
    OneTest (__LINE__, "ffbfe945",                 8, "%jx", -4200123UL);
    OneTest (__LINE__, "4016BB",                   6, "%jX", 4200123UL);
    OneTest (__LINE__, "FFBFE945",                 8, "%jX", -4200123UL);

    /* 'l' modifier */
    OneTest (__LINE__, "4200123",                  7, "%ld", 4200123L);
    OneTest (__LINE__, "-4200123",                 8, "%ld", -4200123L);
    OneTest (__LINE__, "4200123",                  7, "%li", 4200123L);
    OneTest (__LINE__, "-4200123",                 8, "%li", -4200123L);
    OneTest (__LINE__, "20013273",                 8, "%lo", 4200123UL);
    OneTest (__LINE__, "37757764505",             11, "%lo", -4200123UL);
    OneTest (__LINE__, "4200123",                  7, "%lu", 4200123UL);
    OneTest (__LINE__, "4290767173",              10, "%lu", -4200123UL);
    OneTest (__LINE__, "4016bb",                   6, "%lx", 4200123UL);
    OneTest (__LINE__, "ffbfe945",                 8, "%lx", -4200123UL);
    OneTest (__LINE__, "4016BB",                   6, "%lX", 4200123UL);
    OneTest (__LINE__, "FFBFE945",                 8, "%lX", -4200123UL);

    /* 't' modifier */
    OneTest (__LINE__, "4200",                     4, "%td", 4200);
    OneTest (__LINE__, "-4200",                    5, "%td", -4200);
    OneTest (__LINE__, "4200",                     4, "%ti", 4200);
    OneTest (__LINE__, "-4200",                    5, "%ti", -4200);
    OneTest (__LINE__, "10150",                    5, "%to", 4200U);
    OneTest (__LINE__, "167630",                   6, "%to", -4200U);
    OneTest (__LINE__, "4200",                     4, "%tu", 4200U);
    OneTest (__LINE__, "61336",                    5, "%tu", -4200U);
    OneTest (__LINE__, "1068",                     4, "%tx", 4200U);
    OneTest (__LINE__, "ef98",                     4, "%tx", -4200U);
    OneTest (__LINE__, "1068",                     4, "%tX", 4200U);
    OneTest (__LINE__, "EF98",                     4, "%tX", -4200U);

    /* 'z' modifier */
    OneTest (__LINE__, "4200",                     4, "%zd", 4200);
    OneTest (__LINE__, "-4200",                    5, "%zd", -4200);
    OneTest (__LINE__, "4200",                     4, "%zi", 4200);
    OneTest (__LINE__, "-4200",                    5, "%zi", -4200);
    OneTest (__LINE__, "10150",                    5, "%zo", 4200U);
    OneTest (__LINE__, "167630",                   6, "%zo", -4200U);
    OneTest (__LINE__, "4200",                     4, "%zu", 4200U);
    OneTest (__LINE__, "61336",                    5, "%zu", -4200U);
    OneTest (__LINE__, "1068",                     4, "%zx", 4200U);
    OneTest (__LINE__, "ef98",                     4, "%zx", -4200U);
    OneTest (__LINE__, "1068",                     4, "%zX", 4200U);
    OneTest (__LINE__, "EF98",                     4, "%zX", -4200U);

    /* '+' forces a sign for signed conversions */
    OneTest (__LINE__, "u",                        1, "%+c", 'u');
    OneTest (__LINE__, "+4200",                    5, "%+d", 4200);
    OneTest (__LINE__, "-4200",                    5, "%+d", -4200);
    OneTest (__LINE__, "+4200",                    5, "%+i", 4200);
    OneTest (__LINE__, "-4200",                    5, "%+i", -4200);
    OneTest (__LINE__, "10150",                    5, "%+o", 4200U);
    OneTest (__LINE__, "167630",                   6, "%+o", -4200U);
    OneTest (__LINE__, "hello, world",            12, "%+s", "hello, world");
    OneTest (__LINE__, "4200",                     4, "%+u", 4200U);
    OneTest (__LINE__, "61336",                    5, "%+u", -4200U);
    OneTest (__LINE__, "1068",                     4, "%+x", 4200U);
    OneTest (__LINE__, "ef98",                     4, "%+x", -4200U);
    OneTest (__LINE__, "1068",                     4, "%+X", 4200U);
    OneTest (__LINE__, "EF98",                     4, "%+X", -4200U);

    /* ' ': If the first character of a signed conversion is not a sign, or if
    **      a signed conversion results in no characters, a space is prefixed
    **      to the result.
    */
    OneTest (__LINE__, "u",                        1, "% c", 'u');
    OneTest (__LINE__, " 4200",                    5, "% d", 4200);
    OneTest (__LINE__, "-4200",                    5, "% d", -4200);
    OneTest (__LINE__, " 4200",                    5, "% i", 4200);
    OneTest (__LINE__, "-4200",                    5, "% i", -4200);
    OneTest (__LINE__, "10150",                    5, "% o", 4200U);
    OneTest (__LINE__, "167630",                   6, "% o", -4200U);
    OneTest (__LINE__, "hello, world",            12, "% s", "hello, world");
    OneTest (__LINE__, "4200",                     4, "% u", 4200U);
    OneTest (__LINE__, "61336",                    5, "% u", -4200U);
    OneTest (__LINE__, "1068",                     4, "% x", 4200U);
    OneTest (__LINE__, "ef98",                     4, "% x", -4200U);
    OneTest (__LINE__, "1068",                     4, "% X", 4200U);
    OneTest (__LINE__, "EF98",                     4, "% X", -4200U);

    /* If the ' ' and '+' flags both appear, the ' ' flag is ignored */
    OneTest (__LINE__, "u",                        1, "% +c", 'u');
    OneTest (__LINE__, "+4200",                    5, "% +d", 4200);
    OneTest (__LINE__, "-4200",                    5, "% +d", -4200);
    OneTest (__LINE__, "+4200",                    5, "% +i", 4200);
    OneTest (__LINE__, "-4200",                    5, "% +i", -4200);
    OneTest (__LINE__, "10150",                    5, "% +o", 4200U);
    OneTest (__LINE__, "167630",                   6, "% +o", -4200U);
    OneTest (__LINE__, "hello, world",            12, "% +s", "hello, world");
    OneTest (__LINE__, "4200",                     4, "% +u", 4200U);
    OneTest (__LINE__, "61336",                    5, "% +u", -4200U);
    OneTest (__LINE__, "1068",                     4, "% +x", 4200U);
    OneTest (__LINE__, "ef98",                     4, "% +x", -4200U);
    OneTest (__LINE__, "1068",                     4, "% +X", 4200U);
    OneTest (__LINE__, "EF98",                     4, "% +X", -4200U);

    /* Field width given */
    OneTest (__LINE__, "              u",         15, "%15c", 'u');
    OneTest (__LINE__, "           4200",         15, "%15d", 4200);
    OneTest (__LINE__, "          -4200",         15, "%15d", -4200);
    OneTest (__LINE__, "           4200",         15, "%15i", 4200);
    OneTest (__LINE__, "          -4200",         15, "%15i", -4200);
    OneTest (__LINE__, "          10150",         15, "%15o", 4200U);
    OneTest (__LINE__, "         167630",         15, "%15o", -4200U);
    OneTest (__LINE__, "   hello, world",         15, "%15s", "hello, world");
    OneTest (__LINE__, "           4200",         15, "%15u", 4200U);
    OneTest (__LINE__, "          61336",         15, "%15u", -4200U);
    OneTest (__LINE__, "           1068",         15, "%15x", 4200U);
    OneTest (__LINE__, "           ef98",         15, "%15x", -4200U);
    OneTest (__LINE__, "           1068",         15, "%15X", 4200U);
    OneTest (__LINE__, "           EF98",         15, "%15X", -4200U);

    /* Field width given as separate argument */
    OneTest (__LINE__, "              u",         15, "%*c", 15, 'u');
    OneTest (__LINE__, "           4200",         15, "%*d", 15, 4200);
    OneTest (__LINE__, "          -4200",         15, "%*d", 15, -4200);
    OneTest (__LINE__, "           4200",         15, "%*i", 15, 4200);
    OneTest (__LINE__, "          -4200",         15, "%*i", 15, -4200);
    OneTest (__LINE__, "          10150",         15, "%*o", 15, 4200U);
    OneTest (__LINE__, "         167630",         15, "%*o", 15, -4200U);
    OneTest (__LINE__, "   hello, world",         15, "%*s", 15, "hello, world");
    OneTest (__LINE__, "           4200",         15, "%*u", 15, 4200U);
    OneTest (__LINE__, "          61336",         15, "%*u", 15, -4200U);
    OneTest (__LINE__, "           1068",         15, "%*x", 15, 4200U);
    OneTest (__LINE__, "           ef98",         15, "%*x", 15, -4200U);
    OneTest (__LINE__, "           1068",         15, "%*X", 15, 4200U);
    OneTest (__LINE__, "           EF98",         15, "%*X", 15, -4200U);

    /* Field width and '-' flag given */
    OneTest (__LINE__, "u              ",         15, "%-15c", 'u');
    OneTest (__LINE__, "4200           ",         15, "%-15d", 4200);
    OneTest (__LINE__, "-4200          ",         15, "%-15d", -4200);
    OneTest (__LINE__, "4200           ",         15, "%-15i", 4200);
    OneTest (__LINE__, "-4200          ",         15, "%-15i", -4200);
    OneTest (__LINE__, "10150          ",         15, "%-15o", 4200U);
    OneTest (__LINE__, "167630         ",         15, "%-15o", -4200U);
    OneTest (__LINE__, "hello, world   ",         15, "%-15s", "hello, world");
    OneTest (__LINE__, "4200           ",         15, "%-15u", 4200U);
    OneTest (__LINE__, "61336          ",         15, "%-15u", -4200U);
    OneTest (__LINE__, "1068           ",         15, "%-15x", 4200U);
    OneTest (__LINE__, "ef98           ",         15, "%-15x", -4200U);
    OneTest (__LINE__, "1068           ",         15, "%-15X", 4200U);
    OneTest (__LINE__, "EF98           ",         15, "%-15X", -4200U);

    /* A negative field width specified via an argument is treated as if the
    ** '-' flag and a positive field width were given.
    **
    ** Beware: These tests will crash the old printf routine!
    */
#ifndef NOCRASH
    OneTest (__LINE__, "u              ",         15, "%*c", -15, 'u');
    OneTest (__LINE__, "4200           ",         15, "%*d", -15, 4200);
    OneTest (__LINE__, "-4200          ",         15, "%*d", -15, -4200);
    OneTest (__LINE__, "4200           ",         15, "%*i", -15, 4200);
    OneTest (__LINE__, "-4200          ",         15, "%*i", -15, -4200);
    OneTest (__LINE__, "10150          ",         15, "%*o", -15, 4200U);
    OneTest (__LINE__, "167630         ",         15, "%*o", -15, -4200U);
    OneTest (__LINE__, "hello, world   ",         15, "%*s", -15, "hello, world");
    OneTest (__LINE__, "4200           ",         15, "%*u", -15, 4200U);
    OneTest (__LINE__, "61336          ",         15, "%*u", -15, -4200U);
    OneTest (__LINE__, "1068           ",         15, "%*x", -15, 4200U);
    OneTest (__LINE__, "ef98           ",         15, "%*x", -15, -4200U);
    OneTest (__LINE__, "1068           ",         15, "%*X", -15, 4200U);
    OneTest (__LINE__, "EF98           ",         15, "%*X", -15, -4200U);
#endif

    /* Field width smaller than converted value */
    OneTest (__LINE__, "u",                        1, "%1c", 'u');
    OneTest (__LINE__, "4200",                     4, "%1d", 4200);
    OneTest (__LINE__, "-4200",                    5, "%1d", -4200);
    OneTest (__LINE__, "4200",                     4, "%1i", 4200);
    OneTest (__LINE__, "-4200",                    5, "%1i", -4200);
    OneTest (__LINE__, "10150",                    5, "%1o", 4200U);
    OneTest (__LINE__, "167630",                   6, "%1o", -4200U);
    OneTest (__LINE__, "hello, world",            12, "%1s", "hello, world");
    OneTest (__LINE__, "4200",                     4, "%1u", 4200U);
    OneTest (__LINE__, "61336",                    5, "%1u", -4200U);
    OneTest (__LINE__, "1068",                     4, "%1x", 4200U);
    OneTest (__LINE__, "ef98",                     4, "%1x", -4200U);
    OneTest (__LINE__, "1068",                     4, "%1X", 4200U);
    OneTest (__LINE__, "EF98",                     4, "%1X", -4200U);

    /* Field width specified and '0' flag given */
    OneTest (__LINE__, "000000000004200",         15, "%015d", 4200);
    OneTest (__LINE__, "-00000000004200",         15, "%015d", -4200);
    OneTest (__LINE__, "000000000004200",         15, "%015i", 4200);
    OneTest (__LINE__, "-00000000004200",         15, "%015i", -4200);
    OneTest (__LINE__, "000000000010150",         15, "%015o", 4200U);
    OneTest (__LINE__, "000000000167630",         15, "%015o", -4200U);
    OneTest (__LINE__, "000000000004200",         15, "%015u", 4200U);
    OneTest (__LINE__, "000000000061336",         15, "%015u", -4200U);
    OneTest (__LINE__, "000000000001068",         15, "%015x", 4200U);
    OneTest (__LINE__, "00000000000ef98",         15, "%015x", -4200U);
    OneTest (__LINE__, "000000000001068",         15, "%015X", 4200U);
    OneTest (__LINE__, "00000000000EF98",         15, "%015X", -4200U);

    /* If the '-' and '0' flags are both specified, '0' is ignored */
    OneTest (__LINE__, "4200           ",         15, "%-015d", 4200);
    OneTest (__LINE__, "-4200          ",         15, "%-015d", -4200);
    OneTest (__LINE__, "4200           ",         15, "%-015i", 4200);
    OneTest (__LINE__, "-4200          ",         15, "%-015i", -4200);
    OneTest (__LINE__, "10150          ",         15, "%-015o", 4200U);
    OneTest (__LINE__, "167630         ",         15, "%-015o", -4200U);
    OneTest (__LINE__, "4200           ",         15, "%-015u", 4200U);
    OneTest (__LINE__, "61336          ",         15, "%-015u", -4200U);
    OneTest (__LINE__, "1068           ",         15, "%-015x", 4200U);
    OneTest (__LINE__, "ef98           ",         15, "%-015x", -4200U);
    OneTest (__LINE__, "1068           ",         15, "%-015X", 4200U);
    OneTest (__LINE__, "EF98           ",         15, "%-015X", -4200U);

    /* Precision given */
    OneTest (__LINE__, "u",                        1, "%.15c", 'u');
    OneTest (__LINE__, "000000000004200",         15, "%.15d", 4200);
    OneTest (__LINE__, "-000000000004200",        16, "%.15d", -4200);
    OneTest (__LINE__, "000000000004200",         15, "%.15i", 4200);
    OneTest (__LINE__, "-000000000004200",        16, "%.15i", -4200);
    OneTest (__LINE__, "000000000010150",         15, "%.15o", 4200U);
    OneTest (__LINE__, "000000000167630",         15, "%.15o", -4200U);
    OneTest (__LINE__, "hello, world",            12, "%.15s", "hello, world");
    OneTest (__LINE__, "000000000004200",         15, "%.15u", 4200U);
    OneTest (__LINE__, "000000000061336",         15, "%.15u", -4200U);
    OneTest (__LINE__, "000000000001068",         15, "%.15x", 4200U);
    OneTest (__LINE__, "00000000000ef98",         15, "%.15x", -4200U);
    OneTest (__LINE__, "000000000001068",         15, "%.15X", 4200U);
    OneTest (__LINE__, "00000000000EF98",         15, "%.15X", -4200U);

    /* Precision given via argument */
    OneTest (__LINE__, "u",                        1, "%.*c", 15, 'u');
    OneTest (__LINE__, "000000000004200",         15, "%.*d", 15, 4200);
    OneTest (__LINE__, "-000000000004200",        16, "%.*d", 15, -4200);
    OneTest (__LINE__, "000000000004200",         15, "%.*i", 15, 4200);
    OneTest (__LINE__, "-000000000004200",        16, "%.*i", 15, -4200);
    OneTest (__LINE__, "000000000010150",         15, "%.*o", 15, 4200U);
    OneTest (__LINE__, "000000000167630",         15, "%.*o", 15, -4200U);
    OneTest (__LINE__, "hello, world",            12, "%.*s", 15, "hello, world");
    OneTest (__LINE__, "000000000004200",         15, "%.*u", 15, 4200U);
    OneTest (__LINE__, "000000000061336",         15, "%.*u", 15, -4200U);
    OneTest (__LINE__, "000000000001068",         15, "%.*x", 15, 4200U);
    OneTest (__LINE__, "00000000000ef98",         15, "%.*x", 15, -4200U);
    OneTest (__LINE__, "000000000001068",         15, "%.*X", 15, 4200U);
    OneTest (__LINE__, "00000000000EF98",         15, "%.*X", 15, -4200U);

    /* Negative precision is treated as if the precision were omitted */
#ifndef NOCRASH
    OneTest (__LINE__, "u",                        1, "%.*c", -15, 'u');
    OneTest (__LINE__, "4200",                     4, "%.*d", -15, 4200);
    OneTest (__LINE__, "-4200",                    5, "%.*d", -15, -4200);
    OneTest (__LINE__, "4200",                     4, "%.*i", -15, 4200);
    OneTest (__LINE__, "-4200",                    5, "%.*i", -15, -4200);
    OneTest (__LINE__, "10150",                    5, "%.*o", -15, 4200U);
    OneTest (__LINE__, "167630",                   6, "%.*o", -15, -4200U);
    OneTest (__LINE__, "hello, world",            12, "%.*s", -15, "hello, world");
    OneTest (__LINE__, "4200",                     4, "%.*u", -15, 4200U);
    OneTest (__LINE__, "61336",                    5, "%.*u", -15, -4200U);
    OneTest (__LINE__, "1068",                     4, "%.*x", -15, 4200U);
    OneTest (__LINE__, "ef98",                     4, "%.*x", -15, -4200U);
    OneTest (__LINE__, "1068",                     4, "%.*X", -15, 4200U);
    OneTest (__LINE__, "EF98",                     4, "%.*X", -15, -4200U);
#endif

    /* Field width and precision given */
    OneTest (__LINE__, "              u",         15, "%15.10c", 'u');
    OneTest (__LINE__, "     0000004200",         15, "%15.10d", 4200);
    OneTest (__LINE__, "    -0000004200",         15, "%15.10d", -4200);
    OneTest (__LINE__, "     0000004200",         15, "%15.10i", 4200);
    OneTest (__LINE__, "    -0000004200",         15, "%15.10i", -4200);
    OneTest (__LINE__, "     0000010150",         15, "%15.10o", 4200U);
    OneTest (__LINE__, "     0000167630",         15, "%15.10o", -4200U);
    OneTest (__LINE__, "     hello, wor",         15, "%15.10s", "hello, world");
    OneTest (__LINE__, "     0000004200",         15, "%15.10u", 4200U);
    OneTest (__LINE__, "     0000061336",         15, "%15.10u", -4200U);
    OneTest (__LINE__, "     0000001068",         15, "%15.10x", 4200U);
    OneTest (__LINE__, "     000000ef98",         15, "%15.10x", -4200U);
    OneTest (__LINE__, "     0000001068",         15, "%15.10X", 4200U);
    OneTest (__LINE__, "     000000EF98",         15, "%15.10X", -4200U);

    /* For d, i, o, u, x and X conversions, if a precision is specified, the
    ** '0' flag is ignored.
    */
    OneTest (__LINE__, "     0000004200",         15, "%015.10d", 4200);
    OneTest (__LINE__, "    -0000004200",         15, "%015.10d", -4200);
    OneTest (__LINE__, "     0000004200",         15, "%015.10i", 4200);
    OneTest (__LINE__, "    -0000004200",         15, "%015.10i", -4200);
    OneTest (__LINE__, "     0000010150",         15, "%015.10o", 4200U);
    OneTest (__LINE__, "     0000167630",         15, "%015.10o", -4200U);
    OneTest (__LINE__, "     0000004200",         15, "%015.10u", 4200U);
    OneTest (__LINE__, "     0000061336",         15, "%015.10u", -4200U);
    OneTest (__LINE__, "     0000001068",         15, "%015.10x", 4200U);
    OneTest (__LINE__, "     000000ef98",         15, "%015.10x", -4200U);
    OneTest (__LINE__, "     0000001068",         15, "%015.10X", 4200U);
    OneTest (__LINE__, "     000000EF98",         15, "%015.10X", -4200U);

    /* Zero precision, explicitly specified */
    OneTest (__LINE__, "u",                        1, "%.0c", 'u');
    OneTest (__LINE__, "4200",                     4, "%.0d", 4200);
    OneTest (__LINE__, "-4200",                    5, "%.0d", -4200);
    OneTest (__LINE__, "4200",                     4, "%.0i", 4200);
    OneTest (__LINE__, "-4200",                    5, "%.0i", -4200);
    OneTest (__LINE__, "10150",                    5, "%.0o", 4200U);
    OneTest (__LINE__, "167630",                   6, "%.0o", -4200U);
    OneTest (__LINE__, "",                         0, "%.0s", "hello, world");
    OneTest (__LINE__, "4200",                     4, "%.0u", 4200U);
    OneTest (__LINE__, "61336",                    5, "%.0u", -4200U);
    OneTest (__LINE__, "1068",                     4, "%.0x", 4200U);
    OneTest (__LINE__, "ef98",                     4, "%.0x", -4200U);
    OneTest (__LINE__, "1068",                     4, "%.0X", 4200U);
    OneTest (__LINE__, "EF98",                     4, "%.0X", -4200U);

    /* Zero precision, dot only */
    OneTest (__LINE__, "u",                        1, "%.c", 'u');
    OneTest (__LINE__, "4200",                     4, "%.d", 4200);
    OneTest (__LINE__, "-4200",                    5, "%.d", -4200);
    OneTest (__LINE__, "4200",                     4, "%.i", 4200);
    OneTest (__LINE__, "-4200",                    5, "%.i", -4200);
    OneTest (__LINE__, "10150",                    5, "%.o", 4200U);
    OneTest (__LINE__, "167630",                   6, "%.o", -4200U);
    OneTest (__LINE__, "",                         0, "%.s", "hello, world");
    OneTest (__LINE__, "4200",                     4, "%.u", 4200U);
    OneTest (__LINE__, "61336",                    5, "%.u", -4200U);
    OneTest (__LINE__, "1068",                     4, "%.x", 4200U);
    OneTest (__LINE__, "ef98",                     4, "%.x", -4200U);
    OneTest (__LINE__, "1068",                     4, "%.X", 4200U);
    OneTest (__LINE__, "EF98",                     4, "%.X", -4200U);

    /* Zero precision, zero value */
    OneTest (__LINE__, "",                         0, "%.0d", 0);
    OneTest (__LINE__, "",                         0, "%.0i", 0);
    OneTest (__LINE__, "",                         0, "%.0o", 0U);
    OneTest (__LINE__, "",                         0, "%.0u", 0U);
    OneTest (__LINE__, "",                         0, "%.0x", 0U);
    OneTest (__LINE__, "",                         0, "%.0X", 0U);

    /* Field width and zero precision given */
    OneTest (__LINE__, "              u",         15, "%15.0c", 'u');
    OneTest (__LINE__, "           4200",         15, "%15.0d", 4200);
    OneTest (__LINE__, "          -4200",         15, "%15.0d", -4200);
    OneTest (__LINE__, "           4200",         15, "%15.0i", 4200);
    OneTest (__LINE__, "          -4200",         15, "%15.0i", -4200);
    OneTest (__LINE__, "          10150",         15, "%15.0o", 4200U);
    OneTest (__LINE__, "         167630",         15, "%15.0o", -4200U);
    OneTest (__LINE__, "               ",         15, "%15.0s", "hello, world");
    OneTest (__LINE__, "           4200",         15, "%15.0u", 4200U);
    OneTest (__LINE__, "          61336",         15, "%15.0u", -4200U);
    OneTest (__LINE__, "           1068",         15, "%15.0x", 4200U);
    OneTest (__LINE__, "           ef98",         15, "%15.0x", -4200U);
    OneTest (__LINE__, "           1068",         15, "%15.0X", 4200U);
    OneTest (__LINE__, "           EF98",         15, "%15.0X", -4200U);

    /* Field width, zero precision and zero value */
    OneTest (__LINE__, "               ",         15, "%15.0d", 0);
    OneTest (__LINE__, "               ",         15, "%15.0i", 0);
    OneTest (__LINE__, "               ",         15, "%15.0o", 0U);
    OneTest (__LINE__, "               ",         15, "%15.0u", 0U);
    OneTest (__LINE__, "               ",         15, "%15.0x", 0U);
    OneTest (__LINE__, "               ",         15, "%15.0X", 0U);

    /* Alternative form */
    OneTest (__LINE__, "010150",                   6, "%#o", 4200U);
    OneTest (__LINE__, "0167630",                  7, "%#o", -4200U);
    OneTest (__LINE__, "0x1068",                   6, "%#x", 4200U);
    OneTest (__LINE__, "0xef98",                   6, "%#x", -4200U);
    OneTest (__LINE__, "0X1068",                   6, "%#X", 4200U);
    OneTest (__LINE__, "0XEF98",                   6, "%#X", -4200U);

    /* Alternative form with zero value */
#ifndef NOCRASH
    OneTest (__LINE__, "0",                        1, "%#o", 0U);
    OneTest (__LINE__, "0",                        1, "%#x", 0U);
    OneTest (__LINE__, "0",                        1, "%#X", 0U);
#endif

    /* Alternative form with zero value and precision 1 */
    OneTest (__LINE__, "0",                        1, "%#.1o", 0U);
    OneTest (__LINE__, "0",                        1, "%#.1x", 0U);
    OneTest (__LINE__, "0",                        1, "%#.1X", 0U);

    /* Alternative form with non zero value and precision 1 */
    OneTest (__LINE__, "01",                       2, "%#.1o", 1U);
    OneTest (__LINE__, "0x1",                      3, "%#.1x", 1U);
    OneTest (__LINE__, "0X1",                      3, "%#.1X", 1U);

    /* Alternative form and width given */
    OneTest (__LINE__, "         010150",         15, "%#15o", 4200U);
    OneTest (__LINE__, "         0x1068",         15, "%#15x", 4200U);
    OneTest (__LINE__, "         0X1068",         15, "%#15X", 4200U);

    /* Alternative form, width and zero padding */
    OneTest (__LINE__, "000000000010150",         15, "%#015o", 4200U);
    OneTest (__LINE__, "0x0000000001068",         15, "%#015x", 4200U);
    OneTest (__LINE__, "0X0000000001068",         15, "%#015X", 4200U);

    /* n conversion specifier */
    WriteTest (__LINE__, "%d%n",        &W, 0x5A5A0004L);
    WriteTest (__LINE__, "%d%hn",       &W, 0x5A5A0004L);
    WriteTest (__LINE__, "%d%hhn",      &W, 0x5A5A5A04L);
    WriteTest (__LINE__, "%d%ln",       &W, 0x00000004L);

    /* Output the result */
    if (Failures) {
        printf ("%u tests, %u failures\n", Tests, Failures);
    } else {
        printf ("%u tests: Ok\n", Tests);
    }

    /* Wait for a key so we can read the result */
#if defined(__CC65__)
    cgetc ();
#endif
    return 0;
}
