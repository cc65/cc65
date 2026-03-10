/* Exercise compare operator cases in forward and reverse forms.
** Common test framework and macros.
** 
** License: Public Domain
**
** This software is provided 'as-is', without any express or implied warranty.
** In no event will the authors be held liable for any damages arising from
** the use of this software.
*/

/* Total failures */
static unsigned failures = 0;
/* Something for compiler to do between tests */
static int successes = 0;

/* To track the asm code, search for marker value */
static unsigned marker = 0;

#define OP_MUST_FAIL(cond)      if (cond) { fail(#cond); } else { success(); }
#define OP_MUST_SUCCEED(cond)   if (cond) { success(); } else { fail(#cond); }

/* Expected fail in forward, success in reverse forms */
#define MUST_FAIL_FORWARD(v1, op, v2, rev_op)  \
    OP_MUST_FAIL((v1) op (v2)); \
    OP_MUST_FAIL((v2) rev_op (v1)); \
    OP_MUST_SUCCEED((v1) rev_op (v2)); \
    OP_MUST_SUCCEED((v2) op (v1))

/* Expected fail in forward, success in reverse forms for both < and <= */
#define MUST_FAIL_FORWARD_ALL(v1, op1, op2, v2, rev_op1, rev_op2)  \
    MUST_FAIL_FORWARD(v1, op1, v2, rev_op1); \
    MUST_FAIL_FORWARD(v1, op2, v2, rev_op2)

/* Expected fail in both forward and reverse forms (< and >); equality */
#define MUST_FAIL_BOTH(v1, op, v2, rev_op)  \
    OP_MUST_FAIL((v1) op (v2)); \
    OP_MUST_FAIL((v2) rev_op (v1)); \
    OP_MUST_FAIL((v1) rev_op (v2)); \
    OP_MUST_FAIL((v2) op (v1))

/* Expected success in forward, fail in reverse forms */
#define MUST_SUCCEED_FORWARD(v1, op, v2, rev_op)  \
    OP_MUST_SUCCEED((v1) op (v2)); \
    OP_MUST_SUCCEED((v2) rev_op (v1)); \
    OP_MUST_FAIL((v1) rev_op (v2)); \
    OP_MUST_FAIL((v2) op (v1))

/* Expected success in forward, fail in reverse forms for both < and <= */
#define MUST_SUCCEED_FORWARD_ALL(v1, op1, op2, v2, rev_op1, rev_op2)  \
    MUST_SUCCEED_FORWARD(v1, op1, v2, rev_op1); \
    MUST_SUCCEED_FORWARD(v1, op2, v2, rev_op2)

/* Expected success in both forward and reverse forms (<= and =>); equality */
#define MUST_SUCCEED_BOTH(v1, op, v2, rev_op)  \
    OP_MUST_SUCCEED((v1) op (v2)); \
    OP_MUST_SUCCEED((v2) rev_op (v1)); \
    OP_MUST_SUCCEED((v1) rev_op (v2)); \
    OP_MUST_SUCCEED((v2) op (v1))

/* Expected <, > fail / <=, => succeed -- equality */
#define MUST_BE_EQUAL(v1, v2)  \
    MUST_FAIL_BOTH(v1, <, v2, >); \
    MUST_SUCCEED_BOTH(v1, <=, v2, >=)

/* Expected <, <= fail / >, => succeed in forward form, and
**  <, <= succeed / >, => fail in reverse form. */
#define MUST_BE_GREATER_THAN(v1, v2)  \
    MUST_FAIL_FORWARD_ALL(v1, <, <=, v2, >, >=)

/* Expected <, <= succeed / >, => fail in forward form, and
**  <, <= fail / >, => succeed in reverse form. */
#define MUST_BE_LESS_THAN(v1, v2)  \
    MUST_SUCCEED_FORWARD_ALL(v1, <, <=, v2, >, >=)

/* Print a fail message and provide a primary barrier for next test */
static void fail(const char* cond)
{
    failures += 1;
    printf(TEST_NAME ": failure at marker $%04X, %s\n", marker, cond);
    marker += 1;
}

/* Provide a primary barrier for next test */
static void success()
{
    /* Trash the primary */
    int var1 = failures + 1;
    if (var1 != 0) {
        successes += 1;
    }
    marker += 1;
}
