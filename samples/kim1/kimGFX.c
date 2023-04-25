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

// Cursor position

int CursorX = 0;
int CursorY = 0;

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

// ClearScreen
//
// Clear the graphics memory

void ScrollScreen()
{
   int totalBytes = SCREEN_HEIGHT * (SCREEN_WIDTH / 8);
   int i;
   int j = 0;

   for (i = BYTESPERCHARROW; i < totalBytes; i++)
      screen[j++] = screen[i];
}

/* kimmon9000.hex - Not used yet, but available for conversion from PETSCIi to ASCII

const unsigned char petToAscTable[256] =
{
	0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x14,0x09,0x0d,0x11,0x93,0x0a,0x0e,0x0f,
	0x10,0x0b,0x12,0x13,0x08,0x15,0x16,0x17,0x18,0x19,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f,
	0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2a,0x2b,0x2c,0x2d,0x2e,0x2f,
	0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3a,0x3b,0x3c,0x3d,0x3e,0x3f,
	0x40,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6a,0x6b,0x6c,0x6d,0x6e,0x6f,
	0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7a,0x5b,0x5c,0x5d,0x5e,0x5f,
	0xc0,0xc1,0xc2,0xc3,0xc4,0xc5,0xc6,0xc7,0xc8,0xc9,0xca,0xcb,0xcc,0xcd,0xce,0xcf,
	0xd0,0xd1,0xd2,0xd3,0xd4,0xd5,0xd6,0xd7,0xd8,0xd9,0xda,0xdb,0xdc,0xdd,0xde,0xdf,
	0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8a,0x8b,0x8c,0x8d,0x8e,0x8f,
	0x90,0x91,0x92,0x0c,0x94,0x95,0x96,0x97,0x98,0x99,0x9a,0x9b,0x9c,0x9d,0x9e,0x9f,
	0xa0,0xa1,0xa2,0xa3,0xa4,0xa5,0xa6,0xa7,0xa8,0xa9,0xaa,0xab,0xac,0xad,0xae,0xaf,
	0xb0,0xb1,0xb2,0xb3,0xb4,0xb5,0xb6,0xb7,0xb8,0xb9,0xba,0xbb,0xbc,0xbd,0xbe,0xbf,
	0x60,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4a,0x4b,0x4c,0x4d,0x4e,0x4f,
	0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5a,0x7b,0x7c,0x7d,0x7e,0x7f,
	0xa0,0xa1,0xa2,0xa3,0xa4,0xa5,0xa6,0xa7,0xa8,0xa9,0xaa,0xab,0xac,0xad,0xae,0xaf,
	0xb0,0xb1,0xb2,0xb3,0xb4,0xb5,0xb6,0xb7,0xb8,0xb9,0xba,0xbb,0xbc,0xbd,0xbe,0xbf
};
*/

// ascToPetTable
//
// Translation lookup table to go from ASCII to PETSCII

const unsigned char ascToPetTable[256] = 
{
	0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x14,0x20,0x0d,0x11,0x93,0x0a,0x0e,0x0f,
	0x10,0x0b,0x12,0x13,0x08,0x15,0x16,0x17,0x18,0x19,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f,
	0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2a,0x2b,0x2c,0x2d,0x2e,0x2f,
	0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3a,0x3b,0x3c,0x3d,0x3e,0x3f,
	0x40,0xc1,0xc2,0xc3,0xc4,0xc5,0xc6,0xc7,0xc8,0xc9,0xca,0xcb,0xcc,0xcd,0xce,0xcf,
	0xd0,0xd1,0xd2,0xd3,0xd4,0xd5,0xd6,0xd7,0xd8,0xd9,0xda,0x5b,0x5c,0x5d,0x5e,0x5f,
	0xc0,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4a,0x4b,0x4c,0x4d,0x4e,0x4f,
	0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5a,0xdb,0xdc,0xdd,0xde,0xdf,
	0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8a,0x8b,0x8c,0x8d,0x8e,0x8f,
	0x90,0x91,0x92,0x0c,0x94,0x95,0x96,0x97,0x98,0x99,0x9a,0x9b,0x9c,0x9d,0x9e,0x9f,
	0xa0,0xa1,0xa2,0xa3,0xa4,0xa5,0xa6,0xa7,0xa8,0xa9,0xaa,0xab,0xac,0xad,0xae,0xaf,
	0xb0,0xb1,0xb2,0xb3,0xb4,0xb5,0xb6,0xb7,0xb8,0xb9,0xba,0xbb,0xbc,0xbd,0xbe,0xbf,
	0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6a,0x6b,0x6c,0x6d,0x6e,0x6f,
	0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7a,0x7b,0x7c,0x7d,0x7e,0x7f,
	0xe0,0xe1,0xe2,0xe3,0xe4,0xe5,0xe6,0xe7,0xe8,0xe9,0xea,0xeb,0xec,0xed,0xee,0xef,
	0xf0,0xf1,0xf2,0xf3,0xf4,0xf5,0xf6,0xf7,0xf8,0xf9,0xfa,0xfb,0xfc,0xfd,0xfe,0xff
};

// DrawChar
//
// 0 <= x < 40
// 0 <= y < 25
//
// Draws an ASCII character at char location x, y 

void DrawChar(int x, int y, char petscii)
{
   byte * pb = screen;
   int i;
   char c = ascToPetTable[petscii];

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

void DrawText(char * psz)
{
   while (*psz)
   {
      while (CursorX >= CHARSPERROW)
      {
         CursorX -= CHARSPERROW;
         CursorY += 1;
      }

      // If we've gone off the bottom of the screen, we scroll the screen and back up to the last line again

      if (CursorY >= ROWSPERCOLUMN)
      {
         CursorY = ROWSPERCOLUMN - 1;
         ScrollScreen();
      }

      // If we output a newline we advanced the cursor down one line and reset it to the left

      if (*psz == 0x0A)
      {
         CursorX = 0;
         CursorY++;
         psz++;
      }
      else
      {
         DrawChar(CursorX++, CursorY, *psz++);
      }
   }

}
void DrawTextAt(int x, int y, char * psz)
{
   CursorX = x;
   CursorY = y;
   DrawText(psz);
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

// MirrorFont
// 
// RAM font is backwards left-right relative to the way memory is laid out on the KIM-1, so we swap all the 
// bytes in place by reversing the order of the bits in every byte

void MirrorFont()
{
   int c;
   byte * pb = (byte *) font8x8_basic;

   for (c = 0; c < 128 * 8; c++)
      pb[c] = reverse_bits(pb[c]);
}

// DrawScreenMoire
// 
// Draws a moire pattern on the screen without clearing it first

void DrawScreenMoire(int left, int top, int right, int bottom)
{
   int x, y;

   DrawLine(left, top, right, top, 1);
   DrawLine(left, bottom, right, bottom, 1);
   DrawLine(left, top, left, bottom, 1);
   DrawLine(right, top, right, bottom, 1);

   for (x = left; x < right; x += 6)
      DrawLine(x, top, right - x + left, bottom, 1);

   for (y = top; y < bottom; y += 6)
      DrawLine(left, y, right, bottom - y + top, 1);

}

int main (void)
{
   int i;
   
   char buffer [sizeof(int)*8+1];
   
   ClearScreen(0x00);
               // 0123456789012345678901234567890123456789
   DrawTextAt(0, 0, " *** COMMODORE KIM-1 SYSTEM ***"); 
   DrawTextAt(0, 2, "60K RAM SYSTEM.  49152 BYTES FREE.");
   DrawTextAt(0, 4, "READY.\n");
   printf("Done, exiting...\r\n");

   // DrawScreenMoire(20, 50, SCREEN_WIDTH-20, SCREEN_HEIGHT-1);

   for (i = 0; i < 10000; i++)
   {
      sprintf(buffer, "%i\n", i);
      DrawText(buffer);
   }

   return 0;
}
