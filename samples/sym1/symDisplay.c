// --------------------------------------------------------------------------
// Sym-1 front panel display example
//
// Wayne Parham
//
// wayne@parhamdata.com
// --------------------------------------------------------------------------

#include <stdio.h>
#include <sym1.h>

int main (void) {
   int  delay       = 10;
   int  flashes     = 255;
   int  displayable = 1;
   int  e           = 0;
   int  r           = 0;
   int  d           = 0;
   int  i           = 0;
   int  l           = 0;
   int  t           = 0;
   int  z           = 0;
   char c           = 0x00;
   char buffer[41]  = { 0x00 };

   puts ("\nType a message (40 chars max) and press ENTER, please:\n");

   while ( (c != '\n') && (i < 40) ) {
      c = getchar();
      buffer[i] = c;
      i++;
      if ( i == 40 ) {
         puts ("\n\n--- Reached 40 character limit. ---");
      }
   }

   i--;                                // index is one past end

   while ( z == 0 ) {
      puts ("\n\nHow many times (0 for forever) to repeat?");
      c = getchar();
      if ( (c >= '0') && (c <= '9') ) {// between 1 and 9 loops allowed
         z = 1;                        // a number was pressed
         t = c - '0';                  // convert char to int
         puts ("\n\nLook at the front panel.\n");
      }
      else {
         puts ("\nWhat?");
         z = 0;                        // keep asking for a number
      }
   }

   z = 0;
   while ( (z < t) || (t == 0) ) {

      z++;

      putchar ( '\r' );                // Send CR to console

      DISPLAY.d0 = DISP_SPACE;         // Clear the display
      DISPLAY.d1 = DISP_SPACE;
      DISPLAY.d2 = DISP_SPACE;
      DISPLAY.d3 = DISP_SPACE;
      DISPLAY.d4 = DISP_SPACE;
      DISPLAY.d5 = DISP_SPACE;
      DISPLAY.d6 = DISP_SPACE;

      for ( d = 0; d < flashes ; d++ ) {
         fdisp();                      // Display
      }

      for ( l = 0; l <= i; l++ ) {

         displayable = 1;              // Assume character is mapped

         switch ( buffer[l] ) {        // Put the typed characters
            case '1':                  // into the display buffer
               DISPLAY.d6 = DISP_1;    // one at a time
               break;
            case '2':
               DISPLAY.d6 = DISP_2;
               break;
            case '3':
               DISPLAY.d6 = DISP_3;
               break;
            case '4':
               DISPLAY.d6 = DISP_4;
               break;
            case '5':
               DISPLAY.d6 = DISP_5;
               break;
            case '6':
               DISPLAY.d6 = DISP_6;
               break;
            case '7':
               DISPLAY.d6 = DISP_7;
               break;
            case '8':
               DISPLAY.d6 = DISP_8;
               break;
            case '9':
               DISPLAY.d6 = DISP_9;
               break;
            case '0':
               DISPLAY.d6 = DISP_0;
               break;
            case 'A':
               DISPLAY.d6 = DISP_A;
               break;
            case 'a':
               DISPLAY.d6 = DISP_A;
               break;
            case 'B':
               DISPLAY.d6 = DISP_b;
               break;
            case 'b':
               DISPLAY.d6 = DISP_b;
               break;
            case 'C':
               DISPLAY.d6 = DISP_C;
               break;
            case 'c':
               DISPLAY.d6 = DISP_c;
               break;
            case 'D':
               DISPLAY.d6 = DISP_d;
               break;
            case 'd':
               DISPLAY.d6 = DISP_d;
               break;
            case 'E':
               DISPLAY.d6 = DISP_E;
               break;
            case 'e':
               DISPLAY.d6 = DISP_e;
               break;
            case 'F':
               DISPLAY.d6 = DISP_F;
               break;
            case 'f':
               DISPLAY.d6 = DISP_F;
               break;
            case 'G':
               DISPLAY.d6 = DISP_G;
               break;
            case 'g':
               DISPLAY.d6 = DISP_g;
               break;
            case 'H':
               DISPLAY.d6 = DISP_H;
               break;
            case 'h':
               DISPLAY.d6 = DISP_h;
               break;
            case 'I':
               DISPLAY.d6 = DISP_I;
               break;
            case 'i':
               DISPLAY.d6 = DISP_i;
               break;
            case 'J':
               DISPLAY.d6 = DISP_J;
               break;
            case 'j':
               DISPLAY.d6 = DISP_J;
               break;
            case 'K':
               DISPLAY.d6 = DISP_K;
               break;
            case 'k':
               DISPLAY.d6 = DISP_K;
               break;
            case 'L':
               DISPLAY.d6 = DISP_L;
               break;
            case 'l':
               DISPLAY.d6 = DISP_L;
               break;
            case 'M':
               DISPLAY.d0 = DISPLAY.d1;
               DISPLAY.d1 = DISPLAY.d2;
               DISPLAY.d2 = DISPLAY.d3;
               DISPLAY.d3 = DISPLAY.d4;
               DISPLAY.d4 = DISPLAY.d5;
               DISPLAY.d5 = DISP_M_1;
               DISPLAY.d6 = DISP_M_2;
               break;
            case 'm':
               DISPLAY.d0 = DISPLAY.d1;
               DISPLAY.d1 = DISPLAY.d2;
               DISPLAY.d2 = DISPLAY.d3;
               DISPLAY.d3 = DISPLAY.d4;
               DISPLAY.d4 = DISPLAY.d5;
               DISPLAY.d5 = DISP_M_1;
               DISPLAY.d6 = DISP_M_2;
               break;
            case 'N':
               DISPLAY.d6 = DISP_n;
               break;
            case 'n':
               DISPLAY.d6 = DISP_n;
               break;
            case 'O':
               DISPLAY.d6 = DISP_O;
               break;
            case 'o':
               DISPLAY.d6 = DISP_o;
               break;
            case 'P':
               DISPLAY.d6 = DISP_P;
               break;
            case 'p':
               DISPLAY.d6 = DISP_P;
               break;
            case 'Q':
               DISPLAY.d6 = DISP_q;
               break;
            case 'q':
               DISPLAY.d6 = DISP_q;
               break;
            case 'R':
               DISPLAY.d6 = DISP_r;
               break;
            case 'r':
               DISPLAY.d6 = DISP_r;
               break;
            case 'S':
               DISPLAY.d6 = DISP_S;
               break;
            case 's':
               DISPLAY.d6 = DISP_S;
               break;
            case 'T':
               DISPLAY.d6 = DISP_t;
               break;
            case 't':
               DISPLAY.d6 = DISP_t;
               break;
            case 'U':
               DISPLAY.d6 = DISP_U;
               break;
            case 'u':
               DISPLAY.d6 = DISP_u;
               break;
            case 'V':
               DISPLAY.d0 = DISPLAY.d1;
               DISPLAY.d1 = DISPLAY.d2;
               DISPLAY.d2 = DISPLAY.d3;
               DISPLAY.d3 = DISPLAY.d4;
               DISPLAY.d4 = DISPLAY.d5;
               DISPLAY.d5 = DISP_V_1;
               DISPLAY.d6 = DISP_V_2;
               break;
            case 'v':
               DISPLAY.d0 = DISPLAY.d1;
               DISPLAY.d1 = DISPLAY.d2;
               DISPLAY.d2 = DISPLAY.d3;
               DISPLAY.d3 = DISPLAY.d4;
               DISPLAY.d4 = DISPLAY.d5;
               DISPLAY.d5 = DISP_V_1;
               DISPLAY.d6 = DISP_V_2;
               break;
            case 'W':
               DISPLAY.d0 = DISPLAY.d1;
               DISPLAY.d1 = DISPLAY.d2;
               DISPLAY.d2 = DISPLAY.d3;
               DISPLAY.d3 = DISPLAY.d4;
               DISPLAY.d4 = DISPLAY.d5;
               DISPLAY.d5 = DISP_W_1;
               DISPLAY.d6 = DISP_W_2;
               break;
            case 'w':
               DISPLAY.d0 = DISPLAY.d1;
               DISPLAY.d1 = DISPLAY.d2;
               DISPLAY.d2 = DISPLAY.d3;
               DISPLAY.d3 = DISPLAY.d4;
               DISPLAY.d4 = DISPLAY.d5;
               DISPLAY.d5 = DISP_W_1;
               DISPLAY.d6 = DISP_W_2;
               break;
            case 'Y':
               DISPLAY.d6 = DISP_Y;
               break;
            case 'y':
               DISPLAY.d6 = DISP_Y;
               break;
            case 'Z':
               DISPLAY.d6 = DISP_Z;
               break;
            case 'z':
               DISPLAY.d6 = DISP_Z;
               break;
            case ' ':
               DISPLAY.d6 = DISP_SPACE;
               break;
            case '.':
               DISPLAY.d6 = DISP_PERIOD;
               break;
            case '-':
               DISPLAY.d6 = DISP_HYPHEN;
               break;
            case '\'':
               DISPLAY.d6 = DISP_APOSTR;
               break;
            case '"':
               DISPLAY.d6 = DISP_APOSTR;
               break;
            case '=':
               DISPLAY.d6 = DISP_EQUAL;
               break;
            case '_':
               DISPLAY.d6 = DISP_BOTTOM;
               break;
            case '/':
               DISPLAY.d6 = DISP_SLASH;
               break;
            case '\\':
               DISPLAY.d6 = DISP_BACKSLASH;
               break;
            default:
               displayable = 0;        // Character not mapped
         }

         if ( displayable ) {

            putchar ( buffer[l] );     // Send it to the console

            DISPLAY.d0 = DISPLAY.d1;   // Scroll to the left
            DISPLAY.d1 = DISPLAY.d2;
            DISPLAY.d2 = DISPLAY.d3;
            DISPLAY.d3 = DISPLAY.d4;
            DISPLAY.d4 = DISPLAY.d5;
            DISPLAY.d5 = DISPLAY.d6;

            for ( d = 0; d < flashes ; d++ ) {
               fdisp();                // Display
            }
         }
      }

      for ( e = 0; e < 6; e++ ) {      // Gradually fill the
         DISPLAY.d0 = DISPLAY.d1;      // display with spaces
         DISPLAY.d1 = DISPLAY.d2;
         DISPLAY.d2 = DISPLAY.d3;
         DISPLAY.d3 = DISPLAY.d4;
         DISPLAY.d4 = DISPLAY.d5;
         DISPLAY.d5 = DISP_SPACE;
         DISPLAY.d6 = DISP_SPACE;
         for ( d = 0; d < flashes ; d++ ) {
            fdisp();                   // Display
         }
      }
   }

   puts ("\n\nEnjoy your day!\n\n");

   return 0;
}
