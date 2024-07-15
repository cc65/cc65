#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void
error (void)
{
    printf ("strtok() test failed!\n");
    exit (EXIT_FAILURE);
}

void
test (char * s)
{
    if (strcmp ("test", strtok (s, "/")))
        error ();
    if (strcmp ("foo", strtok (NULL, "/")))
        error ();
    if (strcmp ("bar", strtok (NULL, "/")))
        error ();
    if (strtok (NULL, "/"))
        error ();
    if (strtok (NULL, "/"))
        error ();
}

int
main (void)
{
    char s1[] = "test/foo/bar";
    char s2[] = "/test/foo/bar";
    char s3[] = "//test/foo/bar";
    char s4[] = "//test/foo/bar//";

    test (s1);
    test (s2);
    test (s3);
    test (s4);

    return EXIT_SUCCESS;
}
