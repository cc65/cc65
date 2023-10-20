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

extern void ClearScreen(void);      // In subs.asm
extern void ScrollScreen(void);     
extern void DrawCircle(void);       
extern void SetPixel(void);
extern void ClearPixel(void);
extern void DrawChar(void);
extern void DrawLine();
extern void Demo(void);
extern byte __fastcall__ AscToPet(byte in);
extern byte __fastcall__ ReverseBits(byte in);
extern unsigned char font8x8_basic[256][8];

extern int  x1cord;
extern int  y1cord;
extern int  x2cord;
extern int  y2cord;
extern int  cursorX;
extern int  cursorY;

// If in zeropage:
//
// #pragma zpsym("x1cord")
// #pragma zpsym("x2cord")
// #pragma zpsym("y1cord")
// #pragma zpsym("y2cord")

// Screen memory is placed at A000-BFFF, 320x200 pixels, mapped right to left within each horizontal byte

byte * screen    = (byte *) 0xA000;

// Cursor position

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

int c;

void DrawText(char * psz)
{
   while (*psz)
   {
      while (cursorX >= CHARSPERROW)
      {
         cursorX -= CHARSPERROW;
         cursorY += 1;
      }

      // If we've gone off the bottom of the screen, we scroll the screen and back up to the last line again

      if (cursorY >= ROWSPERCOLUMN)
      {
         cursorY = ROWSPERCOLUMN - 1;
         ScrollScreen();
      }

      // If we output a newline we advanced the cursor down one line and reset it to the left

      if (*psz == 0x0A)
      {
         cursorX = 0;
         cursorY++;
         psz++;
      }
      else
      {
         c = *psz;
     
         __asm__ ("ldx %v", cursorX);
         __asm__ ("ldy %v", cursorY);
         __asm__ ("lda %v", c);
         DrawChar();
         cursorX++;
         psz++;
      }
   }
}

void DrawTextAt(int x, int y, char * psz)
{
   cursorX = x;
   cursorY = y;
   DrawText(psz);
}

// Something like Bresenham's algorithm for drawing a line
/*
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
*/

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

void DrawLineC(int x1, int y1, int x2, int y2)
{
   x1cord = x1;
   y1cord = y1;
   x2cord = x2;
   y2cord = y2;
   DrawLine();
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
      pb[c] = ReverseBits(pb[c]);
}

// DrawScreenMoire
//
// Draws a moire pattern on the screen without clearing it first

void DrawScreenMoire(int left, int top, int right, int bottom)
{
   int x, y;

   DrawLineC(left, top, right, top);
   DrawLineC(left, bottom, right, bottom);
   DrawLineC(left, top, left, bottom);
   DrawLineC(right, top, right, bottom);

   for (x = left; x < right; x += 6)
      DrawLineC(x, top, right - x + left, bottom);

   for (y = top; y < bottom; y += 6)
      DrawLineC(left, y, right, bottom - y + top);

}

int main (void)
{
   int i;

   // Clear the screen memory
   while(1)
   {
      ClearScreen();
      Demo();
      DrawScreenMoire(0,30, 319, 199);

      ClearScreen();
      Demo();
      
      for (i = 5; i < 80; i+=5)
         DrawCircleC(SCREEN_WIDTH/2, SCREEN_HEIGHT/2 + 20, i, 1);

   }

   printf("Done, exiting...\r\n");
   return 0;
}
