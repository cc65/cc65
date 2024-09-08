// 2024-07-15 Sven Michael Klose <pixel@hugbox.org>

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define STR_SHORT   "Hello, World!"
#define STR_LONG    "This is a longer test string for stpcpy."

char dest[512];
char multi_page[300];

int
main ()
{
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

    memset(multi_page, 'a', sizeof(multi_page)-1);
    multi_page[sizeof(multi_page)-1] = '\0';
    end = stpcpy (dest, multi_page);
    assert(!strcmp (dest, multi_page));
    assert(!*end);
    assert(end == &dest[sizeof (multi_page) - 1]);
    printf ("Test 4 passed.\n");

    printf ("All tests passed.\n");
    return EXIT_SUCCESS;
}
