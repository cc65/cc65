// 2024-07-15 Sven Michael Klose <pixel@hugbox.org>

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define STR_SHORT   "Hello, World!"
#define STR_LONG    "This is a longer test string for stpcpy."

int
main ()
{
    char dest[50];
    const char *src_empty;
    const char *src_short;
    const char *src_long;
    char       *end;

    src_empty = "";
    end = stpcpy (dest, src_empty);
    assert(!strcmp (dest, src_empty));
    assert(!*end);
    assert(end == dest);
    printf ("Test 1 passed.\n");

    src_short = STR_SHORT;
    end = stpcpy (dest, src_short);
    assert(!strcmp (dest, src_short));
    assert(!*end);
    assert(end == &dest[sizeof (STR_SHORT) - 1]);
    printf ("Test 2 passed.\n");

    src_long = STR_LONG;
    end = stpcpy (dest, src_long);
    assert(!strcmp (dest, src_long));
    assert(!*end);
    assert(end == &dest[sizeof (STR_LONG) - 1]);
    printf ("Test 3 passed.\n");

    printf ("All tests passed.\n");
    return EXIT_SUCCESS;
}

