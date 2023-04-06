// --------------------------------------------------------------------------
// Conway's Game of Life for KIM-1
//
// Assumes the MTU Visible Memory Board mapped at 0x8000 for 8K of video RAM
//
// Dave Plummer on a rainy Thursday
//
// davepl@davepl.com
// --------------------------------------------------------------------------

#include <stdio.h>                  // For printf
#include <stdlib.h>                 // For rand, srand
#include <string.h>                 // For memcpy

typedef unsigned char byte;

// World size

#define WIDTH     320
#define HEIGHT    200
#define NUMBITS  8000
#define NUMBYTES 1000
#define DENSITY    50

// BITARRAY
//
// Access individual bits in a block of memory

#define GETBIT(array, bit) (array[bit >> 3]  &  (1 << (bit & 7)))
#define SETBIT(array, bit) (array[bit >> 3] |=  (1 << (bit & 7)))
#define CLRBIT(array, bit) (array[bit >> 3] &= ~(1 << (bit & 7)))

// Screen memory is placed at 8000, our world copy at A000, and they use the same layout so 
// that we can memcpy from one to the other without translating

byte * world     = (byte *) 0x8000;
byte * new_world = (byte *) 0xA000;

// Access to the screen bitmap

#define SETPIXEL(w, x, y, bit) (bit ? SETBIT(w, y * HEIGHT + x) : CLRBIT(w, y * HEIGHT + x))
#define GETPIXEL(w, x, y) GETBIT(w, y * HEIGHT + x)

// RandomFillWorld
//
// Populates the initial world with random cells

void RandomFillWorld()
{
   int x, y;

   srand(rand());
   for (x = 0; x < WIDTH; x++)
      for (y = 0; y < HEIGHT; y++)
         SETPIXEL(world, x, y, (rand() % 100) < DENSITY);
}

// CountNeighbors
// 
// Count the number of live cells around the given spot, excluding the actual spot specified

int CountNeighbors(int x, int y)
{
   int i, j, nx, ny, count = 0;

   for (j = -1; j <= 1; j++)
   {
      for (i = -1; i <= 1; i++)
      {
         if (i != 0 || j != 0)
         {
            nx = (x + i + WIDTH) % WIDTH;
            ny = (y + j + HEIGHT) % HEIGHT;
            count += GETPIXEL(world, nx, ny) ? 1 : 0;
         }
      }
   }
   return count;
}

// UpdateWorld
//
// Applies the rules of Conway's Game of Life to the cells

void UpdateWorld()
{
   int x, y;

   for (y = 0; y < HEIGHT; y++) 
   {
       for (x = 0; x < WIDTH; x++) 
       {
         int neighbors = CountNeighbors(x, y);
         if (GETPIXEL(world, x, y))
            SETPIXEL(new_world, x, y, (neighbors == 2 || neighbors == 3));
         else 
            SETPIXEL(new_world, x, y, (neighbors == 3));
       }
   }
}

int main (void)
{
   printf("Starting Conway's Game of Life\r\n");

   for (;;)
   {
      UpdateWorld();
      memcpy(world, new_world, NUMBYTES);
   }

   return 0;
}
