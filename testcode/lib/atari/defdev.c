/*
** testprogram printing the default device
**
** 26-Nov-2009, Christian Groessler
*/

#include <stdio.h>
#include <conio.h>
#include <atari.h>
#include <cc65.h>

extern char _defdev[];

int main(void)
{
        printf("default device: %s\n", _defdev);
        if (doesclrscrafterexit()) cgetc();
        return 0;
}
