
/* bug #1098 Empty enumerator-list */

/* The C Standard requires that something exists between the braces for 
 * enum, struct, and union. */

union {
};

int main(void)
{
    return 0;
}
