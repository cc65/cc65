/* Bug #2016 and #2017 - flexible array members */

typedef struct {
    int a;
    int b[];        /* Ok: Flexible array member can be last */
} X;

typedef union {
    X   x;          /* Ok: Contains flexible array member */
    int a;
} U;

typedef struct {
    struct {
        int a;
    };
    int b[];    /* Ok: Flexible array member can be last */
} Y;

X x;
U u;
Y y;

_Static_assert(sizeof x == sizeof (int), "sizeof x != sizeof (int)");
_Static_assert(sizeof u == sizeof (int), "sizeof u != sizeof (int)");
_Static_assert(sizeof y == sizeof (int), "sizeof y != sizeof (int)");

int main(void)
{
    return 0;
}
