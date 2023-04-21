// --------------------------------------------------------------------------
// Simple Graphics Test for KIM-1 with MTU Visible Memory Board
//
// Assumes the MTU Visible Memory Board mapped at 0xA000 for 8K of video RAM
//
// davepl@davepl.com
// --------------------------------------------------------------------------

#include <stdio.h>                  // For printf
#include <stdlib.h>                 // For rand, srand
#include <string.h>                 // For memcpy

typedef unsigned char byte;

// Screen memory is placed at A000-BFFF, 320x200 pixels, mapped right to left within each horizontal byte

byte * screen    = (byte *) 0xA000;
#define SCREEN_WIDTH      320
#define SCREEN_HEIGHT     200

// Access to the screen bitmap

void SETBIT(byte *p, unsigned long n)
{
   *(p + (n >> 3)) |=  (0b10000000 >> (n & 7));
}

void CLRBIT(byte *p, unsigned long n)
{
   *(p + (n >> 3)) &= ~(0b10000000 >> (n & 7));
}

byte GETBIT(byte *p, unsigned long n)
{
   return (*(p + (n >> 3)) & (0b10000000 >> (n & 7))) ? 1 : 0;
}


void SETPIXEL(byte * p, int x, int y, byte b)
{
   if (b)
      SETBIT(p, (long) y * SCREEN_WIDTH + x);
   else
      CLRBIT(p, (long) y * SCREEN_WIDTH + x);
}

byte GETPIXEL(byte *p, int x, int y)
{
   return GETBIT(p, (long) y * SCREEN_WIDTH + x);
}

// Something like Bresenham's algorithm for drawing a line

void DrawLine(int x0, int y0, int x1, int y1, byte val)
{
    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = (dx > dy ? dx : -dy) / 2, e2;

    while (1)
    {
        SETPIXEL(screen, x0, y0, val);

        if (x0 == x1 && y0 == y1)
            break;

        e2 = err;
        
        if (e2 > -dx) 
        {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dy) 
        {
            err += dx;
            y0 += sy;
        }
    }
}


// ClearScreen
//
// Clear the graphics memory

void ClearScreen(byte val)
{
   int totalBytes = SCREEN_HEIGHT * (SCREEN_WIDTH / 8);
   int i;

   for (i = 0; i < totalBytes; i++)
      screen[i] = val;
}

int main (void)
{
   int x, y;

   printf("\nDrawing Lines!\r\n");

   // This works, white or black, so while we know we can write to the memory, SETPIXEL must be broken somehow

   ClearScreen(0x00);
   
   // BUG: Currently pixels drawn to the bottom half of the screen are lost somewhere...

   for (x = 0; x < SCREEN_WIDTH; x += 6)
      DrawLine(x, 0, SCREEN_WIDTH - x, SCREEN_HEIGHT - 1, 1);

   for (y = 0; y < SCREEN_HEIGHT; y += 6)
      DrawLine(0, y, SCREEN_WIDTH - 1, SCREEN_HEIGHT - y, 1);

   printf("Done, exiting...\r\n");

   return 0;
}
