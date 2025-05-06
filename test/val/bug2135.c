/* Bug #2135 - Compound initialization consumes wrong amount of initializers with omitted
**             enclosing curly braces when an array/struct/union to initialize is itself
**             a member/element of a struct/union/array.
*/

#include <stdint.h>
#include <stdio.h>

struct s {
    union {
        int8_t a[2][2];
        char c[sizeof (int8_t) * 2 * 2 + sizeof (int16_t) * 4];
    };
    int16_t b[4];
};
struct s x = { 1, 2, 3, 4, 5, 6 };
struct s y = { {{{1, 2}, {3, 4}}}, {5, 6} };

unsigned failures;

int main(void)
{
    unsigned i, j;

    for (i = 0; i < 2; ++i)
    {
        for (j = 0; j < 2; ++j)
        {
            if (x.a[i][j] != y.a[i][j])
            {
                ++failures;
                printf("x.a[%u][%u] = %d\n, expected %d\n", i, j, x.a[i][j], y.a[i][j]);
            }
        }
    }

    for (i = 0; i < 4; ++i)
    {
        if (x.b[i] != y.b[i])
        {
            ++failures;
            printf("x.b[%u] = %d\n, expected %d\n", i, x.b[i], y.b[i]);
        }
    }

    return failures;
}
