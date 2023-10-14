/* Bug #2017 - cc65 erroneously allows arrays of structs with flexible array members */

struct z {
    int a;
    int c;
    int b[];
};

struct z y[3];          /* Should be an error */
