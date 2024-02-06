
/* preprocessor test #1 */

#define hash_hash       # ## #
#define mkstr(a)        # a
#define in_between(a)   mkstr(a)
#define join(c, d)      in_between(c hash_hash d)

#define x "first"
#define y "second"

char p[] = join(x, y);          // Comment

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int main(void)
{
    printf("expected: %s\n", "\"first\" ## \"second\"");
    printf("p:        %s\n", p);
    if (!strcmp(p, "\"first\" ## \"second\"")) {
        return EXIT_SUCCESS;
    }
    printf("all fine\n");
    return EXIT_FAILURE;
}
