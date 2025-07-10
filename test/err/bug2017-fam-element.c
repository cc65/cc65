/* Bug #2017 - cc65 erroneously allows arrays of structs with flexible array members */

typedef struct x {
    int a;
    int b[];    /* Ok: Flexible array member can be last */
} x;

typedef union u {
    int a;
    x   x;      /* Ok: Union member can contain flexible array member */
} u;

union u y[3];  /* Should be an error */
