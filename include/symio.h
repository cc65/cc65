// symio.h
//
// I/O primitives for Sym-1
//
// Wayne Parham

#ifndef _SYMIO_H
#define _SYMIO_H

#include <stddef.h>
#include <stdarg.h>

int   __fastcall__   beep (void);            // Beep sound
int   __fastcall__   set_D0 (char);          // Set display digit 0
int   __fastcall__   get_D0 (void);          // Get value of display digit 0
int   __fastcall__   set_D1 (char);          // Set display digit 1
int   __fastcall__   get_D1 (void);          // Get value of display digit 1
int   __fastcall__   set_D2 (char);          // Set display digit 2
int   __fastcall__   get_D2 (void);          // Get value of display digit 2
int   __fastcall__   set_D3 (char);          // Set display digit 3
int   __fastcall__   get_D3 (void);          // Get value of display digit 3
int   __fastcall__   set_D4 (char);          // Set display digit 4
int   __fastcall__   get_D4 (void);          // Get value of display digit 4
int   __fastcall__   set_D5 (char);          // Set display digit 5
int   __fastcall__   get_D5 (void);          // Get value of display digit 5
int   __fastcall__   set_D6 (char);          // Set byte to the right of display (leading buffer)
int   __fastcall__   get_D6 (void);          // Get value of memory byte to the right of display
int   __fastcall__   fdisp (void);           // Flash display

int   __fastcall__   loadt (int);            // Read from tape (id)
int   __fastcall__   dumpt (int, int, int);  // Write to tape (id, start_addr, end_addr)

int   __fastcall__   set_DDR1A (int);        // Set data direction register 1A           (U25)
int   __fastcall__   get_DDR1A (void);       // Get value of data direction register 1A
int   __fastcall__   set_IOR1A (int);        // Set I/O register 1A
int   __fastcall__   get_IOR1A (void);       // Get value of I/O register 1A

int   __fastcall__   set_DDR1B (int);        // Set data direction register 1B           (U25)
int   __fastcall__   get_DDR1B (void);       // Get value of data direction register 1B
int   __fastcall__   set_IOR1B (int);        // Set I/O register 1B
int   __fastcall__   get_IOR1B (void);       // Get value of I/O register 1B

int   __fastcall__   set_DDR2A (int);        // Set data direction register 2A           (U28)
int   __fastcall__   get_DDR2A (void);       // Get value of data direction register 2A
int   __fastcall__   set_IOR2A (int);        // Set I/O register 2A
int   __fastcall__   get_IOR2A (void);       // Get value of I/O register 2A

int   __fastcall__   set_DDR2B (int);        // Set data direction register 2B           (U28)
int   __fastcall__   get_DDR2B (void);       // Get value of data direction register 2B
int   __fastcall__   set_IOR2B (int);        // Set I/O register 2B
int   __fastcall__   get_IOR2B (void);       // Get value of I/O register 2B

int   __fastcall__   set_DDR3A (int);        // Set data direction register 3A           (U29)
int   __fastcall__   get_DDR3A (void);       // Get value of data direction register 3A
int   __fastcall__   set_IOR3A (int);        // Set I/O register 3A
int   __fastcall__   get_IOR3A (void);       // Get value of I/O register 3A

int   __fastcall__   set_DDR3B (int);        // Set data direction register 3B           (U29)
int   __fastcall__   get_DDR3B (void);       // Get value of data direction register 3B
int   __fastcall__   set_IOR3B (int);        // Set I/O register 3B
int   __fastcall__   get_IOR3B (void);       // Get value of I/O register 3B

#ifndef _STDIO_H

int   __fastcall__   putchar (char);
int   __fastcall__   puts (const char* s);
int                  printf (const char* format, ...);
int                  sprintf (char* buf, const char* format, ...);
int   __fastcall__   vprintf (const char* format, va_list ap);
int   __fastcall__   vsnprintf (char* buf, size_t size, const char* format, va_list ap);
int   __fastcall__   vsprintf (char* buf, const char* format, va_list ap);
char  __fastcall__   getchar (void);
char* __fastcall__   gets (char* s);
int                  scanf (const char* format, ...);
int                  sscanf (const char* s, const char* format, ...);
int   __fastcall__   vscanf (const char* format, va_list ap);
int   __fastcall__   vsscanf (const char* s, const char* format, va_list ap);

#endif

// Display character definitions

#define DISP_1         0x06   // '1'
#define DISP_2         0x5B   // '2'
#define DISP_3         0x4F   // '3'
#define DISP_4         0x66   // '4'
#define DISP_5         0x6D   // '5'
#define DISP_6         0x7C   // '6'
#define DISP_7         0x07   // '7'
#define DISP_8         0x7F   // '8'
#define DISP_9         0x67   // '9'
#define DISP_0         0x3F   // '0'
#define DISP_A         0x77   // 'A'
#define DISP_b         0x7C   // 'b'
#define DISP_C         0x39   // 'C'
#define DISP_c         0x58   // 'c'
#define DISP_d         0x5E   // 'd'
#define DISP_E         0x79   // 'E'
#define DISP_e         0x7B   // 'e'
#define DISP_F         0x71   // 'F'
#define DISP_G         0x7D   // 'G'
#define DISP_g         0x6F   // 'g'
#define DISP_H         0x76   // 'H'
#define DISP_h         0x74   // 'h'
#define DISP_I         0x06   // 'I'
#define DISP_i         0x04   // 'i'
#define DISP_J         0x1E   // 'J'
#define DISP_K         0x74   // 'K'
#define DISP_L         0x38   // 'L'
#define DISP_M_1       0x33   // 'M'
#define DISP_M_2       0x27   // 2nd half
#define DISP_n         0x54   // 'n'
#define DISP_O         0x3F   // 'O'
#define DISP_o         0x5C   // 'o'
#define DISP_P         0x73   // 'P'
#define DISP_q         0x67   // 'q'
#define DISP_r         0x50   // 'r'
#define DISP_S         0x6D   // 'S'
#define DISP_t         0x46   // 't'
#define DISP_U         0x3E   // 'U'
#define DISP_u         0x1C   // 'u'
#define DISP_V_1       0x64   // 'V'
#define DISP_V_2       0x52   // 2nd half
#define DISP_W_1       0x3C   // 'W'
#define DISP_W_2       0x1E   // 2nd half
#define DISP_Y         0x6E   // 'Y'
#define DISP_Z         0x5B   // 'Z'
#define DISP_SPACE     0x00   // ' '
#define DISP_PERIOD    0x80   // '.'
#define DISP_HYPHEN    0x40   // '-'
#define DISP_APOSTR    0x20   // '''
#define DISP_EQUAL     0x41   // '='
#define DISP_3_BAR     0x49   // '='
#define DISP_BOTTOM    0x08   // '_'
#define DISP_TOP       0x01   // Top segment
#define DISP_LEFT      0x30   // '|' Left side, both segments
#define DISP_RIGHT     0x06   // '|' Right side, both segments
#define DISP_DEGREE    0x63   // 'o' An 'o' character in the upper segments
#define DISP_HAT       0x23   // 'n' An 'n' character in the upper segments
#define DISP_FORK      0x62   // 'u' A  'u' character in the upper segments
#define DISP_SLASH     0x51   // '/'
#define DISP_BACKSLASH 0x34   // '\'
#define DISP_TOP_RIGHT 0x02   // Top right segment
#define DISP_TOP_LEFT  0x20   // Top left segment
#define DISP_LOW_RIGHT 0x04   // Lower right segment
#define DISP_LOW_LEFT  0x10   // Lower left segment

#endif
