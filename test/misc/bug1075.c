/* bug #1075 Internal compiler error */

long rhs;

int main(void)
{
    /* the whole lhs is errorneously treated as an absolute address (integer 
       constant) neglecting its dereference */
    return *(char *)0xD77C + rhs;
} 
