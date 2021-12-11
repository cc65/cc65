
/* bug 1562: cc65 generates incorrect code for logical expression with -O */

#include <stdio.h>
#include <string.h>

int failures = 0;

char input[256];

#define DEBUGTRUE(x) printf("%s=%d\n", #x, (x)); failures += (x) ? 0 : 1

#define DEBUGFALSE(x) printf("%s=%d\n", #x, (x)); failures += (x) ? 1 : 0

int main(void) {
    char* r;
    strcpy(input, "\"XYZ\"");
    r = input+4;
    DEBUGFALSE(*r != '"');       // = false
    DEBUGTRUE(*r == '"');       // = true
    DEBUGFALSE(*(r+1) == '"');   // = false
    // Next answer should be false because
    // (false || true && false) is false, but it is true with -O.
    DEBUGFALSE(*r != '"' || *r == '"' && *(r+1) == '"');
    // Adding parens fixes it even with -O.
    DEBUGFALSE(*r != '"' || (*r == '"' && *(r+1) == '"'));

    printf("failures: %d\n", failures);
    return failures;
}
