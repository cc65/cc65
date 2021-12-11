#ifndef DIV_COMMON_H
#define DIV_COMMON_H

/* check if the result is correct */
#define TEST_A_T(type, _n,_a,_b,_r) \
    int test##_n##a(void) { \
        typedef type int_t; \
        int_t a = ((int_t)_a), b = ((int_t)_b); \
        if (((a/((int_t)_b)) == ((int_t)_r)) && ((a/b) == ((int_t)_r))) { \
            return 0; \
        } else { \
            printf("%d\tincorrect: a/%ld = %ld, a/b = %ld\n\texpected:  %ld/%ld = %ld\n", \
                   (_n), (long)((int_t)_b), (long)(a/((int_t)_b)), (long)(a/b), (long)((int_t)_a), (long)((int_t)_b), (long)((int_t)_r)); \
            return 1; \
        } \
    }

/* check if the results are equal */
#define TEST_B_T(type, _n,_a,_b,_r) \
    int test##_n##b(void) { \
        typedef type int_t; \
        int_t a = ((int_t)_a), b = ((int_t)_b); \
        if (((a/((int_t)_b)) == (a/b))) { \
            return 0; \
        } else { \
            printf("%d\tnot equal: %ld != %ld, a = %ld, b = %ld\n\texpected:  %ld/%ld = %ld\n", \
                   (_n), (long)(a/((int_t)_b)), (long)(a/b), (long)(a), (long)(b), (long)((int_t)_a), (long)((int_t)_b), (long)((int_t)_r)); \
            return 1; \
        } \
    }

#define TEST_A_8(_n,_a,_b,_r) TEST_A_T(int8_t, _n,_a,_b,_r)
#define TEST_B_8(_n,_a,_b,_r) TEST_B_T(int8_t, _n,_a,_b,_r)
#define TEST_A_16(_n,_a,_b,_r) TEST_A_T(int16_t, _n,_a,_b,_r)
#define TEST_B_16(_n,_a,_b,_r) TEST_B_T(int16_t, _n,_a,_b,_r)
#define TEST_A_32(_n,_a,_b,_r) TEST_A_T(int32_t, _n,_a,_b,_r)
#define TEST_B_32(_n,_a,_b,_r) TEST_B_T(int32_t, _n,_a,_b,_r)

/* A and B */
#define TEST_AB_8(_n,_a,_b,_r) \
    TEST_A_8(_n,_a,_b,_r) \
    TEST_B_8(_n,_a,_b,_r)

#define TEST_AB_16(_n,_a,_b,_r) \
    TEST_A_16(_n,_a,_b,_r) \
    TEST_B_16(_n,_a,_b,_r)

#define TEST_AB_32(_n,_a,_b,_r) \
    TEST_A_32(_n,_a,_b,_r) \
    TEST_B_32(_n,_a,_b,_r)

#endif
