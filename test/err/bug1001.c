
/* https://github.com/cc65/cc65/issues/1001 */

#include <stdio.h>

int main(void)
{
    printf("test",); /* should be an error */
    return 0;
}
