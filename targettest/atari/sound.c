/*
** Test program for _sound for atari
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
    _sound(1,121,10,15); //voice, pitch, distortion, volume
    for(i=0;i<9000;i++);
    _sound(1,0,0,0); //silencing, same as Atari Basic
    return 0;
}
