/* C99 inline in declarations */

inline typedef int;                 /* Error */
static inline int;                  /* Error */
inline static int a1;               /* Error */
int inline (*fp1)(void);            /* Error */
typedef inline int f1_t(void);      /* Error */
inline int f1a(void);               /* OK here warning later */
inline extern int f1b(void);        /* OK here warning later */
extern inline int f1b(void);        /* Same as above */
inline static int f1c(void);        /* OK here warning later */
static inline int f1c(void);        /* Same as above */

void foo(inline int x);             /* Error */
int a = sizeof (inline int);        /* TODO: better error message */
int b = sizeof (inline int (int));  /* TODO: better error message */

inline int main(void)               /* Error */
{
    inline typedef int;             /* Error */
    static inline int;              /* Error */
    extern inline int a2;           /* Error */
    int inline (*fp2)(void);        /* Error */
    typedef inline int f2_t(void);  /* Error */
    inline int f2a(void);           /* OK here warning later */
    inline extern int f2b(void);    /* OK here warning later */
    extern inline int f2b(void);    /* Same as above */

    f1a();                          /* Still imported */
    f1b();                          /* Still imported */
    f1c();                          /* Not imported */
    f2a();                          /* Still imported */
    f2b();                          /* Still imported */
}

inline int main(void);              /* Error */

/* Warning: non-external inline functions declared but undefined in TU */
