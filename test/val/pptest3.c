
/* preprocessor test #3 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

char *x1 = "123";
char *x2 = "456";
FILE *s;
char *str = "789";

#define str(s)          # s
#define xstr(s)         str(s)
#define debug(s, t)     printf("x" # s "= %d, x" # t "= %s", \
                               x ## s, x ## t)
#define INCFILE(n)      vers ## n       // Comment
#define glue(a,b)       a ## b
#define xglue(a,b)      glue(a,b)
#define HIGHLOW         "hello"
#define LOW             LOW ", world"

int main(void) {
    s = stdout;

    debug (1, 2);

    fputs (str (strncmp("abc\0d", "abc", '\4') // Comment
                == 0) str (: @\n), s);

    str = glue (HIGH, LOW);
    printf("str: %s\n", str);
    if (strcmp(str, "hello") != 0) {
        return EXIT_FAILURE;
    }

    str = xglue (HIGH, LOW);
    printf("str: %s\n", str);
    if (strcmp(str, "hello, world") != 0) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
