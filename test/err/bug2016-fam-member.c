/* Bug #2016 - cc65 erroneously allows struct fields that are structs with flexible array members */

typedef struct x {
    int a;
    int b[];    /* Ok: Flexible array member can be last */
} x;

typedef union u {
    int a;
    x   x;      /* Ok: Union member can contain flexible array member */
} u;

struct y {
    u   u;      /* Not ok: Contains union that contains flexible array member */
    int a;
};
