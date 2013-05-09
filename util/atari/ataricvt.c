#include <stdio.h>

int main (void)
{
    int C;
    while ((C = getchar ()) != EOF) {
        if (C == 0x9B) {
            putchar ('\n');
        } else if (C == 0x7F) {
            putchar ('\t');
        } else {  
            putchar (C);
        }
    }
    return 0;
}

