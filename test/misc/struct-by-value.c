/* This test ensures that compilation fails if a 3-byte struct by value
** is attempted, to avoid re-introducting a bug by accident:
**     https://github.com/cc65/cc65/issues/2022
** When 3-byte structs are re-enabled, this test will compile,
** which should trigger a "misc" test failure.
** When this happens:
**   Delete this comment from the top.
**   Replace test/val/struct-by-value.c with this one.
** See:
**     https://github.com/cc65/cc65/issues/2086
*/

/* Test of passing and returning structs by value.
   Structs of 1, 2, 3, 4 bytes are supported.
   Note that structs of 3 bytes had a past issue:
       https://github.com/cc65/cc65/issues/2022
*/

int fail = 0;

struct s1 { char a; };
struct s2 { char a, b; };
struct s3 { char a, b, c; };
struct s4 { char a, b, c, d; };

const struct s1 c1 = { 1 };
const struct s2 c2 = { 2, 3 };
const struct s3 c3 = { 4, 5, 6 };
const struct s4 c4 = { 7, 8, 9, 10 };

struct s1 return1() { return c1; }
struct s2 return2() { return c2; }
struct s3 return3() { return c3; }
struct s4 return4() { return c4; }

int compare1(struct s1 a, struct s1 b)
{
    if (a.a != b.a) return 1;
    return 0;
}

int compare2(struct s2 a, struct s2 b)
{
    if (a.a != b.a) return 1;
    if (a.b != b.b) return 1;
    return 0;
}

int compare3(struct s3 a, struct s3 b)
{
    if (a.a != b.a) return 1;
    if (a.b != b.b) return 1;
    if (a.c != b.c) return 1;
    return 0;
}

int compare4(struct s4 a, struct s4 b)
{
    if (a.a != b.a) return 1;
    if (a.b != b.b) return 1;
    if (a.c != b.c) return 1;
    if (a.d != b.d) return 1;
    return 0;
}

int pass1(struct s1 p1)
{
    struct s1 a1;
    a1 = p1;
    if (a1.a != c1.a) return 1;
    return 0;
}

int pass2(struct s2 p2)
{
    struct s2 a2;
    a2 = p2;
    if (a2.a != c2.a) return 1;
    if (a2.b != c2.b) return 1;
    return 0;
}

int pass3(struct s3 p3)
{
    struct s3 a3;
    a3 = p3;
    if (a3.a != c3.a) return 1;
    if (a3.b != c3.b) return 1;
    if (a3.c != c3.c) return 1;
    return 0;
}

int pass4(struct s4 p4)
{
    struct s4 a4;
    a4 = p4;
    if (a4.a != c4.a) return 1;
    if (a4.b != c4.b) return 1;
    if (a4.c != c4.c) return 1;
    if (a4.d != c4.d) return 1;
    return 0;
}

void reset(char* gg)
{
    char i;
    for (i=0;i<5;++i) gg[i] = 128+i;
}

int test(char* gg, char start)
{
    char i;
    for (i=start;i<5;++i)
        if (gg[i] != 128+i) return 1;
    return 0;
}

int main()
{
    /* Used to check #2022 bug condition of extra bytes being overwritten. */
    union
    {
        char gg[5];
        struct s1 g1;
        struct s2 g2;
        struct s3 g3;
        struct s4 g4;
    } guard;

    reset(guard.gg);
    guard.g1 = return1();
    fail += compare1(guard.g1,c1);
    fail += test(guard.gg,1);

    reset(guard.gg);
    guard.g2 = return2();
    fail += compare2(guard.g2,c2);
    fail += test(guard.gg,2);

    reset(guard.gg);
    guard.g3 = return3();
    fail += compare3(guard.g3,c3);
    fail += test(guard.gg,3);

    reset(guard.gg);
    guard.g4 = return4();
    fail += compare4(guard.g4,c4);
    fail += test(guard.gg,4);

    fail += pass1(c1);
    fail += pass2(c2);
    fail += pass3(c3);
    fail += pass4(c4);

    return fail;
}
