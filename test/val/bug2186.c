/* Bug #2186 - Wrong array indexing when index comes from bit-field */

#include <stdio.h>

unsigned failures;

typedef struct {
    char flag : 1;
    char index : 7;
} weird_type;

const char array[] = { '6', '5', '0', '2' };

weird_type data;

int main(void) {
    data.flag = 1;

    data.index = 0;
    if (array[data.index] != array[0])
    {
        ++failures;
        printf("Got '%c', expected '%c'\n", array[data.index], array[0]);
    }

    data.index = 1;
    if (array[data.index] != array[1])
    {
        ++failures;
        printf("Got '%c', expected '%c'\n", array[data.index], array[1]);
    }

    if (failures > 0)
    {
        printf("Failures: %u\n", failures);
    }

    return failures;
}
