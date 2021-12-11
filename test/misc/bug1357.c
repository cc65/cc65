
/* issue #1357 - X Macros don't work with C preprocessor */

#define OPCODES(X) \
    X(PUSHNIL) \
    X(PUSHTRUE) \
    X(PUSHFALSE)

enum {
#define X(op) op,
OPCODES(X)
#undef X
    N_OPS
};

/* cc65 -E bug1357.c -o bug1357.c.pre
   should produce something like this:

enum {
PUSHNIL,
PUSHTRUE,
PUSHFALSE,
    N_OPS
};
*/

int main(void)
{
    return 0;
}
