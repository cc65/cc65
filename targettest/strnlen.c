#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char * str = "0123456789";

void
check (size_t result, size_t expected)
{
    if (result != expected) {
        printf ("Expected strnlen() to return %u, got %u.\n",
                expected, result);
        exit (EXIT_FAILURE);
    }
}

int
main (void)
{
    size_t  maxlen = strlen (str);
    size_t  result;
    size_t  expected;

    for (expected = 0; expected < maxlen; expected++)
        check (strnlen (str, expected), expected);

    check (strnlen (str, maxlen << 1), maxlen);

    printf ("strnlen() OK.\n");

    return EXIT_SUCCESS;
}
