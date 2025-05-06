/* Test for shadowing and linkage of file-scope "static" and block-scope "extern" declarations */

static int g(int x);            /* Generated functions with internal linkage are not always kept in cc65 */

int main(void)
{
    char f = 'f';               /* Shadows global "int f(void)" (if any) */
    char c = 'c';               /* Shadows global "int c" (if any) */
    {
        void* f = 0;            /* Shadows local  "char f" */
        void* c = 0;            /* Shadows local  "char c" */
        {
            int f(void);        /* Shadows local  "char f" and "void* f" */
            extern int g(int);  /* Shadows global "int g(int x)" */
            extern int c;       /* Shadows local  "char c" and "void* c" */
            return f() ^ g(c);  /* Link to global "int g(int x)" */
        }
    }
}

int c = 42;

int f(void)
{
    return 42;
}

int g(int x)
{
    return x;
}
