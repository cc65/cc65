// --------------------------------------------------------------------------
// Diagnostics Test for KIM-1
//
// Dave Plummer
// davepl@davepl.com
//
// Memory test examples by Michael Barr
//
// --------------------------------------------------------------------------

#include <stdio.h>                  // For printf
#include <stdlib.h>                 // For rand, srand
#include <string.h>                 // For memcpy

typedef unsigned char byte;

// RepeatChar
//
// Outputs a given character N times

void RepeatChar(char c, size_t count)
{
   while (count--)
      putc(c, stdout);
}

/**********************************************************************
 *
 * Function:    memTestDataBus()
 *
 * Description: Test the data bus wiring in a memory region by
 *              performing a walking 1's test at a fixed address
 *              within that region.  The address (and hence the
 *              memory region) is selected by the caller.
 *
 * Returns:     0 if the test succeeds.
 *              A non-zero result is the first pattern that failed.
 *
 **********************************************************************/

byte memTestDataBus(volatile byte * address)
{
    byte pattern;

    // Perform a walking 1's test at the given address.

    for (pattern = 1; pattern != 0; pattern <<= 1)
    {
        // Write the test pattern.
        *address = pattern;

        // Read it back and check it
        if (*address != pattern)
        {
            printf("\r\nmemTestDataBus: FAILED at %04x with pattern %02x\r\n", address, pattern);
            return (pattern);
        }
    }

    return (0);
}

/**********************************************************************
 *
 * Function:    memTestAddressBus()
 *
 * Description: Test the address bus wiring in a memory region by
 *              performing a walking 1's test on the relevant bits
 *              of the address and checking for aliasing. This test
 *              will find single-bit address failures such as stuck
 *              -high, stuck-low, and shorted pins.  The base address
 *              and size of the region are selected by the caller.
 *
 * Notes:       For best results, the selected base address should
 *              have enough LSB 0's to guarantee single address bit
 *              changes.  For example, to test a 64-Kbyte region,
 *              select a base address on a 64-Kbyte boundary.  Also,
 *              select the region size as a power-of-two--if at all
 *              possible.
 *
 * Returns:     NULL if the test succeeds.
 *              A non-zero result is the first address at which an
 *              aliasing problem was uncovered.  By examining the
 *              contents of memory, it may be possible to gather
 *              additional information about the problem.
 *
 **********************************************************************/

byte * memTestAddressBus(volatile byte * baseAddress, unsigned long nBytes)
{
    unsigned long addressMask = (nBytes/sizeof(byte) - 1);
    unsigned long offset;
    unsigned long testOffset;

    byte pattern     = (byte) 0xAAAAAAAA;
    byte antipattern = (byte) 0x55555555;


    //Write the default pattern at each of the power-of-two offsets.

    for (offset = 1; (offset & addressMask) != 0; offset <<= 1)
    {
        baseAddress[offset] = pattern;
    }

    // Check for address bits stuck high.

    testOffset = 0;
    baseAddress[testOffset] = antipattern;

    for (offset = 1; (offset & addressMask) != 0; offset <<= 1)
    {
        if (baseAddress[offset] != pattern)
        {
            printf("\r\nmemTestAddressBus: FAILED at %04x with pattern %02x\r\n", baseAddress+offset, pattern);
            return ((byte *) &baseAddress[offset]);
        }
        if (offset % 1024 == 0)
         printf(".");
    }

    baseAddress[testOffset] = pattern;


    // Check for address bits stuck low or shorted.

    for (testOffset = 1; (testOffset & addressMask) != 0; testOffset <<= 1)
    {
        baseAddress[testOffset] = antipattern;

        if (baseAddress[0] != pattern)
        {
            return ((byte *) &baseAddress[testOffset]);
        }

        for (offset = 1; (offset & addressMask) != 0; offset <<= 1)
        {
            if ((baseAddress[offset] != pattern) && (offset != testOffset))
            {
                printf("\r\nmemTestAddressBus: FAILED at %04x with pattern %02x\r\n", baseAddress+offset, pattern);
                return ((byte *) &baseAddress[testOffset]);
            }
        }
        baseAddress[testOffset] = pattern;
    }
    return (NULL);
}

/**********************************************************************
 *
 * Function:    memTestDevice()
 *
 * Description: Test the integrity of a physical memory device by
 *              performing an increment/decrement test over the
 *              entire region.  In the process every storage bit
 *              in the device is tested as a zero and a one.  The
 *              base address and the size of the region are
 *              selected by the caller.
 *
 * Returns:     NULL if the test succeeds.
 *
 *              A non-zero result is the first address at which an
 *              incorrect value was read back.  By examining the
 *              contents of memory, it may be possible to gather
 *              additional information about the problem.
 *
 **********************************************************************/

byte * memTestDevice(volatile byte * baseAddress, unsigned long nBytes)
{
    unsigned long offset;
    unsigned long nWords = nBytes / sizeof(byte);

    byte pattern;
    byte antipattern;


   // Fill memory with a known pattern.

   for (pattern = 1, offset = 0; offset < nWords; pattern++, offset++)
        baseAddress[offset] = pattern;

   // Check each location and invert it for the second pass.

    for (pattern = 1, offset = 0; offset < nWords; pattern++, offset++)
    {
        if (offset % 1024 == 0)
         printf("%04X  ", (int) &baseAddress[offset]);

        if (baseAddress[offset] != pattern)
        {
            printf("\r\nmemTestDevice: FAILED at %04x with pattern %02x\r\n", (int) &baseAddress[offset], pattern);
            return ((byte *) &baseAddress[offset]);
        }

        antipattern = ~pattern;
        baseAddress[offset] = antipattern;

    }

    // Check each location for the inverted pattern and zero it.

    for (pattern = 1, offset = 0; offset < nWords; pattern++, offset++)
    {
        if (offset % 1024 == 0)
         printf("%04X  ", (int) &baseAddress[offset]);

        antipattern = ~pattern;
        if (baseAddress[offset] != antipattern)
        {
            printf("\r\nmemTestDevice: FAILED at %04x with antipattern %02x\r\n", (int) &baseAddress[offset], pattern);
            return ((byte *) &baseAddress[offset]);
        }
    }

    return (NULL);
}

// TestMemory
//
// Run all three memory tests

byte TestMemory(byte * startAddress, unsigned long size)
{
   if ((memTestDataBus(startAddress) != 0) ||
       (memTestAddressBus(startAddress, size) != NULL) ||
       (memTestDevice(startAddress, size) != NULL))
    {
        return (-1);
    }
    else
    {
        return (0);
    }
}

int main (void)
{
   printf("\r\nTesting KIM-1...\r\n");
   RepeatChar('-', 39);

   printf("\r\nTesting RIOT RAM: 1780-17BF\r\n");
   if (TestMemory((byte *)0x1780, 0x17BF - 0x1780))
      return 0;

   printf("\r\nTesting RIOT RAM: 17C0-17E6\r\n");
   if (TestMemory((byte *)0x17C0, 0x17E6 - 0x17C0))
      return 0;

   printf("\r\nTesting Memory: 0400-13FF\r\n");
   if (TestMemory((byte *)0x0400, 0x13FF - 0x0400))
      return 0;

   printf("\r\nTesting Memory: 4000-DFFF\r\n");
   if (TestMemory((byte *)0x4000, 0xDFFF - 0x4000))
      return 0;

   printf("\r\nPASS!\r\n");
   return 1;
}


