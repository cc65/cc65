/*
 * test program to display avail. mem.
 *
 */

#include <stdio.h>

unsigned int *APPMHI = (unsigned int *)0xe;
unsigned int *MEMTOP = (unsigned int *)0x2e5;

unsigned char _graphmode_used = 0;

int main(void)
{
    printf("APPMHI = %04X, MEMTOP = %04X\n",*APPMHI,*MEMTOP);
    printf("press return!\n");
    getchar();
    return(0);
}
