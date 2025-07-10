/* Bug #2312 - Error recovery from preprocessor errors at the end of a declaration */

typedef int A;  /* ';' consumption triggers PP below */

#define         /* PP error during ';' consumption */

A   f(void);    /* Should be OK */
int A(void);    /* Should be an error */
