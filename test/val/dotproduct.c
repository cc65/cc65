/* Matrix dot product computation tests.
** These use compound array expressions and exercise certain optimizers.
*/

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

int failures;

typedef struct matrix4x4i8 {
    int8_t m[4][4];
} matrix4x4i8;

typedef struct matrix4x4u8 {
    uint8_t m[4][4];
} matrix4x4u8;

typedef struct matrix4x4i16 {
    int16_t m[4][4];
} matrix4x4i16;

const matrix4x4i8 mi8a = {{
    {   1,   2,   3,   4 },
    {   5,   6,   7,   8 },
    {  -8,  -7,  -6,  -5 },
    {  -4,  -3,  -2,  -1 },
}};

const matrix4x4i8 mi8b = {{
    {   0,   5,   6,   7 },
    {   8,   0,   1,   2 },
    {   3,   4,   0,   8 },
    {   7,   6,   5,   0 },
}};

const matrix4x4i8 refi8 = {{
    {  53,  41,  28,  35 },
    { 125, 101,  76, 103 },
    {-109, -94, -80,-118 },
    { -37, -34, -32, -50 },
}};

const matrix4x4u8 mu8a = {{
    {   1,   2,   3,   4 },
    {   5,   6,   7,   8 },
    {   2,   1,   8,   7 },
    {   6,   5,   4,   3 },
}};

const matrix4x4u8 refu8 = {{
    {  53,  41,  28,  35 },
    { 125, 101,  76, 103 },
    {  81,  84,  48,  80 },
    {  73,  64,  56,  84 },
}};

const matrix4x4i16 mi16a = {{
    {   1,   2,   3,   4 },
    {   5,   6,   7,   8 },
    {  -8,  -7,  -6,  -5 },
    {  -4,  -3,  -2,  -1 },
}};

const matrix4x4i16 mi16b = {{
    {   0,  15,  16,  17 },
    {  18,   0,  21,  22 },
    {  23,  24,   0,  28 },
    {  27,  26,  25,   0 },
}};

const matrix4x4i16 refi16 = {{
    { 213, 191, 158, 145 },
    { 485, 451, 406, 413 },
    {-399,-394,-400,-458 },
    {-127,-134,-152,-190 },
}};

static matrix4x4i8 resi8;
static matrix4x4u8 resu8;
static matrix4x4i16 resi16;


void stat_dot_product_i8(matrix4x4i8* res)
{
#define a mi8a
#define b mi8b

    static uint8_t i, j, k;

    for (i = 0; i < 4; ++i) {
        for (j = 0; j < 4; ++j) {
            res->m[i][j] = 0;
            for (k = 0; k < 4; ++k) {
                res->m[i][j] += a.m[i][k] * b.m[k][j];
            }
        }
    }

#undef a
#undef b
}

void stat_dot_product_i8_2(matrix4x4i8* res)
{
#define a mi8a
#define b mi8b

    static int8_t i, j, k;

    for (i = 0; i < 4; ++i) {
        for (j = 0; j < 4; ++j) {
            res->m[i][j] = 0;
            for (k = 0; k < 4; ++k) {
                res->m[i][j] += a.m[i][k] * b.m[k][j];
            }
        }
    }

#undef a
#undef b
}

void loc_dot_product_i8(matrix4x4i8* res)
{
    matrix4x4i8 a, b;
    uint8_t i, j, k;

    memcpy(&a, &mi8a, sizeof(mi8a));
    memcpy(&b, &mi8b, sizeof(mi8b));

    for (i = 0; i < 4; ++i) {
        for (j = 0; j < 4; ++j) {
            res->m[i][j] = 0;
            for (k = 0; k < 4; ++k) {
                res->m[i][j] += a.m[i][k] * b.m[k][j];
            }
        }
    }
}

void loc_dot_product_i8_2(matrix4x4i8* res)
{
    matrix4x4i8 a, b;
    int i, j, k;

    memcpy(&a, &mi8a, sizeof(mi8a));
    memcpy(&b, &mi8b, sizeof(mi8b));

    for (i = 0; i < 4; ++i) {
        for (j = 0; j < 4; ++j) {
            res->m[i][j] = 0;
            for (k = 0; k < 4; ++k) {
                res->m[i][j] += a.m[i][k] * b.m[k][j];
            }
        }
    }
}

void loc_dot_product_i8_3(matrix4x4i8* res)
{
    matrix4x4i8 mat[4];
    /* Using (ptr-4*4) pointer hacks */
#define a (mat[1])
#define b (mat[3])

    int8_t i, j, k;

    memcpy(&mat[0], &mi8a, sizeof(mi8a));
    memcpy(&mat[2], &mi8b, sizeof(mi8b));

    for (i = 0; i < 4; ++i) {
        for (j = -16; j < -12; ++j) {
            res->m[i][j+16] = 0;
            for (k = -16; k < -12; ++k) {
                res->m[i][j+16] += a.m[i][k] * b.m[k+16][j];
            }
        }
    }

#undef a
#undef b
}

void stat_dot_product_u8(matrix4x4u8* res)
{
#define a mu8a
#define b (*((const matrix4x4u8*)&mi8b))

    static uint8_t i, j, k;

    for (i = 0; i < 4; ++i) {
        for (j = 0; j < 4; ++j) {
            res->m[i][j] = 0;
            for (k = 0; k < 4; ++k) {
                res->m[i][j] += a.m[i][k] * b.m[k][j];
            }
        }
    }

#undef a
#undef b
}

void loc_dot_product_u8(matrix4x4u8* res)
{
    matrix4x4u8 a, b;
    uint8_t i, j, k;

    memcpy(&a, &mu8a, sizeof(mi8a));
    memcpy(&b, &mi8b, sizeof(mi8b));

    for (i = 0; i < 4; ++i) {
        for (j = 0; j < 4; ++j) {
            res->m[i][j] = 0;
            for (k = 0; k < 4; ++k) {
                res->m[i][j] += a.m[i][k] * b.m[k][j];
            }
        }
    }
}

void loc_dot_product_u8_2(matrix4x4u8* res)
{
    matrix4x4u8 a, b;
    int i, j, k;

    memcpy(&a, &mu8a, sizeof(mi8a));
    memcpy(&b, &mi8b, sizeof(mi8b));

    for (i = 0; i < 4; ++i) {
        for (j = 0; j < 4; ++j) {
            res->m[i][j] = 0;
            for (k = 0; k < 4; ++k) {
                res->m[i][j] += a.m[i][k] * b.m[k][j];
            }
        }
    }
}

void stat_dot_product_i16(matrix4x4i16* res)
{
#define a mi16a
#define b mi16b

    static int i, j, k;

    for (i = 0; i < 4; ++i) {
        for (j = 0; j < 4; ++j) {
            res->m[i][j] = 0;
            for (k = 0; k < 4; ++k) {
                res->m[i][j] += a.m[i][k] * b.m[k][j];
            }
        }
    }

#undef a
#undef b
}

void loc_dot_product_i16(matrix4x4i16* res)
{
    matrix4x4i16 a, b;
    int8_t i, j, k;

    memcpy(&a, &mi16a, sizeof(mi16a));
    memcpy(&b, &mi16b, sizeof(mi16b));

    for (i = 0; i < 4; ++i) {
        for (j = 0; j < 4; ++j) {
            res->m[i][j] = 0;
            for (k = 0; k < 4; ++k) {
                res->m[i][j] += a.m[i][k] * b.m[k][j];
            }
        }
    }
}

void loc_dot_product_i16_2(matrix4x4i16* res)
{
    matrix4x4i16 mat[4];
    /* Using (ptr-4*4) pointer hacks */
#define a (mat[1])
#define b (mat[3])

    int i, j, k;

    memcpy(&mat[0], &mi16a, sizeof(mi16a));
    memcpy(&mat[2], &mi16b, sizeof(mi16b));

    for (i = 0; i < 4; ++i) {
        for (j = -16; j < -12; ++j) {
            res->m[i][j+16] = 0;
            for (k = -16; k < -12; ++k) {
                res->m[i][j+16] += a.m[i][k] * b.m[k+16][j];
            }
        }
    }

#undef a
#undef b
}

void print_matrix_i8(const matrix4x4i8* v)
{
    int8_t i, j;

    for (i = 0; i < 4; ++i) {
        printf("  [ ");
        for (j = 0; j < 4; ++j) {
            printf("%4d ", v->m[i][j]);
        }
        printf(" ]\n");
    }
}

void print_matrix_u8(const matrix4x4u8* v)
{
    uint8_t i, j;

    for (i = 0; i < 4; ++i) {
        printf("  [ ");
        for (j = 0; j < 4; ++j) {
            printf("%4d ", v->m[i][j]);
        }
        printf(" ]\n");
    }
}

void print_matrix_i16(const matrix4x4i16* v)
{
    int i, j;

    for (i = 0; i < 4; ++i) {
        printf("  [ ");
        for (j = 0; j < 4; ++j) {
            printf("%4d ", v->m[i][j]);
        }
        printf(" ]\n");
    }
}


#define CHECK(func, result, ref, print_mat) \
    memset(&result, 0, sizeof(result)); \
    func(&result); \
    if (memcmp(&result, &ref, sizeof(ref)) != 0) \
    { \
        ++failures; \
        printf("Fail: %s\n", #func); \
        print_mat(&result); \
    }


int main(void)
{
    printf("Reference i8 result:\n");
    print_matrix_i8(&refi8);

    printf("Reference u8 result:\n");
    print_matrix_u8(&refu8);

    printf("Reference i16 result:\n");
    print_matrix_i16(&refi16);

    CHECK(stat_dot_product_i8, resi8, refi8, print_matrix_i8);
    CHECK(stat_dot_product_i8_2, resi8, refi8, print_matrix_i8);
    CHECK(loc_dot_product_i8, resi8, refi8, print_matrix_i8);
    CHECK(loc_dot_product_i8_2, resi8, refi8, print_matrix_i8);
    CHECK(loc_dot_product_i8_3, resi8, refi8, print_matrix_i8);

    CHECK(stat_dot_product_u8, resu8, refu8, print_matrix_u8);
    CHECK(loc_dot_product_u8, resu8, refu8, print_matrix_u8);
    CHECK(loc_dot_product_u8_2, resu8, refu8, print_matrix_u8);

    CHECK(stat_dot_product_i16, resi16, refi16, print_matrix_i16);
    CHECK(loc_dot_product_i16, resi16, refi16, print_matrix_i16);
    CHECK(loc_dot_product_i16_2, resi16, refi16, print_matrix_i16);

    printf("Failures: %d\n", failures);

    return failures == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
