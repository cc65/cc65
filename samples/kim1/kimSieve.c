#include <stdio.h>
#include <stdlib.h>

typedef unsigned char byte;
typedef unsigned short int ushort;
typedef unsigned long int  ulong;

#define LIMIT 100000L

// BITARRAY
//
// My bit-access macros pre-divide by two on the presumption that you'll never
// try try access both odd and even bits!

#define GETBIT(array, bit) (array[bit >> 4]  &  (1 << ((bit >> 1) & 7)))
#define SETBIT(array, bit) (array[bit >> 4] |=  (1 << ((bit >> 1) & 7)))
#define CLRBIT(array, bit) (array[bit >> 4] &= ~(1 << ((bit >> 1) & 7)))

// RepeatChar
//
// Outputs a given character N times

void RepeatChar(char c, size_t count)
{
   while (count--)
      putc(c, stdout);
}

// sqrti
//
// Binary search integer square root

ushort sqrti(ulong num)
{
    long i;
    ulong ret = 0;

    for(i = 15; i >= 0; i--)
    {
        ulong temp = ret | (1L << (ulong)i);
        if(temp * temp <= num)
        {
            ret = temp;
        }
    }
    return ret;
}

// main()
//
// CC65 main function receives no parameters

int main(void)
{
   // CC65 cannot mix code and data so we have to declare all variables here in the function prolog

   ulong iNumber;
   ushort currentFactor;
   ulong numBytesAllocated, rootOfLimit;
   byte *array;
   ulong countOfPrimes;

   rootOfLimit = sqrti(LIMIT);
   puts("\r\n\r\n");
   RepeatChar('*', 70);
   puts("\r\n** Prime Number Sieve - Dave Plummer 2022                           **");
   RepeatChar('*', 70);

   printf("\r\n\r\nCalculating primes to %ld using a sqrt of %ld...\r\n", LIMIT, rootOfLimit);

   // Calculate how much memory should be allocated

   numBytesAllocated = (LIMIT + 15) / 16;
   array = malloc(numBytesAllocated);
   if (!array)
   {
      printf("Unable to allocate %ld bytes for %ld bits\r\n", numBytesAllocated, LIMIT);
      return 0;
   }
   else
   {
      printf("Allocated %ld bytes for %ld slots\r\n", numBytesAllocated, LIMIT);

      // Preset all the bits to true

      for (iNumber = 0; iNumber < numBytesAllocated; iNumber++)
         array[iNumber] = 0xFF;
   }

   // Search for next unmarked factor

   currentFactor = 3;
   while (currentFactor <= rootOfLimit)
   {
      ulong num, n;

      for (num = currentFactor; num <= LIMIT; num += 2)
      {
         if (GETBIT(array, num))
         {
            currentFactor = num;
            break;
         }
      }

      for (n = (ulong) currentFactor * currentFactor; n <= LIMIT; n += currentFactor * 2)
         CLRBIT(array, n);

      currentFactor += 2;
   }

   // Display results
   //
   // printf("The following primes were found at or below %ld:\r\n2, ", LIMIT);

   countOfPrimes = 1;
   for (iNumber = 3; iNumber <= LIMIT; iNumber += 2)
      if (GETBIT(array, iNumber))
         countOfPrimes++;

   printf("[END: Count = %ld]\r\n", countOfPrimes);

   free(array);
   return 1;
}
