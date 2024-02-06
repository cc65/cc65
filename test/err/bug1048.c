/* bug #1048: The following code has two errors: a redeclared enum type and an
   undeclared enum type: */

#include <stdlib.h>

// this should NOT compile - but with cc65 it does
enum e { x };
enum e { y };

int f() { return sizeof(enum undeclared); }

int main(void)
{
    return EXIT_SUCCESS;
}
