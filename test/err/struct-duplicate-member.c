/* Ensure that a duplicate member in a struct produces an error.
** https://github.com/cc65/cc65/issues/2015
*/

struct bads {
    int a;
    int a; /* this is an error */
};

union badu {
    int a, a; /* also an error */
};

int main(void)
{
    return 0;
}
