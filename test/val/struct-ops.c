/* Regression tests for struct issues:
** #1102 - Passing structs/unions <= 4 bytes to/from functions
** #2022 - Returning structs by value generates invalid code
** #2566 - Field alignment bug
** #3041 - Structs of size 3/5+ are still not quite correct
**
** TODO: ternary tests; #3040
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int failures = 0;

/* TODO: enable these when they can be compiled; #2086, #2127 */
#if !__CC65__
#   define HAVE_STRUCT_3_PASS       1
#   define HAVE_BIG_STRUCT_PASS     1
#endif

struct sz2p {
    char* p;
};

struct sz3p {
    char z;
    char* p;
};

struct sz4p {
    int z;
    int* p;
};

union uz4p {
    char* p;
    long z;
};

struct sz10p {
    char z[8];
    char* p;
};

struct ofs1sz2 {
    char z;
    struct sz2p s;
};

struct ofs2sz3 {
    int z;
    struct sz3p s;
};

struct ofs2sz4 {
    int z;
    struct sz4p s;
};

struct ofs2uz4 {
    int z;
    union uz4p s;
};

struct ofs2sz10 {
    int z;
    struct sz10p s;
};


/***** Functions passing and returning structs *****/

struct sz2p gsz2p(void)
{
    static char a[5] = { 11, 21, 31, 41, 51 };
    struct sz2p s = { a };
    return s;
}

struct sz2p fsz2p(struct sz2p s)
{
    static char a[5];
    struct sz2p ls;

    /* Copy the input struct contents */
    ls = s;
    memcpy(a, s.p, sizeof(a));
    ls.p = a;
    /* And return the copy */
    return ls;
}

#if HAVE_STRUCT_3_PASS
struct sz3p gsz3p(void)
{
    static char a[5] = { 11, 21, 31, 41, 51 };
    struct sz3p s = { 0, a };
    return s;
}

struct sz3p fsz3p(struct sz3p s)
{
    static char a[5];
    struct sz3p ls;

    /* Copy the input struct contents */
    ls = s;
    memcpy(a, s.p, sizeof(a));
    ls.p = a;
    /* And return the copy */
    return ls;
}
#endif

struct sz4p gsz4p(void)
{
    static int a[5] = { 11, 21, 31, 41, 51 };
    struct sz4p s = { 0, a };
    return s;
}

struct sz4p fsz4p(struct sz4p s)
{
    static int a[5];
    struct sz4p ls;

    /* Copy the input struct contents */
    ls = s;
    memcpy(a, s.p, sizeof(a));
    ls.p = a;
    /* And return the copy */
    return ls;
}

#if HAVE_BIG_STRUCT_PASS
struct sz10p gsz10p(void)
{
    static char a[5] = { 11, 21, 31, 41, 51 };
    struct sz10p s = { {0}, a };
    return s;
}

struct sz10p fsz10p(struct sz10p s)
{
    static char a[5];
    struct sz10p ls;

    /* Copy the input struct contents */
    ls = s;
    memcpy(a, s.p, sizeof(a));
    ls.p = a;
    /* And return the copy */
    return ls;
}
#endif


/***** Checksum functions to check the operation results *****/

void print_ab_ch(char a[5], char b[5])
{
    printf("a[]: { %d, %d, %d, %d, %d }\n", a[0], a[1], a[2], a[3], a[4]);
    printf("b[]: { %d, %d, %d, %d, %d }\n", b[0], b[1], b[2], b[3], b[4]);
}

int sum_arr_ch(char a[5])
{
    int i, sum = 0;

    for (i = 0; i < 5; ++i) {
        sum += (i + 1) * a[i];
    }

    return sum;
}

int sum_ab_ch(char a[5], char b[5])
{
    return sum_arr_ch(a) + sum_arr_ch(b);
}

int sum_arr_int(int a[5])
{
    int i, sum = 0;

    for (i = 0; i < 5; ++i) {
        sum += (i + 1) * a[i];
    }

    return sum;
}

int sum_ab_int(int a[5], int b[5])
{
    return sum_arr_int(a) + sum_arr_int(b);
}


/***** Basic struct operations *****/

int sz2_basic_ops(char x)
{
    static char a[5] = { 11, 21, 31, 41, 51 }; /* end: { 11, 2, 3, 4, 51 } */
    auto   char b[5] = { 15, 25, 35, 45, 55 }; /* end: { 15, 6, 7, 8, 55 } */
    struct sz2p s1, s2, s3, s4;
    static struct sz2p s5, s6;

    s1.p = a;
    (&s2)->p = b;

    /* Modify a[] contents */
    s3.p = s1.p;
    s3.p += 1;
    s3.p[x] = 1;
    s3.p[x]++;
    ++s3.p[x];

    (s5 = s1).p[x + 2] = 1;
    s5.p++[3] += 3;
    *(s3 = s5, s3.p--) = 1;
    s3.p = s3.p + 2;
    s3.p[x - 2]++;

    /* Modify b[] contents */
    s4 = s6 = s2;
    *((&s4)->p++ + 2) = s1.p[1] + 5;
    (s6.p = (&s4)->p)[0] = *s3.p;
    *s6.p = *s6.p + 3;
    (s6.p += 3, s6.p)[-1] = *(s5.p + 2);
    s6.p[-1] += 4;

    print_ab_ch(a, b);

    /* Check the result */
    return sum_ab_ch(a, b);
}

int sz3_basic_ops(char x)
{
    static char a[5] = { 11, 21, 31, 41, 51 }; /* end: { 11, 2, 3, 4, 51 } */
    auto   char b[5] = { 15, 25, 35, 45, 55 }; /* end: { 15, 6, 7, 8, 55 } */
    struct sz3p s1, s2, s3, s4;
    static struct sz3p s5, s6;

    s1.p = a;
    (&s2)->p = b;

    /* Modify a[] contents */
    s3.p = s1.p;
    s3.p += 1;
    s3.p[x] = 1;
    s3.p[x]++;
    ++s3.p[x];

    (s5 = s1).p[x + 2] = 1;
    s5.p++[3] += 3;
    *(s3 = s5, s3.p--) = 1;
    s3.p = s3.p + 2;
    s3.p[x - 2]++;

    /* Modify b[] contents */
    s4 = s6 = s2;
    *((&s4)->p++ + 2) = s1.p[1] + 5;
    (s6.p = (&s4)->p)[0] = *s3.p;
    *s6.p = *s6.p + 3;
    (s6.p += 3, s6.p)[-1] = *(s5.p + 2);
    s6.p[-1] += 4;

    /* Check the result */
    return sum_ab_ch(a, b);
}

int sz4_basic_ops(int x)
{
    static int a[5] = { 11, 21, 31, 41, 51 }; /* end: { 11, 2, 3, 4, 51 } */
    auto   int b[5] = { 15, 25, 35, 45, 55 }; /* end: { 15, 6, 7, 8, 55 } */
    struct sz4p s1, s2, s3, s4;
    static struct sz4p s5, s6;

    s1.p = a;
    (&s2)->p = b;

    /* Modify a[] contents */
    s3.p = s1.p;
    s3.p += 1;
    s3.p[x] = 1;
    s3.p[x]++;
    ++s3.p[x];

    (s5 = s1).p[x + 2] = 1;
    s5.p++[3] += 3;
    *(s3 = s5, s3.p--) = 1;
    s3.p = s3.p + 2;
    s3.p[x - 2]++;

    /* Modify b[] contents */
    s4 = s6 = s2;
    *((&s4)->p++ + 2) = s1.p[1] + 5;
    (s6.p = (&s4)->p)[0] = *s3.p;
    *s6.p = *s6.p + 3;
    (s6.p += 3, s6.p)[-1] = *(s5.p + 2);
    s6.p[-1] += 4;

    /* Check the result */
    return sum_ab_int(a, b);
}

int uz4_basic_ops(char x)
{
    static char a[5] = { 11, 21, 31, 41, 51 }; /* end: { 11, 2, 3, 4, 51 } */
    auto   char b[5] = { 15, 25, 35, 45, 55 }; /* end: { 15, 6, 7, 8, 55 } */
    union uz4p s1, s2, s3, s4;
    static union uz4p s5, s6;

    s1.p = a;
    (&s2)->p = b;

    /* Modify a[] contents */
    s3.p = s1.p;
    s3.p += 1;
    s3.p[x] = 1;
    s3.p[x]++;
    ++s3.p[x];

    (s5 = s1).p[x + 2] = 1;
    s5.p++[3] += 3;
    *(s3 = s5, s3.p--) = 1;
    s3.p = s3.p + 2;
    s3.p[x - 2]++;

    /* Modify b[] contents */
    s4 = s6 = s2;
    *((&s4)->p++ + 2) = s1.p[1] + 5;
    (s6.p = (&s4)->p)[0] = *s3.p;
    *s6.p = *s6.p + 3;
    (s6.p += 3, s6.p)[-1] = *(s5.p + 2);
    s6.p[-1] += 4;

    /* Check the result */
    return sum_ab_ch(a, b);
}

int sz10_basic_ops(char x)
{
    static char a[5] = { 11, 21, 31, 41, 51 }; /* end: { 11, 2, 3, 4, 51 } */
    auto   char b[5] = { 15, 25, 35, 45, 55 }; /* end: { 15, 6, 7, 8, 55 } */
    struct sz10p s1, s2, s3, s4;
    static struct sz10p s5, s6;

    s1.p = a;
    (&s2)->p = b;

    /* Modify a[] contents */
    s3.p = s1.p;
    s3.p += 1;
    s3.p[x] = 1;
    s3.p[x]++;
    ++s3.p[x];

    (s5 = s1).p[x + 2] = 1;
    s5.p++[3] += 3;
    *(s3 = s5, s3.p--) = 1;
    s3.p = s3.p + 2;
    s3.p[x - 2]++;

    /* Modify b[] contents */
    s4 = s6 = s2;
    *((&s4)->p++ + 2) = s1.p[1] + 5;
    (s6.p = (&s4)->p)[0] = *s3.p;
    *s6.p = *s6.p + 3;
    (s6.p += 3, s6.p)[-1] = *(s5.p + 2);
    s6.p[-1] += 4;

    /* Check the result */
    return sum_ab_ch(a, b);
}


/***** Function struct pass and return operations *****/

int sz2_func_ops(char x)
{
    /* gsz(): a[5] = { 11, 21, 31, 41, 51 };    end: { 11, 2, 3, 4, 51 } */
    auto char b[5] = { 15, 25, 35, 45, 55 }; /* end: { 15, 6, 7, 8, 55 } */
    struct sz2p s1, s2, s3, s4;

    /* Modify a[] contents */
    s1 = gsz2p();
    (s2.p = gsz2p().p)[2] = 3;
    s1.p[1] = (s1.p == s2.p) + 1;
    *(gsz2p().p + x + 2) = 4;

    /* Modify b[] contents */
    s3.p = b;
    (&s3)->p[1] = (s1.p++, gsz2p().p[0] - 5);
    s3.p[x + 1] = 7;
    (s4 = fsz2p(s3), ++s4.p)[2] = (fsz2p(s3).p != s3.p) + 7;
    --s4.p;

    /* Check the result */
    return sum_ab_ch(s2.p, s4.p);
}

#if HAVE_STRUCT_3_PASS
int sz3_func_ops(char x)
{
    /* gsz(): a[5] = { 11, 21, 31, 41, 51 };    end: { 11, 2, 3, 4, 51 } */
    auto char b[5] = { 15, 25, 35, 45, 55 }; /* end: { 15, 6, 7, 8, 55 } */
    struct sz3p s1, s2, s3, s4;

    /* Modify a[] contents */
    s1 = gsz3p();
    (s2.p = gsz3p().p)[2] = 3;
    s1.p[1] = (s1.p == s2.p) + 1;
    *(gsz3p().p + x + 2) = 4;

    /* Modify b[] contents */
    s3.p = b;
    (&s3)->p[1] = (s1.p++, gsz3p().p[0] - 5);
    s3.p[x + 1] = 7;
    (s4 = fsz3p(s3), ++s4.p)[2] = (fsz3p(s3).p != s3.p) + 7;
    --s4.p;

    /* Check the result */
    return sum_ab_ch(s2.p, s4.p);
}
#endif

int sz4_func_ops(int x)
{
    /* gsz():a[5] = { 11, 21, 31, 41, 51 };    end: { 11, 2, 3, 4, 51 } */
    auto int b[5] = { 15, 25, 35, 45, 55 }; /* end: { 15, 6, 7, 8, 55 } */
    struct sz4p s1, s2, s3, s4;

    /* Modify a[] contents */
    s1 = gsz4p();
    (s2.p = gsz4p().p)[2] = 3;
    s1.p[1] = (s1.p == s2.p) + 1;
    *(gsz4p().p + x + 2) = 4;

    /* Modify b[] contents */
    s3.p = b;
    (&s3)->p[1] = (s1.p++, gsz4p().p[0] - 5);
    s3.p[x + 1] = 7;
    (s4 = fsz4p(s3), ++s4.p)[2] = (fsz4p(s3).p != s3.p) + 7;
    --s4.p;

    /* Check the result */
    return sum_ab_int(s2.p, s4.p);
}

#if HAVE_BIG_STRUCT_PASS
int sz10_func_ops(char x)
{
    /* gsz(): a[5] = { 11, 21, 31, 41, 51 };    end: { 11, 2, 3, 4, 51 } */
    auto char b[5] = { 15, 25, 35, 45, 55 }; /* end: { 15, 6, 7, 8, 55 } */
    struct sz10p s1, s2, s3, s4;

    /* Modify a[] contents */
    s1 = gsz10p();
    (s2.p = gsz10p().p)[2] = 3;
    s1.p[1] = (s1.p == s2.p) + 1;
    *(gsz10p().p + x + 2) = 4;

    /* Modify b[] contents */
    s3.p = b;
    (&s3)->p[1] = (s1.p++, gsz10p().p[0] - 5);
    s3.p[x + 1] = 7;
    (s4 = fsz10p(s3), ++s4.p)[2] = (fsz10p(s3).p != s3.p) + 7;
    --s4.p;

    /* Check the result */
    return sum_ab_ch(s2.p, s4.p);
}
#endif


/***** Struct reference with offset operations *****/

int sz2_ref_offset(int x)
{
    static char a[5] = { 11, 21, 31, 41, 51 }; /* end: { 11, 2, 3, 4, 51 } */
    auto   char b[5] = { 15, 25, 35, 45, 55 }; /* end: { 15, 6, 7, 8, 55 } */
    struct ofs1sz2 s1 = { 0, { a } }, s2 = { 0 }, s3, s4;
    struct ofs1sz2 *p1 = &s1, *p3 = &s3;
    static struct ofs1sz2 *p2, *p4;

    /* p# points to s# throughout */
    p2 = &s2;
    p4 = &s4;
    p2->s.p = b;

    /* Modify a[] contents */
    s3 = *p1;
    (p4->s = p3->s).p[x] = 2;
    p4->s.p++[x + 1] = 1;
    p4->s.p[x++] += 2;
    *((p3->s = p4->s, p3->s.p) + x) = *s4.s.p + 2;

    /* Modify b[] contents */
    p3->s.p = (*p4 = *p2, p4->s.p);
    *(++(&p3->s)->p) = 1;
    s3.s.p[0] += 5;
    *(2 + p3->s.p++) = 7;
    ++(++s3.s.p)[0];
    p3->s.p[-1] = 7;

    /* Check the result */
    return sum_ab_ch(a, b);
}

int sz3_ref_offset(int x)
{
    static char a[5] = { 11, 21, 31, 41, 51 }; /* end: { 11, 2, 3, 4, 51 } */
    auto   char b[5] = { 15, 25, 35, 45, 55 }; /* end: { 15, 6, 7, 8, 55 } */
    struct ofs2sz3 s1 = { 0, { 0, a } }, s2 = { 0 }, s3, s4;
    struct ofs2sz3 *p1 = &s1, *p3 = &s3;
    static struct ofs2sz3 *p2, *p4;

    /* p# points to s# throughout */
    p2 = &s2;
    p4 = &s4;
    p2->s.p = b;

    /* Modify a[] contents */
    s3 = *p1;
    (p4->s = p3->s).p[x] = 2;
    p4->s.p++[x + 1] = 1;
    p4->s.p[x++] += 2;
    *((p3->s = p4->s, p3->s.p) + x) = *s4.s.p + 2;

    /* Modify b[] contents */
    p3->s.p = (*p4 = *p2, p4->s.p);
    *(++(&p3->s)->p) = 1;
    s3.s.p[0] += 5;
    *(2 + p3->s.p++) = 7;
    ++(++s3.s.p)[0];
    p3->s.p[-1] = 7;

    /* Check the result */
    return sum_ab_ch(a, b);
}

int sz4_ref_offset(char x)
{
    static int a[5] = { 11, 21, 31, 41, 51 }; /* end: { 11, 2, 3, 4, 51 } */
    auto   int b[5] = { 15, 25, 35, 45, 55 }; /* end: { 15, 6, 7, 8, 55 } */
    struct ofs2sz4 s1 = { 0, { 0, a } }, s2 = { 0 }, s3, s4;
    struct ofs2sz4 *p1 = &s1, *p3 = &s3;
    static struct ofs2sz4 *p2, *p4;

    /* p# points to s# throughout */
    p2 = &s2;
    p4 = &s4;
    p2->s.p = b;

    /* Modify a[] contents */
    s3 = *p1;
    (p4->s = p3->s).p[x] = 2;
    p4->s.p++[x + 1] = 1;
    p4->s.p[x++] += 2;
    *((p3->s = p4->s, p3->s.p) + x) = *s4.s.p + 2;

    /* Modify b[] contents */
    p3->s.p = (*p4 = *p2, p4->s.p);
    *(++(&p3->s)->p) = 1;
    s3.s.p[0] += 5;
    *(2 + p3->s.p++) = 7;
    ++(++s3.s.p)[0];
    p3->s.p[-1] = 7;

    /* Check the result */
    return sum_ab_int(a, b);
}

int uz4_ref_offset(int x)
{
    static char a[5] = { 11, 21, 31, 41, 51 }; /* end: { 11, 2, 3, 4, 51 } */
    auto   char b[5] = { 15, 25, 35, 45, 55 }; /* end: { 15, 6, 7, 8, 55 } */
    struct ofs2uz4 s1 = { 0, { a } }, s2 = { 0 }, s3, s4;
    struct ofs2uz4 *p1 = &s1, *p3 = &s3;
    static struct ofs2uz4 *p2, *p4;

    /* p# points to s# throughout */
    p2 = &s2;
    p4 = &s4;
    p2->s.p = b;

    /* Modify a[] contents */
    s3 = *p1;
    (p4->s = p3->s).p[x] = 2;
    p4->s.p++[x + 1] = 1;
    p4->s.p[x++] += 2;
    *((p3->s = p4->s, p3->s.p) + x) = *s4.s.p + 2;

    /* Modify b[] contents */
    p3->s.p = (*p4 = *p2, p4->s.p);
    *(++(&p3->s)->p) = 1;
    s3.s.p[0] += 5;
    *(2 + p3->s.p++) = 7;
    ++(++s3.s.p)[0];
    p3->s.p[-1] = 7;

    /* Check the result */
    return sum_ab_ch(a, b);
}

int sz10_ref_offset(int x)
{
    static char a[5] = { 11, 21, 31, 41, 51 }; /* end: { 11, 2, 3, 4, 51 } */
    auto   char b[5] = { 15, 25, 35, 45, 55 }; /* end: { 15, 6, 7, 8, 55 } */
    struct ofs2sz10 s1 = { 0, { {0}, a } }, s2 = { 0 }, s3, s4;
    struct ofs2sz10 *p1 = &s1, *p3 = &s3;
    static struct ofs2sz10 *p2, *p4;

    /* p# points to s# throughout */
    p2 = &s2;
    p4 = &s4;
    p2->s.p = b;

    /* Modify a[] contents */
    s3 = *p1;
    (p4->s = p3->s).p[x] = 2;
    p4->s.p++[x + 1] = 1;
    p4->s.p[x++] += 2;
    *((p3->s = p4->s, p3->s.p) + x) = *s4.s.p + 2;

    /* Modify b[] contents */
    p3->s.p = (*p4 = *p2, p4->s.p);
    *(++(&p3->s)->p) = 1;
    s3.s.p[0] += 5;
    *(2 + p3->s.p++) = 7;
    ++(++s3.s.p)[0];
    p3->s.p[-1] = 7;

    /* Check the result */
    return sum_ab_ch(a, b);
}


/***** Arrays of structs operations *****/

int sz2_array_ops(char x)
{
    static char a[5] = { 11, 21, 31, 41, 51 }; /* end: { 11, 2, 3, 4, 51 } */
    auto   char b[5] = { 15, 25, 35, 45, 55 }; /* end: { 15, 6, 7, 8, 55 } */
    struct ofs1sz2 s[5] = { { 0 }, { 0, { a } }, { 0 } };
    static struct ofs1sz2* p[5] = { 0 };
    unsigned char i;

    i = 1;
    /* p[] points to s[] throughout */
    p[1] = &s[i], i++;
    p[i++] = &s[2];
    p[3] = (++i, &s[i - 1]);
    p[i] = &s[i];
    p[2]->s.p = b;

    /* Modify a[] contents */
    i = 3;
    s[i] = *p[1];
    (p[i++ + 1]->s = p[3]->s).p[x] = 2;
    p[i]->s.p++[x + 1] = 1;
    p[4]->s.p[x++] += 2;
    *((p[3]->s = p[4]->s, p[i-- - 1]->s.p) + x) = *s[4].s.p + 2;

    /* Modify b[] contents */
    /* i = 3 */
    p[3]->s.p = (*p[i + 1] = *p[2], p[4]->s.p);
    *(++(&p[i]->s)->p) = 1;
    s[i++].s.p[0] += 5;
    *(2 + p[3]->s.p++) = 7;
    ++(++s[3].s.p)[0];
    p[--i]->s.p[-1] = 7;

    /* Check the result */
    return sum_ab_ch(a, b);
}

int sz3_array_ops(char x)
{
    static char a[5] = { 11, 21, 31, 41, 51 }; /* end: { 11, 2, 3, 4, 51 } */
    auto   char b[5] = { 15, 25, 35, 45, 55 }; /* end: { 15, 6, 7, 8, 55 } */
    struct ofs2sz3 s[5] = { { 0 }, { 0, { 0, a } }, { 0 } };
    static struct ofs2sz3* p[5] = { 0 };
    unsigned char i;

    i = 1;
    /* p[] points to s[] throughout */
    p[1] = &s[i], i++;
    p[i++] = &s[2];
    p[3] = (++i, &s[i - 1]);
    p[i] = &s[i];
    p[2]->s.p = b;

    /* Modify a[] contents */
    i = 3;
    s[i] = *p[1];
    (p[i++ + 1]->s = p[3]->s).p[x] = 2;
    p[i]->s.p++[x + 1] = 1;
    p[4]->s.p[x++] += 2;
    *((p[3]->s = p[4]->s, p[i-- - 1]->s.p) + x) = *s[4].s.p + 2;

    /* Modify b[] contents */
    /* i = 3 */
    p[3]->s.p = (*p[i + 1] = *p[2], p[4]->s.p);
    *(++(&p[i]->s)->p) = 1;
    s[i++].s.p[0] += 5;
    *(2 + p[3]->s.p++) = 7;
    ++(++s[3].s.p)[0];
    p[--i]->s.p[-1] = 7;

    /* Check the result */
    return sum_ab_ch(a, b);
}

int sz4_array_ops(int x)
{
    static int a[5] = { 11, 21, 31, 41, 51 }; /* end: { 11, 2, 3, 4, 51 } */
    auto   int b[5] = { 15, 25, 35, 45, 55 }; /* end: { 15, 6, 7, 8, 55 } */
    struct ofs2sz4 s[5] = { { 0 }, { 0, { 0, a } }, { 0 } };
    static struct ofs2sz4* p[5] = { 0 };
    int i;

    i = 1;
    /* p[] points to s[] throughout */
    p[1] = &s[i], i++;
    p[i++] = &s[2];
    p[3] = (++i, &s[i - 1]);
    p[i] = &s[i];
    p[2]->s.p = b;

    /* Modify a[] contents */
    i = 3;
    s[i] = *p[1];
    (p[i++ + 1]->s = p[3]->s).p[x] = 2;
    p[i]->s.p++[x + 1] = 1;
    p[4]->s.p[x++] += 2;
    *((p[3]->s = p[4]->s, p[i-- - 1]->s.p) + x) = *s[4].s.p + 2;

    /* Modify b[] contents */
    /* i = 3 */
    p[3]->s.p = (*p[i + 1] = *p[2], p[4]->s.p);
    *(++(&p[i]->s)->p) = 1;
    s[i++].s.p[0] += 5;
    *(2 + p[3]->s.p++) = 7;
    ++(++s[3].s.p)[0];
    p[--i]->s.p[-1] = 7;

    /* Check the result */
    return sum_ab_int(a, b);
}

int sz10_array_ops(int x)
{
    static char a[5] = { 11, 21, 31, 41, 51 }; /* end: { 11, 2, 3, 4, 51 } */
    auto   char b[5] = { 15, 25, 35, 45, 55 }; /* end: { 15, 6, 7, 8, 55 } */
    struct ofs2sz10 s[5] = { { 0 }, { 0, { {0}, a } }, { 0 } };
    static struct ofs2sz10* p[5] = { 0 };
    unsigned char i;

    i = 1;
    /* p[] points to s[] throughout */
    p[1] = &s[i], i++;
    p[i++] = &s[2];
    p[3] = (++i, &s[i - 1]);
    p[i] = &s[i];
    p[2]->s.p = b;

    /* Modify a[] contents */
    i = 3;
    s[i] = *p[1];
    (p[i++ + 1]->s = p[3]->s).p[x] = 2;
    p[i]->s.p++[x + 1] = 1;
    p[4]->s.p[x++] += 2;
    *((p[3]->s = p[4]->s, p[i-- - 1]->s.p) + x) = *s[4].s.p + 2;

    /* Modify b[] contents */
    /* i = 3 */
    p[3]->s.p = (*p[i + 1] = *p[2], p[4]->s.p);
    *(++(&p[i]->s)->p) = 1;
    s[i++].s.p[0] += 5;
    *(2 + p[3]->s.p++) = 7;
    ++(++s[3].s.p)[0];
    p[--i]->s.p[-1] = 7;

    /* Check the result */
    return sum_ab_ch(a, b);
}


#define CHECK(func, expect) \
    res = (func); \
    if (res != (expect)) { \
        ++failures; \
        printf("%s: Got %d, expected %d\n", #func, res, (expect)); \
    }

int main(void)
{
    int res;

    /* Note: Test expression complexity escalates gradually and
    **   redability decreases accordingly.
    */
    CHECK(sz2_basic_ops(1), 650);
    CHECK(sz3_basic_ops(1), 650);
    CHECK(sz4_basic_ops(1), 650);
    CHECK(uz4_basic_ops(1), 650);
    CHECK(sz10_basic_ops(1), 650);

    CHECK(sz2_func_ops(1), 650);
#if HAVE_STRUCT_3_PASS
    CHECK(sz3_func_ops(1), 650);
#endif
    CHECK(sz4_func_ops(1), 650);
#if HAVE_BIG_STRUCT_PASS
    CHECK(sz10_func_ops(1), 650);
#endif

    CHECK(sz2_ref_offset(1), 650);
    CHECK(sz3_ref_offset(1), 650);
    CHECK(sz4_ref_offset(1), 650);
    CHECK(uz4_ref_offset(1), 650);
    CHECK(sz10_ref_offset(1), 650);

    /* Caution: These tests are unreadable, and they are not meant to be
    **   such. They are produced as mechanical copies of xxx_ref_offset()
    **   with array subscript substitution, and several [i] subscript
    **   tweaks. Then verified with other compilers (GCC, Clang, MSVC).
    */
    CHECK(sz2_array_ops(1), 650);
    CHECK(sz3_array_ops(1), 650);
    CHECK(sz4_array_ops(1), 650);
    CHECK(sz10_array_ops(1), 650);

    printf("failures: %d\n", failures);

    return failures ? EXIT_FAILURE : EXIT_SUCCESS;
}
