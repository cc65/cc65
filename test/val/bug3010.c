/* bug #3010: Incorrect signed char promotion in add with literal expressions.
**   const int + signed char
**   const int + unsigned char
**   const int + signed enum
**   const int + unsigned enum
**   array[negative subscript]
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int failures = 0;

/* Enum fits into unsigned char */
enum uche {
    uche_0 = 0,
    uche_1 = 1,
    uche_88 = 88,
    uche_127 = 127,
    uche_186 = 186,
    uche_255 = 255,
};

/* Enum fits into signed char */
enum sche {
    sche_m128 = -128,
    sche_m88 = -88,
    sche_m1 = -1,
    sche_88 = 88,
    sche_127 = 127,
};

/* Test arrays with values -128..255 */
struct uarray {
    unsigned char tripwire1[128];
    unsigned char non[256];
    unsigned char tripwire2[128];
};
_Static_assert(sizeof(struct uarray) == 512, "struct uarray is not aligned");

struct sarray {
    signed char tripwire1[128];
    signed char neg[128];
    /* NOTE: The alignment is not guaranteed by the C Standard */
    signed char pos[128];
    signed char tripwire2[128];
};
_Static_assert(sizeof(struct sarray) == 512, "struct sarray is not aligned");

/* Temp static locations for tests */
signed char s_schar;
unsigned char s_uchar;
char s_char;
enum uche s_uenum;
enum sche s_senum;

struct sarray s_sarr;
struct uarray s_uarr;


#define FUNC_LOCAL_ADD_LIT(type, typename, literal) \
    int typename##_loc_add_lit_##literal(type x) \
    { \
        return x + 0x##literal; \
    }

#define FUNC_LIT_ADD_LOCAL(type, typename, literal) \
    int lit_##literal##_add_##typename##_loc(type x) \
    { \
        return 0x##literal + x; \
    }

#define FUNC_STATIC_ADD_LIT(type, typename, literal) \
    int typename##_stat_add_lit_##literal(type x) \
    { \
        s_##typename = x; \
        return s_##typename + 0x##literal; \
    }

#define FUNC_LIT_ADD_STATIC(type, typename, literal) \
    int lit_##literal##_add_##typename##_stat(type x) \
    { \
        s_##typename = x; \
        return 0x##literal + s_##typename; \
    }

#define FUNC_4WAY_LIT(type, typename, literal) \
    FUNC_LOCAL_ADD_LIT(type, typename, literal); \
    FUNC_LIT_ADD_LOCAL(type, typename, literal); \
    FUNC_STATIC_ADD_LIT(type, typename, literal); \
    FUNC_LIT_ADD_STATIC(type, typename, literal);

#define FUNC_4WAY_4LIT(type, typename, lit1, lit2, lit3, lit4) \
    FUNC_4WAY_LIT(type, typename, lit1); \
    FUNC_4WAY_LIT(type, typename, lit2); \
    FUNC_4WAY_LIT(type, typename, lit3); \
    FUNC_4WAY_LIT(type, typename, lit4);


FUNC_4WAY_4LIT(signed char, schar, ff, 100, 101, ff80);
FUNC_4WAY_4LIT(unsigned char, uchar, ff, 100, 101, ff80);
FUNC_4WAY_4LIT(char, char, ff, 100, 101, ff80);
FUNC_4WAY_4LIT(enum sche, senum, ff, 100, 101, ff80);
FUNC_4WAY_4LIT(enum uche, uenum, ff, 100, 101, ff80);


/* (array + char) expressions use the same codegen path */
#define FUNC_ARRAY_ADD_LOCAL(type, typename, member) \
    int arr_add_##typename##_loc(type x) \
    { \
        return member[x]; \
    }

#define FUNC_ARRAY_ADD_STATIC(type, typename, member) \
    int arr_add_##typename##_stat(type x) \
    { \
        s_##typename = x; \
        return member[s_##typename]; \
    }

#define FUNC_PTR_ADD_LOCAL(type, typename, member) \
    int ptr_add_##typename##_loc(type x) \
    { \
        type* ptr = member; \
        return *(ptr + x); \
    }

#define FUNC_PTR_ADD_STATIC(type, typename, member) \
    int ptr_add_##typename##_stat(type x) \
    { \
        type* ptr = member; \
        s_##typename = x; \
        return *(ptr + x); \
    }

#define FUNC_2WAY_ARR(type, typename, member) \
    FUNC_ARRAY_ADD_LOCAL(type, typename, member); \
    FUNC_ARRAY_ADD_STATIC(type, typename, member); \

#define FUNC_4WAY_ARR_PTR(type, typename, member) \
    FUNC_2WAY_ARR(type, typename, member); \
    FUNC_PTR_ADD_LOCAL(type, typename, member); \
    FUNC_PTR_ADD_STATIC(type, typename, member);


FUNC_2WAY_ARR(signed char, schar, s_sarr.pos);
FUNC_2WAY_ARR(unsigned char, uchar, s_uarr.non);
FUNC_2WAY_ARR(enum sche, senum, s_sarr.pos);
FUNC_2WAY_ARR(enum uche, uenum, s_uarr.non);


#define CHECK(func, arg, expect) \
    res = func(arg); \
    if (res != (expect)) { \
        ++failures; \
        printf("%s(%d): Got %d, expected %d\n", #func, (int)(arg), res, (expect)); \
    }

#define CHECK_4WAY_LIT(type, typename, literal, arg) \
    CHECK(typename##_loc_add_lit_##literal, arg, (arg) + 0x##literal); \
    CHECK(typename##_stat_add_lit_##literal, arg, (arg) + 0x##literal); \
    CHECK(lit_##literal##_add_##typename##_loc, arg, (arg) + 0x##literal); \
    CHECK(lit_##literal##_add_##typename##_stat, arg, (arg) + 0x##literal);

#define CHECK_4WAY_4LIT(type, typename, lit1, lit2, lit3, lit4, arg) \
    CHECK_4WAY_LIT(type, typename, lit1, arg); \
    CHECK_4WAY_LIT(type, typename, lit2, arg); \
    CHECK_4WAY_LIT(type, typename, lit3, arg); \
    CHECK_4WAY_LIT(type, typename, lit4, arg);

#define CHECK_2WAY_ARR(type, typename, arg) \
    CHECK(arr_add_##typename##_loc, arg, arg); \
    CHECK(arr_add_##typename##_stat, arg, arg);

#define CHECK_4WAY_SET(type, typename, args) \
    { \
        int i, arg, res; \
        for (i = 0; i < sizeof(args) / sizeof(args[0]); ++i) { \
            arg = args[i]; \
            CHECK_4WAY_4LIT(type, typename, ff, 100, 101, ff80, arg); \
        } \
    }

#define CHECK_6WAY_SET(type, typename, args) \
    { \
        int i, arg, res; \
        for (i = 0; i < sizeof(args) / sizeof(args[0]); ++i) { \
            arg = args[i]; \
            CHECK_4WAY_4LIT(type, typename, ff, 100, 101, ff80, arg); \
            CHECK_2WAY_ARR(type, typename, arg); \
        } \
    }


static int svals[5] = { -128, -88, -1, 88, 127 };
static int uvals[5] = { 1, 88, 127, 186, 255 };


void test_schar(void)
{
   CHECK_6WAY_SET(signed char, schar, svals);
}

void test_uchar(void)
{
    CHECK_6WAY_SET(unsigned char, uchar, uvals);
}

void test_char_as_signed(void)
{
    printf("char is signed\n");
    CHECK_4WAY_SET(char, char, svals);
}

void test_char_as_unsigned(void)
{
    printf("char is unsigned\n");
    CHECK_4WAY_SET(char, char, uvals);
}

void test_char(void)
{
    /* Plain 'char' is unsigned by default */
    if ((char)128 == 128) {
        test_char_as_unsigned();
    } else if ((char)128 == -128) {
        test_char_as_signed();
    }
}

void test_senum(void)
{
    CHECK_6WAY_SET(enum sche, senum, svals);
}

void test_uenum(void)
{
    CHECK_6WAY_SET(enum uche, uenum, uvals);
}

int main(void)
{
    int i;

    /* Init the test arrays */
    /* unsigned only has the unsigned part set */
    for (i = 0; i < 256; ++i) {
        s_uarr.non[i] = i;
    }
    /* signed only has the signed part set */
    for (i = -128; i < 128; ++i) {
        s_sarr.neg[i + 128] = i;
    }

    test_schar();
    test_uchar();
    test_char();
    test_senum();
    test_uenum();

    printf("failures: %d\n", failures);

    return failures ? EXIT_FAILURE : EXIT_SUCCESS;
}
