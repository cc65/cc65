/*
** testprogram printing the default device
**
**  January 6 2023 Mariano Domínguez
*/

#include <stdio.h>
#include <conio.h>
#include <atari.h>
#include <cc65.h>

int main(void)
{
    int i=0;
    printf("playing sound \n");
    sound(1,121,10,15);
    for(i=0;i<9000;i++);
    sound(1,0,0,0);
    return 0;
}
