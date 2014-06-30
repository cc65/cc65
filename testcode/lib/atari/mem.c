/*
** show some memory stuff
**
** 04-Aug-2004, Christian Groessler
*/

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <atari.h>

extern int getsp(void);                         /* comes from ../getsp.s */

extern char _dos_type;                          /* bss variable */
unsigned char data = 0x12;                      /* data variable */

unsigned int *APPMHI = (unsigned int *)14;      /* 14,15 */
unsigned char *RAMTOP = (unsigned char *)106;   /* in pages */
unsigned int *LOMEM = (unsigned int *)128;      /* used by BASIC */
unsigned int *MEMTOP = (unsigned int *)741;
unsigned int *MEMLO = (unsigned int *)743;
void *allocmem;

int main(void)
{
  allocmem = malloc(257);

  clrscr();

  printf("  RAMTOP = %02X (%u) - $%04X (%u)\n",
         *RAMTOP, *RAMTOP, *RAMTOP * 256, *RAMTOP * 256);
  printf("  APPMHI = $%04X (%u)\n", *APPMHI, *APPMHI);
  printf("  LOMEM  = $%04X (%u)  <BASIC only>\n", *LOMEM, *LOMEM);
  printf("  MEMTOP = $%04X (%u)\n", *MEMTOP, *MEMTOP);
  printf("  MEMLO  = $%04X (%u)\n", *MEMLO, *MEMLO);

  printf("  ----------------------\n");
  printf("  main:            $%04X  (code)\n", &main);
  printf("  data:            $%04X  (data)\n", &data);
  printf("  _dos_type:       $%04X  (bss)\n", &_dos_type);
  printf("  allocmem:        $%04X  (dyn. data)\n", allocmem);
  printf("  sp:              $%04X  (stack ptr)\n", getsp());

  if (allocmem) free(allocmem);
  if (_dos_type != 1) cgetc();
  return(0);
}
