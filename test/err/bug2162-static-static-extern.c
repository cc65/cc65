/* Bug #2162 - conflicting declarations in functions */

static int i;

int main(void)
{
    static int i = 42; /* OK - this shadows the i in file scope */
    extern int i;      /* Error - if this were accepted, it would be confusing which object i refers to */
    return i;
}
