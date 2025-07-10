/* Bug #2285 - Regression in type composition */

void foo();              /* OK */
void foo(int (*)(int));  /* OK */
void foo(int (*)(long)); /* WRONG: Should be an error */
