/* Bug #1895 - missing diagnostics on incompatible pointer/array types

  Test of incompatible pointer/array types in assignment ans conditional
  expressions, as well as function prototypes.

  In each source file, define a single macro and include this file to perform
  a coresponding test individually.

  https://github.com/cc65/cc65/issues/1895
*/

/* Test 1 suite */
#ifdef DO_TEST_1_SUB_1_A
#define TEST_1_SUB_1_A CMP_TYPES_1
#else
#define TEST_1_SUB_1_A BLANK
#endif

#ifdef DO_TEST_1_SUB_1_B
#define TEST_1_SUB_1_B CMP_TYPES_1
#else
#define TEST_1_SUB_1_B BLANK
#endif

#ifdef DO_TEST_1_SUB_2_A
#define TEST_1_SUB_2_A CMP_TYPES_1
#else
#define TEST_1_SUB_2_A BLANK
#endif

#ifdef DO_TEST_1_SUB_2_B
#define TEST_1_SUB_2_B CMP_TYPES_1
#else
#define TEST_1_SUB_2_B BLANK
#endif

#ifdef DO_TEST_1_SUB_4_A
#define TEST_1_SUB_4_A CMP_TYPES_1
#else
#define TEST_1_SUB_4_A BLANK
#endif

#ifdef DO_TEST_1_SUB_4_B
#define TEST_1_SUB_4_B CMP_TYPES_1
#else
#define TEST_1_SUB_4_B BLANK
#endif

#ifdef DO_TEST_1_SUB_5_A
#define TEST_1_SUB_5_A CMP_TYPES_1
#else
#define TEST_1_SUB_5_A BLANK
#endif

#ifdef DO_TEST_1_SUB_5_B
#define TEST_1_SUB_5_B CMP_TYPES_1
#else
#define TEST_1_SUB_5_B BLANK
#endif

/* Test 2 suite */
#ifdef DO_TEST_2_SUB_1
#define TEST_2_SUB_1 CMP_TYPES_2
#else
#define TEST_2_SUB_1 BLANK
#endif

#ifdef DO_TEST_2_SUB_2
#define TEST_2_SUB_2 CMP_TYPES_2
#else
#define TEST_2_SUB_2 BLANK
#endif

#ifdef DO_TEST_2_SUB_3
#define TEST_2_SUB_3 CMP_TYPES_2
#else
#define TEST_2_SUB_3 BLANK
#endif

#ifdef DO_TEST_2_SUB_4
#define TEST_2_SUB_4 CMP_TYPES_2
#else
#define TEST_2_SUB_4 BLANK
#endif

#ifdef DO_TEST_2_SUB_5
#define TEST_2_SUB_5 CMP_TYPES_2
#else
#define TEST_2_SUB_5 BLANK
#endif

/* Test 3 suite */
#ifdef DO_TEST_3_SUB_1
#define TEST_3_SUB_1 CMP_TYPES_3
#else
#define TEST_3_SUB_1 BLANK
#endif

#ifdef DO_TEST_3_SUB_2
#define TEST_3_SUB_2 CMP_TYPES_3
#else
#define TEST_3_SUB_2 BLANK
#endif

#ifdef DO_TEST_3_SUB_3
#define TEST_3_SUB_3 CMP_TYPES_3
#else
#define TEST_3_SUB_3 BLANK
#endif

#ifdef DO_TEST_3_SUB_4
#define TEST_3_SUB_4 CMP_TYPES_3
#else
#define TEST_3_SUB_4 BLANK
#endif

#ifdef DO_TEST_3_SUB_5
#define TEST_3_SUB_5 CMP_TYPES_3
#else
#define TEST_3_SUB_5 BLANK
#endif

/* Implementation */
#define CONCAT(a, b) CONCAT_impl_(a, b)
#define CONCAT_impl_(a, b) a##b
#define BLANK(...)
#define DECL_FUNCS(A, B)\
    void CONCAT(foo_,__LINE__)(A); void CONCAT(foo_,__LINE__)(B);

/* Test with assignment */
#define CMP_TYPES_1(A, B)\
    do {\
        A p; B q;\
_Pragma("warn(error, on)")\
        p = q;\
_Pragma("warn(error, off)")\
    } while (0)

/* Test with conditional expression */
#define CMP_TYPES_2(A, B)\
    do {\
        A p; B q;\
_Pragma("warn(error, on)")\
        v = v ? p : q;\
_Pragma("warn(error, off)")\
    } while (0)

/* Test with function prototype */
#define CMP_TYPES_3(A, B)\
    do {\
        DECL_FUNCS(A,B);\
    } while (0)

static void *v;

typedef int (*p1)[3];              /* pointer to array */
typedef int **q1;                  /* pointer to pointer */
typedef int (**p2)[3];             /* pointer to pointer to array */
typedef int ***q2;                 /* pointer to pointer to pointer */
typedef int p3[1][3];              /* array of array */
typedef int *q3[1];                /* array of pointer */
typedef int const **p4;            /* pointer to pointer to const */
typedef int **q4;                  /* pointer to pointer to non-const */
typedef int (*p5)(int (*)(p3));    /* pointer to function taking pointer to function taking pointer to array */
typedef int (*q5)(int (*)(q3));    /* pointer to function taking pointer to function taking pointer to pointer */

int main(void)
{
    /* Warnings */
    TEST_1_SUB_1_A(p1, q1);
    TEST_1_SUB_1_B(q1, p1);
    TEST_1_SUB_2_A(p2, q2);
    TEST_1_SUB_2_B(q2, p2);
    /* TEST_1_SUB_3_A(p3, q3); */
    /* TEST_1_SUB_3_B(q3, p3); */
    TEST_1_SUB_4_A(p4, q4);
    TEST_1_SUB_4_B(q4, p4);
    TEST_1_SUB_5_A(p5, q5);
    TEST_1_SUB_5_B(q5, p5);

    /* GCC and clang give warnings while cc65 gives errors */
    TEST_2_SUB_1(p1, q1);
    TEST_2_SUB_2(p2, q2);
    TEST_2_SUB_3(p3, q3);
    TEST_2_SUB_4(p4, q4);
    TEST_2_SUB_5(p5, q5);

    /* Errors */
    TEST_3_SUB_1(p1, q1);
    TEST_3_SUB_2(p2, q2);
    TEST_3_SUB_3(p3, q3);
    TEST_3_SUB_4(p4, q4);
    TEST_3_SUB_5(p5, q5);

    return 0;
}
