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

extern void ClearScreen(void);      // In subs.asm
extern void ScrollScreen(void);     
extern void DrawCircle(void);       
extern void SetPixel(void);
extern void ClearPixel(void);
extern byte __fastcall__ AscToPet(byte in);
extern unsigned char font8x8_basic[256][8];

extern int  x1cord;
extern int  y1cord;
extern int  x2cord;
extern int  y2cord;

// If in zeropage:
//
// #pragma zpsym("x1cord")
// #pragma zpsym("x2cord")
// #pragma zpsym("y1cord")
// #pragma zpsym("y2cord")

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
   x1cord = x;
   y1cord = y;

   if (b)
      SetPixel();
   else
      ClearPixel();
}

// DRAWPIXEL
//
// 0 <= x < 320
// 0 <= y < 200
//
// Turns on a screen pixel at pixel pos x,y

void DRAWPIXEL(int x, int y)
{
   x1cord = x;
   y1cord = y;
   SetPixel();
}

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
   char c = AscToPet(petscii);

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

// DrawCircle
//
// Draw a circle without sin, cos, or floating point!

void DrawCircleC(int x0, int y0, int radius, byte)
{
   x1cord = x0;
   y1cord = y0;
   y2cord = radius;
   DrawCircle();
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

void OldDrawCircleC(int x0, int y0, int radius, byte val) 
{
    int x = radius;
    int y = 0;
    int err = 0;

    while (x >= y) 
    {
        SETPIXEL(x0 + x, y0 + y, val);
        SETPIXEL(x0 + y, y0 + x, val);
        SETPIXEL(x0 - y, y0 + x, val);
        SETPIXEL(x0 - x, y0 + y, val);
        SETPIXEL(x0 - x, y0 - y, val);
        SETPIXEL(x0 - y, y0 - x, val);
        SETPIXEL(x0 + y, y0 - x, val);
        SETPIXEL(x0 + x, y0 - y, val);

        y++;
        err += 1 + 2 * y;
        if (2 * (err - x) + 1 > 0) {
            x--;
            err += 1 - 2 * x;
        }
    }
}

int main (void)
{
   int i;

   // Clear the screen memory
   ClearScreen();

   // Draw the welcome banner at the top of the screen
   DrawTextAt(0, 0, "  *** COMMODORE KIM-1 SHELL v1.1 ***");
   DrawTextAt(0, 2, "   60K RAM SYSTEM.  49152 BYTES FREE.");
   DrawTextAt(0, 4, "READY.\n");

   // DrawScreenMoire(0,30, 319, 199);

   // Print the numbers from 0-9999, forcing the screen to scroll
   for (i = 5; i < 75; i+=5)
      DrawCircleC(SCREEN_WIDTH/2, SCREEN_HEIGHT/2, i, 1);

   printf("Done, exiting...\r\n");
   return 0;
}
