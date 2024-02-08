/* Bug #2162 - conflicting declarations in functions */

static int i;

int main(void)
{
    extern int i; /* cc65 allows this */
    int i = 42;   /* Error - if this were accepted, it would be confusing which object i refers to */
    return i;
}
