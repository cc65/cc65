/* Bug #2016 - cc65 erroneously allows struct fields that are structs with flexible array members */

typedef struct x {
    int a;
    int b[];    /* Ok: Flexible array member can be last */
} x;

struct y {
    x   x;      /* Not ok: Contains flexible array member */
    int a;
};
