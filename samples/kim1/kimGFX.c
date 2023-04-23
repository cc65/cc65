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

#include "ramfont.c"

typedef unsigned char byte;

// Screen memory is placed at A000-BFFF, 320x200 pixels, mapped right to left within each horizontal byte

byte * screen    = (byte *) 0xA000;

#define SCREEN_WIDTH      320
#define SCREEN_HEIGHT     200
#define CHARWIDTH         8
#define CHARHEIGHT        8
#define BYTESPERROW       (SCREEN_WIDTH / 8)
#define BYTESPERCHARROW   (BYTESPERROW * 8)
#define CHARSPERROW       (SCREEN_WIDTH / CHARWIDTH)
#define ROWSPERCOLUMN     (SCREEN_HEIGHT / CHARHEIGHT)

// SETPIXEL  
// 
// 0 <= x < 320
// 0 <= y < 200
//
// Draws a pixel on the screen in white or black at pixel pos x, y

void SETPIXEL(int x, int y, byte b)
{
   byte * pb = screen;
   pb += x >> 3;
   pb += y * BYTESPERROW;
   
   if (b)
      *(pb) |=  (0b10000000 >> (x & 7));
   else
      *(pb) &= ~(0b10000000 >> (x & 7));
}

// DRAWPIXEL
//
// 0 <= x < 320
// 0 <= y < 200
//
// Turns on a screen pixel at pixel pos x,y

void DRAWPIXEL(int x, int y)
{
   byte * pb = screen;
   pb += x >> 3;
   pb += y * BYTESPERROW;
   *(pb) |= (0b10000000 >> (x & 7));
}

// DrawChar
//
// 0 <= x < 40
// 0 <= y < 25
//
// Draws a character at char location x, y

void DrawChar(int x, int y, char c)
{
   byte * pb = screen;
   int i;

   if (x < 0 || y < 0 || x >= CHARSPERROW || y >= ROWSPERCOLUMN)
      return;
   
   pb += y * BYTESPERCHARROW;
   pb += x;

   for (i = 0; i < 8; i++)
   {
      *pb =  font8x8_basic[c][i];
       pb += BYTESPERROW;
   }
}

void DrawText(int x, int y, char * psz)
{
   while (*psz)
   {
      while (x >= CHARSPERROW)
      {
         x -= CHARSPERROW;
         y += 1;
      }
      while (y >= ROWSPERCOLUMN)
         y -= ROWSPERCOLUMN;

      if (*psz == 0x0A)
      {
         x = 0;
         y++;
         psz++;
      }
      else
      {
         DrawChar(x++, y, *psz++);
      }
   }
}

// Something like Bresenham's algorithm for drawing a line

void DrawLine(int x0, int y0, int x1, int y1, byte val)
{
    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = (dx > dy ? dx : -dy) / 2, e2;

    while (1)
    {
        SETPIXEL(x0, y0, val);

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

// reverse_bits
//
// Reverse the bits in a byte

unsigned char reverse_bits(unsigned char octet)
{
   return  (((octet >> 0) & 1) << 7) | \
            (((octet >> 1) & 1) << 6) | \
            (((octet >> 2) & 1) << 5) | \
            (((octet >> 3) & 1) << 4) | \
            (((octet >> 4) & 1) << 3) | \
            (((octet >> 5) & 1) << 2) | \
            (((octet >> 6) & 1) << 1) | \
            (((octet >> 7) & 1) << 0);
}

// InitializeFont
// 
// RAM font is backwards left-right relative to the way memory is laid out on the KIM-1, so we swap all the 
// bytes in place by reversing the order of the bits in every byte

void InitializeFont()
{
   int c;
   byte * pb = (byte *) font8x8_basic;

   for (c = 0; c < 128 * 8; c++)
      pb[c] = reverse_bits(pb[c]);
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

// InitializeScreen
//
// One-time call to clear the screen and initialize the font

void InitializeScreen()
{
   ClearScreen(0x00);
   InitializeFont();
}

// DrawScreenMoire
// 
// Draws a moire pattern on the screen without clearing it first

void DrawScreenMoire()
{
   int x, y;

   for (x = 0; x < SCREEN_WIDTH; x += 6)
      DrawLine(x, 0, SCREEN_WIDTH - x, SCREEN_HEIGHT - 1, 1);

   for (y = 0; y < SCREEN_HEIGHT; y += 6)
      DrawLine(0, y, SCREEN_WIDTH - 1, SCREEN_HEIGHT - y, 1);
}

int main (void)
{
   InitializeScreen();
               // 0123456789012345678901234567890123456789
   DrawText(0, 0, " *** COMMODORE KIM-1 SYSTEM ***"); 
   DrawText(0, 2, "60K RAM SYSTEM.  49152 BYTES FREE.");
   DrawText(0, 4, "READY.\n>");
   printf("Done, exiting...\r\n");

   return 0;
}
